#include "filereader.h"

void FileReader::split1(const std::string& str, vector<string> &cont){
    std::istringstream iss(str);
    std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(cont));
}

FileReader::FileReader(int numberSubframes, int R){
    string filename = "input/entrada-100-" + to_string(R) + "-" + to_string(numberSubframes) + ".txt";
    ifile.open(filename.c_str());
    if (!ifile) {
        cerr << "File '" << filename << "' do not exist.\n";
        exit(0);
    }
}

vector<vector<User>> FileReader::parseFile(){
    string line;
    int userCount;
    vector<User> users;
    vector<vector<User>> subframes;

    while(getline(ifile, line)){
        // remove dash (-) and split the string
        line.erase(remove(line.begin(), line.end(), '-'), line.end());
        vector<string> words;
        split1(line, words);

        // if first word is subframe, we should start a new subframe
        if(words[0].compare("Subframe") == 0){
            if(users.size() > 0){
                vector<User> copy(users);
                subframes.push_back(copy);
                users.clear();
            }
            userCount = 1;
        }
        else{
            int userId = stoi(words[0]);
            int size = -1;
            int begin;
            User user(userId);
            for(auto word : words){
                if(word.compare("[") == 0){
                    size = 0;
                    begin = -1;
                }
                else if(word.compare("]") == 0){
                    if(user.size == -1)
                        user.size = size;

                    user.begins.push_back(begin);
                }
                else {
                    if(begin == -1)
                        begin = stoi(word);

                    size++;
                }
            }
            user.id = userCount;
            user.price = 1;
            //user.price = (1.0 / userCount) * size;
            //sort(user.begins.begin(), user.begins.end());
            //user.price = (1.0/userCount) * user.size;
            userCount++;
            users.push_back(user);
        }
    }
    subframes.push_back(users);

    return subframes;
}


