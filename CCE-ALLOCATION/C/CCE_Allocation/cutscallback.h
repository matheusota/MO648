#ifndef CUTSCALLBACK_H
#define CUTSCALLBACK_H
#include <gurobi_c++.h>
#include <utility>
#include <map>
#include "user.h"
#include <algorithm>

using namespace std;

class CutsCallback: public GRBCallback{
    private:
        map<int, map<int, GRBVar>> &x;
        vector<User> &users;
        int R;
        double (GRBCallback::*solution_value)(GRBVar);

    public:
        CutsCallback(vector<User> &users, int R, map<int, map<int, GRBVar>> &x);
        void callback();
};

#endif // CUTSCALLBACK_H
