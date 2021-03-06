#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "lexical.h"
#include "grammer.h"
#include "midCode.h"
#include "mipsCode.h"
#include "function.h"
#include "simulator.h"
#include "symbolItem.h"
using namespace std;

extern string filecontent;
extern vector<mipsCode> mipsCodeTable;
extern vector<string> stringList;  //store all the strings
extern vector<midCode> midCodeTable;
extern map<string, symbolItem> globalSymbolTable;
extern map<string, map<string, symbolItem>> allLocalSymbolTable;

extern int indexs;
extern int line;
extern int labelId;
extern int tmpVarId;
extern int curFuncReturnType;
extern int realReturnType;
extern int globalAddr;
extern int localAddr;
extern bool isMain;
extern int asciizAddr;

extern map<int, int> address2line;
extern map<int, string> line2instruction;
extern int * end_position;

ifstream inputfile;

ofstream midCodefile;
ofstream mipsCodefile;
ofstream terminalfile;

void copyFile(string path1, string path2) {
    char ch;
    ifstream file1(path1);
    ofstream file2(path2);
    while (file1.get(ch)) {
        file2 << ch;
    }
    file1.close();
    file2.close();
}

void clearAll(){
    vector<mipsCode>().swap(mipsCodeTable);
    vector<string>().swap(stringList);
    vector<midCode>().swap(midCodeTable);
    map<string, symbolItem>().swap(globalSymbolTable);
    map<string, map<string, symbolItem>>().swap(allLocalSymbolTable);
    string().swap(filecontent);
    indexs = 0;
    line = 1;
    labelId = 0;
    tmpVarId = 0;
    curFuncReturnType = -1;
    realReturnType = -1;
    globalAddr = 0;
    localAddr = 0;
    isMain = false;
    asciizAddr = 0;

    map<int, int>().swap(address2line) ;
    map<int, string>().swap(line2instruction);
}

void compiler() {
    inputfile.open("testfile.txt", ios::in);
    midCodefile.open("midCode.txt", ios::out);
    mipsCodefile.open("mips.asm", ios::out);
    terminalfile.open("output.txt", ios::out);

    string tmpIn;
    while (getline(inputfile, tmpIn)) {
        filecontent.append(tmpIn);
        filecontent.append("\n");
    }
//    cout << filecontent.size() << endl;
    int re = getsym();
    if (re < 0) {
        //error()
    }
    else {
        if (procedure()) {
            //success
            //cout << "True!" << endl;
        }
        else {
            //error()
        }
    }
/*    for (auto iter = stringList.cbegin(); iter != stringList.cend(); iter++)
    {
        cout << (*iter) << endl;
    }*/
    outputMidCode();
    genMips();
    outputMipsCode();


    inputfile.close();
    midCodefile.close();
    mipsCodefile.close();

    //showGlobal();
    //showAll();
    //showString();

    simulator();

    terminalfile.close();
    clearAll();
}

/*void simulate(){
        terminalfile.open("output.txt", ios::out);
        simulator();
        terminalfile.close();
}*/
