#include "testlib.h"
#include "cpu.h"
#include "inst_decode.h"

void cpu_info(const CPU &cpu) {
  printf("CPU INFO:\n");
  printf("A:%x\tF:%x\tAF:%x\n", cpu.a, cpu.f, cpu.AF());
  printf("B:%x\tC:%x\tBC:%x\n", cpu.b, cpu.c, cpu.BC());
  printf("D:%x\tE:%x\tDE:%x\n", cpu.d, cpu.e, cpu.DE());
  printf("H:%x\tL:%x\tHL:%x\n", cpu.h, cpu.l, cpu.HL());
  printf("\nSP:%x\n", cpu.SP());

  for(int i=0;i<4;i++) {
    if(cpu.SP()-2 + i > 0xffff) break;
    printf("(%x)->\t%x\n", cpu.SP()-2+i, cpu.mem.read8(cpu.SP()-2+i));
  }
}

void test_add() {
  TEST_CATEGORY("testing add");
   //page 92
  {
    CPU cpu;
    cpu.a = 0x3A;
    cpu.b = 0xc6;
    Instruction instr;
    instr.operation = Instruction::OP_ADDAR;
    instr.op2 = REG_b;

    cpu.execute(instr);
    TEST_EQ(cpu.a, 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 1);
  }

  {
    CPU cpu;
    cpu.a = 0x3c;
    Instruction instr;
    instr.operation = Instruction::OP_ADDAN;
    instr.immediate = 0xff;
    cpu.execute(instr);

    TEST_EQ(cpu.a, 0x3b);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 1);
  }

  { 
    CPU cpu;
    cpu.a = 0x3c;
    cpu.setHL(0xbad0);
    cpu.mem.write8(0xbad0, 0x12);
    Instruction instr;
    instr.operation = Instruction::OP_ADDAHL;

    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x4e);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
    
}

void test_adc() {
  TEST_CATEGORY("testing adc");
  { // page 92 of manual
    CPU cpu;
    cpu.a = 0xe1;
    cpu.e = 0x0f;
    cpu.h = 0x00;
    cpu.l = 0x00;
    cpu.mem.write8(0, 0x0f);
    cpu.setCY(1);

    // ADC A, E
    Instruction instr;
    instr.operation = Instruction::OP_ADCAR;
    instr.op2 = REG_e;
    cpu.execute(instr);

    TEST_EQ(cpu.mem.read8(0), 0x0f);
    TEST_EQ(cpu.a, 0xf1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.CY(), 0);
  }

  { //ADC A, E, where a = 0xe1, e = 0x0f. cy=1
    CPU cpu;
    cpu.a = 0xe1;
    cpu.e = 0x0f;
    cpu.setCY(1);
    Instruction instr;
    instr.operation = Instruction::OP_ADCAR;
    instr.op2 = REG_e;

    cpu.execute(instr);
    TEST_EQ(cpu.a, 0xf1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.CY(), 0);
  }
}

void test_sub() {
  TEST_CATEGORY("test sub");
  // a=0x3e, e=0x3e, (HL) = 0x40
  { // SUB E
    CPU cpu;
    cpu.a = 0x3e;
    cpu.e = 0x3e;
    Instruction instr;
    instr.operation = Instruction::OP_SUBR;
    instr.op2 = REG_e;

    cpu.execute(instr);
    TEST_EQ(cpu.a, 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 0);
  }

  { // SUB 0x0f
    CPU cpu;
    cpu.a = 0x3e;
    Instruction instr;
    instr.operation = Instruction::OP_SUBN;
    instr.immediate = 0x0f;

    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x2f);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 0);
  }

  { // SUB (HL)
    CPU cpu;
    cpu.a = 0x3e;
    cpu.setHL(0xdead);
    cpu.mem.write8(0xdead, 0x40);

    Instruction instr;
    instr.operation = Instruction::OP_SUBHL;
    instr.immediate = 0x0f;

    cpu.execute(instr);
    TEST_EQ(cpu.a, 0xfe);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 1);
  }
}

