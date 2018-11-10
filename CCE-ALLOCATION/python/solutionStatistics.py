from parse import *

class SolutionStatistics:
    def __init__(self):
        self.filledPositions = []
        self.blockedUsers = []
        self.user1count = []
        self.user2count = []
        self.user4count = []
        self.user8count = []
        self.totalUsers = []
        self.maxId = []

        self.aux1count = 0
        self.aux2count = 0
        self.aux4count = 0
        self.aux8count = 0

    def addFilledPositions(self, x):
        self.filledPositions.append(x)
    
    def addBlockedUsers(self, x):
        self.blockedUsers.append(x)
    
    def addMaxId(self, x):
        self.maxId.append(x)
        
    def getFilledPositionMean(self):
        return sum(self.filledPositions)/float(len(self.filledPositions))
    
    def getBlockedUsersMean(self):
        return sum(self.blockedUsers)/float(len(self.blockedUsers))
    
    def countUser(self, user):
        if user.size == 1:
            self.aux1count += 1
        elif user.size == 2:
            self.aux2count += 1
        elif user.size == 4:
            self.aux4count += 1
        else:
            self.aux8count += 1
    
    def addUsers(self):
        self.user1count.append(self.aux1count)
        self.user2count.append(self.aux2count)
        self.user4count.append(self.aux4count)
        self.user8count.append(self.aux8count)
        self.totalUsers.append(self.aux1count + self.aux2count + self.aux4count + self.aux8count)

        self.aux1count = 0
        self.aux2count = 0
        self.aux4count = 0
        self.aux8count = 0

    def getTotalUsers(self):
        return (self.user1count + self.user2count + self.user4count + self.user8count)/len(self.blockedUsers)
        