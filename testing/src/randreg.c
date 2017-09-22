#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define		EXT_MUL

#ifdef EXT_MUL

#define r_type_insn(_f7, _rs2, _rs1, _f3, _rd, _opc) \
(((_f7) << 25) | ((_rs2) << 20) | ((_rs1) << 15) | ((_f3) << 12) | ((_rd) << 7) | ((_opc) << 0))

#define ext_mul(_rd, _rs1, _rs2) \
r_type_insn(0b0000000, _rs2, _rs1, 0b000, _rd, 0b1000111)
#define NO_INSTR  18 //bad idea

#else

#define NO_INSTR  17

#endif

char *instr[]={
  "add",
  "sub",
  "addi",
  "and",
  "andi",
  "or",
  "ori",
  "xor",
  "xori",
  "srl",
  "sra",
  "sll",
  "lui",
  "auipc",
  "slt",
  "slti",
  "sltu",
#ifdef EXT_MUL
  "#ext_mul",
#endif


};

#define SHIFT(s) (!strcmp(s,"srl") || !strcmp(s,"srli") || !strcmp(s,"sll") || !strcmp(s,"slli") || !strcmp(s,"sra") || !strcmp(s,"srai"))
#define UI(s) (strstr(s,"ui"))
#define EXT(s) (strstr(s,"ext"))

void initREGS(int rcnt){
  int i;
  for(i=1; i<rcnt; i++){
    printf("li\tx%d, %d\n", i, rand()%i);
  }
}


void genAL(int cnt, int rcnt){
  int i;
  for(i=0; i<cnt; i++) {
    int r = rand()%12;
    int rs1 = rand()%rcnt;
    int rs2 = rand()%rcnt;
    int rd = rand()%rcnt;
    int Imm = rand()%2048;
    if(rand()%2) Imm = Imm * -1;
    int inst = rand()%NO_INSTR;
    printf("%s",instr[inst]);

	if(EXT(instr[inst])){
		if(strstr(instr[inst], "mul")){
			printf("\t\tx%d, x%d, x%d\n", rd, rs1, rs2);
			printf(".word\t\t%d\n", ext_mul(rd,rs1,rs2));
		
		}
	} else if(UI(instr[inst])) {
        Imm = abs(Imm);
        printf("\t\tx%d, %d\n", rd, Imm);
    }
    else if(instr[inst][strlen(instr[inst])-1]=='i'){
      if(SHIFT(instr[inst]))
        Imm = Imm % 32;
        printf("\t\t\tx%d, x%d, %d\n", rd, rs1, Imm);
    } else {
        if(SHIFT(instr[inst]))
          rs2 = rs2 % 32;
        printf("\t\t\tx%d, x%d, x%d\n", rd, rs1, rs2);
    }
  }
}

void genStore(int rs1, int rs2, int off){
  printf("sw\tx%d, x%d(%d)\n", rs2, rs1, off);
}

void genLoad(int rd, int rs1, int off){
  printf("lw\tx%d, x%d(%d)\n", rd, rs1, off);
}

int main(int argc, char *argv[]){
  int i;

  if(argc<2) {
    printf("missing arguments\nuse: randreg <no_of_inst> [no_of_regs]\n");
    return 0;
  }

  //printf("\n%d\n", argc);

  int cnt = atoi(argv[1]);
  int rcnt = (argc==3) ? atoi(argv[2]) : 32;

  srand (time(NULL));

  //printf("#define r_type_insn(_f7, _rs2, _rs1, _f3, _rd, _opc)\\\n.word (((_f7) << 25) | ((_rs2) << 20) | ((_rs1) << 15) | ((_f3) << 12) | ((_rd) << 7) | ((_opc) << 0))\n\n#define cust_mul(_rd, _rs1, _rs2)\\\nr_type_insn(0b0000000, _rs2, _rs1, 0b000, _rd, 0b1000111)");

  initREGS(rcnt);
  genAL(cnt, rcnt);

  // exit
  printf("li\ta7, 10\necall\n");
  return 0;

}
