#include"convertobinary.h"
#include <sstream>
#include <stack>
#include <algorithm>
#include <cctype>
//#include<iostream>
//using namespace std;
ConverToBinary::ConverToBinary(string str,int PC){
	in = str;
	PrepareDate();
	this->PC = PC;
	ConvertFromStr(in);
	//ConvertToBinary();
}

string ConverToBinary::output()
{
	dword aline;
	dword mask = 0;
	stringstream sstr;
	int line = PC;
	for (int i = 0; i < instruct.size(); i++)
	{
		mask = 0x80000000;
		aline = instruct[i];
		//sstr << line << "; ";
		line += 4;
		for (int j = 0; j < 32; j++,mask>>=1)
		{
			if (aline&mask)
				sstr << '1';
			else sstr << '0';
			if ((j + 1) % 8 == 0)
				sstr << ' ';
		}
		sstr << endl;
	}
	return sstr.str();
}

vector<dword> ConverToBinary::outputInt() const
{
	return instruct;
}


ConverToBinary::ConverToBinary(istream &in,int PC){
	PrepareDate();
	this->PC = PC;
	ConvertFromFile(in);
	//ConvertToBinary();
}

void ConverToBinary::PrepareDate(){
	operator_code["add"] = 32;
	operator_code["addu"] = 33;
	operator_code["nor"] = 39;
	operator_code["sub"] = 34;
	operator_code["subu"] = 35;
	operator_code["and"] = 36;
	operator_code["xor"] = 41;
	operator_code["sllv"] = 4;
	operator_code["or"] = 37;
	operator_code["slt"] = 42;
	operator_code["sltu"] = 43;
	operator_code["srav"] = 7;
	operator_code["srlv"] = 6;
	operator_code["srl"] = 2;
	operator_code["sra"] = 3;
	operator_code["sll"] = 0;
	operator_code["break"] = 13;
	operator_code["syscall"] = 12;
	operator_code["div"] = 26;
	operator_code["divu"] = 27;
	operator_code["mult"] = 24;
	operator_code["multu"] = 25;
	operator_code["jr"] = 8;
	operator_code["addi"] = 8;
	operator_code["addiu"] = 9;
	operator_code["slti"] = 10;
	operator_code["sltiu"] = 11;
	operator_code["xori"] = 14;
	operator_code["andi"] = 12;
	operator_code["ori"] = 13;
	operator_code["lw"] = 35;
	operator_code["sw"] = 43;
	operator_code["lh"] = 33;
	operator_code["lb"] = 32;
	operator_code["sb"] = 48;
	operator_code["sh"] = 41;
	operator_code["beq"] = 4;
	operator_code["bne"] = 5;
	operator_code["j"] = 2;
	operator_code["bgez"] = 1;
	operator_code["bgtz"] = 7;
	operator_code["blez"] = 6;
	operator_code["bltz"] = 1;
	operator_code["lui"] = 15;
	operator_rt["bgez"] = 1;
	operator_rt["bgtz"] = 0;
	operator_rt["blez"] = 0;
	operator_rt["bltz"] = 0;

	reg["zero"] = 0;
	reg["at"] = 1;
	reg["v0"] = 2;
	reg["v1"] = 3;
	reg["a0"] = 4;
	reg["a1"] = 5;
	reg["a2"] = 6;
	reg["a3"] = 7;

	stringstream sstr;
	string s = "t";
	for (int i = 0; i <= 7; i++){
		sstr.str("");
		sstr << s << i;
		reg[sstr.str()] = 8 + i;
	}
	sstr.str("");
	s = "s";
	for (int i = 0; i <= 7; i++){
		sstr.str("");
		sstr << s << i;
		reg[sstr.str()] = 16 + i;
	}

	reg["t8"] = 24;
	reg["t9"] = 25;

	reg["k0"] = 26;
	reg["k1"] = 27;

	reg["gp"] = 28;
	reg["sp"] = 29;
	reg["fp"] = 30;
	reg["ra"] = 31;
}

int to_lower(int c)
    {
        if (isupper(c))
        {
            return c+32;
        }

        return c;
}

