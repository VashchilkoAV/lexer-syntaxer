#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <exception>
#include <set>


struct Task {
    std::string input = "";
    std::vector<std::vector<std::string>> params;
    std::string output = "";
    int size = 0;
};

enum Tokentype {
    COMMAND,
    CONVEYOR_SEPARATOR,
    INPUT_REDIR,
    OUTPUT_REDIR,
    VARIABLE
};

struct Token {
    Tokentype _type;
    std::string value;
};

std::string ParseTicks(const std::string& res) {
    std::stringstream ss(res);
    std::string tmp, result;
    while (getline(ss, tmp, '"')) {
        result+=tmp;
    }
    return result;
}

bool IsReg(const std::string& res) {
    bool in_tick = 0;
    for (const auto& ch : res) {
        if (ch == '"') {
            if (in_tick) {
                in_tick = 0;
            } else {
                in_tick = 1;
            }
        }
        if (!in_tick && (ch == '*' || ch == '?')) {
            return true;
        }
    }
    return false;
}



void Push(const std::string& tmp, std::vector<Token>& result) {
    if (tmp[0] == '$') {
        result.push_back({Tokentype::VARIABLE, tmp});
    } else if (tmp[0] == '<') {
        result.push_back({Tokentype::INPUT_REDIR, tmp});
    } else if (tmp[0] == '>') {
        result.push_back({Tokentype::OUTPUT_REDIR, tmp});
    } else if (tmp[0] == '|') {
        result.push_back({Tokentype::CONVEYOR_SEPARATOR, tmp});
    } else {
        result.push_back({Tokentype::COMMAND, tmp});
    }
}

const std::string MakeError(const std::string& res, unsigned num) {
    std::string a;
    for (unsigned long i = 0; i < num; i++) {
        a.push_back('~');
    }
    a.push_back('^');
    for (unsigned long i = num+1; i < res.length(); i++) {
        a.push_back('~');
    }
    std::string begin = (num != res.length()) ? "Unexpected symdol:\n" : "No closing \":\n";
    return begin+res+"\n"+a;
}

class DFA {
public:
    DFA()  {
        MakeSets();
        MakeAutomat();
    }
    std::vector<Token> Analize(const std::string& res) {
        std::vector<Token> result;
        std::string tmp;
        char ch;
        char stat = 0;
        for (unsigned i = 0; i < res.length(); i++) {
            ch = res[i];
            char next = 0;
            if (terminals.count(ch)) {
                next = 0;
                tmp.push_back(ch);
            } else if (dollar.count(ch)) {
                next = 1;
                tmp.push_back(ch);
            } else if (ticks.count(ch)) {
                next = 2;
                tmp.push_back(ch);
            } else if (spaces.count(ch)) {
                next = 3;
                if (M[next][stat] == 3) {
                    tmp.push_back(ch);
                }
            } else if (specials.count(ch)) {
                next = 4;
                tmp.push_back(ch);
            } else {
                result.clear();
                return result;
            }
            stat = M[next][stat];
            if (stat == 0) {
                if (!tmp.empty()) {
                    Push(tmp, result);
                    tmp.clear();
                }
            }
            if (stat == 100) {
                throw std::domain_error(MakeError(res, i));
            }
        }
        if (stat != 3) {
            if (!tmp.empty()) {
                Push(tmp, result);
                tmp.clear();
            }
        } else {
            throw std::domain_error(MakeError(res, res.length()));
        }
        return result;
    }
private:
    void MakeSets() {
        terminals = {33, 35, 37, 38, 61, 125, 126};
        for (char i = 40; i < 60; i++) {
            terminals.insert(i);
        }
        for (char i = 63; i < 123; i++) {
            terminals.insert(i);
        }
        dollar = {'$'};
        ticks = {'"'};
        spaces = {' ', '\t'};
        specials = {'<', '>', '|'};
    }

    void MakeAutomat() {
        M = {{2, 100, 2, 3, 4, 2}, {5, 100, 100, 3, 100, 100}, {3, 100, 3, 4, 3, 3}, {0, 0, 0, 3, 0, 0}, {1, 100, 100, 3, 100, 100}};
    }


    std::vector<std::vector<char>> M;
    std::set<char> terminals;       //0
    std::set<char> dollar;          //1
    std::set<char> ticks;           //2
    std::set<char> spaces;          //3
    std::set<char> specials;        //4
};

class DFA2 {
public:
    DFA2() {
        MakeAutomat();
    }
    Task Syntax(std::vector<Token>& tokens) {
        Task result;
        char state = 0;
        std::vector<std::string> part;
        for (unsigned long i = 0; i < tokens.size(); i++) {
            auto tok = tokens[i];
            char prev = state;
            state = M[tok._type][state];
            if (state == 100) {
                throw std::domain_error("Unexpected token: " + tok.value);
            }
            if (state == 6) {
                result.params.push_back(part);
                part.clear();
            } else if (state == 4 && (prev == 3 || prev == 5)) {
                result.output = GetParam(tok);
            } else if (state == 1 && prev == 2) {
                result.input = GetParam(tok);
            } else if (state == 9 && prev == 8) {
                result.output = GetParam(tok);
            } else if (state != 3 && state != 2 && state != 5 && state != 8) {
                for (const auto& item : GetParams(tok)) {
                    part.push_back(item);
                }
            }
        }
        if (state == 0 || state == 1 || state == 4 || state == 7 || state == 9) {
            if (part.size()) {
                result.params.push_back(part);
            }
        } else {
            throw std::domain_error("Expected token");
        }
        return result;
    }

private:
    std::vector<std::string> GetParams(Token& tok) {
        if (!IsReg(tok.value)) {
            return {ParseTicks(tok.value)};
        } else {
            return {tok.value};
        }
    }
    std::string GetParam(Token& tok) {
        auto t = GetParams(tok);
        return t.size() ? t[0] : "";
    }
    void MakeAutomat() {
        M = {
                {1, 1, 1, 4, 4, 4, 7, 7, 9, 9}, // 0 -- comm
                {100, 6, 100, 100, 100, 100, 100, 6, 100, 100}, // 1 -- conv
                {2, 2, 100, 100, 5, 100, 100, 100, 100, 100}, // 2 -- input
                {3, 3, 100, 100, 100, 100, 8, 8, 100, 100}, // 3 -- output
                {100, 1, 1, 4, 4, 4, 100, 7, 9, 9} // 4 - var
        };
    }
    std::vector<std::vector<char>> M;
};


int main() {
    std::string res;
    DFA a;
    DFA2 b;
    while (getline(std::cin, res)) {
        std::vector<Token> result;
        Task task;
        std::cout << "Lexed:\n";
        try {
            result = a.Analize(res);
        } catch (std::exception& exc) {
            std::cout << "Lexical error: " << exc.what() << "\n";
            continue;
        }
        for (const auto &item : result) {
            std::cout << item.value << " " << item._type << "\n";
        }
        std::cout << "Syntaxed:\n";
        try {
            task = b.Syntax(result);
        } catch (std::exception& exc) {
            std::cout << "Syntax error: " << exc.what() << "\n";
            continue;
        }
        std::cout << "Input: "+task.input+" , Output: "+task.output << "\n";
        for (unsigned long i = 0; i < task.params.size(); i++) {
            std::cout << i << ": ";
            for (const auto& it : task.params[i]) {
                std::cout << it << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n\n\n";
    }
    return 0;
}