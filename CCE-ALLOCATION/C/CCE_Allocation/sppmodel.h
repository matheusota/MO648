#ifndef SPPMODEL_H
#define SPPMODEL_H
#include <gurobi_c++.h>
#include <map>
#include "user.h"
#include <algorithm>
#include "measures.h"

using namespace std;

class SPPModel
{
    public:
        static void execute(vector<User> &users2, int R, int numberUsers, Measures &measures);
};

#endif // SPPMODEL_H
