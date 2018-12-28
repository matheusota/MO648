#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "user.h"
#include <utility>
#include <algorithm>
#include <array>
#include <random>
#include <map>
#include <vector>
#include <chrono>
#include "measures.h"
#include "main.h"
#include "params.h"
#include <stdlib.h>
#include <set>

using namespace std;

class Simulator
{
    private:
        static map<int, int> PDCCHNumberOfCandidates_UESpecific;
        static map<int, int> PDCCHNumberOfCandidates_Common;

        static void setAggregationLevel(int nUes, vector<User> &users, double pbAl1, double pbAl2, double pbAl4, double pbAl8);
        static void setBegins(vector<User> &users, int nCCEs);
        static int calcCCEIndex(int m, int i, User user, int nCCEs);
        static int Calc_Y(int k, int RNTI);
        static void runUntilEmpty(Params &params, vector<User> &users, Measures &measures, void (*scheduler)(vector<User>&, int, int, Measures&));

    public:
        static int nSubframe;

        static void simulate(Params &params, int numberUsers, void (*scheduler)(vector<User>&, int, int, Measures&));
};

#endif // SIMULATOR_H
