#ifndef MIPS_H
#define MIPS_H

/***************************************************************/
/*                                                             */
/*   General-Purpose Register                                  */
/*                                                             */
/***************************************************************/
#define REG_SYSCALL 2
#define REG_LINK 31

/***************************************************************/
/*                                                             */
/*   Instuction Opcode                                         */
/*                                                             */
/***************************************************************/
#define OPCODE_SPECIAL 0x00
#define OPCODE_REGIMM 0x01
#define OPCODE_ADDI 0x08
#define OPCODE_ADDIU 0x09
#define OPCODE_ANDI 0x0C
#define OPCODE_BEQ 0x04
#define OPCODE_BGTZ 0x07
#define OPCODE_BLEZ 0x06
#define OPCODE_BNE 0x05
#define OPCODE_LB 0x20
#define OPCODE_LBU 0x24
#define OPCODE_LH 0x21
#define OPCODE_LHU 0x25
#define OPCODE_LUI 0x0F
#define OPCODE_LW 0x23
#define OPCODE_ORI 0x0D
#define OPCODE_SB 0x28
#define OPCODE_SLTI 0x0A
#define OPCODE_SLTIU 0x0B
#define OPCODE_SH 0x29
#define OPCODE_SW 0x2B
#define OPCODE_XORI 0x0E
#define OPCODE_J 0x02
#define OPCODE_JAL 0x03

/***************************************************************/
/*                                                             */
/*   Instuction Opcode Extension: Target Register              */
/*                                                             */
/***************************************************************/
#define RT_BGEZ 0x01
#define RT_BGEZAL 0x11
#define RT_BLTZ 0x00
#define RT_BLTZAL 0x10

/***************************************************************/
/*                                                             */
/*   Instuction Opcode Extension: Function                     */
/*                                                             */
/***************************************************************/
#define FUNCT_ADD 0x20
#define FUNCT_ADDU 0x21
#define FUNCT_AND 0x24
#define FUNCT_DIV 0x1A
#define FUNCT_DIVU 0x1B
#define FUNCT_JALR 0x09
#define FUNCT_JR 0x08
#define FUNCT_MFHI 0x10
#define FUNCT_MFLO 0x12
#define FUNCT_MTHI 0x11
#define FUNCT_MTLO 0x13
#define FUNCT_MULT 0x18
#define FUNCT_MULTU 0x19
#define FUNCT_NOR 0x27
#define FUNCT_OR 0x25
#define FUNCT_SLL 0x00
#define FUNCT_SLLV 0x04
#define FUNCT_SLT 0x2A
#define FUNCT_SLTU 0x2B
#define FUNCT_SRA 0x03
#define FUNCT_SRAV 0x07
#define FUNCT_SRL 0x02
#define FUNCT_SRLV 0x06
#define FUNCT_SUB 0x22
#define FUNCT_SUBU 0x23
#define FUNCT_SYSCALL 0x0C
#define FUNCT_XOR 0x26

/***************************************************************/
/*                                                             */
/*   Bit Mask Constants                                        */
/*                                                             */
/***************************************************************/
#define MASK_HIGH_ORDER 0xF0000000
#define MASK_OPCODE 0xFC000000

/* I-type instructions */
#define MASK_I_RS 0x03E00000
#define MASK_I_RT 0x001F0000
#define MASK_I_IMMEDIATE 0x0000FFFF

/* J-type instructions */
#define MASK_J_TARGET 0x03FFFFFF

/* R-type instructions */
#define MASK_R_RS 0x03E00000
#define MASK_R_RT 0x001F0000
#define MASK_R_RD 0x0000F800
#define MASK_R_SHAMT 0x000007C0
#define MASK_R_FUNCT 0x0000003F

/***************************************************************/
/*                                                             */
/*   Bit Shift Constants                                       */
/*                                                             */
/***************************************************************/
#define SHIFT_OPCODE 26

/* I-type instructions */
#define SHIFT_I_RS 21
#define SHIFT_I_RT 16
#define SHIFT_I_IMMEDIATE 0

/* J-type instructions */
#define SHIFT_J_TARGET 0

/* R-type instructions */
#define SHIFT_R_RS 21
#define SHIFT_R_RT 16
#define SHIFT_R_RD 11
#define SHIFT_R_SHAMT 6
#define SHIFT_R_FUNCT 0

#endif /* MIPS_H */
