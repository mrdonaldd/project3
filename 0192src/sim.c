#include <stdio.h>
#include "shell.h"


void process_instruction()
{
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    uint32_t opcode = (instruction >> 26) & ((1 << 6) - 1);
    uint32_t target, rsReg, rtReg, offset;
    int16_t imm;
    uint16_t uimm;
    uint32_t baseReg, sourceReg;
    int32_t rsVal, rtVal;
    uint64_t result;
    int64_t signedResult;
    uint32_t rdReg, targetAddr;
    printf("Instruction: %08x\n", instruction);
    
    if (opcode > 1) {
        switch (opcode) {
            case 2: // J
                printf("j\n");
                target = (instruction & 0x03FFFFFF) << 2;
                CURRENT_STATE.PC = target - 4;
                break;
            case 3: // JAL
                printf("jal\n");
                target = (instruction & 0x03FFFFFF) << 2;
                CURRENT_STATE.PC = target - 4;
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
                printf("target: %x, CURRENT_STATE.pc: %x\n", target, CURRENT_STATE.PC);
                break;
            case 4: // BEQ
                printf("beq\n");
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                offset = (int16_t)(instruction & 0xFFFF);

                if (CURRENT_STATE.REGS[rsReg] == CURRENT_STATE.REGS[rtReg]) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + (offset << 2);
                }
                break;
            case 5: // BNE
                printf("bne\n");
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                offset = (int16_t)(instruction & 0xFFFF);

                if (CURRENT_STATE.REGS[rsReg] != CURRENT_STATE.REGS[rtReg]) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + (offset << 2);
                }
                break;
            case 6: // BLEZ
                printf("blez\n");
                rsReg = (instruction >> 21) & 0x001F;
                offset = (int16_t)(instruction & 0xFFFF);

                if (CURRENT_STATE.REGS[rsReg] <= 0 || (int32_t)CURRENT_STATE.REGS[rsReg] < 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + (offset << 2);
                }
                break;
            case 7: // BGTZ
                printf("bgtz\n");
                rsReg = (instruction >> 21) & 0x001F;
                offset = (int16_t)(instruction & 0xFFFF);

                if (CURRENT_STATE.REGS[rsReg] > 0 && (int32_t)CURRENT_STATE.REGS[rsReg] > 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + (offset << 2);
                }
                break;
            case 8: // ADDI
                imm = (int16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] + imm;
                break;
            case 9: // ADDIU
                printf("addiu\n");
                uimm = (uint16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] + uimm;
                break;
            case 10: // SLTI
                printf("slti\n");
                imm = (int16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = ((int32_t)CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] < imm) ? 1 : 0;
                break;
            case 11: // SLTIU
                printf("sltiu\n");
                uimm = (uint16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = (CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] < uimm) ? 1 : 0;
                break;
            case 12: // ANDI
                printf("andi\n");
                uimm = (uint16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] & uimm;
                break;
            case 13: // ORI
                printf("ori\n");
                uimm = (uint16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] | uimm;
                break;
            case 14: // XORI
                printf("xori\n");
                uimm = (uint16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[instruction & 0x001F] = CURRENT_STATE.REGS[(instruction >> 21) & 0x001F] ^ uimm;
                break;
            case 15: // LUI
                printf("lui\n");
                imm = (int16_t)(instruction & 0x0000FFFF);
                NEXT_STATE.REGS[(instruction >> 16) & 0x001F] = imm << 16;
                break;
            case 32: // LB
                printf("lb\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                uint32_t targetReg = (instruction >> 16) & 0x001F;
                uint32_t virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                uint32_t word = mem_read_32(virtualAddr);
                int8_t byte = (int8_t)((word >> ((virtualAddr & 0x3) * 8)) & 0xFF);
                NEXT_STATE.REGS[targetReg] = (int32_t)byte;
                break;
            case 33: // LH
                printf("lh\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                targetReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                word = mem_read_32(virtualAddr);
                int16_t halfWord = (int16_t)((word >> ((virtualAddr & 0x2) * 8)) & 0xFFFF);
                NEXT_STATE.REGS[targetReg] = (int32_t)halfWord;
                break;
            case 35: // LW
                printf("lw\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                targetReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                NEXT_STATE.REGS[targetReg] = mem_read_32(virtualAddr);
                break;
            case 36: // LBU
                printf("lbu\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                targetReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                word = mem_read_32(virtualAddr);
                uint8_t bytes = (uint8_t)((word >> ((virtualAddr & 0x3) * 8)) & 0xFF);
                NEXT_STATE.REGS[targetReg] = (int32_t)bytes;
                break;
            case 37: // LHU
                printf("lhu\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                targetReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                word = mem_read_32(virtualAddr);
                uint16_t halfWords = (uint16_t)((word >> ((virtualAddr & 0x2) * 8)) & 0xFFFF);
                NEXT_STATE.REGS[targetReg] = (int32_t)halfWords;
                break;
            case 40: // SB
                printf("sb\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                sourceReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                word = mem_read_32(virtualAddr);
                uint32_t byteMask = 0xFF << ((virtualAddr & 0x3) * 8);
                uint32_t newWord = (word & ~byteMask) | ((CURRENT_STATE.REGS[sourceReg] & 0xFF) << ((virtualAddr & 0x3) * 8));
                mem_write_32(virtualAddr, newWord);
                break;
            case 41: // SH
                printf("sh\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                sourceReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                word = mem_read_32(virtualAddr);
                uint32_t halfWordMask = 0xFFFF << ((virtualAddr & 0x2) * 8);
                newWord = (word & ~halfWordMask) | ((CURRENT_STATE.REGS[sourceReg] & 0xFFFF) << ((virtualAddr & 0x2) * 8));
                mem_write_32(virtualAddr, newWord);
                break;
            case 43: // SW
                printf("sw\n");
                offset = (int16_t)(instruction & 0x0000FFFF);
                baseReg = (instruction >> 21) & 0x001F;
                sourceReg = (instruction >> 16) & 0x001F;
                virtualAddr = CURRENT_STATE.REGS[baseReg] + offset;
                mem_write_32(virtualAddr, CURRENT_STATE.REGS[sourceReg]);
                break;
            default:
                break;
        }
    } else if (opcode == 0) {
        opcode = instruction & ((1 << 6) - 1);

        switch (opcode) {
            case 0: // SLL
                printf("sll\n");
                rdReg = (instruction >> 11) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                uint32_t sa = (instruction >> 6) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rtReg] << sa;
                break;
            case 2: // SRL
                printf("srl\n");
                rdReg = (instruction >> 11) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                sa = (instruction >> 6) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rtReg] >> sa;
                break;
            case 3: // SRA
                printf("sra\n");
                rdReg = (instruction >> 11) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                sa = (instruction >> 6) & 0x001F;
                NEXT_STATE.REGS[rdReg] = (int32_t)CURRENT_STATE.REGS[rtReg] >> sa;
                break;
            case 4: // SLLV
                printf("sllv\n");
                rdReg = (instruction >> 11) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                sa = CURRENT_STATE.REGS[rsReg] & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rtReg] << sa;
                break;
            case 6: // SRLV
                printf("srlv\n");
                rdReg = (instruction >> 11) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                sa = CURRENT_STATE.REGS[rsReg] & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rtReg] >> sa;
                break;
            case 7: // SRAV
                printf("srav\n");
                rdReg = (instruction >> 11) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                sa = CURRENT_STATE.REGS[rsReg] & 0x001F;
                NEXT_STATE.REGS[rdReg] = (int32_t)CURRENT_STATE.REGS[rtReg] >> sa;
                break;
            case 8: // JR
                printf("jr\n");
                rsReg = (instruction >> 21) & 0x001F;
                CURRENT_STATE.PC = CURRENT_STATE.REGS[rsReg] - 4;
                break;
            case 9: // JALR
                printf("jalr\n");
                rsReg = (instruction >> 21) & 0x001F;
                rdReg = (instruction >> 11) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.PC + 4;
                CURRENT_STATE.PC = CURRENT_STATE.REGS[rsReg] - 4;
                break;
            case 32: // ADD
                printf("add\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = (int32_t)(CURRENT_STATE.REGS[rsReg] + CURRENT_STATE.REGS[rtReg]);
                break;
            case 33: // ADDU
                printf("addu\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rsReg] + CURRENT_STATE.REGS[rtReg];
                break;
            case 34: // SUB
                printf("sub\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = (int32_t)(CURRENT_STATE.REGS[rsReg] - CURRENT_STATE.REGS[rtReg]);
                break;
            case 35: // SUBU
                printf("subu\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rsReg] - CURRENT_STATE.REGS[rtReg];
                break;
            case 36: // AND
                printf("and\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rsReg] & CURRENT_STATE.REGS[rtReg];
                break;
            case 37: // OR
                printf("or\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rsReg] | CURRENT_STATE.REGS[rtReg];
                break;
            case 38: // XOR
                printf("xor\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.REGS[rsReg] ^ CURRENT_STATE.REGS[rtReg];
                break;
            case 39: // NOR
                printf("nor\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = ~(CURRENT_STATE.REGS[rsReg] | CURRENT_STATE.REGS[rtReg]);
                break;
            case 42: // SLT
                printf("slt\n");
                rdReg = (instruction >> 11) & 0x001F;
                int32_t rsVals = (int32_t)CURRENT_STATE.REGS[(instruction >> 21) & 0x001F];
                int32_t rtVals = (int32_t)CURRENT_STATE.REGS[(instruction >> 16) & 0x001F];
                NEXT_STATE.REGS[rdReg] = (rsVals < rtVals) ? 1 : 0;
                break;
            case 43: // SLTU
                printf("sltu\n");
                rdReg = (instruction >> 11) & 0x001F;
                rsReg = (instruction >> 21) & 0x001F;
                rtReg = (instruction >> 16) & 0x001F;
                NEXT_STATE.REGS[rdReg] = (CURRENT_STATE.REGS[rsReg] < CURRENT_STATE.REGS[rtReg]) ? 1 : 0;
                break;
            case 24: // MULT
                printf("mult\n");
                int32_t rsVal = (int32_t)CURRENT_STATE.REGS[(instruction >> 21) & 0x001F];
                int32_t rtVal = (int32_t)CURRENT_STATE.REGS[(instruction >> 16) & 0x001F];
                int64_t result = (int64_t)rsVal * rtVal;
                NEXT_STATE.HI = (uint32_t)(result >> 32);
                NEXT_STATE.LO = (uint32_t)result;
                break;
            case 16: // MFHI
                printf("mfhi\n");
                rdReg = (instruction >> 11) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.HI;
                break;
            case 18: // MFLO
                printf("mflo\n");
                rdReg = (instruction >> 11) & 0x001F;
                NEXT_STATE.REGS[rdReg] = CURRENT_STATE.LO;
                break;
            case 26: // DIV
                printf("div\n");
                rsVal = (int32_t)CURRENT_STATE.REGS[(instruction >> 21) & 0x001F];
                rtVal = (int32_t)CURRENT_STATE.REGS[(instruction >> 16) & 0x001F];
                if (rtVal == 0) {
                    NEXT_STATE.HI = 0;
                    NEXT_STATE.LO = 0;
                } else {
                    NEXT_STATE.HI = (uint32_t)rsVal % (uint32_t)rtVal;
                    NEXT_STATE.LO = (uint32_t)rsVal / (uint32_t)rtVal;
                }
                break;
            case 27: // DIVU
                printf("divu\n");
                rsVal = CURRENT_STATE.REGS[(instruction >> 21) & 0x001F];
                rtVal = CURRENT_STATE.REGS[(instruction >> 16) & 0x001F];
                if (rtVal == 0) {
                    NEXT_STATE.HI = 0;
                    NEXT_STATE.LO = 0;
                } else {
                    NEXT_STATE.HI = rsVal % rtVal;
                    NEXT_STATE.LO = rsVal / rtVal;
                }
                break;
            case 12://SYSCALL
                printf("syscall\n");
                if (CURRENT_STATE.REGS[2] == 10) {
                    RUN_BIT = FALSE;
                }
                break;
            default:
                break;
        }
    } else {
        opcode = (instruction >> 16) & ((1 << 5) - 1);
        switch (opcode) {
            case 0: //BLTZ
                printf("bltz\n");
                targetAddr = ((int32_t)(int16_t)((instruction >> 0) & ((1 << 16) - 1))) << 2;
                if ((instruction >> 21) & 0x1F) { 
                    CURRENT_STATE.PC = CURRENT_STATE.PC + targetAddr - 4;
                }
                break;
            case 1: //BGEZ
                printf("bgez\n");
                targetAddr = ((int32_t)(int16_t)((instruction >> 0) & ((1 << 16) - 1))) << 2;
                if (!((instruction >> 21) & 0x1F)) {
                    CURRENT_STATE.PC = CURRENT_STATE.PC + targetAddr - 4;
                }
                break;
            case 16: //BLTZAL
                printf("bltzal\n");
                targetAddr = ((int32_t)(int16_t)((instruction >> 0) & ((1 << 16) - 1))) << 2;
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                if ((instruction >> 21) & 0x1F) {
                    CURRENT_STATE.PC = CURRENT_STATE.PC + targetAddr - 4;
                }
                break;
            case 17: //BGEZAL
                printf("bgezal\n");
                targetAddr = ((int32_t)(int16_t)((instruction >> 0) & ((1 << 16) - 1))) << 2;
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                if ((instruction >> 21) & 0x1F) {
                    CURRENT_STATE.PC = CURRENT_STATE.PC + targetAddr - 4;
                }
                break;
            default:
                break;

        }
    }
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
