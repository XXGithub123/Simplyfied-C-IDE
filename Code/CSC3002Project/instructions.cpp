#include "instructions.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdint.h>


using namespace std;

void * p;                   // Memory Address.
int registers[34];          // 32 Registers with registers[32]=$hi and registers[33]=$lo
int * PC;                  // Program Counter.
int offset;                // Difference between the memory address and its "name"(0x00400000).
char * heap_pointer;
fstream file;
output2terminal output_content;

extern ofstream terminalfile;


//  Initialize the Memory Address, Registers, PC
void memary_initialize(){
    p =  malloc(6291456);
//    cout << "The base pointer is : ";
//    cout << p << endl;

    offset = (int &)p - 0x00400000;
//    cout << "The offset is : ";
//    cout << offset << endl;

    PC = (int*)p;
//    cout << "Current PC: ";
//    cout << (void*) PC << endl;
}

void registers_initialize(){
    registers[0] = 0;               //$zero
    registers[28] = 0x00500000;     //gp
    registers[29] = 0x00A00000;     //$sp
    registers[30] = 0x00A00000;     //$fp

}

void run_instruction(int instruction){
    int op = (instruction >> 26) & 0x3f;
    int rs = (instruction >> 21) & 0x1f;
    int rt = (instruction >> 16) & 0x1f;
    int imm = (int)((int16_t)instruction);  //cut the left 16bits and do signal extending.
    switch (op) {
    case 0:     run_R_instruction(instruction); break;
    case 8:     addi(rs,rt,imm);    break;
    case 9:     addiu(rs,rt,imm);   break;
    case 0xc:   andi(rs,rt,imm);    break;
    case 0xd:   ori(rs,rt,imm);     break;
    case 0xe:   xori(rs,rt,imm);    break;
    case 0xf:   lui(rt,imm);        break;
    case 0xa:   slti(rs,rt,imm);    break;
    case 0xb:   sltiu(rs,rt,imm);   break;
    case 4:     beq(rs,rt,imm);     break;
    case 1:
        if (rt == 1)            bgez(rs,imm);
        else if (rt == 0x11)    bgezal(rs,imm);
        else if (rt == 0x10)    bltzal(rs,imm);
        else if (rt == 0)       bltz(rs,imm);
        else if (rt == 0xc)     teqi(rs,imm);
        else if (rt == 0xe)     tnei(rs,imm);
        else if (rt == 8)       tgei(rs,imm);
        else if (rt == 9)       tgeiu(rs,imm);
        else if (rt == 0xa)     tlti(rs,imm);
        else if (rt == 0xb)     tltiu(rs,imm);
        break;
    case 7:     bgtz(rs,imm);       break;
    case 6:     blez(rs,imm);       break;
    case 5:     bne(rs,rt,imm);     break;
    case 2:     j((instruction & 0x3ffffff));  break;
    case 3:     jal((instruction & 0x3ffffff)); break;
    case 0x20:  lb(rs,rt,imm);      break;
    case 0x24:  lbu(rs,rt,imm);     break;
    case 0x21:  lh(rs,rt,imm);      break;
    case 0x25:  lhu(rs,rt,imm);     break;
    case 0x23:  lw(rs,rt,imm);      break;
    case 0x22:  lwl(rs,rt,imm);     break;
    case 0x26:  lwr(rs,rt,imm);     break;
    case 0x30:  ll(rs,rt,imm);      break;
    case 0x28:  sb(rs,rt,imm);      break;
    case 0x29:  sh(rs,rt,imm);      break;
    case 0x2b:  sw(rs,rt,imm);      break;
    case 0x2a:  swl(rs,rt,imm);     break;
    case 0x2e:  swr(rs,rt,imm);     break;
    case 0x38:  sc(rs,rt,imm);      break;
    default: cerr << "ERROR: Can not find the instruction." << endl;
    }
}


