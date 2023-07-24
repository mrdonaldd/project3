#include <stdio.h>
#include "shell.h"
#include "mips.h"

uint32_t op; // decod the opcode
uint32_t rt; // decod the rt operand
uint32_t rs; // decod the rs operand
uint32_t rd; // decod the rd operand
uint32_t imm; // decode iv
uint32_t target; // decode the target address
uint32_t shamt; // decode the shift amount
uint32_t function; // decode the function itself
int signed_imm; // sign iv
uint32_t inst; // the instruction itself.

int64_t prod;
int temp;


// signing the extention functions, helper functions
uint32_t sign_b2w(uint8_t c) { // if msb 1 sets higher bits
    return (c & 0x80) ? (c | 0xffffff80) : c;
}

uint32_t sign_h2w(uint16_t c) {
    return (c & 0x8000) ? (c | 0xffff8000) : c;
}

uint32_t zero_h2w(uint16_t c) {
    return ((uint32_t)c);
}

uint32_t zero_b2w(uint8_t c) {
    return ((uint32_t)c);
}

int sign_18bit() {
    return (signed_imm << 2);
}

void process_instruction() {

    /* This function executes one instruction at a time.
     * It fetches, decodes, and updates the architectural state accordingly. */


    // Fetching the current instruction
    inst = mem_read_32(CURRENT_STATE.PC);

    // decode the instruction into various fields
    uint32_t op = (inst >> 26) & 0x3F;
    uint32_t rs = (inst >> 21) & 0x1F;
    uint32_t rt = (inst >> 16) & 0x1F;
    uint32_t rd = (inst >> 11) & 0x1F;
    uint32_t imm = (inst >> 0) & 0xFFFF;
    uint32_t shamt = (inst >> 6) & 0x1F;
    uint32_t function = (inst >> 0) & 0x3F;
    uint32_t signed_imm = sign_h2w(imm);
    uint32_t target = (inst >> 0) & ((1UL << 26) - 1);

    switch (op) {     // update the state based on the op
        case OP_SPECIAL: {
            switch (function) {
                case OPLB: // byte
                        NEXT_STATE.REGS[rt] = sign_b2w((mem_read_32(CURRENT_STATE.REGS[rs] + signed_imm)) & 0xFF);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    
                case OPLH: // halfword
                        NEXT_STATE.REGS[rt] = sign_h2w((mem_read_32(CURRENT_STATE.REGS[rs] + signed_imm)) & 0xFFFF);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                case OPSH: // storing the byte
                        mem_write_32((CURRENT_STATE.REGS[rs] + signed_imm), (int)((CURRENT_STATE.REGS[rt]) & 0xFF));
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                case OPLW: // work
                        NEXT_STATE.REGS[rt] = mem_read_32((CURRENT_STATE.REGS[rs] + signed_imm));
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                case OPLBU: // byte unsigned
                        NEXT_STATE.REGS[rt] = zero_extend_b2w((mem_read_32(CURRENT_STATE.REGS[rs] + signed_imm)) & 0xFF);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                case OPLHU:
                        NEXT_STATE.REGS[rt] = zero_extend_h2w((mem_read_32(CURRENT_STATE.REGS[rs] + signed_imm)) & 0xFFFF);
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                    // storing
                case OPSH: // Halfword
                    mem_write_32((CURRENT_STATE.REGS[rs] + signed_imm), (int)((CURRENT_STATE.REGS[rt]) & 0xFFFF));
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case OPSW: // word
                        mem_write_32((CURRENT_STATE.REGS[rs] + signed_imm), (int)((CURRENT_STATE.REGS[rt])));
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                // ALU Instructions
                case OP_SPECIAL:
                    switch (function) {
                        case SUBOP_ADD:
                        case SUBOP_ADDU:
                            if (rd != 0)
                                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                            break;
                        case SUBOP_SYSCALL:
                            if (CURRENT_STATE.REGS[2] == 10)
                                RUN_BIT = 0;
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                            break;

                        default:
                            printf("Ending sim\n\n", function);
                            RUN_BIT = 0;
                    }
                    break;

                case BRSPEC: { // special branches
                    switch (rt) {
                    case BLTZ: // < 0
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        if ((int)CURRENT_STATE.REGS[rs] < 0)
                            NEXT_STATE.PC = NEXT_STATE.PC + sign_18bit();
                        break;

                    // >= 0
                    case BROP_BGEZ:
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        if  (int)CURRENT_STATE.REGS[rs] >= 0)
                            NEXT_STATE.PC = NEXT_STATE.PC + sign_18bit();
                        break;

                    // <0 chain
                    case BLTZAL:
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                        if  (int)CURRENT_STATE.REGS[rs] < 0)
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + sign_18bit();
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;

                    case  ROP_BGEZAL: // chain
                        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                        if ((int)CURRENT_STATE.REGS[rs] >= 0)
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + sign_18bit();
                            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                        break;
                    }
                }

                case ANDSUBOP: // AND
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case DIV:
                    if (rt != 0) {
                        NEXT_STATE.HI = (int)CURRENT_STATE.REGS[rs] % (int)CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = (int)CURRENT_STATE.REGS[rs] / (int)CURRENT_STATE.REGS[rt];
                    } else {
                        printf("Divided by zero");
                    }
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case DIVU:
                    if (rt != 0) {
                        NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
                        NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
                    } else {
                        printf("divided by zero");
                    }
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case SUBOP_JALR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 8;
                    if (!(CURRENT_STATE.REGS[rs] & 0x03))
                        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    else
                        printf("add not word aligned");
                    break;
                
                case JR:
                    if (!(CURRENT_STATE.REGS[rs] & 0x03))
                        NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    else {
                        printf("add not word aligned");
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;
                
                case MFHI:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case MFLO:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case MTHI:
                    if (rs != 0)
                        NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case MTLO:
                    if (rs != 0)
                        NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case SUBOP_MULT:
                    prod = ((int)CURRENT_STATE.REGS[rs] * (int)CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.HI = (prod >> 31) & 0xFFFFFFFF;
                    NEXT_STATE.LO = (prod) & 0xFFFFFFFF;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case SUBOP_MULTU:
                    NEXT_STATE.HI = ((CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) >> 31);
                    NEXT_STATE.LO = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case SUBOP_NOR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SUBOP_OR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SUBOP_SLL:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << dcd_shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SUBOP_SLLV:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SUBOP_SLT:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = ((int)CURRENT_STATE.REGS[rs] < (int)CURRENT_STATE.REGS[rt]) ? 1 : 0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                
                case SUBOP_SLTU:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? 1 : 0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                // Arithmetic and Logical Operations // *****
                case OP_ADDI:
                case OP_ADDIU:
                    if (rt != 0)
                        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + signed_imm;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SLTI:
                    NEXT_STATE.REGS[rt] = (((int)CURRENT_STATE.REGS[rs] < (int)signed_imm) ? 1 : 0);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                // < immediate Unsigned
                case SLTIU:
                    NEXT_STATE.REGS[rt] = ((CURRENT_STATE.REGS[rs] < signed_imm) ? 1 : 0);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break; 

                // >>
                case SUBOPSRL:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                // >> var
                case SUBOPSRLV:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SUBB: // sorry seperate from add
                case SUBBU:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;

                case SUBOP_XOR:
                    if (rd != 0)
                        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] ^ CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                default:
                    printf("Encountered unimplemented subopcode (0x%x). Ending simulation...\n\n", function);
                    RUN_BIT = 0;
                }
            }   
            break;
            case OP_ANDI:
                NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] & zero_h2w(imm));
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                break;

            //OR with immidiate
            case OP_ORI:
                NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] | zero_h2w(imm));
                NEXT_STATE.PC = CURRENT_STATE.PC +  4;
                break;

            //XOR with immidiate
            case OP_XORI:
                NEXT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] ^ zero_h2w(imm));
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                break;      

            // Jumps and Branches
            case OP_J:
                NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
                break;

            case OP_JAL:
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
                break;

            case OP_BEQ:
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
                    NEXT_STATE.PC = NEXT_STATE.PC + sign_18bit();
                break;

            case OP_BNE:
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
                    NEXT_STATE.PC = NEXT_STATE.PC + sign_18bit();
                break;

            case OP_BLEZ:
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                if (CURRENT_STATE.REGS[rs] <= 0)
                    NEXT_STATE.PC = NEXT_STATE.PC + sign_18bit();
                break;

            //branch if > 0
            case OP_BGTZ:
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                if (CURRENT_STATE.REGS[rs] > 0)
                    NEXT_STATE.PC = NEXT_STATE.PC + sign_18b();
                break;
                
            case OP_LUI:
                if (rs == 0)
                    NEXT_STATE.REGS[rt] = (imm << 16);
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                break;

            default:
                printf("ending sim.\n\n", op);
                RUN_BIT = 0;
            }
        }

// tests work,  extra tests work too but idt i have everything here