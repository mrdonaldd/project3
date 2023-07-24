#include "sim.h"

void process_instruction()
{
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    int opcode = get_opcode(instruction);
    switch (opcode)
    {
    case OPCODE_SPECIAL:
        switch (get_r_funct(instruction))
        {
        case FUNCT_ADD:
            handle_add(instruction);
            break;
        case FUNCT_ADDU:
            handle_addu(instruction);
            break;
        case FUNCT_AND:
            handle_and(instruction);
            break;
        case FUNCT_DIV:
            handle_div(instruction);
            break;
        case FUNCT_DIVU:
            handle_divu(instruction);
            break;
        case FUNCT_JALR:
            handle_jalr(instruction);
            break;
        case FUNCT_JR:
            handle_jr(instruction);
            break;
        case FUNCT_MFHI:
            handle_mfhi(instruction);
            break;
        case FUNCT_MFLO:
            handle_mflo(instruction);
            break;
        case FUNCT_MTHI:
            handle_mthi(instruction);
            break;
        case FUNCT_MTLO:
            handle_mtlo(instruction);
            break;
        case FUNCT_MULT:
            handle_mult(instruction);
            break;
        case FUNCT_MULTU:
            handle_multu(instruction);
            break;
        case FUNCT_NOR:
            handle_nor(instruction);
            break;
        case FUNCT_OR:
            handle_or(instruction);
            break;
        case FUNCT_SLL:
            handle_sll(instruction);
            break;
        case FUNCT_SLLV:
            handle_sllv(instruction);
            break;
        case FUNCT_SLT:
            handle_slt(instruction);
            break;
        case FUNCT_SLTU:
            handle_sltu(instruction);
            break;
        case FUNCT_SRA:
            handle_sra(instruction);
            break;
        case FUNCT_SRAV:
            handle_srav(instruction);
            break;
        case FUNCT_SRL:
            handle_srl(instruction);
            break;
        case FUNCT_SRLV:
            handle_srlv(instruction);
            break;
        case FUNCT_SUB:
            handle_sub(instruction);
            break;
        case FUNCT_SUBU:
            handle_subu(instruction);
            break;
        case FUNCT_SYSCALL:
            handle_syscall(instruction);
            break;
        case FUNCT_XOR:
            handle_xor(instruction);
            break;
        default:
            printf("ERROR: unsupported instruction\n");
            RUN_BIT = 0;
        }
        break;
    case OPCODE_REGIMM:
        switch (get_i_rt(instruction))
        {
        case RT_BGEZ:
            handle_bgez(instruction);
            break;
        case RT_BGEZAL:
            handle_bgezal(instruction);
            break;
        case RT_BLTZ:
            handle_bltz(instruction);
            break;
        case RT_BLTZAL:
            handle_bltzal(instruction);
            break;
        default:
            printf("ERROR: unsupported instruction\n");
            RUN_BIT = 0;
        }
        break;
    case OPCODE_ADDI:
        handle_addi(instruction);
        break;
    case OPCODE_ADDIU:
        handle_addiu(instruction);
        break;
    case OPCODE_ANDI:
        handle_andi(instruction);
        break;
    case OPCODE_BEQ:
        handle_beq(instruction);
        break;
    case OPCODE_BGTZ:
        handle_bgtz(instruction);
        break;
    case OPCODE_BLEZ:
        handle_blez(instruction);
        break;
    case OPCODE_BNE:
        handle_bne(instruction);
        break;
    case OPCODE_LB:
        handle_lb(instruction);
        break;
    case OPCODE_LBU:
        handle_lbu(instruction);
        break;
    case OPCODE_LH:
        handle_lh(instruction);
        break;
    case OPCODE_LHU:
        handle_lhu(instruction);
        break;
    case OPCODE_LUI:
        handle_lui(instruction);
        break;
    case OPCODE_LW:
        handle_lw(instruction);
        break;
    case OPCODE_ORI:
        handle_ori(instruction);
        break;
    case OPCODE_SB:
        handle_sb(instruction);
        break;
    case OPCODE_SLTI:
        handle_slti(instruction);
        break;
    case OPCODE_SLTIU:
        handle_sltiu(instruction);
        break;
    case OPCODE_SH:
        handle_sh(instruction);
        break;
    case OPCODE_SW:
        handle_sw(instruction);
        break;
    case OPCODE_XORI:
        handle_xori(instruction);
        break;
    case OPCODE_J:
        handle_j(instruction);
        break;
    case OPCODE_JAL:
        handle_jal(instruction);
        break;
    default:
        printf("ERROR: unsupported instruction\n");
        RUN_BIT = 0;
    }
}

