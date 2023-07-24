#include <stdio.h>
#include "shell.h"

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint64_t u64;

typedef int32_t i32;
typedef int16_t i16;
typedef int64_t i64;


// Masking function
// MASK(N) - Creates N bits mask of 1s.
#define CAST(T, v) ((T)v)
#define MASK(N) (CAST(u32, -1) >> (32-N))
u32 GET_IMM_UNSIGNED_EXTEND(u32 instruction) {
  printf("=======GET_IMM_EXTEND==========\n");
  printf("instruction: %u\n", instruction);
  u32 unsign_imm = instruction & MASK(16);
  u8 is_negative = (unsign_imm >> 15) & 1;
  printf("unsign_imm %u\n", unsign_imm);
  printf("is_negative %u\n", is_negative);
  if (is_negative) {
      // printf("INSIDE THE CONDITION\n");
      // printf("MASK: %u", MASK(16) << 16);
      unsign_imm = unsign_imm | 0b11111111111111110000000000000000;
      // unsign_imm = unsign_imm | (MASK(16) << 16);
      // unsign_imm |= MASK(16) << 16;
      // unsign_imm = CAST(i32, unsign_imm);
  }
  printf("result: %u\n", unsign_imm);
  return unsign_imm;
  // printf("==============DONE==============\n");
}
// J
void process_jump(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  printf("=========JUMP==========\n");
  u32 target = instruction & MASK(26);
  printf("target: %u\n", target);
  target = target << 2;
  target |= (CURRENT_STATE.PC >> 28) << 28;
  CURRENT_STATE.PC = target - 4;
}

