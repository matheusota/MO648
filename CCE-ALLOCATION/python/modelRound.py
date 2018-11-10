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

def modelRound(numberOfUsers, R, numberOfSubframes, graph):
    try:
        if not graph:
            f = open("output/cce_output_rounded_" + str(R) + ".txt", "w")
        solutionStatistics = SolutionStatistics()

        for frame in range(numberOfSubframes):
            shouldRunAgain = True
            roundRun = True
            roundedVars = []
            while shouldRunAgain:
                users = getInput(frame, numberOfUsers, R, numberOfSubframes)
                userRange = sorted(users.keys())

                # Create a new model
                m = Model("cce_allocation")

                # Create variables
                if roundRun:
                    x = m.addVars(userRange, range(R), vtype=GRB.CONTINUOUS, name="x")
                else:
                    x = m.addVars(userRange, range(R), vtype=GRB.BINARY, name="x")

                # Set objective
                m.setObjective(quicksum([users[i].price * x[(i, j)] for i in userRange for j in range(R)]), GRB.MAXIMIZE)

                # Add packing constraint
                m.addConstr(quicksum([users[i].size * x[(i, j)] for i in userRange for j in range(R)]) <= R)

                # Add constraint: \sum_j x_{i, j} <= 1, \forall i
                m.addConstrs((quicksum([x[(i, j)] for j in range(R)]) <= 1 \
                    for i in userRange), name = "user_starts_once")
                
                # Add constraint: \sum_i \sum_{t = j - s_i + 1}^j x_{i, t} <= 1, \forall j
                m.addConstrs((quicksum([x[(i, t)] for i in userRange for t in range(max(0, j - users[i].size + 1), j + 1)]) <= 1 \
                    for j in range(R)), name = "one_user_per_position")

                # Add constraint: x_{i, j} = 0, if user i cannot start at position j
                m.addConstrs((x[(i, j)] == 0 \
                    for i in userRange \
                    for j in range(R) \
                    if j not in users[i].begins), name = "user_cannot_start")
                
                # Add constraint x_{i, j} <= 1
                m.addConstrs((x[(i, j)] <= 1 \
                    for i in userRange \
                    for j in range(R)), name = "variable_bound")
                
                # Add round constraints
                if not roundRun:
                    m.addConstrs(x[r] == 1 for r in roundedVars)

                start_time = time.time()
                m.optimize()
                if not graph:
                    f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")
                
                for i in userRange:
                    for j in range(R):
                        if x[(i, j)].x >= 0.5:
                            roundedVars.append((i, j))
                    
                if not roundRun:
                    shouldRunAgain = False

                roundRun = False
                    
            #[28, 28, 9, 14, 7, 12, 22, 22, 39, 39, 39, 39, 10, 8, 17, 4, 21, 21, 20, 2]
            solution = [-1 for _ in range(R)]
            solutionOrigId = [-1 for _ in range(R)]
            for j in range(R):
                for i in userRange:
                    if x[(i, j)].x > 0:
                        solutionStatistics.countUser(users[i])

                        for k in range(j, j + users[i].size):
                            solution[k] = i
                            solutionOrigId[k] = users[i].originalId

            filled = getFilledPositions(solution)
            blocked = getBlockedUsers(solution, R, filled, numberOfUsers)

            solutionStatistics.addFilledPositions(filled)
            solutionStatistics.addBlockedUsers(blocked)
            solutionStatistics.addUsers()

            if not graph:
                f.write("Subframe: " + str(frame) + "\n")
                f.write("Solution:\n")
                f.write(str(solutionOrigId) + "\n")
                f.write("Filled Positions Rate: " + str(filled) + "/" + str(R) + "\n")
                f.write("Number of Blocked Users: " + str(blocked) + "\n")
                f.write("---------------------------------------------------------------------\n")

        if not graph:
            f.write("Mean Filled: " + str(solutionStatistics.getFilledPositionMean()) + "\n")
            f.write("Mean Blocked: " + str(solutionStatistics.getBlockedUsersMean()) + "\n")
        
        return solutionStatistics
    except GurobiError as e:
        print('Error code ' + str(e.errno) + ": " + str(e))

    except AttributeError:
        print('Encountered an attribute error')