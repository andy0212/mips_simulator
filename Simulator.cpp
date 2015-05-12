#include"Simulator.h"
#include<sstream>
#include <iomanip>
Simulator::Simulator()
{
	for (int i = 0; i < 32; i++)
		reg[i] = 0;
	PC = 0;
    //consolechanged = false;
    DataAdd = 64;
	for (auto &item : memory)
		item = 0;
}


int Simulator::CurrentPC(){
    return PC;
}

void Simulator::InsertToSim(string asmstring, string binarystring){
    vector<pair<unsigned long, string>> instrset;
    pair<unsigned long, string> insert;
    int i = 0;
    string line,firstword,secondword;
    istringstream read(asmstring);
    vector<string> dataset;
    while (getline(read,line))
    {
        istringstream readline(line);
        if(readline >> firstword)
            if(firstword[0] == ';'||firstword[0] == '.')
            {
                dataset.push_back(line);
                continue;
            }
            else if(readline>>secondword)
                if(secondword[0] == '.'){
                    dataset.push_back(line);
                    continue;
                }
        else break;
    }
    ReceiveData(dataset);
    conver = new ConverToBinary(asmstring,PC);
    map<string,unsigned long>::iterator it;
    for(it = data.begin();it!=data.end();it++)
        if(conver->table.find((*it).first)!= conver->table.end())
            conver->table[(*it).first] = (*it).second;
    conver->ConvertToBinary();
    vector<unsigned long> binarydata = conver->outputInt();
    string binarytxt = conver->output();
    btoa = new BinaryToAsm(binarytxt,PC);
    btoa->ConvertToAsm();
    string asmtxt = btoa->output();
    for(i = 0;i<binarydata.size();i++){
        insert.first = binarydata[i];
        instrset.push_back(insert);
    }
    read.clear();
    read.str(asmtxt);
    int j = 0;
    while (getline(read,line))
    {
        istringstream readline(line);
        if(j<instrset.size())
             instrset[j++].second = line;
    }
   ReceiveInstr(instrset);
}

vector<string> Simulator::outputonebyone(){
    vector<string> output;
    stringstream sstr;
    for (int i = 0; i < buffer.size();i++)
    {
        if (i == (PC >> 2))
            sstr << '>' << '[' << hex << setw(8) << setfill('0') << i * 4 << ']' << "	" << hex << setw(8) << setfill('0') << memory[i] << " " << buffer[i] << endl;
        else sstr <<' ' <<'[' << hex << setw(8) << setfill('0') << i * 4 << ']' << "	" << hex << setw(8) << setfill('0') << memory[i] << " " << buffer[i] << endl;
        output.push_back(sstr.str());
        sstr.str("");
    }
    return output;
}

string Simulator::outputSim()
{
	stringstream sstr;
	for (int i = 0; i < buffer.size();i++)
	{
		if (i == (PC >> 2))
			sstr << '>' << '[' << hex << setw(8) << setfill('0') << i * 4 << ']' << "	" << hex << setw(8) << setfill('0') << memory[i] << " " << buffer[i] << endl;
		else sstr <<' ' <<'[' << hex << setw(8) << setfill('0') << i * 4 << ']' << "	" << hex << setw(8) << setfill('0') << memory[i] << " " << buffer[i] << endl;
	}
    /*
	for (int i = buffer.size(); i < SIZE;i++)
		if (i == (PC >> 2))
			sstr << '>' << '[' << hex << setw(8) << setfill('0') << i * 4 << ']' << "	" << hex << setw(8) << setfill('0') << memory[i] << endl;
		else sstr << ' '<<'[' << hex << setw(8) << setfill('0') << i*4 << ']' << "	" << hex << setw(8) << setfill('0') << memory[i]<<endl;
*/
    return sstr.str();
}

string Simulator::outputdata(){
    stringstream sstr;
    int32_t mask;
    for (int i = buffer.size(); i < SIZE;i++){
        sstr << ' '<<'[' << hex << setw(8) << setfill('0') << i*4 << ']' << "	"<< hex << setw(8) << setfill('0') << memory[i]<<"  ";
        mask = 0xff000000;
        for(int j = 0;j<4;j++,mask >>= 8)
            if((mask&memory[i]) == 0)
                sstr<<" ·";
            else sstr << ' '<<(char)((mask&memory[i])>>(3-j)*8);
        sstr << endl;
    }
    return sstr.str();
}

