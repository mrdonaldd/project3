#include <stdint.h>
#include "sim.h"

#define WORD_SIZE 32
#define HALF_WORD_SIZE WORD_SIZE/2
#define BYTE_SIZE 8

#define OPCODE_SIZE 6
#define RS_SIZE 5
#define RT_SIZE 5
#define RD_SIZE 5
#define IMM_SIZE 16
#define BASE_SIZE 5
#define OFFSET_SIZE 16
#define SA_SIZE 5
#define SPECIAL_INTSRUCTION_SIZE 6
#define TARGET_SIZE 26
#define REGIMM_INSTRUCTION_SIZE 5

#define OPCODE_IDX 26
#define RS_IDX 21
#define RT_IDX 16
#define RD_IDX 11
#define IMM_IDX 0
#define BASE_IDX 21
#define OFFSET_IDX 0
#define SA_IDX 6
#define SPECIAL_INTSRUCTION_IDX 0
#define TARGET_IDX 0 
#define REGIMM_INSTRUCTION_IDX 16

#define READ_BITS(value, position, size) ((value) >> (position)) & ((1 << (size)) - 1)

/* These get the bits to identify things from the instruction */
#define CURRENT_OPCODE(value) (READ_BITS(value, OPCODE_IDX, OPCODE_SIZE))
#define SPECIAL_OPCODE(value) (READ_BITS(value, SPECIAL_INTSRUCTION_IDX, SPECIAL_INTSRUCTION_SIZE))
#define REGIMM_OPCODE(value) (READ_BITS(value, REGIMM_INSTRUCTION_IDX, REGIMM_INSTRUCTION_SIZE))

#define CURRENT_RT(value) (CURRENT_STATE.REGS[READ_BITS(value, RT_IDX, RT_SIZE)])
#define CURRENT_RS(value) (CURRENT_STATE.REGS[READ_BITS(value, RS_IDX, RS_SIZE)])
#define CURRENT_RD(value) (CURRENT_STATE.REGS[READ_BITS(value, RD_IDX, RD_SIZE)])
#define CURRENT_BASE(value) (CURRENT_STATE.REGS[READ_BITS(value, BASE_IDX, BASE_SIZE)])


#define CURRENT_IMM(value) (READ_BITS(value, IMM_IDX, IMM_SIZE))
#define CURRENT_OFFSET(value) (READ_BITS(value, OFFSET_IDX, OFFSET_SIZE))
#define CURRENT_SA(value) (READ_BITS(value, SA_IDX, SA_SIZE))
#define CURRENT_TARGET(value) (READ_BITS(value, TARGET_IDX, TARGET_SIZE))

#define NEXT_RS_BITS(value) (READ_BITS(value, RS_IDX, RS_SIZE))
#define NEXT_RT_BITS(value) (READ_BITS(value, RT_IDX, RT_SIZE))
#define NEXT_RD_BITS(value) (READ_BITS(value, RD_IDX, RD_SIZE))

uint32_t sign_extend_32(uint32_t value, uint32_t size){
    int signBit = READ_BITS(value, size - 1, 1);
    int newValue = value;

    if(signBit == 1){
        newValue = newValue | ((1 << (32 - size)) - 1) << size;
    } 

    return newValue;

}

void instruction_addi(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = sign_extend_32(CURRENT_IMM(instruction), 16) + CURRENT_RS(instruction);
}


void instruction_addiu(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = CURRENT_RS(instruction) + sign_extend_32(CURRENT_IMM(instruction), IMM_SIZE);
}

void instruction_slti(uint32_t instruction){
    int32_t imm = sign_extend_32(CURRENT_IMM(instruction), IMM_SIZE);
    int32_t rs = CURRENT_RS(instruction);
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = rs < imm;
}

void instruction_sltiu(uint32_t instruction){
    uint32_t imm = sign_extend_32(CURRENT_IMM(instruction), IMM_SIZE);
    uint32_t rs = CURRENT_RS(instruction);
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = rs < imm;
}