void ConverToBinary::ConvertFromFile(istream &in)
{
	string instr;
    string word,firstword,secondword;
	vector<string> aline;
	int i,line;
	line = PC;
	while (!in.eof())
	{
		getline(in, instr);
        istringstream read(instr);
        if(read >> firstword)
            if(firstword[0] == ';'||firstword[0] == '.')
                continue;
            else if(read >> secondword)
                if(secondword[0] == '.')
                    continue;
		i = 0;
		aline.clear();
		while (i < instr.size())
		{
			if (instr[i] == ' ' || instr[i] == 9 || instr[i] == ',' || instr[i] == ';'
				||instr[i]== '('||instr[i]== ')'||instr[i] == ':'||i == instr.size() - 1)
			{
				if (i == instr.size() - 1)
				{
					if (instr[i] >= '0'&&instr[i] <= '9' || instr[i] >= 'A'&&instr[i] <= 'Z'
						|| instr[i] >= 'a'&&instr[i] <= 'z')
						word += instr[i];
				}
				if (word.size())
				{
                    //transform(word.begin(), word.end(),word.begin(),tolower);
                    transform(word.begin(),word.end(),word.begin(),to_lower);
					if (word.size()&&instr[i]== ':')
						table[word] = line;
					else if (operator_code.find(word) == operator_code.end() && reg.find(word) == reg.end()
						&&table.find(word) == table.end())
					{
						bool isExpress = true;
						for (int k = 0; k < word.size();k++)
							if (word[k] >= 'a'&&word[k] <= 'z')
							{
								isExpress = false;
								break;
							}
						if (!isExpress){ //标记
							table[word] = 0;
							aline.push_back(word);
						}
						else aline.push_back(mathexpress(word));//数学表达式	
					}
					else aline.push_back(word);//寄存器或命令
					word.clear();
				}
			}
			else if (instr[i] != '$')
				word += instr[i];
			if (instr[i] == ';')
				break;
			i++;
		}
		if (instr[i] == ';')
			while (i < instr.size())
				i++;
		if (aline.size())
			asmtext.push_back(aline);
		aline.clear();
		line += 4;
	}
}

void ConverToBinary::ConvertFromStr(string inputvalue){
	string word;
	vector<string> aline;
	int line;
	line = PC;
	int i = 0, j = 0;
    istringstream read(inputvalue);
    string str,firstword,secondword;
    while (getline(read,str)){
        istringstream readline(str);
        if(readline >>firstword)
            if(firstword[0] == ';'||firstword[0] == '.')
                continue;
            else if(readline >> secondword)
                if(secondword[0] == '.')
                    continue;
		bool issemcolon = false;
        j = 0;
		while (j<str.size()){
			if (str[j] == ' ' || str[j] == 9 || str[j] == ',' || str[j] == ';'
				|| str[j] == '(' || str[j] == ')' || str[j] == '\n'||str[j] == ':'||j==str.size() - 1)
			{
				if (j == str.size() - 1)
				{
					if (str[j] >= '0'&&str[j] <= '9' || str[j] >= 'A'&&str[j] <= 'Z'
						|| str[j] >= 'a'&&str[j] <= 'z')
						word += str[j];
				}
				if (word.size())
				{
                    transform(word.begin(), word.end(), word.begin(), to_lower);
					if (word.size()&&str[j] == ':')
						table[word] = line;
					else if (operator_code.find(word) == operator_code.end() && reg.find(word) == reg.end()
						&&table.find(word) == table.end())
					{
						bool isExpress = true;
						for (int k = 0; k < word.size(); k++)
							if (word[k] >= 'a'&&word[k] <= 'z')
							{
								isExpress = false;
								break;
							}
						if (!isExpress){ //标记
							table[word] = 0;
							aline.push_back(word);
						}
						else aline.push_back(mathexpress(word));//数学表达式	
					}
					else aline.push_back(word);//寄存器或命令
					word.clear();
				}
			}
			else if (str[j] != '$')
				word += str[j];
			if (str[j] == ';' || str[j] == '\n'){
				if (str[j] == ';')
					issemcolon = true;
				break;
			}
			j++;
		}
		if (issemcolon)
		{
			while (j < str.size() && str[j] != '\n')
				j++;
		}
		//if (word.size() && aline.size())
			//aline.push_back(word);
		if (aline.size())
			asmtext.push_back(aline);
		aline.clear();
		line += 4;
	}
}

