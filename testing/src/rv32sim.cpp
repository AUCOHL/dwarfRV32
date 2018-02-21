#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include "stdlib.h"

using namespace std;

#define MMAP_PRINT 0x80000000

std::set<int> breakpoints;
map<int, int> externalInterrupts; // set containing the pc address value needed to trigger an external interrupt and interrupt number
const int regCount = 32;
int regs[regCount] = {0};
int uie = 0;
int timer = 0;
int ucause = 0; 
int epc = 0; // register used by uret
unsigned int pc = 0x0;
const unsigned int RAM = 64 * 1024; // only 8KB of memory located at address 0
char memory[RAM];
bool terminated = false;


void emitError(string);


unsigned int readInstruction();


void printPrefix(unsigned int, unsigned int);

unsigned int immediateIUNSIGNED(unsigned int);
unsigned int immediateI(unsigned int);
unsigned int immediateS(unsigned int);
unsigned int immediateB(unsigned int);
unsigned int immediateU(unsigned int);
unsigned int immediateJ(unsigned int);

void setZero();

void instDecExec(unsigned int);


void printInstU(string, unsigned int, unsigned int);
void LUI(unsigned int, unsigned int);
void AUIPC(unsigned int, unsigned int);

void printInstUJ(string, unsigned int, unsigned int);
void JAL(unsigned int, unsigned int);

void printInstSys(string inst, unsigned int rd, unsigned int rs1, unsigned int rs2);
void printInstB(string, unsigned int, unsigned int, unsigned int);
void JALR(unsigned int, unsigned int, unsigned int);
void B_Inst(unsigned int, unsigned int, unsigned int, unsigned int);
void BEQ(unsigned int, unsigned int, unsigned int);
void BNE(unsigned int, unsigned int, unsigned int);
void BLT(unsigned int, unsigned int, unsigned int);
void BGE(unsigned int, unsigned int, unsigned int);
void BLTU(unsigned int, unsigned int, unsigned int);
void BGEU(unsigned int, unsigned int, unsigned int);

void printInstL_S(string, unsigned int, unsigned int, unsigned int);
void L_Inst(unsigned int, unsigned int, unsigned int, unsigned int);
void LB(unsigned int, unsigned int, unsigned int, unsigned int);
void LH(unsigned int, unsigned int, unsigned int, unsigned int);
void LW(unsigned int, unsigned int, unsigned int, unsigned int);
void LBU(unsigned int, unsigned int, unsigned int, unsigned int);
void LHU(unsigned int, unsigned int, unsigned int, unsigned int);
void S_Inst(unsigned int, unsigned int, unsigned int, unsigned int);
void SB(unsigned int, unsigned int, unsigned int, unsigned int);
void SH(unsigned int, unsigned int, unsigned int, unsigned int);
void SW(unsigned int, unsigned int, unsigned int, unsigned int);

void printInstI(string, unsigned int, unsigned int, unsigned int);
void I_Inst(unsigned int, unsigned int, unsigned int, unsigned int);
void ADDI(unsigned int, unsigned int, unsigned int);
void SLLI(unsigned int, unsigned int, unsigned int);
void SLTI(unsigned int, unsigned int, unsigned int);
void SLTIU(unsigned int, unsigned int, unsigned int);
void XORI(unsigned int, unsigned int, unsigned int);
void ORI(unsigned int, unsigned int, unsigned int);
void ANDI(unsigned int, unsigned int, unsigned int);
void SRLI(unsigned int, unsigned int, unsigned int);
void SRAI(unsigned int, unsigned int, unsigned int);

