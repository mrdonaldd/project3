#include "shell.h"
#include "utils.h"
#include <stdio.h>

#define CAT(x, s) x##s
#define CAT_HELPER(x, s) CAT(x, s)
#define PADDING CAT_HELPER(PAD, __COUNTER__)

#define BYTE 8
#define WORD 16
#define DWORD 32

static int jump_pending = -1;

/////////////////////////////////////
// NOTE(Appy): Opcodes automation
#define OPCODES(x)\
  x(SPECIAL)      \
  x(REGIMM)       \
  x(J)            \
  x(JAL)          \
  x(BEQ)          \
  x(BNE)          \
  x(BLEZ)         \
  x(BGTZ)         \
  x(ADDI)         \
  x(ADDIU)        \
  x(SLTI)         \
  x(SLTIU)        \
  x(ANDI)         \
  x(ORI)          \
  x(XORI)         \
  x(LUI)          \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(LB)           \
  x(LH)           \
  x(PADDING)      \
  x(LW)           \
  x(LBU)          \
  x(LHU)          \
  x(PADDING)      \
  x(PADDING)      \
  x(SB)           \
  x(SH)           \
  x(PADDING)      \
  x(SW)           \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING)      \
  x(PADDING) 

#define ENUMERATE(OP) OP,
enum EOPCODES { OPCODES(ENUMERATE) NUMBER_OF_OPS };

/////////////////////////////////////
// NOTE(Appy): Registers

#define R_V0 2
#define R_RA 31

/////////////////////////////////////
// NOTE(Appy): Cast

#define u32t(V) (cast(uint32_t, V))

/////////////////////////////////////
// NOTE(Appy): R-Type instructions

#define SLL u32t(0x00)
#define SRL u32t(0x02)
#define SRA u32t(0x03)
#define JR u32t(0x8)
#define JALR u32t(0b001001)
#define MTHI u32t(0x11)
#define MTLO u32t(0x13)
#define MULT u32t(0x18)
#define ADD u32t(0x20)
#define ADDU u32t(0x21)
#define SUB u32t(0x22)
#define AND u32t(0x24)
#define SUBU u32t(0x23)
#define OR u32t(0x25)
#define XOR u32t(0x26)
#define NOR u32t(0x27)
#define DIV u32t(0x1A)
#define DIVU u32t(0x1B)
#define SYSCALL u32t(0xC)
#define MFHI u32t(0b010000)
#define MFLO u32t(0b010010)
#define MULTU u32t(0b011001)
#define SLLV u32t(0b000100)
#define SLT u32t(0b101010)
#define SLTU u32t(0b101011)
#define SRAV u32t(0b000111)
#define SRLV u32t(0b000110)

/////////////////////////////////////
// NOTE(Appy): REGIMM ops
#define BLTZ cast(u8, 0b00000)
#define BLTZAL cast(u8, 0b10000)
#define BGEZ cast(u8, 0b00001)
#define BGEZAL cast(u8, 0b10001)

/////////////////////////////////////
// NOTE(Appy): Segment sizes

#define INSTR_SIZE 32
#define OP_SIZE 6
#define RS_SIZE 5
#define RT_SIZE 5
#define RD_SIZE 5
#define SA_SIZE 5
#define IM_SIZE 16
#define CD_SIZE 6
#define SHAMT_SIZE 5

/////////////////////////////////////
// NOTE(Appy): Segment positions
//
// RS    - Operand A in register file
// RT    - Operand B in register file
// RD    - Result destination in register file
// SHAMT - Shift amount
// CD    - Function code

#define OP_POS (INSTR_SIZE - OP_SIZE)
#define RS_POS (OP_POS - RS_SIZE)
#define RT_POS (RS_POS - RT_SIZE)
#define RD_POS (RT_POS - RD_SIZE)
#define SA_POS (RD_POS - SA_SIZE)
#define IM_POS (RT_POS - IM_SIZE)
#define CD_POS (0)
#define SHAMT_POS (6)

