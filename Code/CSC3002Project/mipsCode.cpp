#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include "mipsCode.h"
#include "midCode.h"
#include "symbolItem.h"
#include "function.h"
using namespace std;

vector<mipsCode> mipsCodeTable;
string curFuncName = "";

extern ofstream mipsCodefile;
extern vector<string> stringList;  //store all the strings
extern vector<midCode> midCodeTable;
extern map<string, symbolItem> globalSymbolTable;
extern map<string, map<string, symbolItem>> allLocalSymbolTable;  //store all the local symbol tables

int asciizAddr = 0;

void loadValue(string name, string regName, bool gene, int& va, bool& get) {
    int addr;
    if (allLocalSymbolTable[curFuncName].find(name) != allLocalSymbolTable[curFuncName].end()) {
        if (allLocalSymbolTable[curFuncName][name].kind == 2) {  //const
            va = allLocalSymbolTable[curFuncName][name].type == 1 ?
                allLocalSymbolTable[curFuncName][name].constInt : allLocalSymbolTable[curFuncName][name].constChar;
            if (gene) {
                mipsCodeTable.push_back(mipsCode(ori, regName, "$zero", "", va));
            }
            get = true;
        }
        else {  //var
            addr = allLocalSymbolTable[curFuncName][name].addr;
            mipsCodeTable.push_back(mipsCode(lw, regName, "$fp", "", -4 * addr));
        }
    }
    else if (globalSymbolTable.find(name) != globalSymbolTable.end()) {
        if (globalSymbolTable[name].kind == 2) {  //const
            va = globalSymbolTable[name].type == 1 ? globalSymbolTable[name].constInt : globalSymbolTable[name].constChar;
            if (gene) {
                mipsCodeTable.push_back(mipsCode(ori, regName, "$zero", "", va));
            }
            get = true;
        }
        else {  //var
            addr = globalSymbolTable[name].addr;
            mipsCodeTable.push_back(mipsCode(lw, regName, "$gp", "", addr * 4));
        }
    }
    else {
        if (name.size() > 0) {
            if (gene) {
                mipsCodeTable.push_back(mipsCode(ori, regName, "$zero", "", string2int(name)));
            }
            va = string2int(name);
            get = true;
        }
    }
}

void storeValue(string name, string regName) {
    int addr;
    if (allLocalSymbolTable[curFuncName].find(name) != allLocalSymbolTable[curFuncName].end()
        && allLocalSymbolTable[curFuncName][name].kind == 1) {
        addr = allLocalSymbolTable[curFuncName][name].addr;
        mipsCodeTable.push_back(mipsCode(sw, regName, "$fp", "", -4 * addr));
    }
    else if (globalSymbolTable.find(name) != globalSymbolTable.end()
        && globalSymbolTable[name].kind == 1) {
        addr = globalSymbolTable[name].addr;
        mipsCodeTable.push_back(mipsCode(sw, regName, "$gp", "", addr * 4));
    }
}

