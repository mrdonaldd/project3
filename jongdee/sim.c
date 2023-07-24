#include <stdio.h>
#include "shell.h"

// special opcode 0 with 6 bits sub-opcode
#define SPECIAL_0    0x0
#define ADD          0x20 // 100000
#define ADDU         0x21 // 100001
#define AND          0x24 // 100100
#define DIV          0x1a // 011010
#define DIVU         0x1b // 011011
#define JALR         0x09 // 001001
#define JR           0x08 // 001000
#define MFHI         0x10 // 010000
#define MFLO         0x12 // 010010
#define MTHI         0x11 // 010001
#define MTLO         0x13 // 010011
#define MULT         0x18 // 011000
#define MULTU        0x19 // 011001
#define NOR          0x27 // 100111
#define OR           0x25 // 100101
#define SLL          0x00 // 000000
#define SLLV         0x04 // 000100
#define SLT          0x2a // 101010
#define SLTU         0x2b // 101011
#define SRA          0x03 // 000011
#define SRAV         0x07 // 000111
#define SRL          0x02 // 000010
#define SRLV         0x06 // 000110
#define SUB          0x22 // 100010
#define SUBU         0x23 // 100011
#define SYSCALL      0x0c // 001100
#define XOR          0x26 // 100110

// special opcode 1 with 5 bits sub-opcode
#define SPECIAL_1    0x01 
#define BGEZ         0x01 // 00001
#define BGEZAL       0x11 // 10001
#define BLTZ         0x00 // 00000
#define BLTZAL       0x10 // 10000 

// normal 6 bits opcode
#define ADDI         0x08 // 001000
#define ADDIU        0x09 // 001001
#define ANDI         0x0c // 001100
#define BEQ          0x04 // 000100
#define BGTZ         0x07 // 000111
#define BLEZ         0x06 // 000110
#define BNE          0x05 // 000101
#define J            0x02 // 000010
#define JAL          0x03 // 000011
#define LB           0x20 // 100000
#define LBU          0x24 // 100100
#define LH           0x21 // 100001
#define LHU          0x25 // 100101
#define LUI          0x0f // 001111
#define LW           0x23 // 100011
#define ORI          0x0d // 001101
#define SB           0x28 // 101000
#define SH           0x29 // 101001
#define SLTI         0x0a // 001010
#define SLTIU        0x0b // 001011
#define SW           0x2b // 101011
#define XORI         0x0e // 001110

// operand decoded from instruction bits
uint32_t inst;          // whole instruction 
uint32_t opcode;        // opcode 
uint32_t rs;            // rs operand 
uint32_t rt;            // rt operand 
uint32_t rd;            // rd operand 
uint32_t shift_amount;  // shift amount 
uint32_t sub_opcode;    // sub opcode for special_0 opcode 
uint32_t imm;           // immediate value 
uint32_t addr_target;   // address target 

// variable for using in various instructions
int sign_extend_imm;    // sign-extended immediate value
int64_t temp_int;       // 64 bits signed int
uint64_t temp_uint;     // 64 bits signed int
uint32_t temp_data;     // 32 bits temp data
uint32_t temp_addr;     // 32 bits temp address

/*---------------function for extend bits---------------*/
// signed extend uint8_t to unint32_t
uint32_t sign_extend_8_to_32(uint8_t num) {
  if (num & 0x80)
    return num | 0xffffff80;
  else
    return num;
}

// signed extend uint16_t to unint32_t
uint32_t sign_extend_16_to_32(uint16_t num) {
  if (num & 0x8000)
    return num | 0xffff8000;
  else
    return num;
}

// unsigned extend uint8_t to unint32_t
uint32_t zero_extend_8_to_32(uint8_t num) { return ((uint32_t)num); }

// unsigned extend uint16_t to unint32_t
uint32_t zero_extend_16_to_32(uint16_t num) { return ((uint32_t)num); }

