#ifndef BESTEFFORT_H
#define BESTEFFORT_H
#include "user.h"
#include <vector>
#include "segtree.h"

class BestEffort
{
    private:
        static bool tryToAllocate(User &user, SegTree &solutionTree);

    public:
        static vector<int> execute(vector<User> users, int R, int numberUsers);
};

#endif // BESTEFFORT_H