void instruction_andi(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = CURRENT_RS(instruction) & CURRENT_IMM(instruction);
}

void instruction_ori(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = CURRENT_RS(instruction) | CURRENT_IMM(instruction);
}

void instruction_xori(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = CURRENT_RS(instruction) ^ CURRENT_IMM(instruction);
}

void instruction_lui(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = ((uint32_t)CURRENT_IMM(instruction)) << 16;
}

void instruction_lb(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = mem_read_32(vaddr);
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = sign_extend_32(READ_BITS(word, 0, BYTE_SIZE), BYTE_SIZE);
}

void instruction_lh(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = mem_read_32(vaddr);
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = sign_extend_32(READ_BITS(word, 0, HALF_WORD_SIZE), HALF_WORD_SIZE);
}

void instruction_lw(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
}

void instruction_lbu(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = mem_read_32(vaddr);
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = READ_BITS(word, 0, BYTE_SIZE);
}

void instruction_lhu(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = mem_read_32(vaddr);
    NEXT_STATE.REGS[NEXT_RT_BITS(instruction)] = READ_BITS(word, 0, HALF_WORD_SIZE);
}

void instruction_sb(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = CURRENT_RT(instruction);
    mem_write_32(vaddr, READ_BITS(word, 0, BYTE_SIZE)); 
}

void instruction_sh(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = CURRENT_RT(instruction);
    mem_write_32(vaddr, READ_BITS(word, 0, HALF_WORD_SIZE)); 
}

void instruction_sw(uint32_t instruction){
    uint32_t vaddr = CURRENT_BASE(instruction) + sign_extend_32(CURRENT_OFFSET(instruction), OFFSET_SIZE);
    uint32_t word = CURRENT_RT(instruction);
    mem_write_32(vaddr, word); 
}

void instruction_sll(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RT(instruction) << CURRENT_SA(instruction);
}

void instruction_srl(uint32_t instruction){
    uint32_t word = CURRENT_RT(instruction);
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = word >> CURRENT_SA(instruction);
}

void instruction_sra(uint32_t instruction){
    uint32_t cool_bits = CURRENT_RS(instruction);
    int rt = CURRENT_RT(instruction);
    if(rt < 0){
        NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = ~(~rt >> cool_bits);
    }
    else{
        NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = rt >> cool_bits;
    }
}

void instruction_sllv(uint32_t instruction){
    int shift = READ_BITS(CURRENT_RS(instruction), 0, 5);
    uint32_t word = CURRENT_RT(instruction);
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = word << shift;
}

void instruction_srav(uint32_t instruction){
    uint32_t cool_bits = READ_BITS(CURRENT_RS(instruction), 0, 5);
    int rt = CURRENT_RT(instruction);
    if(rt < 0){
        NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = ~(~rt >> cool_bits);
    }
    else{
        NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = rt >> cool_bits;
    }
}

void instruction_srlv(uint32_t instruction){
    uint32_t shift = CURRENT_RS(instruction) & 0x1F;
    uint32_t shifted = CURRENT_RT(instruction) >> shift; 
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RT(instruction) >> shift;
}

void instruction_add(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) + CURRENT_RT(instruction);
}

void instruction_addu(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) + CURRENT_RT(instruction);
}

void instruction_sub(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) - CURRENT_RT(instruction);
}

void instruction_and(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) & CURRENT_RT(instruction);
}

void instruction_or(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) | CURRENT_RT(instruction);
}

void instruction_xor(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) ^ CURRENT_RT(instruction);
}

void instruction_nor(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = ~(CURRENT_RS(instruction) | CURRENT_RT(instruction));
}

void instruction_slt(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = (int32_t)CURRENT_RS(instruction) < (int32_t)CURRENT_RT(instruction);
} 

void instruction_sltu(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_RS(instruction) < CURRENT_RT(instruction);
}

