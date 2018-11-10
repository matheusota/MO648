from parse import *
from measures import *
import time
import sys
from solutionStatistics import *
from bestEffort import *
import pdb

# this is just the main routine of the reshuffle scheduler
def reshuffleScheduler(users, R):
    userRange = sorted(users.keys())
    slotsFilled = 0
    resources = [-1 for _ in range(R)]

    for id in userRange:
        users[id].printUser()
        #pdb.set_trace()
        # try to allocate this user
        allocated = allocateUser(id, users, resources)
        print(resources)
        #pdb.set_trace()

        # user was successfully allocated
        if allocated:
            slotsFilled += users[id].size

            # CCE array is full
            if slotsFilled == R:
                break
        
        # we could not allocate this user, try "reshuffling"
        else:
            for b in users[id].begins:
                markedUsers = set()
                tmp = resources
                
                # get users that are occupying the positions of this candidate
                for j in range(b, b + users[id].size):
                    if resources[j] != -1:
                        markedUsers.add(resources[j])
                    tmp[j] = id

                # now remove each user and try to reallocate it somewhere
                success = True
                for id2 in markedUsers:
                    for j in range(R):
                        if tmp[j] == id2:
                            tmp[j] = -1
                    
                    if not allocateUser(id2, users, resources):
                        success = False
                        break
                
                # if we were successful in the reallocation, update
                if success:
                    resources = tmp
                    slotsFilled += users[id].size
                    break

    print("finished!")
    #pdb.set_trace()           
    return resources

# main routine
def reshuffle(numberOfUsers, R, numberOfSubframes, graph):
    if not graph:
        f = open("output/cce_output_reshuffle_" + str(R) + ".txt", "w")
    solutionStatistics = SolutionStatistics()

    for frame in range(numberOfSubframes):
        users = getInput(frame, numberOfUsers, R, numberOfSubframes)

        start_time = time.time()
        resources = reshuffleScheduler(users, R)
        if not graph:
            f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")

        filled = getFilledPositions(resources)
        blocked = getBlockedUsers(resources, R, filled, numberOfUsers)
        solutionStatistics.addFilledPositions(filled)
        solutionStatistics.addBlockedUsers(blocked)

        allocatedUsers = list(set(resources))
        for id in allocatedUsers:
            if id != -1:
                solutionStatistics.countUser(users[id])
        solutionStatistics.addUsers()

        if not graph:
            f.write("Subframe: " + str(frame) + "\n")
            f.write("Filled Positions Rate: " + str(filled) + "/" + str(R) + "\n")
            f.write("Number of Blocked Users: " + str(blocked) + "\n")
            f.write("---------------------------------------------------------------------\n")

    if not graph:
        f.write("Mean Filled: " + str(solutionStatistics.getFilledPositionMean()) + "\n")
        f.write("Mean Blocked: " + str(solutionStatistics.getBlockedUsersMean()) + "\n")

    return solutionStatistics