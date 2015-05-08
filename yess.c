#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 12800 * 4
#define eax 0
#define ecx 1
#define edx 2
#define ebx 3
#define esp 4
#define ebp 5
#define esi 6
#define edi 7

typedef struct{
  int cz;
  int cs;
  int co;
} conditionFlags;

typedef struct{
  conditionFlags cf;
  int reg[8];
  int pc;
  int stat;
  char mem[SIZE];
} everything;


//insert functions
void printStat (everything * s, int status) {
  if (s -> stat == 1){
      printf("Status: AOK\n");
  }
  if (s -> stat == 2){
      printf("Status: HLT\n");
  }
  if (s -> stat == 3){
      printf("Status: ADR\n");
  }
  if (s -> stat == 4){
      printf("Status: INS\n");
  }
}

void printOutput(everything * s, int steps){
  printf("Steps: %d\n", steps);
  printf("PC: 0x%08X\n", s -> pc);
  printStat(s, s -> stat);
  printf("CZ: %d\n", s -> cf.cz);
  printf("CS: %d\n", s -> cf.cs);
  printf("CO: %d\n", s -> cf.co);
  printf("eax:0x%08X\n", s -> reg[eax]);
  printf("ecx:0x%08X\n", s -> reg[ecx]);
  printf("edx:0x%08X\n", s -> reg[edx]);
  printf("ebx:0x%08X\n", s -> reg[ebx]);
  printf("esp:0x%08X\n", s -> reg[esp]);
  printf("ebp:0x%08X\n", s -> reg[ebp]);
  printf("esi:0x%08X\n", s -> reg[esi]);
  printf("edi:0x%08X\n", s -> reg[edi]);
  return;
}

//set registers to zero
void set_reg(everything * t){
  int i;
  for (i = 0; i < 8; i++){
    t -> reg[i] = 0;
  }
}


void rrmovl(everything * t){
    int ifun = (int)(t->mem[t->pc + 1]-'0');
    int zf = t->cf.cz;
    int sf = t->cf.cs;
    int of = t->cf.co;
    int flag = 0;
    switch(ifun){
        case 1:
            if ((sf ^ of) | zf){
                flag = 1;
            }
            break;
        case 2:
            if (sf ^ of){
                flag = 1;
            }
            break;
        case 3:
            if (zf){
                flag = 1;
            }
            break;
        case 4:
            if (!zf){
                flag = 1;
            }
            break;
        case 5:
            if (!(sf ^ of)){
                flag = 1;
            }
            break;
        case 6:
            if (!(sf ^ of) & !zf){
                flag = 1;
            }
            break;
        case 0:
            flag = 1;
            break;
        default:
            t -> stat = 3; //or 4? check...
    }
    if (flag == 1){
        int rA = (int)(t->mem[t->pc+2]-'0');
        int rB = (int)(t->mem[t->pc+3]-'0');
        t->reg[rB]= t->reg[rA];
    }
    t->pc += 4;
}

void irmovl(everything * t){

  char a[8];
  a[0] = t -> mem[t -> pc + 10];
  a[1] = t -> mem[t -> pc + 11];
  a[2] = t -> mem[t -> pc + 8];
  a[3] = t -> mem[t -> pc + 9];
  a[4] = t -> mem[t -> pc + 6];
  a[5] = t -> mem[t -> pc + 7];
  a[6] = t -> mem[t -> pc + 4];
  a[7] = t -> mem[t -> pc + 5];
  char *ptr;
  int x = strtol(a, &ptr, 16);

  int rB = (int)(t->mem[t -> pc + 3] - '0');
  t -> reg[rB] = x;
  t -> pc += 12;

}

