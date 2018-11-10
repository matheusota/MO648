#include "MTRand.h"
#include "BRKGA.h"
#include "CTPBasic.hpp"

using namespace std;

class BRKGADecoder {
public:
    virtual double decode(const std::vector< double >& chromosome) const = 0;
};


class Decoder : public BRKGADecoder{
private:
    std::pair<double, std::vector<int>> decodeWithChromosome (const std::vector< double >& chromosome) const;
public:
    Decoder(){};
    ~Decoder(){};
    double decode(const std::vector< double >& chromosome) const {
        return 1;
    }
    
private:
};


class CCEAllocation : public CTPBasic {
    // size of population
    const unsigned p = 100;
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
    
    
public:
    CCEAllocation() {};
    
    void run() {

        const long unsigned rngSeed = 0;    // seed to the random number generator
        MTRand rng(rngSeed);                // initialize the random number generator
        
        Decoder *decoder = new Decoder();
        
        BRKGA< BRKGADecoder, MTRand > algorithm(40, p, pe, pm, rhoe, *decoder, rng, K, MAXT);
        double best_solution = std::numeric_limits<double>::max();
        
        long int run = 0;
        
        startTimer();
        do {
            algorithm.evolve();    // evolve the population for one generation
            
            if((++generation) % X_INTVL == 0) {
                algorithm.exchangeElite(X_NUMBER);    // exchange top individuals
            }
            
            if(algorithm.getBestFitness() < best_solution){
                best_solution = algorithm.getBestFitness();
//                fflush(stdout);
            }
            run++;
        } while ( run < 2);
        printf("New best result : %lf - microseconds from start %lf \n", best_solution, microsecondsFromStart());

    }
    
};
