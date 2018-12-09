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
        bool shouldSimulate;
        string outputFile;
};

#endif // PARAMS_H
