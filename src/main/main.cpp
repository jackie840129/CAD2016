#include <iostream> 
#include <string>
#include "cirMgr.h"
#include "cirGate.h"

using namespace std;
CirMgr* cirMgr = 0;


int main(int argc, char**argv){
    
    cirMgr = new CirMgr();
    string file1 = "PD_case/case0/input/cadcontest.v";
    cirMgr->read_module(file1);
    string file2 = "PD_case/case0/input/case0";
    cirMgr->read_circuit(file2);
    cirMgr->print_information();
    cirMgr->print_io();
    cirMgr->run_DFS();
    cirMgr->print_DFS();
    string file3 = "PD_case/case1/input/case1.sdc";
    cirMgr->read_timeconstraint(file3);
    cout << "Check assign slack in Makefile: "<<slack<<endl;
	cirMgr->multi_Layer(cirMgr->getLayerSize());    
}