void printInstR(string, unsigned int, unsigned int, unsigned int);
void R_Inst(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void ADD(unsigned int, unsigned int, unsigned int);
void SUB(unsigned int, unsigned int, unsigned int);
void SLL(unsigned int, unsigned int, unsigned int);
void SLT(unsigned int, unsigned int, unsigned int);
void SLTU(unsigned int, unsigned int, unsigned int);
void XOR(unsigned int, unsigned int, unsigned int);
void SRL(unsigned int, unsigned int, unsigned int);
void SRA(unsigned int, unsigned int, unsigned int);
void OR(unsigned int, unsigned int, unsigned int);
void AND(unsigned int, unsigned int, unsigned int);

void Ext_Inst(unsigned int, unsigned int, unsigned int, unsigned int);
void MUL(unsigned int, unsigned int, unsigned int);

void printInstE();
void ECALL();
void SYS_Inst(int rd, int rs1, int imm, int func);
void timerInterrupt();
void URET();
void printInteger();
void printString();
void readInteger();
void terminateExecution();
void EBREAK();
void updateTimer(); 
void loadBreakpoints(char *file);
void loadExternalInterrupts(char *file);
void checkForBreakpoints();
void checkForExternalInterrupts();

int main(int argc, char* argv[]) {
     unsigned int instWord = 0;
     ifstream inFile;

     if(argc < 1){
         emitError("Use: rv32i_sim <machine_code_file_name>");
     }else if(argc == 3){
        puts("loading breakpoints");
     	loadBreakpoints(argv[2]);
     }else if(argc == 4){
         puts("loading breakpoints & externalInterrupt locations");
     	loadBreakpoints(argv[2]);
        loadExternalInterrupts(argv[3]);
     }

     inFile.open(argv[1], ios::in | ios::binary | ios::ate);

     if(inFile.is_open()) {
         long long fsize = inFile.tellg();
         inFile.seekg (0, inFile.beg);

         if(!inFile.read((char *)memory, fsize)){
            emitError("Cannot read from input file");
         }

         while(!terminated) {
            checkForExternalInterrupts();
            checkForBreakpoints();
            updateTimer();

            instWord = 	readInstruction();  // read next instruction
            pc += 4;    // increment pc by 4
            instDecExec(instWord);
            regs[0] = 0;
         }

         // check if terminated correcctly
         if( !terminated  ){
             cerr << "Illegal memory access" << endl;
         }

         // dump the registers
         for(int i = 0; i < regCount; i++) {
             cout << "x" << dec << i << ": \t";
             cout << "0x" << hex << std::setfill('0') << std::setw(regCount / 4) << regs[i] << '\t' << dec << regs[i] << endl;
         }

         exit(0);

     } else {
         emitError("Cannot access input file");
     }
}

void updateTimer()
{
    if(timer > 1){
        timer -= 1;
    }else if(timer > 0 && (uie & 0x1) == 0x1){
        timerInterrupt();
        timer = 0;
        puts("timer went off");
    }
}

// dump the registers
void dumpRegs(){
  for(int i = 0; i < regCount; i++) {
      cout << "x" << dec << i << ": \t";
      cout << "0x" << hex << std::setfill('0') << std::setw(regCount / 4) << regs[i] << '\t' << dec << regs[i] << endl;
  }
}

void emitError(string error_message)
{
    cerr << error_message << endl;;
    exit(0);
}


unsigned int readInstruction()
{
    return (  (unsigned char)memory[pc]           |
            (((unsigned char)memory[pc+1]) << 8)  |
            (((unsigned char)memory[pc+2]) << 16) |
            (((unsigned char)memory[pc+3]) << 24) );
}

void printPrefix(unsigned int instA, unsigned int instW)
{
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t";
    cout << "0x" << std::setw(8) << instW;
}

unsigned int immediateI(unsigned int instWord)
{
    return ( ( (instWord >> 20) & 0x7FF ) | ( (instWord >> 31) ? 0xFFFFF800 : 0x0 ) );
}

unsigned int immediateIUNSIGNED(unsigned int instWord)
{
    return ( ( (instWord >> 20) & 0xFFF ) );
}

unsigned int immediateB(unsigned int instWord)
{
    unsigned int imm = 0;
    unsigned int a, b, c, sign;

    a = (instWord >> 7) & 0x1;
    a = a << 11;

    b = (instWord >> 25) & 0x3F;
    b = b << 5;

    c = (instWord >> 8) & 0xF;
    c = c << 1;

    sign = instWord >> 31;

    imm = imm | a | b | c | (sign ? 0xFFFFF000 : 0x0) ;

    return (imm);

}
unsigned int immediateJ(unsigned int instWord)
{
    unsigned int imm = 0;
    unsigned int a, b, c, d, sign;

    a = (instWord >> 12) & 0x000000FF;
    a = a << 12;

    b = (instWord >> 20) & 0x00000001;
    b = b << 11;

    c = (instWord >> 21) & 0x000003FF;
    c = c << 1;

    d = (instWord >> 31) & 0x00000001;
    d = d << 20;

    sign = instWord >> 31;

    imm = a | b | c | d | (sign ? 0xFFF00000 : 0x0) ;

    return (imm);

}
unsigned int immediateS(unsigned int instWord)
{
    return ( ((instWord >> 7) & 0x1F) | ((instWord >> 20) & 0xFE0) | (((instWord >> 31) ? 0xFFFFF800 : 0x0)) );
}
unsigned int immediateU(unsigned int instWord)
{
    return (instWord & 0xFFFFF000);
}

void setZero()
{
    regs[0] = 0;
}

void instDecExec(unsigned int instWord)
{
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, B_imm, J_imm, S_imm, U_imm;

    unsigned int instPC = pc - 4;

    opcode = instWord & 0x0000007F;

    rd = (instWord >> 7) & 0x0000001F;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;

    funct3 = (instWord >> 12) & 0x00000007;
    funct7 = (instWord >> 25) & 0x0000007F;

    I_imm = immediateI(instWord);
    B_imm = immediateB(instWord);
    J_imm = immediateJ(instWord);
    S_imm = immediateS(instWord);
    U_imm = immediateU(instWord);

    printPrefix(instPC, instWord);

    switch(opcode)
    {
        case 0x47:
            Ext_Inst(rd, rs1, rs2, funct3);
            break;
        case 0x37:  // Load Upper Immediate
            LUI(rd, U_imm);
            break;
        case 0x17:  // Add Unsigned Immediate Program Counter
            AUIPC(rd, U_imm);
            break;
        case 0x6F:  // Jump And Link
            JAL(rd, J_imm);
            break;
        case 0x67:  // Jump And Link Return
            JALR(rd, rs1, I_imm);
            //if((instWord&0xffff)==0x00008067) dumpRegs();
            break;
        case 0x63:  // Branch Instructions
            B_Inst(rs1, rs2, B_imm, funct3);
            break;
        case 0x03:  // Load Instructions
            L_Inst(rd, rs1, I_imm, funct3);
            break;
        case 0x23:  // Store Insturctions
            S_Inst(rs2, rs1, S_imm, funct3);
            break;
        case 0x13:  // Immediate Instructions
            I_Inst(rd, rs1, I_imm, funct3);
            break;
        case 0x33:  // Register Instructions
            R_Inst(rd, rs1, rs2, funct3, funct7);
            break;
        case 0x73:  // system calls & privileged instructions
            {
                int IU_imm = immediateIUNSIGNED(instWord);
                SYS_Inst(rd, rs1, IU_imm, funct3);
            }
            break;
        default:
            cout << "\tUnknown Instruction Type" << endl;
            exit(-1);
    }

    cout << "RF[" << rd << "]=" << regs[rd] << endl;
    setZero();
}


void printInstU(string inst, unsigned int rd, unsigned int imm)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << rd << ", ";
    cout << int(imm) << endl;
}
void LUI(unsigned int rd, unsigned int U_imm)
{
    printInstU("LUI", rd, (U_imm >> 12) & 0xFFFFF);

    regs[rd] =  U_imm;
}
void AUIPC(unsigned int rd, unsigned int U_imm)
{
    printInstU("AUIPC", rd, (U_imm >> 12) & 0xFFFFF);

    regs[rd] = (pc - 4) + U_imm;

    cout << "x" << rd << ": " << regs[rd] << "\n";
}

