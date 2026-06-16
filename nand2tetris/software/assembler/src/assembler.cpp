#include <bits/stdc++.h>

#define LINESIZE 1024

using namespace std;

map<string, int> Symbol_Table;
map<string, string> dest;
map<string, string> comp;
map<string, string> jump;

void initialize(void) {
    Symbol_Table["SP"] = 0;
    Symbol_Table["LCL"] = 1;
    Symbol_Table["ARG"] = 2;
    Symbol_Table["THIS"] = 3;
    Symbol_Table["THAT"] = 4;
    Symbol_Table["R0"] = 0;
    Symbol_Table["R1"] = 1;
    Symbol_Table["R2"] = 2;
    Symbol_Table["R3"] = 3;
    Symbol_Table["R4"] = 4;
    Symbol_Table["R5"] = 5;
    Symbol_Table["R6"] = 6;
    Symbol_Table["R7"] = 7;
    Symbol_Table["R8"] = 8;
    Symbol_Table["R9"] = 9;
    Symbol_Table["R10"] = 10;
    Symbol_Table["R11"] = 11;
    Symbol_Table["R12"] = 12;
    Symbol_Table["R13"] = 13;
    Symbol_Table["R14"] = 14;
    Symbol_Table["R15"] = 15;
    Symbol_Table["SCREEN"] = 0x4000;
    Symbol_Table["KBD"] = 0x6000;

    dest[""] = "000";
    dest["M"] = "001";
    dest["D"] = "010";
    dest["MD"] = "011";
    dest["A"] = "100";
    dest["AM"] = "101";
    dest["AD"] = "110";
    dest["ADM"] = "111";

    jump[""] = "000";
    jump["JGT"] = "001";
    jump["JEQ"] = "010";
    jump["JGE"] = "011";
    jump["JLT"] = "100";
    jump["JNE"] = "101";
    jump["JLE"] = "110";
    jump["JMP"] = "111";

    comp["0"] = "0101010";
    comp["1"] = "0111111";
    comp["-1"] = "0111010";
    comp["D"] = "0001100";
    comp["A"] = "0110000";
    comp["!D"] = "0001101";
    comp["!A"] = "0110001";
    comp["-D"] = "0001111";
    comp["-A"] = "0110011";
    comp["D+1"] = "0011111";
    comp["A+1"] = "0110111";
    comp["D-1"] = "0001110";
    comp["A-1"] = "0110010";
    comp["D+A"] = "0000010";
    comp["D-A"] = "0010011";
    comp["A-D"] = "0000111";
    comp["D&A"] = "0000000";
    comp["D|A"] = "0010101";
    comp["M"] = "1110000";
    comp["!M"] = "1110001";
    comp["-M"] = "1110011";
    comp["M+1"] = "1110111";
    comp["M-1"] = "1110010";
    comp["D+M"] = "1000010";
    comp["D-M"] = "1010011";
    comp["M-D"] = "1000111";
    comp["D&M"] = "1000000";
    comp["D|M"] = "1010101";
}

void format(string& str) {
    int count = 0, i = 0;
    for (i = 0; i < str.size(); i++) {
        if (str[i] == ' ') {
            count++;
            continue;
        }
        if (str[i] == '/') {
            break;
        }
        str[i-count] = str[i];
    }
    str = str.substr(0, i - count);
    return;
}

bool isnum(string str) {
    for (char x : str) if (!isdigit(x)) return false;
    return true;
}

string bin(int num) {
    string res;
    while(num > 0){
        if (num % 2) res = '1' + res;
        else res = '0' + res;
        num /= 2;
    }
    for (int i = 16 - res.size(); i > 0; i--) {
        res = '0' + res;
    }
    return res;
}

string a_instr(string value) {
    if (Symbol_Table.find(value) != Symbol_Table.end()) {
        return bin(Symbol_Table[value]);
    }
    else return bin(stoi(value));
}

string c_instr(string instruction) {
    string arr[3];
    int flag = 0;
    for (char x : instruction) {
        if (x == '=') {
            flag += 1;
        }
        else if (x == ';') {
            if (flag == 0) {
                arr[1] = arr[0];
                arr[0] = "";
            }
            flag += 2;
        }
        else {
            switch(flag) {
                case(0):
                    arr[0] += x;
                    break;
                case(1):
                    arr[1] += x;
                    break;
                default:
                    arr[2] += x;
            }
        }
    }

    arr[0] = dest[arr[0]];
    arr[1] = comp[arr[1]];
    arr[2] = jump[arr[2]];
    return "111" + arr[1] + arr[0] + arr[2];
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "USAGE: ./binary {inputfile} {outputfile}" << endl;
        return 1;
    }

    ifstream inputfile(argv[1]);
    ofstream outputfile(argv[2]);

    if (!inputfile || !outputfile) {
        cout << "Error processing files" << endl;
        return 2;
    }

    initialize();

    string line;
    int program_pointer = 0;
    while(getline(inputfile, line)) {
        format(line);
        if (line.size() == 0) continue;

        if (line[0] == '@') {
            string label = line.substr(1, line.size() - 1);
            
            if (!isnum(label) && Symbol_Table.find(label) == Symbol_Table.end()) {
                Symbol_Table[label] = -1;
            }
            program_pointer++;
        }

        else if (line[0] == '(') {
            string label = line.substr(1, line.size() - 2);
            Symbol_Table[label] = program_pointer;
        }
        
        else program_pointer++;
    }

    int memory_counter = 16;
    for (auto& pair : Symbol_Table) {
        if ((pair.second) == -1) pair.second = memory_counter++;
    }

    inputfile.clear();
    inputfile.seekg(0);

    while(getline(inputfile, line)) {
        format(line);

        if (line.size() == 0) continue;
        else if (line[0] == '(') continue;

        if (line[0] == '@') {
            outputfile << a_instr(line.substr(1,line.size()-1)) << endl;
        }
        else outputfile << c_instr(line) << endl;
    }

    inputfile.close();
    outputfile.close();
}

