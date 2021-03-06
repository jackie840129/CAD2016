#include<iostream>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include<iomanip>
int countpath = 0;
void CirMgr::genProofModel(SatSolver& s, int inputIndex){
    for(size_t i = 0;i<InputList[0].size();++i){
        if(i==unsigned(inputIndex)){
            Var v1 = s.newVar();
            Var v2 = s.newVar();
            InputList[0][i]->setVar(v1);
            InputList[1][i]->setVar(v2);
        }
        else{
            Var v1 = s.newVar();
            InputList[0][i]->setVar(v1);
            InputList[1][i]->setVar(v1);
        }
    }
    for(size_t i = 0;i<WireList.size(); ++i){
        for(size_t j = 0; j< WireList[i].size(); ++j){
                if(i!=0&&i!=1){
                    Var v = s.newVar();
                    WireList[i][j]->setVar(v);
                }
                else if(j>=InputList[0].size()){
                    Var v = s.newVar();
                    WireList[i][j]->setVar(v);
                }
            
        }
    }
    for(size_t i = 0;i<GateList.size();++i){
        for(size_t j = 0; j< GateList[i].size();++j){
            Gate* gate = GateList[i][j];
            if(gate->getType()=="NOT1"){
                Wire* fanout = gate->getFout();
                if(gate->getFinSize()!=1){
                    cerr<<" not gate has more than one fanin!"<<endl;
                }
                Wire* fanin = gate->getFin(0);
                s.addNotCNF(fanout->getVar(),fanin->getVar());
            }
            else if(gate->getType() == "NAND2"){
                Wire* fanout = gate->getFout();
                if(gate->getFinSize()!=2){
                    cerr<<" nand gate has error number of fanin!"<<endl;
                }
                Wire* fanin1 = gate->getFin(0);
                Wire* fanin2 = gate->getFin(1);
                s.addNandCNF(fanout->getVar(),fanin1->getVar(),fanin2->getVar());
            }
            else if(gate->getType() == "NOR2"){
                Wire* fanout = gate->getFout();
                if(gate->getFinSize()!=2){
                    cerr<<" nor gate has error number of fanin!"<<endl;
                }
                Wire* fanin1 = gate->getFin(0);
                Wire* fanin2 = gate->getFin(1);
                s.addNorCNF(fanout->getVar(),fanin1->getVar(),fanin2->getVar());
            }
            else cerr<<"There exist unknown gateType!"<<endl;
        }
    }
    
}
void CirMgr::runsat(){
    SatSolver solver;
    //cout<<"Start running SAT..."<<endl;
    size_t small = time_constraint<_layerSize?time_constraint:_layerSize;
    //cout<<"in sat's time_constraint "<<time_constraint<<endl;
    for(size_t i = 0; i< InputList[0].size();++i){
        solver.initialize();
        genProofModel(solver, i);
        //做兩次  0->1  & 1->0
        //cout<<"Input: "<<InputList[0][i]->getId()<<" from 0 to 1, start SAT..."<<endl;
        //see outs , solve SAT
        size_t arrival_time = small   ; //find the path >slack, one for t=0 ->-inf,
        
       // vector< bool > changed(OutputList[0].size()); //to make sure it is the last time the output change
        while(true){  //for time layer
            //cout<<" *******************LayerSize: "<<_layerSize<<endl;
            if(arrival_time < time_constraint - slack +1 )break; 
           cerr<<"SAT from Layer "<<arrival_time<<endl;
            
            size_t InputSize = InputList[0].size();
            size_t OutputSize = OutputList[0].size();
            for(size_t j = InputSize;j<InputSize+OutputSize;++j){  //for each output
                //cout<<"from "<<WireList[arrival_time][j]->getId()<<endl;   
                Var newV = solver.newVar();
               // Var newV2 = solver.newVar();
                solver.addXorCNF(newV, WireList[arrival_time][j]->getVar(),
                               false, WireList[arrival_time-1][j]->getVar(), false);
               // solver.addXorCNF(newV2, WireList[small][j]->getVar(),
               //             false, WireList[0][j]->getVar(), false);
                solver.assumeRelease();
                solver.assumeProperty(InputList[0][i]->getVar(),false);
                solver.assumeProperty(InputList[1][i]->getVar(),true);
                solver.assumeProperty(newV, true); //output change
               // solver.assumeProperty(newV2, true);

                bool isSat = solver.assumpSolve();
                //if is satisfiable
                if(isSat ){
         //           changed[j-InputSize] = true;
                    //cout<< "Start running DFS..."<<endl;
                    vector<Wire*> path; path.push_back(WireList[arrival_time][j]);
                    Wire* fanin1_wire = WireList[arrival_time][j]->getFin()->getFin0(); // fanin is in previous layer
                    DFS_sat(solver, fanin1_wire, arrival_time-1, path, i, true, WireList[arrival_time][j], arrival_time);
                    if(WireList[arrival_time][j]->getFin()->getFinSize()==2){
                        Wire* fanin2_wire = WireList[arrival_time][j]->getFin()->getFin1();
                        DFS_sat(solver, fanin2_wire, arrival_time-1, path, i, true, WireList[arrival_time][j], arrival_time);
                    }
                }
            
            }
            --arrival_time;
        }
        // 1->0
        //cout<<"Input: "<<InputList[0][i]->getId()<<" from 1 to 0, start SAT..."<<endl;
        //see outs , solve SAT
        arrival_time = small   ; //find the path >slack, one for t=0 ->-inf,

//        vector<bool> changed2(OutputList[0].size()); //to make sure it is the last time the output change

        while(true){  //for time layer
           if(arrival_time < time_constraint - slack + 1)break; 
          // cerr<<"SAT from Layer "<<arrival_time<<endl;
            
           size_t InputSize = InputList[0].size();
           size_t OutputSize = OutputList[0].size();
           for(size_t j = InputSize;j<InputSize+OutputSize;++j){  //for each output
              //cout<<"from "<<WireList[arrival_time][j]->getId()<<endl;   
              Var newV = solver.newVar();
              //Var newV2 = solver.newVar();
              solver.addXorCNF(newV, WireList[arrival_time][j]->getVar(), 
                               false, WireList[arrival_time-1][j]->getVar(), false);
              //solver.addXorCNF(newV2, WireList[small][j]->getVar(),
             //                  false, WireList[0][j]->getVar(), false);
              solver.assumeRelease();
              solver.assumeProperty(InputList[0][i]->getVar(),true);
              solver.assumeProperty(InputList[1][i]->getVar(),false);
              solver.assumeProperty(newV, true); //output change
         //     solver.assumeProperty(newV2,true); //different from -inf

              bool isSat = solver.assumpSolve();
              // if is satisfiable
              if(isSat ){
               //   changed2[j-InputSize] = true;
                  //cout<< "Start running DFS..."<<endl;
                  vector<Wire*> path; path.push_back(WireList[arrival_time][j]);
                  Wire* fanin1_wire = WireList[arrival_time][j]->getFin()->getFin0(); // fanin is in previous layer
                  DFS_sat(solver, fanin1_wire, arrival_time-1, path, i, false, WireList[arrival_time][j], arrival_time);
                  if(WireList[arrival_time][j]->getFin()->getFinSize()==2){
                      Wire* fanin2_wire = WireList[arrival_time][j]->getFin()->getFin1();
                      DFS_sat(solver, fanin2_wire, arrival_time-1, path, i, false, WireList[arrival_time][j], arrival_time);
                  }

              }
            
           }
           --arrival_time;
        }
    }
cout<<"we find "<<countpath<<"path!"<<endl;
}

