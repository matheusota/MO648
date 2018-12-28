#include "schedulingmodel.h"

void SchedulingModel::execute(vector<User> &users2, int R, int numberUsers, Measures &measures){
    // copy users
    vector<User> users(users2.begin(), users2.begin() + numberUsers);

    //for(auto u: users)
    //    u.printUser();

    // gurobi stuff
    map<int, map<int, GRBVar>> x;
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.set(GRB_StringAttr_ModelName, "CCE_SCH"); // gives a name to the problem
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE); // says that lp is a minimization problem
    model.set(GRB_DoubleParam_MIPGap, 0);
    model.getEnv().set(GRB_IntParam_OutputFlag, 0);


    // solution variable
    vector<int> solution(R);
    fill(solution.begin(), solution.end(), 0);

    // initialize gurobi variables
    for(auto user: users){
        for(int j = 0; j < R; j++)
            x[user.id][j] = model.addVar(0.0, 1.0, user.price, GRB_BINARY, "x[" + to_string(user.id) + "][" + to_string(j) + "]");
    }
    model.update();

    // add "user can only be allocated once" constraint
    for(auto user: users){
        GRBLinExpr expr = 0;

        for(int j = 0; j < R; j++)
            expr += x[user.id][j];

        model.addConstr(expr <= 1);
    }

    // add "no overlap" constraint
    for(int j = 0; j < R; j++){
        GRBLinExpr expr = 0;

        for(auto user: users){
            int aux = max(0, j - user.size + 1);

            for(int t = aux; t <= j; t++)
                expr += x[user.id][t];
        }

        model.addConstr(expr <= 1);
    }

    // set to 0 the positions the user cannot start
    for(auto user: users){
        for(int j = 0; j < R; j++){
            if(find(user.begins.begin(), user.begins.end(), j) == user.begins.end()){
                model.addConstr(x[user.id][j] == 0);
            }
        }
    }

    model.update();
    try {
        // now we solve it
        model.optimize();
        model.update();

        cout << "Solution Cost: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

        // create a vector from the solution
        for(auto user: users){
            for(int j = 0; j < R; j++){
                //cout << x[user.id][j].get(GRB_StringAttr_VarName) << " = " << x[user.id][j].get(GRB_DoubleAttr_X) << endl;

                if(x[user.id][j].get(GRB_DoubleAttr_X) > 0.5){
                    for(int t = j; t < j + user.size; t++)
                        solution[t] = user.id;
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