void genMips() {
    mipsCodeTable.push_back(mipsCode(dataSeg, "", "", ""));  //.data
    //.asciiz
    for (int i = 0; i < stringList.size(); i++) {
        mipsCodeTable.push_back(mipsCode(asciizSeg, "s_" + int2string(i), stringList[i], ""));
    }
    //mipsCodeTable.push_back(mipsCode(asciizSeg, "nextLine", "\\n", ""));
    mipsCodeTable.push_back(mipsCode(textSeg, "", "", ""));  //.text
    mipsCodeTable.push_back(mipsCode(j, "main", "", ""));
    bool flag = false;
    int len = 0, addr = 0, va = 0, va2 = 0;
    bool get1 = false, get2 = false;
    int pushCnt = 0;
    int paramSize = 0;
    stack<midCode> pushOpStack;
    for (int i = 0; i < midCodeTable.size(); i++) {
        midCode mc = midCodeTable[i];
        midCode mcNext = mc;
        switch (mc.op) {
        case PLUSOP: {
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            if (get1 && get2) {
                mipsCodeTable.push_back(mipsCode(ori, "$t2", "$zero", "", va + va2));
            }
            else if (get1 && !get2) {
                mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t1", "", va));
            }
            else if (!get1 && get2) {
                mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t0", "", va2));
            }
            else {
                mipsCodeTable.push_back(mipsCode(add, "$t2", "$t0", "$t1"));
            }
            storeValue(mc.z, "$t2");
            break;
        }
        case MINUOP: {
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            if (get1 && get2) {
                mipsCodeTable.push_back(mipsCode(ori, "$t2", "$zero", "", va - va2));
            }
            else if (get1 && !get2) {  //va - $t1
                mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t1", "", -va));  //$t1-va
                mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0", "$t2"));      //0-$t2
            }
            else if (!get1 && get2) {  //$t0 - va2
                mipsCodeTable.push_back(mipsCode(addi, "$t2", "$t0", "", -va2));
            }
            else {
                mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0", "$t1"));
            }
            storeValue(mc.z, "$t2");
            break;
        }
        case MULTOP: {
            loadValue(mc.x, "$t0", true, va, get1);
            loadValue(mc.y, "$t1", true, va2, get2);
            mipsCodeTable.push_back(mipsCode(mult, "$t0", "$t1", ""));
            mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
            storeValue(mc.z, "$t2");
            break;
        }
        case DIVOP: {
            loadValue(mc.x, "$t0", true, va, get1);
            loadValue(mc.y, "$t1", true, va2, get2);
            mipsCodeTable.push_back(mipsCode(divop, "$t0", "$t1", ""));
            mipsCodeTable.push_back(mipsCode(mflo, "$t2", "", ""));
            storeValue(mc.z, "$t2");
            break;
        }
        case LSSOP: {  //<
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            mcNext = midCodeTable[i + 1];
            if (mcNext.op == BZ) {
                if (get1 && get2) {
                    if (va >= va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(blez, "$t1", "", mcNext.z));  //blez
                }
                else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(bgez, "$t0", "", mcNext.z));  //bgez
                }
                else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(bgez, "$t2", "", mcNext.z));  //bgez
                }
            }
            else if (mcNext.op == BNZ) {
                if (get1 && get2) {
                    if (va < va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va < $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(bgtz, "$t1", "", mcNext.z));  //bgtz
                }
                else if (!get1 && get2) {  //$t0 < va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(bltz, "$t0", "", mcNext.z));  //bltz
                }
                else { //$t0 < $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(bltz, "$t2", "", mcNext.z));  //bltz
                }
            }
            i++;
            break;
        }
        case LEQOP: {  //<=
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            mcNext = midCodeTable[i + 1];
            if (mcNext.op == BZ) {
                if (get1 && get2) {
                    if (va > va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va > $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(bltz, "$t1", "", mcNext.z));  //bltz
                }
                else if (!get1 && get2) {  //$t0 > va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(bgtz, "$t0", "", mcNext.z));  //bgtz
                }
                else {  //$t0 > $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(bgtz, "$t2", "", mcNext.z));  //bgtz
                }
            }
            else if (mcNext.op == BNZ) {
                if (get1 && get2) {
                    if (va <= va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va <= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(bgez, "$t1", "", mcNext.z));  //bgez
                }
                else if (!get1 && get2) {  //$t0 <= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(blez, "$t0", "", mcNext.z));  //blez
                }
                else { //$t0 <= $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1",-va));
                    mipsCodeTable.push_back(mipsCode(blez, "$t2", "", mcNext.z));  //blez
                }
            }
            i++;
            break;
        }
        case GREOP: {  //>
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            mcNext = midCodeTable[i + 1];
            if (mcNext.op == BZ) {
                if (get1 && get2) {
                    if (va <= va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va <= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(bgez, "$t1", "", mcNext.z));  //bgez
                }
                else if (!get1 && get2) {  //$t0 <= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(blez, "$t0", "", mcNext.z));  //blez
                }
                else { //$t0 <= $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(blez, "$t2", "", mcNext.z));  //blez
                }
            }
            else if (mcNext.op == BNZ) {
                if (get1 && get2) {
                    if (va > va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va > $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(bltz, "$t1", "", mcNext.z));  //bltz
                }
                else if (!get1 && get2) {  //$t0 > va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(bgtz, "$t0", int2string(va2), mcNext.z));  //bgtz
                }
                else {  //$t0 > $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(bgtz, "$t2", "", mcNext.z));  //bgtz
                }
            }
            i++;
            break;
        }
        case GEQOP: {  //>=
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            mcNext = midCodeTable[i + 1];
            if (mcNext.op == BZ) {
                if (get1 && get2) {
                    if (va < va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va < $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(bgtz, "$t1", int2string(va), mcNext.z));  //bgtz
                }
                else if (!get1 && get2) {  //$t0 < va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(bltz, "$t0", "", mcNext.z));  //bltz
                }
                else { //$t0 < $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(bltz, "$t2", "", mcNext.z));  //bltz
                }
            }
            else if (mcNext.op == BNZ) {
                if (get1 && get2) {
                    if (va >= va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {  //va >= $t1
                    mipsCodeTable.push_back(mipsCode(addi, "$t1", "$t1","",-va));
                    mipsCodeTable.push_back(mipsCode(blez, "$t1", "", mcNext.z));  //blez
                }
                else if (!get1 && get2) {  //$t0 >= va2
                    mipsCodeTable.push_back(mipsCode(addi, "$t0", "$t0","",-va2));
                    mipsCodeTable.push_back(mipsCode(bgez, "$t0", int2string(va2), mcNext.z));  //bgez
                }
                else {  //$t0 >= $t1
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t0","$t1"));
                    mipsCodeTable.push_back(mipsCode(bgez, "$t2", "", mcNext.z));  //bgez
                }
            }
            i++;
            break;
        }
        case EQLOP: {
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            mcNext = midCodeTable[i + 1];
            if (mcNext.op == BZ) {
                if (get1 && get2) {
                    if (va != va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t0", "$zero", "", va));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mcNext.z));
                }
                else if (!get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t1", "$zero", "", va2));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mcNext.z));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mcNext.z));
                }
            }
            else if (mcNext.op == BNZ) {  //1
                if (get1 && get2) {
                    if (va == va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t0", "$zero", "", va));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mcNext.z));
                }
                else if (!get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t1", "$zero", "", va2));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mcNext.z));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mcNext.z));
                }
            }
            i++;
            break;
        }
        case NEQOP: {
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            get2 = false;
            loadValue(mc.y, "$t1", false, va2, get2);
            mcNext = midCodeTable[i + 1];
            if (mcNext.op == BZ) {
                if (get1 && get2) {
                    if (va == va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t0", "$zero", "", va));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mcNext.z));
                }
                else if (!get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t1", "$zero", "", va2));
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mcNext.z));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(beq, "$t0", "$t1", mcNext.z));
                }
            }
            else if (mcNext.op == BNZ) {
                if (get1 && get2) {
                    if (va != va2) {
                        mipsCodeTable.push_back(mipsCode(j, mcNext.z, "", ""));
                    }
                }
                else if (get1 && !get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t0", "$zero", "", va));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mcNext.z));
                }
                else if (!get1 && get2) {
                    mipsCodeTable.push_back(mipsCode(ori, "$t1", "$zero", "", va2));
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mcNext.z));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(bne, "$t0", "$t1", mcNext.z));
                }
            }
            i++;
            break;
        }
        case ASSIGNOP: {

            loadValue(mc.x, "$t0", true, va, get1);
            storeValue(mc.z, "$t0");
            break;
        }
        case GOTO: {
            mipsCodeTable.push_back(mipsCode(j, mc.z, "", ""));
            break;
        }
        case PUSH: {
            pushOpStack.push(mc);
            break;
        }
        case CALL: {
            paramSize = globalSymbolTable[mc.z].parameterTable.size();
            while (paramSize) {
                paramSize--;
                if (pushOpStack.empty()) {
                    cout << "ERROR!!!!!!!!\n";
                }
                midCode tmpMc = pushOpStack.top();
                pushOpStack.pop();
                loadValue(tmpMc.z, "$t0", true, va, get1);
                mipsCodeTable.push_back(mipsCode(sw, "$t0", "$sp", "", -4 * paramSize));
            }
            mipsCodeTable.push_back(mipsCode(addi, "$sp", "$sp", "", -4 * globalSymbolTable[mc.z].length - 8));
            mipsCodeTable.push_back(mipsCode(sw, "$ra", "$sp", "", 4));
            mipsCodeTable.push_back(mipsCode(sw, "$fp", "$sp", "", 8));
            mipsCodeTable.push_back(mipsCode(addi, "$fp", "$sp", "", 4 * globalSymbolTable[mc.z].length + 8));
            mipsCodeTable.push_back(mipsCode(jal, mc.z, "", ""));
            mipsCodeTable.push_back(mipsCode(lw, "$fp", "$sp", "", 8));
            mipsCodeTable.push_back(mipsCode(lw, "$ra", "$sp", "", 4));
            mipsCodeTable.push_back(mipsCode(addi, "$sp", "$sp", "", 4 * globalSymbolTable[mc.z].length + 8));
            break;
        }
        case RET: {
            loadValue(mc.z, "$v0", true, va, get1);
            mipsCodeTable.push_back(mipsCode(jr, "$ra", "", ""));
            break;
        }
        case RETVALUE: {

            if (allLocalSymbolTable[curFuncName].find(mc.z) != allLocalSymbolTable[curFuncName].end()
                && allLocalSymbolTable[curFuncName][mc.z].kind == 1) {
                addr = allLocalSymbolTable[curFuncName][mc.z].addr;
                mipsCodeTable.push_back(mipsCode(sw, "$v0", "$fp", "", -4 * addr));
            }
            break;
        }
        case SCAN: {

            if (allLocalSymbolTable[curFuncName].find(mc.z) != allLocalSymbolTable[curFuncName].end()
                && allLocalSymbolTable[curFuncName][mc.z].kind == 1) {
                if (allLocalSymbolTable[curFuncName][mc.z].type == 1) {  //int
                    mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 5));
                }
                else {  //char
                    mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 12));
                }
                mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                addr = allLocalSymbolTable[curFuncName][mc.z].addr;
                mipsCodeTable.push_back(mipsCode(sw, "$v0", "$fp", "", -4 * addr));
            }
            else if (globalSymbolTable.find(mc.z) != globalSymbolTable.end()
                && globalSymbolTable[mc.z].kind == 1) {
                if (globalSymbolTable[mc.z].type == 1) {  //int
                    mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 5));
                }
                else {  //char
                    mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 12));
                }
                mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
                addr = globalSymbolTable[mc.z].addr;
                mipsCodeTable.push_back(mipsCode(sw, "$v0", "$gp", "", addr * 4));
            }
            break;
        }
        case PRINT: {
            if (mc.x[0] == '3') {  //string
                for (int i = 0; i < stringList.size(); i++) {
                    if (stringList[i] == mc.z) {
                        mipsCodeTable.push_back(mipsCode(lui, "$a0", "80", ""));
                        mipsCodeTable.push_back(mipsCode(ori, "$a0", "$a0", "",asciizAddr));

                        int len = (stringList[i]).length();

                        if ( (len+1) % 4 != 0 ) {
                            int remainder = (len+1) % 4;
                            remainder = 4 - remainder;
                            len = len + remainder + 1;
                        }
                        else {
                            len = len + 1;
                        }
                        asciizAddr += len;

                        break;
                    }
                }
                mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 4));
                mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
            }
            else if (mc.x[0] == '4') {
                // empty
            }
            else { //int char
                loadValue(mc.z, "$a0", true, va, get1);
                mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", mc.x[0] == '1' ? 1 : 11));
                mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
            }
            break;
        }
        case LABEL: {
            mipsCodeTable.push_back(mipsCode(label, mc.z, "", ""));
            break;
        }
        case FUNC: {

            if (flag) {
                mipsCodeTable.push_back(mipsCode(jr, "$ra", "", ""));
            }
            flag = true;
            mipsCodeTable.push_back(mipsCode(label, mc.x, "", ""));
            if (mc.x == "main") {
                len = globalSymbolTable[mc.x].length;
                mipsCodeTable.push_back(mipsCode(add, "$fp", "$sp", "$zero"));
                mipsCodeTable.push_back(mipsCode(addi, "$sp", "$sp", "", -4 * len - 8));
            }
            curFuncName = mc.x;
            break;
        }
        case GETARRAY: {

            get1 = false;
            loadValue(mc.y, "$t0", false, va, get1);
            if (allLocalSymbolTable[curFuncName].find(mc.x) != allLocalSymbolTable[curFuncName].end()
                && allLocalSymbolTable[curFuncName][mc.x].kind == 4) {  //array
                addr = allLocalSymbolTable[curFuncName][mc.x].addr;
                if (!get1) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$fp", "", -4 * addr));
                    mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t2", "$t0"));
                    mipsCodeTable.push_back(mipsCode(lw, "$t1", "$t2", "", 0));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(lw, "$t1", "$fp", "", -4 * (addr + va)));
                }
            }
            else if (globalSymbolTable.find(mc.x) != globalSymbolTable.end()
                && globalSymbolTable[mc.x].kind == 4) {  //array
                addr = globalSymbolTable[mc.x].addr;
                if (!get1) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$gp", "", addr * 4));
                    mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                    mipsCodeTable.push_back(mipsCode(add, "$t2", "$t2", "$t0"));
                    mipsCodeTable.push_back(mipsCode(lw, "$t1", "$t2", "", 0));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(lw, "$t1", "$gp", "", (addr + va) * 4));
                }
            }
            storeValue(mc.z, "$t1");
            break;
        }
        case PUTARRAY: {

            loadValue(mc.y, "$t1", true, va, get1);
            get1 = false;
            loadValue(mc.x, "$t0", false, va, get1);
            if (allLocalSymbolTable[curFuncName].find(mc.z) != allLocalSymbolTable[curFuncName].end()
                && allLocalSymbolTable[curFuncName][mc.z].kind == 4) {  //array
                addr = allLocalSymbolTable[curFuncName][mc.z].addr;
                if (!get1) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$fp", "", -4 * addr));
                    mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                    mipsCodeTable.push_back(mipsCode(sub, "$t2", "$t2", "$t0"));
                    mipsCodeTable.push_back(mipsCode(sw, "$t1", "$t2", "", 0));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(sw, "$t1", "$fp", "", -4 * (addr + va)));
                }
            }
            else if (globalSymbolTable.find(mc.z) != globalSymbolTable.end()
                && globalSymbolTable[mc.z].kind == 4) {  //array
                addr = globalSymbolTable[mc.z].addr;
                if (!get1) {
                    mipsCodeTable.push_back(mipsCode(addi, "$t2", "$gp", "", addr * 4));
                    mipsCodeTable.push_back(mipsCode(sll, "$t0", "$t0", "", 2));
                    mipsCodeTable.push_back(mipsCode(add, "$t2", "$t2", "$t0"));
                    mipsCodeTable.push_back(mipsCode(sw, "$t1", "$t2", "", 0));
                }
                else {
                    mipsCodeTable.push_back(mipsCode(sw, "$t1", "$gp", "", (addr + va) * 4));
                }
            }
            break;
        }
        case EXIT: {
            mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 10));
            mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
        }
        default: {
            break;
        }
        }
    }
    mipsCodeTable.push_back(mipsCode(ori, "$v0", "$zero", "", 10));
    mipsCodeTable.push_back(mipsCode(syscall, "", "", ""));
}

