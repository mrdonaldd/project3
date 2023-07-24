#include "sim.h"
#include "shell.h"
#include <stdio.h>
void process_opcode(uint32_t instruction)
{
    uint8_t opcode = instruction >> 26; // opcode is the first 6 bits of the instruction
    uint8_t rs, rt, rd;
    int32_t immediate, offset;
    uint32_t virtual_address;
    uint32_t memory_word;
    
    switch (opcode)
    {
    case ADDI:
        rt = (instruction >> 16) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        immediate = (int32_t)(int16_t)(instruction & 0x0000FFFF);
        NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + immediate;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("ADDI instruction: rs=%u, rt=%u, immediate=%d, new PC=%u\n", rs, rt, immediate, NEXT_STATE.PC);
        break;

    case ADDIU:
        rt = (instruction >> 16) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        immediate = (int32_t)(int16_t)(instruction & 0x0000FFFF);
        NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + immediate;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("ADDIU instruction: rs=%u, rt=%u, immediate=%d, new PC=%u\n", rs, rt, immediate, NEXT_STATE.PC);
        break;

    case ANDI:
        rt = (instruction >> 16) & 0x1F;
        rs = (instruction >> 21) & 0x1F;
        immediate = instruction & 0x0000FFFF;
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & immediate;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("ANDI instruction: rs=%u, rt=%u, immediate=%d, new PC=%u\n", rs, rt, immediate, NEXT_STATE.PC);
        break;

    case BEQ:
        rs = (instruction >> 21) & 0x1F;
        rt = (instruction >> 16) & 0x1F;
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)) << 2;
        if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        else
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
        printf("BEQ instruction: rs=%u, rt=%u, offset=%d, new PC=%u\n", rs, rt, offset, NEXT_STATE.PC);
        break; 
    case BGTZ:
        rs = (instruction & 0x03E00000) >> 21; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)) << 2; // 16-bit offset
        if ((int32_t)CURRENT_STATE.REGS[rs] > 0)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        else
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
        printf("BGTZ instruction: rs=%u, offset=%d, new PC=%u\n", rs, offset, NEXT_STATE.PC);
        break;
    case BLEZ:
        rs = (instruction & 0x03E00000) >> 21; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)) << 2; // 16-bit offset
        if ((int32_t)CURRENT_STATE.REGS[rs] <= 0)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        else
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
        printf("BLEZ instruction: rs=%u, offset=%d, new PC=%u\n", rs, offset, NEXT_STATE.PC);
        break;
    case BNE:
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)) << 2; // 16-bit offset
        if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        }
        else
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
        printf("BNE instruction: rs=%u, rt=%u, offset=%d, new PC=%u\n", rs, rt, offset, NEXT_STATE.PC);
        break;
    case J:
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | ((instruction & 0x03FFFFFF) << 2);
        printf("J instruction: new PC=%u\n", NEXT_STATE.PC);
        break;
    case JAL:
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | ((instruction & 0x03FFFFFF) << 2);
        printf("JAL instruction: new PC=%u\n", NEXT_STATE.PC);
        break;
    case LB:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        memory_word = mem_read_32(virtual_address & ~0x3);  // Word align the address
        NEXT_STATE.REGS[rt] = (int32_t) (int8_t) (memory_word >> ((virtual_address & 0x3) * 8));  // Extract the byte
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("LB instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, memory_word=%d, new PC=%u\n", rs, rt, offset, virtual_address, memory_word, NEXT_STATE.PC);
        break;
    case LBU:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        memory_word = mem_read_32(virtual_address & ~0x3);  // Word align the address
        NEXT_STATE.REGS[rt] = (uint32_t) (uint8_t) (memory_word >> ((virtual_address & 0x3) * 8));  // Extract the byte
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("LBU instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, memory_word=%d, new PC=%u\n", rs, rt, offset, virtual_address, memory_word, NEXT_STATE.PC);
        break;
    
    case LH:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        memory_word = mem_read_32(virtual_address & ~0x3);  // Word align the address
        NEXT_STATE.REGS[rt] = (int32_t) (int16_t) (memory_word >> ((virtual_address & 0x2) * 8));  // Extract the half-memory_word
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("LH instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, memory_word=%d, new PC=%u\n", rs, rt, offset, virtual_address, memory_word, NEXT_STATE.PC);
        break;

    case LHU:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        memory_word = mem_read_32(virtual_address & ~0x3);  // Word align the address
        NEXT_STATE.REGS[rt] = (uint32_t) (uint16_t) (memory_word >> ((virtual_address & 0x2) * 8));  // Extract the half-memory_word
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("LHU instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, memory_word=%d, new PC=%u\n", rs, rt, offset, virtual_address, memory_word, NEXT_STATE.PC);
        break;    

    case LUI:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        NEXT_STATE.REGS[rt] = (instruction & 0x0000FFFF) << 16;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("LUI instruction: rt=%u, new PC=%u\n", rt, NEXT_STATE.PC);
        break; 


    case LW:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        NEXT_STATE.REGS[rt] = mem_read_32(virtual_address);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("LW instruction: rs=%u, rt=%u, offset=%d,vaddr=%u, new PC=%u\n", rs, rt, offset, virtual_address, NEXT_STATE.PC);
        break;

    case SB:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        memory_word = mem_read_32(virtual_address & ~0x3);  // Word align the address
        memory_word = (memory_word & ~(0xFF << ((virtual_address & 0x3) * 8))) | ((CURRENT_STATE.REGS[rt] & 0xFF) << ((virtual_address & 0x3) * 8));
        mem_write_32(virtual_address & ~0x3, memory_word);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SB instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, memory_word=%d, new PC=%u\n", rs, rt, offset, virtual_address, memory_word, NEXT_STATE.PC);
        break;

    case SH:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        memory_word = mem_read_32(virtual_address & ~0x3);  // Word align the address
        memory_word = (memory_word & ~(0xFFFF << ((virtual_address & 0x2) * 8))) | ((CURRENT_STATE.REGS[rt] & 0xFFFF) << ((virtual_address & 0x2) * 8));
        mem_write_32(virtual_address & ~0x3, memory_word);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SH instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, memory_word=%d, new PC=%u\n", rs, rt, offset, virtual_address, memory_word, NEXT_STATE.PC);
        break;

    case SW:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        offset = (int32_t)((int16_t)(instruction & 0x0000FFFF)); // 16-bit offset
        virtual_address = CURRENT_STATE.REGS[rs] + offset;
        mem_write_32(virtual_address, CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SW instruction: rs=%u, rt=%u, offset=%d,virtual_address=%u, new PC=%u\n", rs, rt, offset, virtual_address, NEXT_STATE.PC);
        break;

    case SLTI:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        immediate = (int32_t)(int16_t)(instruction & 0x0000FFFF); // 16-bit immediate value
        
        if ((int32_t)CURRENT_STATE.REGS[rs] < immediate) {
            NEXT_STATE.REGS[rt] = 1;  // if rs < immediate, then rt = 1
        } else {
            NEXT_STATE.REGS[rt] = 0;  // if rs >= immediate, then rt = 0
        }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLTI instruction: rs=%u, rt=%u, immediate=%d, new PC=%u\n", rs, rt, immediate, NEXT_STATE.PC);
        break;
    
    case SLTIU:
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        immediate = instruction & 0x0000FFFF; // 16-bit immediate value
        
        if ((uint32_t)CURRENT_STATE.REGS[rs] < (uint32_t)immediate) {
            NEXT_STATE.REGS[rt] = 1;  // if rs < immediate, then rt = 1
        } else {
            NEXT_STATE.REGS[rt] = 0;  // if rs >= immediate, then rt = 0
        }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("SLTIU instruction: rs=%u, rt=%u, immediate=%d, new PC=%u\n", rs, rt, immediate, NEXT_STATE.PC);
        break;

    case XORI:
        rs = (instruction >> 21) & 0x1F; // 5-bit rs
        rt = (instruction >> 16) & 0x1F; // 5-bit rt
        immediate = instruction & 0x0000FFFF; // 16-bit immediate value
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ immediate;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        printf("XORI instruction: rs=%u, rt=%u, immediate=%d, new PC=%u\n", rs, rt, immediate, NEXT_STATE.PC);
        break;

        
    default:
        break;
    }
}