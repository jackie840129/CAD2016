#ifndef CIRMGR_H
#define CIRMGR_H

#include <iostream>
#include <vector>
#include <string>
#include "cirGate.h"
#include <map>
#include "sat.h"
using namespace std;

class CirMgr;
extern CirMgr* cirMgr;

class module{

    public:
        module(){}
        ~module(){}
        void setName(const string& n){ name = n;}
        void setOutput(const string& o) { output = o;}
        void setInput(const string& i){ input.push_back(i);}
        void setFunction(const string& f){ function = f;}
        const string getName(){return name;}
        const string getOutput(){return output;}
        const vector<string>& getInput(){return input;}
        const string getFunction(){return function;}
    private:
        string name;
        string function;
        vector<string> input;
        string output;
        int delay;
};

class CirMgr{

    public :
        CirMgr(){}
        ~CirMgr(){}

        bool read_module(const string&);
        bool read_circuit(const string&);
        bool read_timeconstraint(const string&);
        bool run_DFS();
        bool print_DFS();
        bool print_io();
        void print_information();
        void genProofModel(SatSolver&);
        void runsat();
    private :
        int time_constraint;
        vector<Wire*> InputList;
        vector<Wire*> OutputList;
        vector<Gate* > GateList;
        vector<Wire* > WireList;
        vector<string> TruePath;
        vector<module*> ModuleList;
        vector< vector<Wire*> > DFSList;
        
};
#endif
