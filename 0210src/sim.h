#ifndef SIM_H
#define SIM_H

#include <stdio.h>

#include "mips.h"
#include "shell.h"
#include "utils.h"

#define SUCCESS 0
#define ERROR 1

void process_instruction();

int handle_addi(uint32_t instruction);
int handle_addiu(uint32_t instruction);
int handle_andi(uint32_t instruction);
int handle_beq(uint32_t instruction);
int handle_bgtz(uint32_t instruction);
int handle_blez(uint32_t instruction);
int handle_bne(uint32_t instruction);
int handle_lb(uint32_t instruction);
int handle_lbu(uint32_t instruction);
int handle_lh(uint32_t instruction);
int handle_lhu(uint32_t instruction);
int handle_lui(uint32_t instruction);
int handle_lw(uint32_t instruction);
int handle_ori(uint32_t instruction);
int handle_sb(uint32_t instruction);
int handle_slti(uint32_t instruction);
int handle_sltiu(uint32_t instruction);
int handle_sh(uint32_t instruction);
int handle_sw(uint32_t instruction);
int handle_xori(uint32_t instruction);

int handle_bgez(uint32_t instruction);
int handle_bgezal(uint32_t instruction);
int handle_bltz(uint32_t instruction);
int handle_bltzal(uint32_t instruction);

int handle_j(uint32_t instruction);
int handle_jal(uint32_t instruction);

int handle_add(uint32_t instruction);
int handle_addu(uint32_t instruction);
int handle_and(uint32_t instruction);
int handle_div(uint32_t instruction);
int handle_divu(uint32_t instruction);
int handle_jalr(uint32_t instruction);
int handle_jr(uint32_t instruction);
int handle_mfhi(uint32_t instruction);
int handle_mflo(uint32_t instruction);
int handle_mthi(uint32_t instruction);
int handle_mtlo(uint32_t instruction);
int handle_mult(uint32_t instruction);
int handle_multu(uint32_t instruction);
int handle_nor(uint32_t instruction);
int handle_or(uint32_t instruction);
int handle_sll(uint32_t instruction);
int handle_sllv(uint32_t instruction);
int handle_slt(uint32_t instruction);
int handle_sltu(uint32_t instruction);
int handle_sra(uint32_t instruction);
int handle_srav(uint32_t instruction);
int handle_srl(uint32_t instruction);
int handle_srlv(uint32_t instruction);
int handle_sub(uint32_t instruction);
int handle_subu(uint32_t instruction);
int handle_syscall(uint32_t instruction);
int handle_xor(uint32_t instruction);

#endif /* SIM_H */
