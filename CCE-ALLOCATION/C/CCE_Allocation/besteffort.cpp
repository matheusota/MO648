#include "besteffort.h"

// try to allocate a user
bool BestEffort::tryToAllocate(User &user, SegTree &solutionTree){
    // search for a begin position where the user can be allocated
    for(auto b: user.begins){
        if(solutionTree.query(b, b + user.size) == 0){
            // allocate the user
            for(int j = b; j < b + user.size; j++)
                solutionTree.update(j, user.id);

            return true;
        }
    }

    return false;
}

void BestEffort::execute(vector<User> &users, int R, int numberUsers, Measures &measures){
    vector<int> solution(R);
    vector<User> users2(users.begin(), users.begin() + numberUsers);
    bool solved = false;

    // initialize solution vector
    fill(solution.begin(), solution.end(), 0);

    //build a segtree for solution
    SegTree solutionTree(solution);

    // main loop, just tries to allocate each user
    int slotsFilled = 0;
    for(int i = 0; i < users2.size() && !solved; i++){
        if(BestEffort::tryToAllocate(users2[i], solutionTree)){
            slotsFilled += users2[i].size;

            if(slotsFilled == R){
                solved = true;
                break;
            }
        }
    }

    // build solution from segtree
    for(int i = 0; i < R; i++)
        solution[i] = solutionTree.getLeaf(i);
    measures.setSolution(solution);
}