/////////////////////////////////////
// NOTE(Appy): Segment getters

#define GET_BLOCK(addr, start, size) ((addr >> (start)) & MASK(size))
#define GET(SEG, addr) GET_BLOCK(addr, SEG##_POS, SEG##_SIZE)

/////////////////////////////////////
// NOTE(Appy): Uninspiring helpers

#define PASS_DOWN(OP) case OP
#define HANDLE(OP)                                                             \
  case OP: {                                                                   \
    instr_##OP(mem);                                                           \
    break;                                                                     \
  }
#define HANDLER(OP) void instr_##OP(uint32_t mem)
#define CALL_HANDLER(OP) instr_##OP(mem)
#define FORWARD_HANDLER(OP, TO)                                                \
  HANDLER(OP) { CALL_HANDLER(TO); }
#define DISPATCH(code) goto *jumpTable[code];
#define NEXT goto *jumpTable[u32t(NUMBER_OF_OPS)]
#define JUMPTABLE static const void *jumpTable[]
#define MK_LBL(OP) &&lbl_##OP,
#define LBL(OP) lbl_##OP

/////////////////////////////////////
// NOTE(Appy): Handlers

HANDLER(REGIMM) {
  u8 op = GET_BLOCK(mem, 16, 5);
  switch (op) {
  case BLTZ: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, mem)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, mem)] >> 31);
    if (branch == 1) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  case BLTZAL: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, mem)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, mem)] >> 31);
    NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
    if (branch == 1) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  case BGEZ: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, mem)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, mem)] >> 31);
    if (branch == 0) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  case BGEZAL: {
    u32 addr = CURRENT_STATE.PC + (sign_extend_16(GET(IM, mem)) << 2);
    int branch = (CURRENT_STATE.REGS[GET(RS, mem)] >> 31);
    NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
    if (branch == 0) {
      CURRENT_STATE.PC = addr - 4;
    }
    break;
  }
  default:
    break;
  }
}

