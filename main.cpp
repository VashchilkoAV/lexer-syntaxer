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

//enum Tokentype {
//    COMMAND,
//    COMMAND_IN_BACKTICK,
//    CONVEYOR_SEPARATOR ,
//    INPUT_REDIRECTION,
//    OUTPUT_REDIRECTION,
//    ADDRESS_WITH_REGEXP,
//    VARIABLE
//};

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

bool IsReg(const std::string& res) {
    for (const auto& ch : res) {
        if (ch == '*' || ch == '?') {
            return true;
        }
    }
    return false;
}

//std::vector<Token> Lexer(const std::string& res) {
//    std::vector<Token> result;
//    std::vector<std::string> lexemes;
//    std::string tmp;
//    bool symbol = true;
//    bool backtick_opened = false;
//    bool redir_sym = true;
//    bool backtick = true;
//    bool space = true;
//    bool var = true;
//    for (unsigned long i = 0; i < res.length(); i++) {
//        if (res[i] == ' ' || res[i] == '\t') {
//            if (space) {
//                if (backtick_opened) {
//                    tmp.push_back(res[i]);
//                } else {
//                    if (!tmp.empty()) {
//                        lexemes.push_back(tmp);
//                        tmp.clear();
//                    }
//                    var = symbol = backtick = space = redir_sym = true;
//                }
//            } else {
//                throw std::domain_error("Unexpected space symbol\n");
//            }
//        } else if (res[i] == '<') {
//            if (redir_sym) {
//                if (backtick_opened) {
//
//                } else {
//                    space = true;
//                    var = symbol = backtick = redir_sym = false;
//                }
//                tmp.push_back(res[i]);
//            } else {
//                throw std::domain_error("Unexpected '<'\n");
//            }
//        } else if (res[i] == '>') {
//            if (redir_sym) {
//                if (backtick_opened) {
//
//                } else {
//                    space = true;
//                    var = symbol = backtick = redir_sym = false;
//                }
//                tmp.push_back(res[i]);
//            } else {
//                throw std::domain_error("Unexpected '>'\n");
//            }
//        } else if (res[i] == '"') {
//            if (backtick) {
//                if (backtick_opened) {
//                    var = backtick = redir_sym = symbol = backtick_opened = false;
//                    space = true;
//                } else {
//                    var = backtick_opened = backtick = redir_sym = space = symbol = true;
//                }
//            } else {
//                throw std::domain_error("Unexpected \"\n");
//            }
//        } else if (res[i] == '$') {
//            if (var) {
//                if (backtick_opened) {
//                    tmp.push_back(res[i]);
//                } else {
//                    symbol = true;
//                    backtick = space = redir_sym = var = false;
//                }
//            } else {
//                throw std::domain_error("Unexpected '$'\n");
//            }
//        } else {
//            if (symbol) {
//                if (backtick_opened) {
//
//                } else {
//                    space = symbol = true;
//                    backtick = var = redir_sym  = false;
//                }
//                tmp.push_back(res[i]);
//            } else {
//                throw std::domain_error("Unexpected symbol: '"+std::string(1, res[i])+"'\n");
//            }
//        }
//
//    }
//
//    if (!tmp.empty()) {
//        if (backtick_opened == true) {
//            throw std::domain_error("No closing \"\n");
//        } else {
//            lexemes.push_back(tmp);
//            tmp.clear();
//        }
//    }
//    for (const auto& item : lexemes) {
//        if (item[0] == '"') {
//            result.push_back({Tokentype::COMMAND_IN_BACKTICK, item.substr(1, item.length()-2)});
//        } else if (item[0] == '<' && item.length() == 1) {
//            result.push_back({Tokentype::INPUT_REDIRECTION, "<"});
//        } else if (item[0] == '>' && item.length() == 1) {
//            result.push_back({Tokentype::OUTPUT_REDIRECTION, ">"});
//        } else if (item[0] == '|' && item.length() == 1) {
//            result.push_back({Tokentype::CONVEYOR_SEPARATOR, "|"});
//        } else if (IsReg(item)) {
//            result.push_back({Tokentype::ADDRESS_WITH_REGEXP, item});
//        } else if (item[0] == '$') {
//            result.push_back({Tokentype::VARIABLE, item.substr(1)});
//        } else {
//            result.push_back({Tokentype::COMMAND, item});
//        }
//    }
//    return result;
//}
//
//Task Syntaxer(std::vector<Token>& res) {
//    int task_num = 0;
//    Task result;
//    std::vector<std::string> tmp;
//    bool waiting_for_command = true;
//    bool waiting_for_filename = false;
//    bool in = false, out = false;
//    for (const auto& item : res) {
//        if (item._type == Tokentype::CONVEYOR_SEPARATOR) {
//            if (waiting_for_filename) {
//                throw std::domain_error("Expected filename\n");
//            } else if (waiting_for_command) {
//                throw std::domain_error("Expected command\n");
//            } else {
//                if (tmp.empty()) {
//                    throw std::domain_error("Unexpected syntax near the '|'");
//                } else {
//                    result.params.push_back(tmp);
//                    tmp.clear();
//                    task_num++;
//                    waiting_for_command = true;
//                }
//            }
//        } else if (item._type == Tokentype::COMMAND) {
//            if (waiting_for_filename) {
//                if (in) {
//                    result.input = item.value;
//                    in = false;
//                } else if (out) {
//                    result.output = item.value;
//                    out = false;
//                }
//            } else {
//                tmp.push_back(item.value);
//            }
//            waiting_for_command = false;
//            waiting_for_filename = false;
//        } else if (item._type == Tokentype::COMMAND_IN_BACKTICK) {
//            if (waiting_for_filename) {
//                if (in) {
//                    result.input = item.value;
//                    in = false;
//                } else if (out) {
//                    result.output = item.value;
//                    out = false;
//                }
//
//            } else {
//                tmp.push_back(item.value);
//            }
//            waiting_for_command = false;
//            waiting_for_filename = false;
//        } else if (item._type == Tokentype::ADDRESS_WITH_REGEXP) {
//            if (waiting_for_filename) {
//                if (in) {
//                    //regexp
//                    in = false;
//                } else if (out) {
//                    //regexp
//                    out = false;
//                }
//            } else {
//               tmp.push_back(item.value);
//            }
//            waiting_for_command = false;
//            waiting_for_filename = false;
//        } else if (item._type == Tokentype::OUTPUT_REDIRECTION) {
//            if (waiting_for_command) {
//                throw std::domain_error("Unexpected >\n");
//            }  else if (waiting_for_filename) {
//                throw std::domain_error("Expected filename\n");
//            }
//            waiting_for_filename = true;
//            out = true;
//        } else if (item._type == Tokentype::INPUT_REDIRECTION) {
//            if (waiting_for_command) {
//                throw std::domain_error("Unexpected >\n");
//            } else if (waiting_for_filename) {
//                throw std::domain_error("Expected filename\n");
//            }
//            waiting_for_filename = true;
//            in = true;
//        }
//    }
//    if (waiting_for_command) {
//        throw std::domain_error("Expected filename\n");
//    } else if (!tmp.empty()) {
//        result.params.push_back(tmp);
//        tmp.clear();
//    }
//    result.size = result.params.size();
//    return result;
//}

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
//                result.clear();
//                return result;
            }
        }
        if (stat == 0 || stat == 1 || stat == 2 || stat == 4 || stat == 5) {
            if (!tmp.empty()) {
                Push(tmp, result);
                tmp.clear();
            }
        } else {
//            result.clear();
//            return result;

            throw std::domain_error(MakeError(res, res.length()));
        }
        return result;
    }
