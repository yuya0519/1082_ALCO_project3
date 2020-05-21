#include<iostream>
#include<string>
//#include<cstring>
//#include<sstream>
#include<vector>
#include <cstdlib>
using namespace std;
vector<string> inputall, label;
vector<int>lbnum; 
int entry = 8;
int reg[31];
string** bht;
int current = 0;//當前pc
int* miss;
int state(string history) {//對照
	if (history == "00")
		return 1;
	if (history == "01")
		return 2;
	if (history == "10")
		return 3;
	if (history == "11")
		return 4;
}

int findoffset(string str) {//找label位置
	for (int i = 0; i < label.size(); ++i) {
		if (str == label[i]) {
			return lbnum[i];
		}
	}
}


void rType(string input, int pc)
{
	string fun7, fun3, op, str[4];
	int p,rs2,rs1,rd;
	p = input.find(" ", 0);
	str[0] = input.substr(0, p);
	input = input.substr(p + 1, input.length());
	for (int i = 1; i < 4; i++) {
		p = input.find(",", 0);
		str[i] = input.substr(1, p);
		input = input.substr(p + 1, input.length());
	}
	rs2 = atoi(str[3].c_str());
	rs1 = atoi(str[2].c_str());
	rd = atoi(str[1].c_str());
	if (str[0] == "sub")
		reg[rd]= reg[rs1] - reg[rs2];
	else if(str[0]=="add")
		reg[rd] = reg[rs1] + reg[rs2];
}

void iType(string input, int pc)
{
	string fun3, op, str[4];
	int p, imm, rs1, rd;
	p = input.find(" ", 0);
	str[0] = input.substr(0, p);
	input = input.substr(p + 1, input.length());
	if (str[0] == "li") {                              //li
		p = input.find(",", 0);
		str[1] = input.substr(1, p-1);
		input = input.substr(p + 1, input.length());
		str[2] = input.substr(0, input.length());
		rs1 = atoi(str[2].c_str());
		rd = atoi(str[1].c_str());
		reg[rd] = rs1;
	}
	else if(str[0] == "addi") {
		for (int i = 1; i < 3; i++) {
			p = input.find(",", 0);
			str[i] = input.substr(1, p);
			input = input.substr(p + 1, input.length());
		}
		str[3] = input.substr(0, input.length());
		imm = atoi(str[3].c_str());
		rs1 = atoi(str[2].c_str());
		rd = atoi(str[1].c_str());
		reg[rd] = reg[rs1] + imm;
	}
}


