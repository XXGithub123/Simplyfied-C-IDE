#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <set>
#include <map>
#include "lexical.h"
#include "grammer.h"
#include "midCode.h"
#include "mipsCode.h"
#include "symbolItem.h"
#include "function.h"
using namespace std;

extern char token[100000];
extern int num;   // Record constant integer.
extern char con_ch;  //Record constant char.
extern char s[100000];  //Record constant string.
extern enum typeId symbol;

// extern ofstream errorfile;

extern int oldIndex;    // used for recover
extern int line;  // line index

map<string, symbolItem> globalSymbolTable;
map<string, symbolItem> localSymbolTable;
map<string, map<string, symbolItem>> allLocalSymbolTable;  // Save all local symbol table to save variable address.
vector<string> stringList;  // Save all string.
vector<midCode> midCodeTable;

int curFuncReturnType = -1;
int realReturnType = -1;
int globalAddr = 0;
int localAddr = 0;
bool isMain = false;

//＜string＞   ::=  "｛ASCII  character from 32 to 126.｝"
bool strings() {
    if (symbol == STRCON) {
        getsym();  // pre-load a token.
        return true;
    }
    else
        return false;
}

/*
 * ＜procedure＞  ::= ［＜constant declaration＞］［＜variable declaration＞］
 *                      {＜function with return value＞|＜function without return value＞}＜main function＞
 */
bool procedure() {

    constDeclaration(true);

    variableDeclaration(true);
    while (true) {
        if (symbol == INTTK || symbol == CHARTK) {
            if (!functionWithReturnValue()) {
                return false;
            }
        }
        else if (symbol == VOIDTK) {
            if (!functionWithoutReturnValue()) {
                break;
            }
        }
        else
            return false;
    }
    // Analyze main function.
    if (mainFunction()) {
        // Check if there is any token left.
        if (isEOF()) {
            return true;
        }
    }
    return false;
}

//＜constant declaration＞ ::=  {<const>＜constant definition＞<;>}
bool constDeclaration(bool isglobal) {
    if (symbol == CONSTTK) {
        int re = getsym();  // Read the next token.
        if (!constDefinition(isglobal)) {
            return false;
        }
        else {
            if (isEOF()) {   // Read to the end.
                return false;
            }
            if (symbol != SEMICN) {
                retractString(oldIndex);
                raiseError("Need semicolon.", line);
                // errorfile << line << " k\n";
                symbol = SEMICN;
            }
            if (symbol == SEMICN) {  // semicolon
                // Start to analyze constant defination.
                while (true) {
                    re = getsym();
                    if (symbol != CONSTTK) {
                        break;
                    }
                    // Current token is CONST, keep on reading.
                    re = getsym();
                    if (!constDefinition(isglobal)) {
                        return false;
                    }
                    if (isEOF()) {
                        return false;
                    }
                    // Finish constant defination and read next token.
                    if (symbol != SEMICN) {
                        retractString(oldIndex);
                        raiseError("Need semicolon.", line);
                        // errorfile << line << " k\n";
                        symbol = SEMICN;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

//＜Concstant Defination＞ ::= {int＜Indentifier＞＝＜Integer＞}
//                  | {char＜Indentifier＞＝＜charicter＞}
bool constDefinition(bool isglobal) {
    string name;
    if (symbol == INTTK) {
        int re = getsym();
        if (symbol == IDENFR) {
            name = string(token);
            re = getsym();
            if (symbol == ASSIGN) {
                re = getsym();
                int conInt;
                if (integer(conInt)) {
                    if (isglobal) {
                        if (globalSymbolTable.find(name) == globalSymbolTable.end()) {  // Cannot find.
                            globalSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 1, conInt)));
                        }
                        else {  // Finded, which indicated redefination.
                            raiseError("Redefination.", line);
                            // errorfile << line << " b\n";
                        }
                    }
                    else {
                        if (localSymbolTable.find(name) == localSymbolTable.end()) {  // Cannot find.
                            localSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 1, conInt)));
                        }
                        else {  // Finded, which indicated redefination.
                            raiseError("Redefination.", line);
                            // errorfile << line << " b\n";
                        }
                    }
                    midCodeTable.push_back(midCode(CONST, "int", name, int2string(conInt)));
                }
                else {
                    raiseError("Fail to anaylze integer.", line);
                    // errorfile << line << " o\n";
                    getsym();
                }
                while (true) {
                    if (isEOF()) {
                        break;
                    }
                    if (symbol != COMMA) {
                        break;
                    }
                    // Current token is ","
                    re = getsym();
                    if (symbol != IDENFR) { // the token after ", is not identifier.
                        return false;
                    }
                    name = string(token);
                    // Current token is identifier.
                    re = getsym();
                    if (symbol != ASSIGN) { // token after identifier is not "="
                        return false;
                    }
                    // "="
                    re = getsym();
                    if (integer(conInt)) {
                        if (isglobal) {
                            if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                                globalSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 1, conInt)));
                            }
                            else {
                                raiseError("Redefination", line);
                                // errorfile << line << " b\n";
                            }
                        }
                        else {
                            if (localSymbolTable.find(name) == localSymbolTable.end()) {
                                localSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 1, conInt)));
                            }
                            else {
                                raiseError("Redefination", line);
                                // errorfile << line << " b\n";
                            }
                        }
                        midCodeTable.push_back(midCode(CONST, "int", name, int2string(conInt)));
                    }
                    else {
                        raiseError("Fail to anaylze integer.", line);
                        // errorfile << line << " o\n";
                        getsym();
                    }
                }
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else if (symbol == CHARTK) {  //{char＜indentifier＞＝＜character＞}
        int re = getsym();
        if (symbol == IDENFR) {
            name = string(token);
            re = getsym();
            if (symbol == ASSIGN) {
                re = getsym();
                if (symbol == CHARCON) {
                    if (isglobal) {
                        if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                            globalSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 2, 0, con_ch)));
                        }
                        else {
                            raiseError("Redefination", line);
                            // errorfile << line << " b\n";
                        }
                    }
                    else {
                        if (localSymbolTable.find(name) == localSymbolTable.end()) {
                            localSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 2, 0, con_ch)));
                        }
                        else {
                            raiseError("Redefination", line);
                            // errorfile << line << " b\n";
                        }
                    }
                    midCodeTable.push_back(midCode(CONST, "char", name, int2string(con_ch)));
                }
                else {
                    raiseError("Fail to analyze char", line);
                    // errorfile << line << " o\n";
                }
                while (true) {
                    re = getsym();
                    if (symbol != COMMA) {
                        break;
                    }
                    re = getsym();
                    if (symbol != IDENFR) {
                        return false;
                    }
                    name = string(token);
                    re = getsym();
                    if (symbol != ASSIGN) {
                        return false;
                    }
                    re = getsym();
                    if (symbol == CHARCON) {

                        if (isglobal) {
                            if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                                globalSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 2, 0, con_ch)));
                            }
                            else {
                                raiseError("Redefination", line);
                                // errorfile << line << " b\n";
                            }
                        }
                        else {
                            if (localSymbolTable.find(name) == localSymbolTable.end()) {
                                localSymbolTable.insert(make_pair(name, symbolItem(name, -1, 2, 2, 0, con_ch)));
                            }
                            else {
                                raiseError("Redefination", line);
                                // errorfile << line << " b\n";
                            }
                        }
                        midCodeTable.push_back(midCode(CONST, "char", name, int2string(con_ch)));
                    }
                    else {
                        raiseError("", line);
                        // errorfile << line << " o\n";
                    }
                }
                return true;
            }
        }
    }
    return false;
}