dword ConverToBinary::ConvertAInstr(vector<string> aline, int line)
{
	dword instr = 0;
	if (aline.size() == 0)
	{
		instr = ERROR;
		instruct.push_back(instr);
	}
	line += 4;
	if (aline[0] == "add" || aline[0] == "sub" || aline[0] == "and" || aline[0] == "or" || aline[0] == "slt"
		|| aline[0] == "addu" || aline[0] == "nor" || aline[0] == "sllv" || aline[0] == "sltu" || aline[0] == "srav"
		|| aline[0] == "srlw" || aline[0] == "subu" || aline[0] == "xor"){
		if (aline.size() == 4)
			instr = (reg[aline[2]] << 21) | (reg[aline[3]] << 16) | (reg[aline[1]]) << 11 | operator_code[aline[0]];
		else instr = ERROR;
	}
	else if (aline[0] == "sll" || aline[0] == "srl" || aline[0] == "sra")
	{
		if (aline.size() == 4)
			instr = (reg[aline[2]] << 16) | (reg[aline[1]] << 11) | (StrTo2Comp(aline[3]) << 6) | operator_code[aline[0]];
		else instr = ERROR;
	}
	else if (aline[0] == "break" || aline[0] == "syscall")
	{
		if (aline.size() == 1)
			instr = operator_code[aline[0]];
		else instr = ERROR;
	}
	else if (aline[0] == "div" || aline[0] == "divu" || aline[0] == "mult" || aline[0] == "multu")
	{
		if (aline.size() == 3)
			instr = (reg[aline[1]] << 21) | (reg[aline[2]] << 16) | operator_code[aline[0]];
		else instr = ERROR;
	}
	else if (aline[0] == "jr")
	{
		if (aline.size() == 2)
			instr = (reg[aline[1]]) | operator_code[aline[0]];
		else instr = ERROR;
	}
	else if (aline[0] == "addi" || aline[0] == "andi" || aline[0] == "ori" || aline[0] == "slti")
	{
		if (aline.size() == 4)
			instr = (operator_code[aline[0]] << 26) | (reg[aline[2]] << 21) | (reg[aline[1]] << 16) | StrTo2Comp(aline[3]);
		else instr = ERROR;
	}
	else if (aline[0] == "lw" || aline[0] == "sw" || aline[0] == "lb" || aline[0] == "lh" || aline[0] == "sb"
		|| aline[0] == "sh")
	{
		if (aline.size() == 4)
			instr = (operator_code[aline[0]] << 26) | (reg[aline[3]] << 21) | (reg[aline[1]] << 16) | StrTo2Comp(aline[2]);
		//else if (aline.size() == 3)
		//instr = (operator_code[aline[0]] << 26) | (reg[aline[2]] << 21) | (reg[aline[1]] << 16);
		else instr = ERROR;
	}
	else if (aline[0] == "beq" || aline[0] == "bne")
	{
		if (aline.size() == 4)
		{
			instr = (operator_code[aline[0]] << 26) | (reg[aline[1]] << 21) | (reg[aline[2]] << 16);
			if (table.find(aline[3]) != table.end())
                instr |= (table[aline[3]] - line + 4) >> 2 & 0x0000ffff;
			else instr |= StrTo2Comp(aline[3]);
		}
		else instr = ERROR;
	}
	else if (aline[0] == "j" || aline[0] == "jal")
	{
		if (aline.size() == 2)
		{
			instr = operator_code[aline[0]] << 26;
			if (table.find(aline[1]) == table.end())
				instr |= StrTo2Comp(aline[1]);
			else instr |= (table[aline[1]] >> 2);
		}
	}
	else if (aline[0] == "bgez" || aline[0] == "bgtz" || aline[0] == "blez" || aline[0] == "bltz")
	{
		if (aline.size() == 3)
		{
			instr = (operator_code[aline[0]] << 26) | (operator_rt[aline[0]] << 11) | (reg[aline[1]] << 21);
			if (table.find(aline[2]) == table.end())
				instr |= StrTo2Comp(aline[2]);
			else instr |= (table[aline[2]] - line) >> 2 & 0x0000ffff;
		}
		else instr = ERROR;
	}
	else if (aline[0] == "lui")
	{
		if (aline.size() == 3)
		{
            instr = (operator_code[aline[0]] << 26) | (reg[aline[1]] << 16) | static_cast<int16_t>(StrTo2Comp(aline[2]));
		}
		else instr = ERROR;
	}
	return instr;
}

