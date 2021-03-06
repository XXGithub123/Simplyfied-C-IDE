#pragma once
#ifndef GRAMMER_H
#define GRAMMER_H

#include <string>
using namespace std;

//＜string＞   ::=  "｛ASCII  character from 32 to 126.｝"
bool strings();

// ＜procedure＞  ::= ［＜constant declaration＞］［＜variable declaration＞］
//                     {＜function with return value＞|＜function without return value＞}＜main function＞
bool procedure();

//＜constant declaration＞ ::=  {<const>＜constant definition＞<;>}
bool constDeclaration(bool isglobal);

//＜Concstant Defination＞ ::= {int＜Indentifier＞＝＜Integer＞}
//                  | {char＜Indentifier＞＝＜charicter＞}
bool constDefinition(bool isglobal);

//＜unsigned integer＞  ::= ＜non-zero number＞｛＜number＞｝| <zero>
bool unsignedInteger(int& value);

//＜integer＞ ::= ［＋｜－］＜unsigned integer＞
bool integer(int& value);

//＜declearation head＞   ::=  int＜indentifier＞ |char＜indentifier＞
bool declarationHead(string& tmp, int& type);

//＜variable declaration＞  ::= ＜variable defination＞;{＜variable defination＞;}
bool variableDeclaration(bool isglobal);

//＜variable defination＞  ::= ＜type＞(＜identifier＞|<identifier＞'['＜unsigned integer＞']')
//                              {,(＜identifier＞|＜identifier＞'['＜unsigned integer＞']' )}
bool variableDefinition(bool isglobal);

//＜Function with return value＞  ::=  ＜declaration head＞'('＜parameter table＞')' '{'＜compound statement＞'}’
bool functionWithReturnValue();

//＜function without return value＞  ::= void＜identifier＞'('＜parameter table＞')''{'＜compound statment＞'}’
bool functionWithoutReturnValue();

//＜parameter table＞    ::=  ＜type＞＜identifier＞{,＜type＞＜identifier＞}| ＜null＞
bool parameterTable(string funcName, bool isRedefine);

//＜compound statement＞  ::=  ［＜constant declaration＞］［＜vairable declaration＞］＜statement list＞
bool compoundStatement();

//＜main function＞    ::= void main‘(’‘)’ ‘{’＜compound statement＞‘}’
bool mainFunction();

//＜expression＞    ::= ［＋｜－］＜item＞{＜addition expression＞＜item＞}
bool expression(int& type, string& ansTmp);

//＜item＞     ::= ＜factor＞{＜Multiplicative  expression＞＜factor＞}
bool item(int& type, string& ansTmp);

//＜factor＞    ::= ＜type＞｜＜type＞'['＜expression＞']'|'('＜expression＞')'｜＜integer＞|＜char＞｜＜function with return value＞
bool factor(int& type, string& ansTmp);
//＜statement＞    ::= ＜conditional statement＞｜＜loop statement＞| '{'＜statement list＞'}'| ＜function with return value＞;
//              |＜function without return value＞;｜＜assignment statement＞;｜＜write statement＞;｜＜null＞;|＜PUTARRAY＞;
bool statement();

//＜assignment statement＞   ::=  ＜identifier＞＝＜expression＞|＜identifier＞'['＜expression＞']'=＜expression＞
bool assignmentSataement();

//＜conditional statement＞  ::= if '('＜condition＞')'＜statement＞［else＜statement＞］
bool conditionalStatement();

//＜condition＞  ::=  ＜expression＞＜relational operator＞＜expression＞｜＜expression＞
bool condition(string& result);

//＜loop statement＞   ::=  while '('＜condition＞')'＜statement＞| do＜statement＞while '('＜condition＞')'
//              |for'('＜identifier＞＝＜expression＞;＜condition＞;＜identifier＞＝＜identifier＞(+|-)＜step＞')'＜state＞
bool loopStatement();

//＜step＞::= ＜unsigned integer＞
bool step(int& value);

//＜call function with return value＞ ::= ＜identifier＞'('＜value parameter list＞')’
bool callFunctionWithReturnValue();

//＜call function without return value＞ ::= ＜identifier＞'('＜value parameter table＞')’
bool callFunctinWithouReturnValue();

//＜value parameter table＞  ::= ＜expression＞{,＜expression＞}｜＜null＞
bool valueParameterTable(string funcName);

//＜statement list＞   ::= ｛＜statement＞｝
bool statementList();

//＜write statement＞    ::= printf '(' ＜string＞,＜expression＞ ')'| printf '('＜string＞ ')'| printf '('＜expression＞')’
bool writeStatement();

//＜return statement＞   ::=  return['('＜expression＞')']
bool returnStatement();


#endif // !GRAMMER_H
