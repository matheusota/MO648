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

def getBlockedUsers(solution, R, positionsFilled, numberOfUsers):
    n = len(solution)
    users = sorted(list((set(solution))))
    users = [u for u in users if u > 0]
    blocked = 0
    prevUser = 0

    for u in users:
        blocked += (u - prevUser) - 1
        prevUser = u

    if positionsFilled != R:
        blocked += numberOfUsers - prevUser
        
    return blocked