// ADDI
void process_addImmediate(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  printf("===========ADDI========\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  u32 unsign_imm = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_imm = CAST(i32, unsign_imm);
  printf("unsign_imm: %u\n", unsign_imm);
  printf("sign_imm: %i\n", sign_imm);
  NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + sign_imm;
  printf("result: %u\n", CURRENT_STATE.REGS[rs] + sign_imm);
  printf("==========DONE===========\n");
}

// ADDIU
void process_addImmediateUnsigned(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  printf("===========ADDIU========\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  u32 unsign_imm = GET_IMM_UNSIGNED_EXTEND(instruction);
  printf("result: %u\n", CURRENT_STATE.REGS[rs] + unsign_imm);
  NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + unsign_imm;
}

// SLTI
void process_SLTI(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  u32 is_negative = rs & 16;
  u32 rs_u32 = CAST(u32, rs);
  if (is_negative) {
      rs_u32 |= MASK(25) << 25;
  }
  i32 rs_i32 = CAST(i32, rs_u32);

  u32 unsign_imm = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_imm = CAST(i32, unsign_imm);

  if (rs_i32 < sign_imm) {
    NEXT_STATE.REGS[rt] = 1;
  } else {
    NEXT_STATE.REGS[rt] = 0;
  }
}

//SLTIU
void process_SLTIU(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  // u32 is_negative = rs & 16;
  u32 rs_u32 = CAST(u32, rs);
  // if (is_negative) {
  //     rs_u32 |= MASK(25) << 25;
  // }
  // i32 rs_i32 = CAST(i32, rs_u32);

  u32 unsign_imm = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_imm = CAST(i32, unsign_imm);

  if (rs_u32 < sign_imm) {
    NEXT_STATE.REGS[rt] = 1;
  } else {
    NEXT_STATE.REGS[rt] = 0;
  }
}

void process_ANDI(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  u32 unsign_imm = instruction & MASK(16);
  u32 data_in_rs = CURRENT_STATE.REGS[rs] & unsign_imm;
  NEXT_STATE.REGS[rt] = data_in_rs;
}

void process_ORI(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  u32 unsign_imm = instruction & MASK(16);
  u32 data_in_rs = CURRENT_STATE.REGS[rs] | unsign_imm;
  NEXT_STATE.REGS[rt] = data_in_rs;
}

void process_XORI(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  printf("=========XORI=============\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  u32 unsign_imm = instruction & MASK(16);
  u32 data_in_rs = CURRENT_STATE.REGS[rs] ^ unsign_imm;
  printf("result: %u\n", data_in_rs);
  NEXT_STATE.REGS[rt] = data_in_rs;
}

void process_LUI(u32 instruction, u8 opcode, u8 rs, u8 rt) {
  u32 unsign_imm = instruction & MASK(16);
  NEXT_STATE.REGS[rt] = unsign_imm << 16;
}

void process_LB(u32 instruction, u8 opcode, u8 base, u8 rt) {
   
  // u32 offset = (((instruction & MASK(16)) << 15) >> 15);
  // u32 vAddr = CURRENT_STATE.REGS[base] + offset;
  // u32 content = mem_read_32(vAddr) & 0b11111111;
  // printf("vAddr: %u\n", vAddr);
  // printf("content: %x\n",content);
  // NEXT_STATE.REGS[rt] = content;


  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + sign_offset;
  // get 8 bit and the rest is 0
  u32 content = mem_read_32(vAddr) & MASK(8);
  u32 is_negative = content >> 7;
  if (is_negative)
  {
    content |= MASK(24) << 8;
  }
  NEXT_STATE.REGS[rt] = content;
}


void process_LH(u32 instruction, u8 opcode, u8 base, u8 rt) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + sign_offset;
  // u32 halfword = vAddr & MASK(16);
  // get 16 bit and the rest is 0
  u32 content = mem_read_32(vAddr) & MASK(16);
  u32 is_negative = content >> 15;
  if (is_negative == 1)
  {
    content |= (MASK(16) << 16);
  }
  u8 least_sig = content & 1;
  // // 1 is true.
  // if (least_sig)
  // {
  //   printf("ERROR!\n");
  //   printf("LEAST SIG SHOULD NOT BE 1\n");
  //   return;
  // }
  
  NEXT_STATE.REGS[rt] = content;
}

void process_LW(u32 instruction, u8 opcode, u8 base, u8 rt) {
  printf("============LW============\n");
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  printf("offset: %u\n", offset + base);
  u32 vAddr = CURRENT_STATE.REGS[base] + (i32) offset;
  printf("vAddr: %u\n", vAddr);
  u32 word = vAddr;
  // get 32 bit and the rest is 0
  u32 content = mem_read_32(word);
  printf("content: %u\n", content);
  NEXT_STATE.REGS[rt] = content;
}

void process_LBU(u32 instruction, u8 opcode, u8 base, u8 rt) {
  printf("==========LBU=======\n");
  // u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  u32 offset = (((instruction & MASK(16)) << 15) >> 15);
  // i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + offset;
  // get 8 bit and the rest is 0
  
  // u32 content = mem_read_32(vAddr);
  u32 content = mem_read_32(vAddr) & 0b11111111;
  printf("vAddr: %u\n", vAddr);
  printf("content: %x\n",content);
  NEXT_STATE.REGS[rt] = content;
}

void process_LHU(u32 instruction, u8 opcode, u8 base, u8 rt) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + sign_offset;
  // u32 halfword = vAddr & MASK(16);
  // get 16 bit and the rest is 0
  u32 content = mem_read_32(vAddr) & MASK(16);
  
  // u8 least_sig = content & 1;
  // // 1 is true.
  // if (least_sig)
  // {
  //   printf("ERROR!\n");
  //   printf("LEAST SIG SHOULD NOT BE 1\n");
  //   return;
  // }
  
  NEXT_STATE.REGS[rt] = content;
}

void process_SB(u32 instruction, u8 opcode, u8 base, u8 rt) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + sign_offset;
  // u32 least_sig = (vAddr << 24) >> 24;
  mem_write_32(vAddr, CURRENT_STATE.REGS[rt] & MASK(8));
}

