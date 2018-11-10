import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
import itertools
from solutionStatistics import *
from math import sqrt

def std(L):
    mean = sum(L)/float(len(L))

    acc = 0
    for e in L:
        acc += (e - mean) * (e - mean)
    
    acc /= float(len(L) - 1)
    return sqrt(acc)

def createGraph(numberOfUsers, statisticsList):
    sns.set_style("darkgrid")
    sns.set_palette("hls")
    palette = itertools.cycle(sns.color_palette())

    df = pd.DataFrame(columns=["number of users", "count", "legend"])

    i = 0
    for j in range(len(numberOfUsers)):
        s = statisticsList[j]
        #print("std " + str(std(s.blockedUsers)))
        #print(s.blockedUsers)

        for k in range(len(s.filledPositions)):
            df.loc[i] = [numberOfUsers[j], s.filledPositions[k], "filled positions"]
            i += 1

        for k in range(len(s.totalUsers)):
            df.loc[i] = [numberOfUsers[j], s.totalUsers[k], "users allocated"]
            i += 1
        
        for k in range(len(s.maxId)):
            df.loc[i] = [numberOfUsers[j], s.maxId[k], "max id"]
            i += 1
        
        for k in range(len(s.user1count)):
            df.loc[i] = [numberOfUsers[j], s.user1count[k], "users of size 1 allocated"]
            i += 1
        
        for k in range(len(s.user2count)):
            df.loc[i] = [numberOfUsers[j], s.user2count[k], "users of size 2 allocated"]
            i += 1
        
        for k in range(len(s.user4count)):
            df.loc[i] = [numberOfUsers[j], s.user4count[k], "users of size 4 allocated"]
            i += 1
        
        for k in range(len(s.user8count)):
            df.loc[i] = [numberOfUsers[j], s.user8count[k], "users of size 8 allocated"]
            i += 1
        
        for k in range(len(s.blockedUsers)):
            df.loc[i] = [numberOfUsers[j], s.blockedUsers[k], "blocked users"]
            i += 1

    ax = sns.pointplot(x="number of users", y="count", hue="legend", capsize=0.05, ci="sd", data=df)
    plt.legend(frameon=True)
    plt.show()
