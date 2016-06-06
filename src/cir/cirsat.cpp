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