void test_sbc() {
  //a = 0x3b, (HL) = 0x4f, h = 0x2a, cy = 1
  TEST_CATEGORY("test sbc");
  { // sbc a, h
    CPU cpu;
    cpu.a = 0x3b;
    cpu.h = 0x2a;
    cpu.setCY(1);
    Instruction instr;
    instr.operation = Instruction::OP_SBCAR;
    instr.op2 = REG_h;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x10);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 0);
  }

  { // sbc a, 0x3a
    CPU cpu;
    cpu.a = 0x3b;
    cpu.setCY(1);
    Instruction instr;
    instr.operation = Instruction::OP_SBCAN;
    instr.immediate = 0x3a;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x00);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 0);
  }

  { // sbc a, (HL)
    CPU cpu;
    cpu.a = 0x3b;
    cpu.setCY(1);
    cpu.setHL(0xdada);
    cpu.mem.write8(0xdada, 0x4f);
    Instruction instr;
    instr.operation = Instruction::OP_SBCAHL;
    instr.immediate = 0x3a;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0xeb);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 1);
  }
}

void test_and() {
  TEST_CATEGORY("test and");
  // a = 0x5a, l = 0x3f, (HL) = 0x00
  { // and l
    CPU cpu;
    cpu.a = 0x5a;
    cpu.l = 0x3f;
    Instruction instr;
    instr.operation = Instruction::OP_ANDR;
    instr.op2 = REG_l;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x1a);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
  { // and 0x38
    CPU cpu;
    cpu.a = 0x5a;
    Instruction instr;
    instr.operation = Instruction::OP_ANDN;
    instr.immediate = 0x38;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x18);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
  { // and (HL)
    CPU cpu;
    cpu.a = 0x5a;
    cpu.setHL(0xfeed);
    cpu.mem.write8(0xfeed, 0x00);
    Instruction instr;
    instr.operation = Instruction::OP_ANDHL;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
}

void test_or() {
  TEST_CATEGORY("test or");
  // a=0x5a, (hl)=0x0f
  { // or A
    CPU cpu;
    cpu.a = 0x5a;
    Instruction instr;
    instr.operation = Instruction::OP_ORR;
    instr.op2 = REG_a;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x5a);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
  { // or 3
    CPU cpu;
    cpu.a = 0x5a;
    Instruction instr;
    instr.operation = Instruction::OP_ORN;
    instr.immediate = 0x3;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x5b);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
  { // or (HL)
    CPU cpu;
    cpu.a = 0x5a;
    cpu.setHL(0xfeed);
    cpu.mem.write8(0xfeed, 0x0f);
    Instruction instr;
    instr.operation = Instruction::OP_ORHL;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x5f);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
}
void test_xor() {
  TEST_CATEGORY("test xor");
  // a=0xff, (hl)=0x8a
  { // xor A
    CPU cpu;
    cpu.a = 0xff;
    Instruction instr;
    instr.operation = Instruction::OP_XORR;
    instr.op2 = REG_a;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x0);
    TEST_EQ(cpu.Z(), 1);
  }
  { // xor 0xf
    CPU cpu;
    cpu.a = 0xff;
    Instruction instr;
    instr.operation = Instruction::OP_XORN;
    instr.immediate = 0xf;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0xf0);
    TEST_EQ(cpu.Z(), 0);
  }
  { // xor (HL)
    CPU cpu;
    cpu.a = 0xff;
    cpu.setHL(0xfeed);
    cpu.mem.write8(0xfeed, 0x8a);
    Instruction instr;
    instr.operation = Instruction::OP_XORHL;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0x75);
    TEST_EQ(cpu.Z(), 0);
  }
}

void test_cp() {
  TEST_CATEGORY("test cp");
  // a=0x3c, b=0x2f, (hl)=0x40
  { // cp b
    CPU cpu;
    cpu.a = 0x3c;
    cpu.b = 0x2f;
    Instruction instr;
    instr.operation = Instruction::OP_CPR;
    instr.op2 = REG_b;
    cpu.execute(instr);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 0);
  }
  { // cp 0x3c
    CPU cpu;
    cpu.a = 0x3c;
    Instruction instr;
    instr.operation = Instruction::OP_CPN;
    instr.immediate = 0x3c;
    cpu.execute(instr);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 1);
    TEST_EQ(cpu.CY(), 0);
  }
  { // cp (HL)
    CPU cpu;
    cpu.a = 0x3c;
    cpu.setHL(0xfeed);
    cpu.mem.write8(0xfeed, 0x40);
    Instruction instr;
    instr.operation = Instruction::OP_CPHL;
    cpu.execute(instr);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
}

void test_inc() {
  TEST_CATEGORY("test inc");
  {
    CPU cpu;
    cpu.a = 0xff;
    Instruction instr;
    instr.operation = Instruction::OP_INCR;
    instr.op1 = REG_a;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
  }
  {
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_INCHL;
    cpu.setHL(0xd00d);
    cpu.mem.write8(0xd00d, 0x50);
    cpu.execute(instr);
    TEST_EQ(cpu.mem.read8(0xd00d), 0x51);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }
}

