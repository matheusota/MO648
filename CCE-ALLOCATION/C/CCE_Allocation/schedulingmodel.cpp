#include "schedulingmodel.h"
bool SchedulingModel::shouldUseF = false;

void SchedulingModel::execute(vector<User> &users2, int R, int numberUsers, Measures &measures){
    // copy users
    vector<User> users(users2.begin(), users2.begin() + numberUsers);

    // gurobi stuff
    map<int, map<int, GRBVar>> x;
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.set(GRB_StringAttr_ModelName, "CCE_SCH"); // gives a name to the problem
    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE); // says that lp is a minimization problem
    GRBVar f_bar;
    //model.set(GRB_DoubleParam_MIPGap, 0);
    //model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1); //must set to use lazy contraints

    // solution variable
    vector<int> solution(R);
    fill(solution.begin(), solution.end(), 0);

    // initialize gurobi variables
    for(auto user: users){
        for(auto b : user.begins){
            x[user.id][b] = model.addVar(0.0, 1.0, user.price, GRB_BINARY, "x[" + to_string(user.id) + "][" + to_string(b) + "]");
        }
    }
    if(shouldUseF)
        f_bar = model.addVar(0.0, 1.0, -8, GRB_BINARY, "f_bar");
    model.update();

    // add "user can only be allocated once" constraint
    for(auto user: users){
        GRBLinExpr expr = 0;

        for(auto b : user.begins){
            expr += x[user.id][b];
        }

        model.addConstr(expr <= 1);
    }

    // add "no overlap" constraint
    for(int j = 0; j < R; j++){
        GRBLinExpr expr = 0;

        for(auto user: users){
            int aux = max(0, j - user.size + 1);

            for(int t = aux; t <= j; t++){
                if(find(user.begins.begin(), user.begins.end(), t) != user.begins.end()){
                    expr += x[user.id][t];
                }
            }
        }

        model.addConstr(expr <= 1);
    }

    // define f
    if(shouldUseF){
        for(int j = 0; j < R; j++){
            GRBLinExpr expr = 0;

            for(auto user: users){
                int aux = max(0, j - user.size + 1);

                for(int t = aux; t <= j; t++){
                    if(find(user.begins.begin(), user.begins.end(), t) != user.begins.end()){
                        expr += x[user.id][t];
                    }
                }
            }

            model.addConstr(expr >= (1 - f_bar));
        }
    }

    /*
    // set to 0 the positions the user cannot start
    for(auto user: users){
        for(int j = 0; j < R; j++){
            if(find(user.begins.begin(), user.begins.end(), j) == user.begins.end()){
                model.addConstr(x[user.id][j] == 0);
            }
        }
    }
    */

    model.update();
    try {
        // set callback
        //CutsCallback cb = CutsCallback(users, R, x);
        //model.setCallback(&cb);

        // now we solve it
        model.optimize();
        model.update();

        cout << "Solution Cost: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

        // create a vector from the solution
        for(auto user: users){
            for(auto b : user.begins){
                //cout << x[user.id][j].get(GRB_StringAttr_VarName) << " = " << x[user.id][j].get(GRB_DoubleAttr_X) << endl;

                if(x[user.id][b].get(GRB_DoubleAttr_X) > 0.5){
                    for(int t = b; t < b + user.size; t++)
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

