/*
 * File: assembler_util.cpp
 * ------------------------
 * This file implement some functions to assemble the MIPS instruction to machine code.
 */
#include <bitset>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "str.h"
#include "assembler.h"

using namespace std;

int find_register_number(string name){
    if (name == "$zero")    return 0;
    else if (name == "$at") return 1;
    else if (name == "$v0") return 2;
    else if (name == "$v1") return 3;
    else if (name == "$a0") return 4;
    else if (name == "$a1") return 5;
    else if (name == "$a2") return 6;
    else if (name == "$a3") return 7;
    else if (name == "$t0") return 8;
    else if (name == "$t1") return 9;
    else if (name == "$t2") return 10;
    else if (name == "$t3") return 11;
    else if (name == "$t4") return 12;
    else if (name == "$t5") return 13;
    else if (name == "$t6") return 14;
    else if (name == "$t7") return 15;
    else if (name == "$s0") return 16;
    else if (name == "$s1") return 17;
    else if (name == "$s2") return 18;
    else if (name == "$s3") return 19;
    else if (name == "$s4") return 20;
    else if (name == "$s5") return 21;
    else if (name == "$s6") return 22;
    else if (name == "$s7") return 23;
    else if (name == "$t8") return 24;
    else if (name == "$t9") return 25;
    else if (name == "$k0") return 26;
    else if (name == "$k1") return 27;
    else if (name == "$gp") return 28;
    else if (name == "$sp") return 29;
    else if (name == "$fp") return 30;
    else if (name == "$ra") return 31;
    else {
        //cerr << "Can not find the register " << name << endl;
        //return -1;
    }
}

/*
 * Find the offset between the current instruction and the label.
 */
int find_offset(vector<label> labels, string label, int current_pc){
    int x=0;
    for (int i =0; i < (int)labels.size();i++){
        if(labels[i].name == label){
            x = (labels[i].position-current_pc)/4 - 1;
            break;
        }
    }
    return x;
}

// Scan through the file and find the text segment. After this function the fstream
// pointer will point to the first instruction.
void find_text(ifstream & is){
    string str;
    while(getline(is,str)){
        if (!is_Empty(str) && trim(str) == ".text") break;
    }
    if(is.eof()){   // If the is reach the end of file, which means there is no ".text" label.
        is.clear();
        is.seekg(0,ios::beg);
    }
}

bool find_data(ifstream & is){
    string str;
    while(getline(is,str)){
        if (!is_Empty(str) && trim(str) == ".data") break;
    }
    if(is.eof()){   // If the is reach the end of file, which means there is no ".text" label.
        is.clear();
        is.seekg(0,ios::beg);
        return false;
    }
    else return true;
}
/*
 * Scan through the file, and find out the labels.
 * Store them in a vector of label.
 */
vector<label> find_label(ifstream & is){
    vector<label>labels;
    string str;
    int position = 0x00400000; // Start position.
    find_text(is);
    while(getline(is,str)){
        if (!is_Empty(str)){
            trim(str);
            int index = str.find(':');
            if (index != -1){
                label l;
                l.name = str.substr(0,index);
                l.position = position;
                labels.push_back(l);
                position -= 4;
            }
            position += 4;
        }
    }
    is.clear();
    is.seekg(0,ios::beg);
    return labels;
}

