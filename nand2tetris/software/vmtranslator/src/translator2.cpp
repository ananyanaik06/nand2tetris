#include <bits/stdc++.h>

using namespace std;

void format(string& str);
string push(string command);
string pop(string command);
string unary(string type);
string binary(string type);
string comp(string type);
string label(string command);
string go(string command);
string ifgo(string command);
string func(string command);
string call(string command);
string ret();
void parse(string filename);

ofstream outfile;
string current_filename;

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " {inputfile} {outputfile}\n";
        return 1;
    }

    std::filesystem::path inputPath(argv[1]);
    outfile.open(argv[2]);

    if (!outfile || !std::filesystem::exists(inputPath)) {
        cout << "Error opening files\n";
        return 2;
    }

    if (std::filesystem::is_regular_file(inputPath)) {
        if (inputPath.extension() == ".vm") {
            parse(inputPath.string());
        }
    } else if (std::filesystem::is_directory(inputPath)) {
            outfile << "// Bootstrap code\n";
            outfile << "@256\n";
            outfile << "D = A\n";
            outfile << "@SP\n";
            outfile << "M = D\n";
            outfile << call("Sys.init 0");
        for (auto& entry : std::filesystem::directory_iterator(inputPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".vm") {
                parse(entry.path().string());
            }
        }
    } else {
        std::cerr << "Error: Path is neither a file nor a directory\n";
    }

    outfile << "(END)" << endl;
    outfile << "@END" << endl;
    outfile << "0; JMP" << endl;

    outfile.close();
}

void format(string& str) {
    int i = 1;
    for (; i < str.size(); i++) {
        if (str[i] == '/') break;
    }
    str = str.substr(0, i);

    for (i = 0; i < str.size(); i++) {
        if (str[i] <= 'z' && str[i] >= 'a') break;
    }
    if (i != str.size()) str = str.substr(i);
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
        if (!(command[i] >= '0' && command[i] <= '9')) break;
        strindex += command[i];
    }

    if (segment == "constant") {
        output += "@" + strindex + '\n';
        output += "D = A\n";
    }
    else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
        string segment_label;
        if (segment == "local") segment_label = "LCL";
        else if (segment == "argument") segment_label = "ARG";
        else if (segment == "this") segment_label = "THIS";
        else segment_label = "THAT";
        output += "@" + segment_label + '\n';
        output += "D = M\n";
        output += "@" + strindex + '\n';
        output += "A = D + A\n";
        output += "D = M\n";
    }
    else if (segment == "pointer") {
        int addr = 3 + stoi(strindex);  // 3 for pointer 0, 4 for pointer 1
        output += "@" + to_string(addr) + '\n';
        output += "D = M\n";
    }
    else if (segment == "static") {
        // Static variables are file-scoped
        output += "@" + current_filename + "." + strindex + '\n';
        output += "D = M\n";
    }
    else {
        string start_index;
        if (segment == "temp") start_index = "5";
        output += "@" + start_index + '\n';
        output += "D = A\n";
        output += "@" + strindex + '\n';
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
        if (!(command[i] >= '0' && command[i] <= '9')) break;
        strindex += command[i];
    }

    if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
        string segment_label;
        if (segment == "local") segment_label = "LCL";
        else if (segment == "argument") segment_label = "ARG";
        else if (segment == "this") segment_label = "THIS";
        else segment_label = "THAT";
        output += "@" + segment_label + '\n';
        output += "D = M\n";
        output += "@" + strindex + '\n';
        output += "D = D + A\n";
        output += "@R13\n";
        output += "M = D\n";
    }
    else if (segment == "pointer") {
        int addr = 3 + stoi(strindex);  // 3 for pointer 0, 4 for pointer 1
        output += "@" + to_string(addr) + '\n';
        output += "D = A\n";
        output += "@R13\n";
        output += "M = D\n";
    }
    else if (segment == "static") {
        // Static variables are file-scoped
        output += "@" + current_filename + "." + strindex + '\n';
        output += "D = A\n";
        output += "@R13\n";
        output += "M = D\n";
    }
    else {
        string start_index;
        if (segment == "temp") start_index = "5";
        output += "@" + start_index + '\n';
        output += "D = A\n";
        output += "@" + strindex + '\n';
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

string label(string command) {
    string lname;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (!(command[i] >= 'A' && command[i] <= 'Z') && !(command[i] >= 'a' && command[i] <= 'z') && !(command[i] >= '0' && command[i] <= '9') && command[i] != '_') break;
        lname += command[i];
    }
    return ("(" + lname + ")\n");
}

string go(string command) {
    string lname, output;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (command[i] == ' ') break;
        lname += command[i];
    }
    output = "@" + lname + '\n';
    output += "0; JMP\n";
    return output;
}

string ifgo(string command) {
    string lname;
    string output;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (command[i] == ' ') break;
        lname += command[i];
    }
    output += "@SP\n";
    output += "AM = M - 1\n";
    output += "D = M\n";
    output += "@" + lname + '\n';
    output += "D; JNE\n";
    return output;
}

string func(string command) {
    string fname, strvars;
    string output;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (command[i] == ' ') break;
        fname += command[i];
    }
    for (i++; i < command.size(); i++) {
        if (command[i] > '9' || command[i] < '0') break;
        strvars += command[i];
    }
    output += "(" + fname + ")\n";
    int vars = stoi(strvars);
    for (i = 0; i < vars; i++) {
        output += push("constant 0");
    }
    return output;
}

