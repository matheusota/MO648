"""
Scheduling model for CCE Allocation

Variables and Coefficients:
    x_{i,j} -> 1 if user i is allocated to a block that starts at position j
    i goes from 1,..., U
    and j goes from 1,...,R

    s_i -> block size of user i

ILP Model:
    max \sum_i \sum_j x_{i,j}
    subject to:
    \sum_j x_{i, j} <= 1, \forall i -> this guarantees that a user block starts, at maximum, at just one position
    \sum_i \sum_{t = j - s_i + 1}^j x_{i, t} <= 1, \forall j -> for each position j, only one user can occupy it
    x_{i, j} = 0, if user i cannot start at position j
    x_{i, j} \in {0, 1} 
"""
from gurobipy import *
from parse import *
from measures import *
import time
import sys
from solutionStatistics import *

def addIfPossible(solution, users, i, j):
    for k in range(j, j + users[i].size):
        if solution[k] != -1:
            return False
    
    for k in range(j, j + users[i].size):
        solution[k] = i

    return True

def modelBestRound(numberOfUsers, R, numberOfSubframes, graph):
    try:
        if not graph:        
            file = open("output/cce_output_bestround_" + str(R) + ".txt", "w")
        solutionStatistics = SolutionStatistics()

        for frame in range(numberOfSubframes):
            users = getInput(frame, numberOfUsers, R, numberOfSubframes)
            userRange = sorted(users.keys())

            # Create a new model
            m = Model("cce_allocation")

            # Create variables
            s = m.addVars(userRange, range(R), vtype=GRB.CONTINUOUS, name="s") # user i starts at position j
            b = m.addVars(userRange, vtype=GRB.CONTINUOUS, name="b") # user i is blocked
            p = m.addVars(range(R), vtype=GRB.CONTINUOUS, name="p") # position j is occupied
            a = m.addVars(userRange, vtype=GRB.CONTINUOUS, name="a") # user i was allocated
            k = m.addVars(userRange, vtype=GRB.CONTINUOUS, name="x") # there exist a user with id >= i that was allocated
            f = m.addVar(vtype=GRB.CONTINUOUS, name="f") # if cce array is full

            # Set objective
            m.setObjective(quicksum([b[i] for i in userRange]), GRB.MINIMIZE)

            # Variables domains
            m.addConstrs(s[(i, j)] <= 1 for i in userRange for j in range(R))
            m.addConstrs(b[i] <= 1 for i in userRange)
            m.addConstrs(p[j] == 1 for j in range(R))
            m.addConstrs(a[i] <= 1 for i in userRange)
            m.addConstrs(k[i] <= 1 for i in userRange)
            m.addConstr(f <= 1)

            # Add a definition constraint
            m.addConstrs((a[i] == quicksum([s[(i, j)] for j in range(R)]) for i in userRange), name = "a_definition")
            
            # Add p definition constraint
            m.addConstrs((p[j] == (quicksum([s[(i, j2)] for i in userRange for j2 in range(max(0, j - users[i].size + 1), j + 1)]))
                for j in range(R)), name = "p_definition")

            # Now lets define K
            # Add "k is contiguous" constraint
            m.addConstrs((k[i] <= k[i - 1] for i in range(2, userRange[-1] + 1)), name = "k_is_contiguous")

            # This constraint enforces that k[i] cannot be one if there is not a user j >= i that was allocated
            m.addConstrs((k[i] <= quicksum([a[i2] for i2 in range(i, userRange[-1] + 1)]) for i in userRange), name = "k_to_zero")

            # If user i was allocated, than k[i] is one
            m.addConstrs((a[i] <= k[i] for i in userRange), name = "k_user_allocated")

            # Now lets define f
            # Add f definition constraint
            m.addConstr((quicksum([p[j] for j in range(R)]) <= (R - 1) + f), name = "f_definition")

            # If a position was not filled, then f is zero
            m.addConstrs((p[j] >= f for j in range(R)), name = "f_to_zero")

            # Now lets define b
            m.addConstrs((k[i] + (1 - f) - 2 * a[i] <= 2 * b[i] for i in userRange), name = "b_definition")

            # Add constraint: x_{i, j} = 0, if user i cannot start at position j
            m.addConstrs((s[(i, j)] == 0 \
                for i in userRange \
                for j in range(R) \
                if j not in users[i].begins), name = "user_cannot_start")

            start_time = time.time()
            m.optimize()
            if not graph:
                file.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")

            # get variables set to one
            roundedVars = set()
            allocatedUsers = set()

            for i in userRange:
                file.write("User " + str(i) + "\n")
                tmp = []
                for j in range(R):
                    tmp.append(round(s[(i, j)].x, 2))
                    if s[(i, j)].x > 0.5:
                        allocatedUsers.add(i)
                        roundedVars.add((i, j))
                
                file.write(str(tmp) + "\n")

            # mount solution
            solution = [-1 for _ in range(R)]
            solutionOrigId = [-1 for _ in range(R)]
            for j in range(R):
                for i in userRange:
                    if (i, j) in roundedVars:
                        solutionStatistics.countUser(users[i])
                        for k in range(j, j + users[i].size):
                            solution[k] = i
                            solutionOrigId[k] = users[i].originalId

            # allocate more users if possible
            for j in range(R):
                if solution[j] == -1: 
                    for i in userRange:
                        if (i not in allocatedUsers) and (j in users[i].begins):
                            if addIfPossible(solution, users, i, j):
                                solutionStatistics.countUser(users[i])
                                allocatedUsers.add(i)
                                break

            # get statistics
            filled = getFilledPositions(solution)
            blocked = getBlockedUsers(solution, R, filled, numberOfUsers)
            solutionStatistics.addFilledPositions(filled)
            solutionStatistics.addBlockedUsers(blocked)
            solutionStatistics.addUsers()
            solutionStatistics.addMaxId(max(solution))
            
            if not graph:
                file.write("Subframe: " + str(frame) + "\n")
                file.write("Solution:\n")
                file.write(str(solution) + "\n")
                file.write("Filled Positions Rate: " + str(filled) + "/" + str(R) + "\n")
                file.write("Number of Blocked Users: " + str(blocked) + "\n")
                file.write("Objective Value: " + str(m.objVal))
                file.write("---------------------------------------------------------------------\n")

        if not graph:
            file.write("Mean Filled: " + str(solutionStatistics.getFilledPositionMean()) + "\n")
            file.write("Mean Blocked: " + str(solutionStatistics.getBlockedUsersMean()) + "\n")
        
        return solutionStatistics
        
    except GurobiError as e:
        print('Error code ' + str(e.errno) + ": " + str(e))

    except AttributeError as e:
        print(e)
        print('Encountered an attribute error')