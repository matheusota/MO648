#include "measures.h"

Measures::Measures(){}

int Measures::getFilledPositions(vector<int> &solution){
    int count = 0;

    for(auto s : solution){
        if(s != 0)
            count++;
    }

    return count;
}

int Measures::getBlockedUsers(vector<int> &solution, int R, int filledPositions, int numberUsers){
    // convert to a set and sort
    vector<int> solution2;
    unordered_set<int> S;
    for (int x : solution){
        if(x != 0)
            S.insert(x);
    }
    solution2.assign(S.begin(), S.end());
    sort(solution2.begin(), solution2.end());

    // now we count the blocked users
    int blocked = 0;
    int prevUser = 0;

    for(auto u: solution2){
        blocked += (u - prevUser) - 1;
        prevUser = u;
    }

    // not all positions were filled, we need to add the remaining users to blocked
    if(filledPositions != R)
        blocked += numberUsers - prevUser;

    return blocked;
}
