#include "main.h"

using namespace std;

int main(int argc, char *argv[])
{
    // get information from argv and argc
    Params params;
    readCheckParams(params, argc, argv);

    // read input from file
    if(!params.shouldSimulate){
        for(int numberUsers = params.numberUsersLB; numberUsers <= params.numberUsersUB; numberUsers += 10){
            Measures measures(numberUsers, params.R);

            // read file and put subframes in a vector
            FileReader fileReader(params.numberSubframes, params.R);
            vector<vector<User>> subframes = fileReader.parseFile();

            int count = 0;
            for(auto users : subframes){
                cout << "Subframe " << count << endl;
                auto started = chrono::high_resolution_clock::now();

                // execute the chosen algorithm for the subframe
                if(params.alg.compare("heuristic") == 0)
                    Heuristic::execute(users, params.R, numberUsers, measures);

                else if(params.alg.compare("brkga") == 0)
                    BRKGAHeuristic::execute(users, params.R, numberUsers, measures);

                else if(params.alg.compare("exact") == 0)
                    SchedulingModel::execute(users, params.R, numberUsers, measures);

                else if(params.alg.compare("besteffort") == 0)
                    BestEffort::execute(users, params.R, numberUsers, measures);

                else if(params.alg.compare("bruteforce") == 0)
                    BruteForce::execute(users, params.R, numberUsers, measures);

                auto done = chrono::high_resolution_clock::now();

                // get solution data
                measures.computeBlockedUsers();

                /*
                cout << "\t Time: " << chrono::duration_cast<chrono::microseconds>(done-started).count() << " us" << endl;
                cout << "\t Solution [";
                for(auto id : measures.solution)
                    cout << id << ", ";
                cout << "]" << endl;
                cout << "\t Blocked Users: " << measures.getBlockedUsers() << endl;
                */

                // write to file
                generateOutput(params, numberUsers, measures);
                count++;
            }
        }
    }
    // run simulator to generate input
    else{
        for(int numberUsers = params.numberUsersLB; numberUsers <= params.numberUsersUB; numberUsers += 10){
            // simulate and run the algorithms
            if(params.alg.compare("heuristic") == 0)
                Simulator::simulate(params, numberUsers, Heuristic::execute);
            else if(params.alg.compare("brkga") == 0)
                Simulator::simulate(params, numberUsers, BRKGAHeuristic::execute);
            else if(params.alg.compare("exact") == 0)
                Simulator::simulate(params, numberUsers, SchedulingModel::execute);
            else if(params.alg.compare("besteffort") == 0)
                Simulator::simulate(params, numberUsers, BestEffort::execute);
            else if(params.alg.compare("bruteforce") == 0)
                Simulator::simulate(params, numberUsers, BruteForce::execute);
        }
    }

    return 0;
}


// read parameters from terminal
void readCheckParams(Params &params, int argc, char *argv[])
{
    params.alg = "";
    params.numberSubframes = 0;
    params.R = 0;
    params.shouldSimulate = false;
    params.outputFile = "";
    params.numberUsersLB = 0;
    params.numberUsersUB = 0;

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

        if(arg.compare("-n") == 0 && next.size() > 0){
            params.numberSubframes = atoi(next.c_str());
            i++;
            continue;
        }

        if(arg.compare("-u") == 0 && next.size() > 0){
            params.numberUsersLB = atoi(next.c_str());
            params.numberUsersUB = params.numberUsersLB;
            i++;
            continue;
        }

        if(arg.compare("-ulb") == 0 && next.size() > 0){
            params.numberUsersLB = atoi(next.c_str());
            i++;
            continue;
        }

        if(arg.compare("-uub") == 0 && next.size() > 0){
            params.numberUsersUB = atoi(next.c_str());
            i++;
            continue;
        }

        if(arg.compare("-r") == 0 && next.size() > 0){
            params.R = atoi(next.c_str());
            i++;
            continue;
        }

        else if(arg.compare("-s") == 0){
            params.shouldSimulate = true;
            continue;
        }

        else if(arg.compare("-a") == 0){
            params.alg = next;
            i++;
            continue;
        }

        else if(arg.compare("-o") == 0){
            params.outputFile = next;
            i++;
            continue;
        }

        cerr << "Parametro invalido: \"" << arg << "\"" << " (ou parametro faltando)" << endl;
        exit(1);
    }

    // Check
    if(params.alg.compare("") == 0){
        cerr << "Deve ser especificado o algoritmo" << endl;
        exit(1);
    }
    if(params.numberSubframes == 0){
        cerr << "Deve ser especificado a quantidade de subframes (-n X)" << endl;
        exit(1);
    }
    if(params.R == 0){
        cerr << "Deve ser especificado o tamanho do vetor de CCEs (-r X)" << endl;
        exit(1);
    }
    if(params.numberUsersLB == 0){
        cerr << "Deve ser especificado o número de usuários por subframe (-u X)" << endl;
        exit(1);
    }
}

// creates an output file for the acquired measures
void generateOutput(Params &params, int numberUsers, Measures &measures){
    if(params.outputFile.compare("") != 0){
        ofstream data;
        data.open ("output/" + params.outputFile, std::ios_base::app);
        data << params.alg << " ; " << numberUsers << " ; " << measures.getBlockedRate() << endl;
        data.close();
    }
}
