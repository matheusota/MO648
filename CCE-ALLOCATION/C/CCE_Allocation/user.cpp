#include "user.h"

using namespace std;

User::User(){}

User::User(int userId){
    originalId = userId;
    size = -1;
}

void User::printUser(){
    cout << "User " << id << endl;
    cout << "\t Original Id " << originalId << endl;
    cout << "\t Size " << size << endl;

    cout << "\t Begins ";
    for(auto b : begins){
        cout << b << " ";
    }
    cout << endl;
}
