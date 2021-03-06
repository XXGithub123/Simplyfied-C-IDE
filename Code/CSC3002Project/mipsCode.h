#pragma once

#include <string>
using namespace std;

enum mipsOperation {
    add,
    addi,
    sub,
    mult,
    divop,
    mflo,
    mfhi,
    sll,
    beq,
    bne,
    bgtz,
    bgez,
    bltz,
    blez,
    j,
    jal,
    jr,
    lw,
    sw,
    syscall,
    ori, // -> li
    lui, // -> la
    dataSeg,  //.data
    textSeg,  //.text
    asciizSeg,  //.asciiz
    globlSeg,  //.globl
    label,
};

class mipsCode {
public:
    mipsOperation op; //opration
    string z;     // result
    string x;     // left operand
    string y;     // right operand
    int imme;     // immediate value

    mipsCode(mipsOperation o, string zz, string xx, string yy,int i = 0) : op(o), z(zz), x(xx), y(yy), imme(i)  {}
};

void genMips();

void outputMipsCode();

void loadValue(string name, string regName, bool gene, int& va, bool& get);

void storeValue(string name, string regName);