void rmmovl(everything * t) {

  int rA = (int)(t->mem[t->pc+2] - '0');
  int rB = (int)(t->mem[t->pc+3] - '0');

  char a[8];
  a[0] = t -> mem[t -> pc + 10];
  a[1] = t -> mem[t -> pc + 11];
  a[2] = t -> mem[t -> pc + 8];
  a[3] = t -> mem[t -> pc + 9];
  a[4] = t -> mem[t -> pc + 6];
  a[5] = t -> mem[t -> pc + 7];
  a[6] = t -> mem[t -> pc + 4];
  a[7] = t -> mem[t -> pc + 5];
  char *ptr;
  int x = strtol(a, &ptr, 16);
  int adr = t -> reg[rB] + x;
  t -> mem[adr] = ((t -> reg[rA])) & 0xFF;
  t -> mem[adr+1] = ((t -> reg[rA]) >> 8) & 0xFF;
  t -> mem[adr+2] = ((t -> reg[rA]) >> 16) & 0xFF;
  t -> mem[adr+3] = ((t -> reg[rA]) >> 24) & 0xFF;

  t -> pc += 12;
  // int b = t -> reg[rA] & 0xFF;
  // int mask = b << 24;
  // t -> mem[adr] = mask;
  // b = t -> reg[rA] & 0xFF00;
  // mask = b << 16;
  // t -> mem[adr + 1] = mask;
  // b = t -> reg[rA] & 0xFF0000;
  // mask = b << 8;
  // t -> mem[adr + 2] = mask;
  // b = t -> reg[rA] & 0xFF000000;
  // mask = b;
  // t -> mem[adr + 2] = mask;


}

void mrmovl(everything * t){
  int rA = (int)(t->mem[t->pc+2] - '0');
  int rB = (int)(t->mem[t->pc+3] - '0');
   char a[8];
  a[0] = t -> mem[t -> pc + 10];
  a[1] = t -> mem[t -> pc + 11];
  a[2] = t -> mem[t -> pc + 8];
  a[3] = t -> mem[t -> pc + 9];
  a[4] = t -> mem[t -> pc + 6];
  a[5] = t -> mem[t -> pc + 7];
  a[6] = t -> mem[t -> pc + 4];
  a[7] = t -> mem[t -> pc + 5];
  char *ptr;
  int x = strtol(a, &ptr, 16);
  int adr = t -> reg[rA] + x;

  int content = 0;
  int mask = 0;

  mask = (t -> mem[adr + 3]) & 0xFF;
  content += mask << 24;
  mask = (t -> mem[adr + 2]) & 0xFF;
  content += mask << 16;
  mask = (t -> mem[adr + 1]) & 0xFF;
  content += mask << 8;
  mask = (t -> mem[adr]) & 0xFF;
  content += mask;

  t -> reg[rB] = content;

  t->pc += 12;
}

void addl(everything * t){

  int rA = (int)(t->mem[t->pc+2] - '0');
  int rB = (int)(t->mem[t->pc+3] - '0');

  t -> reg[rB] = t -> reg[rA] + t -> reg[rB];
  int result = t -> reg[rB];
  t -> pc += 4;

  // if ((rA >= 8) || (rB >= 8)){
  //   t -> stat = 3;
  //   return;
  // }

  if (result == 0) {
    t -> cf.cz = 1;
  }
  if (result < 0) {
    t -> cf.cs = 1;
  }
  if ((rA < 0 == rB < 0) && (result < 0 != rA < 0)) {
    t -> cf.co = 1;
  }

}

void subl(everything * t){

  int rA = (int)(t->mem[t->pc+2] - '0');
  int rB = (int)(t->mem[t->pc+3] - '0');

  t -> reg[rB] = t -> reg[rB] - t -> reg[rA];
  int result = t -> reg[rB];

  t -> pc += 4;

  // if ((val1 >= 8) || (val2 >= 8)){
  //   t->stat = 3;
  //   return;
  // }

  if (result == 0) {
    t -> cf.cz = 1;
  }
  if (result < 0) {
    t -> cf.cs = 1;
  }
  if ((rA < 0 == rB < 0) && (result < 0 != rA < 0)) {
    t -> cf.co = 1;
  }

}