void printInstUJ(string inst, unsigned int rd, unsigned int target_address)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << rd << ", ";
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << target_address << endl;
}
void JAL(unsigned int rd, unsigned int J_imm)
{
    unsigned address = J_imm + (pc - 4);

    printInstUJ("JAL", rd, address);

    regs[rd] = pc;
    pc =  address;
}

void printInstB(string inst, unsigned int rs1, unsigned int rs2, unsigned int target_address)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << rs1 << ", x" << rs2 << ", ";
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << target_address << endl;
}
void JALR(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    unsigned int address = (I_imm + regs[rs1]) & 0xFFFFFFFE;

    printInstB("JALR", rd, rs1, address);

    regs[rd] = pc;
    pc = address;

    //dumpRegs();
    //cout << "PC=" << pc << endl;
}
void B_Inst(unsigned int rs1, unsigned int rs2, unsigned int B_imm, unsigned int funct3)
{
    unsigned int address = B_imm + (pc - 4);
    switch(funct3)
    {
        case 0x0:
            BEQ(rs1, rs2, address);
            break;
        case 0x1:
            BNE(rs1, rs2, address);
            break;
        case 0x4:
            BLT(rs1, rs2, address);
            break;
        case 0x5:
            BGE(rs1, rs2, address);
            break;
        case 0x6:
            BLTU(rs1, rs2, address);
            break;
        case 0x7:
            BGEU(rs1, rs2, address);
            break;
        default:
            cout << "\tInvalid Branch Instruction" << endl;
    }
}
void BEQ(unsigned int rs1, unsigned int rs2, unsigned int address)
{
    printInstB("BEQ", rs1, rs2, address);

    if (regs[rs1] == regs[rs2])
        pc = address;
}
void BNE(unsigned int rs1, unsigned int rs2, unsigned int address)
{
    printInstB("BNE", rs1, rs2, address);

    if (regs[rs1] != regs[rs2])
        pc = address;
}
void BLT(unsigned int rs1, unsigned int rs2, unsigned int address)
{
    printInstB("BLT", rs1, rs2, address);

    if (regs[rs1] < regs[rs2])
        pc = address;
}
void BGE(unsigned int rs1, unsigned int rs2, unsigned int address)
{
    printInstB("BGE", rs1, rs2, address);

    if (regs[rs1] >= regs[rs2])
        pc = address;
}
void BLTU(unsigned int rs1, unsigned int rs2, unsigned int address)
{
    printInstB("BLTU", rs1, rs2, address);

    if ((unsigned int)(regs[rs1]) < (unsigned int)(regs[rs2]))
        pc = address;
}
void BGEU(unsigned int rs1, unsigned int rs2, unsigned int address)
{
    printInstB("BGEU", rs1, rs2, address);

    if ((unsigned int)regs[rs1] >= (unsigned int)regs[rs2])
        pc = address;
}

