#include <stdio.h>
#include "shell.h"
#define OP_J          0x02
#define OP_JAL        0x03
#define OP_BEQ        0x04
#define OP_BNE        0x05
#define OP_BLEZ       0x06
#define OP_BGTZ       0x07
#define OP_SLL     	  0x0
#define OP_SRL        0x2
#define OP_SRA        0x3
#define OP_SLLV       0x4
#define OP_SRLV       0x6
#define OP_SRAV       0x7

void process_special_fn(uint32_t instr){

//| opcode (6 bits) | rs (5 bits) | rt (5 bits) | rd (5 bits) | shamt (5 bits) | funct (6 bits) |
    // extract higher order 3 bits of the opcode
    uint32_t hi = (instr << 26) >> 29;
    uint32_t lo = (instr << 29) >> 29;
    uint32_t rs = (instr << 6) >> 27;  //source reg
    uint32_t rt = (instr << 11) >> 27; //target reg
    uint32_t rd = (instr << 16) >> 27; //dest reg
    uint32_t shift_amt = (instr << 21) >> 27; //shift amount
    uint32_t rs_val = CURRENT_STATE.REGS[rs];

    if (hi == 0) {
        switch (lo) {
        //Shift Left Logical
        case OP_SLL: 
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shift_amt;
                break;
        //Shift Right Logical
        case OP_SRL: 
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shift_amt;
                break;
        //Shift Right Arithmetic
        case OP_SRA: 
				NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> shift_amt;
                break;
        //Shift Left Logical Variable
        case OP_SLLV: 
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << ((rs_val << 27) >> 27);
                break;
        //Shift Right Logical Variable
        case OP_SRLV: 
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> ((rs_val << 27) >> 27);
                break;
        //Shift Right Arithmetic Variable
        case OP_SRAV: 
				NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> ((rs_val << 27) >> 27);
                break;
        }
    }
    else if (hi == 1) {
        switch (lo) {

        // JR - Jump Register
        case 0: NEXT_STATE.PC = rs_val;
                break;

        // JALR - Jump And Link Register
        case 1: NEXT_STATE.PC = rs_val;
                NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
                break;

        // SYSCALL
        case 4: if (CURRENT_STATE.REGS[2] == 10) {
                    RUN_BIT = 0;
                }
                break;
        }
    }
    else if (hi == 2) {
        switch (lo) {
        // MFHI - move From HI
        case 0: NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                break;
        // MTHI - move To HI
        case 1: NEXT_STATE.HI = rs_val;
                break;
        // MFLO - mv From LO
        case 2: NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                break;
        // MTLO - mv To LO
        case 3: NEXT_STATE.LO = rs_val;
                break;
        }
    }
    else if (hi == 3) {
        int64_t result = 0;
        switch (lo) {
        // MULT multiply
        case 0: result = (int32_t)rs_val * (int32_t)CURRENT_STATE.REGS[rt];
                NEXT_STATE.HI = result >> 32;
                NEXT_STATE.LO = result;
                break;
        // MULTU 
        case 1: result = rs_val * CURRENT_STATE.REGS[rt];
                NEXT_STATE.HI = result >> 32;
                NEXT_STATE.LO = result;
                break;
        // DIV
        case 2: NEXT_STATE.HI = (int32_t)rs_val % (int32_t)CURRENT_STATE.REGS[rt];
                NEXT_STATE.LO = (int32_t)rs_val / (int32_t)CURRENT_STATE.REGS[rt];
                break;
        // DIVU
        case 3: NEXT_STATE.HI = rs_val % CURRENT_STATE.REGS[rt];
                NEXT_STATE.LO = rs_val / CURRENT_STATE.REGS[rt];
                break;
        }
    }
    else if (hi == 4) {
        switch (lo) {

        // ADD n ADDU 
        case 0:
        case 1: NEXT_STATE.REGS[rd] = rs_val + CURRENT_STATE.REGS[rt];
                break;

        // SUB n SUBU 
        case 2:
        case 3: NEXT_STATE.REGS[rd] = rs_val - CURRENT_STATE.REGS[rt];
                break;

        // AND 
        case 4: NEXT_STATE.REGS[rd] = rs_val & CURRENT_STATE.REGS[rt];
                break;

        // OR 
        case 5: NEXT_STATE.REGS[rd] = rs_val | CURRENT_STATE.REGS[rt];
                break;

        // XOR 
        case 6: NEXT_STATE.REGS[rd] = rs_val ^ CURRENT_STATE.REGS[rt];
                break;

        // NOR 
        case 7: NEXT_STATE.REGS[rd] = !(rs_val | CURRENT_STATE.REGS[rt]);
                break;
        }
    }
    else if (hi == 5) {
        int32_t rs_val_signed = (int32_t)rs_val;
		int32_t rt_val_signed = (int32_t)CURRENT_STATE.REGS[rt];
		int32_t result_signed;
		switch (lo) {
			// SLT set on less than
			case 2: result_signed = rs_val_signed < rt_val_signed;
					break;

			// SLTU set on less than unsigned
			case 3: result_signed = rs_val < CURRENT_STATE.REGS[rt];
					break;

			default: result_signed = 0; 
					break;
		}
		// revert the signed res back to unsigned 32
		NEXT_STATE.REGS[rd] = (uint32_t)result_signed;
    }
}