int handle_addi(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t immediate = (int32_t)get_i_immediate(instruction);

    /* note: overflow exception handling is omitted */
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_addiu(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t immediate = (int32_t)get_i_immediate(instruction);

    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_andi(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rs(instruction);

    uint32_t immediate = (uint32_t)get_i_immediate(instruction);

    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & immediate;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_beq(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    /* get offset, shifted left 2 bits, and sign-extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* define the condition: contents of source and target registers are equal */
    int condition = CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt];

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_bgtz(uint32_t instruction)
{
    int rs = get_i_rs(instruction);

    /* get offset, shift left 2 bits, and sign-extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* define the condition: contents of source register greater than zero */
    int condition = CURRENT_STATE.REGS[rs] > 0;

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_blez(uint32_t instruction)
{
    int rs = get_i_rs(instruction);

    /* get offset, shift left 2 bits, and sign-extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* define the condition: contents of source register less than or equal to zero */
    int condition = CURRENT_STATE.REGS[rs] <= 0;

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_bne(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    /* get offset, shift left 2 bits, and sign extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* define the condition: contents of source and taregt registers are not equal */
    int condition = CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt];

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_lb(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* load 32 bit word at address, mask off all but low byte */
    int8_t byte = (int8_t)(mem_read_32(address) & 0x000000FF);

    NEXT_STATE.REGS[rt] = (int32_t)byte;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_lbu(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* load 32 bit word at address, mask off all but low byte */
    uint8_t byte = (uint8_t)(mem_read_32(address) & 0x000000FF);

    NEXT_STATE.REGS[rt] = (uint32_t)byte;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_lh(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* load 32 bit word at address, mask off all but low halfword */
    int16_t halfword = (int16_t)(mem_read_32(address) & 0x0000FFFF);

    NEXT_STATE.REGS[rt] = (int32_t)halfword;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_lhu(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* load 32 bit word at address, mask off all but low halfword */
    uint16_t halfword = (uint16_t)(mem_read_32(address) & 0x0000FFFF);

    NEXT_STATE.REGS[rt] = (uint32_t)halfword;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_lui(uint32_t instruction)
{
    int rt = get_i_rt(instruction);

    /* get immediate, left shift 16 bits */
    int32_t immediate = (int32_t)(get_i_immediate(instruction) << 16);

    NEXT_STATE.REGS[rt] = immediate;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_lw(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* add offset to contents of base register to form address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* load memory contents at effective address into target register */
    NEXT_STATE.REGS[rt] = mem_read_32(address);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_ori(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    uint32_t immediate = (uint32_t)get_i_immediate(instruction);

    /* bitwise OR contents of source register to immediate to form result and store result in target register */
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | immediate;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sb(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* isolate the low byte of target register */
    uint32_t byte = (CURRENT_STATE.REGS[rt] & 0x000000FF);

    /* load current data at memory location and mask off low byte */
    uint32_t mem = (mem_read_32(address) & 0xFFFFFF00);

    /* store contents of target register at memory location specified by address */
    mem_write_32(address, mem | byte);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_slti(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t immediate = (int32_t)get_i_immediate(instruction);

    /* define the condition: signed contents of source register are less than immediate */
    int condition = (int32_t)CURRENT_STATE.REGS[rs] < immediate;

    /* if the conditionis satisfied, contents of target register set to 1; otherwise 0 */
    NEXT_STATE.REGS[rt] = condition ? (uint32_t)1 : (uint32_t)0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sltiu(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    uint32_t immediate = (uint32_t)get_i_immediate(instruction);

    /* define the condition: signed contents of source register are less than immediate */
    int condition = CURRENT_STATE.REGS[rs] < immediate;

    /* if the conditionis satisfied, contents of target register set to 1; otherwise 0 */
    NEXT_STATE.REGS[rt] = condition ? (uint32_t)1 : (uint32_t)0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sh(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* isolate the low halfword of target register */
    uint32_t halfword = (CURRENT_STATE.REGS[rt] & 0x0000FFFF);

    /* load current data at memory location and mask off low halfword */
    uint32_t mem = (mem_read_32(address) & 0xFFFF0000);

    /* store contents of target register at memory location specified by address */
    mem_write_32(address, mem | halfword);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sw(uint32_t instruction)
{
    int base = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    int32_t offset = (int32_t)get_i_immediate(instruction);

    /* combine contents of base register and offset to form virtual address */
    uint32_t address = CURRENT_STATE.REGS[base] + offset;

    /* store contents of target register at memory location specified by address */
    mem_write_32(address, CURRENT_STATE.REGS[rt]);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_xori(uint32_t instruction)
{
    int rs = get_i_rs(instruction);
    int rt = get_i_rt(instruction);

    uint32_t immediate = (uint32_t)get_i_immediate(instruction);

    /* bitwise XOR contents of source register to immediate to form result and store result in target register */
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ immediate;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_bgez(uint32_t instruction)
{
    int rs = get_i_rs(instruction);

    /* get offset, shift left 2 bits, and sign extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* define the condition: contents of source register greater than or equal to zero */
    int condition = CURRENT_STATE.REGS[rs] >= 0;

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_bgezal(uint32_t instruction)
{
    int rs = get_i_rs(instruction);

    /* get offset, shift left 2 bits, and sign extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* address of next instruction stored in link register */
    /* note: PC + 4 is stored instead PC + 8 as specified by the project instruction */
    NEXT_STATE.REGS[REG_LINK] = CURRENT_STATE.PC + 4;

    /* define the condition: contents of source register greater than or equal to zero */
    int condition = CURRENT_STATE.REGS[rs] >= 0;

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_bltz(uint32_t instruction)
{
    int rs = get_i_rs(instruction);

    /* get offset, shift left 2 bits, and sign extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* define the condition: contents of source register less than zero */
    int condition = CURRENT_STATE.REGS[rs] < 0;

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_bltzal(uint32_t instruction)
{
    int rs = get_i_rs(instruction);

    /* get offset, shift left 2 bits, and sign extend */
    int32_t offset = (int32_t)(get_i_immediate(instruction) << 2);

    /* address of next instruction stored in link register */
    /* note: PC + 4 is stored instead PC + 8 as specified by the project instruction */
    NEXT_STATE.REGS[REG_LINK] = CURRENT_STATE.PC + 4;

    /* define the condition: contents of source register less than zero */
    int condition = CURRENT_STATE.REGS[rs] < 0;

    /* if the conditionis satisfied, branch is taken; otherwise not taken */
    NEXT_STATE.PC = CURRENT_STATE.PC + (condition ? offset : 4);

    return SUCCESS;
}

int handle_j(uint32_t instruction)
{
    /* get target address and shift left by 2 bits */
    uint32_t target = (get_j_target(instruction) << 2);

    /* get high-order bits of current address */
    uint32_t current_address = CURRENT_STATE.PC & MASK_HIGH_ORDER;

    /* update the program counter to point to the combined address unconditionally */
    NEXT_STATE.PC = current_address | target;

    return SUCCESS;
}

int handle_jal(uint32_t instruction)
{
    /* get target address and shift left by 2 bits */
    uint32_t target = (get_j_target(instruction) << 2);

    /* get high-order bits of current address */
    uint32_t current_address = (CURRENT_STATE.PC & MASK_HIGH_ORDER);

    /* address of next instruction stored in link register */
    /* note: PC + 4 is stored instead PC + 8 as specified by the project instruction */
    NEXT_STATE.REGS[REG_LINK] = CURRENT_STATE.PC + 4;

    /* update the program counter unconditionally */
    NEXT_STATE.PC = current_address | target;

    return SUCCESS;
}

int handle_add(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    int32_t source = (int32_t)CURRENT_STATE.REGS[rs];
    int32_t target = (int32_t)CURRENT_STATE.REGS[rt];

    NEXT_STATE.REGS[rd] = source + target;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_addu(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    int32_t source = (int32_t)CURRENT_STATE.REGS[rs];
    int32_t target = (int32_t)CURRENT_STATE.REGS[rt];

    /* note: overflow exception handling is omitted */
    NEXT_STATE.REGS[rd] = source + target;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_and(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_div(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);

    int32_t source = (int32_t)CURRENT_STATE.REGS[rs];
    int32_t target = (int32_t)CURRENT_STATE.REGS[rt];

    /* low word of result stored in special register LO */
    NEXT_STATE.LO = source / target;

    /* high word of result stored in special register HI */
    NEXT_STATE.HI = source % target;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_divu(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);

    uint32_t source = (uint32_t)CURRENT_STATE.REGS[rs];
    uint32_t target = (uint32_t)CURRENT_STATE.REGS[rt];

    /* low word of result stored in special register LO */
    NEXT_STATE.LO = source / target;

    /* high word of result stored in special register HI */
    NEXT_STATE.HI = source % target;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_jalr(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rd = get_r_rd(instruction);

    /* address of next sequential instruction stored in destination register */
    /* note: PC + 4 is stored instead PC + 8 as specified by the project instruction */
    NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;

    /* update the program counter to point to the address stored in source register unconditionally */
    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];

    return SUCCESS;
}

int handle_jr(uint32_t instruction)
{
    int rs = get_r_rs(instruction);

    /* update the program counter to point to the address stored in source register unconditionally */
    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];

    return SUCCESS;
}

int handle_mfhi(uint32_t instruction)
{
    int rd = get_r_rd(instruction);

    /* contents of special register HI loaded into destination register */
    NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_mflo(uint32_t instruction)
{
    int rd = get_r_rd(instruction);

    /* contents of special register LO loaded into destination register */
    NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_mthi(uint32_t instruction)
{
    int rs = get_r_rs(instruction);

    /* contents of source register loaded into special register HI */
    NEXT_STATE.HI = CURRENT_STATE.REGS[rs];

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_mtlo(uint32_t instruction)
{
    int rs = get_r_rs(instruction);

    /* contents of source register loaded into special register LO */
    NEXT_STATE.LO = CURRENT_STATE.REGS[rs];

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_mult(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);

    int64_t source = (int64_t)CURRENT_STATE.REGS[rs];
    int64_t target = (int64_t)CURRENT_STATE.REGS[rt];

    int64_t result = source * target;

    /* high word of result stored in special register HI */
    NEXT_STATE.HI = (result >> 32) & 0xFFFFFFFF;

    /* low word of result stored in special register LO */
    NEXT_STATE.LO = (result >> 0) & 0xFFFFFFFF;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_multu(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);

    uint64_t source = (uint64_t)CURRENT_STATE.REGS[rs];
    uint64_t target = (uint64_t)CURRENT_STATE.REGS[rt];

    uint64_t result = source * target;

    /* high word of result stored in special register HI */
    NEXT_STATE.HI = (result >> 32) & 0xFFFFFFFF;

    /* low word of result stored in special register LO */
    NEXT_STATE.LO = (result >> 0) & 0xFFFFFFFF;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_nor(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_or(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sll(uint32_t instruction)
{
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);
    int sa = get_r_shamt(instruction);

    /* shift contents of target register left by sa bits to form result and store result in destination register */
    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] << sa);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sllv(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    int s = (CURRENT_STATE.REGS[rs] & 0x000001F);

    /* shift contents of target register left by s bits to form result and store result in destination register */
    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] << s);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_slt(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    int32_t source = (int32_t)CURRENT_STATE.REGS[rs];
    int32_t target = (int32_t)CURRENT_STATE.REGS[rt];

    /* define the condition: contents of source less than contents of target */
    int condition = source < target;

    /* if the conditionis satisfied, contents of destination register set to 1; otherwise 0 */
    NEXT_STATE.REGS[rd] = condition ? (uint32_t)1 : (uint32_t)0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sltu(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    uint32_t source = (uint32_t)CURRENT_STATE.REGS[rs];
    uint32_t target = (uint32_t)CURRENT_STATE.REGS[rt];

    /* define the condition: contents of source less than contents of target */
    int condition = source < target;

    /* if the conditionis satisfied, contents of destination register set to 1; otherwise 0 */
    NEXT_STATE.REGS[rd] = condition ? (uint32_t)1 : (uint32_t)0;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sra(uint32_t instruction)
{
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);
    int sa = get_r_shamt(instruction);

    uint32_t mask = ((~((int32_t)0)) << (32 - sa));
    uint32_t result = CURRENT_STATE.REGS[rt] >> sa;

    /* bitwise OR result with sign bit of original register content */
    NEXT_STATE.REGS[rd] = result | mask;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_srav(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    int s = (CURRENT_STATE.REGS[rs] & 0x000001F);

    uint32_t mask = (~((int32_t)0)) << (32 - s);
    uint32_t result = CURRENT_STATE.REGS[rt] >> s;

    /* bitwise OR result with sign bit of original register content */
    NEXT_STATE.REGS[rd] = result | mask;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_srl(uint32_t instruction)
{
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);
    int sa = get_r_shamt(instruction);

    /* contents of target register shifted right by sa bits to form result and store result in the destination register */
    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] >> sa);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_srlv(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    /* shift amount determined by the low-order five bits of the source register */
    int s = (CURRENT_STATE.REGS[rs] & 0x000001F);

    /* store the result of right shift of target register content in the destination register */
    NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] >> s);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_sub(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    int32_t source = (int32_t)CURRENT_STATE.REGS[rs];
    int32_t target = (int32_t)CURRENT_STATE.REGS[rt];

    NEXT_STATE.REGS[rd] = source - target;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_subu(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    uint32_t source = CURRENT_STATE.REGS[rs];
    uint32_t target = CURRENT_STATE.REGS[rt];

    NEXT_STATE.REGS[rd] = source - target;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_syscall(uint32_t instruction)
{
    /* check whether the simulator should be stopped */
    if (CURRENT_STATE.REGS[REG_SYSCALL] == 0x0000000A)
    {
        RUN_BIT = 0;
    }

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}

int handle_xor(uint32_t instruction)
{
    int rs = get_r_rs(instruction);
    int rt = get_r_rt(instruction);
    int rd = get_r_rd(instruction);

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    return SUCCESS;
}
