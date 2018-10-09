#ifndef FILEREADER_H
#define FILEREADER_H
#import "user.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

template <class Container>

class FileReader
{
    private:
        ifstream ifile;
        void split1(const std::string& str, Container& cont);

    public:
        FileReader(string filename);
        vector<vector<User>> parseFile();
};

#endif // FILEREADER_H
