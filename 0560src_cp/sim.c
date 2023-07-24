#include <stdio.h>
#include "shell.h"

// front op

/*
#define SPECIAL 	= 0b000000

OP-here		target
6			26

#define J			= 0b000010
#define JAL			= 0b000011

*OP-here*	rs	rt	offset
6			5	5	16

#define BEQ			= 0b000100
#define BNE			= 0b000101
#define BLEZ		= 0b000110
#define BGTZ		= 0b000111
#define ADDI		= 0b001000
#define ADDIU		= 0b001001
#define SLTI		= 0b001010
#define SLTIU		= 0b001011
#define ANDI		= 0b001100
#define XORI		= 0b001110
#define ORI			= 0b001101
#define LUI			= 0b001111
#define LB			= 0b100000
#define LH			= 0b100001
#define LW			= 0b100011
#define LBU			= 0b100100
#define LHU			= 0b100101
#define SB			= 0b101000
#define SH			= 0b101001
#define SW			= 0b101011

REGIMM	rs	*OP-here*	offset
6		5		5		16

#define BLTZ		= 0b00000
#define BGEZ		= 0b00001
#define BLTZAL		= 0b10000
#define BGEZAL		= 0b10001

back op
#define SLL			= 0b000000
#define SRL			= 0b000010
#define SRA			= 0b000011
#define SLLV		= 0b000100
#define SRLV		= 0b000110
#define SRAV		= 0b000111
#define JR			= 0b001000
#define JALR		= 0b001001
#define ADD			= 0b100000
#define ADDU		= 0b100001
#define SUB			= 0b100010
#define SUBU		= 0b100011
#define AND			= 0b100100
#define OR			= 0b100101
#define XOR			= 0b100110
#define NOR			= 0b100111
#define SLT			= 0b101010
#define SLTU		= 0b101011
#define MULT		= 0b011000
#define MFHI		= 0b010000
#define MFLO		= 0b010010
#define MTHI		= 0b010001
#define MTLO		= 0b010011
#define MULTU		= 0b011001
#define DIV			= 0b011010
#define DIVU		= 0b011011
#define SYSCALL		= 0b001100
*/


uint32_t  arith  ( uint32_t * argv ) { //argv = { x, op, y }
	int32_t  x = argv[0],
			op = argv[1],
			 y = argv[2],
			 z = 0;
		 
	if      (op == '+') { z = x+y; }
	else if (op == '-') { z = x-y; }
	else if (op == '*') { z = x*y; }
	else if (op == '/') { z = z/y; }

	return z; 
	}

uint32_t uarith (uint32_t * argv) { //argv = { x, op, y }
	uint32_t x = argv[0],
			op = argv[1],
			 y = argv[2],
			 z = 0;
	
	if      (op == '+') { z = x+y; }
	else if (op == '-') { z = x-y; }
	else if (op == '*') { z = x*y; }
	else if (op == '/') { z = z/y; }

	return z; 
	}
	
uint32_t shift  (uint32_t * argv) { //argv = { x, op(0=="<<" , 1==">>"), y, Arith }
	
	uint32_t x = argv[0], 
		    op = argv[1],
		     y = argv[2],
		 arith = argv[3],
		     z = 0;

	if      (op == 0) { z = x<<y; }
	else if (op == 1) { z = x>>y; 
	
		if ( arith ) { z |= ~(0xffffffff >> y); }
		}
	return z;
	}

