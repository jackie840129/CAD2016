#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include "cirGate.h"
#include "cirMgr.h"

using namespace std;

//#define DEBUG

int global_dfsNum = 0;
// int layerSize = 0;

vector<string> lexOption(string str, vector<string> & vec){
    size_t pos1=0, pos2=0;
    string lex = "";
    while(pos2!=string::npos){
       pos1 = str.find_first_not_of(" ",pos2);
       pos2 = str.find_first_of(" ",pos1+1);
       lex = str.substr(pos1,pos2-pos1);
       vec.push_back(lex);
    }
    return vec; 
}

int assignWire(Wire* &wire, vector<Wire*> vec){
    for(size_t i=0 ; i<vec.size() ; ++i){
        if(wire->getId()==vec[i]->getId()){
            wire = vec[i];
			cout << "found " << wire->getId() << " ";
            return i;
        }
    }
    return -1;
}

// check Gate
void checkGate(Gate* gate){
	cerr<<"========================================="<<endl;
	cerr<<"Gate "<< gate->getId()<<"("<<gate->getType()<<")"<<endl;
	if(gate->getFin0()->getFin()!=0){
		Gate* gateIn0 = gate->getFin0()->getFin();
		cerr<<"FanIn0 connected to Gate "<<gateIn0->getId()<<"("<<gateIn0->getType()<<")"<<endl;
	}
	else{
		cerr<<"FanIn0 connected to Input "<<gate->getFin0()->getId()<<endl;
	}
	if(gate->getType()!="NOT1"){
		if(gate->getFin1()->getFin()!=0){
			Gate* gateIn1 = gate->getFin1()->getFin();
			cerr<<"FanIn1 connected to Gate "<<gateIn1->getId()<<"("<<gateIn1->getType()<<")"<<endl;
		}
		else{
			cerr<<"FanIn1 connected to Input "<<gate->getFin1()->getId()<<endl;
		}	
	}
	if((gate->getFout())->getFout().size()==0)
		cerr<<"This is Connect to output wire: "<<(gate->getFout())->getId()<<endl;
	cerr<<"========================================="<<endl;
}



int max(int a, int b){
	return (a>b)?a :b;
}

void CirMgr::DFS_Visit(Wire* wire, vector<Wire*> &dfsOrder, int& depth){

    wire->setdfsCheck(global_dfsNum);
    if(wire->getFin()==0){
        //dfsOrder.push_back(wire);
		int d = depth;
		setLayerSize(max(_layerSize, d));
        return;
    }
    size_t finSize = wire->getFin()->getFinSize();
    for(size_t i=0 ; i<finSize ; ++i){
        Wire* w = wire->getFin()->getFin(i);
        //if(w->getdfsCheck()!=global_dfsNum){
            depth += 1; 
            DFS_Visit(w,dfsOrder, depth);
			depth -= 1;
            dfsOrder.push_back(w);
        //}
    }
    return;
}

