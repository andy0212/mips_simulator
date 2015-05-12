#include"binarytoasm.h"
#include<sstream>

BinaryToAsm::BinaryToAsm(string str,int PC)
{
	this->PC = PC;
	PrepareData();
	target = 'A';
	int line = PC;
	string aline;
	dword instruct, operator_code,address,immet;
	int i = 0;
	while (i<str.size())
	{
		if (str[i] == '\n' || i == str.size() - 1)
		{
			if (i == str.size() - 1 && (str[i] == '0' || str[i] == '1'))
				aline += str[i];
			line += 4;
			instruct = StrToBinary(aline);
			aline.clear();
			binary.push_back(instruct);
			operator_code = (instruct & 0xfc000000)>>26;
			if (operator_code == 2 || operator_code == 3)
			{
				immet = instruct & 0x03ffffff;
				immet = immet << 2;
				address = ((line & 0xf0000000) << 28) | immet;
				table[address] = target;
				target++;
			}
			else if (operator_code == 4 || operator_code == 5)
			{
                int16_t immet = instruct & 0x0000ffff;
				immet <<= 2;

                table[int16_t(immet + line)] = target;
				target++;
			}
		}
		else if (str[i] == '0' || str[i] == '1')
			aline += str[i];
			i++;
	}
}

void BinaryToAsm::ConvertToAsm()
{
	int line = PC;
	int i = 0;
	dword instruct, operator_type, operator_code;
	dword rs, rt, rd, sa, immet,address;
	string aline;
	for (i = 0; i < binary.size(); i++)
	{
		instruct = binary[i];
		if (table.find(line) != table.end())
			aline = aline + table[line] + ": ";
		line += 4;
		operator_type = (instruct & 0xfc000000)>>26;
		if (operator_type == 0)
		{
			operator_code = instruct & 0x0000003f;
			if (operator_code == 32 || operator_code == 33 || operator_code == 36 || operator_code == 39
				|| operator_code == 37 || operator_code == 42 || operator_code == 43 || operator_code == 34
				|| operator_code == 35 || operator_code == 38) //rd,rs,rt
			{
				aline += Rinstr[operator_code] + ' ';
				rs = (instruct & 0x03e00000)>>21;
				rt = (instruct & 0x001f0000)>>16;
				rd = (instruct & 0x0000f800)>>11;
				aline += reg[rd] + ',' + reg[rs] + ',' + reg[rt];
			}
			else if (operator_code == 0||operator_code == 3||operator_code == 2)//rd,rs,sa
			{
				aline += Rinstr[operator_code] + ' ';
				rt = (instruct & 0x001f0000) >> 16;
				rd = (instruct & 0x0000f800) >> 11;
				sa = instruct & 0x000007c0;
				stringstream sstr;
				sstr << reg[rd] << ',' << reg[rt] << ',' << sa;
				aline += sstr.str();
			}
			else if (operator_code== 26||operator_code == 27||operator_code == 24||operator_code == 25)//rs,rt
			{
				aline += Rinstr[operator_code] + ' ';
				rs = (instruct & 0x03e00000)>>21;
				rt = (instruct & 0x001f0000)>>16;
				aline += reg[rs] + ',' + reg[rt];
			}
            else if(operator_code == 12)//syscall
                aline += Rinstr[operator_code];
		}
		else if (operator_type == 2 || operator_type == 3)
		{
			aline += Jinstr[operator_type] + ' ';
			immet = instruct & 0x03ffffff;
			immet = immet << 2;
			address = ((line & 0xf0000000) << 28) | immet;
			aline += table[address];
		}
		else
		{
			if (operator_type == 8||operator_type == 9||operator_type == 12||operator_type == 13
				||operator_type == 10||operator_type == 11||operator_type == 14)//rt,rs,immediate
			{
				aline += Iinstr[operator_type] + ' ';
				rs = (instruct & 0x03e00000)>>21;
				rt = (instruct & 0x001f0000)>>16;
				int immediate = instruct & 0x0000ffff;
				if (immediate & 0x00008000)//is negetive
				{
					immediate = 0x00010000 - immediate;
					immediate = -immediate;
				}
				stringstream sstr;
				sstr << reg[rt] << ',' << reg[rs] << ',' << immediate;
				aline += sstr.str();
			}
			else if (operator_type == 5||operator_type == 4)//beq,bne
			{
				aline += Iinstr[operator_type] + ' ';
				rs = (instruct & 0x03e00000)>>21;
				rt = (instruct & 0x001f0000)>>16;
                int16_t immet = instruct & 0x0000ffff;
				immet <<= 2;
                aline += reg[rs] + ',' + reg[rt] + ',' + table[int16_t(immet + line)];
			}
			else if (operator_type  == 32||operator_type == 36||operator_type == 33||operator_type == 37
				||operator_type ==35 ||operator_type == 40||operator_type== 41||operator_type == 43)//rt,immediate(rs)
			{
				aline += Iinstr[operator_type] + ' ';
				rs = (instruct & 0x03e00000)>>21;
				rt = (instruct & 0x001f0000)>>16;
				int immediate = instruct & 0x0000ffff;
				if (immediate & 0x00008000)//is negetive
				{
					immediate = 0x00010000 - immediate;
					immediate = -immediate;
				}
				stringstream sstr;
				sstr << reg[rt] << ',' << immediate << '(' << reg[rs] << ')';
				aline += sstr.str();
			}
            else if(operator_type == 15)//rt,immediate
            {
                aline += Iinstr[operator_type] + ' ';
                rt = (instruct & 0x001f0000)>>16;
                int immediate = instruct & 0x0000ffff;
                if (immediate & 0x00008000)//is negetive
                {
                    immediate = 0x00010000 - immediate;
                    immediate = -immediate;
                }
                stringstream sstr;
                sstr <<reg[rt]<<','<<immediate;
                aline += sstr.str();
            }
		}
		//aline += '\n';
		pair<dword, string> insert;
		insert.first = instruct;
		insert.second = aline;
		BinaryandAsm.push_back(insert);
		instr.push_back(aline);
		aline.clear();
	}
}

