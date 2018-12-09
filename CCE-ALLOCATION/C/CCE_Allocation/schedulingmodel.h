#ifndef SCHEDULINGMODEL_H
#define SCHEDULINGMODEL_H
#include <gurobi_c++.h>
#include <map>
#include "user.h"
#include <algorithm>
#include "measures.h"

using namespace std;

class SchedulingModel
{
    public:
        static void execute(vector<User> &users2, int R, int numberUsers, Measures &measures);
};

#endif // SCHEDULINGMODEL_H