bool CirMgr::read_module(const string& file_name){

    ifstream file(file_name.c_str());
        string line;
        int module_no = 0; //which module is reading now
        int line_no = 0;
        while(getline(file, line)){
            //remove the space
            if(line.find_first_not_of(" ",0)==string::npos)continue;
            line  = line.substr(line.find_first_not_of(" ",0));
            // read module
            // if it's a new module , push it into modulelist
            if(line[0] == 'm'){
                module* m = new module();
                m->setName(line.substr(7,line.find_first_of(" ",7)-7));
                ModuleList.push_back(m);
                #ifdef DEBUG
                cerr<<"name = "<<ModuleList[module_no]->getName()<<endl;
                #endif

            }
            //read this module's output
            else if(line[0] =='o'){
                ModuleList[module_no]->setOutput(line.substr(7,1));
                #ifdef DEBUG 
                cerr<<"output = "<<ModuleList[module_no]->getOutput()<<endl;
                #endif
            }
            //read this module's inputs
            else if(line[0] =='i'){
                int start = 6;
                while(true){
                    int end = line.find_first_of(",",start);
                    if(end == string::npos){
                        ModuleList[module_no]->setInput(line.substr(start,line.find_first_of(";",start)-start));
                        break;
                    }
                    ModuleList[module_no]->setInput(line.substr(start,end-start));
                    start = end+1;
                }
                #ifdef DEBUG
                vector<string>temp = ModuleList[module_no]->getInput();
                for(size_t i=0;i<temp.size();i++){
                    cerr<<"input = "<<temp[i]<<endl;
                }
                #endif
            }
            //read this module's function
            else if(line[0] == 'n'){
                int end = line.find_first_of(" ",0);
                ModuleList[module_no]->setFunction(line.substr(0,end));
                #ifdef DEBUG
                cerr<<"function = "<<ModuleList[module_no]->getFunction()<<endl;
                #endif
            }
            //endmodule
            else if(line[0]=='e'&&line[3] == 'm'){
                ++module_no;
            }

            ++line_no;

            }

	return true;

}
void setVNum(Gate* gate, int out, int in0, int in1=-1){
	gate->setFoutVecNum(out);
	gate->addFinVecNum(in0);
	if(in1!=-1){
		gate->addFinVecNum(in1);
	}
}
void CirMgr::read_circuit(const string& file_name){    
    cout << "now reading case1" << endl;
	int count = 0;
    ifstream ifs(file_name.c_str());
    string line; // read every line in the file into "line"
    string category; // specify which catergory I am now reading. Ex. input, output, wire.
    string type; // determine which gate type it is
    while(getline(ifs,line).good()){
        //cout << "fuck" << endl;
        /*if(line.compare("\n")==0){
            cout << "skip a line" << endl;
            continue;
        }*/
        if(line.find_first_not_of(" ",0)==string::npos)continue;
        //remove all the spaces from the beginning of a line
        line  = line.substr(line.find_first_not_of(" ",0));
        string first_word = line.substr(0,line.find_first_of(" "));
        if(first_word.compare("module")==0){
            category = "module";
            line = line.substr(line.find_first_of(" "));
        }
        if(first_word.compare("input")==0){ 
            category = "input";
            line = line.substr(line.find_first_of(" "));
        }
        else if(first_word.compare("output")==0){ 
            category = "output";
            line = line.substr(line.find_first_of(" "));
        }
        else if(first_word.compare("wire")==0){ 
            category = "wire";
            line = line.substr(line.find_first_of(" "));
        }
        else if ( first_word.compare("NOT1")==0 || first_word.compare("NOR2")==0 
                || first_word.compare("NAND2")==0 ){ 
            category = "gate";
            type = first_word;
        }
        #ifdef DEBUG
        cerr << line << endl;
        #endif
        // PARSE INPUT AND OUTPUT
        string name; // parse the name of each input and outputi
        if(category.compare("input")==0){
            #ifdef DEBUG
            cerr<<"input: "<<endl;
            #endif
            vector<string> parse;
            lexOption(line, parse);
            for(size_t i=0 ; i<parse.size() ; ++i){
                size_t pos = parse[i].find_first_of(",;");
                string name = parse[i].substr(0,pos);
                #ifdef DEBUG
                cerr<<name<<" ";
                #endif
                Wire* wire = new Wire(name);
                wire->setFin(0);
				wire->setListNum(count);
				++count;
                InputList[0].push_back(wire);
                WireList[0].push_back(wire);
            }
        }
        else if(category.compare("output")==0){
            #ifdef DEBUG
            cerr<<"output: "<<endl;
            #endif
            vector<string> parse;
            lexOption(line, parse);
            for(size_t i=0 ; i<parse.size() ; ++i){
                size_t pos = parse[i].find_first_of(",;");
                string name = parse[i].substr(0,pos);
                #ifdef DEBUG
                cerr<<name<<" ";
                #endif
                Wire* wire = new Wire(name);
                wire->addFout(0);
				wire->setListNum(count);
				++count;
                OutputList[0].push_back(wire);
                WireList[0].push_back(wire);
            }       
        }
        else if(category.compare("wire")==0){
            #ifdef DEBUG
            cerr<<"wire: "<<endl;
            #endif
            vector<string> parse;
            lexOption(line, parse);
            for(size_t i=0 ; i<parse.size() ; ++i){
                size_t pos = parse[i].find_first_of(",;");
                string name = parse[i].substr(0,pos);
                #ifdef DEBUG
                cerr<<name<<" ";
                #endif
                Wire* wire = new Wire(name);
				wire->setListNum(count);
				++count;
                WireList[0].push_back(wire);
            }       
        }
        else if(category.compare("gate")==0){
            
            vector<string> param;
            
            size_t found = line.find_first_of(" ");
            string lex = line.substr(0,found);
            param.push_back(lex);
            line = line.substr(found+1);
            found = line.find_first_of(" ");
            lex = line.substr(0,found);
            param.push_back(lex);
            line = line.substr(found+1);
            //cout << "xxx" << endl;
            size_t pos1 = 0, pos2 = 0, pos3 = 0; // pos1 finds "." pos2 finds "(" pos3 finds ")"
            while(1){
                pos1 = line.find_first_of(".",pos3+1);
                if(pos1==string::npos) break;
                pos2 = line.find_first_of("(",pos1+1);
                pos3 = line.find_first_of(")",pos2+1);
                param.push_back(line.substr(pos1+1,pos2-pos1-1));
                param.push_back(line.substr(pos2+1,pos3-pos2-1));
            }
            
            #ifdef DEBUG
            cerr<<"gate: ";
            for(size_t i=0 ; i<param.size() ; ++i){
                cerr<<param[i]<<" ";
            }
            #endif
            //cout << endl;
			int in0;
			int in1;
			int out;
            if(param[0].compare("NOT1")==0){
                if(param[2].compare("Y")==0){
                    Wire* wire_in = new Wire(param[5]);
                    Wire* wire_out = new Wire(param[3]);
                    in0 = assignWire(wire_in,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
					Gate* gate = new Gate("NOT1", param[1], wire_in, 0, wire_out);
                    setVNum(gate,out,in0);
					GateList[0].push_back(gate);
                }
                else if(param[4].compare("Y")==0){
                    Wire* wire_in = new Wire(param[3]);
                    Wire* wire_out = new Wire(param[5]);
                    in0 = assignWire(wire_in,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NOT1", param[1], wire_in, 0, wire_out);
                    setVNum(gate,out,in0);
                    GateList[0].push_back(gate);
                }
            }
            else if(param[0].compare("NOR2")==0){
                if(param[2].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[5]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[3]);
                    in0 = assignWire(wire_in1,WireList[0]);
                    in1 = assignWire(wire_in2,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NOR2", param[1], wire_in1, wire_in2, wire_out);
                    setVNum(gate,out,in0,in1);
                    GateList[0].push_back(gate);
                }
                else if(param[4].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[5]);
                    in0 = assignWire(wire_in1,WireList[0]);
                    in1 = assignWire(wire_in2,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NOR2", param[1], wire_in1, wire_in2, wire_out);
                    setVNum(gate,out,in0,in1);
                    GateList[0].push_back(gate);
                }
                else if(param[6].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[5]);
                    Wire* wire_out = new Wire(param[7]);
                    in0 = assignWire(wire_in1,WireList[0]);
                    in1 = assignWire(wire_in2,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NOR2", param[1], wire_in1, wire_in2, wire_out);
                    setVNum(gate,out,in0,in1);
                    GateList[0].push_back(gate);
                }
            }
            else if(param[0].compare("NAND2")==0){
                if(param[2].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[5]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[3]);
                    in0 = assignWire(wire_in1,WireList[0]);
                    in1 = assignWire(wire_in2,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NAND2", param[1], wire_in1, wire_in2, wire_out);
                    setVNum(gate,out,in0,in1);
                    GateList[0].push_back(gate);
                }
                else if(param[4].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[5]);
                    in0 = assignWire(wire_in1,WireList[0]);
                    in1 = assignWire(wire_in2,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NAND2", param[1], wire_in1, wire_in2, wire_out);
                    setVNum(gate,out,in0,in1);
                    GateList[0].push_back(gate);
                }
                else if(param[6].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[5]);
                    Wire* wire_out = new Wire(param[7]);
                    in0 = assignWire(wire_in1,WireList[0]);
                    in1 = assignWire(wire_in2,WireList[0]);
                    out = assignWire(wire_out,WireList[0]);
                    Gate* gate = new Gate("NAND2", param[1], wire_in1, wire_in2, wire_out);
                    setVNum(gate,out,in0,in1);
                    GateList[0].push_back(gate);
                }
            }
        }
    }
    cout << endl;
}
//Multi-Layer Construction
//J-You class must change to multiple WireList & GateList
//
void CirMgr::multi_Layer(size_t Layer=0){
	//TODO input the longest route
	size_t layerSize = Layer;
	size_t wireSize = WireList[0].size();
	size_t gateSize = GateList[0].size();
	size_t inputSize = InputList[0].size();
	size_t outputSize = OutputList[0].size();
	for(size_t i=0; i<layerSize; ++i){
		//i is previous levelu
		//i+1 is current level
		vector<Wire*> currentWireList;
		vector<Gate*> currentGateList;
		vector<Wire*> currentInputList;
		int in0, in1, out;
		for(size_t j=0; j<wireSize; ++j){	
			Wire* wire = new Wire(WireList[i][j]->getId());
			wire->setListNum(j);
			currentWireList.push_back(wire);
			/*if(j>=inputSize && j<outputSize+inputSize){
				currentWireList.push_back(wire);
			}*/
		}
        #ifdef DEBUG
        cerr << "constuct a layer, new current WireList[i]" << endl;
        #endif

		//input of newer
		if(i == 0){
			for(size_t k=0; k<inputSize; ++k){
				currentWireList[k]->setFin(0);
				currentInputList.push_back(currentWireList[k]);
			}
			InputList.push_back(currentInputList);
		}
        
        #ifdef DEBUG
        if(i==0){
        	cerr<< " Second layer input = " << endl;
            for(size_t j=0 ; j<inputSize ; ++j){
                cerr << currentInputList[j]->getId() << " ";
            }
        }
        #endif
		//gate implement
		for(size_t j=0; j<gateSize; ++j){
			Gate* gate = new Gate(GateList[i][j]->getType(),GateList[i][j]->getId(),0,0,0);
			#ifdef DEBUG
            cerr << "new Gate in gateList["<<i<<"][" << j << "] --FINISHED..." << endl;
            #endif

			in0 = GateList[i][j]->getFin0VecNum();
			out = GateList[i][j]->getFoutVecNum();
			if(in0<(int)inputSize){
				gate->setFin0(InputList[1][in0]);
				InputList[1][in0]->addFout(gate);
			}
			else{
				gate->setFin0(WireList[i][in0]);
				WireList[i][in0]->addFout(gate);
			}
			gate->setFout(currentWireList[out]);
			currentWireList[out]->setFin(gate);
			if(gate->getType()!="NOT1"){
				in1 = GateList[i][j]->getFin1VecNum();
				if(in1<(int)inputSize){
					gate->setFin1(InputList[1][in1]);
					InputList[1][in1]->addFout(gate);
				}
				else{
					gate->setFin1(WireList[i][in1]);
					WireList[i][in1]->addFout(gate);
				}
                setVNum(gate,out,in0,in1);
			}
			else{
                setVNum(gate,out,in0);
			}
			currentGateList.push_back(gate);
		}
		WireList.push_back(currentWireList);
		GateList.push_back(currentGateList);
	}
	#ifdef DEBUG
    cerr <<"Check All Gate"<< endl;
    
	for(size_t i=0;i<layerSize;++i){
		cerr<<"\n*********LAYER "<<i<<"*********\n"<<endl;
		for(size_t j=0;j<gateSize;++j){
			checkGate(GateList[i][j]);	
		}
	}
	#endif	
}


bool CirMgr::read_timeconstraint(const string& file_name){
    ifstream file(file_name.c_str());
    if(!file)return false;
    string line;
    while(getline(file,line)){
        // check max_delay
        if(line.size()>9&&line.substr(8,5)=="delay"){
           // substr the  number
            int start = 14 ;//start of the time_constraint
            int end = line.find_first_of(" ",start);
            string t = line.substr(start, end-start);
            time_constraint = atoi(t.c_str());
            cout << "Time Constraint = "<<time_constraint<<endl;
            break;
        }
    }
    return true;
}
bool CirMgr::run_DFS(){
   
    /*global_dfsNum += 1;
    vector<Wire*> vec;
    cout << "fuck" << endl;
    DFS_Visit(OutputList[0],vec);
    DFSList.push_back(vec);*/
	_layerSize = 0;
    for(size_t i=0 ; i<OutputList[0].size() ; ++i){
        //cout << "Output(" << i << ") " << OutputList[i]->getId() << endl;
		int depth = 1;
        global_dfsNum += 1; 
        vector<Wire*> vec;
        DFS_Visit(OutputList[0][i],vec, depth);
        vec.push_back(OutputList[0][i]);
        DFSList.push_back(vec);
    } 
	cout << "layerSize = " << getLayerSize() << endl;
	return true;

}

bool CirMgr::print_DFS(){

    cout << "PRINT_DFS" << endl;
    for(size_t i=0 ; i<DFSList.size() ; ++i){
        cout << "Output" << i << " (" << OutputList[0][i]->getId() << ")" << endl;
        for(size_t j=0 ; j<DFSList[i].size() ; ++j){
            if(j==0)
                cout << DFSList[i][j]->getId();
            else
                cout << " -> " << DFSList[i][j]->getId();
        }
        cout << endl;
    }
	return true;
}

bool CirMgr::print_io(){
    cout << "========= PRINT_IO =========" << endl; 
    cout << "GATE:" << endl;
    for(size_t i=0 ; i<GateList[0].size() ; ++i){
        cout << "(" << i << ")";
        cout << " ID = " << GateList[0][i]->getId();
        cout << ", Type = " << GateList[0][i]->getType();
        cout << ", Fanin0 = " << GateList[0][i]->getFin0()->getId();
        if(GateList[0][i]->getFin1()!=0)
            cout << ", Fanin1 = " << GateList[0][i]->getFin1()->getId();
        cout << ", Fanout = " << GateList[0][i]->getFout()->getId();
        cout << endl;
    }

    cout << "WIRE: " << endl;
    for(size_t i=0 ; i<WireList[0].size() ; ++i){
        cout << "(" << i << ")";
        cout << " ID = " << WireList[0][i]->getId();
        if(WireList[0][i]->getFin()!=0){
            cout << ", Fanin = " << WireList[0][i]->getFin()->getId() << " (";
            Gate* gate = WireList[0][i]->getFin();
            cout << " " << gate->getFin0()->getId();
            if(gate->getFin1()!=0)
                cout << " " << gate->getFin1()->getId();
            cout << " )";
        }
        cout << endl;
    }
    cout << endl;
    return true;
}
void CirMgr::print_information(){
    cout<<"Input List size = "<<InputList[0].size()<<endl;
    cout<<"Output List size = "<<OutputList[0].size()<<endl;
    cout<<"Gate List size = "<<GateList[0].size()<<endl;
}