void printInstL_S(string inst, unsigned int first, unsigned int second, unsigned int offset)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << first << ", " << int(offset) << "(x" << second << ")" << endl;
    //if(inst[0]=='L') cout << "RF[" << first << "]=" << regs[first] << endl;
}
void L_Inst(unsigned int rd, unsigned int rs1, unsigned int I_imm, unsigned int funct3)
{
    unsigned int address = regs[rs1] + I_imm;
    unsigned int offset = I_imm;

    switch (funct3)
    {
        case 0:
            LB(rd, rs1, offset, address);
            break;
        case 1:
            LH(rd, rs1, offset, address);
            break;
        case 2:
            LW(rd, rs1, offset, address);
            break;
        case 4:
            LBU(rd, rs1, offset, address);
            break;
        case 5:
            LHU(rd, rs1, offset, address);
            break;
        default:
            cout << "\tInvalid Load Instruction" << endl;
    }
}
void LB(unsigned int rd, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("LB", rd, rs1, offset);

    regs[rd] = memory[address];
}
void LH(unsigned int rd, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("LH", rd, rs1, offset);

    regs[rd] = memory[address+1];
    regs[rd] = regs[rd] << 8;
    regs[rd] = regs[rd] | (unsigned char)memory[address];
}
void LW(unsigned int rd, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("LW", rd, rs1, offset);
    if(address%4) cout << "Memory address not divisible by 4 -- " << address << endl;


    regs[rd] = (unsigned char)memory[address+3];
    regs[rd] = regs[rd] << 8;
    regs[rd] = regs[rd] | (unsigned char)memory[address+2];
    regs[rd] = regs[rd] << 8;
    regs[rd] = regs[rd] | (unsigned char)memory[address+1];
    regs[rd] = regs[rd] << 8;
    regs[rd] = regs[rd] | (unsigned char)memory[address];

    //cout << "reading from: " << address << " " << regs[rd] << endl;
}
void LBU(unsigned int rd, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("LBU", rd, rs1, offset);

    regs[rd] = (unsigned char)memory[address];
}
void LHU(unsigned int rd, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("LHU", rd, rs1, offset);

    regs[rd] = (unsigned char)memory[address+1];
    regs[rd] = regs[rd] << 8;
    regs[rd] = regs[rd] | (unsigned char)memory[address];
}
void S_Inst(unsigned int rs2, unsigned int rs1, unsigned int S_imm, unsigned int funct3)
{
    unsigned int address = regs[rs1] + S_imm;
    unsigned int offset = S_imm & 0xFFF;

    switch (funct3)
    {
        case 0:
            SB(rs2, rs1, offset, address);
            break;
        case 1:
            SH(rs2, rs1, offset, address);
            break;
        case 2:
            SW(rs2, rs1, offset, address);
            break;
        default:
            cout << "\tInvalid Store Instruction" << endl;
    }
}
void SB(unsigned int rs2, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("SB", rs2, rs1, offset);

    memory[address] = regs[rs2] & 0xFF;
}
void SH(unsigned int rs2, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("SH", rs2, rs1, offset);

    memory[address] = regs[rs2] & 0xFF;
    memory[address + 1] = (regs[rs2] >> 8) & 0xFF;
}
void SW(unsigned int rs2, unsigned int rs1, unsigned int offset, unsigned int address)
{
    printInstL_S("SW", rs2, rs1, offset);

    if(address%4) cout << "Memory address not divisible by 4 -- " << address << endl;
    if (address == MMAP_PRINT)
	    cout << regs[rs2];
    else{
	    cout << "writing to: " << address << " " << regs[rs2] << endl;
	    memory[address] = regs[rs2] & 0xFF;
	    memory[address + 1] = (regs[rs2] >> 8) & 0xFF;
	    memory[address + 2] = (regs[rs2] >> 16) & 0xFF;
	    memory[address + 3] = (regs[rs2] >> 24) & 0xFF;
    }
}

