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
        # self.price = 1.0 / userCount
        self.price = 1
        self.begins = set()

    def printUser(self):
        print("User: " + str(self.id))
        print("\tsize: " + str(self.size))
        print("\tbegins:" + str(self.begins))

def getInput(frame, R):
    users = {}
    userCount = 0

    file = open("input/entrada-50-" + str(R) + ".txt")
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
                        user.begins.add(begin)
                        users[userCount] = user
                    else:
                        user.begins.add(begin)
                    
                    size = None

                elif size != None:
                    if begin == None:
                        begin = int(item)
                    size += 1

    #for u in users.keys():
    #    users[u].printUser()

    return users
                