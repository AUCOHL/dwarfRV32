#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define NO_INSTR  17
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

};

#define SHIFT(s) (!strcmp(s,"srl") || !strcmp(s,"srli") || !strcmp(s,"sll") || !strcmp(s,"slli") || !strcmp(s,"sra") || !strcmp(s,"srai"))
#define UI(s) (strstr(s,"ui"))

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

    if(UI(instr[inst])) {
        Imm = abs(Imm);
        printf("\tx%d, %d\n", rd, Imm);
    }
    else if(instr[inst][strlen(instr[inst])-1]=='i'){
      if(SHIFT(instr[inst]))
        Imm = Imm % 32;
        printf("\tx%d, x%d, %d\n", rd, rs1, Imm);
    } else {
        if(SHIFT(instr[inst]))
          rs2 = rs2 % 32;
        printf("\tx%d, x%d, x%d\n", rd, rs1, rs2);
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

  initREGS(rcnt);
  genAL(cnt, rcnt);

  // exit
  printf("li\ta7, 10\necall\n");
  return 0;

}