void outputMipsCode() {
    for (int i = 0; i < mipsCodeTable.size(); i++) {
        mipsCode mc = mipsCodeTable[i];
        switch (mc.op) {
        case add:
            mipsCodefile << "add " << mc.z << "," << mc.x << "," << mc.y << "\n";
            break;
        case sub:
            mipsCodefile << "sub " << mc.z << "," << mc.x << "," << mc.y << "\n";
            break;
        case mult:
            mipsCodefile << "mult " << mc.z << "," << mc.x << "\n";
            break;
        case divop:
            mipsCodefile << "div " << mc.z << "," << mc.x << "\n";
            break;
        case addi:
            mipsCodefile << "addi " << mc.z << "," << mc.x << "," << mc.imme << "\n";
            break;
        case sll:
            mipsCodefile << "sll " << mc.z << "," << mc.x << "," << mc.imme << "\n";
            break;
        case mflo:
            mipsCodefile << "mflo " << mc.z << "\n";
            break;
        case mfhi:
            mipsCodefile << "mfhi " << mc.z << "\n";
        case beq:
            mipsCodefile << "beq " << mc.z << "," << mc.x << "," << mc.y << "\n";
            break;
        case bne:
            mipsCodefile << "bne " << mc.z << "," << mc.x << "," << mc.y << "\n";
            break;
        case bgtz:
            mipsCodefile << "bgtz " << mc.z << ","  << mc.y << "\n";
            break;
        case bgez:
            mipsCodefile << "bgez " << mc.z << ","  << mc.y << "\n";
            break;
        case bltz:
            mipsCodefile << "bltz " << mc.z << "," << mc.y << "\n";
            break;
        case blez:
            mipsCodefile << "blez " << mc.z << "," << mc.y << "\n";
            break;
        case j:
            mipsCodefile << "j " << mc.z << "\n";
            break;
        case jal:
            mipsCodefile << "jal " << mc.z << "\n";
            break;
        case jr:
            mipsCodefile << "jr " << mc.z << "\n";
            break;
        case lw:
            mipsCodefile << "lw " << mc.z << "," << mc.imme << "(" << mc.x << ")\n";
            break;
        case sw:
            mipsCodefile << "sw " << mc.z << "," << mc.imme << "(" << mc.x << ")\n";
            break;
        case syscall:
            mipsCodefile << "syscall\n";
            break;
        case ori:
            mipsCodefile << "ori " << mc.z << "," << mc.x << "," << mc.imme << "\n";
            break;
        case lui:
            mipsCodefile << "lui " << mc.z << "," << "80" << "\n";
            break;
        case dataSeg:
            mipsCodefile << ".data\n";
            break;
        case textSeg:
            mipsCodefile << ".text\n";
            break;
        case asciizSeg:
            mipsCodefile << mc.z << ": .asciiz \"" << mc.x << "\"\n";
            break;
        case globlSeg:
            mipsCodefile << ".globl main\n";
            break;
        case label:
            mipsCodefile << mc.z << ": " << "\n";
            break;
        default:
            break;
        }
    }
}
