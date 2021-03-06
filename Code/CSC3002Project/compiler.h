#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <fstream>
#include "lexical.h"
#include "grammer.h"
#include "midCode.h"
#include "mipsCode.h"
#include "function.h"
#include "simulator.h"

using namespace std;

void copyFile(string path1, string path2);

void clearAll();

void compiler();


#endif // COMPILER_H