string BinaryToAsm::output()
{
	int line = PC;
	stringstream sstr;
	for (int i = 0; i < instr.size(); i++, line += 4)
		sstr << instr[i] << endl;
	return sstr.str();
}

void BinaryToAsm::PrepareData()
{
	Rinstr[32] = "add";
	Rinstr[33] = "addu";
	Rinstr[36] = "and";
	Rinstr[13] = "break";
	Rinstr[26] = "div";
	Rinstr[27] = "divu";
	Rinstr[9] = "jalr";
	Rinstr[8] = "jr";
	Rinstr[16] = "mfhi";
	Rinstr[18] = "mflo";
	Rinstr[17] = "mthi";
	Rinstr[19] = "mtlo";
	Rinstr[24] = "mtlu";
	Rinstr[39] = "nor";
	Rinstr[37] = "or";
	Rinstr[0] = "sll";
	Rinstr[4] = "sllv";
	Rinstr[42] = "slt";
	Rinstr[43] = "sltu";
	Rinstr[3] = "sra";
	Rinstr[7] = "srav";
	Rinstr[2] = "srl";
	Rinstr[6] = "srlv";
    Rinstr[12] = "syscall";

	Iinstr[8] = "addi";
	Iinstr[9] = "addiu";
	Iinstr[12] = "andi";
	Iinstr[4] = "beq";
	Iinstr[1] = "beqz";//bltz
	Iinstr[7] = "bgtz";
	Iinstr[6] = "blez";
	Iinstr[5] = "bne";
	Iinstr[32] = "lb";
	Iinstr[36] = "lbu";
	Iinstr[33] = "lh";
	Iinstr[37] = "lhu";
	Iinstr[15] = "lui";
	Iinstr[35] = "lw";
	Iinstr[43] = "sw";
	Iinstr[13] = "ori";
	Iinstr[40] = "sb";

	Jinstr[2] = "j";
	Jinstr[3] = "jal";

	reg[0] = "$zero";
	reg[1] = "$at";
	reg[2] = "$v0";
	reg[3] = "$v1";
	reg[4] = "$a0";
	reg[5] = "$a1";
	reg[6] = "$a2";
	reg[7] = "$a3";

	stringstream sstr;
	for (int i = 0; i <= 7; i++)
	{
		sstr.str("");
		sstr << "$t" << i;
		reg[i + 8] = sstr.str();
	}

	for (int i = 0; i <= 7; i++)
	{
		sstr.str("");
		sstr << "$s" << i;
		reg[i + 16] = sstr.str();
	}

	reg[24] = "$t8";
	reg[25] = "$t9";
	reg[26] = "$k0";
	reg[27] = "$k1";
	reg[28] = "$gp";
	reg[29] = "$sp";
	reg[30] = "$fp";
	reg[31] = "$ra";
}

BinaryToAsm::dword BinaryToAsm::StrToBinary(string str)
{
	dword num = 0;
	int i = 0;
	while (i < str.size()){
		num = num * 2 + str[i] - '0';
		i++;
	}
	return num;
}
