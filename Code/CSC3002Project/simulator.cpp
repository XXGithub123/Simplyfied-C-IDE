#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "str.h"
#include "assembler.h"
#include "instructions.h"

using namespace std;

//The following variables are defined in instruction.h
//extern void * p;                   // Memory Address.
//extern int registers[34];          // 32 Registers with registers[32]=$hi and registers[33]=$lo
//extern int * PC;                  // Program Counter.
//extern long offset;                // Difference between the memory address and its "name"(0x00400000).
ifstream is;                // Input file(.asm).


// Open the file.
void open_file(){
    string file_name;
    //cout << "Please input the name of file you want to simulate: " << endl;
    file_name = "mips.asm";
//    getline(cin,file_name);
//    cout << file_name;

    is.open(file_name.c_str());
    if (!is) cout << "Open failed" << endl;
    //else cout << "Open successfully, start to simulate the MIPS code." << endl;
}


int* assemble_machine_code(){
    vector<label> labels = find_label(is);
    int * end_position = assemble(is,PC,labels);
    return end_position;
}

void load_data(){
    char * gp = (char*) p +(0x00500000-0x00400000); //store the current gp;
//    cout << "We are loading data." << endl;
    string str;
    if(find_data(is)){
        while(getline(is,str) && trim(str) != ".text"){
            if(!is_Empty(str)){
                vector<string> s = split(str);
                if (s[0] == ".ascii")       gp = load_ascii(gp,str);
               else if (s[0] == ".asciiz") gp = load_asciiz(gp,str);
                else if (s[0] == ".word")   gp = load_word(gp,s);
                else if (s[0] == ".half")   gp = load_half(gp,s);
                else if (s[0] == ".byte")   gp = load_byte(gp,s);
                else cerr << "ERROR in loading data." << endl;
            }
        }
        heap_pointer = gp;  //heap pointer points to the top of static data.
        is.clear();
        is.seekg(0,ios::beg);
    }
}



void simulator(){
    memary_initialize();
    registers_initialize();
    open_file();
    int * end_position = assemble_machine_code();
    load_data();

    while (PC < end_position){
        run_instruction(*PC);
        PC++;
    }
    free(p);
    p = NULL;
    is.close();
}

