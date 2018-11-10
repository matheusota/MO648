#ifndef SCHEDULINGMODEL_H
#define SCHEDULINGMODEL_H
#include <gurobi_c++.h>
#include <map>
#include "user.h"
#include <algorithm>

using namespace std;

class SchedulingModel
{
    public:
        static vector<int> execute(vector<User> users2, int R, int numberUsers);
};

#endif // SCHEDULINGMODEL_H