int * assemble(ifstream &is, int * p, vector<label> labels){
    find_text(is);
    string str;
    int current_pc = 0x00400000;
    while(getline(is,str)){
        if(!is_Empty(str)){
//             trim(str);
            vector<string> s = split(str);
            if (s[0] == "") continue;   // lable.
            if (s[0] == "add")        output_R_format(p,0,s[1],s[2],s[3],0,0x20);
            else if (s[0] == "addu")  output_R_format(p,0,s[1],s[2],s[3],0,0x21);
            else if (s[0] == "addi")
                output_I_format(p,8,s[1],s[2],string_to_integer(s[3]));
            else if (s[0] == "addiu") output_I_format(p,9,s[1],s[2],string_to_integer(s[3]));
            else if (s[0] == "and")   output_R_format(p,0,s[1],s[2],s[3],0,0x24);
            else if (s[0] == "andi")  output_I_format(p,0xc,s[1],s[2],string_to_integer(s[3]));
            else if (s[0] == "div")   output_R_format(p,0,"$zero",s[1],s[2],0,0x1a);
            else if (s[0] == "divu")  output_R_format(p,0,"$zero",s[1],s[2],0,0x1b);
            else if (s[0] == "mult")  output_R_format(p,0,"$zero",s[1],s[2],0,0x18);
            else if (s[0] == "multu") output_R_format(p,0,"$zero",s[1],s[2],0,0x19);
            else if (s[0] == "nor")   output_R_format(p,0,s[1],s[2],s[3],0,0x27);
            else if (s[0] == "or")    output_R_format(p,0,s[1],s[2],s[3],0,0x25);
            else if (s[0] == "ori")   output_I_format(p,0xd,s[1],s[2],string_to_integer(s[3]));
            else if (s[0] == "sll")   output_R_format(p,0,s[1],"$zero",s[2],string_to_integer(s[3]),0);
            else if (s[0] == "sllv")  output_R_format(p,0,s[1],s[3],s[2],0,4);
            else if (s[0] == "sra")   output_R_format(p,0,s[1],"$zero",s[2],string_to_integer(s[3]),3);
            else if (s[0] == "srav")  output_R_format(p,0,s[1],s[3],s[2],0,7);
            else if (s[0] == "srl")   output_R_format(p,0,s[1],"$zero",s[2],string_to_integer(s[3]),2);
            else if (s[0] == "srlv")  output_R_format(p,0,s[1],s[3],s[2],0,6);
            else if (s[0] == "sub")   output_R_format(p,0,s[1],s[2],s[3],0,0x22);
            else if (s[0] == "subu")  output_R_format(p,0,s[1],s[2],s[3],0,0x23);
            else if (s[0] == "xor")   output_R_format(p,0,s[1],s[2],s[3],0,0x26);
            else if (s[0] == "xori")  output_I_format(p,0xe,s[1],s[2],string_to_integer(s[3]));
            else if (s[0] == "lui")   output_I_format(p,0xf,s[1],"$zero",string_to_integer(s[2]));
            else if (s[0] == "slt")   output_R_format(p,0,s[1],s[2],s[3],0,0x2a);
            else if (s[0] == "sltu")  output_R_format(p,0,s[1],s[2],s[3],0,0x2b);
            else if (s[0] == "slti")  output_I_format(p,0xa,s[1],s[2],string_to_integer(s[3]));
            else if (s[0] == "sltiu") output_I_format(p,0xb,s[1],s[2],string_to_integer(s[3]));
            // else if (s[0] == "bclf");
            // else if (s[0] == "bclt");
            else if (s[0] == "beq")   output_I_format(p,4,s[2],s[1],find_offset(labels,s[3],current_pc));
            else if (s[0] == "bgez")  output_I_format(p,1,"$at",s[1],find_offset(labels,s[2],current_pc));
            else if (s[0] == "bgezal") output_I_format(p,1,"$s1",s[1],find_offset(labels,s[2],current_pc));  //??????????????
            else if (s[0] == "bgtz")  output_I_format(p,7,"$zero",s[1],find_offset(labels,s[2],current_pc));
            else if (s[0] == "blez")  output_I_format(p,6,"$zero",s[1],find_offset(labels,s[2],current_pc));
            else if (s[0] == "bltzal") output_I_format(p,1,"$s0",s[1],find_offset(labels,s[2],current_pc)); //?????
            else if (s[0] == "bltz")  output_I_format(p,1,"$zero",s[1],find_offset(labels,s[2],current_pc));
            else if (s[0] == "bne")   output_I_format(p,5,s[2],s[1],find_offset(labels,s[3],current_pc));
            else if (s[0] == "j")     output_J_format(p,labels,2,s[1]);
            else if (s[0] == "jal")   output_J_format(p,labels,3,s[1]);
            else if (s[0] == "jalr")  output_R_format(p,0,s[2],s[1],"$zero",0,9);
            else if (s[0] == "jr")    output_R_format(p,0,"$zero",s[1],"$zero",0,8);
            else if (s[0] == "teq")   output_R_format(p,0,"$zero",s[1],s[2],0,0x34);
            else if (s[0] == "teqi")  output_I_format(p,1,"$t4",s[1],string_to_integer(s[2]));
            else if (s[0] == "tne")   output_R_format(p,0,"$zero",s[1],s[2],0,0x36);
            else if (s[0] == "tnei")  output_I_format(p,1,"$t6",s[1],string_to_integer(s[2]));
            else if (s[0] == "tge")   output_R_format(p,0,"$zero",s[1],s[2],0,0x30);
            else if (s[0] == "tgeu")  output_R_format(p,0,"$zero",s[1],s[2],0,0x31);
            else if (s[0] == "tgei")  output_I_format(p,1,"$t0",s[1],string_to_integer(s[2]));
            else if (s[0] == "tgeiu") output_I_format(p,1,"$t1",s[1],string_to_integer(s[2]));
            else if (s[0] == "tlt")   output_R_format(p,0,"$zero",s[1],s[2],0,0x32);
            else if (s[0] == "tltu")  output_R_format(p,0,"$zero",s[1],s[2],0,0x33);
            else if (s[0] == "tlti")  output_I_format(p,1,"$t2",s[1],string_to_integer(s[2]));
            else if (s[0] == "tltiu") output_I_format(p,1,"$t3",s[1],string_to_integer(s[2]));
            else if (s[0] == "lb")    output_load_and_store(p,0x20,s[1],s[2]);
            else if (s[0] == "lbu")   output_load_and_store(p,0x24,s[1],s[2]);
            else if (s[0] == "lh")    output_load_and_store(p,0x21,s[1],s[2]);
            else if (s[0] == "lhu")   output_load_and_store(p,0x25,s[1],s[2]);
            else if (s[0] == "lw")    output_load_and_store(p,0x23,s[1],s[2]);
//            else if (s[0] == "lwcl")  output_load_and_store(os,0x31,s[1],s[2]);       //
            else if (s[0] == "lwl")   output_load_and_store(p,0x22,s[1],s[2]);
            else if (s[0] == "lwr")   output_load_and_store(p,0x26,s[1],s[2]);
            else if (s[0] == "ll")    output_load_and_store(p,0x30,s[1],s[2]);
            else if (s[0] == "sb")    output_load_and_store(p,0x28,s[1],s[2]);
            else if (s[0] == "sh")    output_load_and_store(p,0x29,s[1],s[2]);
            else if (s[0] == "sw")    output_load_and_store(p,0x2b,s[1],s[2]);
//            else if (s[0] == "swcl")  output_load_and_store(os,0x31,s[1],s[2]);       //
//            else if (s[0] == "sdcl")  output_load_and_store(os,0x3d,s[1],s[2]);       //
            else if (s[0] == "swl")   output_load_and_store(p,0x2a,s[1],s[2]);
            else if (s[0] == "swr")   output_load_and_store(p,0x2e,s[1],s[2]);
            else if (s[0] == "sc")    output_load_and_store(p,0x38,s[1],s[2]);
            else if (s[0] == "mfhi")  output_R_format(p,0,s[1],"$zero","$zero",0,0x10);
            else if (s[0] == "mflo")  output_R_format(p,0,s[1],"$zero","$zero",0,0x12);
            else if (s[0] == "mthi")  output_R_format(p,0,"$zero",s[1],"$zero",0,0x11);
            else if (s[0] == "mtlo")  output_R_format(p,0,"$zero",s[1],"$zero",0,0x13);
//            else if (s[0] == "mfc0")  output_R_format(os,0x10,s[2],"$zero",s[1],0,0);  //
//            else if (s[0] == "mfcl")  output_R_format(os,0x11,"$zero",s[2],s[1],0,0);  //
            else if (s[0] == "syscall")output_I_format(p,0,"$zero","$zero",0);
            else cerr << "Assemble Error!!! " << "The current instruction is " << str <<endl;
            current_pc += 4;
            p++;
        }
    }
    is.clear();
    is.seekg(0,ios::beg);
    return p;
}