void run_R_instruction(int instruction){
    int op = (instruction >> 26) & 0x3f;
    int rs = (instruction >> 21) & 0x1f;
    int rt = (instruction >> 16) & 0x1f;
    int rd = (instruction >> 11) & 0x1f;
    int shamt = (instruction >> 6) & 0x1f;
    int func = instruction & 0x3f;
    switch (func) {
    case 0:
        if(instruction == 0) syscall();
        else sll(rs,rt,rd,shamt);
        break;
    case 0x20:  add(rs,rt,rd);  break;
    case 0x21:  addu(rs,rt,rd); break;
    case 0x24:  and_(rs,rt,rd); break;
    case 0x1a:  div_(rs,rt);    break;
    case 0x1b:  divu(rs,rt);    break;
    case 0x18:  mult(rs,rt);    break;
    case 0x19:  multu(rs,rt);   break;
    case 0x27:  nor(rs,rt,rd);  break;
    case 0x25:  or_(rs,rt,rd);  break;
    case 4:     sllv(rs,rt,rd); break;
    case 3:     sra(rs,rt,rd,shamt);    break;
    case 7:     srav(rs,rt,rd); break;
    case 2:     srl(rs,rt,rd,shamt);    break;
    case 6:     srlv(rs,rt,rd); break;
    case 0x22:  sub(rs,rt,rd);  break;
    case 0x23:  subu(rs,rt,rd); break;
    case 0x26:  xor_(rs,rt,rd); break;
    case 0x2a:  slt(rs,rt,rd);  break;
    case 0x2b:  sltu(rs,rt,rd); break;
    case 9:     jalr(rs,rd);    break;
    case 8:     jr(rs);         break;
    case 0x34:  teq(rs,rt);     break;
    case 0x36:  tne(rs,rt);     break;
    case 0x30:  tge(rs,rt);     break;
    case 0x31:  tgeu(rs,rt);    break;
    case 0x32:  tlt(rs,rt);     break;
    case 0x33:  tltu(rs,rt);    break;
    case 0x10:  mfhi(rd);       break;
    case 0x12:  mflo(rd);       break;
    default: cerr << "ERROR when run R instruction.";
    }
}


void add(int rs, int rt, int rd){
    registers[rd] = registers[rs] + registers[rt];
    // Detect overflow.
    if( (registers[rs]>0 && registers[rt]>0 && registers[rd]<0) \
     || (registers[rs]<0 && registers[rt]<0 && registers[rd]>0)){
        cerr << "Overflow! The program will exit." << endl;
        abort();
    }
}

void addu(int rs, int rt, int rd){
    registers[rd] = registers[rs] + registers[rt];
}