void bType(string input, int pc)
{
	string  fun3, op, str[4], i12, i10, i4, i11;
	int p,imm, rs2, rs1,sta;
	p = input.find(" ", 0);
	str[0] = input.substr(0, p);
	input = input.substr(p + 1, input.length());
	for (int i = 1; i < 3; i++) {
		p = input.find(",", 0);
		str[i] = input.substr(1, p-1);
		input = input.substr(p + 1, input.length());
	}
	str[3] = input.substr(0, input.length());
	if (str[3][str[3].length() - 1] == ' ') 
		str[3] = str[3].substr(0, str[3].length()-1);
	rs2 = atoi(str[2].c_str());
	rs1 = atoi(str[1].c_str());
	sta = state(bht[current % entry][0]);
	cout << "entry: " << current % entry << "         " << inputall[current] << endl;
	cout << "(" << bht[current % entry][0] << "," << bht[current % entry][1] << ","
		<< bht[current % entry][2] << "," << bht[current % entry][3] << "," << bht[current % entry][4] << ") ";
	if (str[0] == "beq") {
		if (reg[rs1] == reg[rs2])
		{
			if (bht[current % entry][sta][1] == 'N') {
				miss[current % entry]++;
				cout << "N "<<"T" << "                misprediction: " << miss[current % entry];			
				if (bht[current % entry][sta] == "SN") 
					bht[current % entry][sta] = "WN";
				else if (bht[current % entry][sta] == "WN") 
					bht[current % entry][sta] = "WT";			
			}
			else {
				if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "ST";
				cout << "T " << "T" << "                misprediction: " << miss[current % entry];
			}
			bht[current % entry][0] += '1';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
			current = findoffset(str[3]);
			current--;
		}
		else {
			if (bht[current % entry][sta][1] == 'N') {
				cout << "N " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "SN";
			}
			else
			{
				miss[current % entry]++;
				cout << "T " << "N" << "                misprediction: " << miss[current % entry];			
				if (bht[current % entry][sta] == "ST")
					bht[current % entry][sta] = "WT";
				else if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "WN";
			}
			bht[current % entry][0] += '0';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
		}
	}
	else if (str[0] == "bne") {
		if (reg[rs1] != reg[rs2])
		{
			if (bht[current % entry][sta][1] == 'N') {
				miss[current % entry]++;
				cout << "N " << "T" << "                misprediction: " << miss[current % entry];				
				if (bht[current % entry][sta] == "SN")
					bht[current % entry][sta] = "WN";
				else if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "WT";
			}
			else {
				if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "ST";
				cout << "T " << "T" << "                misprediction: " << miss[current % entry];
			}
			bht[current % entry][0] += '1';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
			current = findoffset(str[3]);
			current--;
		}
		else {
			if (bht[current % entry][sta][1] == 'N') {
				cout << "N " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "SN";
			}
			else
			{		
				miss[current % entry]++;
				cout << "T " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "ST")
					bht[current % entry][sta] = "WT";
				else if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "WN";
			}
			bht[current % entry][0] += '0';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
		}
	}
		
	else if (str[0] == "blt") {
		if (reg[rs1] < reg[rs2])
		{
			if (bht[current % entry][sta][1] == 'N') {
				miss[current % entry]++;
				cout << "N " << "T" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "SN")
					bht[current % entry][sta] = "WN";
				else if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "WT";
			}
			else {
				if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "ST";
				cout << "T " << "T" << "                misprediction: " << miss[current % entry];
			}
			bht[current % entry][0] += '1';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
			current = findoffset(str[3]);
			current--;
		}
		else {
			if (bht[current % entry][sta][1] == 'N') {
				cout << "N " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "SN";
			}
			else
			{			
				miss[current % entry]++;
				cout << "T " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "ST")
					bht[current % entry][sta] = "WT";
				else if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "WN";
			}
			bht[current % entry][0] += '0';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
		}
	}
	else if (str[0] == "bge") {
		if (reg[rs1] >= reg[rs2])
		{
			if (bht[current % entry][sta][1] == 'N') {
				miss[current % entry]++;
				cout << "N " << "T" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "SN")
					bht[current % entry][sta] = "WN";
				else if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "WT";
			}
			else {
				if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "ST";
				cout << "T " << "T" << "                misprediction: " << miss[current % entry];
			}
			bht[current % entry][0] += '1';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
			current = findoffset(str[3]);
			current--;
		}
		else {
			if (bht[current % entry][sta][1] == 'N') {
				cout << "N " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "WN")
					bht[current % entry][sta] = "SN";
			}
			else
			{
				miss[current % entry]++;
				cout << "T " << "N" << "                misprediction: " << miss[current % entry];
				if (bht[current % entry][sta] == "ST")
					bht[current % entry][sta] = "WT";
				else if (bht[current % entry][sta] == "WT")
					bht[current % entry][sta] = "WN";
			}
			bht[current % entry][0] += '0';
			bht[current % entry][0] = bht[current % entry][0].substr(1, 2);
		}
	}
	cout << endl << endl;
}
int main() {
	reg[0] = 0;
	string input;	
	int p, pc;
	getline(cin, input);
	int cou = 0;
	
	bht = new string*[entry];
	for (int i = 0; i < entry; ++i) 
		bht[i] = new string[5];
	for (int i = 0; i < entry; ++i){//初始
		bht[i][0] ="00";
		for (int j = 1; j < 5; ++j)
			bht[i][j] = "SN";
	}
	miss = new int[entry]();
	bool push = true;
	while (input[0] != cin.eof()) {
		push = true;
		p = input.find("//", 0); //解決註解
		if (p != input.npos) {
			input = input.substr(0, p);
			if (p == 0) {
				cou--;
				push = false;
			}
		}
		p = input.find(':', 0); //處理label
		if (p != input.npos) {
			label.push_back(input.substr(0, p));
			lbnum.push_back(cou);
			if (p + 1 < input.length()) {//若label與inst未分行
				input = input.substr(p + 1, input.length());
				if (input[0] == ' ')
					input = input.substr(1, input.length());
			}
			else {
				push = false;
				cou--;
			}
		}
		if(push)
			inputall.push_back(input);
		getline(cin, input);
		cou++;
	}
	while(current< inputall.size()){//執行
		if (inputall[current].find("ui", 0) != inputall[current].npos) {
			continue;
		}
		else if (inputall[current][0] == 'l' || inputall[current].find("i", 0) != inputall[current].npos || inputall[current].find("jalr", 0) != inputall[current].npos) {
			p = inputall[current].find(",", 0);
			if(inputall[current][p-1]=='0'&& inputall[current][p - 2]=='R')
				continue;
			iType(inputall[current], current);
		}
		else if (inputall[current][0] == 's' && inputall[current].find(" ", 0) == 2) {
			continue;
		}
		else if (inputall[current][0] == 'b') {
			bType(inputall[current], current);
		}
		else if (inputall[current].find("jal", 0) != inputall[current].npos) {
			continue;
		}
		else {
			p = inputall[current].find(",", 0);
			if (inputall[current][p - 1] == '0' && inputall[current][p - 2] == 'R')
				continue;
			rType(inputall[current], current);
		}
		current++;
	}
}