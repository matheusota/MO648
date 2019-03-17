#include "sppmodel.h"

void SPPModel::execute(vector<User> &users2, int R, int numberUsers, Measures &measures){
    // copy users
    vector<User> users(users2.begin(), users2.begin() + numberUsers);

    // gurobi stuff
    map<int, map<int, GRBVar>> x;
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.set(GRB_StringAttr_ModelName, "CCE_SPP"); // gives a name to the problem
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE); // says that lp is a minimization problem
    model.set(GRB_DoubleParam_MIPGap, 0);
    model.set(GRB_IntParam_Cuts, 0);
    //model.getEnv().set(GRB_IntParam_OutputFlag, 0);

    // solution variable
    vector<int> solution(R);
    fill(solution.begin(), solution.end(), 0);

    // initialize gurobi variables
    for(auto user: users){
        for(int k = 0; k < user.begins.size(); k++)
            x[user.id][k] = model.addVar(0.0, 1.0, user.price, GRB_BINARY, "x[" + to_string(user.id) + "][" + to_string(k) + "]");
    }
    model.update();

    // add "user can only be allocated once" constraint
    for(auto user: users){
        GRBLinExpr expr = 0;

        for(int k = 0; k < user.begins.size(); k++)
            expr += x[user.id][k];

        model.addConstr(expr <= 1);
    }

    // add "no overlap" constraint
    for(int j = 0; j < R; j++){
        GRBLinExpr expr = 0;

        for(auto user: users){
            for(int k = 0; k < user.begins.size(); k++){
                if(k < user.begins.size() && (user.begins[k] <= j && j < user.begins[k] + user.size))
                    expr += x[user.id][k];
            }
        }

        model.addConstr(expr <= 1);
    }

    model.update();
    try {
        // now we solve it
        model.optimize();
        model.update();

        cout << "Solution Cost: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

        // create a vector from the solution
        for(auto user: users){
            for(int k = 0; k < user.begins.size(); k++){
                //cout << x[user.id][j].get(GRB_StringAttr_VarName) << " = " << x[user.id][j].get(GRB_DoubleAttr_X) << endl;

                if(x[user.id][k].get(GRB_DoubleAttr_X) > 0.5){
                    for(int j = user.begins[k]; j < user.begins[k] + user.size; j++)
                        solution[j] = user.id;
                }
            }
        }
    } catch(GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }

    measures.setSolution(solution);
}

