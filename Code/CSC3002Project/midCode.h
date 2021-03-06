#pragma once

#include <string>
using namespace std;

enum operation {
    PLUSOP, //+
    MINUOP, //-
    MULTOP, //*
    DIVOP,  // /
    LSSOP,  //<
    LEQOP,  //<=
    GREOP,  //>
    GEQOP,  //>=
    EQLOP,  //==
    NEQOP,  //!=
    ASSIGNOP,  //=
    GOTO,  // unconditional jump
    BZ,    // jump with dis
    BNZ,   // jump with satisfact
    PUSH,
    CALL,
    RET,
    RETVALUE,
    SCAN,  //read
    PRINT, //print
    LABEL,
    CONST,
    ARRAY,
    VAR,
    FUNC,
    PARAM,
    GETARRAY,  //  t = a[]
    PUTARRAY,  //  a[] = t
    EXIT,  //
};

class midCode {  //z = x op y
public:
    operation op; // operation
    string z;     // result
    string x;     // left operand
    string y;     // right operand
    midCode(operation o, string zz, string xx, string yy) : op(o), z(zz), x(xx), y(yy) {}
};

void outputMidCode();
