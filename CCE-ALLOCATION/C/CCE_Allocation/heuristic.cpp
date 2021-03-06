#include "heuristic.h"
vector<int> Heuristic::finalSolution;
int Heuristic::best;
int Heuristic::numberTries;

// this is the main routine, which will apply randomization when trying to allocate a user
bool Heuristic::tryToAllocate(User &user, SegTree &solutionTree, SegTree &counterTree){
    vector<pair<double, int>> beginsCount;
    double sum = 0;

    // we sum the counters for each position where the user can be allocated
    for(auto b: user.begins){
        if(solutionTree.query(b, b + user.size) == 0){
            double c = counterTree.query(b, b + user.size);
            sum += 1.0 / c; // the bigger c, the smaller the chance of being allocated at it
            beginsCount.push_back(make_pair(1.0 / c, b));
        }
    }

    // user cannot be allocated
    if(beginsCount.size() == 0)
        return false;

    // get a random number between 0 and 1
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    double p = dis(gen);

    // we will use the beginsCount.first for the probabilistic distribution
    for(int i = 0; i < beginsCount.size(); i++){
        if(i > 0){
            beginsCount[i].first = beginsCount[i - 1].first + (beginsCount[i].first / sum);
        }
        else{
            beginsCount[i].first = beginsCount[i].first / sum;
        }
    }

    for(auto x : beginsCount){
        if(p <= x.first){
            // allocate the user
            for(int j = x.second; j < x.second + user.size; j++){
                solutionTree.update(j, user.id);
                counterTree.update(j, counterTree.getLeaf(j) - 1);
            }

            return true;
        }
    }

    return false;
}


// this runs once the heuristic
void Heuristic::runHeuristic(vector<User> users, int R, int numberUsers, unsigned seed){
    vector<int> counter(R);
    vector<int> solution(R);
    vector<User> users2(users.begin(), users.begin() + numberUsers);
    int aux_i;
    bool solved = false;

    // initialize vectors
    fill(solution.begin(), solution.end(), 0);

    //build a segtree for solution
    SegTree solutionTree(solution);

    int slotsFilled = 0;
    for(int i = 0; i < users2.size() && !solved; i++){
        // zero the counter
        fill(counter.begin(), counter.end(), 0);

        // recalculate the counter for the current chunk
        for(int i2 = i; i2 < i + 5; i2++){
            for(auto b : users2[i2].begins){
                for(int j = b; j < b + users2[i2].size; j++)
                    counter[j]++;
            }
        }

        // create a segtree for the counter
        SegTree counterTree(counter);

        // now we shuffle a [0...4] vector to simulate shuffling the users
        array<int,5> auxRange {0,1,2,3,4};
        shuffle(auxRange.begin(), auxRange.end(), default_random_engine(seed));

        // allocate the shuffled chunk
        aux_i = i;
        for(int k = 0; k < 5; k++){
            //cout << "examining " << users2[aux_i + auxRange[k]].id << endl;
            if(Heuristic::tryToAllocate(users2[aux_i + auxRange[k]], solutionTree, counterTree)){
                slotsFilled += users2[aux_i + auxRange[k]].size;

                if(slotsFilled == R){
                    solved = true;
                    break;
                }
            }

            i++;
        }

        i--;
    }

    // build solution from segtree
    for(int i = 0; i < R; i++)
        solution[i] = solutionTree.getLeaf(i);

    // get measures
    Measures measures(numberUsers, R, solution);
    measures.computeBlockedUsers();

    // update the solution, if necessary
    #pragma omp critical
    {
        if(measures.getBlockedUsers() < Heuristic::best){
            Heuristic::best = measures.getBlockedUsers();
            Heuristic::finalSolution = measures.solution;
        }
    }
}

// this will run the heuristic 100 times and return the best answer
void Heuristic::execute(vector<User> &users, int R, int numberUsers, Measures &measures){
    Heuristic::best = 99999;
    vector<User> users2(users.begin(), users.end());

    #pragma omp parallel for firstprivate(users2, R, numberUsers) num_threads(10)
    for(int i = 0; i < numberTries; i++){
        Heuristic::runHeuristic(users2, R, numberUsers, i);
    }

    measures.setSolution(Heuristic::finalSolution);
}
