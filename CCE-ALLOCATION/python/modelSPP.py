"""
Set Packing Formulation
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

def modelSPP(numberOfUsers, R, numberOfSubframes, graph):
    try:
        if not graph:        
            f = open("output/cce_output_spp_" + str(R) + ".txt", "w")
        solutionStatistics = SolutionStatistics()

        for frame in range(numberOfSubframes):
            users = getInput(frame, numberOfUsers, R, numberOfSubframes)
            userRange = sorted(users.keys())

            # Create a new model
            m = Model("cce_allocation")

            # Create variables
            x = m.addVars(userRange, range(6), vtype=GRB.INTEGER, name="x")

            # Set objective
            m.setObjective(quicksum([users[i].getPriceForCandidate(k) * x[(i, k)] for i in userRange for k in range(6)]), GRB.MAXIMIZE)

            # Add constraint: \sum_i sum_k a_{i, j} ^k * x_i^k <= 1, \forall j
            m.addConstrs((quicksum([users[i].getCandidateCoef(j, k) * x[(i, k)] for i in userRange for k in range(6)]) <= 1 \
                for j in range(R)), name = "one_user_per_position")
            
            # Add constraint: \sum_k x_i^k <= 1, \forall i
            m.addConstrs((quicksum([x[(i, k)] for k in range(6)]) <= 1 \
                for i in userRange), name = "user_starts_once")

            # x can be set only for allowed candidates
            for i in userRange:
                for k in range(len(users[i].begins), 6):
                    m.addConstr(x[(i, k)] == 0)

            start_time = time.time()
            m.optimize()
            if not graph:
                f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")
            
            roundedVars = set()

            # get variables set to one
            for i in userRange:
                for k in range(6):
                    if x[(i, k)].x > 0.5:
                        roundedVars.add((i, k))

            #[28, 28, 9, 14, 7, 12, 22, 22, 39, 39, 39, 39, 10, 8, 17, 4, 21, 21, 20, 2]
            allocatedUsers = set()
            solution = [-1 for _ in range(R)]
            solutionOrigId = [-1 for _ in range(R)]
            for i in userRange:
                for k in range(6):
                    if (i, k) in roundedVars:
                        allocatedUsers.add(i)
                        users[i].printUser()
                        solutionStatistics.countUser(users[i])

                        if users[i].getCandidateBegin(k) >= 0:
                            start = users[i].getCandidateBegin(k)
                            for j in range(start, start + users[i].size):
                                solution[j] = i
                                solutionOrigId[j] = users[i].originalId
                        else:
                            print("WAT " + str(k))
                            users[i].printUser()

            # allocate more users if possible
            for j in range(R):
                if solution[j] == -1: 
                    for i in userRange:
                        if (i not in allocatedUsers) and (j in users[i].begins):
                            if addIfPossible(solution, users, i, j):
                                print("blabla")
                                solutionStatistics.countUser(users[i])
                                allocatedUsers.add(i)
                                break

            print(solution)
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