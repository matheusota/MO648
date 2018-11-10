import sys
from modelRound import *
from modelRoundProb import *
from modelSimple import *
from bestEffort import *
from graphCreator import *
from solutionStatistics import *
from modelSPP import *
from reshuffle import *
from modelQuadratic import *
from modelBest import *
from modelBestRound import *
from bruteForce import *
from modelSimple2 import *
from heuristic import *

if len(sys.argv) < 3:
    print("Please, pass the number of CCE's as a parameter")
    sys.exit(0)
else:
    R = int(sys.argv[1])
    mode = sys.argv[2]

    if mode == "-g":
        statisticsList = []
        numberOfUsers = []
        for i in range(10, 70, 10): 
            solutionStatistics = reshuffle(i, R, 100, True)
            statisticsList.append(solutionStatistics)
            numberOfUsers.append(i)
        
        createGraph(numberOfUsers, statisticsList)

    elif mode == "-best_effort":
        bestEffort(51, R, 10, False)
    elif mode == "-simple":
        modelSimple(51, R, 10, False)
    elif mode == "-simple2":
        modelSimple2(51, R, 10, False)
    elif mode == "-round":
        modelRound(51, R, 10, False)
    elif mode == "-roundprob":
        modelRoundProb(51, R, 10, False)
    elif mode == "-spp":
        modelSPP(51, R, 10, False)
    elif mode == "-reshuffle":
        reshuffle(51, R, 10, False)
    elif mode == "-quadratic":
        modelQuadratic(51, R, 10, False)
    elif mode == "-best":
        modelBest(51, R, 10, False)
    elif mode == "-best_round":
        modelBestRound(51, R, 10, False)
    elif mode == "-brute_force":
        bruteForce(51, R, 10, False)
    elif mode == "-heuristic":
        heuristic(51, R, 10, False)