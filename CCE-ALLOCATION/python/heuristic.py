from parse import *
from measures import *
import time
import sys
from solutionStatistics import *
import random
import pdb

probabilistic = False

def allocateUser(u, resources, b):
    for j in range(b, b + u.size):
        if resources[j] != -1:
            return False
    
    for j in range(b, b + u.size):
        resources[j] = u.id
    
    return True

# this is just the main routine of the heuristic scheduler
def heuristicScheduler(users, R):
    global probabilistic

    userRange = sorted(users.keys())
    slotsFilled = 0
    resources = [-1 for _ in range(R)]
    count = [0 for _ in range(R)]
    #random.shuffle(userRange)
    #print(userRange)

    # fill the count array
    for i in userRange:
        for b in users[i].begins:
            for j in range(b, b + users[i].size):
                count[j] += 1

    # main algorithm loop
    it = 0
    while it < len(users) and slotsFilled < R:
        # count only for five users ahead
        if it % 5 == 0:
            count = [0 for _ in range(R)]

            # fill the count array
            for i2 in userRange[it: it + 5]:
                for b in users[i2].begins:
                    for j in range(b, b + users[i2].size):
                        count[j] += 1

            # we will iterate through this range
            newUserRange = userRange[it:it + 5]
            if probabilistic:
                random.shuffle(newUserRange)

        # main loop
        for i in newUserRange:
            allocated = False

            if probabilistic:
                """
                r = random.random()

                if users[i].size == 8 and r < 0.2:
                    it += 1
                    continue
                """
                
                # put on tmp allowed beginnings
                tmp = []
                for b in users[i].begins:
                    flag = False
                    for j in range(b, b + users[i].size):
                        if resources[j] != -1:
                            flag = True
                            break
                    
                    if not flag:
                        tmp.append(b)

                if len(tmp) == 0:
                    it += 1
                    continue
                
                # use the counts as probability
                s = 0
                for b in tmp:
                    for j in range(b, b + users[i].size):
                        s += count[j]
                s = float(s)

                prob = []
                for b in tmp:
                    p = 0
                    for j in range(b, b + users[i].size):
                        p += count[j]
                    p = float(p)
                    if len(prob) == 0:
                        prob.append(p/s)
                    else:
                        prob.append(prob[-1] + p/s)
                
                print(prob)
                
                # allocate accordingly
                r = random.random()
                for k in range(len(tmp)):
                    if prob[k] >= r:
                        allocated = allocateUser(users[i], resources, tmp[k])
                        
                        for j in range(b, b + users[i].size):
                            count[j] -= 1
                        
                        break
            else:
                # we will sort the begins positions looking at the count array
                tmp = []
                for b in users[i].begins:
                    c = 0
                    for j in range(b, b + users[i].size):
                        c += count[j]
                    tmp.append((c, b))
                tmp = sorted(tmp)

                # now, in this order, we try to allocate the user
                allocated = False
                for (_, b) in tmp:
                    allocated = allocateUser(users[i], resources, b)
                    if allocated:
                        for j in range(b, b + users[i].size):
                            count[j] -= 1
                        
                        break

            if allocated:
                slotsFilled += users[i].size
            
            if slotsFilled == R:
                break
            
            #pdb.set_trace()
            it += 1
    
    return resources

# main routine
def heuristic(numberOfUsers, R, numberOfSubframes, graph):
    global probabilistic

    if not graph:
        f = open("output/cce_output_heuristic_" + str(R) + ".txt", "w")
    solutionStatistics = SolutionStatistics()

    #if R == 20:
    #    numberOfUsers = min(25, numberOfUsers)

    for frame in range(numberOfSubframes):
        users = getInput(frame, numberOfUsers, R, numberOfSubframes)

        start_time = time.time()
        bestBlocked = float("inf")
        bestFilled = 0
        solution = []
        maxIt = 1
        if probabilistic:
            maxIt = 100
        for _ in range(maxIt):
            resources = heuristicScheduler(users, R)

            filled = getFilledPositions(resources)
            blocked = getBlockedUsers(resources, R, filled, numberOfUsers)
            if blocked < bestBlocked:
                bestBlocked = blocked
                bestFilled = filled
                solution = resources

        if not graph:
            f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")
        
        solutionStatistics.addFilledPositions(bestFilled)
        solutionStatistics.addBlockedUsers(bestBlocked)
        solutionStatistics.addMaxId(max(solution))
        
        allocatedUsers = list(set(solution))
        for id in allocatedUsers:
            if id != -1:
                solutionStatistics.countUser(users[id])
        solutionStatistics.addUsers()

        print(solution)
        if not graph:
            f.write("Subframe: " + str(frame) + "\n")
            f.write("Solution:\n")
            f.write(str(solution) + "\n")
            f.write("Filled Positions Rate: " + str(bestFilled) + "/" + str(R) + "\n")
            f.write("Number of Blocked Users: " + str(bestBlocked) + "\n")
            f.write("---------------------------------------------------------------------\n")

    if not graph:
        f.write("Mean Filled: " + str(solutionStatistics.getFilledPositionMean()) + "\n")
        f.write("Mean Blocked: " + str(solutionStatistics.getBlockedUsersMean()) + "\n")

    return solutionStatistics