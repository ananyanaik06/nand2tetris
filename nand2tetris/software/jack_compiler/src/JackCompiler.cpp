#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

const unordered_set<string> KEYWORDS = {
    "class", "constructor", "function", "method", "field", "static", "var",
    "int", "char", "boolean", "void", "true", "false", "null", "this",
    "let", "do", "if", "else", "while", "return"
};

const unordered_set<char> SYMBOLS = {
    '{','}','(',')','[',']','.',',',';','+','-','*','/','&','|','<','>','=','~'
};

struct Token {
    string type;
    string val;
};

struct Sym {
    string type;
    string kind;
    int idx;
};

class SymTable {
public:
    unordered_map<string, Sym> cls;
    unordered_map<string, Sym> sub;
    int stat = 0, field = 0, arg = 0, var = 0;

    void startSub() {
        sub.clear();
        arg = 0;
        var = 0;
    }

    void startClass() {
        cls.clear();
        sub.clear();
        stat = field = arg = var = 0;
    }

    void define(const string &n, const string &t, const string &k) {
        if (k == "static") cls[n] = {t, k, stat++};
        else if (k == "field") cls[n] = {t, k, field++};
        else if (k == "arg") sub[n] = {t, k, arg++};
        else if (k == "var") sub[n] = {t, k, var++};
    }

    bool has(const string &n) const {
        return sub.count(n) || cls.count(n);
    }

    Sym get(const string &n) const {
        if (sub.count(n)) return sub.at(n);
        return cls.at(n);
    }

    int count(const string &k) const {
        if (k == "field") return field;
        if (k == "var") return var;
        return 0;
    }
};

string seg(const string &k) {
    if (k == "static") return "static";
    if (k == "field") return "this";
    if (k == "arg") return "argument";
    if (k == "var") return "local";
    return "";
}

bool isSymbol(char c) { return SYMBOLS.count(c); }
bool isKeyword(const string &s) { return KEYWORDS.count(s); }