void andl(everything * t){

  int rA = (int)(t->mem[t->pc+2] - '0');
  int rB = (int)(t->mem[t->pc+3] - '0');

  t -> reg[rB] = t -> reg[rB] & t -> reg[rA];
  int result = t -> reg[rB];

  t -> pc += 4;

  // if ((val1 >= 8) || (val2 >= 8)){
  //   t->stat = 3;
  //   return;
  // }

//deal with co

  if (result == 0) {
    t -> cf.cz = 1;
  }
  if (result < 0) {
    t -> cf.cs = 1;
  }
  if ((rA < 0 == rB < 0) && (result < 0 != rA < 0)) {
    t -> cf.co = 1;
  }

}

void xorl(everything * t){

  int rA = (int)(t->mem[t->pc+2] - '0');
  int rB = (int)(t->mem[t->pc+3] - '0');

  t -> reg[rB] = t -> reg[rB] ^ t -> reg[rA];
  int result = t -> reg[rB];

  t -> pc += 4;

  // if ((val1 >= 8) || (val2 >= 8)){
  //   t->stat = 3;
  //   return;
  // }

  if (result == 0) {
    t -> cf.cz = 1;
  }
  if (result < 0) {
    t -> cf.cs = 1;
  }
  if ((rA < 0 == rB < 0) && (result < 0 != rA < 0)) {
    t -> cf.co = 1;
  }
}

void OPl(everything * t){

  switch(t -> mem[t -> pc + 1]){
    case '0': addl(t); break;
    case '1': subl(t); break;
    case '2': andl(t); break;
    case '3': xorl(t); break;
    default:
      t -> stat = 3; //or 4?
  }

}

void jumps(everything * t){
    int zf = t -> cf.cz;
    int sf = t -> cf.cs;
    int of = t -> cf.co;

    int flag = 0;
    int ifun = (int)(t -> mem[t -> pc + 1] - '0');

    switch(ifun){
        case 0x0:
            flag = 1;
            break;
        case 0x1:
            if((sf ^ of)|zf){
                flag = 1;
            }
            break;
        case 0x2:
            if(sf ^ of){
                flag = 1;
            }
            break;
        case 0x3:
            if(zf){
                flag = 1;
            }
            break;
        case 0x4:
            if(!zf){
                flag = 1;
            }
            break;
        case 0x5:
            if(!(sf^zf)){
                flag = 1;
            }
            break;
        case 0x6:
            if(!(sf^zf)&(~zf)){
                flag = 1;
            }
            break;
        default:
            t->stat = 3;
            break;

    }

    if(flag){
      char a[8];
      a[0] = t -> mem[t -> pc + 8];
      a[1] = t -> mem[t -> pc + 9];
      a[2] = t -> mem[t -> pc + 6];
      a[3] = t -> mem[t -> pc + 7];
      a[4] = t -> mem[t -> pc + 4];
      a[5] = t -> mem[t -> pc + 5];
      a[6] = t -> mem[t -> pc + 2];
      a[7] = t -> mem[t -> pc + 3];
      char *ptr;
      int x = strtol(a, &ptr, 16);

      t -> pc = x;

    }
    return;
}

void call(everything * t){
  char a[8];
      a[0] = t -> mem[t -> pc + 8];
      a[1] = t -> mem[t -> pc + 9];
      a[2] = t -> mem[t -> pc + 6];
      a[3] = t -> mem[t -> pc + 7];
      a[4] = t -> mem[t -> pc + 4];
      a[5] = t -> mem[t -> pc + 5];
      a[6] = t -> mem[t -> pc + 2];
      a[7] = t -> mem[t -> pc + 3];
      char *ptr;
      int x = strtol(a, &ptr, 16);

      t -> pc = x;
}