void printInstI(string inst, unsigned int rd, unsigned int rs1, unsigned int imm)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << rd << ", x" << rs1 << ", " << int(imm) << endl;
    //cout <<"RF["<<rd<<"]="<< regs[rd] << endl;
}
void I_Inst(unsigned int rd, unsigned int rs1, unsigned int I_imm, unsigned int funct3)
{
    unsigned int shamt = I_imm & 0x1F;
    unsigned int funct7 = (I_imm >> 5) & 0x7F;

    switch(funct3){
        case 0x0:
            ADDI(rd, rs1, I_imm);
            break;
        case 0x1:
            SLLI(rd, rs1, shamt);
            break;
        case 0x2:
            SLTI(rd, rs1, I_imm);
            break;
        case 0x3:
            SLTIU(rd, rs1, I_imm);
            break;
        case 0x4:
            XORI(rd, rs1, I_imm);
            break;
        case 0x5:
            switch(funct7){
                case 0x0:
                    SRLI(rd, rs1, shamt);
                    break;
                case 0x20:
                    SRAI(rd, rs1, shamt);
                    break;
                default:
                    cout << "\tInvalid I Instruction" << endl;
            }
            break;
        case 0x6:
            ORI(rd, rs1, I_imm);
            break;
        case 0x7:
            ANDI(rd, rs1, I_imm);
            break;
        default:
            cout << "\tInvalid I Instruction" << endl;
    }
}
void ADDI(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    printInstI("ADDI", rd, rs1, I_imm);

    regs[rd] = regs[rs1] + (int)I_imm;
}
void SLLI(unsigned int rd, unsigned int rs1, unsigned int shamt)
{
    printInstI("SLLI", rd, rs1, shamt);

    regs[rd] = regs[rs1] << shamt;
}
void SLTI(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    printInstI("SLTI", rd, rs1, I_imm);

    regs[rd] = regs[rs1] < int(I_imm);
}
void SLTIU(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    printInstI("SLTIU", rd, rs1, I_imm);

    regs[rd] = (unsigned int)(regs[rs1]) < I_imm;
}
void XORI(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    printInstI("XORI", rd, rs1, I_imm);

    regs[rd] = regs[rs1] ^ int(I_imm);
}
void SRLI(unsigned int rd, unsigned int rs1, unsigned int shamt)
{
    printInstI("SRLI", rd, rs1, shamt);

    regs[rd] = (unsigned int)(regs[rs1]) >> shamt;
}
void SRAI(unsigned int rd, unsigned int rs1, unsigned int shamt)
{
    printInstI("SRAI", rd, rs1, shamt);

    regs[rd] = regs[rs1] >> shamt;
}
void ORI(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    printInstI("ORI", rd, rs1, I_imm);

    regs[rd] = regs[rs1] | int(I_imm);
}
void ANDI(unsigned int rd, unsigned int rs1, unsigned int I_imm)
{
    printInstI("ANDI", rd, rs1, I_imm);

    regs[rd] = regs[rs1] & int(I_imm);
}

