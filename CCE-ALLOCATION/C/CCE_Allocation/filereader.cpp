#include "filereader.h"

void FileReader::split1(const std::string& str, Container& cont)
{
    std::istringstream iss(str);
    std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(cont));
}

FileReader::FileReader(string filename)
{
    ifile.open(filename.c_str());
    if (!ifile) {
        cerr << "File '" << filename << "' do not exist.\n";
        exit(0);
    }
}

vector<vector<User>> FileReader::parseFile(){
    string line;
    int subframe = -1;
    int userCount;
    vector<User> users;
    vector<vector<User>> subframes;

    while(getline(ifile, line)){
        replace(line.begin(), line.end(), '-', '');
        vector<string> words;
        split1(line, words);

        if(words[0].compare("Subframe") == 0){
            if(users.size() > 0){
                vector<User> copy(users);
                subframes.push_back(copy);
                users.clear();
            }
            subframe++;
            userCount = 0;
        }
        else{
            int userId = stoi(words[0]);
            int size = -1;
            int begin;
            User user = new User(userId);
            for(uint i = 1; i < words.size(); i++){
                if(words[i].compare("[") == 0){
                    size = 0;
                    begin = -1;
                }
                else if(words[i].compare("]") == 0){
                    if(user.size == -1)
                        user.size = size;

                    user.begins.insert(begin);
                    size = -1;
                }
                else if(size != -1){
                    if(begin  == -1)
                        begin = stoi(words[i]);

                    size++;
                }
            }
            user.id = userCount;
            user.price = (1.0/userCount) * user.size;
            userCount++;
            users.push_back(user);
        }
    }
    subframes.push_back(users);

    return subframes;
}