void instruction_mult(uint32_t instruction){
    int32_t rs_data = (int32_t) CURRENT_RS(instruction);
    int32_t rt_data = (int32_t) CURRENT_RT(instruction);
    int64_t result = (int64_t)rs_data * (int64_t)rt_data;

    NEXT_STATE.HI = READ_BITS(result, 32, 32);
    NEXT_STATE.LO = READ_BITS(result, 0, 32);
}

void instruction_multu(uint32_t instruction){
    uint32_t rs_data = (uint32_t) CURRENT_RS(instruction);
    uint32_t rt_data = (uint32_t) CURRENT_RT(instruction);
    uint64_t result = (uint64_t)rs_data * (uint64_t)rt_data;

    NEXT_STATE.HI = READ_BITS(result, 32, 32);
    NEXT_STATE.LO = READ_BITS(result, 0, 32);
}

void instruction_mfhi(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_STATE.HI;
}

void instruction_mflo(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_STATE.LO;
}

void instruction_mthi(uint32_t instruction){
    NEXT_STATE.HI = CURRENT_RS(instruction);
}

void instruction_mtlo(uint32_t instruction){
    NEXT_STATE.LO = CURRENT_RS(instruction);
}

void instruction_div(uint32_t instruction){
    int32_t rs_data = (int32_t) CURRENT_RS(instruction);
    int32_t rt_data = (int32_t) CURRENT_RT(instruction);

    int32_t quotient = rs_data / rt_data;
    int32_t remainder = rs_data % rt_data;

    NEXT_STATE.LO = (uint32_t) quotient;
    NEXT_STATE.HI = (uint32_t) remainder;
}

void instruction_divu(uint32_t instruction){
    uint32_t rs_data = (uint32_t) CURRENT_RS(instruction);
    uint32_t rt_data = (uint32_t) CURRENT_RT(instruction);

    uint32_t quotient = rs_data / rt_data;
    uint32_t remainder = rs_data % rt_data;

    NEXT_STATE.LO = quotient;
    NEXT_STATE.HI = remainder;
}

void instruction_syscall(uint32_t instruction){
    if (CURRENT_STATE.REGS[2] == 10){
        RUN_BIT = 0;
    }
}

void instruction_j(uint32_t instruction){
    uint32_t target = CURRENT_TARGET(instruction) << 2;
    NEXT_STATE.PC = target;
}

void instruction_jal(uint32_t instruction){
    uint32_t target = CURRENT_TARGET(instruction) << 2;
    NEXT_STATE.PC = target;
    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
}