string call(string command) {
    static int call_count = 0;
    string fname, strargs;
    string output;
    int i;
    for (i = 0; i < command.size(); i++) {
        if (command[i] == ' ') break;
        fname += command[i];
    }
    for (i++; i < command.size(); i++) {
        if (!(command[i] >= '0' && command[i] <= '9')) break;
        strargs += command[i];
    }

    output += "@RET_" + to_string(call_count) + "\n";
    output += "D = A\n";
    output += "@SP\n";
    output += "M = M + 1\n";
    output += "A = M - 1\n";
    output += "M = D\n";
    
    output += "@LCL\n";
    output += "D = M\n";
    output += "@SP\n";
    output += "M = M + 1\n";
    output += "A = M - 1\n";
    output += "M = D\n";
    
    output += "@ARG\n";
    output += "D = M\n";
    output += "@SP\n";
    output += "M = M + 1\n";
    output += "A = M - 1\n";
    output += "M = D\n";
    
    output += "@THIS\n";
    output += "D = M\n";
    output += "@SP\n";
    output += "M = M + 1\n";
    output += "A = M - 1\n";
    output += "M = D\n";
    
    output += "@THAT\n";
    output += "D = M\n";
    output += "@SP\n";
    output += "M = M + 1\n";
    output += "A = M - 1\n";
    output += "M = D\n";
    
    output += "@SP\n";
    output += "D = M\n";
    output += "@LCL\n";
    output += "M = D\n";

    output += "@" + strargs + '\n';
    output += "D = A\n";
    output += "@5\n";
    output += "D = D + A\n";
    output += "@SP\n";
    output += "D = M - D\n";
    output += "@ARG\n";
    output += "M = D\n";

    output += "@" + fname + '\n';
    output += "0; JMP\n";
    output += "(RET_" + to_string(call_count) + ")\n";
    call_count++;
    return output;
}

string ret() {
    string output;
    // Store LCL in R13 (frame)
    output += "@LCL\n";
    output += "D = M\n";
    output += "@R13\n";
    output += "M = D\n";
    
    // Store return address in R14 (retAddr = *(frame - 5))
    output += "@5\n";
    output += "A = D - A\n";
    output += "D = M\n";
    output += "@R14\n";
    output += "M = D\n";
    
    // *ARG = pop()
    output += "@SP\n";
    output += "A = M - 1\n";
    output += "D = M\n";
    output += "@ARG\n";
    output += "A = M\n";
    output += "M = D\n";
    
    // SP = ARG + 1
    output += "@ARG\n";
    output += "D = M + 1\n";
    output += "@SP\n";
    output += "M = D\n";
    
    // Restore THAT = *(frame - 1)
    output += "@R13\n";
    output += "AM = M - 1\n";
    output += "D = M\n";
    output += "@THAT\n";
    output += "M = D\n";
    
    // Restore THIS = *(frame - 2)
    output += "@R13\n";
    output += "AM = M - 1\n";
    output += "D = M\n";
    output += "@THIS\n";
    output += "M = D\n";
    
    // Restore ARG = *(frame - 3)
    output += "@R13\n";
    output += "AM = M - 1\n";
    output += "D = M\n";
    output += "@ARG\n";
    output += "M = D\n";
    
    // Restore LCL = *(frame - 4)
    output += "@R13\n";
    output += "AM = M - 1\n";
    output += "D = M\n";
    output += "@LCL\n";
    output += "M = D\n";
    
    // goto retAddr
    output += "@R14\n";
    output += "A = M\n";
    output += "0; JMP\n";
    
    return output;
}

void parse(string filename) {
    ifstream infile(filename);
    cout << "Parsing file " << filename << endl;

    size_t lastSlash = filename.find_last_of("/\\");
    size_t lastDot = filename.find_last_of(".");
    if (lastSlash == string::npos) lastSlash = 0;
    else lastSlash++;
    if (lastDot == string::npos) lastDot = filename.length();
    current_filename = filename.substr(lastSlash, lastDot - lastSlash);

    string line;
    while(getline(infile, line)) {
        format(line);
        if (line.size() <= 1) continue;
        outfile << "//" << line << endl;

        string type;
        int i;
        for (i = 0; i < line.size(); i++) {
            if (!(line[i] >= 'A' && line[i] <= 'Z') && !(line[i] >= 'a' && line[i] <= 'z') && line[i] != '-') break;
            type += line[i];
        }
        
        string oplines;
        if (type == "push") oplines = push(line.substr(i+1));
        else if (type == "pop") oplines = pop(line.substr(i+1));
        else if (type == "neg" || type == "not") oplines = unary(type);
        else if (type == "eq" || type == "gt" || type == "lt") oplines = comp(type);
        else if (type == "add" || type == "sub" || type == "and" || type == "or") oplines = binary(type);
        else if (type == "label") oplines = label(line.substr(i+1));
        else if (type == "goto") oplines = go(line.substr(i+1));
        else if (type == "if-goto") oplines = ifgo(line.substr(i+1));
        else if (type == "function") oplines = func(line.substr(i+1));
        else if (type == "call") oplines = call(line.substr(i+1));
        else if (type == "return") oplines = ret();
        
        outfile << oplines;
    }

    infile.close();
}