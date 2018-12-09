#include "measures.h"

Measures::Measures(int numberOfUsers, int R){
    this->numberOfUsers = numberOfUsers;
    this->R = R;
}

Measures::Measures(int numberOfUsers, int R, vector<int> &solution){
    this->numberOfUsers = numberOfUsers;
    this->R = R;
    this->solution = solution;
}

void Measures::setSolution(vector<int> &solution){
    this->solution = solution;
}

// count number of filled positions and then use this to compute number of blocked users
void Measures::computeBlockedUsers(){
    setFilledPositions();
    setBlockedUsers();
}

// count the number of position filled
void Measures::setFilledPositions(){
    int count = 0;

    for(auto s : solution){
        if(s != 0)
            count++;
    }

    filledPositions = count;
}

// count number of blocked users
void Measures::setBlockedUsers(){
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
        blocked += numberOfUsers - prevUser;

    blockedUsers = blocked;
}

double Measures::getBlockedRate(){
    return ((double) blockedUsers)/((double) numberOfUsers);
}

double Measures::getResourceRate(){
    return ((double) filledPositions)/((double) R);
}

int Measures::getBlockedUsers(){
    return blockedUsers;
}

int Measures::getFilledPositions(){
    return filledPositions;
}
