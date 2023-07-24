#include <stdio.h>
#include "shell.h"

//MIPS Opcodes: ttps://docs.rtems.org/doxygen/branches/master/group__mips__ops.html

#define OP_BLTZ   0b000
#define OP_REGIMM 0b001
#define OP_J      0b010
#define OP_JAL    0b011
#define OP_BEQ    0b100
#define OP_BNE    0b101
#define OP_BLEZ   0b110
#define OP_BGTZ   0b111
#define OP_ADDI   0b1000
#define OP_ADDIU  0b1001
#define OP_SLTI   0b1010
#define OP_SLTIU  0b1011
#define OP_ANDI   0b1100
#define OP_ORI    0b1101
#define OP_XORI   0b1110
#define OP_LUI    0b1111
#define OP_MFHI   0b10000
#define OP_MTHI   0b10001
#define OP_MFLO   0b10010
#define OP_MTLO   0b10011
#define OP_BLTZAL 0b10000
#define OP_BGEZAL 0b10001
#define OP_LB     0b100000
#define OP_LH     0b100001
#define OP_LW     0b100011
#define OP_LBU    0b100100
#define OP_LHU    0b100101
#define OP_SB     0b101000
#define OP_SH     0b101001
#define OP_SW     0b101011
#define OP_SPECIAL 0b000
#define OP_SLL    0b000
#define OP_SRL    0b010
#define OP_SRA    0b011
#define OP_SLLV   0b100
#define OP_SRLV   0b110
#define OP_SRAV   0b111
#define OP_JR     0b1000
#define OP_JALR   0b1001
#define OP_ADD    0b100000
#define OP_ADDU   0b100001
#define OP_SUB    0b100010
#define OP_SUBU   0b100011
#define OP_AND    0b100100
#define OP_OR     0b100101
#define OP_XOR    0b100110
#define OP_NOR    0b100111
#define OP_SLT    0b101010
#define OP_SLTU   0b101011
#define OP_MULT   0b11000
#define OP_MULTU  0b11001
#define OP_DIV    0b11010
#define OP_DIVU   0b11011
#define OP_SYSCALL 0b001100
#define OP_BRSPEC 0x01
#define OP_BGEZ 0x1
int prod;