void process_SH(u32 instruction, u8 opcode, u8 base, u8 rt) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + sign_offset;
  // u32 least_sig = (vAddr << 16) >> 16;
  mem_write_32(vAddr, CURRENT_STATE.REGS[rt] & MASK(16));
}

void process_SW(u32 instruction, u8 opcode, u8 base, u8 rt) {
  printf("===========SW=============\n");
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 sign_offset = CAST(i32, offset);
  u32 vAddr = CURRENT_STATE.REGS[base] + sign_offset;
  u8 first_sig = vAddr & 1;
  u8 second_sig = vAddr & 2;

  printf("offset: %u\n", offset);
  printf("vAddr: %u\n", vAddr);
  // printf("")
  if (first_sig || second_sig) {
    printf("ERROR!\n");
    printf("EITER TWO LEAST SIG SHOULD NOT BE 1\n");
    return;
  }
  // u32 least_sig = (vAddr << 16) >> 16;
  mem_write_32(vAddr, CURRENT_STATE.REGS[rt]);
  // mem_write_32()
}

/*
///////////////////////////
///////////BRANCH//////////
//////////////////////////
*/
void process_BLTZ(u32 instruction, u8 opcode, u8 rs, u8 BLTZ) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 target = CAST(i32, offset);
  target = target << 2;
  if ((CURRENT_STATE.REGS[rs] >> 31) == 1)
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + target - 4;
  } 
}

void process_BGEZ(u32 instruction, u8 opcode, u8 rs, u8 BGEZ) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 target = CAST(i32, offset);
  target = target << 2;
  if ((CURRENT_STATE.REGS[rs] >> 31) == 0)
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + target - 4;
  } 
}

void process_BLTZAL(u32 instruction, u8 opcode, u8 rs, u8 BGEZ) {
  NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 target = CAST(i32, offset);
  target = target << 2;
  if ((CURRENT_STATE.REGS[rs] >> 31) == 1)
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + target - 4;
  } 
}

void process_BGEZAL(u32 instruction, u8 opcode, u8 rs, u8 BGEZ) {
  NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction);
  i32 target = CAST(i32, offset);
  target = target << 2;
  if ((CURRENT_STATE.REGS[rs] >> 31) == 0)
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + target - 4;
  } 
}

/*
////////////////////////////
///////////SPECIAL//////////
///////////////////////////
*/

void process_SLL(u32 instruction, u8 rd, u8 rt, u8 sa) {
  printf("=============SLL============\n");
  printf("rd: %u\n", rd);
  printf("rt: %u\n", rt);
  u32 content = CURRENT_STATE.REGS[rt] << sa;
  printf("reslut: %u\n", content);
  NEXT_STATE.REGS[rd] = content;
}
void process_SRL(u32 instruction, u8 rd, u8 rt, u8 sa) {
  printf("============SRL============\n");
  printf("rd: %u\n", rd);
  printf("rt: %u\n", rt);
  // current register is unsign then the  sign bit do not extend (not sign bit)
  u32 content = CURRENT_STATE.REGS[rt] >> sa;
  NEXT_STATE.REGS[rd] = content;
}

void process_SRA(u32 instruction, u8 rd, u8 rt, u8 sa) {

  u32 content = CURRENT_STATE.REGS[rt];
  i32 new_content = CAST(i32, content);
  new_content = new_content >> sa;
  NEXT_STATE.REGS[rd] = new_content;
}

void process_SLLV(u32 instruction, u8 rd, u8 rt, u8 rs) {
  u32 content = CURRENT_STATE.REGS[rt] << (0b11111 & CURRENT_STATE.REGS[rs]);
  NEXT_STATE.REGS[rd] = content;
}

void process_SRLV(u32 instruction, u8 rd, u8 rt, u8 rs) {
  u32 content = CURRENT_STATE.REGS[rt] >> (0b11111 & CURRENT_STATE.REGS[rs]);
  NEXT_STATE.REGS[rd] = content;
}