vector<string> ConverToBinary::AStrToVector(string str,int line)
{
	vector<string> aline;
	string word;
	bool issemcolon = false;
	int j = 0;
	while (j<str.size()){
		if (str[j] == ' ' || str[j] == 9 || str[j] == ',' || str[j] == ';'
			|| str[j] == '(' || str[j] == ')' || str[j] == '\n' || str[j] == ':' || j == str.size() - 1)
		{
			if (j == str.size() - 1)
			{
				if (str[j] >= '0'&&str[j] <= '9' || str[j] >= 'A'&&str[j] <= 'Z'
					|| str[j] >= 'a'&&str[j] <= 'z')
					word += str[j];
			}
			if (word.size())
			{
                transform(word.begin(), word.end(), word.begin(), to_lower);
				if (word.size() && str[j] == ':')
					table[word] = line;
				else if (operator_code.find(word) == operator_code.end() && reg.find(word) == reg.end()
					&& table.find(word) == table.end())
				{
					bool isExpress = true;
					for (int k = 0; k < word.size(); k++)
						if (word[k] >= 'a'&&word[k] <= 'z')
						{
							isExpress = false;
							break;
						}
					if (!isExpress){ //标记
						table[word] = 0;
						aline.push_back(word);
					}
					else aline.push_back(mathexpress(word));//数学表达式	
				}
				else aline.push_back(word);//寄存器或命令
				word.clear();
			}
		}
		else if (str[j] != '$')
			word += str[j];
		if (str[j] == ';' || str[j] == '\n'){
			if (str[j] == ';')
				issemcolon = true;
			break;
		}
		j++;
	}
	if (issemcolon)
	{
		while (j < str.size() && str[j] != '\n')
			j++;
	}
	return aline;
}

