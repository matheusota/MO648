#ifndef USER_H
#define USER_H
#include <vector>
#include <iostream>

using namespace std;

class User
{
    public:
        int size;
        int id;
        int originalId;
        double price;
        int allocatedIt;
        vector<int> begins;

        User();
        User(int userId);
        void printUser();
};

#endif // USER_H