//*** changed_output means that we need to consider the XOR of changing output when DFS

void CirMgr::DFS_sat(SatSolver& solver, Wire* o, size_t time, vector<Wire*> path, int input_num, bool RiseFall, Wire* change_ouput, int change_time){
    // check if it is satisfiable with previous layer
 //   cout<<"Wire "<<o->getId()<<" at layer: "<<time<<endl;
    Var newV = solver.newVar();
    Wire* previous = WireList[time-1][o->getListNum()];
    solver.addXorCNF(newV, o->getVar(), false, previous->getVar(), false);
    solver.assumeRelease();
    if(RiseFall){   
        solver.assumeProperty(InputList[0][input_num]->getVar(),false);
        solver.assumeProperty(InputList[1][input_num]->getVar(),true);
    }
    else{
        solver.assumeProperty(InputList[0][input_num]->getVar(),true);
        solver.assumeProperty(InputList[1][input_num]->getVar(),false);
    }
    Var newV1 = solver.newVar();
    //Var newV2 = solver.newVar();
    solver.addXorCNF(newV1, change_ouput->getVar(), 
        false, WireList[change_time-1][change_ouput->getListNum()]->getVar(), false);
 //   solver.addXorCNF(newV2, stable->getVar(),
   //     false, WireList[0][stable->getListNum()]->getVar(), false);

    solver.assumeProperty(newV, true);
    solver.assumeProperty(newV1, true);
//    solver.assumeProperty(newV2, true);

    bool isSat = solver.assumpSolve();
    if(isSat){
        
        path.push_back(o);

        if(time > 1 && o->getFin()!=0){ //not the bottom layer and not the PI
            Wire* fanin1_wire = o->getFin()->getFin0();
            DFS_sat(solver, fanin1_wire, time-1, path, input_num, RiseFall, change_ouput, change_time);// fanin in previous layers 
            if(o->getFin()->getFinSize()==2){
                Wire* fanin2_wire = o->getFin()->getFin1();
                DFS_sat(solver, fanin2_wire, time-1, path, input_num, RiseFall, change_ouput, change_time);
            }
        }
        //if it is PI or bottom layer
        else if(time == 1){ 
            //if(o->getFin()!=0){
             if(o->getFin()->getFin(0)->getFin()==0||o->getFin()->getFin(1)->getFin()==0){
                //if it is PI
                //get input vector
                //output to file
               //bool isTrue = lastCheck(solver, o, input_num);
               //if(isTrue){
                  outputPath(solver, path,input_num,RiseFall);    
                  countpath++;
                  //cout<<"find a path!"<<endl
                  cout<<"find a path!"<<endl;

              // }
               /*cout<<"Input vector:"<<endl;
               for(size_t i=0;i<InputList[1].size();i++){
                   cout<<InputList[1][i]->getId()<<" : [ "<<solver.getValue(InputList[1][i]->getVar())<<" ]"<<endl;
               }
                for(size_t i=path.size()-1;;--i){
                    if(i==path.size()-1)cout<<path[i]->getFin()->getId()<<" ["<<solver.getValue(path[i]->getVar())<<"] "<<endl;

                    else cout<<path[i]->getFin()->getId()<<" ["<<solver.getValue(path[i]->getVar())<<"] "<<endl;

                    if(i==0)break;//for the last of path
                }
                */
            }
            else return; //bottom but not PI
            

        }
    }
    
}
/*
bool CirMgr::lastCheck(SatSolver& s, Wire* lastwire, int input_num){
    Gate* lastGate = lastwire->getFin();
    string type = lastGate->getType();
    if(type == "NOT1"){
        if(lastGate->getFin(0)==0) return true;
        else return false;
    }
    else if(type == "NAND2"){
        if(lastGate->getFin(0)==InputList[1][input_num]){
            if(s.getValue(lastGate->getFin(1)->getVar())==0)return false;
            else return true;
        }
        else{
            if(s.getValue(lastGate->getFin(0)->getVar()) == 0) return false;
            else return true;
          }
    }
    else if (type == "NOR2"){

        if(lastGate->getFin(0)==InputList[1][input_num]){
            if(s.getValue(lastGate->getFin(1)->getVar())==0)return true;
            else return false;
        }
        else{
            if(s.getValue(lastGate->getFin(0)->getVar()) == 0) return true;
            else return false;
        }
          
    }
}
*/
int PATH_NO = 0;