void test_dec() {
  TEST_CATEGORY("test dec");
  {
    CPU cpu;
    cpu.a = 0x01;
    Instruction instr;
    instr.operation = Instruction::OP_DECR;
    instr.op1 = REG_a;
    cpu.execute(instr);
    TEST_EQ(cpu.a, 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 1);
  }
  {
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_DECHL;
    cpu.setHL(0xd00d);
    cpu.mem.write8(0xd00d, 0x00);
    cpu.execute(instr);
    TEST_EQ(cpu.mem.read8(0xd00d), 0xFF);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 1);
  }
}

void test_pushpop() {
  { // push bc
    TEST_CATEGORY("test pushpop");
    CPU cpu;
    Instruction instr;
    cpu.setSP(0xfffc);
    cpu.setBC(0xdead);
    instr.operation = Instruction::OP_PUSHQQ;
    instr.op1 = 0b000;  //bc
    cpu.execute(instr);

    instr.operation = Instruction::OP_POPQQ;
    instr.op1 = 0b010;  //de
    cpu.execute(instr);

    TEST_EQ(cpu.DE(), 0xdead);
  }

  { //pop bc
    TEST_CATEGORY("test popqq");
    CPU cpu;
    Instruction instr;
    cpu.setSP(0xfffc);
    cpu.mem.write8(0xfffc, 0x5f);
    cpu.mem.write8(0xfffd, 0x3c);
    instr.operation = Instruction::OP_POPQQ;
    instr.op1 = 0b000; //BC
    cpu.execute(instr);

    TEST_EQ(cpu.mem.read8(0xfffc), 0x5f);
    TEST_EQ(cpu.b, 0x3c);
    TEST_EQ(cpu.c, 0x5f);
    TEST_EQ(cpu.SP(), 0xfffe);
  }
}