//＜unsigned integer＞  ::= ＜non-zero number＞｛＜number＞｝| <zero>
bool unsignedInteger(int& value) {
    if (symbol == INTCON) {
        value = num;
        getsym();    // pre-read a token.
        return true;
    }
    else {
        return false;
    }
}

//＜integer＞ ::= ［＋｜－］＜unsigned integer＞
bool integer(int& value) {
    int re;
    if (symbol == PLUS || symbol == MINU) {
        bool isPLUS = (symbol == PLUS);
        re = getsym();
        if (unsignedInteger(value)) {
            if (!isPLUS) {
                value = -value;
            }
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if (unsignedInteger(value)) {
            return true;
        }
        else {
            return false;
        }
    }
}

//＜declearation head＞   ::=  int＜indentifier＞ |char＜indentifier＞
bool declarationHead(string& tmp, int& type) {
    if (symbol == INTTK || symbol == CHARTK) {
        if (symbol == INTTK) {
            type = 1;
        }
        else {
            type = 2;
        }
        int re = getsym();
        if (symbol != IDENFR) {
            return false;
        }
        tmp = string(token);
        re = getsym();
        return true;
    }
    else {
        return false;
    }
}

//＜variable declaration＞  ::= ＜variable defination＞;{＜variable defination＞;}
bool variableDeclaration(bool isglobal) {
    if (!isglobal) {
        if (symbol == INTTK || symbol == CHARTK) {
            if (!variableDefinition(isglobal)) {
                return false;
            }
            if (isEOF()) {
                return false;
            }
            if (symbol != SEMICN) {
                retractString(oldIndex);
                raiseError("Expect a semicolon.", line);
                //errorfile << line << " k\n";
                symbol = SEMICN;
            }
            if (symbol == SEMICN) {
                // Start to analyze {＜varibale defination＞;}
                while (true) {
                    int re = getsym();
                    if (!variableDefinition(isglobal)) {
                        break;
                    }
                    if (isEOF()) {
                        return false;
                    }
                    if (symbol != SEMICN) {
                        retractString(oldIndex);
                        raiseError("Expect a semicolon.", line);
                        // errorfile << line << " k\n";
                        symbol = SEMICN;
                    }
                }
                return true;
            }
        }
        return false;
    }
    else {
        int old = oldIndex;
        if (symbol == INTTK || symbol == CHARTK) {
            int re = getsym(0);
            if (symbol == IDENFR) {
                re = getsym(0);
                if (symbol == LPARENT) {
                    retractString(old);
                    getsym(0);
                    return false;
                }
                else {
                    retractString(old);
                    getsym();
                    if (symbol == INTTK || symbol == CHARTK) {
                        if (!variableDefinition(isglobal)) {
                            return false;
                        }
                        if (isEOF()) {
                            return false;
                        }
                        if (symbol != SEMICN) {
                            retractString(oldIndex);
                            raiseError("Expect a semicolon.", line);
                            // errorfile << line << " k\n";
                            symbol = SEMICN;
                        }
                        if (symbol == SEMICN) {
                            // Start to analyze {＜variable defination＞;}
                            while (true) {
                                re = getsym();
                                old = oldIndex;
                                if (symbol == INTTK || symbol == CHARTK) {
                                    re = getsym(0);
                                    if (symbol == IDENFR) {
                                        re = getsym(0);
                                        if (symbol == LPARENT) {
                                            retractString(old);
                                            getsym(0);
                                            break;
                                        }
                                        else {
                                            retractString(old);
                                            getsym();
                                            if (symbol == INTTK || symbol == CHARTK) {
                                                if (!variableDefinition(isglobal)) {
                                                    break;
                                                }
                                                if (isEOF()) {
                                                    return false;
                                                }
                                                if (symbol != SEMICN) {
                                                    retractString(oldIndex);
                                                    raiseError("Expect a semicolon.", line);
                                                    // errorfile << line << " k\n";
                                                    symbol = SEMICN;
                                                }
                                                if (symbol == SEMICN) {
                                                }
                                                else {
                                                    return false;
                                                }
                                            }
                                        }
                                    }
                                    else {  // is not identifier
                                        return false;
                                    }
                                }
                                else {
                                    break;
                                }
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


//＜variable defination＞  ::= ＜type＞(＜identifier＞|<identifier＞'['＜unsigned integer＞']')
//                              {,(＜identifier＞|＜identifier＞'['＜unsigned integer＞']' )}
bool variableDefinition(bool isglobal) {
    string name;
    int type;
    if (symbol == INTTK || symbol == CHARTK) {
        if (symbol == INTTK) {
            type = 1;
        }
        else {
            type = 2;
        }
        int re = getsym();
        if (symbol == IDENFR) {
            name = string(token);
            re = getsym();
            if (symbol == LBRACK) {  // "["
                re = getsym();
                int conInt;
                if (!unsignedInteger(conInt)) {
                    return false;
                }
                if (symbol != RBRACK) {
                    retractString(oldIndex);
                    raiseError("Expect \"]\"", line);
                    // errorfile << line << " m\n";
                    symbol = RBRACK;
                }
                if (symbol != RBRACK) {
                    return false;
                }
                else {  // "]"
                    if (isglobal) {
                        if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                            globalSymbolTable.insert(make_pair(name, symbolItem(name, globalAddr, 4, type, 0, ' ', conInt)));
                            globalAddr += conInt;
                        }
                        else {
                            raiseError("Redefination", line);
                            // errorfile << line << " b\n";
                        }
                    }
                    else {
                        if (localSymbolTable.find(name) == localSymbolTable.end()) {
                            localSymbolTable.insert(make_pair(name, symbolItem(name, localAddr, 4, type, 0, ' ', conInt)));
                            localAddr += conInt;
                        }
                        else {
                            raiseError("Redefination", line);
                            // errorfile << line << " b\n";
                        }
                    }
                    re = getsym();
                    midCodeTable.push_back(midCode(ARRAY, type==1 ? "int" : "char", name, int2string(conInt)));
                }
            }
            else {
                if (isglobal) {
                    if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                        globalSymbolTable.insert(make_pair(name, symbolItem(name, globalAddr, 1, type)));
                        globalAddr++;
                    }
                    else {
                        raiseError("Redefination", line);
                        //errorfile << line << " b\n";
                    }
                }
                else {
                    if (localSymbolTable.find(name) == localSymbolTable.end()) {
                        localSymbolTable.insert(make_pair(name, symbolItem(name, localAddr, 1, type)));
                        localAddr++;
                    }
                    else {
                        raiseError("Redefination", line);
                        // errorfile << line << " b\n";
                    }
                }
                midCodeTable.push_back(midCode(VAR, type == 1 ? "int" : "char", name, ""));
            }
            while (true) {
                if (symbol != COMMA) {
                    break;
                }
                re = getsym();
                if (symbol == IDENFR) {
                    name = string(token);
                    re = getsym();
                    if (symbol == LBRACK) {  // "["
                        re = getsym();
                        int conInt;
                        if (!unsignedInteger(conInt)) {
                            return false;
                        }
                        if (symbol != RBRACK) {
                            retractString(oldIndex);
                            raiseError("Expect \"]\"", line);
                            // errorfile << line << " m\n";
                            symbol = RBRACK;
                        }
                        if (symbol != RBRACK) {
                            return false;
                        }
                        else {  // ]
                            if (isglobal) {
                                if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                                    globalSymbolTable.insert(make_pair(name, symbolItem(name, globalAddr, 4, type, 0, ' ', conInt)));
                                    globalAddr += conInt;
                                }
                                else {
                                    raiseError("Redefination", line);
                                    // errorfile << line << " b\n";
                                }
                            }
                            else {
                                if (localSymbolTable.find(name) == localSymbolTable.end()) {
                                    localSymbolTable.insert(make_pair(name, symbolItem(name, localAddr, 4, type, 0, ' ', conInt)));
                                    localAddr += conInt;
                                }
                                else {
                                    raiseError("Redefination", line);
                                    // errorfile << line << " b\n";
                                }
                            }
                            re = getsym();
                            midCodeTable.push_back(midCode(ARRAY, type == 1 ? "int" : "char", name, int2string(conInt)));
                        }
                    }
                    else {
                        if (isglobal) {
                            if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
                                globalSymbolTable.insert(make_pair(name, symbolItem(name, globalAddr, 1, type)));
                                globalAddr++;
                            }
                            else {
                                raiseError("Redefination", line);
                                // errorfile << line << " b\n";
                            }
                        }
                        else {
                            if (localSymbolTable.find(name) == localSymbolTable.end()) {
                                localSymbolTable.insert(make_pair(name, symbolItem(name, localAddr, 1, type)));
                                localAddr++;
                            }
                            else {
                                raiseError("Redefination", line);
                                // errorfile << line << " b\n";
                            }
                        }
                        midCodeTable.push_back(midCode(VAR, type == 1 ? "int" : "char", name, ""));
                    }
                }
                else {
                    return false;
                }
            }
            return true;
            //}
        }
    }
    return false;
}

//＜Function with return value＞  ::=  ＜declaration head＞'('＜parameter table＞')' '{'＜compound statement＞'}’
bool functionWithReturnValue() {
    string name;
    int type;
    if (!declarationHead(name, type)) {
        return false;
    }
    bool isRedefine = false;
    if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
        globalSymbolTable.insert(make_pair(name, symbolItem(name, -1, 3, type)));
        midCodeTable.push_back(midCode(FUNC, type == 1 ? "int" : "char", name, ""));
    }
    else {
        raiseError("Redefination", line);
        // errorfile << line << " b\n";
        isRedefine = true;
    }
    curFuncReturnType = type;
    if (isEOF()) {
        return false;
    }
    if (symbol == LPARENT) {
        int re = getsym();
        if (!parameterTable(name, isRedefine)) {
            return false;
        }
        if (isEOF()) {
            return false;
        }
        if (symbol != RPARENT) {
            retractString(oldIndex);
            raiseError("Expect \"\)\"", line);
            // errorfile << line << " l\n";
            symbol = RPARENT;
        }
        if (symbol == RPARENT) {
            re = getsym();
            if (symbol != LBRACE) {
                return false;
            }
            re = getsym();
            realReturnType = -1;
            if (!compoundStatement()) {
                return false;
            }
            if (isEOF()) {
                return false;
            }
            if (symbol == RBRACE) {
                if (realReturnType == -1) {
                    raiseError("Expect \"return\" for function with return value", line);
                    // errorfile << line << " h\n";
                }
                re = getsym();
                showLocal();
                allLocalSymbolTable.insert(make_pair(name, localSymbolTable));
                globalSymbolTable[name].length = localAddr;
                localSymbolTable.clear();
                localAddr = 0;
                curFuncReturnType = -1;
                return true;
            }
        }
    }
    return false;
}

//＜function without return value＞  ::= void＜identifier＞'('＜parameter table＞')''{'＜compound statment＞'}’
bool functionWithoutReturnValue() {
    // Need to distinguish used defined function and and main function.
    string name;
    int old = oldIndex;   // the initial position of void function.
    if (symbol == VOIDTK) {
        int re = getsym(0);
        if (symbol != IDENFR && symbol != MAINTK) {  // is not identifier or main function.
            return false;
        }
        if (symbol == MAINTK) {  // is main function, need to return back.
            retractString(old);
            getsym(0);
            return false;
        }

        symbol = VOIDTK;
        symbol = IDENFR;
        name = string(token);
        bool isRedefine = false;
        if (globalSymbolTable.find(name) == globalSymbolTable.end()) {
            globalSymbolTable.insert(make_pair(name, symbolItem(name, -1, 3, 3)));
            midCodeTable.push_back(midCode(FUNC, "void", name, ""));
        }
        else {
            raiseError("Redefination", line);
            // errorfile << line << " b\n";
            isRedefine = true;
        }
        curFuncReturnType = 3;  // void
        re = getsym();
        if (symbol == LPARENT) {
            re = getsym();
            // start to analyze parameter table.
            if (!parameterTable(name, isRedefine)) {
                return false;
            }
            if (isEOF()) {
                return false;
            }
            if (symbol != RPARENT) {
                retractString(oldIndex);
                raiseError("Expect \"\)\"", line);
                // errorfile << line << " l\n";
                symbol = RPARENT;
            }
            if (symbol == RPARENT) {
                re = getsym();
                if (symbol != LBRACE) {
                    return false;
                }
                re = getsym();
                realReturnType = -1;
                if (!compoundStatement()) {  // start to analyze compound statment.
                    return false;
                }
                if (isEOF()) {
                    return false;
                }
                if (symbol == RBRACE) {
                    re = getsym();
                    showLocal();
                    allLocalSymbolTable.insert(make_pair(name, localSymbolTable));
                    globalSymbolTable[name].length = localAddr;
                    localSymbolTable.clear();
                    localAddr = 0;
                    curFuncReturnType = -1;
                    return true;
                }
            }
        }
    }
    return false;
}

//＜parameter table＞    ::=  ＜type＞＜identifier＞{,＜type＞＜identifier＞}| ＜null＞
bool parameterTable(string funcName, bool isRedefine) {
    string name;
    int type;
    if (symbol == RPARENT || symbol == LBRACE) {  //) {
        return true;
    }
    if (symbol == INTTK || symbol == CHARTK) {
        if (symbol == INTTK) {
            type = 1;
        }
        else {
            type = 2;
        }
        int re = getsym();
        if (symbol != IDENFR) {
            return false;
        }
        name = string(token);
        if (localSymbolTable.find(name) == localSymbolTable.end()) {
            localSymbolTable.insert(make_pair(name, symbolItem(name, localAddr, 1, type)));
            localAddr++;
            if (!isRedefine) {
                globalSymbolTable[funcName].insert(type);
            }
            midCodeTable.push_back(midCode(PARAM, type == 1 ? "int" : "char", name, ""));
        }
        else {
            raiseError("Redefination.", line);
            // errorfile << line << " b\n";
        }
        while (true) {
            re = getsym();
            if (symbol != COMMA) {
                break;
            }
            re = getsym();
            if (symbol != INTTK && symbol != CHARTK) {
                return false;
            }
            if (symbol == INTTK) {
                type = 1;
            }
            else {
                type = 2;
            }
            re = getsym();
            if (symbol != IDENFR) {
                return false;
            }
            name = string(token);
            if (localSymbolTable.find(name) == localSymbolTable.end()) {
                localSymbolTable.insert(make_pair(name, symbolItem(name, localAddr, 1, type)));
                localAddr++;
                if (!isRedefine) {
                    globalSymbolTable[funcName].insert(type);
                }
                midCodeTable.push_back(midCode(PARAM, type == 1 ? "int" : "char", name, ""));
            }
            else {
                raiseError("Redefination.", line);
                // errorfile << line << " b\n";
            }
        }
        return true;
    }
    else
        return false;
}

//＜compound statement＞  ::=  ［＜constant declaration＞］［＜vairable declaration＞］＜statement list＞
bool compoundStatement() {
    constDeclaration(false);

    variableDeclaration(false);

    if (statementList()) {
        return true;
    }
    else
        return false;
}

//＜main function＞    ::= void main‘(’‘)’ ‘{’＜compound statement＞‘}’
bool mainFunction() {
    if (symbol == VOIDTK) {
        int re = getsym();
        if (symbol == MAINTK) {
            isMain = true;
            re = getsym();
            globalSymbolTable.insert(make_pair("main", symbolItem("main", -1, 3, 3)));
            midCodeTable.push_back(midCode(FUNC, "void", "main", ""));
            if (symbol == LPARENT) {
                re = getsym();
                if (symbol != RPARENT) {
                    retractString(oldIndex);
                    raiseError("Expect \"\)\"", line);
                    // errorfile << line << " l\n";
                    symbol = RPARENT;
                }
                if (symbol == RPARENT) {
                    re = getsym();
                    if (symbol == LBRACE) {
                        re = getsym();
                        if (!compoundStatement()) { // Start to analyze compound statement.
                            return false;
                        }
                        if (isEOF()) {
                            return false;
                        }
                        if (symbol == RBRACE) {
                            re = getsym();
                            showLocal();
                            allLocalSymbolTable.insert(make_pair("main", localSymbolTable));
                            globalSymbolTable["main"].length = localAddr;
                            localSymbolTable.clear();
                            localAddr = 0;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

//＜expression＞    ::= ［＋｜－］＜item＞{＜addition expression＞＜item＞}
bool expression(int& type, string& ansTmp) {
    bool first = false;
    int re;
    bool isPLUS;
    if (symbol == PLUS || symbol == MINU) {
        isPLUS = (symbol == PLUS);
        re = getsym();
        first = true;
    }
    string op1, op2, res;
    if (!item(type, op1)) {
        return false;
    }
    if (first) {
        if (!isPLUS) {
            res = genTmp();
            localSymbolTable.insert(make_pair(res, symbolItem(res, localAddr, 1, 1)));  //kind=1=var,type=1=int
            localAddr++;
            midCodeTable.push_back(midCode(MINUOP, res, int2string(0), op1));
            op1 = res;
        }
    }
    bool flag = false;
    while (true) {  // Start to analyze {＜addition expression＞＜item＞}
        if (isEOF()) {
            break;
        }
        if (symbol != PLUS && symbol != MINU) {  //is not eithor + or -
            break;
        }
        isPLUS = (symbol == PLUS);
        flag = true;
        re = getsym();
        if (!item(type, op2)) {
            return false;
        }
        res = genTmp();
        localSymbolTable.insert(make_pair(res, symbolItem(res, localAddr, 1, 1)));  //kind=1=var,type=1=int
        localAddr++;
        midCodeTable.push_back(midCode(isPLUS ? PLUSOP : MINUOP, res, op1, op2));
        op1 = res;
    }
    if (first) {
        type = 1;
    }
    else {
        if (flag) {
            type = 1;
        }
    }
    ansTmp = op1;
    return true;
}

//＜item＞     ::= ＜factor＞{＜Multiplicative  expression＞＜factor＞}
bool item(int& type, string& ansTmp) {
    string op1, op2, res;
    if (!factor(type, op1)) {
        return false;
    }
    bool flag = false;
    bool isMULT;
    while (true) {
        if (isEOF()) {
            break;
        }
        if (symbol != MULT && symbol != DIV) {
            break;
        }
        isMULT = (symbol == MULT);
        flag = true;
        int re = getsym();
        if (!factor(type, op2)) {
            return false;
        }
        res = genTmp();
        localSymbolTable.insert(make_pair(res, symbolItem(res, localAddr, 1, 1)));  //kind=1=var,type=1=int
        localAddr++;
        midCodeTable.push_back(midCode(isMULT ? MULTOP : DIVOP, res, op1, op2));
        op1 = res;
    }
    if (flag) {
        type = 1;
    }
    ansTmp = op1;
    return true;
}

//＜factor＞    ::= ＜type＞｜＜type＞'['＜expression＞']'|'('＜expression＞')'｜＜integer＞|＜char＞｜＜function with return value＞
bool factor(int& type, string& ansTmp) {
    int re;
    int old = oldIndex;
    int conInt;
    if (symbol == IDENFR) {
        re = getsym(0);
        if (symbol == LBRACK) {  // is "["
            symbol = IDENFR;
            string name = string(token);
            if (localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind == 4) {
                type = localSymbolTable[name].type;
            }
            else {
                if (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind == 4) {
                    type = globalSymbolTable[name].type;
                }
                else {
                    raiseError("Undefined identifier.", line);
                    // errorfile << line << " c\n";
                }
            }
            symbol = LBRACK;
            re = getsym();
            // Start to analyze expression.
            int t;
            string op1;
            if (!expression(t, op1)) {
                return false;
            }
            if (t != 1) {
                raiseError("Subscript of array element is not int.", line);
                // errorfile << line << " i\n";
            }
            if (symbol != RBRACK) {
                retractString(oldIndex);
                raiseError("Expect \"]\"", line);
                // errorfile << line << " m\n";
                symbol = RBRACK;
            }
            if (symbol == RBRACK) {  //is "]"
                re = getsym();
                string op2 = genTmp();
                localSymbolTable.insert(make_pair(op2, symbolItem(op2, localAddr, 1, type)));
                localAddr++;
                midCodeTable.push_back(midCode(GETARRAY, op2, name, op1));
                ansTmp = op2;
                return true;
            }
            else {
                return false;
            }
        }
        else if (symbol == LPARENT) {
            retractString(old);
            getsym(0);
            string name = string(token);
            if (globalSymbolTable.find(name) != globalSymbolTable.end()
                && globalSymbolTable[name].kind == 3
                && (globalSymbolTable[name].type == 1 || globalSymbolTable[name].type == 2)
                && localSymbolTable.find(name) == localSymbolTable.end()
                ) {
                if (!callFunctionWithReturnValue()) {
                    return false;
                }
                type = globalSymbolTable[name].type;
                string op1 = genTmp();
                localSymbolTable.insert(make_pair(op1, symbolItem(op1, localAddr, 1, type)));
                localAddr++;
                midCodeTable.push_back(midCode(RETVALUE, op1, "RET", ""));
                ansTmp = op1;
                return true;
            }
            else {
                raiseError("Undefined identifier.", line);
                // errorfile << line << " c\n";
                while (1) {
                    get_ch();
                    if (isRparent()) {
                        break;
                    }
                }
                getsym();
                return true;
            }
        }
        else {
            retractString(old);
            getsym(0);
            string name = string(token);
            if (localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind != 3) {
                type = localSymbolTable[name].type;
            }
            else {
                if (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind != 3) {
                    type = globalSymbolTable[name].type;
                }
                else {
                    raiseError("Undefined identifier.", line);
                    // errorfile << line << " c\n";
                }
            }
            ansTmp = name;
            getsym();
            return true;
        }
    }
    else if (symbol == LPARENT) {
        re = getsym();
        if (!expression(type, ansTmp)) {
            return false;
        }
        type = 1;
        if (symbol != RPARENT) {
            retractString(oldIndex);
            raiseError("Expect \"\)\"", line);
            // errorfile << line << " l\n";
            symbol = RPARENT;
        }
        if (symbol == RPARENT) {
            re = getsym();
            return true;
        }
        else
            return false;
    }
    else if (symbol == CHARCON) {
        type = 2;
        re = getsym();
        ansTmp = int2string(con_ch);
        return true;
    }
    else if (integer(conInt)) {
        type = 1;
        ansTmp = int2string(conInt);
        return true;
    }
    else {
        type = 0;
        return false;
    }
}

//＜statement＞    ::= ＜conditional statement＞｜＜loop statement＞| '{'＜statement list＞'}'| ＜function with return value＞;
//              |＜function without return value＞;｜＜assignment statement＞;｜＜write statement＞;｜＜null＞;|＜PUTARRAY＞;
bool statement() {
    if (symbol == SEMICN) {
        getsym();
        return true;
    }
    else if (symbol == RETURNTK) {  //＜return statement＞;
        if (returnStatement()) {
            if (symbol != SEMICN) {
                retractString(oldIndex);
                raiseError("Expect semicolon.", line);
                // errorfile << line << " k\n";
                symbol = SEMICN;
            }
            if (symbol == SEMICN) {
                getsym();
                return true;
            }
        }
        return false;
    }

    else if (symbol == PRINTFTK) {  //＜write statement＞;
        if (writeStatement()) {
            if (symbol != SEMICN) {
                retractString(oldIndex);
                raiseError("Expect semicolon.", line);
                // errorfile << line << " k\n";
                symbol = SEMICN;
            }
            if (symbol == SEMICN) {
                getsym();
                return true;
            }
        }
        return false;
    }
    else if (symbol == IFTK) {  //＜conditional statement＞
        if (conditionalStatement()) {
            return true;
        }
        else
            return false;
    }
    else if (symbol == WHILETK || symbol == DOTK || symbol == FORTK) {  //＜loop statement＞
        if (loopStatement()) {
            return true;
        }
        else
            return false;
    }
    else if (symbol == LBRACE) {  //'{'＜statement list＞'}'
        int re = getsym();
        if (!statementList()) {
            return false;
        }
        if (symbol == RBRACE) {  // "}"
            getsym();
            return true;
        }
        else
            return false;
    }
    else if (symbol == IDENFR) {  //＜function with return value＞; |＜funciont without return value＞;｜＜assignment statement＞;
        int old = oldIndex;
        int re = getsym(0);
        if (symbol == LBRACK || symbol == ASSIGN) {
            retractString(old);
            getsym(0);
            if (!assignmentSataement()) {
                return false;
            }
            if (symbol != SEMICN) {
                retractString(oldIndex);
                raiseError("Expect semicolon.", line);
                // errorfile << line << " k\n";
                symbol = SEMICN;
            }
            if (symbol == SEMICN) {  // ";"
                getsym();
                return true;
            }
            else
                return false;
        }
        else if (symbol == LPARENT) {
            retractString(old);
            getsym(0);
            string name = string(token);
            if (globalSymbolTable.find(name) != globalSymbolTable.end()
                && globalSymbolTable[name].kind == 3
                && localSymbolTable.find(name) == localSymbolTable.end()
                ) {
                if (globalSymbolTable[name].type == 3) {  //func void
                    if (!callFunctinWithouReturnValue()) {
                        return false;
                    }
                    if (symbol != SEMICN) {
                        retractString(oldIndex);
                        raiseError("Expect semicolon.", line);
                        // errorfile << line << " k\n";
                        symbol = SEMICN;
                    }
                    if (symbol == SEMICN) {  // ";"
                        getsym();
                        return true;
                    }
                    else
                        return false;
                }
                else {  //func int char
                    if (!callFunctionWithReturnValue()) {
                        return false;
                    }
                    if (symbol != SEMICN) {
                        retractString(oldIndex);
                        raiseError("Expect semicolon.", line);
                        // errorfile << line << " k\n";
                        symbol = SEMICN;
                    }
                    if (symbol == SEMICN) {  // ";"
                        getsym();
                        return true;
                    }
                    else {
                        return false;
                    }
                }
            }
            else {
                raiseError("Undefined identifier.", line);
                // errorfile << line << " c\n";
                while (1) {
                    get_ch();
                    if (isSemicn()) {  // ";"
                        break;
                    }
                }
                getsym();
                return true;
            }
        }
        else {
            return false;
        }
    }
    else if (symbol == ELSETK || symbol == RBRACE) {
        raiseError("", line);
        // errorfile << line << " k\n";
        return true;
    }
    else
        return false;
}

//＜assignment statement＞   ::=  ＜identifier＞＝＜expression＞|＜identifier＞'['＜expression＞']'=＜expression＞
bool assignmentSataement() {
    string value;
    if (symbol == IDENFR) {
        string name = string(token);
        int re = getsym();
        if (symbol == LBRACK) {
            if (!((localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind == 4)
                || (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind == 4))
                ) {
                raiseError("Undefined identifier", line);
                // errorfile << line << " c\n";
            }
            re = getsym();
            // Start to analyze expression.
            int t;
            string op1;
            if (!expression(t, op1)) {
                return false;
            }
            if (t != 1) {
                raiseError(".", line);
                // errorfile << line << " i\n";
            }

            if (symbol != RBRACK) {
                retractString(oldIndex);
                raiseError("Expect \"]\".", line);
                // errorfile << line << " m\n";
                symbol = RBRACK;
            }
            if (symbol == RBRACK) {  // "]"
                re = getsym();
                if (symbol == ASSIGN) {  // =
                    re = getsym();
                    int tt;
                    if (!expression(tt, value)) {
                        return false;
                    }
                    midCodeTable.push_back(midCode(PUTARRAY, name, op1, value));
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else  if (symbol == ASSIGN) {
            if (localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind != 3) {
                if (localSymbolTable[name].kind == 2) {  //const
                    raiseError("Try to change a constant value.", line);
                    // errorfile << line << " j\n";
                }
            }
            else {
                if (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind != 3) {
                    if (globalSymbolTable[name].kind == 2) {  //const
                        raiseError("Try to change a constant value.", line);
                        // errorfile << line << " j\n";
                    }
                }
                else {
                    raiseError("Undefined identifier.", line);
                    // errorfile << line << " c\n";
                }
            }
            re = getsym();
            // start to analyze expression.
            int t;
            if (!expression(t, value)) {
                return false;
            }
            midCodeTable.push_back(midCode(ASSIGNOP, name, value, ""));
            return true;
        }
    }
    return false;
}

//＜conditional statement＞  ::= if '('＜condition＞')'＜statement＞［else＜statement＞］
bool conditionalStatement() {
    string laba, labb;
    if (symbol == IFTK) {  // if
        int re = getsym();
        if (symbol == LPARENT) {  // (
            re = getsym();
            string result;
            if (!condition(result)) {  // analyze <condition>
                return false;
            }
            if (symbol != RPARENT) {
                retractString(oldIndex);
                raiseError("Expect \"\)\".", line);
                // errorfile << line << " l\n";
                symbol = RPARENT;
            }
            if (symbol == RPARENT) {  // )
                re = getsym();
                laba = genLabel();
                midCodeTable.push_back(midCode(BZ, laba, result, ""));
                if (!statement()) {
                    return false;
                }
                if (symbol == ELSETK) {  // else
                    labb = genLabel();
                    midCodeTable.push_back(midCode(GOTO, labb, "", ""));
                    midCodeTable.push_back(midCode(LABEL, laba, "", ""));
                    re = getsym();
                    if (!statement()) {
                        return false;
                    }
                    midCodeTable.push_back(midCode(LABEL, labb, "", ""));
                }
                else {
                    midCodeTable.push_back(midCode(LABEL, laba, "", ""));
                }
                return true;
            }
        }
    }
    return false;
}

//＜condition＞  ::=  ＜expression＞＜relational operator＞＜expression＞｜＜expression＞
bool condition(string& result) {
    result = genTmp();
    localSymbolTable.insert(make_pair(result, symbolItem(result, localAddr, 1, 1)));  //kind=1=var,type=1=int
    localAddr++;
    int typeLeft, typeRight;
    string v1, v2;
    if (!expression(typeLeft, v1)) {
        return false;
    }
    if (symbol == LSS || symbol == LEQ || symbol == GRE || symbol == GEQ || symbol == EQL || symbol == NEQ) {  // relational operator
        operation op;
        switch (symbol) {
            case LSS:
                op = LSSOP; break;
            case LEQ:
                op = LEQOP; break;
            case GRE:
                op = GREOP; break;
            case GEQ:
                op = GEQOP; break;
            case EQL:
                op = EQLOP; break;
            case NEQ:
                op = NEQOP; break;
        }
        int re = getsym();
        if (!expression(typeRight, v2)) {
            return false;
        }
        if (typeLeft != 1 || typeRight != 1) {
            raiseError("Illegal type in conditional expression. All type must be int.", line);
            // errorfile << line << " f\n";
        }
        midCodeTable.push_back(midCode(op, result, v1, v2));
        return true;
    }
    else {
        if (typeLeft != 1) {
            raiseError("Illegal type in conditional expression. All type must be int.", line);
            // errorfile << line << " f\n";
        }
        // only one expresion i.e. expression != 0;
        midCodeTable.push_back(midCode(NEQOP, result, v1, int2string(0)));
        return true;
    }
}

//＜loop statement＞   ::=  while '('＜condition＞')'＜statement＞| do＜statement＞while '('＜condition＞')'
//              |for'('＜identifier＞＝＜expression＞;＜condition＞;＜identifier＞＝＜identifier＞(+|-)＜step＞')'＜state＞
bool loopStatement() {
    if (symbol == WHILETK) {  //while '('＜condition＞')'＜statement＞
        string labr, labf;
        labr = genLabel();
        midCodeTable.push_back(midCode(LABEL, labr, "", ""));
        int re = getsym();
        if (symbol == LPARENT) {  // (
            re = getsym();
            string result;
            if (!condition(result)) {
                return false;
            }
            if (symbol != RPARENT) {
                retractString(oldIndex);
                raiseError("Expect \"\)\".", line);
                // errorfile << line << " l\n";
                symbol = RPARENT;
            }
            if (symbol == RPARENT) {  // )
                re = getsym();
                labf = genLabel();
                midCodeTable.push_back(midCode(BZ, labf, result, ""));
                if (!statement()) {  // analyze statement.
                    return false;
                }
                midCodeTable.push_back(midCode(GOTO, labr, "", ""));
                midCodeTable.push_back(midCode(LABEL, labf, "", ""));
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else if (symbol == DOTK) {  // do＜statement＞while '('＜condition＞')'
        string labr = genLabel();
        midCodeTable.push_back(midCode(LABEL, labr, "", ""));
        int re = getsym();
        if (!statement()) {
            return false;
        }
        if (symbol != WHILETK) {
            retractString(oldIndex);
            raiseError("Expect \"while\".", line);
            // errorfile << line << " n\n";
            symbol = WHILETK;
        }
        if (symbol == WHILETK) {
            re = getsym();
            if (symbol == LPARENT) {  // (
                re = getsym();
                string result;
                if (!condition(result)) {
                    return false;
                }
                if (symbol != RPARENT) {
                    retractString(oldIndex);
                    raiseError("Expect \"\)\".", line);
                    // errorfile << line << " l\n";
                    symbol = RPARENT;
                }
                if (symbol == RPARENT) {  //)
                    midCodeTable.push_back(midCode(BNZ, labr, result, ""));
                    getsym();
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }
    else if (symbol == FORTK) {  //for'('＜identifier＞＝＜expression＞;＜condition＞;＜identifier＞＝＜identifier＞(+|-)＜step＞')'＜statement＞
        string lbegin, lend;
        int re = getsym();
        if (symbol != LPARENT) {  // (
            return false;
        }
        re = getsym();
        if (symbol != IDENFR) {
            return false;
        }
        string name = string(token);
        if (localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind != 3) {
            if (localSymbolTable[name].kind == 2) {  // const
                raiseError("Try to change a constant value", line);
                // errorfile << line << " j\n";
            }
        }
        else {
            if (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind != 3) {
                if (globalSymbolTable[name].kind == 2) {  //const
                    raiseError("Try to change a constant value", line);
                    // errorfile << line << " j\n";
                }
            }
            else {
                raiseError("Undefined identifier", line);
                // errorfile << line << " c\n";
            }
        }
        re = getsym();
        if (symbol != ASSIGN) {  //is not =
            return false;
        }
        re = getsym();
        int t;
        string value;
        if (!expression(t, value)) {  // analyze expression.
            return false;
        }
        midCodeTable.push_back(midCode(ASSIGNOP, name, value, ""));
        if (symbol != SEMICN) {
            retractString(oldIndex);
            raiseError("Expect semicolon.", line);
            // errorfile << line << " k\n";
            symbol = SEMICN;
        }
        if (symbol == SEMICN) {  //;
            re = getsym();
        }
        else {
            return false;
        }
        lbegin = genLabel();
        midCodeTable.push_back(midCode(LABEL, lbegin, "", ""));
        string result;
        if (!condition(result)) {
            return false;
        }
        lend = genLabel();
        midCodeTable.push_back(midCode(BZ, lend, result, ""));
        if (symbol != SEMICN) {
            retractString(oldIndex);
            raiseError("Expect semicolon.", line);
            // errorfile << line << " k\n";
            symbol = SEMICN;
        }
        if (symbol == SEMICN) {  //;
            re = getsym();
        }
        else {
            return false;
        }
        if (symbol != IDENFR) {
            return false;
        }
        name = string(token);
        if (localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind != 3) {
            if (localSymbolTable[name].kind == 2) {  //const
                raiseError("Try to change a constant value", line);
                // errorfile << line << " j\n";
            }
        }
        else {
            if (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind != 3) {
                if (globalSymbolTable[name].kind == 2) {  //const
                    raiseError("Try to change a constant value", line);
                    //  errorfile << line << " j\n";
                }
            }
            else {
                raiseError("Undefined identifier", line);
                // errorfile << line << " c\n";
            }
        }
        string nameLeft = name;
        re = getsym();
        if (symbol != ASSIGN) {
            return false;
        }
        re = getsym();
        if (symbol != IDENFR) {
            return false;
        }
        name = string(token);
        if (localSymbolTable.find(name) != localSymbolTable.end() && localSymbolTable[name].kind != 3) {
            if (localSymbolTable[name].kind == 2) {  //const
                raiseError("Try to change a constant value", line);
                // errorfile << line << " j\n";
            }
        }
        else {
            if (globalSymbolTable.find(name) != globalSymbolTable.end() && globalSymbolTable[name].kind != 3) {
                if (globalSymbolTable[name].kind == 2) {  //const
                    raiseError("Try to change a constant value", line);
                    // errorfile << line << " j\n";
                }
            }
            else {
                raiseError("Undefined identifier", line);
                // errorfile << line << " c\n";
            }
        }
        string nameRight = name;
        re = getsym();
        if (symbol != PLUS && symbol != MINU) {
            return false;
        }
        bool isPLUS = (symbol == PLUS);
        re = getsym();
        int conInt;
        if (!step(conInt)) {
            return false;
        }
        string stepNum = int2string(conInt);
        if (symbol != RPARENT) {
            retractString(oldIndex);
            raiseError("Expect \"\)\".", line);
            // errorfile << line << " l\n";
            symbol = RPARENT;
        }
        if (symbol != RPARENT) {
            return false;
        }
        re = getsym();
        if (!statement()) {
            return false;
        }
        midCodeTable.push_back(midCode(isPLUS ? PLUSOP : MINUOP, nameLeft, nameRight, stepNum));
        midCodeTable.push_back(midCode(GOTO, lbegin, "", ""));
        midCodeTable.push_back(midCode(LABEL, lend, "", ""));
        return true;
    }
    else {
        return false;
    }
}

//＜step＞::= ＜unsigned integer＞
bool step(int& value) {
    if (unsignedInteger(value)) {
        return true;
    }
    else
        return false;
}

//＜call function with return value＞ ::= ＜identifier＞'('＜value parameter list＞')’
bool callFunctionWithReturnValue() {
    if (symbol == IDENFR) {
        string name = string(token);
        //midCodeTable.push_back(midCode(CALL, name, "", ""));
        int re = getsym();
        if (symbol == LPARENT) {  // (
            re = getsym();
            if (!valueParameterTable(name)) {
                return false;
            }
            if (symbol != RPARENT) {
                retractString(oldIndex);
                raiseError("Expect \"\)\".", line);
                // errorfile << line << " l\n";
                symbol = RPARENT;
            }
            if (symbol == RPARENT) {  // )
                midCodeTable.push_back(midCode(CALL, name, "", ""));
                getsym();
                return true;
            }
        }
    }
    return false;
}

//＜call function without return value＞ ::= ＜identifier＞'('＜value parameter table＞')’
bool callFunctinWithouReturnValue() {
    if (symbol == IDENFR) {
        string name = string(token);
        //midCodeTable.push_back(midCode(CALL, name, "", ""));
        int re = getsym();
        if (symbol == LPARENT) {  // (
            re = getsym();
            if (!valueParameterTable(name)) {
                return false;
            }
            if (symbol != RPARENT) {
                retractString(oldIndex);
                raiseError("Expect \"\)\".", line);
                // errorfile << line << " l\n";
                symbol = RPARENT;
            }
            if (symbol == RPARENT) {  // )
                midCodeTable.push_back(midCode(CALL, name, "", ""));
                getsym();
                return true;
            }
        }
    }
    return false;
}

//＜value parameter table＞  ::= ＜expression＞{,＜expression＞}｜＜null＞
bool valueParameterTable(string funcName) {
    vector<int> typeList;  // type of value parameter table
    int type;
    string value;
    if (!expression(type, value)) {
        //if (symbol == RPARENT) {
        if (globalSymbolTable[funcName].parameterTable.size() != 0) {
            raiseError("The number of parameters does not match.", line);
            // errorfile << line << " d\n";
        }
        return true;
    }
    typeList.push_back(type);
    midCodeTable.push_back(midCode(PUSH, value, "", ""));
    while (true) {
        if (isEOF()) {
            break;
        }
        if (symbol != COMMA) {
            break;
        }
        int re = getsym();
        if (!expression(type, value)) {
            return false;
        }
        typeList.push_back(type);
        midCodeTable.push_back(midCode(PUSH, value, "", ""));
    }
    if (typeList.size() != globalSymbolTable[funcName].parameterTable.size()) {
        raiseError("The number of parameters does not match", line);
        // errorfile << line << " d\n";
    }
    else {
        for (int i = 0; i < typeList.size(); i++) {
            if (typeList[i] != globalSymbolTable[funcName].parameterTable[i]) {
                raiseError("The number of parameters does not match", line);
                // errorfile << line << " e\n";
                break;
            }
        }
    }
    return true;
}

//＜statement list＞   ::= ｛＜statement＞｝
bool statementList() {
    while (symbol != RBRACE) {  // while (true)
        if (!statement()) {
            break;
        }
    }
    return true;
}


//＜write statement＞    ::= printf '(' ＜string＞,＜expression＞ ')'| printf '('＜string＞ ')'| printf '('＜expression＞')’
bool writeStatement() {
    if (symbol == PRINTFTK) {
        int re = getsym();
        if (symbol == LPARENT) {  // (
            re = getsym();
            if (strings()) {  // string.
                string smodify = string(s);
                int p=-2;
                while ((p = smodify.find("\\", p+2)) != string::npos) {
                    smodify.replace(p, 1, "\\\\");
                }
                stringList.push_back(smodify);
                if (symbol == COMMA) {  //,  printf '(' ＜string＞,＜expression＞ ')'
                    midCodeTable.push_back(midCode(PRINT, smodify, "3", ""));
                    re = getsym();
                    int t;
                    string value;
                    if (!expression(t, value)) {
                        return false;
                    }
                    midCodeTable.push_back(midCode(PRINT, value, int2string(t), ""));
                    if (symbol != RPARENT) {
                        retractString(oldIndex);
                        raiseError("Expect \"\)\".", line);
                        // errorfile << line << " l\n";
                        symbol = RPARENT;
                    }
                    if (symbol == RPARENT) {  //)
                        midCodeTable.push_back(midCode(PRINT, "EndLine", "4", ""));
                        getsym();
                        return true;
                    }
                    else {
                        return false;
                    }
                }
                else {
                    if (symbol != RPARENT) {
                        retractString(oldIndex);
                        raiseError("Expect \"\)\".", line);
                        // errorfile << line << " l\n";
                        symbol = RPARENT;
                    }
                    if (symbol == RPARENT) {  //)  printf '('＜string＞ ')'
                        midCodeTable.push_back(midCode(PRINT, smodify, "3", ""));
                        midCodeTable.push_back(midCode(PRINT, "EndLine", "4", ""));
                        getsym();
                        return true;
                    }
                    else
                        return false;
                }
            }
            else {
                int t;
                string value;
                if (!expression(t, value)) {
                    return false;
                }
                midCodeTable.push_back(midCode(PRINT, value, int2string(t), ""));
                if (symbol != RPARENT) {
                    retractString(oldIndex);
                    raiseError("Expect \"\)\".", line);
                    // errorfile << line << " l\n";
                    symbol = RPARENT;
                }
                if (symbol == RPARENT) {  //)
                    midCodeTable.push_back(midCode(PRINT, "EndLine", "4", ""));
                    getsym();
                    return true;
                }
            }
        }
    }
    return false;

}

//＜return statement＞   ::=  return['('＜expression＞')']
bool returnStatement() {
    if (symbol == RETURNTK) {  //return
        int type;
        int re = getsym();
        if (re < 0) {
            type = 3;
            realReturnType = type;
            if (curFuncReturnType == 1 || curFuncReturnType == 2) {
                raiseError("There is unmatched return statement for function with return value.", line);
                // errorfile << line << " h\n";
            }
            return true;
        }
        if (symbol == LPARENT) {  // (
            re = getsym();
            string value;
            if (!expression(type, value)) {
                return false;
            }
            realReturnType = type;
            if (curFuncReturnType == 3) {
                raiseError("There is unmatched return statement for function without return value.", line);
                // errorfile << line << " g\n";
            }
            else if (curFuncReturnType == 1 || curFuncReturnType == 2) {
                if (curFuncReturnType != type) {
                    raiseError("There is unmatched return statement for function with return value.", line);
                    // errorfile << line << " h\n";
                }
            }
            if (symbol != RPARENT) {
                retractString(oldIndex);
                raiseError("Expect \"\)\".", line);
                // errorfile << line << " l\n";
                symbol = RPARENT;
            }
            if (symbol == RPARENT) {  //)
                if (!isMain) {
                    midCodeTable.push_back(midCode(RET, value, "", ""));
                }
                else {
                    midCodeTable.push_back(midCode(EXIT, "", "", ""));
                }
                getsym();
                return true;
            }
            else
                return false;
        }
        else {
            type = 3;
            realReturnType = type;
            if (curFuncReturnType == 1 || curFuncReturnType == 2) {
                raiseError("There is unmatched return statement for function with return value.", line);
                // errorfile << line << " h\n";
            }
            if (!isMain) {
                midCodeTable.push_back(midCode(RET, "", "", ""));
            }
            else {
                midCodeTable.push_back(midCode(EXIT, "", "", ""));
            }
            return true;
        }
    }
    else
        return false;
}
