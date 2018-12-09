#ifndef BESTEFFORT_H
#define BESTEFFORT_H
#include "user.h"
#include <vector>
#include "segtree.h"
#include "measures.h"

class BestEffort
{
    private:
        static bool tryToAllocate(User &user, SegTree &solutionTree);

    public:
        static void execute(vector<User> &users, int R, int numberUsers, Measures &measures);
};

#endif // BESTEFFORT_H
