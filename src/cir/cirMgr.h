#ifndef CIRMGR_H
#define CIRMGR_H

#include <iostream>
#include <vector>
#include <string>
#include "cirGate.h"
#include <fstream>
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
           // time_constraint = 10; //just for test case 0;
            output_file = 0;
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
        string get_inputfile(){return input_file;}
        ofstream* get_outputfile(){return output_file;}
        void set_inputfile(string f){input_file = f;}
        void set_outputfile(ofstream* f){output_file = f;}
        void print_information();
        void genProofModel(SatSolver&, int);
        void runsat();
        void DFS_sat(SatSolver& s, Wire* o, size_t t, vector<Wire*> path, int i, bool RF);
        void outputPath(SatSolver& s, vector<Wire*> path);
		// GET SET LAYERSIZE
		void setLayerSize(int l){ _layerSize = l; }
		int getLayerSize(){ return _layerSize; }
    private :
        string input_file;
        ofstream* output_file;
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