void process_SRAV(u32 instruction, u8 rd, u8 rt, u8 rs) {
  u32 content = CURRENT_STATE.REGS[rt];
  i32 new_content = CAST(i32, content);

  new_content = new_content >> (0b11111 & CURRENT_STATE.REGS[rs]);
  NEXT_STATE.REGS[rd] = new_content;
}

void process_JR(u32 instruction, u8 rs) {
  CURRENT_STATE.PC = CURRENT_STATE.REGS[rs] - 4;
}

void process_JALR(u32 instruction, u8 rs, u8 rd) {
  // CURRENT_STATE.REGS[rs]
  u32 temp = CURRENT_STATE.REGS[rs];
  NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
  CURRENT_STATE.PC = temp;
}

void process_ADD(u32 instruction, u8 rs, u8 rt, u8 rd) {
  printf("===========ADD========\n");
  printf("result: %i\n", CAST(i32, CURRENT_STATE.REGS[rs]) + CAST(i32, CURRENT_STATE.REGS[rt]));
  NEXT_STATE.REGS[rd] = CAST(i32, CURRENT_STATE.REGS[rs]) + CAST(i32, CURRENT_STATE.REGS[rt]);
}

void process_ADDU(u32 instruction, u8 rs, u8 rt, u8 rd) {
  printf("=======ADDU==========\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  printf("result: %u\n", CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt]);
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
}

void process_SUB(u32 instruction, u8 rs, u8 rt, u8 rd) {
  // printf("===========SUB========\n");
  // printf("rs: %u\n", rs);
  // printf("rt: %u\n", rt);
  // printf("rs_value: %u\n", CURRENT_STATE.REGS[rs]);
  // printf("rt_value: %u\n", CURRENT_STATE.REGS[rt]);

  NEXT_STATE.REGS[rd] = CAST(i32, CURRENT_STATE.REGS[rs]) - CAST(i32, CURRENT_STATE.REGS[rt]);
}

void process_SUBU(u32 instruction, u8 rs, u8 rt, u8 rd) {
  printf("===========SUBU===========\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  printf("reslut: %u", CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt]);
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
}

void process_AND(u32 instruction, u8 rs, u8 rt, u8 rd) {
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
}

void process_OR(u32 instruction, u8 rs, u8 rt, u8 rd) {
  printf("==========OR===========\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  printf("result: %u\n",CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
}

void process_XOR(u32 instruction, u8 rs, u8 rt, u8 rd) {
  printf("=========XOR===========\n");
  printf("rs: %u\n", rs);
  printf("rt: %u\n", rt);
  printf("result: %u\n",CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt]);
  NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
}

void process_NOR(u32 instruction, u8 rs, u8 rt, u8 rd) {
  NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
}

void process_SLT(u32 instruction, u8 rs, u8 rt, u8 rd) {
  if (CAST(i32, CURRENT_STATE.REGS[rs]) < CAST(i32, CURRENT_STATE.REGS[rt])) {
    NEXT_STATE.REGS[rd] = 1;
  } else {
    NEXT_STATE.REGS[rd] = 0;
  }
}

void process_SLTU(u32 instruction, u8 rs, u8 rt, u8 rd) {
  if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) {
    NEXT_STATE.REGS[rd] = 1;
  } else {
    NEXT_STATE.REGS[rd] = 0;
  }
}

void process_MULT(u32 instruction, u8 rs, u8 rt) {
  i32 rs_value = CAST(i32, CURRENT_STATE.REGS[rs]);
  i32 rt_value = CAST(i32, CURRENT_STATE.REGS[rt]);
  i64 result = CAST(i64, rs_value * rt_value);

  u32 low  = result & MASK(32);
  u32 high = (result >> 32) & MASK(32);

  NEXT_STATE.LO = low;
  NEXT_STATE.HI = high;

}

void process_MFHI(u32 instruction, u8 rd) {
  NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
}

void process_MFLO(u32 instruction, u8 rd) {
  NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
}

void process_MTHI(u32 instruction, u8 rs) {
  NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
}

void process_MTLO(u32 instruction, u8 rs) {
  NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
}

void process_MULTU(u32 instruction, u8 rs, u8 rt) {
  u64 result = CAST(u64, CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]);

  u32 low  = result & MASK(32);
  u32 high = (result >> 32) & MASK(32);

  NEXT_STATE.LO = low;
  NEXT_STATE.HI = high;
}

