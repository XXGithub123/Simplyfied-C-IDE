#pragma once

#include <string>
using namespace std;

void showGlobal();

void showLocal();

void showAll();

void showString();

string int2string(int t);

int string2int(string s);

string genLabel();

string genTmp();

void raiseError(string s, int line);
