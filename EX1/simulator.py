"""
Primeiro exercicio: Fazer um programa de simulacao e coletar dados para plotar um grafico retardo medio x utilizacao. 
A utilizacao e a razao entre a taxa de chegada e a taxa de servico do servidor. Assuma que o intervalo entre chegada 
de pacotes e exponencialmente distribuido, bem como o tamanho dos pacotes. Entregar relatorio com grafico, codigo 
fonte e explicacao como derivou intervalo de confianca e qual foi o criterio adotado para eliminar o transiente da 
simulacao.
"""

import numpy as np

class Simulator():
    def __init__(self):
        self.usersCount = 0
        self.currTime = 0
        self.arrivalParam = 1./3
        self.departureParam = 1./4
        self.arrivalTime = self.getRandomTime(self.arrivalParam)
        self.departureTime = float("inf")
        self.arrivalsCount = 0
        self.departuresCount = 0
        self.totalWaitingTime = 0 

    def getRandomTime(self, param):
        return np.random.exponential(param)

    def handleArrivalEvent(self):
        self.usersCount += 1
        self.arrivalsCount += 1

        if self.usersCount <= 1:
            self.departureTime = self.currTime + self.getRandomTime(self.departureParam)

        self.arrivalTime = self.currTime + self.getRandomTime(self.arrivalParam)

    def handleDepartureEvent(self):
        self.usersCount -= 1
        self.departuresCount += 1

        if self.usersCount > 0:
            self.departureTime = self.currTime + self.getRandomTime(self.departureParam)
        else:
            self.departureTime = float("inf")

    def goToNextEvent(self):
        if self.arrivalTime < self.departureTime:
            self.totalWaitingTime += (self.arrivalTime - self.currTime)
            self.currTime = self.arrivalTime
            self.handleArrivalEvent()
        else:
            self.totalWaitingTime += (self.departureTime - self.currTime)
            self.currTime = self.departureTime
            self.handleDepartureEvent()
    
    def simulate(self, numberOfEvents):
        for i in range(numberOfEvents):
            self.goToNextEvent()
    
    def printData(self):
        print("departures " + str(self.departuresCount))
        print("arrivals " + str(self.arrivalsCount))
        print("total time " + str(self.totalWaitingTime))

np.random.seed(10)
s = Simulator()
s.simulate(1000)
s.printData()