string ConverToBinary::mathexpress(string str)
{
	stack<int> optor;
	stack<char> oprend;
	int i = 0, num = 0, op1, op2;
	char tpoprend;
	while (i<str.size())
	{
		if (i == str.size() - 1 && str[i] >= '0'&&str[i] <= '9'){
			num = num * 10 + str[i] - '0';
			optor.push(num);
		}
		else if (str[i] >= '0'&&str[i] <= '9')
			num = num * 10 + str[i] - '0';
		else
		{
			optor.push(num);
			num = 0;
			if (str[i] == '+' || str[i] == '-')
			{
				while (oprend.size())
				{
					tpoprend = oprend.top();
					if (tpoprend == '(')
						break;
					oprend.pop();
					if (optor.size() >= 2)
					{
						op1 = optor.top();
						optor.pop();
						op2 = optor.top();
						optor.pop();
						if (tpoprend == '*')
							optor.push(op1*op2);
						else if (tpoprend == '/')
							optor.push(op2 / op1);
						else if (tpoprend == '+')
							optor.push(op1 + op2);
						else if (tpoprend == '-')
							optor.push(op2 - op1);
					}
					else if (optor.size() == 1)
					{
						op1 = 0;
						op2 = optor.top();
						optor.pop();
					if (tpoprend == '+')
						optor.push(op1 + op2);
					else if (tpoprend == '-')
						optor.push(op2 - op1);
					}
				}
				oprend.push(str[i]);
			}
			else if (str[i] == '*' || str[i] == '/')
			{
				while (oprend.size())
				{
					tpoprend = oprend.top();
					if (tpoprend != '*'&&tpoprend != '/')
						break;
					oprend.pop();
					if (optor.size() >= 2)
					{
						op1 = optor.top();
						optor.pop();
						op2 = optor.top();
						optor.pop();
						if (tpoprend == '*')
							optor.push(op1*op2);
						else if (tpoprend == '/')
							optor.push(op2 / op1);
					}
					else break;
				}
				oprend.push(str[i]);
			}
			else if (str[i] == '(')
				oprend.push(str[i]);
			else if (str[i] == ')')
			{
				while (oprend.size())
				{
					tpoprend = oprend.top();
					if (tpoprend == '(')
						break;
					oprend.pop();
					if (optor.size() >= 2)
					{
						op1 = optor.top();
						optor.pop();
						op2 = optor.top();
						optor.pop();
						if (tpoprend == '*')
							optor.push(op1*op2);
						else if (tpoprend == '/')
							optor.push(op2 / op1);
						else if (tpoprend == '+')
							optor.push(op1 + op2);
						else if (tpoprend == '-')
							optor.push(op2 - op1);
					}
				 else if (optor.size() == 1)
					{
						op1 = 0;
						op2 = optor.top();
						optor.pop();
						if (tpoprend == '+')
							optor.push(op1 + op2);
						else if (tpoprend == '-')
							optor.push(op2 - op1);
					}
				}
				if (optor.top() == '(')
					optor.pop();
			}
		}
		i++;
	}
	while (oprend.size())
	{
		tpoprend = oprend.top();
		oprend.pop();
		if (optor.size() >= 2)
		{
			op1 = optor.top();
			optor.pop();
			op2 = optor.top();
			optor.pop();
			if (tpoprend == '*')
				optor.push(op1*op2);
			else if (tpoprend == '/')
				optor.push(op2 / op1);
			else if (tpoprend == '+')
				optor.push(op1 + op2);
			else if (tpoprend == '-')
				optor.push(op2 - op1);
		}
		else if (optor.size() == 1)
		{
			op1 = 0;
			op2 = optor.top();
			optor.pop();
			if (tpoprend == '+')
				optor.push(op1 + op2);
			else if (tpoprend == '-')
				optor.push(op2 - op1);
		}
	}
	if (optor.size()){
		num = optor.top();
		optor.pop();
	}
	else num = 0;
	stringstream sstr;
	sstr << num;
	return sstr.str();
}

