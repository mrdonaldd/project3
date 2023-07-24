#include "sim.h"
#include "shell.h"
#include <stdio.h>
void process_regi_code(uint32_t instruction)
{
    uint8_t rs, opcodeSecond;
    int32_t offset;
    rs = (instruction >> 21) & 0x1F;
    opcodeSecond = (instruction >> 16) & 0x1F;
    offset = ((int32_t)((int16_t)(instruction & 0x0000FFFF)) << 2);
    switch (opcodeSecond){
        case BGEZ:
            if ((int32_t)CURRENT_STATE.REGS[rs] >= 0)
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            }
            else
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            printf("SPECIAL INSTRUCTION: BGEZ rs: %d offset: %d, new PC: %d\n", rs, offset, NEXT_STATE.PC);
            break;
        case BLTZ:
            if ((int32_t)CURRENT_STATE.REGS[rs] < 0)
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            }
            else
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            printf("SPECIAL INSTRUCTION: BLTZ rs: %d offset: %d, new PC: %d\n", rs, offset, NEXT_STATE.PC);
            break;
        case BLTZAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            if ((int32_t)CURRENT_STATE.REGS[rs] < 0)
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            }
            else
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            printf("SPECIAL INSTRUCTION: BLTZAL rs: %d offset: %d, new PC: %d\n", rs, offset, NEXT_STATE.PC);
            break;
        case BGEZAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            if ((int32_t)CURRENT_STATE.REGS[rs] >= 0)
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            }
            else
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            printf("SPECIAL INSTRUCTION: BGEZAL rs: %d offset: %d, new PC: %d\n", rs, offset, NEXT_STATE.PC);
            break;

        default:
            break;
    }
}