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
        CirMgr(){
			_layerSize = 0;
            vector<Wire*> wVec;
            vector<Gate*> gVec;
			InputList.push_back(wVec);
			GateList.push_back(gVec);
			OutputList.push_back(wVec);
			WireList.push_back(wVec);
		}
        ~CirMgr(){}

        bool read_module(const string&);
        bool read_circuit(const string&);
        bool read_timeconstraint(const string&);
		void multi_Layer(size_t);
        void DFS_Visit(Wire*, vector<Wire*>&, int&);
        bool run_DFS();
        bool print_DFS();
        bool print_io();
        void print_information();
        void genProofModel(SatSolver&);
        void runsat();	
		// GET SET LAYERSIZE
		void setLayerSize(int l){ _layerSize = l; }
		int getLayerSize(){ return _layerSize; }
    private :
        int time_constraint;
		int _layerSize;
        vector< vector<Wire*> > InputList;
        vector< vector<Wire*> > OutputList;
        vector< vector<Gate*> > GateList;
        vector< vector<Wire*> > WireList;
        vector<string> TruePath;
        vector<module*> ModuleList;
        vector< vector<Wire*> > DFSList;
        
};
#endif
