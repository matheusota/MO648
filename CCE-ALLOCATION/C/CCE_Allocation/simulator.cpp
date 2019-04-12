#include "simulator.h"

map<int, int> Simulator::PDCCHNumberOfCandidates_UESpecific = {{1, 6}, {2, 6}, {4, 2}, {8, 2}};
map<int, int> Simulator::PDCCHNumberOfCandidates_Common = {{1, -1}, {2, -1}, {4, 4}, {8, 2}};
int Simulator::nSubframe = 0;

// set the pdcch formats of the user equipments according to the distribution given by the pbs
void Simulator::setAggregationLevel(int nUes, vector<User> &users, double pbAl1, double pbAl2, double pbAl4, double pbAl8){
    int nUesAl1 = floor(nUes * pbAl1);
    int nUesAl2 = floor(nUes * pbAl2);
    int nUesAl4 = floor(nUes * pbAl4);
    int nUesAl8 = nUes - (nUesAl1 + nUesAl2 + nUesAl4);

    for (int i = 0 ; i < nUes ; i++) {
        if (i < nUesAl1)
            users[i].size = 1;
        else if (i < nUesAl1 + nUesAl2)
            users[i].size = 2;
        else if (i < nUesAl1 + nUesAl2 + nUesAl4)
            users[i].size = 4;
        else if (i < nUesAl1 + nUesAl2 + nUesAl4 + nUesAl8)
            users[i].size = 8;
    }
}

// set the users begins vector
void Simulator::setBegins(vector<User> &users, int nCCEs){
    for(auto &user: users){
        for(int m = 0; m < PDCCHNumberOfCandidates_UESpecific[user.size]; m++)
            user.begins.push_back(calcCCEIndex(m, 0, user, nCCEs));
    }
}

// compute the CCE index
int Simulator::calcCCEIndex(int m, int i, User user, int nCCEs){
    int C = -1;

    int L = user.size;
    int k = nSubframe % 10;
    int N = nCCEs; //m_mac->GetDevice()->GetPhy()->GetBandwidthManager()->GetPDCCHTotalCces();
    int T = floor(N / L);

    C = L * ((Calc_Y(k, user.originalId) + m) % T) + i;

    return C;
}

// compute the Y parameter, used for computing the CCE index
int Simulator::Calc_Y(int k, int RNTI){
  long int y, t;

  /* Common search space */
  if (RNTI == -1)
    return 0;

  /* Dedicate search space */
  if (k < 0) {
    y = RNTI;
  } else {
    t = Calc_Y(k - 1, RNTI);
    y = (39827 * t) % 65537;
  }

  return y;
}

// runs the scheduler until the users vector is empty. This is used for counting how many iterations
// we need to allocate all incoming users
void Simulator::runUntilEmpty(Params &params, vector<User> &users, Measures &measures, void (*scheduler)(vector<User>&, int, int, Measures&)){
    vector<User> usersAux(users);

    while(usersAux.size() > 0){
        // assign ids and price
        for(int i = 0; i < usersAux.size(); i++){
            usersAux[i].id = i + 1;

            if(params.objFunc == 0)
                usersAux[i].price = 1;
            else if(params.objFunc == 1)
                usersAux[i].price = (1.0 / double(i + 1));
            else
                usersAux[i].price = (1.0 / double(i + 1)) * usersAux[i].size;
        }

        // run scheduling algorithm
        scheduler(usersAux, params.R, usersAux.size(), measures);
        measures.incrementIterations();

        // for debugging purposes
        cout << "\nUsers: ";
        for(auto user: usersAux)
            cout << user.originalId << ", ";
        cout << endl;

        cout << "Iteration: " << measures.getIterations() <<  endl;
        cout << "Solution [ ";
        for(auto id : measures.solution){
            if(id != 0)
                cout << "(" << id << "," << usersAux[id - 1].originalId << ") , ";
            else
                cout << "X , ";
        }
        cout << " ]\n" << endl;

        // get allocated users
        set<int> toRemoveUsers;

        for(auto id: measures.solution)
            toRemoveUsers.insert(id);

        // remove allocated users from the users vector
        vector<User> users2;

        for(int i = 0; i < usersAux.size(); i++){
            if(toRemoveUsers.find(i + 1) == toRemoveUsers.end())
                users2.push_back(usersAux[i]);
        }

        usersAux.clear();
        usersAux = users2;
    }
}

