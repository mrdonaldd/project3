#include <stdio.h>
#include <stdint.h>
#include "shell.h"

uint32_t signExtend16(uint32_t input);

#define GET_BITS(start, len, input) ((uint32_t) (((input) >> (start)) & ((1 << (len)) - 1)))
#define VIRTUAL_ADDRESS(input) ((uint32_t) (signExtend16(GET_BITS(0, 16, input)) + CURRENT_STATE.REGS[GET_BITS(21, 5, input)])) 
#define PRINT_LINE //(printf("Did till line %d\n", __LINE__))

// Credits: https://www.geeksforgeeks.org/binary-representation-of-a-given-number/
void bin(unsigned n)
{
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}

/*

DONE: SYSCALL, ADDIU, ADDU, OR, ADDI, SUBU, XOR, XORI, SRL, SRA, AND, SUB, J, ORI, BNE, BEQ,
BGTZ, JAL, JR, JALR, BLEZ, BLTZ, BLTZAL, BGEZ, BGEZAL, ANDI, LW, SW, SB, LBU, LB, SH, LHU, LH, 
SLL, SLLV, SRLV, SRAV, ADD, DIV, DIVU, MULT, MFHI, MFLO, MTHI, MTLO, MULTU, SLTIU, SLTI, SLT, SLTU, NOR
UNSURE: 

TO_TEST:  

*/

uint32_t getOpcode(uint32_t input){
    // Get's the bits at position 26 to 31
    return GET_BITS(26, 6, input);
}

uint32_t getRs(uint32_t input){
    // Get's the bits at position 21 till 25
    return GET_BITS(21, 5, input);
}

uint32_t getRt(uint32_t input){
    // Get's the bits at position 16 till 20
    return GET_BITS(16, 5, input);
}

uint32_t getRd(uint32_t input){
    // Get's the bits at position 11 till 15
    return GET_BITS(11, 5, input);
}

uint32_t getImmediate(uint32_t input){
    // Get's the bits at position 0 till 15
    return GET_BITS(0, 16, input);
}

uint32_t signExtend18(uint32_t input){
    if (input & 0b00000000000000100000000000000000) {
        return (input | (uint32_t) 0b11111111111111100000000000000000);
    } else {
        return input;
    }
}

uint32_t signExtend16(uint32_t input){
    // if (input & 0x00008000) {
    if (input & 0b00000000000000001000000000000000) {
        return (input | (uint32_t) 0xFFFF0000);
    } else {
        return input;
    }
}

uint32_t signExtend14(uint32_t input) {
    if (input & 0x00004000) {
        return (input | 0xFFFFC000);
    } else {
        return input;
    }
}

uint32_t signExtendCheck6th(uint32_t input){
    // Get an input and extend the 6th bit till 32
    if (input & 0b100000) {
        return (input | 0b11111111111111111111111111000000);
    } else {
        return input;
    }
}

uint32_t signExtendCheck8th(uint32_t input){
    // Get an input and extend the 6th bit till 32
    if (input & 0b10000000) {
        return (input | 0b11111111111111111111111100000000);
    } else {
        return input;
    }
}

uint32_t zeroExtend(uint32_t input, int position) {
    // Extends the zero bit from a certain position
    uint32_t mask = ~(0xFFFFFFFF << position);
    uint32_t extendedValue = input & mask;
    return extendedValue;
}

