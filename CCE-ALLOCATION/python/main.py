import sys
from modelRound import *
from modelRoundProb import *
from modelSimple import *
from bestEffort import *

if len(sys.argv) < 3:
    print("Please, pass the number of CCE's as a parameter")
    sys.exit(0)
else:
    R = int(sys.argv[1])
    mode = sys.argv[2]

    if len(sys.argv) == 4:
        graph = sys.argv[3]

if graph == "-g":
    if mode == "-b":
        pass
    if mode == "-s":
        pass
    if mode == "-r":
        pass
    if mode == "-p":
        pass
else:
    pass