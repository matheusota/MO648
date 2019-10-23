#include "bestmodel.h"

void BestModel::execute(vector<User> &users2, int R, int numberUsers, Measures &measures){
    // copy users
    vector<User> users(users2.begin(), users2.begin() + numberUsers);

    // gurobi stuff
    map<int, map<int, GRBVar>> s;
    map<int, GRBVar> b;
    map<int, GRBVar> p;
    map<int, GRBVar> a;
    map<int, GRBVar> k;
    GRBVar f;

    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.set(GRB_StringAttr_ModelName, "CCE_SCH"); // gives a name to the problem
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE); // says that lp is a minimization problem
    model.set(GRB_DoubleParam_MIPGapAbs, 1);
    //model.getEnv().set(GRB_IntParam_OutputFlag, 0);
    //model.getEnv().set(GRB_IntParam_LazyConstraints, 1); //must set to use lazy contraints

    // solution variable
    vector<int> solution(R);
    fill(solution.begin(), solution.end(), 0);

    // initialize gurobi variables
    for(auto user: users){
        k[user.id] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "k[" + to_string(user.id) + "]");
        a[user.id] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "a[" + to_string(user.id) + "]");
        b[user.id] = model.addVar(0.0, 1.0, 1.0, GRB_BINARY, "b[" + to_string(user.id) + "]");

        for(auto b : user.begins){
            s[user.id][b] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "s[" + to_string(user.id) + "][" + to_string(b) + "]");
        }
    }
    for(int j = 0; j < R; j++){
        p[j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "p[" + to_string(j) + "]");
    }
    f = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "f");
    model.update();

    // a definition constraint
    for(auto user: users){
        GRBLinExpr expr = 0;

        for(auto b : user.begins){
            expr += s[user.id][b];
        }

        model.addConstr(expr == a[user.id]);
    }

    // p definition constraint
    for(int j = 0; j < R; j++){
        GRBLinExpr expr = 0;

        for(auto user: users){
            int aux = max(0, j - user.size + 1);

            for(int t = aux; t <= j; t++){
                if(find(user.begins.begin(), user.begins.end(), t) != user.begins.end()){
                    expr += s[user.id][t];
                }
            }
        }

        model.addConstr(expr == p[j]);
    }

    // now we define k

    // k is contiguous
    for(int i = 1; i < users.size(); i++){
        model.addConstr(k[users[i].id] <= k[users[i - 1].id]);
    }

    // this constraint enforces that k[i] cannot be one if there is not a user j >= i
    // that was allocated
    for(int i = 0; i < users.size(); i++){
        GRBLinExpr expr = 0;

        for(int i2 = i; i2 < users.size(); i2++){
            expr += a[users[i2].id];
        }

        model.addConstr(expr >= k[users[i].id]);
    }

    // if user i was allocated, than k[i] is one
    for(auto user: users){
        model.addConstr(a[user.id] <= k[user.id]);
    }

    // now lets define f
    // add f definition constraint
    GRBLinExpr expr_f = 0;
    for(int j = 0; j < R; j++){
        expr_f += p[j];
    }
    model.addConstr(expr_f <= (R - 1) + f);

    // if a position was not filled, then f is zero
    for(int j = 0; j < R; j++){
        model.addConstr(p[j] >= f);
    }

    // now lets define b
    for(auto user: users){
        model.addConstr(k[user.id] + (1 - f) - 2 * a[user.id] <= 2 * b[user.id]);
    }

    model.update();
    model.write("model.lp");
    try {
        // now we solve it
        model.optimize();
        model.update();

        cout << "Solution Cost: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

        // create a vector from the solution
        for(auto user: users){
//            cout << "k[" << user.id << "] 0.0" << endl;
//            cout << "a[" << user.id << "] 0.0" << endl;
//            cout << "b[" << user.id << "] 1.0" << endl;

            for(auto b : user.begins){
                //cout << "x[" << user.id << "][" << b << "] 0.0" << endl;
                if(s[user.id][b].get(GRB_DoubleAttr_X) > 0.5){
                    for(int t = b; t < b + user.size; t++)
                        solution[t] = user.id;
                }
            }
        }
//        for(int j = 0; j < R; j++){
//            cout << "p[" << j << "] 0.0" << endl;
//        }
    } catch(GRBException e) {
        cout << "Error number: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }

    measures.setSolution(solution);
}

