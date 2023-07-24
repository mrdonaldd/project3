#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>

#include "mips.h"

int get_opcode(uint32_t instruction);
int get_i_rs(uint32_t instruction);
int get_i_rt(uint32_t instruction);
int16_t get_i_immediate(uint32_t instruction);

uint32_t get_j_target(uint32_t instruction);

int get_r_rs(uint32_t instruction);
int get_r_rt(uint32_t instruction);
int get_r_rd(uint32_t instruction);
int get_r_shamt(uint32_t instruction);
int get_r_funct(uint32_t instruction);

#endif /* UTILS_H */
