#ifndef _CONVERTTOBINARY_H
#define _CONVERTTOBINARY_H
#define ERROR -1
typedef unsigned long dword;
#include<fstream>
#include<map>
//#include<queue>
#include<vector>
using namespace std;

class ConverToBinary{
public:
	vector<dword> instruct;
	dword PC;
    map<string, dword> table;
	ConverToBinary(istream &fin,int PC = 0);
	ConverToBinary(string str = 0,int PC = 0);

	void ConvertToBinary();
	void setPC(int num = 0)
	{
		PC = num;
	}

	string output();
	vector<dword> outputInt() const;
private:
    map<string, dword> operator_code,reg,operator_rt;
	vector<vector<string>> asmtext;
	//istream fin;
	string in;
	void PrepareDate();
	void ConvertFromFile(istream&);
	void ConvertFromStr(string);
	dword ConvertAInstr(vector<string>, int line);
	vector<string> AStrToVector(string,int);
	string mathexpress(string str);
	dword itoc(int m);		//整数转化为16位补码 
	dword StrTo2Comp(string str);
	int StrToUnsigned(string str);
	//dword atom(string str);	//字符串转化为二进制码 
	//int atou(string *str);		//补码转化为整数绝对值 
};
#endif
