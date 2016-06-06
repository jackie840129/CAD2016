#include<iostream>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"

void CirMgr::genProofModel(SatSolver& s){
    for(size_t i = 0;i<WireList.size();++i){
        Var v = s.newVar();
        WireList[i]->setVar(v);
    }
}
void CirMgr::runsat(){
    SatSolver solver;
    solver.initialize();
    genProofModel(solver);
    /*
    for(size_t i = 0; i< InputList.size();++i){
        //做兩次  0->1  & 1->0
        solver.assertProperty(WireList[0][i]->getVar(),false);
        solver.assertProperty(WireList[1][i]->getVar(),true);
        //see outs , solve SAT
        for(size_t j = 0;j<OutputList.size();++j){
            // 
        }
        //2.
        solver.assertProperty(WireList[0][i]->getVar(),true);
        solver.assertProperty(WireList[1][i]->getVar(),false);
        //see out , solve SAT
        //backtrack
        */
}



