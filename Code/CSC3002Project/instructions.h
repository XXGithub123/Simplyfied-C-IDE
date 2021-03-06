#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <string>

extern void * p;                   // Memory Address.
extern int registers[34];          // 32 Registers with registers[32]=$hi and registers[33]=$lo
extern int * PC;                  // Program Counter.
extern int offset;                // Difference between the memory address and its "name"(0x00400000).
extern char* heap_pointer;
//  Initialize the Memory Address, Registers, PC

/*
 * This sturcture is used for debugging, when the MIPS code call syscall and is going to
 * output something, it will change "to_print" to true and assign the content. The user
 * interface will print the content and change "to_print" back to false.
 */
struct output2terminal{
    bool to_print = false;
    std::string content;
};
extern output2terminal output_content;
void memary_initialize();

void registers_initialize();

void run_instruction(int instruction);

void run_R_instruction(int instruction);

void add(int rs,int rt, int rd);
void addu(int rs, int rt, int rd);
void addi(int rs, int rt, int imm);
void addiu(int rs, int rt, int imm);
void and_(int rs, int rt, int rd);      // Add an underline because "and" is a keyword in c++ 11.
void andi(int rs, int rt, int imm);
void div_(int rs, int rt);              // Add an underline to avoid confilct.
void divu(int rs, int rt);
void mult(int rs, int rt);
void multu(int rs, int rt);
void nor(int rs, int rt, int rd);
void or_(int rs, int rt, int rd);        // Add an underline.
void ori(int rs, int rt, int imm);
void sll(int rs, int rt, int rd, int shamt);
void sllv(int rs, int rt, int rd);
void sra(int rs, int rt, int rd, int shamt);
void srav(int rs, int rt, int rd);
void srl(int rs, int rt, int rd, int shamt);
void srlv(int rs, int rt, int rd);
void sub(int rs, int rt, int rd);
void subu(int rs, int rt, int rd);
void xor_(int rs, int rt, int rd);      // Add an underline.
void xori(int rs, int rt, int imm);
void lui(int rt, int imm);
void slt(int rs, int rt, int rd);
void sltu(int rs, int rt, int rd);
void slti(int rs, int rt, int imm);
void sltiu(int rs, int rt, int imm);
void beq(int rs, int rt, int os);
void bgez(int rs, int os);
void bgezal(int rs, int os);
void bgtz(int rs, int os);
void blez(int rs, int os);
void bltzal(int rs, int os);
void bltz(int rs, int os);
void bne(int rs, int rt, int os);
void j(int target);
void jal(int target);
void jalr(int rs, int rd);
void jr(int rs);
void teq(int rs, int rt);
void teqi(int rs, int imm);
void tne(int rs, int rt);
void tnei(int rs, int imm);
void tge(int rs, int rt);
void tgeu(int rs, int rt);
void tgei(int rs, int imm);
void tgeiu(int rs, int imm);
void tlt(int rs, int rt);
void tltu(int rs, int rt);
void tlti(int rs, int imm);
void tltiu(int rs, int imm);
void lb(int rs, int rt, int os);
void lbu(int rs, int rt, int os);
void lh(int rs, int rt, int os);
void lhu(int rs, int rt, int os);
void lw(int rs, int rt, int os);
void lwl(int rs, int rt, int os);
void lwr(int rs, int rt, int os);
void ll(int rs, int rt, int os);
void sb(int rs, int rt, int os);
void sh(int rs, int rt, int os);
void sw(int rs, int rt, int os);
void swl(int rs, int rt, int os);
void swr(int rs, int rt, int os);
void sc(int rs, int rt, int os);
void mfhi(int rd);
void mflo(int rd);
void syscall();
void print_int();
void print_string();
void read_int();
void read_string();
void sbrk();
void exit();
void print_char();
void read_char();
void open();
void read();
void write();
void close();
void exit2();

#endif // INSTRUCTIONS_H
