#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <iostream>
#include <vector>
#include <string>


using namespace std;

class Gate;

class Wire{
	public:
		Wire(string id,int time=-1){
			_id = id;
			_time = time;
            _dfsCheck = 0;
		}
		~Wire(){}
		Gate* getFin(){return _fin;}
		void setFin(Gate* g){_fin=g;}
		vector<Gate *> getFout(){return _fout;}
		void addFout(Gate * g){_fout.push_back(g);}
		//void 
		bool getValue(){return _value;}
		void setValue(bool b){_value = b;}
		bool getTF(){return _isTF;}
		void setTF(bool b){_value = b;}
		bool getTime(){return _time;}
		void setTime(int t){_time = t;}
        bool getdfsCheck(){ return _dfsCheck; }
        void setdfsCheck(int num){_dfsCheck = num;}
        string getId(){ return _id; }
	private:
		Gate* _fin;
		vector<Gate *> _fout;
		bool _value;
		bool _isTF;
        int _dfsCheck;
		int _time;
		string _id;
};
class Gate{
	public:
		Gate(string type,string id, Wire* fin0, Wire* fin1, Wire* fo, int delay=1){
			_type = type;
            _id = id;
			_fout = fo;
			fo->setFin(this);
			_fin.push_back(fin0);
			fin0->addFout(this);
            if(fin1!=0){
			    _fin.push_back(fin1);
			    fin1->addFout(this);
            }
			_delay = delay;
		}
		~Gate(){};
		string getId(){return _id;}
		void setId(string s){_id = s;}
		string getType(){return _type;}
		void setType(string s){_type = s;}
		
		Wire* getFout(){return _fout;}
		void setFout(Wire* w){_fout = w;}
        size_t getFinSize(){return _fin.size();}
        //vector<Wire*> getFin()
        Wire* getFin(size_t i) {
            if(i>_fin.size()-1)return 0;
            else return _fin[i];
        }
		Wire* getFin0(){return _fin[0];}
		Wire* getFin1(){
			if(_fin.size()==2)return _fin[1];
			else return 0;
		}
		void setFin0(Wire* w){
			if(_fin.size()==0)
				_fin.push_back(w);
			else
				_fin[0]=w;
		}
		void setFin1(Wire* w){
			if(_fin.size()==1)
				_fin.push_back(w);
			else if (_fin.size()>1)
				_fin[1]=w;
		}
		
		//Call this function while you are sure that the fanin wire is initialized
		void simulation();
		
	private:
		string _id;			
		string _type;
		Wire* _fout;
		vector<Wire*> _fin;
		int _delay;
};



#endif
