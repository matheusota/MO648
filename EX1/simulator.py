import numpy as np
import pandas as pd
import itertools
import matplotlib.pyplot as plt
import math
import random
import statistics 
import seaborn as sns

# queue class
class Queue():
    def __init__(self, arrivalRate, departureRate):
        self.usersInQueue = 0
        self.servicedUsers = 0
        self.currTime = 0
        self.arrivalParam = 1./arrivalRate
        self.departureParam = 1./departureRate
        self.arrivalTime = self.getRandomTime(self.arrivalParam)
        self.departureTime = float("inf")
        self.totalWaitingTime = 0
    
    def getRandomTime(self, param):
        return random.expovariate(param)

    def goToNextEvent(self):
        if self.arrivalTime < self.departureTime:
            self.totalWaitingTime += (self.arrivalTime - self.currTime)
            self.currTime = self.arrivalTime
            self.handleArrivalEvent()
        else:
            self.totalWaitingTime += (self.departureTime - self.currTime)
            self.currTime = self.departureTime
            self.handleDepartureEvent()
    
    def handleArrivalEvent(self):
        self.usersInQueue += 1

        if self.usersInQueue <= 1:
            self.departureTime = self.currTime + self.getRandomTime(self.departureParam)

        self.arrivalTime = self.currTime + self.getRandomTime(self.arrivalParam)

    def handleDepartureEvent(self):
        self.usersInQueue -= 1
        self.servicedUsers += 1
        
        if self.usersInQueue > 0:
            self.departureTime = self.currTime + self.getRandomTime(self.departureParam)
        else:
            self.departureTime = float("inf")

    def getMeanDelay(self):
        if self.servicedUsers == 0:
            return 0
        else:
            return self.totalWaitingTime / self.servicedUsers

# the main code

q = Queue(0.25, 0.5)

df = pd.DataFrame(columns=["events executed", "mean delay"])


def getMean(L):
    return sum(L) / len(L)

def getErr(L):
    return (1.96) * (statistics.stdev(L) / math.sqrt(len(L)))

means = []
errs = []
utilization = []
departureRate = 1

# we change the arrival rate to change the utilization
for x in range(5, 100, 5):
    arrivalRate = x / 100.
    L = []
    
    # run a bunch of simulations and collect mean delays and errors
    for j in range(100):
        q = Queue(arrivalRate, departureRate)
        
        for _ in range(1000):
            q.goToNextEvent()
    
        L.append(q.getMeanDelay())
    
    utilization.append(round(arrivalRate / departureRate, 2))
    means.append(getMean(L))
    errs.append(getErr(L))

# create a pandas dataframe for the collected data
aux = {}
aux["utilization"] = utilization
aux["mean delay"] = means
df = pd.DataFrame(aux)

# plot the means using seaborn
ax = sns.pointplot(x="utilization", y="mean delay", data=df, color="limegreen", ci = 95)

# plot confidence intervals using matplotlib
ax.errorbar(df.index, means, yerr=errs, fmt="-", color="limegreen", capsize = 3, elinewidth = 2)

plt.show()