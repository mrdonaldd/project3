#include <stdio.h>
#include "shell.h"

void process_instruction() {
    // Fetch the instruction from memory
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    // Extract fields from the instruction
    uint8_t opcode = (instruction >> 26) & 0x3F;
    uint8_t rs = (instruction >> 21) & 0x1F;
    uint8_t rt = (instruction >> 16) & 0x1F;
    uint8_t rd = (instruction >> 11) & 0x1F;

    if (opcode == 0) // R-type instruction
    {
        uint8_t funct = instruction & 0x3F;

        if (funct == 8) // JR (Jump Register)
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            NEXT_STATE.PC = rs_value;
        } else if (funct == 9) // JALR (Jump and Link Register)
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t temp_pc = NEXT_STATE.PC;

            NEXT_STATE.REGS[rd] = temp_pc + 4;
            NEXT_STATE.PC = rs_value;
        } else if (funct == 32) // ADD
        {
            int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
            int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];
            int32_t result = rs_value + rt_value;

            NEXT_STATE.REGS[rd] = (uint32_t) result;
        } else if (funct == 33) // ADDU
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = rs_value + rt_value;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 34) // SUB
        {
            int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
            int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];
            int32_t result = rs_value - rt_value;

            NEXT_STATE.REGS[rd] = (uint32_t) result;
        } else if (funct == 35) // SUBU
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = rs_value - rt_value;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 36) // AND
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = rs_value & rt_value;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 37) // OR
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = rs_value | rt_value;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 38) // XOR
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = rs_value ^ rt_value;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 39) // NOR
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = ~(rs_value | rt_value);

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 42) // SLT
        {
            int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
            int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];
            uint32_t result = (rs_value < rt_value) ? 1 : 0;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 43) // SLTU
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t result = (rs_value < rt_value) ? 1 : 0;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 24) // MULT
        {
            int64_t
                    rs_value = (int64_t)(int32_t)
            CURRENT_STATE.REGS[rs];
            int64_t
                    rt_value = (int64_t)(int32_t)
            CURRENT_STATE.REGS[rt];
            int64_t product = rs_value * rt_value;

            NEXT_STATE.HI = (product >> 32) & 0xFFFFFFFF;
            NEXT_STATE.LO = product & 0xFFFFFFFF;
        } else if (funct == 25) // MULTU
        {
            uint64_t rs_value = CURRENT_STATE.REGS[rs];
            uint64_t rt_value = CURRENT_STATE.REGS[rt];
            uint64_t product = rs_value * rt_value;

            NEXT_STATE.HI = (product >> 32) & 0xFFFFFFFF;
            NEXT_STATE.LO = product & 0xFFFFFFFF;
        } else if (funct == 16) // MFHI (Move from HI)
        {
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
        } else if (funct == 18) // MFLO (Move from LO)
        {
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
        } else if (funct == 17) // MTHI (Move to HI)
        {
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
        } else if (funct == 19) // MTLO (Move to LO)
        {
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
        } else if (funct == 26) // DIV
        {
            int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
            int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];

            if (rt_value != 0) {
                NEXT_STATE.LO = (uint32_t)(rs_value / rt_value);
                NEXT_STATE.HI = (uint32_t)(rs_value % rt_value);
            }
        } else if (funct == 27) // DIVU
        {
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t rt_value = CURRENT_STATE.REGS[rt];

            if (rt_value != 0) {
                NEXT_STATE.LO = rs_value / rt_value;
                NEXT_STATE.HI = rs_value % rt_value;
            }
        } else if (funct == 0) // SLL (Shift Left Logical)
        {
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t sa = (instruction >> 6) & 0x1F;
            uint32_t result = rt_value << sa;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 2) // SRL (Shift Right Logical)
        {
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t sa = (instruction >> 6) & 0x1F;
            uint32_t result = rt_value >> sa;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 3) // SRA (Shift Right Arithmetic)
        {
            int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];
            uint32_t sa = (instruction >> 6) & 0x1F;
            int32_t result = rt_value >> sa;

            NEXT_STATE.REGS[rd] = (uint32_t) result;
        } else if (funct == 4) // SLLV (Shift Left Logical Variable)
        {
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t sa = rs_value & 0x1F;
            uint32_t result = rt_value << sa;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 6) // SRLV (Shift Right Logical Variable)
        {
            uint32_t rt_value = CURRENT_STATE.REGS[rt];
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t sa = rs_value & 0x1F;
            uint32_t result = rt_value >> sa;

            NEXT_STATE.REGS[rd] = result;
        } else if (funct == 7) // SRAV (Shift Right Arithmetic Variable)
        {
            int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];
            uint32_t rs_value = CURRENT_STATE.REGS[rs];
            uint32_t sa = rs_value & 0x1F;
            int32_t result = rt_value >> sa;

            NEXT_STATE.REGS[rd] = (uint32_t) result;
        }
    }
    {
        uint32_t target = (instruction & 0x03FFFFFF) << 2;
        uint32_t upper_pc_bits = NEXT_STATE.PC & 0xF0000000;
        NEXT_STATE.PC = upper_pc_bits | target;
    }

    else if (opcode == 3) // JAL
    {
        uint32_t target = (instruction & 0x03FFFFFF) << 2;
        uint32_t upper_pc_bits = NEXT_STATE.PC & 0xF0000000; //
        NEXT_STATE.REGS[31] = NEXT_STATE.PC + 4;
        NEXT_STATE.PC = upper_pc_bits | target;
    } else if (opcode == 4) // BEQ
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
        int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];

        if (rs_value == rt_value) {
            NEXT_STATE.PC += (immediate << 2);
        } else {
            NEXT_STATE.PC += 4;
        }
    } else if (opcode == 5) // BNE
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
        int32_t rt_value = (int32_t) CURRENT_STATE.REGS[rt];

        if (rs_value != rt_value) {
            NEXT_STATE.PC += (immediate << 2);
        } else {
            NEXT_STATE.PC += 4;
        }
    } else if (opcode == 6) // BLEZ
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];

        if (rs_value <= 0) {
            NEXT_STATE.PC += (immediate << 2);
        } else {
            NEXT_STATE.PC += 4;
        }
    } else if (opcode == 7) // BGTZ
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];

        if (rs_value > 0) {
            NEXT_STATE.PC += (immediate << 2);
        } else {
            NEXT_STATE.PC += 4;
        }
    } else if (opcode == 8) // ADDI
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
        int32_t result = rs_value + immediate;

        NEXT_STATE.REGS[rt] = (uint32_t) result;
    } else if (opcode == 9) // ADDIU
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
        int32_t result = rs_value + immediate;

        NEXT_STATE.REGS[rt] = (uint32_t) result;
    } else if (opcode == 10) // SLTI
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        int32_t rs_value = (int32_t) CURRENT_STATE.REGS[rs];
        uint32_t result = (rs_value < immediate) ? 1 : 0;

        NEXT_STATE.REGS[rt] = result;
    } else if (opcode == 11) // SLTIU
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        uint32_t result = (rs_value < (uint32_t) immediate) ? 1 : 0;

        NEXT_STATE.REGS[rt] = result;
    } else if (opcode == 12) // ANDI
    {
        uint16_t immediate = (uint16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        uint32_t result = rs_value & immediate;

        NEXT_STATE.REGS[rt] = result;
    } else if (opcode == 13) // ORI
    {
        uint16_t immediate = (uint16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        uint32_t result = rs_value | immediate;

        NEXT_STATE.REGS[rt] = result;
    } else if (opcode == 14) // XORI
    {
        uint16_t immediate = (uint16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        uint32_t result = rs_value ^ immediate;

        NEXT_STATE.REGS[rt] = result;
    } else if (opcode == 15) // LUI
    {
        uint16_t immediate = (uint16_t)(instruction & 0xFFFF);
        uint32_t result = immediate << 16;

        NEXT_STATE.REGS[rt] = result;
    } else if (opcode == 32) // LB
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint32_t data = mem_read_8(mem_address);

        // Sign-extend data to 32-bit
        if (data & 0x80) {
            data |= 0xFFFFFF00;
        }

        NEXT_STATE.REGS[rt] = data;
    } else if (opcode == 33) // LH
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint32_t data = mem_read_16(mem_address);

        // Sign-extend data to 32-bit
        if (data & 0x8000) {
            data |= 0xFFFF0000;
        }

        NEXT_STATE.REGS[rt] = data;
    } else if (opcode == 35) // LW
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint32_t data = mem_read_32(mem_address);

        NEXT_STATE.REGS[rt] = data;
    } else if (opcode == 36) // LBU
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint32_t data = mem_read_8(mem_address);

        NEXT_STATE.REGS[rt] = data;
    } else if (opcode == 37) // LHU
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint32_t data = mem_read_16(mem_address);

        NEXT_STATE.REGS[rt] = data;
    } else if (opcode == 40) // SB
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint8_t data = CURRENT_STATE.REGS[rt] & 0xFF;

        mem_write_8(mem_address, data);
    } else if (opcode == 41) // SH
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint16_t data = CURRENT_STATE.REGS[rt] & 0xFFFF;

        mem_write_16(mem_address, data);
    } else if (opcode == 43) // SW
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];
        int32_t mem_address = rs_value + immediate;
        uint32_t data = CURRENT_STATE.REGS[rt];

        mem_write_32(mem_address, data);
    } else if (opcode == 1) // BLTZ/BGEZ
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];

        if ((rt == 0 && (int32_t) rs_value < 0) || (rt == 1 && (int32_t) rs_value >= 0)) {
            NEXT_STATE.PC += (immediate << 2);
        } else {
            NEXT_STATE.PC += 4;
        }
    } else if (opcode == 1) // BLTZAL/BGEZAL
    {
        int16_t immediate = (int16_t)(instruction & 0xFFFF);
        uint32_t rs_value = CURRENT_STATE.REGS[rs];

        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; // Link register (31) holds the return address

        if ((rt == 0 && (int32_t) rs_value < 0) || (rt == 1 && (int32_t) rs_value >= 0)) {
            NEXT_STATE.PC += (immediate << 2);
        } else {
            NEXT_STATE.PC += 4;
        }
    } else if (opcode == 12) // SYSCALL
    {
        uint32_t syscall_code = CURRENT_STATE.REGS[2];

        if (syscall_code == 0x0A) {
            RUN_BIT = FALSE;
        }
    }

    NEXT_STATE.PC += 4;
}

