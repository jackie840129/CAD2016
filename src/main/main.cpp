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
    //cirMgr->print_io();
    cirMgr->print_io();
    cirMgr->run_DFS();
    cirMgr->print_DFS();
    //cirMgr->read_circuit(argv[2]);
    

}
