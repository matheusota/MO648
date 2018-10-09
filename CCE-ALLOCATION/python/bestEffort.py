from parse import *
from measures import *
import time
import sys

# this is just the main routine of the best effort scheduler
def bestEffortScheduler(users, R):
    userRange = sorted(users.keys())
    slotsFilled = 0
    resources = [-1 for _ in range(R)]

    for id in userRange:
        (size, allocated) = allocateUser(id, users, resources)

        if allocated:
            slotsFilled += size
        
        if slotsFilled == R:
            break
    
    return resources

# given a user id, this function will, if possible, allocate the user in the first empty position 
def allocateUser(id, users, resources):
    begins = sorted(list(users[id].begins))
    for b in range(len(begins)):
        if tryToAllocateUser(b, begins, 0, id, users, resources):
            return (users[id].size, True)
    
    return (0, False)

# this function will try to allocate a user. Note that we use recursion here to backtrack when trying to allocate
def tryToAllocateUser(b, begins, i, id, users, resources):
    if i == users[id].size:
        return True

    idx = getCCEIndex(b, i, id, users, resources)

    if resources[idx] != -1:
        return False
    else:
        if tryToAllocateUser(b, begins, i + 1, id, users, resources):
            resources[idx] = users[id].originalId
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
if len(sys.argv) < 2:
    print("Please, pass the number of CCE's as a parameter")
    sys.exit(0)
else:
    R = int(sys.argv[1])

f = open("output/cce_output_besteffort_" + str(R) + ".txt", "w")
meanFilled = 0
meanBlocked = 0

for frame in range(10):
    users = getInput(frame, R)

    start_time = time.time()
    resources = bestEffortScheduler(users, R)
    f.write("Time to solve model: " + str(time.time() - start_time) + " seconds.\n")

    filled = getFilledPositions(resources)
    blocked = getBlockedUsers(resources)
    meanFilled += filled
    meanBlocked += blocked

    f.write("Subframe: " + str(frame) + "\n")
    f.write("Solution:\n")
    f.write(str(resources) + "\n")
    f.write("Filled Positions Rate: " + str(filled) + "/" + str(R) + "\n")
    f.write("Number of Blocked Users: " + str(blocked) + "\n")
    f.write("---------------------------------------------------------------------\n")

meanFilled = float(meanFilled)/10.0
meanBlocked = float(meanBlocked)/10.0
f.write("Mean Filled: " + str(meanFilled) + "\n")
f.write("Mean Blocked: " + str(meanBlocked) + "\n")