void process_register_immediate(uint32_t instr){

    uint32_t hi = (instr << 11) >> 30;
    uint32_t lo = (instr << 13) >> 29;
    uint32_t rs = (instr << 6) >> 27;
    int32_t target = instr << 16;
    target = target >> 14;
    int32_t rs_val = CURRENT_STATE.REGS[rs];

    if (hi == 0) {
        switch (lo) {
        // BLTZ branch on < 0
        case 0: if ((rs_val >> 31) == -1) {NEXT_STATE.PC = CURRENT_STATE.PC + target;}
				printf("BLTZ rs_val:%d \n", rs_val);
                break;
        // BGEZ branch on >= 0
        case 1: if ((rs_val >> 31) == 0) {NEXT_STATE.PC = CURRENT_STATE.PC + target;}
                break;
        }
    }
    else if (hi == 2) {
        switch (lo) {
        // BLTZAL branch on < 0 and Link
		////do run n and compare the result with qtSpim using rdump :P
        case 0: NEXT_STATE.REGS[31] = NEXT_STATE.PC;
                if ((rs_val >> 31) == -1) {NEXT_STATE.PC = CURRENT_STATE.PC + target;}
                break;
        // BGEZAL branch on >= 0 and Link
        case 1: NEXT_STATE.REGS[31] = NEXT_STATE.PC;
                if ((rs_val >> 31) == 0) {NEXT_STATE.PC = CURRENT_STATE.PC + target;}
                break;
        }
    }
}