void process_DIV(u32 instruction, u8 rs, u8 rt) {
  i32 rs_value = CAST(i32, CURRENT_STATE.REGS[rs]);
  i32 rt_value = CAST(i32, CURRENT_STATE.REGS[rt]);
  i64 result = CAST(i64, rs_value / rt_value);

  u32 low  = result & MASK(32);
  u32 high = (result >> 32) & MASK(32);

  NEXT_STATE.LO = low;
  NEXT_STATE.HI = high;
}

void process_DIVU(u32 instruction, u8 rs, u8 rt) {
  u64 result = CAST(u64, CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt]);

  u32 low  = result & MASK(32);
  u32 high = (result >> 32) & MASK(32);

  NEXT_STATE.LO = low;
  NEXT_STATE.HI = high;
}

void process_SYSCALL(u32 instruction) {
  RUN_BIT = 0;
  // u32 code = (instruction << 6) & MASK(12);
}


////////////////////////////////
//////////First Row/////////////
////////////////////////////////

void process_JAL(u32 instruction) {
  //   u32 target = instruction & MASK(26);
  // printf("target: %u\n", target);
  // target = target << 2;
  // target |= (CURRENT_STATE.PC >> 28) << 28;
  // CURRENT_STATE.PC = target - 4;
  printf("=========JAL=============\n");

  u32 target = instruction & MASK(26);
  NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
  target = target << 2;
  printf("target: %u\n",target);
  target |= (CURRENT_STATE.PC >> 28) << 28;
  CURRENT_STATE.PC = target - 4;
}

void process_BEQ(u32 instruction, u8 rs, u8 rt) {
  printf("===========BEQ==========\n");
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction) << 2;
  if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + offset - 4;
  }
}

void process_BNE(u32 instruction, u8 rs, u8 rt) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction) << 2;
  if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + offset - 4;
  }
}

void process_BLEZ(u32 instruction, u8 rs) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction) << 2;
  if ((CURRENT_STATE.REGS[rs] == CAST(u32, 0)) || ((CURRENT_STATE.REGS[rs] >> 31) == 1))
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + offset - 4;
  }
}

void process_BGTZ(u32 instruction, u8 rs) {
  u32 offset = GET_IMM_UNSIGNED_EXTEND(instruction) << 2;
  if ((CURRENT_STATE.REGS[rs] != CAST(u32, 0)) || ((CURRENT_STATE.REGS[rs] >> 31) == 0))
  {
    CURRENT_STATE.PC = CURRENT_STATE.PC + offset - 4;
  }
}

enum OPCODE{
  J         = 2 , JAL       = 3 , BEQ       = 4 , BNE         = 5 , BLEZ      = 6 , BGTZ  = 7 ,
  ADDI      = 8 , ADDIU     = 9 , SLTI      = 10, SLTIU       = 11, ANDI      = 12, ORI   = 13,
  XORI      = 14, LUI       = 15, LB        = 32, LH          = 33, LW        = 35, LBU   = 36,
  LHU       = 37, SB        = 40, SH        = 41, SW          = 43,
  
  BRANCH    = 1 ,
  // BLTZ      = 0 , BGEZ          ,
  // BLTZAL        , BGEZAL        ,
  SPECIAL = 0,

  // SLL           , SRL           , SRA           , SLLV      ,
  // SRLV          , SRAV          , JR            , JALR          ,
  // ADD           , ADDU          ,SUB            , SUBU ,


