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

using namespace std;

typedef struct structParams
{
    string alg;
    int numberSubframes;
    int numberUsers;
    int R;
    bool shouldSimulate;
} Params;

int main(int argc, char *argv[]);
void readCheckParams(Params &params, int argc, char *argv[]);
#endif // MAIN_H