void Simulator::runSimulationRound(Params &params, int numberUsers, void (*scheduler)(vector<User>&, int, int, Measures&), int seed){
    // set seed
    srand(seed * 10);

    // tmp
    double totalBlock = 0;
    double totalFill = 0;
    double count = 0;

    Heuristic::numberTries = params.heuristicTries;

    // create an array of UEs
    vector<User> users(numberUsers);
    set<int> allocatedIds;

    for(int i = 0; i < numberUsers; i++){
        int RNTI = rand() % 65523 + 61;
        while(allocatedIds.find(RNTI) != allocatedIds.end())
            RNTI = rand() % 65523 + 61;

        users[i].originalId = RNTI;
    }

    cout << "\n********************************************************************" << endl;
    cout << "********************* New Simulation *******************************" << endl;
    cout << "********************************************************************\n" << endl;
    cout << "Generated Users RNTIs [";
    for(auto user: users)
        cout << user.originalId << ", ";
    cout << "]" << endl;

    // set the pdcch formats
    setAggregationLevel(numberUsers, users, 0.40, 0.25, 0.20, 0.15);

    // set the begins
    setBegins(users, params.R);

    // shuffle the UEs
    random_shuffle(users.begin(), users.end());

    // if we are measuring iterations, we run the algorithm until the input array is empty
    if(params.metricType.compare("its") == 0){
        Measures measures(numberUsers, params.R);
        runUntilEmpty(params, users, measures, scheduler);
        generateOutput(params, numberUsers, measures);
    }
    // else we go on shuffling the users vector and running the desired scheduling algorithm
    else{
        for(nSubframe = 0 ; nSubframe < params.numberSubframes; nSubframe++){
            cout << "********************************************************************" << endl;
            cout << "Subframe " << nSubframe + 1 << endl;

            // shuffle the UEs
            random_shuffle(users.begin(), users.end());

            // assign ids and price
            for(int i = 0; i < numberUsers; i++){
                users[i].id = i + 1;

                if(params.objFunc == 0)
                    users[i].price = 1;
                else if(params.objFunc == 1)
                    users[i].price = (1.0 / (i + 1));
                else
                    users[i].price = (1.0 / (i + 1)) * users[i].size;
            }

            Measures measures(numberUsers, params.R);

            auto started = chrono::high_resolution_clock::now();
            scheduler(users, params.R, numberUsers, measures);
            auto done = chrono::high_resolution_clock::now();

            // print measures
            measures.computeBlockedUsers();

            measures.setTime(chrono::duration_cast<chrono::microseconds>(done-started).count());
            cout << "Solution Details:" << endl;
            cout << "\t Time: " << measures.getTime() << " us" << endl;
            cout << "\t Solution [";
            for(auto id : measures.solution)
                cout << id << ", ";
            cout << "]" << endl;
            cout << "\t Blocked Users: " << measures.getBlockedUsers() << endl;
            totalBlock += measures.getBlockedRate();
            totalFill += measures.getResourceRate();
            count++;

            // write to file
            generateOutput(params, numberUsers, measures);
        }
    }

    /*
    cout << "Total Block Rate: " << totalBlock/count << endl;
    ofstream data;
    data.open ("output/heuristic.csv", std::ios_base::app);
    data << "heuristic ; " << params.heuristicTries << " ; " << totalBlock/totalBlockCount << endl;
    */

    generateOutputAll(params, numberUsers, totalBlock, totalFill, count);
}

void Simulator::simulate(Params &params, int numberUsers, void (*scheduler)(vector<User>&, int, int, Measures&)){
    for(int i = 0; i < params.simulations; i++)
        runSimulationRound(params, numberUsers, scheduler, i);
}
