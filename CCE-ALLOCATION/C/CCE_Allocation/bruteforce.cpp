#include "bruteforce.h"

int BruteForce::R;
int BruteForce::numberUsers;
double BruteForce::bestCost;
vector<int> BruteForce::bestSol;

double BruteForce::bruteForceScheduler(vector<User> users, SegTree &solutionTree, int slotsFilled){
    if(slotsFilled == R){
        // build a solution array from segtree
        vector<int> solution(R);
        fill(solution.begin(), solution.end(), 0);

        for(int i = 0; i < R; i++)
            solution[i] = solutionTree.getLeaf(i);

        // get number of blocked users in this solution
        Measures measures(BruteForce::numberUsers, BruteForce::R, solution);
        measures.computeBlockedUsers();

        // if this solution is better, update
        if(measures.getBlockedUsers() < BruteForce::bestCost){
            for(int i = 0; i < R; i++)
                BruteForce::bestSol[i] = solution[i];

            BruteForce::bestCost = measures.getBlockedUsers();
        }
    }
    else {
        // this is the main brute force loop, for each user, we try to allocate it and recurse
        for(auto u: users){
            if(u.allocatedIt == 0){
                for(auto b: u.begins){
                    if(solutionTree.query(b, b + u.size) == 0){
                        // allocate the user
                        for(int j = b; j < b + u.size; j++)
                            solutionTree.update(j, u.id);
                        u.allocatedIt = 1;

                        // solve recursively
                        BruteForce::bruteForceScheduler(users, solutionTree, slotsFilled + u.size);

                        // remove user
                        for(int j = b; j < b + u.size; j++)
                            solutionTree.update(j, 0);
                        u.allocatedIt = 0;
                    }
                }
            }
        }
    }
}

void BruteForce::execute(vector<User> &users, int R, int numberUsers, Measures &measures){
    vector<User> users2(users.begin(), users.begin() + numberUsers);

    // copy parameters to class attributes
    BruteForce::R = R;
    BruteForce::bestCost = numeric_limits<double>::max();
    BruteForce::numberUsers = numberUsers;

    // initialize best solution
    for(int i = 0; i < R; i++)
        BruteForce::bestSol.push_back(0);

    // build a segtree for solution
    SegTree solutionTree(BruteForce::bestSol);

    // run brute force algorithm
    BruteForce::bruteForceScheduler(users2, solutionTree, 0);

    // set found solution measures
    measures.setSolution(BruteForce::bestSol);
}