void CirMgr::outputPath(SatSolver& solver, vector<Wire*> path,int input_num,bool risefall){
    //first path
    PATH_NO++;
    if(PATH_NO == 1){
        (*output_file)<<"Header { A True Path Set }"<<endl;
        (*output_file)<<endl;
        (*output_file)<<" Benchmark { "<<input_file<<" }"<<endl;
        (*output_file)<<endl;
    }
    (*output_file)<<" Path { "<<PATH_NO<<" }"<<endl;
    (*output_file)<<endl;
    (*output_file)<<" A True Path List\n"<<" {"<<endl;
    (*output_file)<<" ";
    for(int i = 0;i<75;++i)(*output_file)<<"-";
    (*output_file)<<endl;
    (*output_file)<<" ";
    (*output_file)<<setw(10)<<left<<"Pin";
    (*output_file)<<setw(25)<<left<<"type";
    (*output_file)<<setw(20)<<left<<"Incr";
    (*output_file)<<setw(15)<<left<<"Path delay";
    (*output_file)<<endl;
    (*output_file)<<" ";
    for(int i = 0;i<75;++i)(*output_file)<<"-";
    (*output_file)<<endl;
    //change to gate
    vector<Gate*>path_g;
    vector<Wire*>new_path;
    new_path.push_back(InputList[1][input_num]);
    for(size_t i = path.size()-1;;--i){
        path_g.push_back(path[i]->getFin());
        new_path.push_back(path[i]);
        if(i == 0)break;
    }
    //output input~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int delay=0;
    string English = new_path[0]->getId().substr(0,1);
    string number =  new_path[0]->getId().substr(1);
    string rf = risefall?"r":"f";
    string temp = (number!="")?(English+"["+number+"]"):English;
    (*output_file)<<" ";
    (*output_file)<<setw(10)<<left<<temp;
    (*output_file)<<setw(25)<<left<<"(in)";
    (*output_file)<<setw(20)<<left<<" 0";
    temp = "0 "+rf;
    (*output_file)<<setw(15)<<left<<temp;
    (*output_file)<<endl;
    //-------output gate
    for(size_t i =0;i<path_g.size();++i){
        int whichwire ;
        string AB;
        for(size_t j =0; j<path_g[i]->getFinSize();++j){
            if((path_g[i]->getFin(j))->getId() == new_path[i]->getId()){
                whichwire = j;
            }
        }
        //here ↓↓↓↓↓↓↓↓
        AB = whichwire == 0? "A":"B";
        temp = path_g[i]->getId()+"/"+AB;
        (*output_file)<<" ";
        (*output_file)<<setw(10)<<left<<temp;
        temp = "("+path_g[i]->getType()+")";
        (*output_file)<<setw(25)<<left<<temp;
        (*output_file)<<setw(20)<<left<<" 0";
        (*output_file)<<left<<delay<<" ";
        ++delay;
        rf = solver.getValue(new_path[i]->getVar())?"r":"f";
        (*output_file)<<left<<rf<<endl;
        //--
        (*output_file)<<" ";
        temp = path_g[i]->getId()+"/"+"Y";
        (*output_file)<<setw(10)<<left<<temp;
        temp = "("+path_g[i]->getType()+")";
        (*output_file)<<setw(25)<<left<<temp;
        (*output_file)<<setw(20)<<left<<" 1";
        (*output_file)<<left<<delay<<" ";
        rf = solver.getValue(new_path[i+1]->getVar())?"r":"f";
        (*output_file)<<left<<rf<<endl;
    }
    //output output
    English = new_path.back()->getId().substr(0,1);
    number =  new_path.back()->getId().substr(1);
    rf = solver.getValue(new_path.back()->getVar())?"r":"f";
    temp = (number!="")?(English+"["+number+"]"):English;
    (*output_file)<<" ";
    (*output_file)<<setw(10)<<left<<temp;
    (*output_file)<<setw(25)<<left<<"(out)";
    (*output_file)<<setw(20)<<left<<" 0";
    (*output_file)<<left<<delay<<" ";
    (*output_file)<<left<<rf<<endl;

    (*output_file)<<" ";
    for(int i = 0;i<75;++i)(*output_file)<<"-";
    (*output_file)<<endl;
    (*output_file)<<" Data Required Time";
    (*output_file)<<setw(14)<<right<<time_constraint<<endl;
    (*output_file)<<" Data Arrival Time";
    (*output_file)<<setw(15)<<right<<delay<<endl;
    (*output_file)<<" ";
    for(int i = 0;i<75;++i)(*output_file)<<"-";
    (*output_file)<<endl;
    (*output_file)<<" Slack";
    (*output_file)<<setw(27)<<right<<time_constraint-delay<<endl;
    (*output_file)<<" }\n\n";


    (*output_file)<<" Input Vector\n"<<" {"<<endl;
    for(size_t i = 0;i<InputList[1].size();++i){
        English = InputList[1][i]->getId().substr(0,1);
        number = InputList[1][i]->getId().substr(1); 
        temp = (number!="")?(English+"["+number+"]"):English;
        (*output_file)<<"   "<<temp<<"  =  ";
        if(i!=input_num){
            (*output_file)<<solver.getValue(InputList[1][i]->getVar())<<endl;
        }
        else{
            if(risefall)(*output_file)<<"r"<<endl;
            else (*output_file)<<"f"<<endl;
        }

    }
    (*output_file)<<" }\n"<<endl;

}



