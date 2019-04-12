#include "reshuffle.h"

// try to allocate a user
bool Reshuffle::tryToAllocate(User &user, vector<int> &solution){
    // search for a begin position where the user can be allocated
    for(auto b: user.begins){
        bool canAllocate = true;

        // check if user can be allocated
        for(int j = b; j < b + user.size; j++){
            if(solution[j] != 0){
                canAllocate = false;
                break;
            }
        }

        // allocate the user
        if(canAllocate){
            for(int j = b; j < b + user.size; j++)
                solution[j] = user.id;

            return true;
        }
    }

    return false;
}

void Reshuffle::execute(vector<User> &users2, int R, int numberUsers, Measures &measures){
    vector<int> solution(R);
    vector<User> users(users2.begin(), users2.begin() + numberUsers);
    bool solved = false;

    for(auto user: users){
        cout << "user " << user.id << endl;
        cout << "size " << user.size << endl;
    }

    // initialize solution vector
    fill(solution.begin(), solution.end(), 0);

    // main loop of the reshuffling algorithm
    int slotsFilled = 0;
    for(int i = 0; i < users.size() && !solved; i++){
        // if this user can be allocated, just allocate it
        if(Reshuffle::tryToAllocate(users[i], solution)){
            slotsFilled += users[i].size;

            if(slotsFilled == R){
                solved = true;
                break;
            }
        }

        // otherwise, for each candidate position, we try the "reshuffling"
        else{
            for(int k = 0; k < users[i].begins.size(); k++){
                vector<int> markedUsersIds;
                vector<int> tmp(solution);

                /*
                 * this is actually better than the paper!
                // get users blocking this candidate
                for(int j = users[i].begins[k]; j < users[i].begins[k] + users[i].size; j++){
                    if(tmp[j] != 0){
                        // traverse to the left and to the right, removing this user
                        int removedId = tmp[j];

                        int p = j;
                        while(p >= 0 && tmp[p] == removedId){
                            tmp[p] = 0;
                            p--;
                        }

                        p = j + 1;
                        while(p < tmp.size() && tmp[p] == removedId){
                            tmp[p] = 0;
                            p++;
                        }

                        // add removed user to marked users
                        markedUsersIds.push_back(removedId);
                    }
                }

                // fill this candidate on tmp vector
                for(int j = users[i].begins[k]; j < users[i].begins[k] + users[i].size; j++)
                    tmp[j] = users[i].id;

                // try to allocate each marked user
                bool success = true;
                for(auto id: markedUsersIds){
                    if(!Reshuffle::tryToAllocate(users[id - 1], tmp)){
                        success = false;
                        break;
                    }
                }

                // reshuffling was successful, update solution and keep going with other users
                if(success){
                    solution = tmp;
                    slotsFilled += users[i].size;
                    if(slotsFilled == R)
                        solved = true;

                    break;
                }
                */
                bool success = true;

                for(int j = users[i].begins[k]; j < users[i].begins[k] + users[i].size; j++){
                    // user j is blocking curr user
                    if(tmp[j] != 0){
                        int removedId = tmp[j];

                        // traverse to the left and to the right, removing this user
                        int start = 0;
                        int end = 0;
                        int p = j;
                        while(p >= 0 && tmp[p] == removedId){
                            tmp[p] = -1;
                            p--;
                        }
                        start = p + 1;

                        p = j + 1;
                        while(p < tmp.size() && tmp[p] == removedId){
                            tmp[p] = -1;
                            p++;
                        }
                        end = p - 1;

                        // try to allocate user j somewhere else
                        if(!Reshuffle::tryToAllocate(users[removedId - 1], tmp)){
                            success = false;
                            break;
                        }
                        else{
                            for(int p = start; p <= end; p++){
                                tmp[p] = 0;
                            }
                        }
                    }
                }

                // reshuffling was successful, update solution and keep going with other users
                if(success){
                    // fill this candidate on tmp vector
                    for(int j = users[i].begins[k]; j < users[i].begins[k] + users[i].size; j++)
                        tmp[j] = users[i].id;

                    // reshuffling was successful, update solution and keep going with other users
                    solution = tmp;
                    slotsFilled += users[i].size;
                    if(slotsFilled == R)
                        solved = true;

                    break;
                }
            }
        }
    }

    measures.setSolution(solution);
}