string Simulator::outputReg()
{
	stringstream sstr;
	sstr << "$zero = " << reg[0] << "	$at = " << reg[1] << "	$v0 = " << reg[2] << "	$v1 = " << reg[3];
	sstr << "	$a0 = " << reg[4] << "	$a1 = " << reg[5] << "	$a2 = " << reg[6] << "	$a3 = " << reg[7];
	for (int i = 0; i < 8; i++)
		sstr << "	$t" << i << " = " << reg[i + 8];
	for (int i = 0; i < 8; i++)
		sstr << "	$s" << i << " = " << reg[i + 16];
	sstr << "	$t8 = " << reg[24] << "	$t9 = " << reg[25] << "	$k0 = " << reg[26] << "	$k1 = " << reg[27];
	sstr << "	$gp = " << reg[28] << "	$sp = " << reg[29] << "	$fp = " << reg[30] << "	$ra = " << reg[31];
	return sstr.str();
}

void Simulator::ReceiveInstr(vector<pair<unsigned long, string>> instrset)
{
    int32_t currentPC = PC;
    for (auto instr : instrset)
		ReceiveInstr(instr);
    PC = currentPC;
}

void Simulator::ReceiveInstr(pair<unsigned long, string> instr)
{
	//pair<unsigned long, unsigned long> insert;
	//insert.first = instr.first;
	//insert.second = PC;
	instruct[PC] = instr.first;
	memory[PC >> 2] = instr.first;
	//stringstream sstr;
	//sstr << '[' << hex << setw(8) << setfill('0') << PC << ']' << "	" << hex << setw(8) << setfill('0') << instr.first << "	" << instr.second;
	PC += 4;
	buffer.push_back(instr.second);
}

void Simulator::ReceiveData(vector<string> dataset){
    string word,type;
    for(int i = 0;i<dataset.size();i++){
        istringstream read(dataset[i]);
        read >> word;
        if(word == ".data"||word == ".text")
            continue;
        word.erase(word.size()-1, 1);
        data[word] = DataAdd;
        read >> type;
        bool complete = false;
        if(type == ".2byte"){
            int16_t value;
            int32_t record = 0;
            char ch;
            while(read >> value){
                if(complete == false){
                    record = value << 16;
                    complete = true;
                }
                else {
                    record |= value;
                    memory[DataAdd>>2] = record;
                    DataAdd += 4;
                    record = 0;
                    complete = false;
                }
                read >> ch;
            }
            if(complete == false)
            {
                memory[DataAdd>>2] = record;
                DataAdd += 4;
            }
        }
        else if(type == ".4byte"){
            int32_t record;
            char ch;
            while(read >> record){
                memory[DataAdd>>2] = record;
                DataAdd += 4;
                read >> ch;
            }
        }
        else if(type == ".8byte"){
            int64_t record;
            char ch;
            while (read >> record){
                memory[DataAdd>>2] = (record & 0xffffffff00000000)>>32;
                DataAdd += 4;
                memory[DataAdd>>2] = record & 0x00000000ffffffff;
            }
        }
        else if(type == ".ascii"){
            string list;
            getline(read,list);
            int8_t ch;
            int count = 0;
            int i = 0;
            int32_t record = 0;
            while(i<list.size()&&list[i] != '\"'){
                i++;
            }
            i++;
            while(i<list.size()){
                if(list[i] == '\"'&&list[i-1]!='\\')
                        break;
                if(list[i] == '\\' && list[i] != '\\')
                   continue;
                ch = list[i];
                count++;
                record |= ch << (4 - count)*8;
                if(count == 4){
                    memory[DataAdd>>2] = record;
                    DataAdd += 4;
                    record = 0;
                    count = 0;
                }
                i++;
            }
            if(count != 0){
                memory[DataAdd>>2] = record;
                DataAdd += 4;
            }
        }
        else if(type == ".asciiz"){
            string list;
            getline(read,list);
            int8_t ch;
            int count = 0;
            int i = 0;
            int32_t record = 0;
            while(i<list.size()&&list[i] != '\"'){
                i++;
            }
            i++;
            while(i<list.size()){
                if(list[i] == '\"'&&list[i-1]!='\\')
                        break;
                if(list[i] == '\\' && list[i] != '\\')
                   continue;
                ch = list[i];
                count++;
                record |= ch << (4 - count)*8;
                if(count == 4){
                    memory[DataAdd>>2] = record;
                    DataAdd += 4;
                    record = 0;
                    count = 0;
                }
                i++;
            }
            record |= 0 << (4 - count)*8;
            memory[DataAdd>>2] = record;
            DataAdd += 4;
        }
        else if(type == ".byte"){
            string list;
            getline(read,list);
            int i,j;
            i = j = 0;
            int8_t ch;
            int count = 0;
            int32_t record = 0;
            bool flag = false;
            while(i<list.size()&&list[i] == ' '){
                i++;
            }
            while(i<list.size()){
                if(list[i] == '\''&&flag == false){
                    i++;
                    ch = list[i];
                    i += 2;
                }
                else if(list[i] == '\"'&&flag == false){
                    flag = true;
                    i++;
                    continue;
                }
                else if(list[i] == '\"'&&flag == true){
                    flag = false;
                    i += 2;
                    continue;
                }
                else if(flag == true)
                    ch = list[i];
                else{
                    ch = 0;
                    while(i<list.size()&&list[i]!=','){
                        ch = ch * 10 + list[i] - '0';
                        i++;
                    }
                }
                count++;
                record |= ch << (4 - count)*8;
                if(count == 4){
                    memory[DataAdd>>2] = record;
                    DataAdd += 4;
                    record = 0;
                    count = 0;
                }
                i++;
            }
            if(count != 0){
                memory[DataAdd>>2] = record;
                DataAdd += 4;
            }
        }
        else if(type == ".float"){
            float num;
            int32_t record;
            char ch;
            while(read >> num){
                record = (int32_t) num;
                memory[DataAdd>>2] = record;
                DataAdd += 4;
                record = 0;
               read >> ch;
            }
        }
        else if(type == ".word"){
            int32_t record = 0;
            char ch;
            while(read >> record){
                memory[DataAdd>>2] = record;
                DataAdd += 4;
                record = 0;
               read >> ch;
            }
        }
        else if(type == ".space"){
            int num;
            read >> num;
            DataAdd += 4*(num%4 + 1);
        }
        else if(type == ".double"){
            int32_t record = 0;
            double num;
            int64_t dnum;
            char ch;
            while(read >> num){
                dnum = (int64_t) num;
                record = (dnum&0xffffffff00000000) >> 32;
                memory[DataAdd>>2] = record;
                DataAdd += 4;
                record = (dnum&0x00000000ffffffff);
                memory[DataAdd>>2] = record;
                DataAdd += 4;
                read >> ch;
            }
        }
    }
}

