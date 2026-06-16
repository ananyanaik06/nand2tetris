#include<iostream>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<unordered_set>
#include<fstream>

using namespace std;


const unordered_set<string> keywords = {"class", "method","function","constructor", "int", "boolean","char", 
                "void","var","static","field","let","do","if","else","while","return",
                "true","false","null","this"};

const unordered_set<char> symbolSet = {'{', '}', '(', ')', '[', ']',
        '.', ',', ';', '+', '-', '*',
        '/', '&', '|', '<', '>', '=', '~', '"'
        };

const map<char, string> xmlMap = {
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'"', "&quot;"},
        {'&', "&amp;"}
    };

string trim(const string& s){
    int start=0;
    int end=s.size();
    while(start<s.size() && s[start]==' ') start++;
    while(end>start && s[end-1]==' ') end--;
    return s.substr(start, end-start);
}

string strip(const string& line){
    static bool inBlock=false;
    string res;
    int n=line.size();=
    for(int i=0;i<n;i++){
        if(inBlock){
            if(i+1<n&&line[i]=='*'&&line[i+1]=='/'){
                inBlock=false;
                i++;
            }
            continue;
        }
        if(i+1<n&&line[i]=='/'&&line[i+1]=='*'){
            inBlock=true;
            i++;
            continue;
        }
        if(i+1<n&&line[i]=='/'&&line[i+1]=='/')break;
        res+=line[i];
    }
    return trim(res);
}


bool isInteger(const string& token){
    if(token.empty()) return false;
    for(char c:token){
        if(!isdigit(c)){
            return false;
        }
    }
    return true;
}

string tokenType(const string& token){
    if(keywords.count(token)){
        return "keyword";
    }
    if(token.size()==1 && symbolSet.count(token[0])){
        return "symbol";
    }
    if(isInteger(token)){
        return "integerConstant";
    }
    if(token[0]=='"' && token[token.size()-1]=='"') {
        return "stringConstant";
    }
    return "identifier";
}


string xmlSafe(const string& s) {
    string res;
    for(char c:s){
        if(xmlMap.count(c)){
            res+=xmlMap.at(c);
        }
        else res+=c;
    }
    return res;
}


vector<string> tokenize(const string& code) {
    vector<string> tokens;
    string token;
    bool instring = false;
    for (char c : code) {
        if (instring) {
            if (c == '"') {
                tokens.push_back("\"" + token + "\""); 
                token.clear();
                instring = false;
            }
             else {
                token += c;
            }
            continue;
        }

        if (isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (c == '"') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            instring = true; 
        }
        else if (symbolSet.count(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
        else {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}



int main(){
    vector<string> infilenames = {"./ArrayTest/Main.jack", "./ExpressionLessSquare/Main.jack", "./ExpressionLessSquare/Square.jack", "./ExpressionLessSquare/SquareGame.jack", "./Square/Main.jack", "./Square/Square.jack", "./Square/SquareGame.jack"};
    vector<string> outfilenames = {"./ArrayTest/myMainT.xml", "./ExpressionLessSquare/myMainT.xml", "./ExpressionLessSquare/mySquareT.xml", "./ExpressionLessSquare/mySquareGameT.xml", "./Square/myMainT.xml", "./Square/mySquareT.xml", "./Square/mySquareGameT.xml"};

    for(int i=0;i<infilenames.size();i++){
        ifstream myfile(infilenames[i]);
        ofstream outfile(outfilenames[i]);

        string line;
        outfile << "<tokens>" << endl;
        while(getline(myfile,line)){
            string stripped_line = strip(line);
            auto tokens = tokenize(stripped_line);
            for(auto token: tokens){
                string type = tokenType(token);
                if(type!="stringConstant")
                outfile << "<" << type << "> " << xmlSafe(token) << " </" << type << ">" << endl;
                else{
                outfile << "<" << type << "> " << token.substr(1,token.size()-2) << " </" << type << ">" << endl;
                }

            }
        }
        outfile <<"</tokens>"<<endl;

    }

}




