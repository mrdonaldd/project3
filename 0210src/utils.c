#include "utils.h"

int get_opcode(uint32_t instruction)
{
	return (int)((instruction & MASK_OPCODE) >> SHIFT_OPCODE);
}

int get_i_rs(uint32_t instruction)
{
	return (int)((instruction & MASK_I_RS) >> SHIFT_I_RS);
}

int get_i_rt(uint32_t instruction)
{
	return (int)((instruction & MASK_I_RT) >> SHIFT_I_RT);
}

int16_t get_i_immediate(uint32_t instruction)
{
	return (int16_t)((instruction & MASK_I_IMMEDIATE) >> SHIFT_I_IMMEDIATE);
}

uint32_t get_j_target(uint32_t instruction)
{
	return (uint32_t)((instruction & MASK_J_TARGET) >> SHIFT_J_TARGET);
}

int get_r_rs(uint32_t instruction)
{
	return (int)((instruction & MASK_R_RS) >> SHIFT_R_RS);
}

int get_r_rt(uint32_t instruction)
{
	return (int)((instruction & MASK_R_RT) >> SHIFT_R_RT);
}

int get_r_rd(uint32_t instruction)
{
	return (int)((instruction & MASK_R_RD) >> SHIFT_R_RD);
}

int get_r_shamt(uint32_t instruction)
{
	return (int)((instruction & MASK_R_SHAMT) >> SHIFT_R_SHAMT);
}

int get_r_funct(uint32_t instruction)
{
	return (int)((instruction & MASK_R_FUNCT) >> SHIFT_R_FUNCT);
}
