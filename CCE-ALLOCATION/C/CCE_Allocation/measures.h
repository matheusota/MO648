#ifndef MEASURES_H
#define MEASURES_H
#include "user.h"
#include <utility>
#include <algorithm>
#include <array>
#include <random>
#include <unordered_set>

class Measures
{
public:
    Measures();
    int getFilledPositions(vector<int> &solution);
    int getBlockedUsers(vector<int> &solution, int R, int filledPositions, int numberUsers);
};

#endif // MEASURES_H