private:
    void MakeSets() {
//        for (char i = 42; i < 58; i++) {
//            terminals.insert(i);
//        }
//        for (char i = 63; i < 123; i++) {
//            terminals.insert(i);
//        }
//        terminals.insert(37);
//        any = {33, 35, 38, 39, 40, 41, 58, 59, 61, 123, 125, 126};
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


void NewLexer(const std::string& res) {
    DFA a;
    a.Analize(res);
}


int main() {
    std::string res;
//    while(getline(std::cin, res)) {
//        std::vector<Token> lexed;
//        try {
//            lexed = Lexer(res);
//        } catch (std::exception& exc) {
//            std::cout << "Lexical error: "+std::string(exc.what());
//            continue;
//        }
//        std::cout << "Lexed:\n";
//        for (const auto& item : lexed) {
//            std::cout << item.value << "\n";
//        }
//        Task syntaxed;
//        try {
//            syntaxed = Syntaxer(lexed);
//        } catch (std::exception& exc) {
//            std::cout << "Syntax error: "+std::string(exc.what());
//            continue;
//        }
//        std::cout << "Syntaxed:\n";
//        int n = 0;
//        for (const auto& item : syntaxed.params) {
//            std::cout << n << ":\n";
//            for (const auto& str : item) {
//                std::cout << str << " ";
//            }
//            std::cout << "\n";
//            n++;
//        }
//    }
    DFA a;
    while (getline(std::cin, res)) {
        std::vector<Token> result;
        try {
            result = a.Analize(res);
        } catch (std::exception& exc) {
            std::cout << exc.what() << "\n";
        }
        for (const auto& item : result) {
            std::cout << item.value << " " << item._type << "\n";
        }
        std::cout << "\n\n";
    }
    return 0;
}