/*
 * Ouput the binary code of R instruction to the ostream.
 */
void output_R_format(ofstream &os, int op, string rd, string rs, string rt, int shamt, int func){
    bitset<6> op_b(op); // the binary number of the operation code.
    bitset<5> rs_b(find_register_number(rs)); // the binary number of the rs.
    bitset<5> rt_b(find_register_number(rt)); // the binary number of the rt.
    bitset<5> rd_b(find_register_number(rd)); // the binary number of the rd.
    bitset<5> shamt_b(shamt);
    bitset<6> func_b(func);
    os << op_b << rs_b << rt_b << rd_b << shamt_b << func_b << endl; // out put the binary code.
}

/*
 * Assemble the MIPS code and store the binary code into the memory.
 */
void output_R_format(int * p, int op, string rd, string rs, string rt, int shamt, int func){
    int rd_i = find_register_number(rd);
    int rs_i = find_register_number(rs);
    int rt_i = find_register_number(rt);
    *p = (op << 26) | (rs_i << 21) | (rt_i << 16) | (rd_i << 11) | ((shamt & 0x1f) << 6) | func;
    // The reason for "shamt & 0x4f" is that shamt amount may be negetive. In that case it will begin with
    // all ones in binary form. Thus we need to take an "and" operation with 0x4f(111111 in binary)
    // to get the last four bits.
//    bitset<32> i(*p);
//    cout << i << endl;

}