void instruction_beq(uint32_t instruction){
    if(CURRENT_RS(instruction) == CURRENT_RT(instruction)){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
}

void instruction_bne(uint32_t instruction){
    if(CURRENT_RS(instruction) != CURRENT_RT(instruction)){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
}

void instruction_blez(uint32_t instruction){
    if(CURRENT_RS(instruction) == 0 || CURRENT_RS(instruction) >> 31 ){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
}

void instruction_bgtz(uint32_t instruction){
    if(CURRENT_RS(instruction) != 0 && ((CURRENT_RS(instruction) >> 31) == 0)){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
}

void instruction_bltz(uint32_t instruction){
    if(CURRENT_RS(instruction) >> 31 ){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
}

void instruction_bgez(uint32_t instruction){
    if(CURRENT_RS(instruction) >> 31 == 0){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
}

void instruction_bltzal(uint32_t instruction){
    if(CURRENT_RS(instruction) >> 31 ){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
}

void instruction_bgezal(uint32_t instruction){
    if(CURRENT_RS(instruction) == 0 || !CURRENT_RS(instruction) >> 31){
        uint32_t target = sign_extend_32(CURRENT_OFFSET(instruction) << 2, 18) + CURRENT_STATE.PC;
        NEXT_STATE.PC = target;
    }
    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
}

void instruction_jr(uint32_t instruction){
    NEXT_STATE.PC = CURRENT_RS(instruction);
}


void instruction_jalr(uint32_t instruction){
    NEXT_STATE.REGS[NEXT_RD_BITS(instruction)] = CURRENT_STATE.PC + 4;
    NEXT_STATE.PC = CURRENT_RS(instruction);
}



void regimm_instruction_handler(uint32_t instruction){
    switch(REGIMM_OPCODE(instruction)){
        case BLTZ:
            instruction_bltz(instruction); break;
        case BGEZ:
            instruction_bgez(instruction); break;
        case BLTZAL:
            instruction_bltzal(instruction); break;
        case BGEZAL:
            instruction_bgezal(instruction); break;
    };
}



void special_instruction_handler(uint32_t instruction){
    switch(SPECIAL_OPCODE(instruction)){
        case SLL:
            instruction_sll(instruction); break;
        case SRL:
            instruction_srl(instruction); break;
        case SRA:
            instruction_sra(instruction); break;
        case SLLV:
            instruction_sllv(instruction); break;
        case SRAV: 
            instruction_srav(instruction); break;
        case SRLV:
            instruction_srlv(instruction); break;
        case ADDU:
        case ADD:
            instruction_add(instruction); break;
        case SUBU:
        case SUB:
            instruction_sub(instruction); break;
        case AND:
            instruction_and(instruction); break;
        case OR:
            instruction_or(instruction); break;
        case XOR:
            instruction_xor(instruction); break;
        case NOR:
            instruction_nor(instruction); break;
        case SLT:
            instruction_slt(instruction); break;
        case MULTU:
            instruction_multu(instruction); break;
        case MULT:
            instruction_mult(instruction); break;
        case MFHI:
            instruction_mfhi(instruction); break;
        case MFLO:
            instruction_mflo(instruction); break;
        case MTHI:
            instruction_mthi(instruction); break;
        case MTLO:
            instruction_mtlo(instruction); break;
        case DIVU:
            instruction_divu(instruction); break;
        case DIV:
            instruction_div(instruction); break;
        case SYSCALL:
            instruction_syscall(instruction); break;
        case JR:
            instruction_jr(instruction); break;
        case JALR:
            instruction_jalr(instruction); break;
    }
}

// void test_sign_extension(uint32_t instruction){
//     printf("%x\n", instruction);
//     printf("%x\n", EXTEND_INT32(instruction));
//     printf("%x\n", sign_extend_32(instruction, 16));
// }

void process_instruction(){
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    switch(CURRENT_OPCODE(instruction)){
        case J:
            instruction_j(instruction); break;
        case JAL:
            instruction_jal(instruction); break;
        case BEQ:
            instruction_beq(instruction); break;
        case BNE:
            instruction_bne(instruction); break;
        case BLEZ:
            instruction_blez(instruction); break;
        case BGTZ:
            instruction_bgtz(instruction); break;
        case ADDI:
            instruction_addi(instruction); break;
        case ADDIU:
            instruction_addiu(instruction); break;
        case SLTI:
            instruction_slti(instruction); break;
        case SLTIU:
            instruction_sltiu(instruction); break;
        case ANDI:
            instruction_andi(instruction); break;
        case ORI:
            instruction_ori(instruction); break;
        case XORI:
            instruction_xori(instruction); break;
        case LUI:
            instruction_lui(instruction); break;
        case LB:
            instruction_lb(instruction); break;
        case LH:
            instruction_lh(instruction); break;
        case LW:
            instruction_lw(instruction); break;
        case LBU:
            instruction_lbu(instruction); break;
        case LHU:
            instruction_lhu(instruction); break;
        case SB:
            instruction_sb(instruction); break;
        case SH:
            instruction_sh(instruction); break;
        case SW:
            instruction_sw(instruction); break;
        case SPECIAL:
            special_instruction_handler(instruction); break;
        case REGIMM:
            regimm_instruction_handler(instruction); break;
        default:
            printf("Unknown instruction\n");
            break;
    }

    return;
}
