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
        static constexpr unsigned p = 20;
        // fraction of population to be the elite-set
        static constexpr double pe = 0.20;
        // fraction of population to be replaced by mutants
        static constexpr double pm = 0.15;
        // probability that offspring inherit an allele from elite parent
        static constexpr double rhoe = 0.70;
        // number of independent populations
        static constexpr unsigned K = 4;
        // number of threads for parallel decoding
        static constexpr unsigned MAXT = 6;
        // exchange best individuals at every 10 generations
        static constexpr unsigned X_INTVL = 10;
        // exchange top 4 best
        static constexpr unsigned X_NUMBER = 4;
        // max generations to run for 100 gens
        static constexpr unsigned MAX_GENS = 100;
        // seed for rng
        static constexpr long unsigned rngSeed = 0;

    public:
        // current generation
        static unsigned generation;

        static vector<int> execute(vector<User> users, int R, int numberUsers);
};

#endif // BRKGAHEURISTIC_H
