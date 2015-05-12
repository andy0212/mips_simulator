#ifndef  _BINARYTOASM_H
#define  _BINARYTOASM_H
#include <string>
#include <map>
#include <vector>
using namespace std;

class BinaryToAsm
{
public:
	typedef unsigned long dword;
	BinaryToAsm(string str,int PC = 0);
	void ConvertToAsm();
	string output();
private:
	void PrepareData();
	dword StrToBinary(string str);
	std::map<int, string> Rinstr, Iinstr, Jinstr,table,reg;
	vector<string> instr;
	vector<dword> binary;
	vector<pair<dword, string>> BinaryandAsm;
	int PC;
	char target;
};
#endif
