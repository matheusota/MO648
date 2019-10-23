#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "filereader.h"
#include "main.h"
#include <vector>
#include "heuristic.h"
#include <chrono>
#include "brkgaheuristic.h"
#include "schedulingmodel.h"
#include "simulator.h"
#include "besteffort.h"
#include "bruteforce.h"
#include "params.h"
#include "sppmodel.h"
#include "reshuffle.h"
#include "schedulingmodel2.h"
#include "bestmodel.h"

using namespace std;

int main(int argc, char *argv[]);
void readCheckParams(Params &params, int argc, char *argv[]);
void generateOutput(Params &params, int numberUsers, Measures &measures);
void generateOutputAll(Params &params, int numberUsers, double totalBlock, double totalFill, int totalTime, double count);
#endif // MAIN_H
