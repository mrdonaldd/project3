//Thana Lertlum-umpaiwong 6380271

#include "shell.h"
#include <stdio.h>

void handle_syscall(uint32_t syscall_number) {
    if (syscall_number == 10) {  // If $v0 is 10, stop the simulation.
        RUN_BIT = 0;
    }
}

void process_instruction() {
    //Fetch the instruction
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    //Decode the instruction
    uint32_t opcode = instruction >> 26;  

    switch (opcode) {
        case 0x2: {  //J
            uint32_t target = instruction & 0x03FFFFFF;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
            return;  
        }
        case 0x3: {  //JAL
            uint32_t target = instruction & 0x03FFFFFF;
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;  // link
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
            return;
        }
        case 0x4: {  //BEQ
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            int16_t offset = instruction & 0xFFFF;  

            if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                return;
            }
            break;
        }
        case 0x5: {  //BNE
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            int16_t offset = instruction & 0xFFFF;  

            if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                return;
            }
            break;
        }
        case 0x6: {  //BLEZ
            uint32_t rs = (instruction >> 21) & 0x1F;
            int16_t offset = instruction & 0xFFFF;  

            if ((int32_t)CURRENT_STATE.REGS[rs] <= 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                return;
            }
            break;
        }
        case 0x7: {  //BGTZ
            uint32_t rs = (instruction >> 21) & 0x1F;
            int16_t offset = instruction & 0xFFFF;  

            if ((int32_t)CURRENT_STATE.REGS[rs] > 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                return;
            }
            break;
        }
        case 0x8: {  // ADDI
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            int16_t imm = instruction & 0xFFFF;

            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
            break;
        }
        case 0x9: {  // ADDIU
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            int16_t imm = instruction & 0xFFFF;

            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
            break;
        }
        case 0xA: {  // SLTI
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            int16_t imm = instruction & 0xFFFF;

            NEXT_STATE.REGS[rt] = ((int32_t)CURRENT_STATE.REGS[rs] < imm) ? 1 : 0;
            break;
        }
        case 0xB: {  // SLTIU
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint16_t imm = instruction & 0xFFFF;

            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] < imm) ? 1 : 0;
            break;
        }
        case 0xC: {  // ANDI
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint16_t imm = instruction & 0xFFFF;

            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
            break;
        }
        case 0xD: {  // ORI
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint16_t imm = instruction & 0xFFFF;

            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
            break;
        }
        case 0xE: {  //XORI
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ immediate;
            break;
        }
        case 0x20: {  //LB
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            uint32_t byte = mem_read_32(address & 0xFFFFFFFC) >> ((address & 0x3) * 8);  // read byte
            NEXT_STATE.REGS[rt] = (int32_t)((int8_t)(byte & 0xFF));  // sign extend byte to 32 bits
            break;
        }
        case 0x21: {  //LH
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            uint32_t halfword = mem_read_32(address & 0xFFFFFFFE) >> ((address & 0x2) * 8);  // read halfword
            NEXT_STATE.REGS[rt] = (int32_t)((int16_t)(halfword & 0xFFFF));  // sign extend halfword to 32 bits
            break;
        }
        case 0x23: {  //LW
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            NEXT_STATE.REGS[rt] = mem_read_32(address);
            break;
        }
        case 0x0F: {  //LUI
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            NEXT_STATE.REGS[rt] = immediate << 16;  // Load immediate into upper 16 bits
            break;
        }
        case 0x24: {  //LBU
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            uint32_t byte = mem_read_32(address & 0xFFFFFFFC) >> ((address & 0x3) * 8);  // read byte
            NEXT_STATE.REGS[rt] = byte & 0xFF;  // zero extend byte to 32 bits
            break;
        }
        case 0x25: {  //LHU
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            uint32_t halfword = mem_read_32(address & 0xFFFFFFFE) >> ((address & 0x2) * 8);  // read halfword
            NEXT_STATE.REGS[rt] = halfword & 0xFFFF;  // zero extend halfword to 32 bits
            break;
        }
        case 0x28: {  //SB
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            uint32_t byte = CURRENT_STATE.REGS[rt] & 0xFF;  // get the byte to store
            uint32_t word = mem_read_32(address & 0xFFFFFFFC);
            word = (word & ~(0xFF << ((address & 0x3) * 8))) | (byte << ((address & 0x3) * 8));
            mem_write_32(address & 0xFFFFFFFC, word);
            break;
        }
        case 0x29: {  //SH
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            uint32_t halfword = CURRENT_STATE.REGS[rt] & 0xFFFF;  // get the halfword to store
            uint32_t word = mem_read_32(address & 0xFFFFFFFE);
            word = (word & ~(0xFFFF << ((address & 0x2) * 8))) | (halfword << ((address & 0x2) * 8));
            mem_write_32(address & 0xFFFFFFFE, word);
            break;
        }
        case 0x2B: {  //SW
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t immediate = instruction & 0xFFFF;
            uint32_t address = CURRENT_STATE.REGS[rs] + (int16_t)immediate;  // sign-extended immediate
            mem_write_32(address, CURRENT_STATE.REGS[rt]);
            break;
        }
        case 0x1: {  //(BLTZ, BGEZ, BLTZAL, BGEZAL)
            uint32_t rs = (instruction >> 21) & 0x1F;
            int16_t offset = instruction & 0xFFFF;
            uint32_t rt = (instruction >> 16) & 0x1F;  // used to differentiate between different operations

            switch(rt) {
                case 0x0:  // BLTZ
                    if((int32_t)CURRENT_STATE.REGS[rs] < 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                        return;
                    }
                    break;
                case 0x1:  // BGEZ
                    if((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                        return;
                    }
                    break;
                case 0x10:  // BLTZAL
                    if((int32_t)CURRENT_STATE.REGS[rs] < 0) {
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;  // link
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                        return;
                    }
                    break;
                case 0x11:  // BGEZAL
                    if((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;  // link
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (offset << 2);
                        return;
                    }
                    break;
            }
            break;
        }
        case 0x0: { 
            uint32_t rs = (instruction >> 21) & 0x1F;
            uint32_t rt = (instruction >> 16) & 0x1F;
            uint32_t rd = (instruction >> 11) & 0x1F;
            uint32_t sa = (instruction >> 6) & 0x1F;
            uint32_t funct = instruction & 0x3F;

            switch (funct) {
                case 0x0:  // SLL
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
                    break;
                case 0x2:  // SRL
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
                    break;
                case 0x3:  // SRA
                    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> sa;
                    break;
                case 0x4:  // SLLV
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
                    break;
                case 0x6:  // SRLV
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    break;
                case 0x7:  // SRAV
                    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    break;
                case 0x8:  // JR
                    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    return;
                case 0x9:  // JALR
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;  // link
                    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    return;
                case 0x20:  // ADD
                    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] + (int32_t)CURRENT_STATE.REGS[rt];
                    break;
                case 0x21:  // ADDU
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    break;
                case 0x22:  // SUB
                    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] - (int32_t)CURRENT_STATE.REGS[rt];
                    break;
                case 0x23:  // SUBU
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    break;
                case 0x24:  // AND
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    break;
                case 0x25:  // OR
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                    break;
                case 0x26:  // XOR
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
                    break;
                case 0x27:  // NOR
                    NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                    break;
                case 0x2A:  // SLT
                    NEXT_STATE.REGS[rd] = ((int32_t)CURRENT_STATE.REGS[rs] < (int32_t)CURRENT_STATE.REGS[rt]) ? 1 : 0;
                    break;
                case 0x2B:  // SLTU
                    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? 1 : 0;
                    break;
                case 0x18:  // MULT
                    {
                        int64_t result = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt];
                        CURRENT_STATE.HI = result >> 32;
                        CURRENT_STATE.LO = result & 0xFFFFFFFF;
                    }
                    break;
                case 0x10:  // MFHI
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    break;
                case 0x12:  // MFLO
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    break;
                case 0x11:  // MTHI
                    CURRENT_STATE.HI = CURRENT_STATE.REGS[rs];
                    break;
                case 0x13:  // MTLO
                    CURRENT_STATE.LO = CURRENT_STATE.REGS[rs];
                    break;
                case 0x19:  // MULTU
                    {
                        uint64_t result = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
                        CURRENT_STATE.HI = result >> 32;
                        CURRENT_STATE.LO = result & 0xFFFFFFFF;
                    }
                    break;
                case 0x1A:  // DIV
                    if(CURRENT_STATE.REGS[rt] != 0) {
                        CURRENT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
                        CURRENT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
                    }
                    break;
                case 0x1B:  // DIVU
                    if(CURRENT_STATE.REGS[rt] != 0) {
                        CURRENT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
                        CURRENT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
                    }
                    break;
                case 0xC:  // SYSCALL
                    handle_syscall(CURRENT_STATE.REGS[2]);  // Syscall number is stored in $v0 (register 2).
                    return;
            }
            break;
        }
        default: //debug
            printf("Unknown instruction with opcode 0x%02X at PC = 0x%08X\n", opcode, CURRENT_STATE.PC);
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;  // Increment PC
}
