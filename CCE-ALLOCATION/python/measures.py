"""
Measures:
    blocked users
    filled positions
    sequential(?)
"""

def getFilledPositions(solution):
    n = len(solution)
    count = 0

    for x in solution:
        if x != -1:
            count += 1
    
    return count

def getBlockedUsers(solution):
    n = len(solution)
    users = sorted(list((set(solution))))
    print("blabla " + str(users))
    blocked = 0
    prevUser = 0

    for u in users:
        blocked += (u - prevUser) - 1
        prevUser = u

    return blocked