/*---------------main function---------------*/
void process_instruction()
{
  /* execute one instruction here. You should use CURRENT_STATE and modify
    * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
    * access memory. */

  inst = mem_read_32(CURRENT_STATE.PC);

  // decode operand
  opcode = (inst >> 26) & 0x3F;                // first 6 bits
  rs = (inst >> 21) & 0x1F;                    // next 5 bits 
  rt = (inst >> 16) & 0x1F;                    // next 5 bits 
  rd = (inst >> 11) & 0x1F;                    // next 5 bits 
  shift_amount = (inst >> 6) & 0x1F;           // next 5 bits 
  sub_opcode = inst & 0x3F;                    // last 6 bits
  imm = inst & 0xFFFF;                         // last 16 bits
  sign_extend_imm = sign_extend_16_to_32(imm); // signed extend imm
  addr_target = inst & 0x3FFFFFF;              // last 26 bits

  // run instruction
  switch (opcode) {
    /*-------normal opcode-------*/
    case ADDI:
    case ADDIU:
      // rt = rs + imm
      if (rt != 0)
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + sign_extend_imm;
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case ANDI:
      // rt = rs & extended imm
      NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] & zero_extend_16_to_32(imm));
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case BEQ:
      // if rs == rt, next PC = PC + (sign_extend_imm << 2)
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
        NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
      break;
    case BGTZ:
      // if rs > 0, next PC = PC + (sign_extend_imm << 2)
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      if (CURRENT_STATE.REGS[rs] > 0)
        NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
      break;
    case BLEZ:
      // if rs <= 0, next PC = PC + (sign_extend_imm << 2)
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      if (CURRENT_STATE.REGS[rs] <= 0)
        NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
      break;
    case BNE:
      // if rs != rt, next PC = PC + (sign_extend_imm << 2)
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
        NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);  
      break;
    case J:
      // jump to (addr_taget << 2) | first 4 bits of PC
      NEXT_STATE.PC = (addr_target << 2) | (CURRENT_STATE.PC & 0xF0000000);
      break;
    case JAL:
      // same as J but also r31 = PC + 4
      NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
      NEXT_STATE.PC = (addr_target << 2) | (CURRENT_STATE.PC & 0xF0000000);
      break;
    case LB:
      // load signed extend 1 byte data from rs + sign_extend_imm to rt
      temp_data = mem_read_32(CURRENT_STATE.REGS[rs] + sign_extend_imm);
      NEXT_STATE.REGS[rt] = sign_extend_8_to_32(temp_data & 0xFF);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case LBU:
      // same as LB but unsigned extend data
      temp_data = mem_read_32(CURRENT_STATE.REGS[rs] + sign_extend_imm);
      NEXT_STATE.REGS[rt] = zero_extend_8_to_32(temp_data & 0xFF);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case LH:
      // same as LB but 2 bytes data
      temp_data = mem_read_32(CURRENT_STATE.REGS[rs] + sign_extend_imm);
      NEXT_STATE.REGS[rt] = sign_extend_16_to_32(temp_data & 0xFFFF);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case LHU:
      // same as LBU but 2 bytes data
      temp_data = mem_read_32(CURRENT_STATE.REGS[rs] + sign_extend_imm);
      NEXT_STATE.REGS[rt] = zero_extend_16_to_32(temp_data & 0xFFFF);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case LUI:
      // rt = (imm << 16)
      if (rs == 0) { NEXT_STATE.REGS[rt] = (imm << 16);}
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case LW:
      // same as LB but 4 bytes data
      temp_data = mem_read_32(CURRENT_STATE.REGS[rs] + sign_extend_imm);
      NEXT_STATE.REGS[rt] = sign_extend_16_to_32(temp_data);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case ORI:
      // rt = rs | zero extend imm
      NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] | zero_extend_16_to_32(imm));
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case SB:
      // write signed 1 byte data from rt to rs + sign_extend_imm
      temp_data = CURRENT_STATE.REGS[rt] & 0xFF;
      temp_addr = CURRENT_STATE.REGS[rs] + sign_extend_imm;
      mem_write_32(temp_addr, (int)temp_data);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case SH:
      // same as SB but 2 bytes data
      temp_data = CURRENT_STATE.REGS[rt] & 0xFFFF;
      temp_addr = CURRENT_STATE.REGS[rs] + sign_extend_imm;
      mem_write_32(temp_addr, (int)temp_data);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case SW:
      // same as SB but 4 bytes data
      temp_data = CURRENT_STATE.REGS[rt];
      temp_addr = CURRENT_STATE.REGS[rs] + sign_extend_imm;
      mem_write_32(temp_addr, (int)temp_data);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case SLTI:
      // if signed rs < signed sign_extend_imm, rt = 1; else, rt = 0
      if ((int)CURRENT_STATE.REGS[rs] < (int)sign_extend_imm)
        NEXT_STATE.REGS[rt] = 1;
      else
        NEXT_STATE.REGS[rt] = 0;
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case SLTIU:
      // same as SLTI but compare as unsigned int
      if (CURRENT_STATE.REGS[rs] < sign_extend_imm)
        NEXT_STATE.REGS[rt] = 1;
      else
        NEXT_STATE.REGS[rt] = 0;
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;
    case XORI:
      // rt = rs ^ zero extend imm
      NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] ^ zero_extend_16_to_32(imm));
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      break;

    /*-------special opcode 0-------*/
    case SPECIAL_0:
      switch (sub_opcode) {
        case ADD:
        case ADDU:
          // rd = rs + rt
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case AND:
          // rd = rs & t
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case DIV:
          // HI = rs%rt, LO = rs/rt, both computed by signed value
          if (rt != 0) {
            NEXT_STATE.HI = (int)CURRENT_STATE.REGS[rs] % (int)CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (int)CURRENT_STATE.REGS[rs] / (int)CURRENT_STATE.REGS[rt];
          } else {
            // if rt==0, undefined result and report error
            printf("Divided by zero, undefined result\n");
          }
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case DIVU:
          // same as DIV but computed by unsigned value
          if (rt != 0) {
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
          } else {
            printf("Divided by zero, undefined result\n");
          }
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case JALR:
          // jump to rs, rd = PC + 4
          NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
          // two low-order bits of rs must be 0, if not, report error
          if(!(CURRENT_STATE.REGS[rs] & 0x03))
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
          else {
            printf("Invalid Address, address is not word-aligned\n");
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          }
          break;
        case JR:
          // same as JALR but not save PC+4 to rd
          if(!(CURRENT_STATE.REGS[rs] & 0x03))
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
          else {
            printf("Invalid Address, address is not word-aligned\n");
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          }
          break;
        case MFHI:
          // rd = HI
          NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case MFLO:
          // rd = LO
          NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case MTHI:
          // HI = rs
          NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case MTLO:
          // LO = rs
          NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case MULT:
          // 64 signed bits rs*rt, first 32 bits saved in HI, last 32 bits saved in LO
          temp_int = ((int)CURRENT_STATE.REGS[rs] * (int)CURRENT_STATE.REGS[rt]);
          NEXT_STATE.HI = (temp_int >> 32) & 0xFFFFFFFF;
          NEXT_STATE.LO = temp_int & 0xFFFFFFFF;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case MULTU:
          // same as MULT but 64 unsigned bits
          temp_uint = CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt];
          NEXT_STATE.HI = (temp_uint >> 32) & 0xFFFFFFFF;
          NEXT_STATE.LO = temp_uint & 0xFFFFFFFF;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case NOR:
          // rd = ~(rs | rt)
          NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case OR:
          // rd = rs | rt
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SLL:
          // rd = rt << shift_amount
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shift_amount;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SLLV:
          // rd = rt << five low-order bits of rs
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SLT:
          // if signed rs < rt, rd = 1; else, rd = 0
          if ((int)CURRENT_STATE.REGS[rs] < (int)CURRENT_STATE.REGS[rt])
            NEXT_STATE.REGS[rd] = 1;
          else
            NEXT_STATE.REGS[rd] = 0;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SLTU:
          // same as SLT but unsigned
          if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])
            NEXT_STATE.REGS[rd] = 1;
          else
            NEXT_STATE.REGS[rd] = 0;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SRA:
          // rd = signed rt >> shift_amount
          NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> shift_amount;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SRAV:
          // rd = signed rt >> five low-order bits of rs
          NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SRL:
          // rd = rt >> shift_amount
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shift_amount;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SRLV:
          // rd = rt >> five low-order bits of rs
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SUB:
        case SUBU:
          // rd = rs - rt
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case SYSCALL:
          // if r2 == 10, RUN_BIT = 0
          if (CURRENT_STATE.REGS[2] == 10) { RUN_BIT = 0; }
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        case XOR:
          // rd = rs ^ rt
          NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          break;
        /*-------default of special_0 switch case-------*/
        default:
          // report unimplemented instruction error
          fprintf(stderr, "Unimplemented instruction under Special opcode 0x%x\n", sub_opcode);
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      }
      break;

    /*-------special opcode 1-------*/
    case SPECIAL_1:
      // 5 bits sub opcode is in the place of rt
      switch (rt) {
        case BGEZ:
          // if rs >= 0, next PC = PC + (sign_extend_imm << 2)
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          if ((int)CURRENT_STATE.REGS[rs] >= 0)
            NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
          break;
        case BGEZAL:
          // same as BGEZ but also r31 = PC + 4
          NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          if ((int)CURRENT_STATE.REGS[rs] >= 0)
            NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
          break;
        case BLTZ:
          // if rs < 0, next PC = PC + (sign_extend_imm << 2)
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          if ((int)CURRENT_STATE.REGS[rs] < 0)
            NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
          break;
        case BLTZAL:
          // same as BLTZ but also r31 = PC + 4
          NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
          if ((int)CURRENT_STATE.REGS[rs] < 0)
            NEXT_STATE.PC = CURRENT_STATE.PC + (sign_extend_imm << 2);
          break;
        /*-------default of special_1 switch case-------*/
        default:
          // report unimplemented instruction error
          fprintf(stderr, "Unimplemented instruction under Special branch 0x%x\n", rt);
          NEXT_STATE.PC = CURRENT_STATE.PC + 4;
      }
      break;
    
    /*-------default of main switch case-------*/
    default:
      // report unimplemented instruction error
      fprintf(stderr, "Unimplemented instruction 0x%x\n", opcode);
      NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