void decode(uint32_t instruction){
	
	uint8_t msb6, lsb6;
	
	CPU_State NOW  = CURRENT_STATE;
    CPU_State NEXT = NEXT_STATE;
	
	msb6 	= instruction >> 26u;		// Most  Significant Bits 6
	lsb6	= instruction;				// Least Significant Bits 6
	
	uint32_t (* f)(), (* f1)(), (* f2)();
	
	lsb6 &= ~(0b11 << 6);				// clear bit position 7,8
	
	uint8_t rs, rt, rd, sa;				// address of register
	rs = ( instruction >> 21u ) & ~(0b11 << 6);
	rt = ( instruction >> 16u ) & ~(0b11 << 6);
	rd = ( instruction >> 11u ) & ~(0b11 << 6);
	sa = ( instruction >>  6u ) & ~(0b11 << 6);
	
	uint32_t RS, RT, RD, SA, TMP1, TMP2, RESULT;	// content of register
	uint64_t ULONG;
	uint32_t * REGS;
	
	REGS = CURRENT_STATE.REGS;
	RS = REGS[rs];
	RT = REGS[rt];
	RD = REGS[rd];
	SA = REGS[sa];
	
	/*
		THIS IS REGISTER INSTRUCTION HANDLING PART
	*/	
	if( msb6 == 0b000000 ){
		
		if ( lsb6 == 0b001100){ //SYSCALL
			if (NOW.REGS[2] == 0xA) {RUN_BIT = FALSE;}
		}

		if ( lsb6 >= 0b0 && lsb6 <= 0b111 ){	// for shifting register
		
			//argv = { x, op(0=="<<" , 1==">>"), y, Arith(0==FALSE, 1==TRUE) }
			uint32_t argv[4]; 
			
			f = shift;				// use shift function			
			TMP1 = SA & ~(0xffffffe0);		// trucated SA
		
			switch(lsb6){
				//SLL
				case 0b000000:
					//RT    <<     SA  Logic
					argv[0] = RT;
					argv[1] = FALSE;
					argv[2] = SA;
					argv[3] = FALSE;
					break;
				
				//SRL	
				case 0b000010: 
					//RT    >>     SA  Logic
					argv[0] = RT;
					argv[1] = TRUE;
					argv[2] = SA;
					argv[3] = FALSE;
					break;
				
				//SRA
				case 0b000011:   
					//RT    <<     SA  Arith
					argv[0] = RT;
					argv[1] = TRUE;
					argv[2] = SA;
					argv[3] = TRUE;
					break;

				//SLLV	
				case 0b000100: 
					//RT   <<	 t-SA  Logic
					argv[0] = RT;
					argv[1] = FALSE;
					argv[2] = TMP1;
					argv[3] = FALSE;
					break;
				
				//SRLV
				case 0b000110:
					//RT    <<  t-SA  Logic
					argv[0] = RT;
					argv[1] = TRUE;
					argv[2] = TMP1;
					argv[3] = FALSE;
					break;
				
				//SRAV
				case 0b000111:      
					//RT    <<  t-SA  Arith
					argv[0] = RT;
					argv[1] = TRUE;
					argv[2] = TMP1;
					argv[3] = TRUE;
					break;
			}
			
		NEXT.REGS[rd] = f(argv);	
		}
		
		if ( lsb6 >= 0b1000 && lsb6 <= 0b1001 ){
			switch(lsb6){
			
				//JALR
				case 0b001001: 
					if(RD != 0x0) { TMP1 = rd;         }
					else          { TMP1 = 0xffffffff; }
					NEXT.REGS[TMP1] = NOW.PC+4;
				//JR
				case 0b001000:
					NEXT.PC = RS;
					break;
				}
			}
		
			
		if ( lsb6 >= 0b100000 && lsb6 <= 0b100011){ //argv = { x, op, y }
		
			f =  &arith;
			int32_t * argv;
			switch(lsb6){

				//ADD
				case 0b100000:
					argv[0] = RS;
					argv[0] = '+';
					argv[0] = RT;
					
					break;

				//SUB
				case 0b100010:
					argv[0] = RS;
					argv[0] = '-';
					argv[0] = RT;
					break;
				
				default:
					f = uarith;
					uint32_t argv;
				}
			switch(lsb6){

				//ADDU
				case 0b100001:
					argv[0] = RS;
					argv[0] = '+';
					argv[0] = RT;
					break;
				
				//SUBU
				case 0b100011:
					argv[0] = RS;
					argv[0] = '-';
					argv[0] = RT;
					break;
				}
		
			NEXT.REGS[rd] = f(argv);
			}
		
		if (lsb6 >= 0b100100 && lsb6 <= 0b100111){
		
			switch(lsb6){	
				case 0b100100: //AND:
					RESULT = RS & RT;
					break;
				case 0b100101: //OR:
					RESULT = RT | RS;
					break;
				case 0b100110: //XOR:
					RESULT = RS ^ RT;
					break;		
				case 0b100111: //NOR:
					RESULT = ~(RT | RS);
					break;
				}	
			NEXT.REGS[rd] = RESULT;
			}
		
		if (lsb6 >= 0b101010 && lsb6 <= 0b101011){
			switch(lsb6){
				case 0b101010: //SLT:
					int32_t intRT = RT, 
							intRS = RS;

					if (intRT < intRS)	{ RESULT = 1; }
					else				{ RESULT = 0; }
					break;

				case 0b101011: //SLTU:
					if (RT < RS)     { RESULT = 1; }
					else             { RESULT = 0; }
					break;
				}
			NEXT.REGS[rd] = RESULT;
			}
		
		if (lsb6 >= 0b011000 && lsb6 <= 0b011011){
			
			uint64_t ULONG;
			 int64_t  LONG;	
			
			uint32_t uword, remainder, quotient, i=0;
			 int32_t  word;
			
			uint32_t argv[3] = {RS, '*', RT};

			f = arith;
			switch(lsb6){

			//MULT:
			case 0b011000: 
				argv[1] = '*';
				LONG = f( argv );
				NEXT.HI = LONG >> 8u;
				NEXT.LO = LONG;
				break;

			//MFHI:
			case 0b010000: 
				NEXT.REGS[rd] = NOW.HI;
				break;

			//MFLO:
			case 0b010010: 
				NEXT.REGS[rd] = NOW.LO;
				break;

			//MTHI:
			case 0b010001: 
				NEXT.HI = RS;
				break;

			//MTLO:
			case 0b010011: 
				NEXT.LO = RS;
				break;

			//MULTU:
			case 0b011001: 
				f = uarith;
				argv[1] = '*';
				LONG = f( argv );
				NEXT.HI = LONG >> 8u;
				NEXT.LO = LONG;
				break;

			//DIV:
			case 0b011010: 
				argv[1] = '/';
				LONG = f( argv );
				NEXT.LO = LONG;
				NEXT.HI = RS % RT;
				break;

			//DIVU:
			case 0b011011: 
				f = uarith;
				argv[1] = '/';
				LONG = f( argv );
				NEXT.LO = LONG;
				NEXT.HI = RS % RT;
				break;
			}
		}
	}
	int16_t offset = instruction;
	
	int32_t target = offset<<2,
		immediate = offset;
	
	uint16_t uoffset = instruction;
	uint32_t jAddress = ( (instruction << 6) >> 4 ) | (NEXT.PC & 0xf0000000),
		uimmediate = uoffset;
	
	uint32_t virtual = RS + immediate;
	int32_t  uextend;
	uint32_t  extend;
	switch(msb6){
		
		//JAL:
		case 0b000011: 
			NEXT.REGS[31] = jAddress;

		//J:
		case 0b000010: 
			NEXT.PC = jAddress;
			break;
/*			
OP-here		rs	rt	offset
6		5	5	16
*/
		case 0b000100: //BEQ:
			if(RS == RT) { NEXT.PC = target; }
			break;
		case 0b000101: //BNE:
			if(RS != RT) { NEXT.PC = target; }
			break;
		case 0b000110: //BLEZ:
			if(RS <=  0) { NEXT.PC = target; }
			break;
		case 0b000111: //BGTZ:
			if(RS  >  0) { NEXT.PC = target; }
			break;
		case 0b001000: //ADDI:
			NEXT.REGS[rt] = (int32_t)RS + immediate;
			break;
		case 0b001001: //ADDIU:
			NEXT.REGS[rt] = RS + uimmediate;
			break;
		case 0b001010: //SLTI:
			if (( int32_t)RS < immediate ) 
			{ NEXT.REGS[rt] = 1; }
			else                
			{ NEXT.REGS[rt] = 0; }
			break;
			
		case 0b001011: //SLTIU:
			if ((uint32_t)RS < (uint32_t)immediate ) 
			{ NEXT.REGS[rt] = 1; }
			else                
			{ NEXT.REGS[rt] = 0; }
			break;

		case 0b001100: //ANDI:
			NEXT.REGS[rt] = (uint32_t)RS & uimmediate;
			break;

		case 0b001110: //XORI:
			NEXT.REGS[rt] = (uint32_t)RS ^ uimmediate;
			break;

		case 0b001101: //ORI:
			NEXT.REGS[rt] = (uint32_t)RS | uimmediate;
			break;

		case 0b001111: //LUI:
			NEXT.REGS[rt] = uimmediate << 16;
			break;
		case 0b100000: //LB:
			 int8_t  readB    = mem_read_32(virtual)>>24;
			 extend  = readB;
			 NEXT.REGS[rt] = extend;
			break;
		case 0b100001: //LH:
			
			if( virtual & ~0xfffffffe != 0 ) { printf("Address Error Exeption"); RUN_BIT=FALSE; }			// ----- raise address error exception
			
			 int16_t readHW    = mem_read_32(virtual)>>16;
			 extend  = readHW;
			 NEXT.REGS[rt] = extend;
			 break;
		case 0b100011: //LW:
			
			if( virtual & ~0xfffffffd != 0 ) { printf("Address Error Exeption"); RUN_BIT=FALSE; }			// ----- raise address error exception
			
			 int32_t readW    = mem_read_32(virtual);
			 extend  = readW;
			 NEXT.REGS[rt] = extend;
			 break;
		case 0b100100: //LBU:
			uint8_t  readBU    = mem_read_32(virtual)>>24;
			uextend  = readBU;
			 NEXT.REGS[rt] = extend;
			 break;
		case 0b100101: //LHU:
			
			if( virtual & ~0xfffffffe != 0 ) { printf("Address Error Exeption"); RUN_BIT=FALSE; }			// ----- raise address error exception
			
			uint16_t readHWU    = mem_read_32(virtual)>>16;
			uextend  = readHWU;
			 NEXT.REGS[rt] = extend;
			 break;
		case 0b101000: //SB:
			uint8_t  byte    = RT;
			 mem_write_32(virtual, byte);
		case 0b101001: //SH:
			uint16_t hw      = RT;
			 mem_write_32(virtual, hw);
		case 0b101011: //SW:
			uint32_t w      = RT;
			 mem_write_32(virtual, w);
	}

	switch(RT){
		//BLTZ
		case 0b00000: 
			if(RS >= 0x8000000){ NEXT.PC = target; }
			break;
		
		//BGEZ
		case 0b00001: 
			if(RS  < 0x8000000){ NEXT.PC = target; }
			break;

		//BLTZAL
		case 0b10000:
			if(RS >= 0x8000000){ NEXT.PC = target; }
			NEXT.REGS[31] = target;
			break;

		//BGEZAL
		case 0b10001:
			if(RS  < 0x8000000){ NEXT.PC = target; }
			NEXT.REGS[31] = target;
			break;
	}
//RUN_BIT = FALSE;
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
	CPU_State NOW  = CURRENT_STATE;
    CPU_State NEXT = NEXT_STATE;
    uint32_t instruction = mem_read_32(NOW.PC);
	decode(instruction);
    NEXT.PC = NOW.PC + 4;
	NEXT_STATE = NEXT;
}

