#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H
#include "user.h"
#include <utility>
#include <algorithm>
#include <array>
#include <random>
#include "measures.h"
#include "mtrand.h"
#include "brkga.h"
#include "decoder.h"

class BruteForce
{
    private:
        static bool tryToAllocate(User &user, SegTree &solutionTree);
        static int R;
        static int numberUsers;
        static double bestCost;
        static vector<int> bestSol;
        static double bruteForceScheduler(vector<User> users, SegTree &solutionTree, int slotsFilled);
    public:
        static void execute(vector<User> &users, int R, int numberUsers, Measures &measures);
};

#endif // BRUTEFORCE_H
