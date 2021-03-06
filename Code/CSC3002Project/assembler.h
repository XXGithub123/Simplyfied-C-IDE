/*
 * File: assembler_utils.h
 * ----------------------
 * This file implement some functions to assemble the MIPS instruction to machine code.
 */
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <bitset>
#include <fstream>
#include <vector>
#include "str.h"

using namespace std;

struct label{
    string name;
    int position;
};

/*
 * Input the name of register and output its number.
 */
int find_register_number(string name);

// Scan through the file and find the text segment. After this function the fstream
// pointer will point to the first instruction.
void find_text(ifstream & is);
bool find_data(ifstream & is);
/*
 * Find the offset between the current instruction and the label.
 */
int find_offset(vector<label> labels, string label, int current_pc);

/*
 * Scan through the file, and find out the labels.
 * Store them in a vector of label.
 */
vector<label> find_label(ifstream & is);

/*
 * The main program to assemble the input file.
 */
int * assemble(ifstream &is, int * p, vector<label> labels);

/*
 * Ouput the binary code of R instruction.
 */
void output_R_format(ofstream &os, int op, string rd, string rs, string rt, int shamt, int func);

/*
 * Assemble the MIPS code and store the binary code into the memory.
 */
void output_R_format(int * p, int op, string rd, string rs, string rt, int shamt, int func);

/*
 * Ouput the binary code of I instruction.
 */
void output_I_format(ofstream &os, int op, string rt, string rs, int imm);

/*
 * Assemble the MIPS code and store the binary code into the memory.
 */
void output_I_format(int * p, int op, string rt, string rs, int imm);

/*
 * Ouput the binary code of J instruction.
 */
void output_J_format(ofstream &os, vector<label> labels, int op, string label);

/*
 * Assemble the MIPS code and store the binary code into the memory.
 */
void output_J_format(int * p, vector<label> labels, int op, string label);

void output_load_and_store(ofstream &os, int op, string rt, string rs_and_offset);

void output_load_and_store(int * p, int op, string rt, string rs_and_offset);

char * load_ascii(char * gp, string str);
char * load_asciiz(char * gp, string str);
char * load_word(char * gp, vector<string> s);
char * load_half(char * gp, vector<string> s);
char * load_byte(char * gp, vector<string> s);
#endif