void ret(everything * t){
  int adr = t -> reg[ebp];
  int content = 0;
  int mask = 0;

  mask = (t -> mem[adr + 4]) & 0xFF;
  content += mask << 24;
  mask = (t -> mem[adr + 3]) & 0xFF;
  content += mask << 16;
  mask = (t -> mem[adr + 2]) & 0xFF;
  content += mask << 8;
  mask = (t -> mem[adr + 1]) & 0xFF;
  content += mask;

  t -> reg[eax] = content;

  int i;
  for (i=0; i<4; i++){
    t -> mem[t -> reg[ebp + i]] = 0;
  }

  t -> reg[ebp] += 4;
  t->pc += 4;
}

void pushl(everything * t){
  int rA = (int)(t->mem[t->pc+2] - '0');
  t -> reg[ebp] -= 4;

  int adr = t -> reg[ebp];
  t -> mem[adr+1] = ((t -> reg[rA])) & 0xFF;
  t -> mem[adr+2] = ((t -> reg[rA]) >> 8) & 0xFF;
  t -> mem[adr+3] = ((t -> reg[rA]) >> 16) & 0xFF;
  t -> mem[adr+4] = ((t -> reg[rA]) >> 24) & 0xFF;

  t -> pc += 4;

}

void popl(everything * t){

  int rA = (int)(t->mem[t->pc+2] - '0');
  int adr = t -> reg[ebp];
  int content = 0;
  int mask = 0;

  mask = (t -> mem[adr + 4]) & 0xFF;
  content += mask << 24;
  mask = (t -> mem[adr + 3]) & 0xFF;
  content += mask << 16;
  mask = (t -> mem[adr + 2]) & 0xFF;
  content += mask << 8;
  mask = (t -> mem[adr + 1]) & 0xFF;
  content += mask;

  t -> reg[rA] = content;

  int i;
  for (i=0; i<4; i++){
    t -> mem[t -> reg[ebp + i]] = 0;
  }

  t -> reg[ebp] += 4;
  t->pc += 4;
}

int main(int argc, char * argv[]){

  //our main object
  everything s;
  //pointer to that object
  everything * ps = &s;

  //status is 1, or AOK
  s.stat = 1;

  //program counter is at zero to start
  s.pc = 0;

  //condition flags are all set to zero to start
  s.cf.cz = s.cf.cs = s.cf.co = 0;

  int steps = 0;
  char icode;

  FILE *fp;
  char c;
  fp = fopen("file.yo", "r");

  int i = 0;
  while ((c = fgetc(fp)) != EOF) {
    s.mem[i] = c;
    //printf("%c", s.mem[i]);
    i++;
  }

  set_reg(ps);
  s.reg[esp] = SIZE-4;
  s.reg[ebp] = SIZE-4;


  while (s.stat == 1){
    icode = s.mem[s.pc];
    //icode = s.mem[s.pc];
    switch(icode){
      case '0': //halt, DONE
        s.stat = 2;
        break;
      case '1': //no-op, DONE
        s.pc += 2;
        break;
      case '2': //rrmovl, insert pointer for s (DONE)
        rrmovl(ps);
        break;
      case '3': //irmovl, insert pointer for s (DONE)
        irmovl(ps);
        break;
      case '4': //rmmovl, insert pointer for s
        rmmovl(ps);
        break;
      case '5': //mrmovl, insert pointer for s
        mrmovl(ps);
        break;
      case '6': //add (DONE), subtract (DONE), andl (DONE), xorl(DONE)
        OPl(ps);
        break;
      case '7': //will include all jumps, insert pointer for s (DONE)
        jumps(ps);
        break;
      case '8': //call, insert pointer for s
        call(ps);
        break;
      case '9': //return, insert pointer for s
        ret(ps);
        break;
      case 'a': //push, insert pointer for s
        pushl(ps);
        break;
      case 'b': //pop, insert pointer for s
        popl(ps);
        break;
      default:
        s.stat = 4;
        break;
    }

    steps++;

    // if (!(s.mem[s.pc])){
    //   s.stat = 3; //or 4
    // }

  }

  printOutput(&s, steps);

  return 0;


}