uint32_t nextBranch(uint32_t instruction) {
    uint32_t target = signExtend16(GET_BITS(0, 16, instruction)) << 2; 
    return target + CURRENT_STATE.PC;
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    // printf("Binary: ");
    // bin(instruction);
    // printf("\n");
    uint32_t opcode = getOpcode(instruction);
    // printf("opcode: %d, special: %d\n", opcode, (GET_BITS(0, 5, instruction)));
    switch (opcode){
        case 0:
            {
                // Special is 0
                // Switch bottom 6 bits
                // printf("Bottom six bits: %d\n", GET_BITS(0, 6, instruction));
                switch ((GET_BITS(0, 6, instruction))){
                    case 0:
                    {
                        // Possible switch case here
                        if ((GET_BITS(21, 5, instruction)) == 0){
                            // Case SLL
                            NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRt(instruction)] << (GET_BITS(6, 5, instruction));
                        }
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    }
                    case 2:
                        // Case SRL
                        {
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            uint32_t sa = GET_BITS(6, 5, instruction);
                            NEXT_STATE.REGS[getRd(instruction)] = rt >> sa;
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 3:
                        // Case SRA
                        NEXT_STATE.REGS[getRd(instruction)] = signExtendCheck6th(CURRENT_STATE.REGS[getRt(instruction)] >> (GET_BITS(6, 5, instruction)));
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 4:
                        // Case SLLV
                        {
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                            uint32_t value = rt << rs;
                            NEXT_STATE.REGS[getRd(instruction)] = value;
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 6:
                        // Case SRLV
                        {
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                            uint32_t value = (uint32_t) (rt >> rs);
                            NEXT_STATE.REGS[getRd(instruction)] = value;
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 7:
                        // Case SRAV
                        {
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                            uint32_t value = (uint32_t) (((int32_t)rt) >> rs);
                            NEXT_STATE.REGS[getRd(instruction)] = value;
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 8:
                        // Case JR
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.REGS[getRs(instruction)];
                        break;
                    case 9:
                        // Case JALR
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.PC + 4;
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.REGS[getRs(instruction)];
                        break;
                    case 12:
                        // Case SYSCALL
                        if (CURRENT_STATE.REGS[2] == 10) {
                            RUN_BIT = 0;
                        } 
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 16:
                        // Case MFHI
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.HI;
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 17:
                        // Case MTHI
                        NEXT_STATE.HI = CURRENT_STATE.REGS[getRs(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 18:
                        // Case MFLO
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.LO;
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 19:
                        // Case MTLO
                        NEXT_STATE.LO = CURRENT_STATE.REGS[getRs(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 24:
                        // Case MULT
                        {
                            int64_t rt = (int64_t) ((int32_t) CURRENT_STATE.REGS[getRt(instruction)]);
                            int64_t rs = (int64_t) ((int32_t) CURRENT_STATE.REGS[getRs(instruction)]);
                            // int64_t output = rs * rt;
                            // printf("OUTPUT: %ld\n", rs * rt);
                            NEXT_STATE.HI = ((uint64_t) (rs * rt)) >> 32;
                            NEXT_STATE.LO = ((uint64_t) (rs * rt)) & 0xFFFFFFFF;
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 25:
                        // Case MULTU
                        {
                            uint64_t rt = (uint64_t) ((uint32_t) CURRENT_STATE.REGS[getRt(instruction)]);
                            uint64_t rs = (uint64_t) ((uint32_t) CURRENT_STATE.REGS[getRs(instruction)]);
                            // printf("OUTPUT: %ld\n", rs * rt);
                            NEXT_STATE.HI = (rs * rt) >> 32;
                            NEXT_STATE.LO = (rs * rt) & 0xFFFFFFFF;
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 26:
                        // Case DIV
                        {
                            int32_t rt = (int32_t) CURRENT_STATE.REGS[getRt(instruction)];
                            int32_t rs = (int32_t) CURRENT_STATE.REGS[getRs(instruction)];
                            if (rt != 0){
                                NEXT_STATE.HI = (uint32_t) (rs % rt);
                                NEXT_STATE.LO = (uint32_t) (rs / rt);
                            }
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 27:
                        // Case DIVU
                        {
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                            if (rt != 0){
                                NEXT_STATE.HI = rs % rt;
                                NEXT_STATE.LO = rs / rt;
                            }
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 32:
                        // Case ADD
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] + CURRENT_STATE.REGS[getRt(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 33:
                        // Case ADDU
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] + CURRENT_STATE.REGS[getRt(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 34:
                        // Case SUB
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] - CURRENT_STATE.REGS[getRt(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 35:
                        // Case SUBU
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] - CURRENT_STATE.REGS[getRt(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 36:
                        // Case AND
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] & CURRENT_STATE.REGS[getRt(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 37:
                        // Case OR
                        NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] | CURRENT_STATE.REGS[getRt(instruction)];
                        PRINT_LINE;
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    case 38:
                        {
                            if ((GET_BITS(6, 5, instruction)) == 0){
                                // Case XOR
                                // printf("XOR!\n");
                                NEXT_STATE.REGS[getRd(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] ^ CURRENT_STATE.REGS[getRt(instruction)];
                                PRINT_LINE;
                                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                                break;
                            }
                        }
                    case 39:
                        // Case NOR
                        {
                            uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            NEXT_STATE.REGS[getRd(instruction)] = ~(rs | rt);   
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;
                    case 42:
                        // Case SLT
                        {
                            int32_t rs = (int32_t) CURRENT_STATE.REGS[getRs(instruction)];
                            int32_t rt = (int32_t) CURRENT_STATE.REGS[getRt(instruction)];
                            // printf("SLT!!\n");
                            if (rs < rt){
                                NEXT_STATE.REGS[getRd(instruction)] = (uint32_t) 1;
                            }
                            else {
                                NEXT_STATE.REGS[getRd(instruction)] = (uint32_t) 0;
                            }
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break;  
                    case 43:
                        // Case SLTU
                        {
                            uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                            uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                            rs = (rs << 1) >> 1;
                            rt = (rt << 1) >> 1;
                            // printf("SLTU!!\n");
                            if (rs < rt){
                                NEXT_STATE.REGS[getRd(instruction)] = (uint32_t) 1;
                            }
                            else {
                                NEXT_STATE.REGS[getRd(instruction)] = (uint32_t) 0;
                            }
                            PRINT_LINE;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        }
                        break; 
                    default:
                        break;
                }
            break;
            }
        case 1:
            {
                // REGIMM is 1
                // Switch bits 16-20
                switch (GET_BITS(16, 5, instruction)){
                    case 0:
                        // Case BLTZ
                        {
                            if ((CURRENT_STATE.REGS[getRs(instruction)] >> 31) == 1){
                                PRINT_LINE;
                                NEXT_STATE.PC = nextBranch(instruction);
                            }
                            else {
                                PRINT_LINE;
                                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                            }
                        }
                        break;
                    case 1:
                        // Case BGEZ
                        {
                            if ((CURRENT_STATE.REGS[getRs(instruction)] >> 31) == 0){
                                PRINT_LINE;
                                NEXT_STATE.PC = nextBranch(instruction);
                            }
                            else {
                                PRINT_LINE;
                                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                            }
                        }
                        break;
                    case 16:
                        // Case BLTZAL
                        {  
                            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; 
                            if ((CURRENT_STATE.REGS[getRs(instruction)] >> 31) == 1){
                                // printf("Less than zero\n");
                                PRINT_LINE;
                                NEXT_STATE.PC = nextBranch(instruction);
                            }
                            else {
                                // printf("Larger than or equal to zero!\n");
                                PRINT_LINE;
                                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                            }
                        }
                        break;
                    case 17:
                        // Case BGEZAL
                        {
                            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                            if(CURRENT_STATE.REGS[getRs(instruction)] == 0 || !CURRENT_STATE.REGS[getRs(instruction)] >> 31){
                            // if ((GET_BITS(31, 1, CURRENT_STATE.REGS[getRs(instruction)]) == 0)){
                                printf("BGEZAL\n");
                                PRINT_LINE;
                                NEXT_STATE.PC = nextBranch(instruction);
                            }
                            else {
                                PRINT_LINE;
                                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                            }
                        }
                        break;
                    default:
                        break;
                }
            break;
            }
        case 2:
            // Case J
            PRINT_LINE;
            NEXT_STATE.PC = (GET_BITS(0, 26, instruction)) << 2;
            break;
        case 3:
            // Case JAL
            PRINT_LINE;
            NEXT_STATE.PC = (GET_BITS(0, 26, instruction)) << 2;
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; 
            break;
        case 4:
            // Case BEQ
            {
                if (CURRENT_STATE.REGS[getRs(instruction)] == CURRENT_STATE.REGS[getRt(instruction)]){
                    // printf("EQUAL\n");
                    PRINT_LINE;
                    NEXT_STATE.PC = nextBranch(instruction);
                    // printf("NEXT_STATE.PC: ");
                    // bin(NEXT_STATE.PC);
                    // printf("\n");
                }
                else {
                    PRINT_LINE;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                }
                // printf("Binary: ");
                // bin(mem_read_32(NEXT_STATE.PC));
                // printf("\n");
            }
            break;
        case 5:
            // Case BNE
            {
                if (CURRENT_STATE.REGS[getRs(instruction)] != CURRENT_STATE.REGS[getRt(instruction)]){
                    PRINT_LINE;
                    NEXT_STATE.PC = nextBranch(instruction);
                }
                else {
                    PRINT_LINE;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                }
            }
            break;
        case 6:
            // Case BLEZ
            {
                if ((CURRENT_STATE.REGS[getRs(instruction)] == 0) || ((CURRENT_STATE.REGS[getRs(instruction)] >> 31) & 1)) {
                    PRINT_LINE;
                    NEXT_STATE.PC = nextBranch(instruction);
                    // printf("Less than or equal to 0\n");
                }
                else {
                    PRINT_LINE;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                }
            }
            break;
        case 7:
            // Case BGTZ run 15 for brtest1.s
            {
                if ((CURRENT_STATE.REGS[getRs(instruction)] != 0) && ((CURRENT_STATE.REGS[getRs(instruction)] >> 31) == 0)){
                    PRINT_LINE;
                    NEXT_STATE.PC = nextBranch(instruction);
                }
                else {
                    PRINT_LINE;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                }
            }
            break;
        case 8:
            // Case ADDI
            NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] + signExtend16(getImmediate(instruction));
            PRINT_LINE;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case 9:
            // Case ADDIU
            NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] + signExtend16(getImmediate(instruction));
            PRINT_LINE;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case 10:
            // Case SLTI
            {
                int32_t rs = (int32_t) CURRENT_STATE.REGS[getRs(instruction)];
                int32_t offset = (int32_t) signExtend16(GET_BITS(0, 16, instruction));
                if (rs < offset){
                    NEXT_STATE.REGS[getRt(instruction)] = (uint32_t) 1;
                }
                else {
                    NEXT_STATE.REGS[getRt(instruction)] = (uint32_t) 0;
                }
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 11:
            // Case SLTIU
            {
                uint32_t rs = CURRENT_STATE.REGS[getRs(instruction)];
                uint32_t rt = CURRENT_STATE.REGS[getRt(instruction)];
                uint32_t offset = signExtend16(GET_BITS(0, 16, instruction));
                if (rs < offset){
                    NEXT_STATE.REGS[getRt(instruction)] = 1;
                }
                else {
                    NEXT_STATE.REGS[getRt(instruction)] = 0;
                }
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 12:
            // Case ANDI
            NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] & ((uint32_t) getImmediate(instruction));
            // NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] & zeroExtend(getImmediate(instruction), 16);
            PRINT_LINE;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case 13:
            // Case ORI
            NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] | ((uint32_t) getImmediate(instruction));
            // NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] | zeroExtend(getImmediate(instruction), 16);
            PRINT_LINE;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case 14:
            // Case XORI
            NEXT_STATE.REGS[getRt(instruction)] = CURRENT_STATE.REGS[getRs(instruction)] ^ getImmediate(instruction);
            PRINT_LINE;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case 15:
            // Case LUI
            NEXT_STATE.REGS[getRt(instruction)] = getImmediate(instruction) << 16;
            PRINT_LINE;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case 32:
            // Case LB
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t readValue = signExtendCheck8th(mem_read_32(virtualAddress) & 0x000000FF);
                NEXT_STATE.REGS[getRt(instruction)] = readValue;
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 33:
            // Case LH
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t readValue = signExtend16(mem_read_32(virtualAddress) & 0x0000FFFF);
                NEXT_STATE.REGS[getRt(instruction)] = readValue;
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 35:
            // Case LW
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t readValue = mem_read_32(virtualAddress);
                NEXT_STATE.REGS[getRt(instruction)] = readValue;
                // printf("Virtual Address: %d\n", virtualAddress);
                // printf("Read Value: %d\n", readValue);
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 36:
            // Case LBU
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t readValue = mem_read_32(virtualAddress) & 0x000000FF;
                NEXT_STATE.REGS[getRt(instruction)] = readValue;
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 37:
            // Case LHU
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t readValue = mem_read_32(virtualAddress) & 0x0000FFFF;
                NEXT_STATE.REGS[getRt(instruction)] = readValue;
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 40:
            // Case SB
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t writeValue = CURRENT_STATE.REGS[getRt(instruction)] & 0x000000FF;
                mem_write_32(virtualAddress, writeValue);
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 41:
            // Case SH
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t writeValue = CURRENT_STATE.REGS[getRt(instruction)] & 0x0000FFFF;
                mem_write_32(virtualAddress, writeValue);
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        case 43:
            // Case SW
            {
                uint32_t virtualAddress = VIRTUAL_ADDRESS(instruction);
                uint32_t writeValue = CURRENT_STATE.REGS[getRt(instruction)];
                mem_write_32(virtualAddress, writeValue);
                // printf("Virtual Address: %d\n", virtualAddress);
                // printf("Write Value: %d\n", writeValue);
                PRINT_LINE;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        default:
            break;
    }
}