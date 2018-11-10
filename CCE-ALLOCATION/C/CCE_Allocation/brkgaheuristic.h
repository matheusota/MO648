#ifndef BRKGAHEURISTIC_H
#define BRKGAHEURISTIC_H
#include "user.h"
#include <utility>
#include <algorithm>
#include <array>
#include <random>
#include "measures.h"
#include "mtrand.h"
#include "brkga.h"
#include "decoder.h"

class BRKGAHeuristic
{
    private:
        // size of population
        const unsigned p = 20;
        // fraction of population to be the elite-set
        const double pe = 0.20;
        // fraction of population to be replaced by mutants
        const double pm = 0.15;
        // probability that offspring inherit an allele from elite parent
        const double rhoe = 0.70;
        // number of independent populations
        const unsigned K = 4;
        // number of threads for parallel decoding
        const unsigned MAXT = 6;
        // current generation
        unsigned generation = 0;
        // exchange best individuals at every 10 generations
        const unsigned X_INTVL = 10;
        // exchange top 4 best
        const unsigned X_NUMBER = 4;
        // max generations to run for 100 gens
        const unsigned MAX_GENS = 100;
        // seed for rng
        const long unsigned rngSeed = 0;

    public:
        BRKGAHeuristic();
        vector<int> execute(vector<User> users, int R, int numberUsers);
};

#endif // BRKGAHEURISTIC_H