void addi(int rs,int rt,int imm){
    registers[rt] = registers[rs] + imm;
    if( (registers[rs]>0 && imm>0 && registers[rt]<0) \
     || (registers[rs]<0 && imm<0 && registers[rt]>0)){
        cerr << "Overflow! The program will exit." << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void addiu(int rs, int rt, int imm){
    registers[rt] = registers[rs] + imm;
}

void and_(int rs, int rt, int rd){
    registers[rd] = registers[rs] & registers[rt];
}

void andi(int rs, int rt, int imm){
    registers[rt] = registers[rs] & (imm & 0xffff);
}

void div_(int rs, int rt){
    registers[32] = registers[rs] % registers[rt];
    registers[33] = registers[rs] / registers[rt];
}

void divu(int rs, int rt){
    registers[32] = (unsigned int)registers[rs] % (unsigned int)registers[rt];
    registers[33] = (unsigned int)registers[rs] / (unsigned int)registers[rt];
}

void mult(int rs, int rt){
    long long x = registers[rs];
    long long y = registers[rt];
    long long result = x*y;
    registers[32] = (int) (result >> 32);
    registers[33] = (int) (result & 0xffffffff);
}

void multu(int rs, int rt){
    unsigned long long x = registers[rs];
    unsigned long long y = registers[rt];
    unsigned long long result = x*y;
    registers[32] = (unsigned int) (result >> 32);
    registers[33] = (unsigned int) (result & 0xffffffff);
}


void nor(int rs, int rt, int rd){
    registers[rd] = ~(registers[rs] | registers[rt]);
}

void or_(int rs, int rt, int rd){
    registers[rd] = registers[rs] | registers[rt];
}

void ori(int rs, int rt, int imm){
    registers[rt] = registers[rs] | (imm & 0xffff);
}

void sll(int rs, int rt, int rd, int shamt){
    registers[rd] = registers[rt] << shamt;
}

void sllv(int rs, int rt, int rd){
    registers[rd] = registers[rt] << registers[rs];
}
//
void sra(int rs, int rt, int rd, int shamt){
    registers[rd] = registers[rt] >> shamt;
}

void srav(int rs, int rt, int rd){
    registers[rd] = registers[rt] >> registers[rs];
}

void srl(int rs, int rt, int rd, int shamt){
    registers[rd] = (int)((unsigned int) registers[rt] >> (unsigned int) shamt);
}

void srlv(int rs, int rt, int rd){
    registers[rd] = (int)((unsigned int) registers[rt] >> (unsigned int) registers[rs]);
}

void sub(int rs, int rt, int rd){

    registers[rd] = registers[rs]-registers[rt];

    /*
    if( (registers[rs]>0 && registers[rt]<0 && registers[rd]<0) \
     || (registers[rs]<0 && registers[rt]>0 && registers[rd]>0)){
        cerr << "Overflow! The program will exit." << endl;
        abort();
    }
    */
}

void subu(int rs, int rt, int rd){
    registers[rd] = registers[rs]-registers[rt];
}

void xor_(int rs, int rt, int rd){
    registers[rd] = registers[rs] ^ registers[rt];
}

void xori(int rs, int rt, int imm){
    registers[rt] = registers[rs] ^ (imm & 0xffff);
}

void lui(int rt, int imm){
    registers[rt] = (imm << 16) & 0xffff0000;
}

void slt(int rs, int rt, int rd){
    if (registers[rs] < registers[rt]) registers[rd] = 1;
    else registers[rd] = 0;
}

void sltu(int rs, int rt, int rd){
    if ((unsigned int)registers[rs] < (unsigned int)registers[rt]) registers[rd] = 1;
    else registers[rd] = 0;
}

void slti(int rs, int rt, int imm){
    if (registers[rs] < imm) registers[rt] = 1;
    else registers[rt] = 0;
}
/////////////
void sltiu(int rs, int rt, int imm){
    if ((unsigned int)registers[rs] < (unsigned int)imm) registers[rt] = 1;
    else registers[rt] = 0;
}

void beq(int rs, int rt, int os){
    if (registers[rs] == registers[rt]) PC += os;
}

void bgez(int rs, int os){
    if (registers[rs] >= 0) PC += os;
}

void bgezal(int rs, int os){
    if (registers[rs] >= 0){
        registers[31] = (int&)PC-offset+4;  // Store the address of next instruction in $ra.
        PC += os;
    }
}

void bgtz(int rs, int os){
    if (registers[rs] > 0) PC += os;
}

void blez(int rs, int os){
    if(registers[rs]<=0) PC+=os;
}

void bltzal(int rs, int os){
    if(registers[rs]<0){
        registers[31] = (int&)PC-offset+4;  // Store the address of next instruction in $ra.
        PC += os;
    }
}

void bltz(int rs, int os){
    if (registers[rs]<0) PC += os;
}

void bne(int rs, int rt, int os){
    if (registers[rs] != registers[rt]) PC += os;
}

void j(int target){
    PC = (int*)p + (target-0x00100000-1);
    // target -0x001000 is the difference between the current target and the base address.
    // Because we will excute PC++ after every instruction, so we minus 1 here.
}

void jal(int target){
    registers[31] = ((int&)PC-(int&)p+4+0x00400000)/4;  // Store the address of next instruction in $ra.
    PC = (int*)p + (target-0x00100000-1);
}

void jalr(int rs, int rd){
    registers[rd] = ((int&)PC-(int&)p+4+0x00400000)/4;
    PC = (int*)p + (registers[rs]-0x00100000-1);
}

void jr(int rs){
    PC = (int*)p + (registers[rs]-0x00100000-1);
}

void teq(int rs, int rt){
    if (registers[rs] == registers[rt]){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void teqi(int rs, int imm){
    if (registers[rs] == imm){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tne(int rs, int rt){
    if (registers[rs] != registers[rt]){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tnei(int rs, int imm){
    if (registers[rs] != imm){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tge(int rs, int rt){
    if (registers[rs] >= registers[rt]){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tgeu(int rs, int rt){
    if ((unsigned int)registers[rs] >= (unsigned int)registers[rt]){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tgei(int rs, int imm){
    if (registers[rs] >= imm){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tgeiu(int rs, int imm){
    if ((unsigned int)registers[rs] >= (unsigned int)imm){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tlt(int rs, int rt){
    if (registers[rs] < registers[rt]){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tltu(int rs, int rt){
    if ((unsigned int)registers[rs] < (unsigned int)registers[rt]){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tlti(int rs, int imm){
    if (registers[rs] < imm){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void tltiu(int rs, int imm){
    if ((unsigned int)registers[rs] < (unsigned int)imm){
        cerr << "Trap exception!" << endl;
        cerr << "The current PC address is: " << (int&)PC-(int&)p+0x00400000;
        abort();
    }
}

void lb(int rs, int rt, int os){
    char * source_address = (char *) p + (registers[rs] + os-0x00400000);
    registers[rt] = (int) *source_address;
}

void lbu(int rs, int rt, int os){
    char * source_address = (char *) p + (registers[rs] + os - 0x00400000);
    registers[rt] = (int) *source_address & 0xff; // Take the last 4 bits and extent the last bits with 0s.
}
/////
void lh(int rs, int rt, int os){
    int16_t * source_address = (int16_t *) p + ((registers[rs] - 0x00400000 + os)/2 );
    registers[rt] = (int) *source_address;
}
//////
void lhu(int rs, int rt ,int os){
    int16_t * source_address = (int16_t *) p + ((registers[rs] - 0x00400000 + os)/2 );
    registers[rt] = (int) * source_address & 0xffff;
}

void lw(int rs, int rt, int os){
    int * source_address = (int*)p + ((registers[rs] - 0x00400000 + os)/4 );
    registers[rt] = *source_address;
}

void lwl(int rs, int rt, int os){
    char * source_address = (char*)p + (registers[rs] + os - 0x00400000);
    registers[rt] = (int)*source_address << 24;     // store the first bit in bits 31-24.
    source_address++;
    registers[rt] = registers[rt] | (((int)*source_address << 16) & 0xff0000);
}

void lwr(int rs, int rt, int os){
    char * source_address = (char*)p + (registers[rs] + os - 0x00400000);
    registers[rt] = (int)*source_address & 0xff;    // store the right-most bit in bits 7-0.
    source_address--;
    registers[rt] = registers[rt] | (((int)*source_address << 4) & 0xff00);
}

void ll(int rs, int rt, int os){
    int * source_address = (int*)p + ((registers[rs] - 0x00400000 + os)/4 );
    registers[rt] = *source_address;
}

void sb(int rs, int rt, int os){
    char * tartget_address = (char*)p + (registers[rs] + os - 0x00400000);
    *tartget_address = (char) (registers[rt] & 0xff);
}

void sh(int rs, int rt, int os){
    int16_t * target_address = (int16_t*)p + ((registers[rs] - 0x00400000 + os)/2 );
    *target_address = (int16_t) (registers[rt] & 0xffff);
}

void sw(int rs, int rt, int os){
    int * target_address = (int*)p + ((registers[rs] - 0x00400000 + os)/4 );
    *target_address = registers[rt];
}

void swl(int rs, int rt, int os){
    int16_t * target_address = (int16_t*)p + ((registers[rs] - 0x00400000 + os)/2 );
    *target_address = (int16_t) ((registers[rt] & 0xffff0000) >> 16);
}

void swr(int rs, int rt, int os){
    int16_t * target_address = (int16_t*)p + ((registers[rs] - 0x00400000 + os -1)/2 );
    // we minus one here to get the left address.
    *target_address = (int16_t) (registers[rt] & 0xffff);
}

void sc(int rs, int rt, int os){
    int * target_address = (int*)p + ((registers[rs] - 0x00400000 + os)/4 );
    *target_address = registers[rt];
}

void mfhi(int rd){
    registers[rd] = registers[32];
}

void mflo(int rd){
    registers[rd] = registers[33];
}

void syscall(){
    switch (registers[2]) {
    case 1: print_int();    break;
    case 4: print_string(); break;
    case 5: read_int();     break;
    case 8: read_string();  break;
    case 9: sbrk();         break;
    case 10: exit();        break;
    case 11: print_char();  break;
    case 12: read_char();   break;
    case 13: open();        break;
    case 14: read();        break;
    case 15: write();       break;
    case 16: close();       break;
    case 17: exit2();       break;
    }
}

void print_int(){

    int i = registers[4];
    //cout << dec << i << endl;
    terminalfile << dec << i << "\n";
    output_content.to_print = true;
    output_content.content = to_string(i);
}
/////
void print_string(){
    char * string_address = (char*)p + (registers[4]-0x00400000);
    //cout << string_address << endl;
    terminalfile << string_address << "\n";
    output_content.to_print = true;
    output_content.content = string_address;
}

void read_int(){
    int i;
    cin >> i;
    registers[2] = i;
}
//
void read_string(){
    int length = registers[5];
    int address = registers[4];
    char *str = (char*) malloc(length);
    cin >> str;
    char * real_address = (char*)p + (address-0x00400000);
    *real_address = *str;
}

void sbrk(){
    int amount = registers[4];
    registers[2] = (int&)heap_pointer-(int&)p+0x00400000;
    heap_pointer += ((amount-1)/4+1)*4;
}

void exit(){
    //exit(0);
    //cout << "Program exit" << endl;
}

void print_char(){
    char c = (char)registers[4];
    //cout << c << endl;
    terminalfile << c << "\n";
    output_content.to_print = true;
    output_content.content = to_string(c);
}

void read_char(){
    char c;
    cin >> c;
    registers[2] = (int) c;
}

void open(){
    char * filename = (char*)p+registers[4]-0x00400000;
    file.open(filename);
    if (file.fail()) file.open(filename,ios::in | ios::out | ios::trunc);
    if (!file.fail()) registers[4] = 1;
    else {
        registers[4] = -1;
        cerr << "Fail to open file" << endl;
    }
}

void read(){
    char * buf = (char*)p + (registers[5]-0x00400000);
    int length = registers[6];
    registers[4] =file.read(buf,length).gcount();
}

void write(){
    char * buf = (char*)p + (registers[5]-0x00400000);
    int length = registers[6];
    file.write(buf,length);
    registers[4] = length;
}

void close(){
    int descriptor = registers[4];
    if (descriptor == 1) file.close();
    else if (descriptor == -1) cerr << "Fail to close the file." << endl;
}

void exit2(){
    int result = registers[4];
    cout << "exit2. The return value is: ";
    cout << result;
    exit(result);
}
