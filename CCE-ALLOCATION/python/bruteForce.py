from parse import *
from measures import *
import time
import sys
from solutionStatistics import *
import pdb

def canAllocate(u, b, A):
    for j in range(b, b + u.size):
        if A[j] == 1:
            return False
    
    return True

# this is just the main routine of the best effort scheduler
def bruteForceScheduler(users, U, A, DP):
    #print("calling")
    #print("U = " + str(U))
    #print("A = " + str(A))

    #pdb.set_trace()
    if sum(A) == len(A):
        return ([], 0)

    U2 = frozenset(U)
    A2 = tuple(A)

    #print(DP)
    if (U2, A2) in DP.keys():
        return DP[(U2, A2)]
    
    else:
        ansVal = 0
        ans = []

        for i in U:
            for b in users[i].begins:
                 if canAllocate(users[i], b, A):
                    # allocate user
                    for j in range(b, b + users[i].size):
                        A[j] = 1
                    
                    # solve recursively
                    (sol, solVal) = bruteForceScheduler(users, U - {i}, A, DP)

                    # remove allocated user
                    for j in range(b, b + users[i].size):
                        A[j] = 0

                    # update solution
                    if solVal + users[i].size > ansVal:
                        ansVal = solVal + users[i].size
                        ans = list(sol)
                        ans.append((i, b))

                        if ansVal == 20:
                            print("solution " + str(ans))
                            DP[(U2, A2)] = (ans, ansVal)
                            return (ans, ansVal)

        DP[(U2, A2)] = (ans, ansVal)
        return (ans, ansVal)

# main routine
def bruteForce(numberOfUsers, R, numberOfSubframes, graph):
    if not graph:
        f = open("output/cce_output_bruteforce_" + str(R) + ".txt", "w")
    solutionStatistics = SolutionStatistics()

    if numberOfUsers > 15 and R == 20:
        numberOfUsers = 9
    elif numberOfUsers > 30 and R == 40:
        numberOfUsers = 30

    for frame in range(numberOfSubframes):
        users = getInput(frame, numberOfUsers, R, numberOfSubframes)

        numberOfSubframes = 1
        start_time = time.time()
        A = [0 for _ in range(R)]
        U = set(range(1, numberOfUsers + 1))
        DP = {}
        (solution, _) = bruteForceScheduler(users, U, A, DP)
        
        print("solved")
        # create solution
        resources = [-1 for _ in range(R)]
        print(solution)
        for (id, b) in solution:
            for j in range(b, b + users[id].size):
                resources[j] = id
        
        print(resources)
        if not graph:
            f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")

        filled = getFilledPositions(resources)
        blocked = getBlockedUsers(resources, R, filled, numberOfUsers)
        solutionStatistics.addFilledPositions(filled)
        solutionStatistics.addBlockedUsers(blocked)
        solutionStatistics.addMaxId(max(resources))
        
        allocatedUsers = list(set(resources))
        for id in allocatedUsers:
            if id != -1:
                solutionStatistics.countUser(users[id])
        solutionStatistics.addUsers()

        if not graph:
            f.write("Subframe: " + str(frame) + "\n")
            f.write("Solution:\n")
            f.write(str(resources) + "\n")
            f.write("Filled Positions Rate: " + str(filled) + "/" + str(R) + "\n")
            f.write("Number of Blocked Users: " + str(blocked) + "\n")
            f.write("---------------------------------------------------------------------\n")

    if not graph:
        f.write("Mean Filled: " + str(solutionStatistics.getFilledPositionMean()) + "\n")
        f.write("Mean Blocked: " + str(solutionStatistics.getBlockedUsersMean()) + "\n")

    return solutionStatistics