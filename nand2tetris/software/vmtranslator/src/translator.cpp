#include <bits/stdc++.h>

using namespace std;

void format(string& str);
string push(string command);
string pop(string command);
string unary(string type);
string binary(string type);
string comp(string type);

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " {inputfile} {outputfile}\n";
        return 1;
    }
    
    ifstream infile(argv[1]);
    ofstream outfile(argv[2]);

    if (!infile || !outfile) {
        cout << "Error opening files\n";
        return 2;
    }

    string line;

    while(getline(infile, line)) {
        format(line);
        if (line.size() == 0) continue;
        outfile << "//" << line << endl;

        string type;
        int i;
        for (i = 0; i < line.size(); i++) {
            if (line[i] == ' ') break;
            type += line[i];
        }
        
        string oplines;
        if (type == "push") oplines = push(line.substr(i+1));
        else if (type == "pop") oplines = pop(line.substr(i+1));
        else if (type == "neg" || type == "not") oplines = unary(type);
        else if (type == "eq" || type == "gt" || type == "lt") oplines = comp(type);
        else if (type == "add" || type == "sub" || type == "and" || type == "or") oplines = binary(type);
        
        outfile << oplines;
    }

    outfile << "(END)" << endl;
    outfile << "@END" << endl;
    outfile << "0; JMP" << endl;

    infile.close();
    outfile.close();
}

void format(string& str) {
    int i = 0;
    for (; i < str.size(); i++) {
        if (str[i] == '/') break;
    }
    str = str.substr(0, i - 1);
    return;
}

string push(string command) {
    string segment, strindex;
    string output;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (command[i] == ' ') break;
        segment += command[i];
    }
    for (i++; i < command.size(); i++) {
        if (command[i] == ' ') break;
        strindex += command[i];
    }

    if (segment == "constant") {
        output += "@ " + strindex + '\n';
        output += "D = A\n";
    }
    else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
        string segment_label;
        if (segment == "local") segment_label = "LCL";
        else if (segment == "argument") segment_label = "ARG";
        else if (segment == "this") segment_label = "THIS";
        else segment_label = "THAT";
        output += "@ " + segment_label + '\n';
        output += "D = M\n";
        output += "@ " + strindex + '\n';
        output += "A = D + A\n";
        output += "D = M\n";
    }
    else {
        string start_index;
        if (segment == "temp") start_index = "5";
        else if (segment == "static") start_index = "16";
        else start_index = "THIS";
        output += "@ " + start_index + '\n';
        output += "D = A\n";
        output += "@ " + strindex + '\n';
        output += "A = D + A\n";
        output += "D = M\n";
    }
    output += "@SP\n";
    output += "M = M + 1\n";
    output += "A = M - 1\n";
    output += "M = D\n";
    return output;
}

string pop(string command) {
    string segment, strindex;
    string output;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (command[i] == ' ') break;
        segment += command[i];
    }
    for (i++; i < command.size(); i++) {
        if (command[i] == ' ') break;
        strindex += command[i];
    }

    if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
        string segment_label;
        if (segment == "local") segment_label = "LCL";
        else if (segment == "argument") segment_label = "ARG";
        else if (segment == "this") segment_label = "THIS";
        else segment_label = "THAT";
        output += "@ " + segment_label + '\n';
        output += "D = M\n";
        output += "@ " + strindex + '\n';
        output += "D = D + A\n";
        output += "@R13\n";
        output += "M = D\n";
    }
    else {
        string start_index;
        if (segment == "temp") start_index = "5";
        else if (segment == "static") start_index = "16";
        else start_index = "THIS";
        output += "@ " + start_index + '\n';
        output += "D = A\n";
        output += "@ " + strindex + '\n';
        output += "D = D + A\n";
        output += "@R13\n";
        output += "M = D\n";
    }
    output += "@SP\n";
    output += "M = M - 1\n";
    output += "A = M\n";
    output += "D = M\n";
    output += "@R13\n";
    output += "A = M\n";
    output += "M = D\n";
    return output;
}

string unary(string type) {
    string output;
    output += "@SP\n";
    output += "A = M - 1\n";
    output += string("M = ") + ((type == "neg") ? "-" : "!") + "M\n";
    return output;
}

string binary(string type) {
    string output;
    output += "@SP\n";
    output += "M = M - 1\n";
    output += "A = M\n";
    output += "D = M\n";
    output += "A = A - 1\n";
    if (type == "sub") output += "M = M - D\n";
    else if (type == "add") output += "M = D + M\n";
    else if (type == "and") output += "M = D & M\n";
    else output += "M = D | M\n";
    return output;
}

string comp(string type) {
    static int jump_count = 0;
    string output;
    output += "@SP\n";
    output += "M = M - 1\n";
    output += "A = M\n";
    output += "D = M\n";
    output += "A = A - 1\n";
    output += "D = M - D\n";
    output += "@TRUE" + to_string(jump_count) + '\n';
    if (type == "eq") output += "D; JEQ\n";
    else if (type == "gt") output += "D; JGT\n";
    else output += "D; JLT\n";
    output += "@SP\n";
    output += "A = M - 1\n";
    output += "M = 0\n";
    output += "@END" + to_string(jump_count) + '\n';
    output += "0; JMP\n";
    output += "(TRUE" + to_string(jump_count) + ")\n";
    output += "@SP\n";
    output += "A = M - 1\n";
    output += "M = -1\n";
    output += "(END" + to_string(jump_count) + ")\n";
    jump_count++;
    return output;
}