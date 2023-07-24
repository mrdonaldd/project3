#include <stdio.h>
#include "shell.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    uint32_t insReg;
    uint8_t opcode;
    uint32_t jumpTarget;
    uint8_t rt;
    uint8_t rs;
    int32_t offset;
    int32_t imm;
    uint8_t base;
    uint32_t virAddr;
    uint8_t opcodeWithREG;
    uint8_t opcodeWithSPE;
    uint8_t rd;
    uint8_t sa;
    int64_t multAns;

    insReg = mem_read_32(CURRENT_STATE.PC);
    opcode = insReg >> 26;
    
    switch (opcode)
    {
    case 2: //J 
        jumpTarget = insReg&0x03FFFFFF; // bitmask to get rid of opcode
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (jumpTarget << 2);
        break;

    case 3: //JAL
        jumpTarget = insReg&0x03FFFFFF; // bitmask to get rid of opcode
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (jumpTarget << 2);
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        break;
    
    case 4: //BEQ
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 14;
        if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]){
            NEXT_STATE.PC = CURRENT_STATE.PC + offset; }
        else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; } 
        break;

    case 5: //BNE
        rs = (insReg&0x03E00000) >> 21;
        rt = (insReg&0x001F0000) >> 16;
        offset = (((int32_t) insReg&0x0000FFFF) << 16) >> 14;
        if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]){
            NEXT_STATE.PC = CURRENT_STATE.PC + offset; }
        else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; } 
        break;

    case 6: //BLEZ
        rs = (insReg & 0x03E00000) >> 21;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 14;
        if ((CURRENT_STATE.REGS[rs] & 0x8000000) || CURRENT_STATE.REGS[rs] == 0) { //rs has sign bit or == 0
            NEXT_STATE.PC = CURRENT_STATE.PC + offset; } 
        else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
        break;
    
    case 7: //BGTZ
        rs = (insReg & 0x03E00000) >> 21;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 14;
        if ((CURRENT_STATE.REGS[rs] & 0x8000000) == 0 && CURRENT_STATE.REGS[rs]) { //rs has sign bit clear and == 0
            NEXT_STATE.PC = CURRENT_STATE.PC + offset; } 
        else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
        break;

    case 8: //ADDI
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 9: //ADDIU
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break; 

    case 10: //SLTI
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;            
        if ((int32_t) CURRENT_STATE.REGS[rs] < imm) { NEXT_STATE.REGS[rt] = (uint32_t) 1; } 
        else { NEXT_STATE.REGS[rt] = (uint32_t) 0; }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 11: //SLTIU
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        if (CURRENT_STATE.REGS[rs] < (uint32_t) imm) { CURRENT_STATE.REGS[rt] = (uint32_t) 1;} 
        else { CURRENT_STATE.REGS[rt] = (uint32_t) 0; }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;      
        break; 

    case 12: //ANDI
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = insReg & 0x0000FFFF;
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 13: //ORI
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = insReg & 0x0000FFFF;
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break; 

    case 14: //XORI
        rs = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        imm = insReg & 0x0000FFFF;
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ imm;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break; 

    case 15: //LUI
        rt = (insReg & 0x001F0000) >> 16;
        imm = (imm & 0x0000FFFF) << 16;
        NEXT_STATE.REGS[rt] = imm;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 32: //LB
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        NEXT_STATE.REGS[rt] = (int32_t) (int8_t) mem_read_32(virAddr);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 33: //LH
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        NEXT_STATE.REGS[rt] = (int32_t) (int16_t) mem_read_32(virAddr);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 35: //LW
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        NEXT_STATE.REGS[rt] = mem_read_32(virAddr);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 36: //LBU
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        NEXT_STATE.REGS[rt] = (uint8_t) mem_read_32(virAddr);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;       

    case 37: //LHU
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        NEXT_STATE.REGS[rt] = (uint16_t) mem_read_32(virAddr);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    case 40: //SB
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        mem_write_32(virAddr, (uint8_t) CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break; 

    case 41: //SH
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        mem_write_32(virAddr, (uint16_t) CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break; 

    case 43: //SW
        base = (insReg & 0x03E00000) >> 21;
        rt = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 16;
        virAddr = CURRENT_STATE.REGS[base] + offset;
        mem_write_32(virAddr, CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;  

    case 1: //REGIMM
        rs = (insReg & 0x03E00000) >> 21;
        opcodeWithREG = (insReg & 0x001F0000) >> 16;
        offset = (((int32_t) insReg & 0x0000FFFF) << 16) >> 14;
        switch (opcodeWithREG) 
        {
        case 0: //BLTZ
            if (CURRENT_STATE.REGS[rs] >> 31) { 
                NEXT_STATE.PC = CURRENT_STATE.PC + offset; } 
            else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
            break;

        case 1: //BGEZ
            if (CURRENT_STATE.REGS[rs] >> 31 == 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset; } 
            else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
            break; 

        case 16: //BLTZAL
            CURRENT_STATE.REGS[rs] = CURRENT_STATE.PC + 4;
            if (CURRENT_STATE.REGS[rs] >> 31) {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset; }
            else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }    
            break;

        case 17: //BGEZAL
            CURRENT_STATE.REGS[rs] = CURRENT_STATE.PC + 4;
            if (CURRENT_STATE.REGS[rs] >> 31 == 0) {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset; } 
            else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
            break; 

        default:
            break;     
        }

    case 0: //SPECIAL
        opcodeWithSPE = insReg & 0x0000003F;    
        switch (opcodeWithSPE)
        {  
       
        case 0: //SLL
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            sa = (insReg & 0x000007C0) >> 6;  
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case 2: //SRL
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            sa = (insReg & 0x000007C0) >> 6;  
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case 3: //SRA
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            sa = (insReg & 0x000007C0) >> 6;  
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = ((int32_t) CURRENT_STATE.REGS[rt]) >> sa;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;            
            break; 

        case 4: //SLLV
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            sa = (insReg & 0x000007C0) >> 6;  
            rs = (insReg & 0x03E00000) >> 21;          
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x0000001F);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case 6: //SRLV
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            sa = (insReg & 0x000007C0) >> 6;  
            rs = (insReg & 0x03E00000) >> 21;              
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x0000001F);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        case 7: //SRAV
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            sa = (insReg & 0x000007C0) >> 6;  
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = ((int32_t) CURRENT_STATE.REGS[rt]) >> (CURRENT_STATE.REGS[rs] & 0x0000001F);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
            break;     

        case 8: //JR
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;

        case 9: //JALR
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;          
            NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;          

        case 32: //ADD
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11; 
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = (int32_t) CURRENT_STATE.REGS[rs] + (int32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 33: //ADDU
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = (int32_t) CURRENT_STATE.REGS[rs] + (int32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 34: //SUB
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = (int32_t) CURRENT_STATE.REGS[rs] - (int32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;            
            break; 

        case 35: //SUBU
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = (int32_t) CURRENT_STATE.REGS[rs] - (int32_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 36: //AND
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 37: //OR
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;        
            break; 

        case 38: //XOR
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 39: //NOR
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 42: //SLT
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            if ((int32_t) CURRENT_STATE.REGS[rs] < (int32_t) CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.REGS[rd] = 1; } 
            else { NEXT_STATE.REGS[rd] = 0; }
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 43: //SLTU
            rt = (insReg & 0x001F0000) >> 16;
            rd = (insReg & 0x0000F800) >> 11;
            rs = (insReg & 0x03E00000) >> 21;  
            if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.REGS[rd] = 1; } 
            else { NEXT_STATE.REGS[rd] = 0; }
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;         

        case 24: //MULT
            rt = (insReg & 0x001F0000) >> 16;
            rs = (insReg & 0x03E00000) >> 21;  
            multAns = (int64_t) CURRENT_STATE.REGS[rs] * (int64_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = multAns & 0xFFFFFFFF00000000;
            NEXT_STATE.LO = multAns & 0x00000000FFFFFFFF;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 16: //MFHI
            rd = (insReg & 0x0000F800) >> 11;
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 18: //MFLO
            rd = (insReg & 0x0000F800) >> 11;  
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 17: //MTHI
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rs] = CURRENT_STATE.HI;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 19: //MTLO
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.REGS[rs] = CURRENT_STATE.LO;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 25: //MULTU
            rt = (insReg & 0x001F0000) >> 16;
            rs = (insReg & 0x03E00000) >> 21;  
            multAns = (uint64_t) CURRENT_STATE.REGS[rs] * (uint64_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = multAns & 0xFFFFFFFF00000000;
            NEXT_STATE.LO = multAns & 0x00000000FFFFFFFF;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;         

        case 26: //DIV
            rt = (insReg & 0x001F0000) >> 16;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.HI = (int64_t) CURRENT_STATE.REGS[rs] % (int64_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (int64_t) CURRENT_STATE.REGS[rs] / (int64_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;

            break; 

        case 27: //DIVU
            rt = (insReg & 0x001F0000) >> 16;
            rs = (insReg & 0x03E00000) >> 21;  
            NEXT_STATE.HI = (uint64_t) CURRENT_STATE.REGS[rs] % (uint64_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (uint64_t) CURRENT_STATE.REGS[rs] / (uint64_t) CURRENT_STATE.REGS[rt];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break; 

        case 12: //SYSCALL
            if (CURRENT_STATE.REGS[2] == 0x0000000A) { RUN_BIT = 0; } 
            else { NEXT_STATE.PC = CURRENT_STATE.PC + 4; }
            break;
        
        default:

            break;
        }

    default:

        break;
    }
}