void test_add16() {
  TEST_CATEGORY("test add 16bit");
  { //add hl, sp
    CPU cpu;
    Instruction instr;
    cpu.setHL(0x8a23);
    cpu.setSP(0x0605);
    instr.operation = Instruction::OP_ADDHLSS;
    instr.op1 = 0b110; 
    cpu.execute(instr);

    TEST_EQ(cpu.HL(), 0x9028);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
  { //add hl, hl
    CPU cpu;
    Instruction instr;
    cpu.setHL(0x8a23);
    instr.operation = Instruction::OP_ADDHLSS;
    instr.op1 = 0b101; 
    cpu.execute(instr);

    TEST_EQ(cpu.HL(), 0x1446);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 1);
  }

  { // add SP, e
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_ADDSPE;
    instr.immediate = 0x02;
    cpu.setSP(0xfff8);
    cpu.execute(instr);

    TEST_EQ(cpu.SP(), 0xfffa);
    TEST_EQ(cpu.CY(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.Z(), 0);
  }
}

void test_incdecs() {
  TEST_CATEGORY("inc / dec ss");
  { // inc DE
    CPU cpu;
    Instruction instr;
    instr.op1 = 0b10;  // DE
    instr.operation = Instruction::OP_INCSS;
    cpu.setDE(0x235F);
    cpu.execute(instr);

    TEST_EQ(cpu.DE(), 0x2360);

    instr.operation = Instruction::OP_DECSS;
    cpu.execute(instr);
    
    TEST_EQ(cpu.DE(), 0x235F);
  }
}

void test_rotates() {
  TEST_CATEGORY("testing rotates");
  { // RLCA
    TEST_CATEGORY("rlca");
    CPU cpu;
    Instruction instr;
    cpu.a = 0x85;
    cpu.setCY(0);
    instr.operation = Instruction::OP_RLCA;
    cpu.execute(instr);

    TEST_EQ(cpu.a, 0x0b);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  { // RLA
    TEST_CATEGORY("rla");
    CPU cpu;
    Instruction instr;
    cpu.a = 0x95;
    cpu.setCY(1);
    instr.operation = Instruction::OP_RLA;
    cpu.execute(instr);

    TEST_EQ(cpu.a, 0x2b);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  { // RRCA
    TEST_CATEGORY("rra");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RRCA;
    cpu.a = 0x3b;
    cpu.setCY(0);
    cpu.execute(instr);

    TEST_EQ(cpu.a, 0x9d);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  { // RRA
    TEST_CATEGORY("rra");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RRA;
    cpu.a = 0x81;
    cpu.setCY(0);
    cpu.execute(instr);

    TEST_EQ(cpu.a, 0x40);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  { 
    TEST_CATEGORY("rlcr");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RLCR;
    instr.immediate = REG_b;
    cpu.b = 0x85;
    cpu.setCY(0);
    cpu.execute(instr);

    TEST_EQ(cpu.b, 0x0b);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    TEST_CATEGORY("rlchl");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RLCHL;
    cpu.setHL(0xb0b0);
    cpu.mem.write8(0xb0b0, 0x00);
    cpu.execute(instr);

    TEST_EQ(cpu.mem.read8(cpu.HL()), 0x00);
    TEST_EQ(cpu.CY(), 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    TEST_CATEGORY("rlr");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RLR;
    instr.immediate = REG_l;
    cpu.l = 0x80;
    cpu.execute(instr);

    TEST_EQ(cpu.l, 0x00);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    TEST_CATEGORY("rlhl");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RLHL;
    cpu.setHL(0xbabe);
    cpu.mem.write8(0xbabe, 0x11);
    cpu.execute(instr);

    TEST_EQ(cpu.mem.read8(cpu.HL()), 0x22);
    TEST_EQ(cpu.CY(), 0);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    TEST_CATEGORY("rrcr");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RRCR;
    instr.immediate = REG_c;
    cpu.c = 0x1;
    cpu.execute(instr);

    TEST_EQ(cpu.c, 0x80);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    TEST_CATEGORY("rrchl");
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RRCHL;
    cpu.setHL(0xfeed);
    cpu.mem.write8(0xfeed, 0x00);
    cpu.setCY(0);
    cpu.execute(instr);

    TEST_EQ(cpu.mem.read8(cpu.HL()), 0);
    TEST_EQ(cpu.CY(), 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }
}

void test_srotate() {
  {
    CPU cpu;
    TEST_EQ(0x00, cpu.alu_sla(0x80));
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }
  {
    CPU cpu;
    TEST_EQ(0xFE, cpu.alu_sla(0xFF));
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    CPU cpu;
    TEST_EQ(0xc5, cpu.alu_sra(0x8a));
    TEST_EQ(cpu.CY(), 0);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }
  {
    CPU cpu;
    TEST_EQ(0x00, cpu.alu_sra(0x01));
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    CPU cpu;
    TEST_EQ(cpu.alu_srl(0x01), 0);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_srl(0xff), 0x7f);
    TEST_EQ(cpu.CY(), 1);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
  }

  {
    CPU cpu;
    TEST_EQ(cpu.alu_swap(0), 0);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_swap(0xf0), 0x0f);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 0);
    TEST_EQ(cpu.N(), 0);
    TEST_EQ(cpu.CY(), 0);
  }
}

void test_bitsetres() {
  {
    CPU cpu;
    cpu.alu_bit(0x80, 7);
    TEST_EQ(cpu.Z(), 0);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
  }
  {
    CPU cpu;
    cpu.alu_bit(0xef, 4);
    TEST_EQ(cpu.Z(), 1);
    TEST_EQ(cpu.H(), 1);
    TEST_EQ(cpu.N(), 0);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_set(0x80, 2), 0x84);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_set(0x3b, 7), 0xbb);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_set(0x0, 2), 0x4);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_res(0x80, 7), 0x0);
  }
  {
    CPU cpu;
    TEST_EQ(cpu.alu_res(0x3b, 1), 0x39);
  }
}

void test_callret() {
  {
    CPU cpu;
    cpu.pc = 0x8000;
    cpu.sp = 0xfffe;
    cpu.call(0x1234);
    TEST_EQ(cpu.pc, 0x1234);
    TEST_EQ(cpu.mem.read8(0xfffd), 0x80);
    TEST_EQ(cpu.mem.read8(0xfffc), 0x03);
    TEST_EQ(cpu.sp, 0xfffc);
    cpu.ret();
    TEST_EQ(cpu.pc, 0x8003);
    TEST_EQ(cpu.sp, 0xfffe);
  }

  {
    CPU cpu;
    Instruction instr;
    instr.operation = Instruction::OP_RST;
    instr.op1 = 4;
    cpu.execute(instr);
    TEST_EQ(cpu.pc, 0x20);
  }
}

void test_ret() {
}
int main() {
  test_add();
  test_adc();
  test_sub();
  test_sbc();
  test_and();
  test_or();
  test_xor();
  test_cp();
  test_inc();
  test_dec();
  test_pushpop();
  test_add16();
  test_incdecs();
  test_rotates();
  test_srotate();
  test_bitsetres();
  test_callret();
}