void printInstSys(string inst, unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << rd << ", " << rs1 << ", x" << rs2 <<hex<<"( 0x"<< regs[rs2] <<" )"<< endl;
    //cout <<"RF["<<rd<<"]="<< regs[rd] << endl;
}

void printInstR(string inst, unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    cout << dec;
    cout << '\t' << inst << "\tx" << rd << ", x" << rs1 << ", x" << rs2 << endl;
    //cout <<"RF["<<rd<<"]="<< regs[rd] << endl;
}

void R_Inst(unsigned int rd, unsigned int rs1, unsigned int rs2, unsigned int funct3, unsigned int funct7)
{
    switch(funct3){
        case 0x0:
            switch(funct7){
                case 0x0:
                    ADD(rd, rs1, rs2);
                    break;
                case 0x20:
                    SUB(rd, rs1, rs2);
                    break;
                default:
                    cout << "\tInvalid R Instruction \n";
            }
            break;
        case 0x1:
            SLL(rd, rs1, rs2);
            break;
        case 0x2:
            SLT(rd, rs1, rs2);
            break;
        case 0x3:
            SLTU(rd, rs1, rs2);
            break;
        case 0x4:
            XOR(rd, rs1, rs2);
            break;
        case 0x5:
            switch(funct7){
                case 0x0:
                    SRL(rd, rs1, rs2);
                    break;
                case 0x20:
                    SRA(rd, rs1, rs2);
                    break;
                default:
                    cout << "\tInvalid R Instruction \n";
            }
            break;
        case 0x6:
            OR(rd, rs1, rs2);
            break;
        case 0x7:
            AND(rd, rs1, rs2);
            break;
        default:
            cout << "\tInvalid R Instruction" << endl;
    }
}
void ADD(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("ADD", rd, rs1, rs2);

    regs[rd] = regs[rs1] + regs[rs2];
}
void SUB(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("SUB", rd, rs1, rs2);

    regs[rd] = regs[rs1] - regs[rs2];
}
void SLL(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("SLL", rd, rs1, rs2);

    regs[rd] = regs[rs1] <<  regs[rs2];
}
void SLT(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("SLT", rd, rs1, rs2);

    regs[rd] = regs[rs1] <  regs[rs2];
}
void SLTU(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("SLTU", rd, rs1, rs2);

    regs[rd] = (unsigned int)(regs[rs1]) < (unsigned int)(regs[rs2]);
}
void XOR(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("XOR", rd, rs1, rs2);

    regs[rd] = regs[rs1] ^ regs[rs2];
}
void SRL(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("SRL", rd, rs1, rs2);

    regs[rd] = (unsigned int)(regs[rs1]) >> (unsigned int)(regs[rs2]);
    //cout << "SRL x" << rd << " gets " << regs[rd] << " - " <<  regs[rs1] << " - " << <<\n";
}
void SRA(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("SRA", rd, rs1, rs2);

    regs[rd] = regs[rs1] >> (unsigned int)(regs[rs2]);
}
void OR(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("OR", rd, rs1, rs2);

    regs[rd] = regs[rs1] | regs[rs2];
}
void AND(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("AND", rd, rs1, rs2);

    regs[rd] = regs[rs1] & regs[rs2];
}

