#ifndef HEURISTIC_H
#define HEURISTIC_H
#include "user.h"
#include <utility>
#include <algorithm>
#include <array>
#include <random>
#include "measures.h"
#include <thread>
#include <mutex>
#include "segtree.h"

using namespace std;

class Heuristic
{
    private:
        static vector<int> finalSolution;
        static int best;

        static bool tryToAllocate(User &user, SegTree &solutionTree, SegTree &counterTree);
        static void runHeuristic(vector<User> users, int R, int numberUsers, unsigned seed);

    public:
        static void execute(vector<User> &users, int R, int numberUsers, Measures &measures);
};
#endif // HEURISTIC_H
