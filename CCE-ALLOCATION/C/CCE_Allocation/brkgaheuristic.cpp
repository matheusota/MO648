#include "brkgaheuristic.h"

unsigned BRKGAHeuristic::generation = 0;

vector<int> BRKGAHeuristic::execute(vector<User> users, int R, int numberUsers){
    // initialize the random number generator
    MTRand rng(rngSeed);

    Decoder *decoder = new Decoder(users, R);

    BRKGA<Decoder, MTRand> algorithm(50, p, pe, pm, rhoe, *decoder, rng, K, MAXT);
    double best_solution = numeric_limits<double>::max();

    long int run = 0;

    do {
        // evolve the population for one generation
        algorithm.evolve();

        // exchange top individuals
        if((++generation) % X_INTVL == 0)
            algorithm.exchangeElite(X_NUMBER);

        // update solution
        if(algorithm.getBestFitness() < best_solution)
            best_solution = algorithm.getBestFitness();

        run++;
    } while(run < 100);

    // convert best chromossome and return
    const vector<double> bestChromossome = algorithm.getBestChromosome();
    return decoder->convertToSolution(bestChromossome);
}