void Ext_Inst(unsigned int rd, unsigned int rs1, unsigned int rs2, unsigned int funct3)
{
  switch(funct3){
      case 0x0: //mul
		  MUL(rd, rs1, rs2);
		  break;
	  default:
		  cout << "\tUnsupported Extension Instruction \n";
   }
}

void MUL(unsigned int rd, unsigned int rs1, unsigned int rs2)
{
    printInstR("EXT_MUL", rd, rs1, rs2);

	regs[rd] = regs[rs1] * regs[rs2];
}

void printInstE()
{
    cout << "\tECALL" << endl;
}
void ECALL()
{
    printInstE();

    switch(regs[17]){
        case 1:
            printInteger();
            break;
        case 4:
            printString();
            break;
        case 5:
            readInteger();
            break;
        case 10:
            terminateExecution();
            break;
        default:
            cout << "\tUnknown Enviroment Instruction" << endl;
    }
}
void printInteger()
{
    cout << dec << regs[10] << endl;
}
void printString()
{
    unsigned int address = (unsigned int)regs[10];

    while(address < RAM && memory[address] != '\0'){
        cout.put(memory[address++]);
    }
}
void readInteger()
{
    cin >> regs[10];
}
void terminateExecution()
{
    terminated = true;
}

void SYS_Inst(int rd, int rs1, int imm, int func)
{
    if(func == 0x0 && imm == 0){
        puts("\nMAKING ECALL");
        ECALL(); // ecall  
    }else if(func == 0 && imm == 0x2){ // uret
        puts("\nMAKING URET");
        URET();
    }else if(func == 0x1){ // csrrw rd, uie, rs1
        if(imm == 0x4){ //uie
            puts("\n updating UIE");
            int tmp = regs[rs1];
            regs[rd] = uie;
            uie = tmp;
            printInstSys("csrrw ", rd, 0x4 ,rs1);
        }else if(imm  == 0xc01){ // csrrw rd, timer, rs1
            int tmp = regs[rs1];
            regs[rd] = timer;
            timer = tmp;
            printf("\n updating TIMER\t%d\n", tmp);
        }else if(imm  == 0x41){ // csrrw rd, epc, rs1
			int tmp = regs[rs1];
            regs[rd] = epc;
            epc = tmp;
			printf("\nEPC UPDATED with %d\n", epc);
		}else if(imm == 0x42){
            int tmp = regs[rs1];
            regs[rd] = uie;
            ucause = tmp;
        }else{
            printf("\nimmediate %d\n",imm);
            puts("Accessing unimplemented control/status register");
            throw "Accessing unimplemented control/status register";
        }
        puts("\n doing CSRRW operation");
    }else if(func == 0x7 && imm == 0x4){
        int clearBit = 1<<rs1;
        if(uie & clearBit != 0)
            uie ^= clearBit;
        regs[rd] = uie;
        printInstSys("csrrci", rd, imm, rs1);
    }else if(imm == 1 && rs1 == 0 && rd == 0 && func == 0){
        EBREAK();
    }else{
        puts("unimplemented system instruction error");
        throw "unimplemented system instruction error";
    }
}

