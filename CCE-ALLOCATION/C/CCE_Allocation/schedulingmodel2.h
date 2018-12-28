#ifndef SCHEDULINGMODEL2_H
#define SCHEDULINGMODEL2_H
#include <gurobi_c++.h>
#include <map>
#include "user.h"
#include <algorithm>
#include "measures.h"

class SchedulingModel2
{
    public:
        static void execute(vector<User> &users2, int R, int numberUsers, Measures &measures);
};

#endif // SCHEDULINGMODEL2_H