void process_instruction()
{
    /* Innitialize variables */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC); 
    uint32_t opcode = (instruction >> 26) & 0x3F; 
    uint32_t rt = (instruction >> 16) & 0x1F;     
    uint32_t rs = (instruction >> 21) & 0x1F;
    uint32_t rd = (instruction >> 11) & 0x1F;    
    uint32_t shiftAmount = (instruction >> 6) & 0x1F; 
    uint32_t func = (instruction >> 0) & 0x3F; 
    uint32_t immediate = (instruction >> 0) & 0xFFFF;    
    int signExtImmediate  = (immediate & 0x8000) ? (immediate | 0xffff8000) : immediate;      
    uint32_t jumpTarget  = (instruction >> 0) & ((1UL << 26) - 1); 

    switch (opcode) {
        case OP_SPECIAL: {
            switch (func) {
                case OP_ADD:
                case OP_ADDU:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                case OP_SYSCALL:
                    if (CURRENT_STATE.REGS[2] == 10)
                        RUN_BIT = 0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_AND:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_DIV:
                    if (rt != 0)
                    {
                        NEXT_STATE.HI = (int)CURRENT_STATE.REGS[rs] % (int)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = (int)CURRENT_STATE.REGS[rs] / (int)CURRENT_STATE.REGS[rt];
                    }
                    else
                        printf("Division can not be 0\n");
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_DIVU:
                    if (rt != 0)
                    {
                        NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
                    }
                    else
                        printf("Division can not be 0\n");
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_JALR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 8;
                    if (!(CURRENT_STATE.REGS[rs] & 0x03))
                        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    
                    break;

                case OP_JR:
                    if (!(CURRENT_STATE.REGS[rs] & 0x03))
                        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    else
                    {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;

                case OP_MFHI:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_MFLO:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_MTHI:
                    if (rs != 0)
                        NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_MTLO:
                    if (rs != 0)
                        NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_MULT:
                    prod = ((int)CURRENT_STATE.REGS[rs] * (int)CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.HI = (prod >> 31) & 0xFFFFFFFF;
                    NEXT_STATE.LO = (prod)&0xFFFFFFFF;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_MULTU:
                    NEXT_STATE.HI = ((CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) >> 31);
                    NEXT_STATE.LO = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_NOR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_OR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SLL:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shiftAmount;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SLLV:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SLT:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = ((int)CURRENT_STATE.REGS[rs] < (int)CURRENT_STATE.REGS[rt]) ? 1 : 0;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SLTU:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? 1 : 0;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SRA:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> shiftAmount;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SRAV:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SRL:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shiftAmount;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SRLV:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_SUB:
                case OP_SUBU:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OP_XOR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] ^ CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                default:
                    printf("(0x%x) unimplemented subopcode\n\n", func);
                    RUN_BIT = 0;
                } 
        }    
        break;
        
        case OP_ADD:
        case OP_ADDU:
            if (rt != 0)
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + signExtImmediate;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
            case OP_BRSPEC: { 
                switch (rt) {
                    case OP_BLTZ:
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        if ((int)CURRENT_STATE.REGS[rs] < 0)
                            NEXT_STATE.PC = NEXT_STATE.PC + (signExtImmediate << 2);
                        break;

                    case OP_BGEZ:
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        if ((int)CURRENT_STATE.REGS[rs] >= 0)
                            NEXT_STATE.PC = NEXT_STATE.PC + (signExtImmediate << 2);
                        break;

                    case OP_BLTZAL:
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                        if ((int)CURRENT_STATE.REGS[rs] < 0)
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (signExtImmediate << 2);
                        else
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                    case OP_BGEZAL:
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                        if ((int)CURRENT_STATE.REGS[rs] >= 0)
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (signExtImmediate << 2);
                        else
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                }
            }
            break;

        case OP_SLTI:
            NEXT_STATE.REGS[rt] = (((int)CURRENT_STATE.REGS[rs] < (int)signExtImmediate) ? 1 : 0);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_SLTIU:
            NEXT_STATE.REGS[rt] = ((CURRENT_STATE.REGS[rs] < signExtImmediate) ? 1 : 0);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_ANDI:
            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] & ((uint32_t)immediate));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_ORI:
            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] | ((uint32_t)immediate));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_XORI:
            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] ^ ((uint32_t)immediate));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_J:
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (jumpTarget << 2);
            break;

        case OP_JAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (jumpTarget << 2);
            break;

        case OP_BEQ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
                NEXT_STATE.PC = NEXT_STATE.PC + (signExtImmediate << 2);
            break;

        case OP_BNE:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
                NEXT_STATE.PC = NEXT_STATE.PC + (signExtImmediate << 2);
            break;

        case OP_BLEZ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if (CURRENT_STATE.REGS[rs] <= 0)
                NEXT_STATE.PC = NEXT_STATE.PC + (signExtImmediate << 2);
            break;

        case OP_BGTZ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if (CURRENT_STATE.REGS[rs] > 0)
                NEXT_STATE.PC = NEXT_STATE.PC + (signExtImmediate << 2);
            break;

        case OP_LUI:
            if (rs == 0)
                NEXT_STATE.REGS[rt] = (immediate << 16);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_LW:
            NEXT_STATE.REGS[rt] = mem_read_32((CURRENT_STATE.REGS[rs] + signExtImmediate));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_LBU:
            NEXT_STATE.REGS[rt] = ((uint32_t)(mem_read_32(CURRENT_STATE.REGS[rs] + signExtImmediate)) & 0xFF);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_LHU:
            NEXT_STATE.REGS[rt] = ((uint32_t)(mem_read_32(CURRENT_STATE.REGS[rs] + signExtImmediate)) & 0xFFFF);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_SB:
            mem_write_32((CURRENT_STATE.REGS[rs] + signExtImmediate), (int)((CURRENT_STATE.REGS[rt]) & 0xFF));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_SH:
            mem_write_32((CURRENT_STATE.REGS[rs] + signExtImmediate), (int)((CURRENT_STATE.REGS[rt]) & 0xFFFF));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case OP_SW:
            mem_write_32((CURRENT_STATE.REGS[rs] + signExtImmediate), (int)((CURRENT_STATE.REGS[rt])));
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        default:
            printf("(0x%x) unimplemented opcode\n\n", opcode);
            RUN_BIT = 0;
        } 
}