/*
 * Ouput the binary code of I instruction.
 */
void output_I_format(ofstream &os, int op, string rt, string rs, int imm){
    bitset<6> op_b(op);
    bitset<5> rs_b(find_register_number(rs));
    bitset<5> rt_b(find_register_number(rt));
    bitset<16> imm_b(((imm>0)?imm:0x10000+imm)); // if imm is negetive, then store the 2's complement of imm.
    os << op_b << rs_b << rt_b << imm_b << endl;
}

/*
 * Assemble the MIPS code and store the binary code into the memory.
 */
void output_I_format(int * p, int op, string rt, string rs, int imm){
    int rt_i = find_register_number(rt);
    int rs_i = find_register_number(rs);
    *p = (op << 26) | (rs_i << 21) | (rt_i << 16) | ( imm & 0xffff);
    // (Please refer to the comment in output_R_format() to understand "imm & 0xffff".
//    bitset<32> i(*p);
//    cout << i << endl;
}

/*
 * Ouput the binary code of J instruction.
 */
void output_J_format(ofstream &os, vector<label> labels, int op, string label){
    bitset<6> op_b(op);
    int address;
    for (int i =0; i < (int)labels.size();i++){
        if(labels[i].name == label){
            address = labels[i].position;
            break;
        }
    }
    bitset<26> address_b(address);
    os << op_b << address_b << endl;
}

/*
 * Assemble the MIPS code and store the binary code into the memory.
 */
void output_J_format(int * p, vector<label> labels, int op, string label){
    int address;
    for (int i =0; i < (int)labels.size();i++){
        if(labels[i].name == label){
            address = labels[i].position;
            break;
        }
    }
    *p = (op << 26) | (address/4);
//    bitset<32> i(*p);
//    cout << i << endl;
}

void output_load_and_store(ofstream &os, int op, string rt, string rs_and_offset){
    int left = rs_and_offset.find('(');
    int right = rs_and_offset.find(')');
    string rs = rs_and_offset.substr(left+1,right-left-1);
    string offset = rs_and_offset.substr(0,left);
    output_I_format(os,op,rt,rs,string_to_integer(offset));
}

void output_load_and_store(int * p, int op, string rt, string rs_and_offset){
    int left = rs_and_offset.find('(');
    int right = rs_and_offset.find(')');
    string rs = rs_and_offset.substr(left+1,right-left-1);
    string offset = rs_and_offset.substr(0,left);
    output_I_format(p,op,rt,rs,string_to_integer(offset));
}


char * load_ascii(char * gp, string str){
    str.erase(0,str.find_first_of("\"")+1);
    str.erase(str.find_last_of("\""));
//    cout << str << endl << endl;
    for (int i=0; i < (int) str.length();i++){
        gp[i] = str[i];
    }
    int space;
    if ((int)str.length()/4*4 == (int)str.length()) space = str.length();   // if space can be divided by 4.
    else space = ((int)str.length()/4+1)*4;
    gp = gp +space;
    return gp;
}

char * load_asciiz(char * gp, string str){
    str.erase(0,str.find_first_of("\"")+1);
    str.erase(str.find_last_of("\""));
//    cout << str << endl << endl;

    int i = 0;
    for (i=0; i < (int) str.length();i++){
        gp[i] = str[i];
    }
    gp[i] = '\0';
    int space;
    int length = (int)str.length()+1;
    if (length/4*4 == length) space = length;   // if space can be divided by 4.
    else space = (length/4+1)*4;
    gp = gp+space;
    return gp;
}

char * load_word(char * gp,vector<string> s){
    int * ip = (int*) gp;
    for (int i = 1; i < (int)s.size();i++){
        ip[i-1] = string_to_integer(s[i]);
    }
    ip = ip + (s.size()-1);
    return (char*) ip;
}

char * load_half(char * gp,vector<string> s){
    int16_t * hp = (int16_t*) gp;
    for (int i = 1; i < (int)s.size();i++){
        hp[i-1] = (int16_t)string_to_integer(s[i]);
    }

    hp = hp + ((s.size()-2)/2+1)*2; // Find the small integer that can be divided by 2 can is larger than the amount.
    return (char*) hp;
}

char * load_byte(char * gp,vector<string> s){
    for (int i=1;i<(int)s.size();i++){
        gp[i-1] = (char) string_to_integer(s[i]);
    }
    gp = gp + ((s.size()-2)/4+1)*4;
    return gp;
}
