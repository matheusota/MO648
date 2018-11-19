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
    public:
        static int nSubframe;

        static void simulate(int nUEs, int nSubframes, int nCCEs, vector<int> (*scheduler)(vector<User>, int, int));
};

#endif // SIMULATOR_H
