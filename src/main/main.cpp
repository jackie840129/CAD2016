#include <iostream> 
#include <fstream>
#include <string>
#include "cirMgr.h"
#include "cirGate.h"

using namespace std;
CirMgr* cirMgr = 0;


int main(int argc, char**argv){
    
    cirMgr = new CirMgr();
    string file1 = "PD_case/case3/input/cadcontest.v";
    cirMgr->read_module(file1);
    string file2 = "PD_case/case3/input/case3";
    cirMgr->read_circuit(file2);
    cirMgr->set_inputfile("case3");
    cirMgr->print_information();
    cirMgr->print_io();
  
    cirMgr->run_DFS();
    //cirMgr->print_DFS();
    //string file3 = "PD_case/case3/input/case3.sdc";
    //cirMgr->read_timeconstraint(file3);
    cirMgr->settime_constraint(45);
    cirMgr->setLayerSize(cirMgr->get_time_constraint());

    cout << "Check assign slack in Makefile: "<<slack<<endl;
	cirMgr->multi_Layer(cirMgr->getLayerSize());    

    
 //   cout << "Check assign slack in Makefile: "<<slack<<endl;
    
    cirMgr->multi_Layer(cirMgr->getLayerSize());
    
    string s = "output/"+cirMgr->get_inputfile()+"_out";
    ofstream* output = new ofstream(s.c_str());
    cirMgr->set_outputfile(output);
    cirMgr->runsat();
}
