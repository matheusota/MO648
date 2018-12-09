#ifndef DECODER_H
#define DECODER_H
#include <utility>
#include <algorithm>
#include <vector>
#include "segtree.h"
#include "user.h"
#include "measures.h"

using namespace std;

class Decoder
{
    private:
        int R;
        int numberUsers;
        vector<User> users;

    public:
        Decoder(vector<User> users, int numberUsers, int R);
        double decode(const vector<double> &chromosome) const;
        vector<int> convertToSolution(const vector<double> &chromosome) const;
};

#endif // DECODER_H
