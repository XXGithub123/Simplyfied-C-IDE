#pragma once
#ifndef LEXICAL_H
#define LEXICAL_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

enum typeId {
    CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK, IFTK, ELSETK, DOTK, WHILETK, FORTK, PRINTFTK, RETURNTK,  // Reserved word
    IDENFR, INTCON, CHARCON, STRCON,
    PLUS, MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
};

bool isSpace();

bool isNewline();

bool isBlank();

bool isNoDigit();

bool isDigit();

bool isPlus();

bool isMinu();

bool isMult();

bool isDiv();

bool isLss();

bool isGre();

bool isExcla();

bool isAssign();

bool isSemicn();

bool isComma();

bool isLparent();

bool isRparent();

bool isLbrack();

bool isRbrack();

bool isLbrace();

bool isRbrace();

bool isSquo();

bool isDquo();

bool isBackSlash();

bool isEOF();

bool isStringChar();

bool isOperator();

void clearToken();

void formToken();

void get_ch();

void retract();

void retractString(int oldIndex);

int reserver();

int transNum();

void parseWord();

void parseNum();

void parseChar(int out);

void parseString(int out);

void preprocess(int out);

void parseComment(int out);

int getsym(int out=1);


#endif // !LEXICAL_H

