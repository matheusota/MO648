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

void Simulator::simulate(Params &params, int numberUsers, void (*scheduler)(vector<User>&, int, int, Measures&)){
    // set seed
    int seed = 0;

    // create an array of UEs
    vector<User> users(numberUsers);
    for(int i = 0; i < numberUsers; i++){
        users[i].originalId = i + 1;
        users[i].price = 1;
    }

    // set the pdcch formats
    setAggregationLevel(numberUsers, users, 0.40, 0.25, 0.20, 0.15);

    // set the begins
    setBegins(users, params.R);

    // now we generate the subframes
    for (nSubframe = 0 ; nSubframe < params.numberSubframes ; nSubframe++) {
        cout << "Subframe " << nSubframe << endl;

        auto started = chrono::high_resolution_clock::now();

        // shuffle the UEs
        shuffle(users.begin(), users.end(), default_random_engine(seed));

        // assign ids
        for(int i = 0; i < numberUsers; i++)
            users[i].id = i + 1;

        Measures measures(numberUsers, params.R);
        scheduler(users, params.R, numberUsers, measures);

        auto done = chrono::high_resolution_clock::now();

        // print measures
        measures.computeBlockedUsers();

        cout << "\t Time: " << chrono::duration_cast<chrono::microseconds>(done-started).count() << " us" << endl;
        cout << "\t Solution [";
        for(auto id : measures.solution)
            cout << id << ", ";
        cout << "]" << endl;
        cout << "\t Blocked Users: " << measures.getBlockedUsers() << endl;

        // write to file
        generateOutput(params, numberUsers, measures);
    }
}
