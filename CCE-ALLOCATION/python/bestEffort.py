from parse import *
from measures import *
import time
import sys
from solutionStatistics import *
import random

# this is just the main routine of the best effort scheduler
def bestEffortScheduler(users, R):
    userRange = sorted(users.keys())
    slotsFilled = 0
    resources = [-1 for _ in range(R)]

    for id in userRange:
        allocated = allocateUser(id, users, resources)

        if allocated:
            slotsFilled += users[id].size
        
        if slotsFilled == R:
            break
    
    return resources

# given a user id, this function will, if possible, allocate the user in the first empty position 
def allocateUser(id, users, resources):
    begins = users[id].begins
    
    for b in begins:
        if tryToAllocateUser(b, begins, 0, id, users, resources):
            return True

    return False
    """
    tmp = []
    for b in begins:
        flag = False
        for j in range(b, b + users[id].size):
            if resources[j] != -1:
                flag = True
                break
        
        if not flag:
            tmp.append(b)

    #print(tmp)
    if len(tmp) == 0:
        return False

    r = random.randint(0, len(tmp) - 1)

    for j in range(tmp[r], tmp[r] + users[id].size):
        resources[j] = id

    return True
    """

# this function will try to allocate a user. Note that we use recursion here to backtrack when trying to allocate
def tryToAllocateUser(b, begins, i, id, users, resources):
    if i == users[id].size:
        return True

    idx = b + i
    #idx = getCCEIndex(b, i, id, users, resources)
    #print("allocating at " + str(idx))

    if resources[idx] != -1:
        return False
    else:
        if tryToAllocateUser(b, begins, i + 1, id, users, resources):
            resources[idx] = id
            return True

# this implements the hash function to get the index it should try to allocate
def getCCEIndex(m, i, id, users, resources):
    L = users[id].size
    k = 50 % 10
    N = len(resources)
    T = N//L
    C = L * ((getY(k, id) + m) % T) + i

    return C

# get a parameter of the hash function
def getY(k, id):
    y = id

    if k >= 0:
        t = getY(k - 1, id)
        y = (39827 * t) % 65537

    return y

# main routine
def bestEffort(numberOfUsers, R, numberOfSubframes, graph):
    if not graph:
        f = open("output/cce_output_besteffort_" + str(R) + ".txt", "w")
    solutionStatistics = SolutionStatistics()

    for frame in range(numberOfSubframes):
        users = getInput(frame, numberOfUsers, R, numberOfSubframes)

        start_time = time.time()
        resources = bestEffortScheduler(users, R)
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