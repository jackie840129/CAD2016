#include "cirGate.h"
#include <iostream>

using namespace std;
/*
void
Gate::simulation(){
	int time0, time1, fouttime;
	bool TF0,TF1;
	bool value0,value1,foutvalue;
	if(_type == "not"){
		//Value_of_fanout
		if(_fin[0]->getValue()==true)
			_fout->setValue(false);
		else
			_fout->setValue(true);
		//Time_of_fanout
		time0 = _fin[0]->getTime();
		_fout->setTime(time0+_delay);
		//TF_of_fanin
		_fin[0]->setTF(true);
	}
	else if (_type == "nand"){
		time0 = _fin[0]->getTime();
		time1 = _fin[1]->getTime();
		value0 = _fin[0]->getValue();
		value1 = _fin[1]->getValue();
		//Same time 
		if(time0 == time1){
			if			(!value0 && !value1) //0,0
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = true;
			}
			else if		(!value0 && value1 ) //0.1
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if 	(value0  && !value1) //1,0
			{
				foutvalue = true; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if		(value0  && value1 ) //1,1
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = true;	
			}
		}
		//fan[0] arrive faster
		else if(time0 < time1){
			if			(!value0 && !value1) //0,0
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if		(!value0  && value1) //0.1
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if 	(value0 && !value1 ) //1,0
			{
				foutvalue = true; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if		(value0  && value1 ) //1,1
			{
				foutvalue = false; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
		}
		//fan[1] arrive faster
		else{
			if			(!value0 && !value1) //0,0
			{
				foutvalue = true; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if		(!value0  && value1) //0.1
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if 	(value0 && !value1 ) //1,0
			{
				foutvalue = true; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if		(value0  && value1 ) //1,1
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
		}
		//Value
		_fout->setValue(foutvalue);
		//Time
		_fout->setTime(fouttime);
		//TF
		_fin[0]->setTF(TF0);
		_fin[1]->setTF(TF1);

	}
	else if (_type == "nor"){
		time0 = _fin[0]->getTime();
		time1 = _fin[1]->getTime();
		value0 = _fin[0]->getValue();
		value1 = _fin[1]->getValue();
		//Same time 
		if(time0 == time1){
			if			(!value0 && !value1) //0,0
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = true;
			}
			else if		(!value0 && value1 ) //0.1
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if 	(value0  && !value1) //1,0
			{
				foutvalue = false; 
				fouttime = time1 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if		(value0  && value1 ) //1,1
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = true;	
			}
		}
		//fan[0] arrive faster
		else if(time0 < time1){
			if			(!value0 && !value1) //0,0
			{
				foutvalue = true; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;
			}
			else if		(!value0 && value1 ) //0.1
			{
				foutvalue = false; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if 	(value0  && !value1) //1,0
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if		(value0  && value1 ) //1,1
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
		}
		//fan[1] arrive faster
		else{
			if			(!value0 && !value1) //0,0
			{
				foutvalue = true; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;
			}
			else if		(!value0 && value1 ) //0.1
			{
				foutvalue = false; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
			else if 	(value0  && !value1) //1,0
			{
				foutvalue = false; 
				fouttime = time0 + _delay; 
				TF0 = true;
				TF1 = false;	
			}
			else if		(value0  && value1 ) //1,1
			{
				foutvalue = false; 
				fouttime = time1 + _delay; 
				TF0 = false;
				TF1 = true;	
			}
		}
		//Value
		_fout->setValue(foutvalue);
		//Time
		_fout->setTime(fouttime);
		//TF
		_fin[0]->setTF(TF0);
		_fin[1]->setTF(TF1);
	}
	else
		cerr<<"ERROR: no such gate: \""<<_type<<"\""<<endl;
}
*/
