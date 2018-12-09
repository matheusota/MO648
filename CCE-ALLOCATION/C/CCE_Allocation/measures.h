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
    private:
        int R;
        int numberOfUsers;
        int blockedUsers;
        int filledPositions;

        void setFilledPositions();
        void setBlockedUsers();

    public:
        vector<int> solution;

        Measures(int numberOfUsers, int R);
        Measures(int numberOfUsers, int R, vector<int> &solution);

        void setSolution(vector<int> &solution);
        void computeBlockedUsers();
        double getBlockedRate();
        double getResourceRate();
        int getBlockedUsers();
        int getFilledPositions();
};

#endif // MEASURES_H