HANDLER(SPECIAL) {
  uint8_t code = GET(CD, mem);

/* Undefine register helpers from process instruction */
#undef RS
#undef RT
#undef IMM

/* Register helpers */
#define RD (NEXT_STATE.REGS[GET(RD, mem)])
#define RS (CURRENT_STATE.REGS[GET(RS, mem)])
#define RT (CURRENT_STATE.REGS[GET(RT, mem)])
#define SA (GET(SA, mem))

  switch (code) {
  case SYSCALL: // System call
  {

    if (CURRENT_STATE.REGS[R_V0] == 0x0A) {
      RUN_BIT = FALSE;
    }
    /* else do nothing... increment as usual */
    break;
  }
  case ADDU:
  case ADD: {
    RD = RS + RT;
    break;
  } // Addition
  case SUB: {
    RD = RS - RT;
    break;
  } // Subtraction
  case OR: {
    RD = RS | RT;
    break;
  } // Bit Or
  case AND: {
    RD = RS & RT;
    break;
  } // Bit And
  case SUBU: {
    RD = RS - RT;
    break;
  } // Subtraction Unsigned
  case XOR: {
    RD = RS ^ RT;
    break;
  } // Exclusive Or
  case NOR: {
    RD = NOR_OP(RS, RT);
    break;
  } // Nor
  case JR: {
    CURRENT_STATE.PC = RS - 4;
    break;
  }
  case JALR: {
    RD = CURRENT_STATE.PC + 4;
    CURRENT_STATE.PC = RS - 4;
    break;
  }
  case MTLO: {
    NEXT_STATE.LO = RS;
    break;
  }
  case MTHI: {
    NEXT_STATE.HI = RS;
    break;
  }
  case MFHI: {
    RD = CURRENT_STATE.HI;
    break;
  }
  case MFLO: {
    RD = CURRENT_STATE.LO;
    break;
  }
  case MULT: {
    s32 rs     = RS;
    s32 rt     = RT;
    s64 result = rs * rt;
    NEXT_STATE.HI = cast(u32, (result >> 32));
    NEXT_STATE.LO = cast(u32, result);
    break;
  }
  case MULTU: {
    u64 result = cast(u64, RS) * cast(u64, RT);
    NEXT_STATE.HI = cast(u32, (result >> 32));
    NEXT_STATE.LO = cast(u32, result);
    break;
  }
  case SLLV:
  {
    u8 shift_amount = (RS & MASK(5));
    RD = (RT << shift_amount);
    break;
  }
  case SLT:
  {
    s32 rt = RT;
    s32 rs = RS;
    RD = (rs<rt) ? 1 : 0;
    break;
  }
  case SLTU:
  {
    u32 rt = RT;
    u32 rs = RS;
    RD = (rs<rt) ? 1 : 0;
    break;
  }
  case SRAV:
  {
    u8 shift_amount = (RS & MASK(5));
    u32 rt = RT;
    u8 sign_bit = ((rt >> 31) & 1);
    u32 res = (rt >> shift_amount);
    if (sign_bit==1) res |= (MASK(shift_amount) << (32-shift_amount));
    RD = res;
    break;
  }
  case SRLV:
  {
    u8 shift_amount = (RS & MASK(5));
    RD = (RT >> shift_amount);
    break;
  }
  case DIV: {
    s32 numerator   = RS;
    s32 denominator = RT;
    if (denominator != 0) {
      NEXT_STATE.LO = u32t(numerator / denominator);
      NEXT_STATE.HI = u32t(numerator % denominator);
    }
    break;
  }
  case DIVU: {
    u32 denominator = RT;
    u32 numerator = RS;
    if (denominator != 0) {
      NEXT_STATE.LO = u32t(numerator / denominator);
      NEXT_STATE.HI = u32t(numerator % denominator);
    }
    break;
  }
  case SLL: {
    RD = RT << SA;
    break;
  } // Shift Left
  case SRL: {
    RD = RT >> SA;
    break;
  }         // Shirt Right
  case SRA: // Shift Right Addition
  {
    uint8_t sa = SA;
    uint32_t operand = RT;

    int need_extend = (operand >> 31) & 1;
    uint32_t result = (operand >> sa);

    if (need_extend) {
      /* Sign extension */
      result = result | MASK1(sa, INSTR_SIZE - sa);
    }

    RD = result;
    break;
  }
  }

#undef RD
#undef RS
#undef RT
#undef SA
}

