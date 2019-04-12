#ifndef FILEREADER_H
#define FILEREADER_H
#import "user.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cmath>

using namespace std;

class FileReader
{
    private:
        ifstream ifile;
        void split1(const std::string& str, vector<string> &cont);

    public:
        FileReader(int numberSubframes, int R);
        vector<vector<User>> parseFile();
};

#endif // FILEREADER_H