void Simulator::clear()
{
	buffer.clear();
	PC = 0;
	instruct.clear();
	for (auto &item : memory)
		item = 0;
	for (auto &item : reg)
		item = 0;
    inputconsole.clear();
}

void Simulator::SingleStep()
{
    if (instruct.find(PC) == instruct.end()){
        inputconsole += "\n==========finish run===========\n";
        return;
    }
    stringstream sstr;
	unsigned long instr = instruct[PC],operator_type,operator_code;
	PC += 4;
	operator_type = (instr & 0xfc000000) >> 26;
	if (operator_type == 0)
	{
		unsigned int rs, rt, rd,sft;
		rs = (instr & 0x03e00000) >> 21;
		rt = (instr & 0x001f0000) >> 16;
		rd = (instr & 0x0000f800) >> 11;
		operator_code = instr & 0x0000003f;
		sft = (instr & 0x0000007c0) >> 6;
		switch (operator_code)
		{
		case 0x20://add
			reg[rd] = reg[rs] + reg[rt];
			break;
		case 0x24://and
			reg[rd] = reg[rs] & reg[rt];
			break;
		case 9://jalr rs,rd rd = PC+4,PC = rs
			reg[rd] = PC;
			PC = reg[rs];
			break;
		case 8://jr rs PC = rs
			PC = reg[rs];
			break;
		case 0x27://nor rd,rs,rt
			reg[rd] = ~(reg[rs] | reg[rt]);
			break;
		case 0://sll rd,rt,sft
			reg[rd] = reg[rt] << sft;
			break;
		case 4://sllv,rd,rs,rt
			reg[rd] = reg[rt] << reg[rs];
			break;
		case 0x2a://slt rd,rs,rt
			reg[rd] = reg[rs] < reg[rt] ? 1 : 0;
			break;
		case 0x2b://sltu rd,rs,rt
			reg[rd] = static_cast<unsigned long>(reg[rs])<static_cast<unsigned long>(reg[rt]) ? 1 : 0;
			break;
		case 3://sra rd,rt,sft 
			reg[rd] = reg[rt] >> sft;
			break;
		case 7://srav rd,rs,rt
			reg[rd] = reg[rt] >> reg[rs];
			break;
		case 2://srl rd,rt,sft
			reg[rd] = static_cast<unsigned long>(reg[rt]) >> sft;
			break;
		case 6://srlv rd,rs,rt
			reg[rd] = static_cast<unsigned long>(reg[rt]) >> reg[rs];
			break;
		case 0x22://sub rd,rs,rt
			reg[rd] = reg[rs] - reg[rt];
			break;
		case 0x26://xor rd,rs,rt
			reg[rd] = reg[rs] ^ reg[rt];
			break;
        case 0xc://syscall
           {
                int type = reg[2];
                switch(type){
                case 4://print_string
                    {
                        int32_t address = reg[4];
                        int32_t record = memory[address>>2],mask = 0xff000000;
                        char ch;
                        int count = 0;
                        while(1){
                            if(count == 4){
                                mask = 0xff000000;
                                address += 4;
                                record = memory[address>>2];
                                count = 0;
                            }
                            if((mask&record) == 0)
                                break;
                            count++;
                            ch = (mask&record)>>(4-count)*8;
                            sstr<<ch;
                            mask >>= 8;
                        }
                        inputconsole += sstr.str();
                        //consolechanged = true;
                    }
                }
            }
        }
	}
	else if (operator_type == 2 || operator_type == 3)//j jal
	{
		unsigned long address = instr&0x07ffffff;
		if (operator_type == 3)
			reg[31] = PC;
		address <<= 2;
		address |= (PC & 0xf000000);
		PC = address;
	}
	else//i type
	{
		unsigned int rs, rt;
		rs = (instr & 0x03e00000) >> 21;
		rt = (instr & 0x001f0000) >> 16;
        int16_t immediate = instr & 0x0000ffff;
		switch (operator_type)
		{
		case 8://addi rt,rs,imm
			reg[rt] = reg[rs] + immediate;
			break;
		case 0x0c://andi
			reg[rt] = reg[rs] & immediate;
			break;
		case 4://beq
			PC += reg[rs] == reg[rt] ? (immediate << 2) : 0;
			break;
		case 1://bgez,bgezal,bltz,bltzal
			if (rt == 1)
				PC += reg[rs] >= 0 ? (immediate << 2) : 0;
			else if (rt == 17)
			{
				reg[31] = PC;
				PC += reg[rs] >= 0 ? (immediate << 2) : 0;
			}
			else if (rt == 0)
				PC += reg[rs] < 0 ? (immediate << 2) : 0;
			else if (rt == 16)
			{
				reg[31] = PC;
				PC += reg[rs] < 0 ? (immediate << 2) : 0;
			}
			break;
		case 7://bgtz
			PC += reg[rs] > 0 ? (immediate << 2) : 0;
			break;
		case 6://blez
			PC += reg[rs] <= 0 ? (immediate << 2) : 0;
			break;
		case 5://bne
			PC += reg[rs] != reg[rt] ? (immediate<<2) : 0;
			break;
		case 0x23://lw rt,imm(rs)
			if (((rs + immediate) >> 2)<SIZE)
			reg[rt] = memory[(rs + immediate) >> 2];
			break;
		case 0x0a://slti
			reg[rt] = reg[rs] < immediate ? 1 : 0;
			break;
		case 0x0b://sltiu
            reg[rt] = reg[rs] < (int16_t)immediate ? 1 : 0;
			break;
		case 0x2b://sw
			if (((rs + immediate) >> 2) < SIZE)
				memory[(rs + immediate) >> 2] = reg[rt];
			break;
		case 0x0e://xori
			reg[rt] = reg[rs] ^ immediate;
            break;
        case 0x0d://ori
            reg[rt] = reg[rs] | immediate;
            break;
        case 0x0f://lui
            reg[rt] = immediate<< 16;
            break;
		}
	}
}

bool Simulator::Run()
{
	int count = 0;
    inputconsole.clear();
	while (memory[PC>>2] != 0&&count<10000){
		SingleStep();
		count++;
	}
    if(memory[PC>>2] == 0)
        inputconsole += "\n==========finish run===========\n";
	if (count == 10000)
		return false;
	return true;
}