vector<Token> tokenize(const string &path) {
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "Error reading " << path << "\n";
        return {};
    }

    string s((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    f.close();

    vector<string> raw;
    string t;
    bool str_lit = false, blk = false;

    for (size_t i = 0; i < s.size(); i++) {
        if (!blk && i + 1 < s.size() && s.substr(i, 2) == "/*") {
            blk = true; i++; continue;
        }
        if (blk && i + 1 < s.size() && s.substr(i, 2) == "*/") {
            blk = false; i++; continue;
        }
        if (blk) continue;

        if (!blk && i + 1 < s.size() && s.substr(i, 2) == "//") {
            while (i < s.size() && s[i] != '\n') i++;
            continue;
        }

        char c = s[i];

        if (str_lit) {
            t += c;
            if (c == '"') {
                raw.push_back(t);
                t = "";
                str_lit = false;
            }
        } else if (isspace(c)) {
            if (!t.empty()) { raw.push_back(t); t = ""; }
        } else if (isSymbol(c)) {
            if (!t.empty()) { raw.push_back(t); t = ""; }
            raw.push_back(string(1, c));
        } else if (c == '"') {
            if (!t.empty()) { raw.push_back(t); t = ""; }
            t = "\""; str_lit = true;
        } else t += c;
    }
    if (!t.empty()) raw.push_back(t);

    vector<Token> tok;
    for (auto &x : raw) {
        Token tk;
        if (isKeyword(x)) tk.type = "keyword";
        else if (isSymbol(x[0])) tk.type = "symbol";
        else if (all_of(x.begin(), x.end(), ::isdigit)) tk.type = "int";
        else if (x[0] == '"') {
            tk.type = "string"; tk.val = x.substr(1, x.size() - 2);
            tok.push_back(tk); continue;
        }
        else tk.type = "ident";
        tk.val = x;
        tok.push_back(tk);
    }
    return tok;
}

class VM {
public:
    ofstream o;
    VM(const string &f) {
        o.open(f);
        if (!o.is_open()) cerr << "Error opening " << f << "\n";
    }
    ~VM() { if (o.is_open()) o.close(); }

    void fdef(const string &n, int l) { o << "function " << n << " " << l << "\n"; }
    void push(const string &s, int i) { o << "push " << s << " " << i << "\n"; }
    void pop(const string &s, int i) { o << "pop " << s << " " << i << "\n"; }
    void op(const string &c) { o << c << "\n"; }
    void call(const string &n, int a) { o << "call " << n << " " << a << "\n"; }
    void ret() { o << "return\n"; }
    void label(const string &l) { o << "label " << l << "\n"; }
    void go(const string &l) { o << "goto " << l << "\n"; }
    void ifgo(const string &l) { o << "if-goto " << l << "\n"; }
};

class Compiler {
public:
    vector<Token> t; int p = 0; SymTable st; VM &vm; string cls; int L = 0;

    Compiler(vector<Token> T, VM &V) : t(std::move(T)), vm(V) {}

    Token cur() { return p < t.size() ? t[p] : Token(); }
    Token adv() { return p < t.size() ? t[p++] : Token(); }
    bool eq(const string &s) { return p < t.size() && t[p].val == s; }
    void need(const string &s) { if (!eq(s)) cerr << "Expected " << s << " got " << cur().val << "\n"; else p++; }
    bool type() {
        return p < t.size() &&
               (t[p].type == "ident" ||
                (t[p].type == "keyword" && (t[p].val == "int" || t[p].val == "char" || t[p].val == "boolean")));
    }

    void class_() {
        st.startClass(); 
        need("class"); cls = adv().val; need("{");
        while (eq("static") || eq("field")) classVar();
        while (eq("constructor") || eq("function") || eq("method")) subr();
        need("}");
    }

    void classVar() {
        string k = adv().val, ty = adv().val, n = adv().val;
        st.define(n, ty, k);
        while (eq(",")) { adv(); string n2 = adv().val; st.define(n2, ty, k); }
        need(";");
    }

    void subr() {
        string sk = adv().val;
        if (eq("void") || type()) adv();
        string name = adv().val;
        st.startSub();
        if (sk == "method") st.define("this", cls, "arg");
        need("("); params(); need(")");
        need("{"); while (eq("var")) varDec();
        vm.fdef(cls + "." + name, st.count("var"));
        if (sk == "constructor") {
            vm.push("constant", st.count("field"));
            vm.call("Memory.alloc", 1);
            vm.pop("pointer", 0);
        } else if (sk == "method") {
            vm.push("argument", 0);
            vm.pop("pointer", 0);
        }
        stats(); need("}");
    }

    void params() {
        if (type()) {
            string ty = adv().val, n = adv().val;
            st.define(n, ty, "arg");
            while (eq(",")) { adv(); ty = adv().val; n = adv().val; st.define(n, ty, "arg"); }
        }
    }

    void varDec() {
        need("var"); string ty = adv().val, n = adv().val;
        st.define(n, ty, "var");
        while (eq(",")) { adv(); string n2 = adv().val; st.define(n2, ty, "var"); }
        need(";");
    }

    void stats() {
        while (true) {
            if (eq("let")) let_(); else if (eq("if")) if_();
            else if (eq("while")) while_(); else if (eq("do")) do_();
            else if (eq("return")) ret_(); else break;
        }
    }

    void do_() { need("do"); subCall(); need(";"); vm.pop("temp", 0); }

    void let_() {
        need("let"); string name = adv().val; bool arr = false;
        if (eq("[")) { adv(); expr(); need("]"); auto s = st.get(name); vm.push(seg(s.kind), s.idx); vm.op("add"); arr = true; }
        need("="); expr(); need(";");
        if (arr) { vm.pop("temp", 0); vm.pop("pointer", 1); vm.push("temp", 0); vm.pop("that", 0); }
        else { auto s = st.get(name); vm.pop(seg(s.kind), s.idx); }
    }

    void while_() {
        need("while"); int a = L, b = L + 1; L += 2;
        vm.label("WHILE_EXP" + to_string(a));
        need("("); expr(); need(")"); vm.op("not"); vm.ifgo("WHILE_END" + to_string(b));
        need("{"); stats(); need("}"); vm.go("WHILE_EXP" + to_string(a)); vm.label("WHILE_END" + to_string(b));
    }

    void if_() {
        need("if"); need("("); expr(); need(")");
        int f = L, e = L + 1; L += 2;
        vm.op("not"); vm.ifgo("IF_FALSE" + to_string(f));
        need("{"); stats(); need("}");
        if (eq("else")) {
            vm.go("IF_END" + to_string(e)); vm.label("IF_FALSE" + to_string(f));
            adv(); need("{"); stats(); need("}"); vm.label("IF_END" + to_string(e));
        } else vm.label("IF_FALSE" + to_string(f));
    }

    void ret_() {
        need("return");
        if (!eq(";")) expr(); else vm.push("constant", 0);
        need(";"); vm.ret();
    }

    int exprList() {
        int n = 0; if (!eq(")")) { expr(); n = 1; while (eq(",")) { adv(); expr(); n++; } } return n;
    }

    void subCall() {
        string n = adv().val; int nargs = 0;
        if (eq(".")) {
            adv(); string sub = adv().val; need("(");
            if (st.has(n)) { auto s = st.get(n); vm.push(seg(s.kind), s.idx); nargs = 1 + exprList(); need(")"); vm.call(s.type + "." + sub, nargs); }
            else { nargs = exprList(); need(")"); vm.call(n + "." + sub, nargs); }
        } else {
            need("("); vm.push("pointer", 0); nargs = 1 + exprList(); need(")"); vm.call(cls + "." + n, nargs);
        }
    }

    void expr() {
        term();
        while (p < t.size() && string("+-*/&|<>=~").find(cur().val) != string::npos) {
            string op = adv().val; term();
            if (op == "+") vm.op("add");
            else if (op == "-") vm.op("sub");
            else if (op == "*") vm.call("Math.multiply", 2);
            else if (op == "/") vm.call("Math.divide", 2);
            else if (op == "&") vm.op("and");
            else if (op == "|") vm.op("or");
            else if (op == "<") vm.op("lt");
            else if (op == ">") vm.op("gt");
            else if (op == "=") vm.op("eq");
        }
    }

    void term() {
        auto x = cur();
        if (x.type == "int") { adv(); vm.push("constant", stoi(x.val)); return; }
        if (x.type == "string") { adv(); vm.push("constant", x.val.size()); vm.call("String.new", 1);
            for (char c : x.val) { vm.push("constant", c); vm.call("String.appendChar", 2); } return; }
        if (x.type == "keyword") { adv(); if (x.val == "true") { vm.push("constant", 1); vm.op("neg"); }
            else if (x.val == "false" || x.val == "null") vm.push("constant", 0);
            else if (x.val == "this") vm.push("pointer", 0); return; }
        if (x.val == "(") { adv(); expr(); need(")"); return; }
        if (x.val == "-" || x.val == "~") { adv(); term(); vm.op(x.val == "-" ? "neg" : "not"); return; }
        if (x.type == "ident") {
            string name = adv().val;
            if (eq("[")) { adv(); expr(); need("]"); auto s = st.get(name); vm.push(seg(s.kind), s.idx); vm.op("add"); vm.pop("pointer", 1); vm.push("that", 0); return; }
            if (eq("(") || eq(".")) { p--; subCall(); return; }
            if (st.has(name)) { auto s = st.get(name); vm.push(seg(s.kind), s.idx); }
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 2) { cerr << "Usage: JackCompiler <file.jack|directory>\n"; return 1; }
    fs::path in(argv[1]);
    vector<fs::path> files;
    if (fs::is_directory(in)) {
        for (auto &p : fs::directory_iterator(in))
            if (p.path().extension() == ".jack") files.push_back(p.path());
    } else if (in.extension() == ".jack") files.push_back(in);
    if (files.empty()) { cerr << "No .jack files found\n"; return 1; }

    for (auto &f : files) {
        auto tok = tokenize(f.string());
        if (tok.empty()) { cerr << "Failed tokenize " << f << "\n"; continue; }
        string out = f.replace_extension(".vm").string();
        VM vm(out);
        Compiler c(tok, vm);
        c.class_();
        cerr << "Compiled " << f.filename().string() << " -> " << out << "\n";
    }
}
