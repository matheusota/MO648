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

def modelQuadratic(numberOfUsers, R, numberOfSubframes, graph):
    try:
        if not graph:        
            f = open("output/cce_output_quadratic_" + str(R) + ".txt", "w")
        solutionStatistics = SolutionStatistics()

        for frame in range(numberOfSubframes):
            users = getInput(frame, numberOfUsers, R, numberOfSubframes)
            userRange = sorted(users.keys())

            # Create a new model
            m = Model("cce_allocation")

            # Create variables
            x = m.addVars(userRange, range(R), vtype=GRB.BINARY, name="x")
            y = m.addVars(userRange, vtype=GRB.BINARY, name="x")
            a = m.addVars(userRange, vtype=GRB.BINARY, name="x")
            z = m.addVars(range(R), vtype=GRB.BINARY, name="x")

            """
            m.addConstrs(y[i] <= 1 for i in userRange)
            m.addConstrs(a[i] <= 1 for i in userRange)
            m.addConstrs(z[j] <= 1 for j in range(R))
            """

            # Set objective
            m.setObjective(quicksum([y[i] for i in userRange]), GRB.MINIMIZE)

            # Add constraint: \sum_j x_{i, j} <= 1, \forall i
            m.addConstrs((quicksum([x[(i, j)] for j in range(R)]) <= 1 \
                for i in userRange), name = "user_starts_once")
            
            # Add z definition constraint
            m.addConstrs((quicksum([x[(i, t)] for i in userRange for t in range(max(0, j - users[i].size + 1), j + 1)]) == z[j] \
                for j in range(R)), name = "one_user_per_position")

            # Add a definition constraint
            m.addConstrs(quicksum([x[(i, j)] for j in range(R)]) == a[i] for i in userRange)

            # Add blocking definition constraint
            m.addConstrs(1000 * y[i] >= (1 - a[i]) * (quicksum([a[i2] for i2 in range(i + 1, userRange[-1] + 1)]) + R - quicksum([z[j] for j in range(R)])) \
                for i in range(1, userRange[-1]))
            #m.addConstr(1000 * y[userRange[-1]] >= (1 - a[userRange[-1]]) * (R - quicksum([z[j] for j in range(R)])))

            # Add constraint: x_{i, j} = 0, if user i cannot start at position j
            print(users[1].begins)
            m.addConstrs((x[(i, j)] == 0 \
                for i in userRange \
                for j in range(R) \
                if j not in users[i].begins), name = "user_cannot_start")

            m.setParam('TimeLimit', 30)
            start_time = time.time()
            m.optimize()
            if not graph:
                f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")

            #[28, 28, 9, 14, 7, 12, 22, 22, 39, 39, 39, 39, 10, 8, 17, 4, 21, 21, 20, 2]
            solution = [-1 for _ in range(R)]
            solutionOrigId = [-1 for _ in range(R)]
            solved = False
            for j in range(R):
                for i in userRange:
                    if x[(i, j)].x > 0.1:
                        solved = True
                        users[i].printUser()
                        solutionStatistics.countUser(users[i])
                        for k in range(j, j + users[i].size):
                            solution[k] = i
                            solutionOrigId[k] = users[i].originalId
                        #print("user " + str(i) + " start at " + str(j) + " ends at " + str(j + users[i].size - 1))

            if solved:
                print(solution)
                filled = getFilledPositions(solution)
                blocked = getBlockedUsers(solution, R, filled, numberOfUsers)
                solutionStatistics.addFilledPositions(filled)
                solutionStatistics.addBlockedUsers(blocked)
                solutionStatistics.addUsers()
                solutionStatistics.maxId = max(solution)
                print(solutionStatistics.maxId)
            
            if not graph:
                f.write("Subframe: " + str(frame) + "\n")
                f.write("Solution:\n")
                f.write(str(solution) + "\n")
                f.write("Filled Positions Rate: " + str(filled) + "/" + str(R) + "\n")
                f.write("Number of Blocked Users: " + str(blocked) + "\n")
                f.write("Objective Value: " + str(m.objVal))
                f.write("---------------------------------------------------------------------\n")

        if not graph:
            f.write("Mean Filled: " + str(solutionStatistics.getFilledPositionMean()) + "\n")
            f.write("Mean Blocked: " + str(solutionStatistics.getBlockedUsersMean()) + "\n")
        
        return solutionStatistics
        
    except GurobiError as e:
        print('Error code ' + str(e.errno) + ": " + str(e))

    except AttributeError:
        print('Encountered an attribute error')