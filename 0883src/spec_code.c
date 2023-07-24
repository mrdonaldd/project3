#include "sim.h"
#include "shell.h"
#include <stdio.h>
void process_spec_code(uint32_t instruction)
{
    uint8_t secondary_opcode = instruction & 0x0000003F;
    uint8_t rs, rt, rd;
    uint8_t shift;
    uint16_t imm;
    switch (secondary_opcode)
    {
    case ADD:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = (int32_t) CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("ADD instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;
    case ADDU:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] + (int32_t)CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("ADDU instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;
    case AND_FUNCTION:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("AND instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;
    case DIV:
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
        NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("DIV instruction: rs=%u, rt=%u, new PC=%u\n", rs, rt, NEXT_STATE.PC);
        break;
    
    case DIVU:
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.HI = (uint64_t) CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
        NEXT_STATE.LO = (uint64_t) CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("DIVU instruction: rs=%u, rt=%u, new PC=%u\n", rs, rt, NEXT_STATE.PC);
        break;

    case JR:
        rs = (instruction >> 21) & 0x1F; // 
        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
        printf("JR instruction: rs=%u, new PC=%u\n", rs, NEXT_STATE.PC);
        break;
    
    case MFHI:
        rd = (instruction >> 11) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("MFHI instruction: rd=%u, new PC=%u\n", rd, NEXT_STATE.PC);
        break;
    
    case MFLO:
        rd = (instruction >> 11) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("MFLO instruction: rd=%u, new PC=%u\n", rd, NEXT_STATE.PC);
        break;

    case MTHI:
        rs = (instruction >> 21) & 0x1F;
        NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("MTHI instruction: rs=%u, new PC=%u\n", rs, NEXT_STATE.PC);
        break;

    case MTLO:
        rs = (instruction >> 21) & 0x1F;
        NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("MTLO instruction: rs=%u, new PC=%u\n", rs, NEXT_STATE.PC);
        break;

    case MULT:
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.HI = (int64_t) CURRENT_STATE.REGS[rs] * (int64_t) CURRENT_STATE.REGS[rt];
        NEXT_STATE.LO = (int64_t) CURRENT_STATE.REGS[rs] * (int64_t) CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("MULT instruction: rs=%u, rt=%u, new PC=%u\n, HI=%u, LO=%u", rs, rt, NEXT_STATE.PC, NEXT_STATE.HI, NEXT_STATE.LO);
        break;
    
    case MULTU:
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.HI = (uint64_t) CURRENT_STATE.REGS[rs] * (uint64_t) CURRENT_STATE.REGS[rt];
        NEXT_STATE.LO = (uint64_t) CURRENT_STATE.REGS[rs] * (uint64_t) CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("MULTU instruction: rs=%u, rt=%u, new PC=%u\n, HI=%u, LO=%u", rs, rt, NEXT_STATE.PC, NEXT_STATE.HI, NEXT_STATE.LO);
        break;

    case NOR:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("NOR instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;

    case OR:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("OR instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;

    case SLL:
        rd = (instruction >> 11) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        shift = (instruction >> 6) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shift;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLL instruction: rt=%u, rd=%u, shift=%u, new PC=%u\n", rt, rd, shift, NEXT_STATE.PC);
        break;


    case SLLV:
        rd = (instruction >> 11) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLLV instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;

    case SLT:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        if ((int32_t)CURRENT_STATE.REGS[rs] < (int32_t)CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.REGS[rd] = 1;
        }
        else
        {
            NEXT_STATE.REGS[rd] = 0;
        }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLT instruction: result=0x%08X\n rd=%u, rs=%u, rt=%u, new PC=%u\n", NEXT_STATE.REGS[rd], rd, rs, rt, NEXT_STATE.PC);
        break;

    case SLTI:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs

        if (CURRENT_STATE.REGS[rs] < (int16_t)(instruction & 0x0000FFFF)) { 
            NEXT_STATE.REGS[rt] = 1;  // if rs < immediate, then rt = 1
        } else {
            NEXT_STATE.REGS[rt] = 0;  // if rs >= immediate, then rt = 0
        }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLTI instruction: result=0x%08X\n rs=%u, rt=%u, new PC=%u\n", NEXT_STATE.REGS[rt], rs, rt, NEXT_STATE.PC);
        break;

    case SLTIU:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs

        if ((uint32_t)CURRENT_STATE.REGS[rs] < (uint32_t)(instruction & 0x0000FFFF)) { 
            NEXT_STATE.REGS[rt] = 1;  // if rs < immediate, then rt = 1
        } else {
            NEXT_STATE.REGS[rt] = 0;  // if rs >= immediate, then rt = 0
        }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLTIU instruction: result=0x%08X\n rs=%u, rt=%u, new PC=%u\n", NEXT_STATE.REGS[rt], rs, rt, NEXT_STATE.PC);
        break;
    
    case SRA:
        rd = (instruction >> 11) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        uint8_t shift = (instruction >> 6) & 0x1F;
        NEXT_STATE.REGS[rd] = ((int32_t)CURRENT_STATE.REGS[rt]) >> shift;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SRA instruction: rt=%u, rd=%u, shift=%u, new PC=%u\n", rt, rd, shift, NEXT_STATE.PC);
        break;

        
        break;
    case SRAV:
        rd = (instruction >> 11) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SRAV instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;

    case SRL:
        rd = (instruction >> 11) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        shift = (instruction >> 6) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shift;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SRL instruction: rt=%u, rd=%u, shift=%u, new PC=%u\n", rt, rd, shift, NEXT_STATE.PC);
        break;

    
    case SRLV:
        rd = (instruction >> 11) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> CURRENT_STATE.REGS[rs];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SRLV instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;
        
    case SUB:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SUB instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;

    case SUBU:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SUBU instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;

    case XOR:
        rd = (instruction >> 11) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("XOR instruction: rs=%u, rt=%u, rd=%u, new PC=%u\n", rs, rt, rd, NEXT_STATE.PC);
        break;



    case SYSCALL:
        if (CURRENT_STATE.REGS[2] == 0xA)
        {
            RUN_BIT = FALSE;
            printf("0x%08x: syscall\n", CURRENT_STATE.PC);
        }
        else
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
        break;
    default:
        break;
    }
}
