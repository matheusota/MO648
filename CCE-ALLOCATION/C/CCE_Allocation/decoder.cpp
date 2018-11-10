#include "decoder.h"

Decoder::Decoder(vector<User> users, int R){
    this->users = users;
    this->R = R;
}

vector<int> Decoder::convertToSolution(const vector<double> &chromosome) const{
    // first we sort the genes
    vector<pair<int, int>> tmp;

    for(int i = 0; i < chromosome.size(); i++)
        tmp.push_back(make_pair(chromosome[i], i));

    sort(tmp.begin(), tmp.end());

    vector<int> solution(this->R);
    SegTree solutionTree(solution);

    // now we allocate the users according to that order
    for(auto x : tmp){
        for(auto b: users[x.second].begins){
            // check if its possible to allocate the user
            if(solutionTree.query(b, b + users[x.second].size) == 0){

                // allocate the users
                for(int j = b; j < b + users[x.second].size; j++)
                    solutionTree.update(j, users[x.second].id);
            }
        }
    }

    // build solution from segtree
    for(int i = 0; i < R; i++)
        solution[i] = solutionTree.getLeaf(i);

    return solution;
}

double Decoder::decode(const vector<double> &chromosome) const{
    vector<int> solution = this->convertToSolution(chromosome);

    // get number of blocked users
    Measures measures;
    int filledPositions = measures.getFilledPositions(solution);
    double blocked = measures.getBlockedUsers(solution, R, filledPositions, 50);

    return blocked;

    return 1;
}
