#include "main.h"

using namespace std;

int main(int argc, char *argv[])
{
    // get information from argv and argc
    Params params;
    readCheckParams(params, argc, argv);

    // read input from file
    if(!params.shouldSimulate){
        // read file and put subframes in a vector
        FileReader fileReader(params.numberSubframes, params.R);
        vector<vector<User>> subframes = fileReader.parseFile();

        // execute the chosen algorithm for each subframe
        int count = 0;
        for(auto users : subframes){
            cout << "Subframe " << count << endl;
            vector<int> solution;
            auto started = chrono::high_resolution_clock::now();

            if(params.alg.compare("heuristic") == 0)
                solution = Heuristic::execute(users, params.R, 50);

            else if(params.alg.compare("brkga") == 0)
                solution = BRKGAHeuristic::execute(users, params.R, 50);

            else if(params.alg.compare("exact") == 0)
                solution = SchedulingModel::execute(users, params.R, 50);

            else if(params.alg.compare("besteffort") == 0)
                solution = BestEffort::execute(users, params.R, 50);

            auto done = chrono::high_resolution_clock::now();

            Measures measures;
            int filledPositions = measures.getFilledPositions(solution);
            double blocked = measures.getBlockedUsers(solution, params.R, filledPositions, 50);

            cout << "\t Time: " << chrono::duration_cast<chrono::microseconds>(done-started).count() << " us" << endl;
            cout << "\t Solution [";
            for(auto id : solution)
                cout << id << ", ";
            cout << "]" << endl;
            cout << "\t Blocked Users: " << blocked << endl;

            count++;
        }
    }
    // run simulator to generate input
    else{
        if(params.alg.compare("heuristic") == 0)
            Simulator::simulate(params.numberUsers, params.numberSubframes, params.R, Heuristic::execute);
        else if(params.alg.compare("brkga") == 0)
            Simulator::simulate(params.numberUsers, params.numberSubframes, params.R, BRKGAHeuristic::execute);
        else if(params.alg.compare("exact") == 0)
            Simulator::simulate(params.numberUsers, params.numberSubframes, params.R, SchedulingModel::execute);
        else if(params.alg.compare("besteffort") == 0)
            Simulator::simulate(params.numberUsers, params.numberSubframes, params.R, BestEffort::execute);
    }

    return 0;
}


void readCheckParams(Params &params, int argc, char *argv[])
{
    params.alg = "";
    params.numberSubframes = 0;
    params.R = 0;
    params.shouldSimulate = false;

    // Read
    for(int i = 1; i < argc; i++){
        const string arg(argv[i]);
        string next;
        if((i+1) < argc){
            next = string(argv[i+1]);
        }
        else{
            next = string("");
        }

        if(arg.find("-n") == 0 && next.size() > 0){
            params.numberSubframes = atoi(next.c_str());
            i++;
            continue;
        }

        if(arg.find("-u") == 0 && next.size() > 0){
            params.numberUsers = atoi(next.c_str());
            i++;
            continue;
        }

        if(arg.find("-r") == 0 && next.size() > 0){
            params.R = atoi(next.c_str());
            i++;
            continue;
        }

        else if(arg.find("-s") == 0){
            params.shouldSimulate = true;
            continue;
        }

        else if(arg.find("-a") == 0){
            params.alg = next;
            i++;
            continue;
        }

        cerr << "Parametro invalido: \"" << arg << "\"" << " (ou parametro faltando)" << endl;
        exit(1);
    }

    // Check
    if(params.alg.compare("") == 0){
        cerr << "Deve ser especificado o algoritmo (h ou e)" << endl;
        exit(1);
    }
    if(params.numberSubframes == 0){
        cerr << "Deve ser especificado a quantidade de subframes" << endl;
        exit(1);
    }
    if(params.R == 0){
        cerr << "Deve ser especificado o tamanho do vetor de CCEs" << endl;
        exit(1);
    }
}
