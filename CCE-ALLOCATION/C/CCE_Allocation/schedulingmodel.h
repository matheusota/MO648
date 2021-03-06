#ifndef SCHEDULINGMODEL_H
#define SCHEDULINGMODEL_H
#include <gurobi_c++.h>
#include <map>
#include "user.h"
#include <algorithm>
#include "measures.h"
#include "cutscallback.h"

using namespace std;

class SchedulingModel
{
    public:
        static bool shouldUseF;
        static void execute(vector<User> &users2, int R, int numberUsers, Measures &measures);
};

#endif // SCHEDULINGMODEL_H