void process_instruction() {

  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory. */
  uint32_t mem = mem_read_32(CURRENT_STATE.PC);
  uint8_t instr = GET(OP, mem);

  /* Instruction jump tables */
  static const void *jumpTable[] = {OPCODES(MK_LBL) MK_LBL(NEXT_STATE)};

/* Helpers */
#define END_LABEL LBL(NEXT_STATE) :
#define RS (CURRENT_STATE.REGS[GET(RS, mem)])
#define RT (NEXT_STATE.REGS[GET(RT, mem)])
#define IMM (GET(IM, mem))

  /* Dispatch the instruction. */
  DISPATCH(instr) {
    /* First six bits are 0s */
    LBL(SPECIAL) : {
      CALL_HANDLER(SPECIAL);
      NEXT;
    }
    LBL(REGIMM) : {
      CALL_HANDLER(REGIMM);
      NEXT;
    }

    /* Basic cases, none of these messes control flow */
    LBL(ADDI) : LBL(ADDIU) : {
      uint32_t result = sign_extend_16(u32t(IMM));
      RT = RS + result;
      NEXT;
    }
    LBL(XORI) : {
      RT = RS ^ u32t(IMM);
      NEXT;
    }
    LBL(ANDI) : {
      RT = RS & u32t(IMM);
      NEXT;
    }
    LBL(ORI) : {
      RT = RS | u32t(IMM);
      NEXT;
    }
    LBL(LUI) : {
      RT = (IMM << 16);
      NEXT;
    }
    LBL(J) : {
      u32 jp = GET_BLOCK(mem, 0, 26);
      CURRENT_STATE.PC = ((jp << 2) - 4);
      NEXT;
    }

    /* Default case. */
    LBL(PADDING) : { NEXT; }
    LBL(SB) : {
      uint32_t offset = sign_extend_16(IMM);
      uint32_t address = offset + RS;
      uint32_t r_rt = CURRENT_STATE.REGS[GET(RT, mem)];
      uint32_t last_byte = (GET_BLOCK(r_rt, 0, 8));
      mem_write_32(address, last_byte);
      NEXT;
    }
    LBL(SH) : {
      uint32_t offset = sign_extend_16(IMM);
      uint32_t address = offset + RS;
      uint32_t r_rt = CURRENT_STATE.REGS[GET(RT, mem)];
      uint32_t last_byte = (GET_BLOCK(r_rt, 0, 16));
      mem_write_32(address, last_byte);
      NEXT;
    }
    LBL(SW) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      mem_write_32(vAddr, RT);
      NEXT;
    }
    LBL(SLTI) : {
      RT = (cast(s32, RS) < cast(s32, sign_extend_16(IMM))) ? 1 : 0;
      NEXT;
    }
    LBL(SLTIU) : {
      u32 result = RS - sign_extend_16(IMM);
      RT = (RS < result) ? 1 : 0;
      NEXT;
    }
    LBL(LB) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 8);

      u32 byte_extended = sign_extend_8(byte);
      RT = byte_extended;
      NEXT;
    }
    LBL(LBU) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 8);
      RT = byte;
      NEXT;
    }
    LBL(LH) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 16);

      u32 byte_extended = sign_extend_16(byte);
      RT = byte_extended;
      NEXT;
    }
    LBL(LHU) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      u32 content = mem_read_32(vAddr);
      u32 byte = GET_BLOCK(content, 0, 16);
      RT = byte;
      NEXT;
    }
    LBL(LW) : {
      u32 vAddr = RS + sign_extend_16(IMM);
      RT = mem_read_32(vAddr);
      NEXT;
    }
    LBL(JAL) : {
      u32 temp = (GET_BLOCK(mem, 0, 25) << 2);
      NEXT_STATE.REGS[R_RA] = CURRENT_STATE.PC + 4;
      CURRENT_STATE.PC = temp - 4;
      NEXT;
    }
    LBL(BEQ) : {
      u32 rs = RS;
      u32 rt = RT;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);
      if (rs == rt) {
        CURRENT_STATE.PC = addr - 4;
      }
      NEXT;
    }
    LBL(BNE) : {
      u32 rs = RS;
      u32 rt = RT;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

      if (rs != rt) {
        CURRENT_STATE.PC = addr - 4;
      }

      NEXT;
    }
    LBL(BLEZ) : {
      u32 rs = RS;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

      if (rs == 0 || ((rs >> 31) == 1)) {
        CURRENT_STATE.PC = addr - 4;
      }

      NEXT;
    }
    LBL(BGTZ) : {
      u32 rs = RS;
      u32 addr = CURRENT_STATE.PC + (sign_extend_16(IMM) << 2);

      if (rs != 0 && ((rs >> 31) == 0)) {
        CURRENT_STATE.PC = addr - 4;
      }
      NEXT;
    }
  }

  /* This is place in this way in order to allow disabling of threaded code */
  END_LABEL {
    /* Increment the program counter */
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
  }

/* Undefine guards */
#undef END_LABEL

#ifdef RS
#undef RS
#endif

#ifdef RT
#undef RT
#endif

#ifdef IMM
#undef IMM
#endif
}