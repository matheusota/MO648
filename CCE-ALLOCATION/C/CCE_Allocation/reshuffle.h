#ifndef RESHUFFLE_H
#define RESHUFFLE_H
#include "user.h"
#include <utility>
#include <algorithm>
#include <array>
#include "measures.h"

class Reshuffle
{
    private:
        static bool tryToAllocate(User &user, vector<int> &solution);
    public:
        static void execute(vector<User> &users2, int R, int numberUsers, Measures &measures);
};

#endif // RESHUFFLE_H
