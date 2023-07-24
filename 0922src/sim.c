#include <stdio.h>
#include "shell.h"

int lastMHI = 3;
int lastMLO = 3;
int lastMULTDIV = 3;

void special_instruction(uint32_t currentInstr, uint32_t rs, uint32_t rt) {
    uint32_t rd = (currentInstr >> 11) & 0x1F;
    uint32_t immediate;
    uint32_t result;
    uint32_t sa;
    uint64_t doubleResult;
    switch(currentInstr & 0x3F) {
        case 0b100000:
            //ADD
            result = (int32_t) CURRENT_STATE.REGS[rs] + (int32_t) CURRENT_STATE.REGS[rt];
            if ( (result >> 31) ^ (result >> 30)) break; //overflow
            NEXT_STATE.REGS[rd] = result;
            break;
        case 0b100001:
            //ADDU
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;
        case 0b100100:
            //AND
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
            break;
        case 0b100101:
            //OR
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            break;
        case 0b100010:
            //SUB
            result = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            if ( (result >> 31) ^ ((result >> 30) & 0x1)) break; //overflow
            NEXT_STATE.REGS[rd] = result;
            break;
        case 0b100011:
            //SUBU
            result = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            NEXT_STATE.REGS[rd] = result;
            break;
        case 0b100111:
            //NOR
            NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
            break;
        case 0b100110:
            //XOR
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            break;
        case 0b011000:
            //MULT
            if (lastMHI < 3 && lastMLO < 3) break;
            doubleResult = (int32_t) CURRENT_STATE.REGS[rs] * (int32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = doubleResult & 0xFFFFFFFF;
            NEXT_STATE.HI = (int64_t) (doubleResult >> 32) & 0xFFFFFFFF;
            lastMULTDIV = 1;
            break;
        case 0b011001:
            //MULTU
            if (lastMHI < 3 && lastMLO < 3) break;
            doubleResult = (uint32_t) CURRENT_STATE.REGS[rs] * (uint32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = doubleResult & 0xFFFFFFFF;
            NEXT_STATE.HI = (doubleResult >> 32) & 0xFFFFFFFF;
            lastMULTDIV = 1;
            break;
        case 0b011010:
            //DIV
            if (lastMHI < 3 && lastMLO < 3) break;
            if (CURRENT_STATE.REGS[rt] == 0) break; // divby0  
            NEXT_STATE.LO = (int32_t) CURRENT_STATE.REGS[rs] / (int32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = (int32_t) CURRENT_STATE.REGS[rs] % (int32_t) CURRENT_STATE.REGS[rt];
            lastMULTDIV = 1;
            break;
        case 0b011011:
            //DIVU
            if (lastMHI < 3 && lastMLO < 3) break;
            if (CURRENT_STATE.REGS[rt] == 0) break; // divby0
            doubleResult = (uint32_t) CURRENT_STATE.REGS[rs] / (uint32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (uint32_t) CURRENT_STATE.REGS[rs] / (uint32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = (uint32_t) CURRENT_STATE.REGS[rs] % (uint32_t) CURRENT_STATE.REGS[rt];
            lastMULTDIV = 1;
            break;
        case 0b010000:
            //MFHI
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            lastMHI = 1;
            break;
        case 0b010010:
            //MFLO
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            lastMLO = 1;
            break;
        case 0b010001:
            //MTHI
            if (lastMULTDIV < lastMHI && lastMULTDIV < lastMLO) {
                NEXT_STATE.HI = 0;
                break;
            }
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
            lastMHI = 1;
            break;
        case 0b010011:
            //MTLO
            if (lastMULTDIV < lastMHI && lastMULTDIV < lastMLO) {
                NEXT_STATE.LO = 0;
                break;
            }
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
            lastMLO = 1;
            break;
        case 0b101010:
            //SLT
            result = 0;
            if ( (int32_t) CURRENT_STATE.REGS[rs] < (int32_t) CURRENT_STATE.REGS[rt])
                result = 1;
            NEXT_STATE.REGS[rd] = result;
            break;
        case 0b101011:
            //SLTU
            result = 0;
            if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])
                result = 1;
            NEXT_STATE.REGS[rd] = result;
            break;
        case 0b000000:
            //SLL
            sa = (currentInstr >> 6) & 0x1F;
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
            break;
        case 0b000100:
            //SLLV
            sa = CURRENT_STATE.REGS[rs] & 0x1F;
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
            break;
        case 0b000010:
            //SRL
            sa = (currentInstr >> 6) & 0x1F;
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
            break;
        case 0b000110:
            //SRLV
            sa = CURRENT_STATE.REGS[rs] & 0x1F;
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
            break;
        case 0b000011:
            //SRA
            sa = (currentInstr >> 6) & 0x1F;
            NEXT_STATE.REGS[rd] = (int32_t) ( CURRENT_STATE.REGS[rt]) >> sa;
            break;
        case 0b000111:
            //SRAV
            sa = CURRENT_STATE.REGS[rs] & 0x1F;
            NEXT_STATE.REGS[rd] = (int32_t) ( CURRENT_STATE.REGS[rt]) >> sa;
            break;
        case 0b001000:
            //JR
            if ( (CURRENT_STATE.REGS[rs] & 0x3) != 0) break; //Both low order bits of rs is non-zero
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;
        case 0b001001:
            //JALR
            if ( (CURRENT_STATE.REGS[rs] & 0x3) != 0) break; //Both low order bits of rs is non-zero
            NEXT_STATE.REGS[rd] = NEXT_STATE.PC;
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
        case 0b001100:
            //SYSCALL
            if (CURRENT_STATE.REGS[2] == 10) {
                RUN_BIT = 0;
            }
            break;
    }
}

void regimm_instruction(uint32_t currentInstr, uint32_t rs, uint32_t rt) {
    uint32_t offset = currentInstr & 0xFFFF;
    uint32_t result;
    uint32_t address;
    uint64_t doubleResult;
    uint32_t instr = (currentInstr >> 16) & 0x1F;
    switch ((currentInstr >> 16) & 0x1F) {
        case 0b00000:
            //BLTZ
            address = (int32_t) (offset << 16) >> 14;
            if ( (CURRENT_STATE.REGS[rs]>>31) == 1) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b00001:
            //BGEZ
            address = (int32_t) (offset << 16) >> 14;
            if ( (CURRENT_STATE.REGS[rs]>>31) == 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b10000:
            //BLTZAL
            address = (int32_t) (offset << 16) >> 14;
            NEXT_STATE.REGS[31] = NEXT_STATE.PC;
            if ( (CURRENT_STATE.REGS[rs]>>31) == 1) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b10001:
            //BGEZAL
            address = (int32_t) (offset << 16) >> 14;
            NEXT_STATE.REGS[31] = NEXT_STATE.PC;
            if ( (CURRENT_STATE.REGS[rs]>>31) == 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
    }
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t currentInstr = mem_read_32(CURRENT_STATE.PC);
    uint32_t top6bits = currentInstr >> 26;
    uint32_t rs = (currentInstr >> 21) & 0x1F;
    uint32_t rt = (currentInstr >> 16) & 0x1F;
    uint32_t immediate = currentInstr & 0xFFFF;
    uint32_t result;
    uint32_t temp;
    uint32_t address;
    lastMHI++; lastMLO++;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    switch (top6bits & 0x3F) {
        case 0b000000:
        //Special case -- instr is at the bottom instead
            special_instruction(currentInstr, rs, rt);
            break;
        case 0b001000:
            //ADDI
            immediate = (int32_t) ( (int32_t) immediate << 16) >> 16;
            result = (int32_t) CURRENT_STATE.REGS[rs] + (int32_t) immediate;
            if ( (result >> 31) ^ (result >> 30)) break; //overflow
            NEXT_STATE.REGS[rt] = result;
            break;
        case 0b001001:  
            //ADDIU
            immediate = (int32_t) (immediate << 16) >> 16;
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
            break;
        case 0b001100:
            //ANDI
            NEXT_STATE.REGS[rt] = immediate & CURRENT_STATE.REGS[rs];
            break;
        case 0b001101:
            //ORI
            result = immediate | CURRENT_STATE.REGS[rs];
            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] & 0xFFFF0000) | result;
            break;
        case 0b001110:
            //XORI
            result = CURRENT_STATE.REGS[rs] ^ immediate;
            NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] & 0xFFFF0000) | result;
            break;
        case 0b000010:
            //J
            result = (currentInstr & 0x3FFFFFF) << 2;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | result;
            break;
        case 0b000011:
            //JAL
            result = (currentInstr & 0x3FFFFFF) << 2;
            NEXT_STATE.REGS[31] = NEXT_STATE.PC;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | result;
            break;
        case 0b001111:
            //LUI
            NEXT_STATE.REGS[rt] = immediate << 16;
            break;
        case 0b100011:
            //LW
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            if ((address & 0b11) > 1) break;
            NEXT_STATE.REGS[rt] = mem_read_32(address);
            break;
        case 0b100001:
            //LH
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            if ((address & 0b1) == 1) break;
            result =  (int32_t) ((mem_read_32(address) & 0xFFFF) << 16 ) >> 16;
            NEXT_STATE.REGS[rt] = (int32_t) ((result & 0xFFFF) << 16) >> 16;
            break;
        case 0b100101:
            //LHU
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            if ((address & 0b1) == 1) break;
            result = mem_read_32(address) & 0xFFFF;
            NEXT_STATE.REGS[rt] = result;
            break;
        case 0b100000:
            //LB
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            result =  (int32_t) ((mem_read_32(address) & 0xFF) << 24 ) >> 24;
            result = mem_read_32(address) & 0xFF;
            NEXT_STATE.REGS[rt] = (int32_t) (result << 24) >> 24;
            break;
        case 0b100100:
            //LBU
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            result = mem_read_32(address) & 0xFF;
            NEXT_STATE.REGS[rt] = result;
            break;
        case 0b101000:
            //SB
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            result = CURRENT_STATE.REGS[rt] & 0xFF;
            mem_write_32(address, (mem_read_32(address) & 0xFFFFFF00) + result);
            break;
        case 0b101001:
            //SH
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            if ((address & 0x1) == 1) break; //address error
            result = CURRENT_STATE.REGS[rt] & 0xFFFF;
            mem_write_32(address, (mem_read_32(address) & 0xFFFF0000) + result);
            break;
        case 0b101011:
            //SW
            address = (((int32_t) (immediate << 16) >> 16) + CURRENT_STATE.REGS[rs]);
            if ((address & 0x3) > 0) break; //address error
            mem_write_32(address, CURRENT_STATE.REGS[rt]);
            break;
        case 0b001010:
            //SLTI
            result = 0;
            if ( (int32_t) CURRENT_STATE.REGS[rs] < (int32_t) ( (immediate << 16) >> 16)) {
                result = 1;
            }
            NEXT_STATE.REGS[rt] = result;
            break;
        case 0b001011:
            //SLTIU
            result = 0;
            if ( (CURRENT_STATE.REGS[rs] & 0x7FFFFFFFFF) < ( (immediate << 16) >> 16)) {
                result = 1;
            }
            NEXT_STATE.REGS[rt] = result;
            break;
        case 0b000100:
            //BEQ
            address = (int32_t) (immediate << 16) >> 14;
            if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b000111:
            //BGTZ
            address = (int32_t) (immediate << 16) >> 14;
            if ( (CURRENT_STATE.REGS[rs]>>31) == 0 && CURRENT_STATE.REGS[rs] != 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b000110:
            //BLEZ
            address = (int32_t) (immediate << 16) >> 14;
            if ( (CURRENT_STATE.REGS[rs]>>31) == 1 || CURRENT_STATE.REGS[rs] == 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b000101:
            //BNE
            address = (int32_t) (immediate << 16) >> 14;
            if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC = CURRENT_STATE.PC + address;
            }
            break;
        case 0b00001:
            //REGIMM [instr is at rt instead]
            regimm_instruction(currentInstr, rs, rt);
            break;
    }
}


