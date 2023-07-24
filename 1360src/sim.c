#include <stdio.h>
#include "shell.h"

uint8_t move = 0;

uint32_t next_counter()
{
    return CURRENT_STATE.PC + 4;
}

uint8_t get_opcode(uint32_t instruction)
{
    return (instruction >> 26) & 0x3f;
}

uint8_t get_register_source(uint32_t instruction)
{
    return (instruction >> 21) & 0x1f;
}

uint8_t get_register_target(uint32_t instruction)
{
    return (instruction >> 16) & 0x1f;
}

uint8_t get_register_destination(uint32_t instruction)
{
    return (instruction >> 11) & 0x1f;
}

uint8_t get_shift_amount(uint32_t instruction)
{
    return (instruction >> 6) & 0x1f;
}

uint8_t get_function(uint32_t instruction)
{
    return instruction & 0x3f;
}
uint16_t get_immediate(uint32_t instruction)
{
    return instruction;
}

uint32_t get_target(uint32_t instruction)
{
    return instruction & 0x3FFFFFF;
}

void special_instruction(uint32_t instruction)
{
    uint8_t rs = get_register_source(instruction);
    uint8_t rt = get_register_target(instruction);
    uint8_t rd = get_register_destination(instruction);
    uint8_t shift = get_shift_amount(instruction);
    uint8_t funct = get_function(instruction);
    switch (funct)
    {
    case 0x20:
        printf("ADD\n");
        NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] + (int32_t)CURRENT_STATE.REGS[rt];
        break;
    case 0x21:
        printf("ADDU\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
        break;
    case 0x22:
        printf("SUB\n");
        NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] - (int32_t)CURRENT_STATE.REGS[rt];
        break;
    case 0x23:
        printf("SUBU\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
        break;
    case 0x24:
        printf("AND\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
        break;
    case 0x25:
        printf("OR\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
        break;
    case 0x26:
        printf("XOR\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
        break;
    case 0x27:
        printf("NOR\n");
        NEXT_STATE.REGS[rd] = ~CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
        break;
    case 0x2A:
        printf("SLT\n");
        if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rs])
        {
            NEXT_STATE.REGS[rd] = 0x80000001;
        }
        else
        {
            NEXT_STATE.REGS[rd] = 0x00000000;
        }
        break;
    case 0x2B:
        printf("SLTU\n");
        if ((0 || CURRENT_STATE.REGS[rs]) < (0 || CURRENT_STATE.REGS[rt]))
        {
            NEXT_STATE.REGS[rd] = 0x80000001;
        }
        else
        {
            NEXT_STATE.REGS[rd] = 0x00000000;
        }
        break;
    case 0x00:
        printf("SLL: %u %u\n", CURRENT_STATE.REGS[rt], shift);
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shift;
        break;
    case 0x02:
        printf("SRL\n");
        NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] >> shift) & ~(((0x1 << sizeof(uint32_t)) >> shift) << 1);
        break;
    case 0x03:
        printf("SRA\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shift;
        break;
    case 0x04:
        printf("SLLV\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
        break;
    case 0x06:
        printf("SRLV\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F) & ~(((0x1 << sizeof(uint32_t)) >> (CURRENT_STATE.REGS[rs] & 0x1F)) << 1);
        break;
    case 0x07:
        printf("SRAV\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
        break;
    case 0x08:
        printf("JR\n");
        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
        move = 1;
        break;
    case 0x09:
        printf("JALR\n");
        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
        NEXT_STATE.REGS[rd] = next_counter();
        move = 1;
        break;
    case 0x10:
        printf("MFHI\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
        break;
    case 0x12:
        printf("MFLO\n");
        NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
        break;
    case 0x11:
        printf("MTHI\n");
        NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
        break;
    case 0x13:
        printf("MTLO\n");
        NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
        break;
    case 0x18:
        printf("MULT\n");
        NEXT_STATE.HI = ((int32_t)CURRENT_STATE.REGS[rs] * (int32_t)CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF00000000;
        NEXT_STATE.LO = ((int32_t)CURRENT_STATE.REGS[rs] * (int32_t)CURRENT_STATE.REGS[rt]) & 0x00000000FFFFFFFF;
        break;
    case 0x19:
        printf("MULTU\n");
        NEXT_STATE.HI = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF00000000;
        NEXT_STATE.LO = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0x00000000FFFFFFFF;
        break;
    case 0x1A:
        printf("DIV\n");
        NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
        NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
        break;
    case 0x1B:
        printf("DIVU\n");
        NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
        NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
        break;
    case 0x0C:
        printf("SYSCALL\n");
        if (CURRENT_STATE.REGS[2] == 0x0A)
        {
            RUN_BIT = 0;
            move = 1;
        }
        break;
    default:
        printf("Unknown instruction\n");
        break;
    }
}

void regimm_instruction(uint32_t instruction)
{
    uint8_t rt = get_register_target(instruction);
    uint8_t rs = get_register_source(instruction);
    uint16_t im = get_immediate(instruction);
    switch (rt)
    {
    case 0x00:
        printf("BLTZ\n");
        if (CURRENT_STATE.REGS[rs] >= 0x80000000)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    case 0x01:
        printf("BGEZ\n");
        if (CURRENT_STATE.REGS[rs] < 0x80000000)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    case 0x10:
        printf("BLTZAL\n");
        CURRENT_STATE.REGS[rs] = next_counter();
        if (CURRENT_STATE.REGS[rs] >= 0x80000000)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
        }
        break;
    case 0x11:
        printf("BGEZAL\n");
        CURRENT_STATE.REGS[rs] = next_counter();
        if (CURRENT_STATE.REGS[rs] < 0x80000000)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    default:
    }
}

void else_instruction(uint32_t instruction)
{
    uint8_t opcode = get_opcode(instruction);
    uint8_t rs = get_register_source(instruction);
    uint8_t rt = get_register_target(instruction);
    uint16_t im = get_immediate(instruction);
    uint32_t tg = get_target(instruction);
    switch (opcode)
    {
    case 0x02:
        printf("J\n");
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (tg << 2);
        move = 1;
        break;
    case 0x03:
        printf("JAL\n");
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (tg << 2);
        NEXT_STATE.REGS[31] = next_counter();
        move = 1;

        break;
    case 0x04:
        printf("BEQ\n");
        if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    case 0x05:
        printf("BNE\n");
        if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    case 0x06:
        printf("BLEZ\n");
        if ((CURRENT_STATE.REGS[rs] & 0x8000000) || CURRENT_STATE.REGS[rs] == 0)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    case 0x07:
        printf("BGTZ\n");
        if ((CURRENT_STATE.REGS[rs] & 0x8000000) == 0 && CURRENT_STATE.REGS[rs])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + im;
            move = 1;
        }
        break;
    case 0x08:
        printf("ADDI\n");
        NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + (int32_t)im;
        break;
    case 0x09:
        printf("ADDIU: %u + %u\n", CURRENT_STATE.REGS[rs], im);
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + im;
        break;
    case 0x0A:
        printf("SLTI\n");
        if ((int32_t)CURRENT_STATE.REGS[rs] < im)
        {
            NEXT_STATE.REGS[rt] = 1;
        }
        else
        {
            NEXT_STATE.REGS[rt] = 0;
        }
        break;
    case 0x0B:
        printf("SLTIU\n");
        if (CURRENT_STATE.REGS[rs] < im)
        {
            CURRENT_STATE.REGS[rt] = 1;
        }
        else
        {
            CURRENT_STATE.REGS[rt] = 0;
        }
        break;
    case 0x0C:
        printf("ANDI\n");
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & im;
        break;
    case 0x0D:
        printf("ORI\n");
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | im;
        break;
    case 0x0E:
        printf("XORI\n");
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ im;
        break;
    case 0x0F:
        printf("LUI\n");
        NEXT_STATE.REGS[rt] = im;
        break;
    case 0x20:
        printf("LB\n");
        NEXT_STATE.REGS[rt] = (((int8_t)mem_read_32(CURRENT_STATE.REGS[rs] + im)) << 24) >> 24;
        break;
    case 0x21:
        printf("LH\n");
        NEXT_STATE.REGS[rt] = (((int16_t)mem_read_32(CURRENT_STATE.REGS[rs] + im)) << 16) >> 16;
        break;
    case 0x23:
        printf("LW\n");
        NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + im);
        break;
    case 0x24:
        printf("LBU\n");
        NEXT_STATE.REGS[rt] = ((mem_read_32(CURRENT_STATE.REGS[rs] + im) & 0x000000FF));
        break;
    case 0x25:
        printf("LHU\n");
        NEXT_STATE.REGS[rt] = ((mem_read_32(CURRENT_STATE.REGS[rs] + im) & 0x0000FFFF));
        break;
    case 0x28:
        printf("SB\n");
        mem_write_32(CURRENT_STATE.REGS[rs] + im, (uint8_t)CURRENT_STATE.REGS[rt]);
        break;
    case 0x29:
        printf("SH\n");
        mem_write_32(CURRENT_STATE.REGS[rs] + im, (uint16_t)CURRENT_STATE.REGS[rt]);
        break;
    case 0x2B:
        printf("SW\n");
        mem_write_32(CURRENT_STATE.REGS[rs] + im, CURRENT_STATE.REGS[rt]);
        break;
    default:
        printf("Unknown instruction\n");
        break;
    }
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    uint8_t opcode = get_opcode(instruction);
    switch (opcode)
    {
    case 0x00:
        special_instruction(instruction);
        break;
    case 0x01:
        regimm_instruction(instruction);
        break;
    default:
        else_instruction(instruction);
        break;
    }
    if (!move)
    {
        NEXT_STATE.PC = next_counter();
    }
}
