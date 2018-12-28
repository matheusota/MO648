#ifndef PARAMS_H
#define PARAMS_H
#include <string>

class Params
{
    public:
        string alg;
        int numberSubframes;
        int numberUsersLB;
        int numberUsersUB;
        int R;
        int simulations;
        string outputFile;
        string metricType;
        int objFunc;
};

#endif // PARAMS_H