void ConverToBinary::ConvertToBinary()
{
	vector<string> aline,pseudo;
	string word;
	dword instr = 0;
	vector<vector<string>>::iterator it;
	int line = PC;
	for (it = asmtext.begin(); it != asmtext.end(); ++it)
	{
		aline = *it;
		line += 4;
		if (aline[0] == "abs")
		{
			if (aline.size() == 3)
			{
				stringstream sstr;
				sstr << "addu" << " $"<<aline[1]<<' ' << "$zero,$"<<aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				instruct.push_back(instr);
				pseudo.clear();
				line += 4;
				sstr.str("");
				sstr << "bgez" <<" $" <<aline[1]<<',' << '1';
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				instruct.push_back(instr);
				line += 4;
				pseudo.clear();
				sstr.str("");
				sstr << "sub" << " $"<<aline[1] << ",$zero,$" << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				//instruct.push_back(instr);
				pseudo.clear();
			}
			else instr = ERROR;
		}
		else if (aline[0] == "beqz")//equal to zero jump to label
		{
			if (aline.size() == 3)
			{
				stringstream sstr;
				sstr << "beq " << "$zero,$" << aline[1] << "," << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
			else instr = ERROR;
		}
		else if (aline[0] == "bge")//bge rs,rt,l,great or equal jump to label
		{
			if (aline.size() == 4)
			{
				stringstream sstr;
				sstr << "slt $at,$" << aline[1] << ",$" << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				instruct.push_back(instr);
				pseudo.clear();
				line += 4;
				sstr.str("");
				sstr << "beq $at,$zero," << aline[3];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
			else instr = ERROR;
		}
		else if (aline[0] == "bgt")//bgt rs,rt,l if(rs>rt) jump to L
		{
			if (aline.size() == 4)
			{
				stringstream sstr;
				sstr << "slt $at,$" << aline[2] << ",$" << aline[1];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				instruct.push_back(instr);
				pseudo.clear();
				line += 4;
				sstr.str("");
				sstr << "bne $at,$zero," << aline[3];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
			else instr = ERROR;
		}
		else if (aline[0] == "ble") // ble rs,rt,l if(rs<=rt) jump to l
		{
			if (aline.size() == 4)
			{
				stringstream sstr;
				sstr << "slt $at,$" << aline[2] << ",$" << aline[1];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				instruct.push_back(instr);
				pseudo.clear();
				line += 4;
				sstr.str("");
				sstr << "beq $at,$zero," << aline[3];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
		}
		else if (aline[0] == "blt")//blt rs,rt,l if(rs<rt) jump to l
		{
			if (aline.size() == 4)
			{
				stringstream sstr;
				sstr << "slt $at,$" << aline[1] << ",$" << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				instruct.push_back(instr);
				pseudo.clear();
				line += 4;
				sstr.str("");
				sstr << "bne $at,$zero," << aline[3];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
		}
		else if (aline[0] == "bnez")//bnez rs,l if(rs!=0) jump to l
		{
			if (aline.size() == 3)
			{
				stringstream sstr;
				sstr << "bne " << "$zero,$" << aline[1] << "," << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
			else instr = ERROR;
		}
        else if (aline[0] == "lb")//lb rd,imm(rs) load address
		{
			if (aline.size() == 4)
			{
				stringstream sstr;
				sstr << "addi $" <<aline[1]<< ",$" << aline[3] << "," << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
			else instr = ERROR;
		}
        else if(aline[0] == "la")//la rt,label
        {
            if(aline.size() == 3){
                stringstream sstr;
                sstr << "lui $at," << ((table[aline[2]]&0xffff0000)>>16);
                pseudo = AStrToVector(sstr.str(), line);
                instr = ConvertAInstr(pseudo, line);
                line += 4;
                instruct.push_back(instr);
                pseudo.clear();
                sstr.str("");
                sstr << "ori $" << aline[1] << ",$at," << (table[aline[2]]&0x0000ffff);
                pseudo = AStrToVector(sstr.str(), line);
                instr = ConvertAInstr(pseudo, line);
                pseudo.clear();
            }
        }
		else if (aline[0] == "li")//li rd,imm
		{
			if (aline.size() == 3)
			{
				stringstream sstr;
                sstr << "lui $at," << ((StrTo2Comp(aline[2])&0xffff0000)>>16);
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				line += 4;
				instruct.push_back(instr);
				pseudo.clear();
				sstr.str("");
                sstr << "ori $" << aline[1] << ",$at," << (StrTo2Comp(aline[2])&0x0000ffff);
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
			else instr = ERROR;
		}
		else if (aline[0] == "move")//move rd,rs rd = rs
		{
			if (aline.size() == 3)
			{
				stringstream sstr;
				sstr << "add $" << aline[1] << ",$zero,$" << aline[2];
				pseudo = AStrToVector(sstr.str(), line);
				instr = ConvertAInstr(pseudo, line);
				pseudo.clear();
			}
		}
		else instr = ConvertAInstr(aline, line);
		instruct.push_back(instr);
	}
}

dword ConverToBinary::itoc(int m)
{
	if (m<0) m = 0x10000 + m;
	return m;
}

dword ConverToBinary::StrTo2Comp(string str)
{
	dword num = 0;
	int i = 0;
	bool flag = false;
	if (str[0] == '-')
	{
		flag = true;
		i++;
	}
	while (i<str.size())
	{
		num = num * 10 + str[i] - '0';
		i++;
	}
	if (flag)
		num = 0x10000 - num;
	return num;
}

int ConverToBinary::StrToUnsigned(string str)
{
	int num = 0;
	int i = 0;
	bool flag = false;
	if (str[0] == '-')
	{
		flag = true;
		i++;
	}
	while (i<str.size())
		num = num * 10 + str[i] - '0';
	return num;
}

ostream& operator<<(ostream& os, ConverToBinary& c)
{
	dword aline;
	dword mask = 0;
	for (int i = 0; i < c.instruct.size(); i++)
	{
		mask = 0x80000000;
		aline = c.instruct[i];
		os << c.PC << "; ";
		for (int j = 0; j < 32; j++)
		{
			if (aline&mask)
				os << '1';
			else os << '0';
			if ((j + 1) % 8 == 0)
				os << ' ';
		}
		os << endl;
	}
	return os;
}
