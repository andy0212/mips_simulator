#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include <string>
#include <vector>
#include <map>
#include "convertobinary.h"
#include "binarytoasm.h"
using namespace std;

class Simulator
{
public:
#define SIZE 128
	Simulator();
    int32_t reg[32];
    string inputconsole;
    //bool consolechanged;
	string outputSim();
	string outputReg();
    string outputdata();
    vector<string> outputonebyone();
    ConverToBinary *conver;
    BinaryToAsm *btoa;
    void ReceiveInstr(vector<pair<unsigned long, string> >);
	void ReceiveInstr(pair<unsigned long, string>);
    void ReceiveData(vector<string>);
	void clear();
	void SingleStep();
    void InsertToSim(string asmstring,string binarystring);
	bool Run();
    int CurrentPC();
private:
    int32_t address,PC,DataAdd;
	//vector<pair<unsigned long,unsigned long>> instruct;
	map<unsigned long, unsigned long> instruct;
    map<string,unsigned long> data;
	vector <string> buffer;
	unsigned long memory[SIZE];
};

#endif