  // AND           , OR            , XOR         , NOR               ,
  // SLT           , SLTU          , MULT          ,
  // MFHI            , MFLO          , MTHI      ,
  // MTLO          , MULTU         ,
  // DIV           , DIVU            ,
  // SYSCALL       ,
  
};

enum BRANCH {
  BLTZ      = 0 , BGEZ      = 1 ,
  BLTZAL    = 16, BGEZAL    = 17 ,
};

enum SPECIAL {
  SLL       = 0 , SRL       = 2 , SRA       = 3 , SLLV   = 4 ,
  SRLV      = 5 , SRAV      = 7 , JR        = 8 , JALR   = 9 ,
  ADD       = 32, ADDU      = 33, SUB       = 34, SUBU   = 35,
  AND       = 36, OR        = 37, XOR       = 38, NOR    = 39,
  SLT       = 42, SLTU      = 43, MULT      = 24, MFHI   = 16,
  MFLO      = 18, MTHI      = 17, MTLO      = 19, MULTU  = 25,
  DIV       = 26, DIVU      = 27, SYSCALL   = 12,   
};

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
  u32 instruction = mem_read_32(CURRENT_STATE.PC);
  printf("PC AT : %x\n", CURRENT_STATE.PC);
  u8 opcode = (instruction >> 26) & MASK(6);
  u8 rs = (instruction >> 21) & MASK(5);
  u8 rt = (instruction >> 16) & MASK(5);
  printf("%u\n", MASK(2));

  
  RUN_BIT = 1;
  enum OPCODE op;
  op = opcode;
  switch(op) {
    case(J):
    {
      process_jump(instruction, opcode, rs, rt);
      break;
    }
    case(JAL):
    {
      // JAL
      process_JAL(instruction);
      break;
    }
    case(BEQ):
    {
      process_BEQ(instruction, rs, rt);
      break;
    }
    case(BNE):
    {
      // BNE
      process_BNE(instruction, rs, rt);
      break;
    }
    case(BLEZ):
    {
      // BLEZ
      process_BLEZ(instruction, rs);
      break;
    }
    case(BGTZ):
    {
      // BGTZ
      process_BGTZ(instruction, rs);
      break;
    }
    case(ADDI):
    {
      // ADDI
      process_addImmediate(instruction, opcode, rs, rt);
      break;
    }
    case(ADDIU):
    {
      // ADDIU
      process_addImmediateUnsigned(instruction, opcode, rs, rt);
      break;
    }
  
    case(SLTI):
    {
      // SLTI
      process_SLTI(instruction, opcode, rs, rt);
      break;
    }
    case(SLTIU):
    {
      // SLTIU   
      process_SLTIU(instruction, opcode, rs, rt);
      break;
    }
    case(ANDI):
    {
      // ANDI
      process_ANDI(instruction, opcode, rs, rt);
      break;
    }
    case(ORI):
    {
      // ORI
      process_ORI(instruction, opcode, rs, rt);
      break;
    }
    case(XORI):
    {
      // XORI
      process_XORI(instruction, opcode, rs, rt);
      break;
    }
    case(LUI):
    {
      // LUI
      process_LUI(instruction, opcode, rs, rt);
      break;
    }
    case(LB):
    {
      process_LB(instruction, opcode, rs, rt);
      break;
    }
    case(LH):
    {
      process_LH(instruction, opcode, rs, rt);
      break;
    }
    case(LW): 
    {
      process_LW(instruction, opcode, rs, rt);
      break;
    }
    case(LBU):
    {
      process_LBU(instruction, opcode, rs, rt);
      break;
    }
    case(LHU):
    {
      process_LHU(instruction, opcode, rs, rt);
      break;
    }
    case(SB):
    {
      process_SB(instruction, opcode, rs, rt);
      break;
    }
    case(SH):
    {
      process_SH(instruction, opcode, rs, rt);
      break;
    }
    case(SW):
    {
      process_SW(instruction, opcode, rs, rt);
      break;
    }
    case(BRANCH):
    {
      enum BRANCH rts;
      rts = rt;
      switch (rts)
      {
        case(BLTZ):
        {
          process_BLTZ(instruction, opcode, rs, rt);
          break;
        }
        case(BGEZ):
        {
          process_BGEZ(instruction, opcode, rs, rt);
          break;
        }
        case(BLTZAL):
        {
          process_BLTZAL(instruction, opcode, rs, rt);
          break;
        }
        case(BGEZAL):
        {
          process_BGEZAL(instruction, opcode, rs, rt);
          break;
        }
      }
      break;
    }
    case(SPECIAL):
    {
      enum SPECIAL spec;
      printf("INSTRUCTION: %d\n", instruction);
      u8 sll = instruction & 0b111111;
      u8 sa = (instruction & 0b11111000000) >> 6;
      u8 rd = (instruction & 0b1111100000000000) >> 11;

      printf("SPECIAL\n");
      spec = sll;
      printf("%d\n", spec);
      switch (spec)
      {
        case (SLL):
        {
          process_SLL(instruction, rd, rt, sa);
          break;
        }
        case(SRL):
        {
          process_SRL(instruction, rd, rt, sa);
          break;
        }
        case(SRA):
        {
          process_SRA(instruction, rd, rt, sa);
          break;
        }
        case(SLLV):
        {
          process_SLLV(instruction, rd, rt, rs);
          break;
        }
        case(SRLV):
        {
          process_SRLV(instruction, rd, rt, rs);
          break;
        }
        case(SRAV):
        {
          process_SRAV(instruction, rd, rt, rs);
          break;
        }
        case(JR):
        {
          process_JR(instruction, rs);
          break;
        }
        case(JALR):
        {
          process_JALR(instruction, rs, rd);
          break;
        }
        



        case(ADD):
        {
          printf("ADD\n");
          process_ADD(instruction, rs, rt, rd);
          break;
        }
        case(ADDU):
        {
          process_ADDU(instruction, rs, rt, rd);
          break;
        }
        case(SUB):
        {
          process_SUB(instruction, rs, rt, rd);
          break;
        }
        case(SUBU):
        {
          process_SUBU(instruction, rs, rt, rd);
          break;
        }
        case(AND):
        {
          process_AND(instruction, rs, rt, rd);
          break;
        }
        case(OR):
        {
          process_OR(instruction, rs, rt, rd);
          break;
        }
        case(XOR):
        {
          process_XOR(instruction, rs, rt, rd);
          break;
        }
        case(NOR):
        {
          process_NOR(instruction, rs, rt, rd);
          break;
        }
        case(SLT):
        {
          process_SLT(instruction, rs, rt, rd);
          break;
        }
        case(SLTU):
        {
          process_SLTU(instruction, rs, rt, rd);
          break;
        }

        case(MULT):
        {
          process_MULT(instruction, rs, rt);
          break;
        }
        case(MFHI):
        {
          process_MFHI(instruction, rd);
          break;
        }
        case(MFLO):
        {
          process_MFLO(instruction, rd);
          break;
        }
        case(MTHI):
        {
          process_MTHI(instruction, rs);
          break;
        }
        case(MTLO):
        {
          process_MTLO(instruction, rs);
          break;
        }
        case(MULTU):
        {
          process_MULTU(instruction, rs, rt);
          break;
        }
        case(DIV):
        {
          process_DIV(instruction, rs, rt);
          break;
        }   
        case(DIVU):
        {
          process_DIVU(instruction, rs, rt);
          break;
        }      
        case(SYSCALL):
        {
          process_SYSCALL(instruction);
          break;
        }     
      }
      break;
    }


  }
  
  // Iterate to the next cycle
  NEXT_STATE.PC = CURRENT_STATE.PC  + 4;
}


// addi $t0, $zero, 5
// NEXT_STATE.REGS[$t0] = CURRENT_STATE[$zero] + 5;