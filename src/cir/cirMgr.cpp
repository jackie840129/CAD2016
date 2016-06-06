#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "cirGate.h"
#include "cirMgr.h"

using namespace std;

//#define DEBUG

int global_dfsNum = 0;

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

void assignWire(Wire* &wire, vector<Wire*> vec){
    for(int i=0 ; i<vec.size() ; ++i){
        if(wire->getId()==vec[i]->getId()){
            wire = vec[i];
            break;
        }
    }
}

void DFS_Visit(Wire* wire, vector<Wire*> &dfsOrder){

    wire->setdfsCheck(global_dfsNum);

    if(wire->getFin()==0){
        //dfsOrder.push_back(wire);
        return;
    }
    size_t finSize = wire->getFin()->getFinSize();
    for(size_t i=0 ; i<finSize ; ++i){
        //if(wire->getFin()==0) continue;
        Wire* w = wire->getFin()->getFin(i);
        if(w->getdfsCheck()!=global_dfsNum){
            DFS_Visit(w,dfsOrder);
            dfsOrder.push_back(w);
        }
    }

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
                for(int i=0;i<temp.size();i++){
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



}

bool CirMgr::read_circuit(const string& file_name){    
    cout << "now reading case1" << endl;
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
        /*if(category.compare("module")==0){
            #ifdef DEBUG
            cerr << "modele: " << endl;
            #endif
            vector<string> parse;
            lexOption(line,parse);
            circuitName = parse[0];
            for(int i=2 ; i<parse.size()-1 ; ++i){
                circuitIOList.push_back(parse[i][0]);
            }  
        }*/
        if(category.compare("input")==0){
            #ifdef DEBUG
            cerr<<"input: "<<endl;
            #endif
            vector<string> parse;
            lexOption(line, parse);
            for(int i=0 ; i<parse.size() ; ++i){
                size_t pos = parse[i].find_first_of(",;");
                string name = parse[i].substr(0,pos);
                #ifdef DEBUG
                cerr<<name<<" ";
                #endif
                Wire* wire = new Wire(name);
                wire->setFin(0);
                InputList.push_back(wire);
                WireList.push_back(wire);
            }
        }
        else if(category.compare("output")==0){
            #ifdef DEBUG
            cerr<<"output: "<<endl;
            #endif
            vector<string> parse;
            lexOption(line, parse);
            for(int i=0 ; i<parse.size() ; ++i){
                size_t pos = parse[i].find_first_of(",;");
                string name = parse[i].substr(0,pos);
                #ifdef DEBUG
                cerr<<name<<" ";
                #endif
                Wire* wire = new Wire(name);
                wire->addFout(0);
                OutputList.push_back(wire);
                WireList.push_back(wire);
            }       
        }
        else if(category.compare("wire")==0){
            #ifdef DEBUG
            cerr<<"wire: "<<endl;
            #endif
            vector<string> parse;
            lexOption(line, parse);
            for(int i=0 ; i<parse.size() ; ++i){
                size_t pos = parse[i].find_first_of(",;");
                string name = parse[i].substr(0,pos);
                #ifdef DEBUG
                cerr<<name<<" ";
                #endif
                Wire* wire = new Wire(name);
                WireList.push_back(wire);
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
            for(int i=0 ; i<param.size() ; ++i){
                cerr<<param[i]<<" ";
            }
            #endif
            //cout << endl;
            if(param[0].compare("NOT1")==0){
                if(param[2].compare("Y")==0){
                    Wire* wire_in = new Wire(param[5]);
                    Wire* wire_out = new Wire(param[3]);
                    assignWire(wire_in,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("not", param[1], wire_in, 0, wire_out);
                    GateList.push_back(gate);
                }
                else if(param[4].compare("Y")==0){
                    Wire* wire_in = new Wire(param[3]);
                    Wire* wire_out = new Wire(param[5]);
                    assignWire(wire_in,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("not", param[1], wire_in, 0, wire_out);
                    GateList.push_back(gate);
                }
            }
            else if(param[0].compare("NOR2")==0){
                if(param[2].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[5]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[3]);
                    assignWire(wire_in1,WireList);
                    assignWire(wire_in2,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("nor", param[1], wire_in1, wire_in2, wire_out);
                    GateList.push_back(gate);
                }
                else if(param[4].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[5]);
                    assignWire(wire_in1,WireList);
                    assignWire(wire_in2,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("nor", param[1], wire_in1, wire_in2, wire_out);
                    GateList.push_back(gate);
                }
                else if(param[6].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[5]);
                    Wire* wire_out = new Wire(param[7]);
                    assignWire(wire_in1,WireList);
                    assignWire(wire_in2,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("nor", param[1], wire_in1, wire_in2, wire_out);
                    GateList.push_back(gate);
                }
            }
            else if(param[0].compare("NAND2")==0){
                if(param[2].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[5]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[3]);
                    assignWire(wire_in1,WireList);
                    assignWire(wire_in2,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("nand", param[1], wire_in1, wire_in2, wire_out);
                    GateList.push_back(gate);
                }
                else if(param[4].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[7]);
                    Wire* wire_out = new Wire(param[5]);
                    assignWire(wire_in1,WireList);
                    assignWire(wire_in2,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("nand", param[1], wire_in1, wire_in2, wire_out);
                    GateList.push_back(gate);
                }
                else if(param[6].compare("Y")==0){
                    Wire* wire_in1 = new Wire(param[3]);
                    Wire* wire_in2 = new Wire(param[5]);
                    Wire* wire_out = new Wire(param[7]);
                    assignWire(wire_in1,WireList);
                    assignWire(wire_in2,WireList);
                    assignWire(wire_out,WireList);
                    Gate* gate = new Gate("nand", param[1], wire_in1, wire_in2, wire_out);
                    GateList.push_back(gate);
                }
            }
        }
    }
    cout << endl;
}

bool CirMgr::run_DFS(){
   
    /*global_dfsNum += 1;
    vector<Wire*> vec;
    cout << "fuck" << endl;
    DFS_Visit(OutputList[0],vec);
    DFSList.push_back(vec);*/
    for(int i=0 ; i<OutputList.size() ; ++i){
        //cout << "Output(" << i << ") " << OutputList[i]->getId() << endl;
        global_dfsNum += 1;
        vector<Wire*> vec;
        DFS_Visit(OutputList[i],vec);
        vec.push_back(OutputList[i]);
        DFSList.push_back(vec);
    }   
}

bool CirMgr::print_DFS(){

    cout << "PRINT_DFS" << endl;
    for(int i=0 ; i<DFSList.size() ; ++i){
        cout << "Output" << i << " (" << OutputList[i]->getId() << ")" << endl;
        for(int j=0 ; j<DFSList[i].size() ; ++j){
            if(j==0)
                cout << DFSList[i][j]->getId();
            else
                cout << " -> " << DFSList[i][j]->getId();
        }
        cout << endl;
    }
}

bool CirMgr::print_io(){
    
    cout << "========= PRINT_IO =========" << endl;
    cout << "GATE:" << endl;
    for(int i=0 ; i<GateList.size() ; ++i){
        cout << "(" << i << ")";
        cout << " ID = " << GateList[i]->getId();
        cout << ", Type = " << GateList[i]->getType();
        cout << ", Fanin0 = " << GateList[i]->getFin0()->getId();
        if(GateList[i]->getFin1()!=0)
            cout << ", Fanin1 = " << GateList[i]->getFin1()->getId();
        cout << ", Fanout = " << GateList[i]->getFout()->getId();
        cout << endl;
    }

    cout << "WIRE: " << endl;
    for(int i=0 ; i<WireList.size() ; ++i){
        cout << "(" << i << ")";
        cout << " ID = " << WireList[i]->getId();
        if(WireList[i]->getFin()!=0){
            cout << ", Fanin = " << WireList[i]->getFin()->getId() << " (";
            Gate* gate = WireList[i]->getFin();
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
    cout<<"Input List size = "<<InputList.size()<<endl;
    cout<<"Output List size = "<<OutputList.size()<<endl;
    cout<<"Gate List size = "<<GateList.size()<<endl;
}

