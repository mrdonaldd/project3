#include <stdio.h>
#include "shell.h"
#include "sim.h"

void process_spec_code(uint32_t instruction);
void process_regi_code(uint32_t instruction);
void process_opcode(uint32_t instruction);

void process_instruction()
{
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC); // fetch instruction
    uint8_t opcode = instruction >> 26; // get opcode
    // SWITCH based on type of instruction
    if (opcode == SPECIAL){
        // printf("opcode: %d\n", opcode);
        process_spec_code(instruction);
    }
    else if (opcode == REGIMM){
        // printf("opcode: %d\n", opcode);
        process_regi_code(instruction);
    }
    else{
        // printf("opcode: %d\n", opcode);
        process_opcode(instruction);
    }
}
