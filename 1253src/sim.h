#include <stdio.h>
#include "shell.h"


typedef enum {
    ADDI = 0b001000,
    ADDIU = 0b001001,
    SLTI = 0b001010,
    SLTIU = 0b001011,
    ANDI = 0b001100,
    ORI = 0b001101,
    XORI = 0b001110,
    LUI = 0b001111,
    LB = 0b100000,
    LH = 0b100001,
    LW = 0b100011,
    LBU = 0b100100,
    LHU = 0b100101,
    SB = 0b101000,
    SH = 0b101001,
    SW = 0b101011,

    SPECIAL = 0b000000,
    REGIMM = 0b000001,

    J = 0b000010,
    JAL = 0b00011,
    BEQ = 0b000100,
    BNE = 0b000101,
    BLEZ = 0b000110,
    BGTZ = 0b000111,

} Instruction;

typedef enum{
    SLL = 0b000000,
    SRL = 0b000010,
    SRA = 0b000011,
    SLLV = 0b000100,
    SRAV = 0b000111,
    SRLV = 0b000110,
    ADD = 0b100000,
    ADDU = 0b100001,
    SUB = 0b100010,
    SUBU = 0b100011,
    AND = 0b100100,
    OR = 0b100101,
    XOR = 0b100110,
    NOR = 0b100111,
    SLT = 0b101010,
    SLTU = 0b101011,
    MULT = 0b011000,
    MFHI = 0b010000,
    MFLO = 0b010010,
    MTHI = 0b010001,
    MTLO = 0b010011,
    MULTU = 0b011001,
    DIV = 0b011010,
    DIVU = 0b011011,
    SYSCALL = 0b001100,
    JR = 0b001000,
    JALR = 0b001001
} SpecialInstruction;

typedef enum{
    BLTZ = 0b00000,
    BGEZ = 0b00001,
    BLTZAL = 0b10000,
    BGEZAL = 0b10001,
} Regimm;

