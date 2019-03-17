#include "cutscallback.h"

CutsCallback::CutsCallback(vector<User> &users, int R, map<int, map<int, GRBVar>> &x): users(users), x(x){
    this->R = R;
}

// callback function
// Implements Van den Akker algorithm
void CutsCallback::callback(){
     // integer solution
    if (where == GRB_CB_MIPSOL){
        solution_value = &CutsCallback::getSolution;
    }
    // fractional solution
    else if ((where == GRB_CB_MIPNODE) && (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL)){
        solution_value = &CutsCallback::getNodeRel;
    }
    else{
        return;
    }

    map<int, int> countSize;
    countSize[1] = 0;
    countSize[2] = 0;
    countSize[4] = 0;
    countSize[8] = 0;
    for(auto user: users){
        countSize[user.size]++;
    }

    for(auto user : users){
        countSize[user.size]--;
        int maxSize = 0;
        for(int s = 8; s >= 1; s /= 2){
            if(countSize[s] > 0){
                maxSize = s;
                break;
            }
        }

        for(int l = user.size - 1; l < R; l++){
            if(find(user.begins.begin(), user.begins.end(), l) != user.begins.end() &&
                ((this ->*solution_value)(x[user.id][l]) >= 0.1) && ((this ->*solution_value)(x[user.id][l]) <= 0.9)){
                for(int u = l + 1; u < l + maxSize; u++){
                    if(find(user.begins.begin(), user.begins.end(), u) != user.begins.end() &&
                        ((this ->*solution_value)(x[user.id][u]) >= 0.1) && ((this ->*solution_value)(x[user.id][u]) <= 0.9)){
                        double sum1 = 0;
                        for(int s = max(0, l - user.size); s <= u; s++){
                            if(find(user.begins.begin(), user.begins.end(), s) != user.begins.end()){
                                sum1 += (this ->*solution_value)(x[user.id][s]);
                            }
                        }

                        double sum2 = 0;
                        for(auto user2 : users){
                            if(user2.id != user.id){
                                for(int s = max(0, u - user2.size); s <= l; s++){
                                    if(find(user2.begins.begin(), user2.begins.end(), s) != user2.begins.end()){
                                        sum2 += (this ->*solution_value)(x[user2.id][s]);
                                    }
                                }
                            }
                        }

                        // violation found, add the cut
                        if(sum1 + sum2 > 1){
                            GRBLinExpr expr;

                            for(int s = max(0, l - user.size); s <= u; s++){
                                if(find(user.begins.begin(), user.begins.end(), s) != user.begins.end()){
                                    expr += x[user.id][s];
                                }
                            }

                            for(auto user2 : users){
                                if(user2.id != user.id){
                                    for(int s = max(0, u - user2.size); s <= l; s++){
                                        if(find(user2.begins.begin(), user2.begins.end(), s) != user2.begins.end()){
                                            expr += x[user2.id][s];
                                        }
                                    }
                                }
                            }

                            addLazy(expr <= 1);
                        }
                    }
                }
            }
        }
    }
}
