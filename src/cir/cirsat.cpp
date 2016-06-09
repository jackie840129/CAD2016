#include<iostream>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"


int PATH_NO = 1 ; // the number of path to be printed

void CirMgr::genProofModel(SatSolver& s, int inputIndex){

    for(size_t i = 0;i<InputList[0].size();++i){
        if(i==inputIndex){
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
            if(gate->getType()=="not"){
                Wire* fanout = gate->getFout();
                if(gate->getFinSize()!=1){
                    cerr<<" not gate has more than one fanin!"<<endl;
                }
                Wire* fanin = gate->getFin(0);
                s.addNotCNF(fanout->getVar(),fanin->getVar());
            }
            else if(gate->getType() == "nand"){
                Wire* fanout = gate->getFout();
                if(gate->getFinSize()!=2){
                    cerr<<" nand gate has error number of fanin!"<<endl;
                }
                Wire* fanin1 = gate->getFin(0);
                Wire* fanin2 = gate->getFin(1);
                s.addNandCNF(fanout->getVar(),fanin1->getVar(),fanin2->getVar());
            }
            else if(gate->getType() == "nor"){
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
    cout<<"Start running SAT..."<<endl;
   
    for(size_t i = 0; i< InputList[0].size();++i){
        solver.initialize();
        genProofModel(solver, i);
        //做兩次  0->1  & 1->0
        cout<<"Input: "<<InputList[0][i]->getId()<<" from 0 to 1, start SAT..."<<endl;
        //see outs , solve SAT
        size_t arrival_time = time_constraint - slack + 1  ; //find the path >slack, one for t=0 ->-inf,


        while(true){  //for time layer
            cout<<" *******************LayerSize: "<<_layerSize<<endl;
           if(arrival_time > _layerSize)break; 
         //  cerr<<"SAT from Layer "<<arrival_time<<endl;
            
           size_t InputSize = InputList[0].size();
           size_t OutputSize = OutputList[0].size();
           for(size_t j = InputSize;j<InputSize+OutputSize;++j){  //for each output
              cout<<"from "<<WireList[arrival_time][j]->getId()<<endl;   
              Var newV = solver.newVar();
              solver.addXorCNF(newV, WireList[arrival_time][j]->getVar(), false, WireList[arrival_time-1][j]->getVar(), false);
              solver.assumeRelease();
              solver.assumeProperty(InputList[0][i]->getVar(),false);
              solver.assumeProperty(InputList[1][i]->getVar(),true);
              solver.assumeProperty(newV, true); //output change

              bool isSat = solver.assumpSolve();
              // if is satisfiable
              if(isSat){
                  cout<< "Start running DFS..."<<endl;
                  vector<Wire*> path; path.push_back(WireList[arrival_time][j]);
                  Wire* fanin1_wire = WireList[arrival_time][j]->getFin()->getFin0(); // fanin is in previous layer
                  DFS_sat(solver, fanin1_wire, arrival_time-1, path, i, true);
                  if(WireList[arrival_time][j]->getFin()->getFinSize()==2){
                      Wire* fanin2_wire = WireList[arrival_time][j]->getFin()->getFin1();
                      DFS_sat(solver, fanin2_wire, arrival_time-1, path, i, true);
                  }

              }
            
           }
           ++arrival_time;
        }
        // 1->0
        cout<<"Input: "<<InputList[0][i]->getId()<<" from 1 to 0, start SAT..."<<endl;
        //see outs , solve SAT
         arrival_time = time_constraint - slack + 1  ; //find the path >slack, one for t=0 ->-inf,


        while(true){  //for time layer
           if(arrival_time > _layerSize)break; 
           cerr<<"SAT from Layer "<<arrival_time<<endl;
            
           size_t InputSize = InputList[0].size();
           size_t OutputSize = OutputList[0].size();
           for(size_t j = InputSize;j<InputSize+OutputSize;++j){  //for each output
              cout<<"from "<<WireList[arrival_time][j]->getId()<<endl;   
              Var newV = solver.newVar();
              solver.addXorCNF(newV, WireList[arrival_time][j]->getVar(), false, WireList[arrival_time-1][j]->getVar(), false);
              solver.assumeRelease();
              solver.assumeProperty(InputList[0][i]->getVar(),true);
              solver.assumeProperty(InputList[1][i]->getVar(),false);
              solver.assumeProperty(newV, true); //output change

              bool isSat = solver.assumpSolve();
              // if is satisfiable
              if(isSat){
                  cout<< "Start running DFS..."<<endl;
                  vector<Wire*> path; path.push_back(WireList[arrival_time][j]);
                  Wire* fanin1_wire = WireList[arrival_time][j]->getFin()->getFin0(); // fanin is in previous layer
                  DFS_sat(solver, fanin1_wire, arrival_time-1, path, i, false);
                  if(WireList[arrival_time][j]->getFin()->getFinSize()==2){
                      Wire* fanin2_wire = WireList[arrival_time][j]->getFin()->getFin1();
                      DFS_sat(solver, fanin2_wire, arrival_time-1, path, i, false);
                  }

              }
            
           }
           ++arrival_time;
        }
    }

}



void CirMgr::DFS_sat(SatSolver& solver, Wire* o, size_t time, vector<Wire*> path, int input_num, bool RiseFall){
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
    solver.assumeProperty(newV, true);

    bool isSat = solver.assumpSolve();
    if(isSat){
        
        path.push_back(o);

        if(time > 1 && o->getFin()!=0){ //not the bottom layer and not the PI
            Wire* fanin1_wire = o->getFin()->getFin0();
            DFS_sat(solver, fanin1_wire, time-1, path, input_num, RiseFall);// fanin in previous layers 
            if(o->getFin()->getFinSize()==2){
                Wire* fanin2_wire = o->getFin()->getFin1();
                DFS_sat(solver, fanin2_wire, time-1, path, input_num, RiseFall);
            }
        }
        //if it is PI or bottom layer
        else if(time == 1){ 
            if(o->getFin()!=0&&(o->getFin()->getFin(0)->getFin()==0||o->getFin()->getFin(1)->getFin()==0)){ //if it is PI
                //get input vector
                //output to file
               // outputPath(solver, path);    
               cout<<"Input vector:"<<endl;
               for(size_t i=0;i<InputList[1].size();i++){
                   cout<<InputList[1][i]->getId()<<" : [ "<<solver.getValue(InputList[1][i]->getVar())<<" ]"<<endl;
               }
                for(size_t i=path.size()-1;;--i){
                    if(i==path.size()-1)cout<<path[i]->getFin()->getId()<<" ["<<solver.getValue(path[i]->getVar())<<"] "<<endl;

                    else cout<<path[i]->getFin()->getId()<<" ["<<solver.getValue(path[i]->getVar())<<"] "<<endl;

                    if(i==0)break;//for the last of path
                }
            }
            else return; //bottom but not PI
            

        }
    }
    
}


void CirMgr::outputPath(SatSolver& solver, vector<Wire*> path){
    //first path
    if(PATH_NO == 1){
        (*output_file)<<"Header { A True Path Set }"<<endl;
        (*output_file)<<endl;
        (*output_file)<<endl;
        (*output_file)<<" Benchmark { "<<input_file<<" }"<<endl;
        (*output_file)<<endl;
        (*output_file)<<endl;
    }

    (*output_file)<<" Path { "<<PATH_NO<<" }"<<endl;
    (*output_file)<<endl;
    (*output_file)<<endl;


    

}



