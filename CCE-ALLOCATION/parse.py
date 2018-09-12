class User:
    def __init__(self, id, size, userCount):
        self.size = size
        self.id = id
        
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
        self.price = 1.0 / userCount     
        self.begins = set()

    def printUser(self):
        print("User: " + str(self.id))
        print("\tsize: " + str(self.size))
        print("\tbegins:" + str(self.begins))

def getInput():
    users = {}
    userCount = 0

    file = open("entrada-50-20.txt")
    i = 0
    frame = 0
    for line in file:
        line_items = line.split()

        if line_items[0] == "Subframe":
            if i == frame + 1:
                break
            else:
                i += 1
        else:
            first = False
            if line_items[0] == "Subframe":
                continue
            else:
                userCount += 1
                userid = int(line_items[0])
                begin = None
                count = None
                user = None

                for item in line_items[1:]:
                    if item == "[":
                        count = 0
                        begin = None
                    
                    elif item == "]":
                        if user == None:
                            user = User(userid, count, userCount)
                            user.begins.add(begin)
                            users[userid] = user
                        else:
                            user.begins.add(begin)
                        
                        count = None

                    elif count != None:
                        if begin == None:
                            begin = int(item)
                        count += 1

    #for u in users.keys():
    #    users[u].printUser()

    return users

getInput()
                