void timerInterrupt()
{
   // if(uie == 0x3){ // global and timer interrupt enabled
    uie = uie & 0xfffe; // turn off global interrupts
    epc = pc;
    pc = 48;
    //}
}

void URET()
{
    printf("SETTING PC TO EPC %d\n",epc);
    pc = epc; // return pc to proper location
    uie |= 1; // enable interrupts
}

void EBREAK()
{
    epc = pc;
    pc = 32;
    uie = uie & 0xfffe; // turn off global interrupts
}

void loadBreakpoints(char *file)
{
	std::ifstream fs(file);
	if(!fs.is_open()){
		puts("failed to load break points");
		exit(-1);
	}
	
    do{
        
	    int address;
		fs>>address;
        printf("INPUT: %d\n",address);
		
        if(fs.eof()){
			fs.close();
		}else{
			breakpoints.insert(address);
            printf("breakpoint at  %d\n", address);
		}
	}while(fs.is_open());
}

void loadExternalInterrupts(char *file)
{
    std::ifstream fs(file);
	if(!fs.is_open()){
		puts("failed to load break points");
		exit(-1);
	}
	
    do{
	    int address, interruptNumber;
		fs>>address>>interruptNumber;
        printf("INPUT: %d\n",address);
		
        if(fs.eof()){
			fs.close();
		}else{
			externalInterrupts[address] = interruptNumber;
            printf("breakpoint at  %d\n", address);
		}
	}while(fs.is_open());
}

void checkForBreakpoints()
{
    if(breakpoints.count(pc)){
        // printf("---BREAKPOINT-----%08x----------------------------------\n", pc);
        //     for(int i = 0; i < 32; ++i)
        //         printf("REG %d \t %08x\n",i, regs[i]);
            
        //     printf("REG UIE \t %08x\n", uie);
        //     printf("REG EPC \t %08x\n", epc);
        //     printf("REG TIMER \t %08x\n", timer);
        //     // cout<<"REG UIE"<<"\t"<<uie<<endl;
        //     // cout<<"REG EPC"<<"\t"<<epc<<endl;
        //     // cout<<"REG TIMER"<<"\t"<<timer<<endl;

        // printf("---END---BREAKPOINT-----------------------------------\n");

        void *ptr = memory + 13760;
        int *num = (int *) ptr;

        puts("final test array");
        for(int i = 0; i < 150; ++i){
            printf("%d\n", num[i]);
        }

        exit(0);
    }
}

void checkForExternalInterrupts()
{
    if((uie & 1) == 0 || externalInterrupts.empty()) 
        return;
    
    int i, uieTmp = uie>>3;
    for(i = 0; i < 15; ++i, uieTmp >>=1){
        if(uieTmp & 1 == 1){
            break;
        }
    }

    if(i < 15){ // external interrupt flag on
        uie = uie & 0xfffe; // turn off global interrupts
        epc = pc;
        pc = 64 + i * 4; //jump to address
        ucause = i;
    }
}