#include "heuristic.h"
vector<int> Heuristic::finalSolution;
int Heuristic::best;

// this is the main routine, which will apply randomization when trying to allocate a user
bool Heuristic::tryToAllocate(User &user, SegTree &solutionTree, SegTree &counterTree){
    vector<pair<double, int>> beginsCount;
    double sum = 0;

    // we sum the counters for each position where the user can be allocated
    for(auto b: user.begins){
        if(solutionTree.query(b, b + user.size) == 0){
            double c = counterTree.query(b, b + user.size);
            sum += c;
            beginsCount.push_back(make_pair(c, b));
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

    for(auto x : beginsCount){
        if(p <= x.first / sum){
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
    Measures measures;
    int filledPositions = measures.getFilledPositions(solution);
    int blocked = measures.getBlockedUsers(solution, R, filledPositions, numberUsers);

    // update the solution, if necessary
    #pragma omp critical
    {
        if(blocked < Heuristic::best){
            Heuristic::best = blocked;
            Heuristic::finalSolution = solution;
        }
    }
}

// this will run the heuristic 100 times and return the best answer
vector<int> Heuristic::execute(vector<User> users, int R, int numberUsers){
    Heuristic::best = 99999;

    #pragma omp parallel for firstprivate(users, R, numberUsers) num_threads(10)
    for(int i = 0; i < 100; i++){
        Heuristic::runHeuristic(users, R, numberUsers, i);
    }

    return Heuristic::finalSolution;
}

