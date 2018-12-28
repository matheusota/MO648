class User:
    def __init__(self, id, size, userCount):
        self.size = size
        self.id = userCount
        self.originalId = id
        
        """
        if self.size == 2:
            self.price = self.size + 0.125
        elif self.size == 4:
            self.price = self.size + 0.25
        elif self.size == 8:
            self.price = self.size + 0.5
        else:
            self.price = self.size
        """
        self.price = 1.0 / userCount * size
        #self.price = 1
        self.begins = []

    def getCandidateCoef(self, pos, k):
        candidates = sorted(list(self.begins))

        if k >= len(candidates):
            return 0

        if candidates[k] <= pos and candidates[k] + self.size > pos:
            return 1
        else:
            return 0
    
    def getPriceForCandidate(self, k):
        candidates = sorted(list(self.begins))

        if k >= len(candidates):
            return 0
        else:
            return self.price
    
    def getCandidateBegin(self, k):
        candidates = sorted(list(self.begins))

        if k >= len(candidates):
            return -1
        else:
            return candidates[k]

    def printUser(self):
        print("User: " + str(self.id))
        print("\tsize: " + str(self.size))
        print("\tbegins:" + str(self.begins))

def getInput(frame, numberOfUsers, R, numberOfSubframes):
    users = {}
    userCount = 0

    file = open("input/entrada-100-" + str(R) + "-" + str(numberOfSubframes) + ".txt")
    currFrame = 0
    for line in file:
        line = line.replace("-", "")
        line_items = line.split()

        if line_items[0] == "Subframe":
            if currFrame == frame + 1:
                break
            else:
                currFrame += 1

        elif currFrame == frame + 1:
            userCount += 1
            if userCount == numberOfUsers + 1:
                break
                
            userid = int(line_items[0])
            begin = None
            size = None
            user = None

            for item in line_items[1:]:
                if item == "[":
                    size = 0
                    begin = None
                
                elif item == "]":
                    if user == None:
                        user = User(userid, size, userCount)
                        user.begins.append(begin)
                        users[userCount] = user
                    else:
                        user.begins.append(begin)
                    
                    size = None

                elif size != None:
                    if begin == None:
                        begin = int(item)
                    size += 1

    #for u in users.keys():
    #    users[u].printUser()

    return users
                