void process_instruction()
{
    uint32_t  curr_instr = mem_read_32(CURRENT_STATE.PC);
    uint32_t opcode = curr_instr >> 26;
	// printf("Opcode val = %d", opcode);
    NEXT_STATE = CURRENT_STATE;
    NEXT_STATE.PC += 4;

	// extract the higher order 3 bits of the opcode
    uint32_t hi = curr_instr >> 29;

    // extract the lower order 3 bits of the opcode
    uint32_t lo = (curr_instr << 3) >> 29;

	//MIPS instruction format, the "rs" field corresponds to bits 25 to 21 (inclusive)
	// << 6 shifts the entire 32-bit instruction left by 6 positions, 
	//aligning bits 31 to 6 with the "rs" field. Then >> 27 extracts the 5-bit val
	//corresponding to the "rs" field
	//e.g. curr_instr = 0b00000011001001100010100100100000
	//curr_instr << 6:  00011001001100010100100100000000
	//(rs):           00000000000000000000000000011001
    uint32_t rs = (curr_instr << 6) >> 27;
	// rt = bits 20 to 16 
    uint32_t rt = (curr_instr << 11) >> 27;
    int32_t rs_val = CURRENT_STATE.REGS[rs];

    switch(opcode) {
    // special sub-opcodes (opcode=0)
    case 0: process_special_fn(curr_instr);
            break;

    // opcode = 1 => REGIMM rt 
	//REGIMM instruction format: |opcode (6 bits) | rs (5 bits) | rt (5 bits) | imm (16 bits) |
    case 1: process_register_immediate(curr_instr);
            break;

    // default on normal opcode
    default: 
	//hi = 0 is R-type i.e. operations between registers
	//hi = 1 is I-tpe i.e. operations with Intermediate (const)
	//hi = 4 load
	//hi = 5 is an old social media xD
		if (hi == 0) {
			int32_t target = curr_instr << 16;
			target = target >> 14;
			switch (lo) {
			//jump
			case OP_J: 
				NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) + ((curr_instr << 2) & 0x0ffffffc);
				break;
			//jump n link
			case OP_JAL: 
				NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
				NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) + ((curr_instr << 2) & 0x0ffffffc);
				break;
			//branch on equal
			case OP_BEQ: 
				if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
					NEXT_STATE.PC = CURRENT_STATE.PC + target;}
				break;
			//branch on not eq
			case OP_BNE: 
				if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
					NEXT_STATE.PC = CURRENT_STATE.PC + target;}
				break;
			//branch on <= 0
			case OP_BLEZ: 
				if ((rs_val == 0) || ((rs_val >> 31) == -1)) {
					NEXT_STATE.PC = CURRENT_STATE.PC + target;}
				break;
			//branch on > 0
			case OP_BGTZ: if ((rs_val != 0) && ((rs_val >> 31) == 0)) {
					NEXT_STATE.PC = CURRENT_STATE.PC + target;}
				break;
			}
		}
		else if (hi == 1) {
			int32_t immediate = curr_instr << 16;
			immediate = immediate >> 16;
			switch (lo) {
			// ADDI n ADDIU - Add Immediate n Add Immediate Unsigned
			case 0:
			case 1:	NEXT_STATE.REGS[rt] = NEXT_STATE.REGS[rs] + immediate;
					break;
			// SLTI - Set On Less Than Immediate
			case 2: if (rs_val < immediate) {NEXT_STATE.REGS[rt] = 1;}
					else {NEXT_STATE.REGS[rt] = 0;}
					break;
			// SLTIU - Set On Less Than Immediate Unsigned
			case 3: if ((uint32_t)rs_val < (uint32_t)immediate) {NEXT_STATE.REGS[rt] = 1;}
					else {NEXT_STATE.REGS[rt] = 0;}
					break;
			// ANDI - And Immediate
			case 4: NEXT_STATE.REGS[rt] = ((curr_instr << 16) >> 16) & rs_val;
					break;

			// ORI - Or Immediate
			case 5: NEXT_STATE.REGS[rt] = immediate | rs_val;
					break;

			// XORI - Xor Immediate
			case 6: NEXT_STATE.REGS[rt] = ((curr_instr << 16) >> 16) ^ rs_val;
					break;

			// LUI - Load Upper Immediate
			case 7: NEXT_STATE.REGS[rt] = immediate << 16;
					break;
			}
		}
		else if (hi == 4) {
			int32_t mem_addr = curr_instr << 16;
			mem_addr = mem_addr >> 16;
			mem_addr += rs_val;
			int32_t mem_val = mem_read_32(mem_addr);
			switch (lo) {
			// LB - Load Byte
			case 0: mem_val = mem_val << 24;
					mem_val = mem_val >> 24;
					NEXT_STATE.REGS[rt] = mem_val;
					break;
			// LH - Load Halfword
			case 1: mem_val = mem_val << 16;
					mem_val = mem_val >> 16;
					NEXT_STATE.REGS[rt] = mem_val;
					break;
			// LW - Load Word
			case 3: NEXT_STATE.REGS[rt] = mem_val;
					break;
			// LBU - Load Byte Unsigned
			case 4: mem_val = mem_val << 24;
					mem_val = (uint32_t)mem_val >> 24;
					NEXT_STATE.REGS[rt] = mem_val;
					break;
			// LHU - Load Halfword Unsigned
			case 5: mem_val = mem_val << 16;
					mem_val = (uint32_t)mem_val >> 16;
					NEXT_STATE.REGS[rt] = mem_val;
					break;
			}
		}
		else if (hi == 5) {
			int32_t mem_addr = curr_instr << 16;
			mem_addr = mem_addr >> 16;
			mem_addr += rs_val;
			uint32_t rt_val = CURRENT_STATE.REGS[rt];
			uint32_t mem_val = mem_read_32(mem_addr);
			switch (lo) {
			// SB - Store Byte
			case 0: mem_val = (mem_val >> 8) << 8;
					mem_val += (rt_val << 24) >> 24;
					mem_write_32(mem_addr, mem_val);
					break;
			// SH - Store Halfword
			case 1: mem_val = (mem_val >> 16) << 16;
					mem_val += (rt_val << 16) >> 16;
					mem_write_32(mem_addr, mem_val);
					break;
			// SW - Store Word
			case 3: mem_write_32(mem_addr, rt_val);
					break;
			}
		}
				break; //if not implement then keep on breaking
		}
}

