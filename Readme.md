`T1-compile-and-run.cpp` - Reads program code written in T1-Assmbler from  `input.fasm` and compiles it to a executable file on the T1 machine emulator. Then runs the emulator program.

`T1-load-and-run.cpp` - Reads the executable file `input.bin` and executes the T1 processor machine instructions contained in that file.

### Table of Contents
[1. T1 computer architecture](#1)  

[2. Processor T1 description](#2)  

[2.1 Instruction set of processor T1](#21) 

[3. T1OS executable file format](#3)

[4. Examples of programs with comments](#4)

[4.1 A simple program for squaring a number](#41)

[4.2 A program for squaring a number using functions](#42)

[4.3 A program that uses a recursive factorial function](#43)

[4.4 A program that enters two real numbers and prints their sum, difference, product, and quotient quotient](#44)

[4.5 A program that brute-force calculates the number of lucky tickets lucky tickets](#45)

<a name="1"/>

## 1. T1 computer architecture
T1 - machine with Von Neumann architecture with address space in 2^20 words, each of which takes 32 bits. 
Each command takes exactly one word, the 8 high bits of which are the operation code, and the use of the remaining 24 bits depends on the operation. The computer has sixteen single word (32 bits each) registers r0-r15, their values are given in Table 1.

#### Table 1: T1 processor register allocation
|               |                          |
|----------------|-------------------------------|
|r0-r12          |freely used       |
|r13          |call frame pointer   |
|r14          |stack pointer|
|r15          |program counter       |
|flags          |result of comparison operation      |

Depending on the operation code, each command can be one of the following formats:
 - **RM** - 8 high bits command code, 4 next bits register code (receiver or source), 20 low bits memory address as an unsigned number from 0 to 2^20-1. An example of such a command:
```
load r0, 12323
```
  - **RR** - 8 bits the command code, 4 bits the receiver register code, 4 bits the source register code, 1 6-bit source modifier, the number with a sign from -2^15 to 2^15 - 1. Examples of this command:
```
mov r1, r2, -123
add r1, r2, 0
```
In commands of this type the immediate operand is always added to the source register. If this is not required, 0 is put in the direct operand field.
  - **RI** - 8 bits the command code, 4 bits the pickup register code, 20 bits the immediate operand, a number with a sign from -2^19 to 2^19. An example of this command:
```
ori r2, 64
```
  - **J** - 8 bits command code, 4 bits are ignored, 20 lower bits - address in memory as an unsigned number from 0 to 2^20 - 1. An example of this command:
```
calli 3121
```

For ease of writing an assembler program, all arguments in the command are mandatory.

### Example program
```
; A program that enters a number from the keyboard, adds 1 to it and
; outputs the result
syscall r0, 100 ; query a number into the r0 register
addi r0, 1      ; r0++
syscall r0, 102 ; outputs value of r0
lc r1, 10       ; loading code of ’\n’
syscall r1, 105 ; character output
syscall r1, 0   ; exit
```
<a name="2"/>

## 2. Processor T1 description
<a name="21"/>

### 2.1 Instruction set of processor T1
#### Table 2: Description of the machine instructions 
|Code   | Name         |Format  |Description  |
|-------|--------------|--------|-------------|
|0      |`halt`        | RI     |Processor stop.<br /> ``` halt r1 0 ```|
|1      |`syscall`     | RI     |Calling the operating system. <br /> ``` syscall r0, 100```|
|2      |`add`         | RR     |Addition of registers. To R1 (register-first argument) is added the value of R2 (register-second argument) modified by the direct operand. <br /> ``` add r1, r2, 3 ``` <br /> To the register r1 is added r2+3.|
|3      |`addi`        | RI     |Addition of a register to the direct operand. The value of Imm (direct operand) is added to the content of R1. <br /> ``` addi r4, 10 ```|
|4      |`sub`         | RR     |Subtract registers. The content of R2 are subtracted from R1. <br /> ``` sub r3, r5, 5 ``` <br /> From the register r3 is subtracted r5+5.|
|5      |`subi`        | RI     |Subtract from the register of the direct operand. <br /> ``` subi r4 1 ``` |
|6      |`mul`         | RR     |Multiplication of registers. The contents of R1 are multiplied by R2 . The result is placed in the pair of registers, beginning with R1.<br /> ``` mul r3, r10, 0 ``` <br /> The result of the product of r3 and r10 will be placed in r3 and r4. |
|7      |`muli`        | RI     |The multiplication of register R1 by Imm. The result is placed in the register pair beginning with R1.<br /> ``` muli r5, 100 ``` <br /> After execution: in the register r5 the lowest 32 bits of the first result, in the register r6 - the highest 32 bits. |
|8      |`div`         | RR     | Register division. The lower 32 bits of the first operand are in the register R1, the higher ones are in the register whose number is 1 more than R1. The second operand is in the register. The partial is placed in the register R1, the remainder in the next register. <br /> ``` div r3, r10, 0 ``` <br /> r3 will contain the quotient of the division of the pair (r3,r4) by r10, r4 - the remainder of this division.|
|9      |`divi`        | RI     |Dividing a register by the direct operand. Similarly div. <br /> ``` divi r3 10 ``` <br /> r3 will contain the quotient of division of the pair (r3,r4) by 10, r4 is the remainder of this division.|
|12     |`lc`          | RI     |Loading constant Imm into register R1. To load constants > 2^20 you need additional shift and logical addition commands. <br /> ``` lc r7, 123 ``` <br /> r7 will contain 123. |
|13     |`shl`         | RR     |Shift the bits in register R1 to the left by the value of register R2. <br /> ``` shl r1, r2, 0 ``` |
|14     |`shli`        | RI     |Shift the bits in register R 1 to the left by Imm. <br /> ``` shli r1, 2 ```|
|15     |`shr`         | RR     |Shift the bits in register R1 to the right by the value of register R2. Shift of 32 or more bits resets the register R1. <br /> ``` shr r1, r2, 0 ```|
|16     |`shri`        | RI     |Shift bits in the register R1 again by Imm. <br /> ``` shri r1, 2 ```|
|17     |`and`         | RR     |Logic AND registers R1 and R2 . The result is in the register R1. <br /> ``` and r1 r2 0 ```|
|18     |`andi`        | RI     |Logical AND over the register R1 and Imm.<br /> ``` andi r1, 255 ```|
|19     |`or`          | RR     |Logical OR of the registers R1 and R2. The result is in the register R1.<br /> ``` or r1, r2, 0 ```|
|20     |`ori`         | RI     |Logical OR over the register R1 and Imm.<br /> ``` ori r1, 255 ```|
|21     |`xor`         | RR     |Logical exclusive OR of registers R1 and R2. The result is in the register R1.<br /> ``` xor r1 r2 0 ```|
|22     |`xori`        | RI     |Logical exclusive OR over the register R1 and Imm.<br /> ``` xori r1 255 ```|
|23     |`not`         | RI     |Bitwise NOT over all bits of R1. The immediate operand is ignored, but it is present in the command for compiler simplicity. <br /> ``` not r1, 0 ```|
|24     |`mov`         | RR     |Transfer from the register R2 to the register R1 .<br /> ``` mov r0, r3, 22 ``` <br /> The value r3+22 is placed in the register r0.|
|32     |`addd`        | RR     |Real addition of the registers R0 and R1. The real numbers occupy a pair of registers, the least of which appear in the code of the operation. <br /> ``` addd r2, r5, 0 ``` <br /> To the real number occupying the pair of registers (r2,r3) the value of the real number from the pair of registers (r5,r6) is added.|
|33     |`subd`        | RR     |The real subtraction of registers R0 and R1. <br /> ``` subd r2, r5, 0 ``` <br /> From the real number occupying the pair of registers (r2,r3) is subtracted the value of the real number from the pair of registers (r5,r6). |
|34     |`muld`        | RR     |Real multiplication of registers R0 and R1. <br /> ``` muld r2, r5, 0 ``` <br /> The real number occupying the pair of registers (r2,r3) is multiplied by the value of the real number from the pair of registers (r5,r6).|
|35     |`divd`        | RR     |The real division of registers R0 and R1. <br /> ``` divd r2, r5, 0 ``` <br /> The real number occupying the pair of registers (r2,r3) is divided by the value of a real number from the pair of registers (r5,r6).|
|36     |`itod`        | RR     |Conversion of an integer R2 into a real R1.<br /> ``` itod r2, r5, 0 ``` <br /> The value located in the register r5 is converted into a real number and placed in the pair of registers (r2,r3).|
|37     |`dtoi`        | RR     |Conversion of a real number R2 into an integer R1.<br /> ``` dtoi r2, r5, 0 ``` <br /> The real number occupying the pair of registers (r5,r6) is converted into integer number by discarding the fractional part. If the number does not fit in the register, an error occurs.|
|38     |`push`        | RI     |Send the value contained in the register R1 with addition of Imm to it to the stack. <br /> ``` push r0, 255 ``` <br /> Place number equal to r0+255 on the stack. The stack pointer (r14) will decrease by 1.|
|39     |`pop`         | RI     |Extract the number from the stack, put it into the register R1 and then add Imm to the register R1.<br /> ``` push r2, 0;  pop r3, 1 ``` <br /> After execution r3 will contain r2+1.|
|40     |`call`        | RI     |Call the function whose address is in R2 + Imm. <br /> ``` call r0, r5, 0 ``` <br /> Call a function whose address is taken from the register r5. When the function is called, the address of the instruction that follows the current instruction is put on the stack and control is transferred to the specified address. The same address is also placed in the register r0. |
|41     |`calli`       | RI     |Call the function the address of which is located in imm20. <br /> ``` calli 13323 ```  <br />  Call a function that starts with the operator at address 13323. When the function is called, the address of the instruction that follows the current one is put on the stack and control is transferred to the specified address. |
|42     |`ret`         | RI     |Return from the function. <br /> ``` ret r0, 0 ```  <br />  Return from the called function to the called function. The address is taken from the stack, by which control is passed and that will be the address of the next executable word. The immediate argument shows the number of additional words to advance the stack pointer by (it must match the number of arguments in the function call). |
|43     |`cmp`         | RI     |Comparison.<br /> ``` cmp r0, r1, 0 ``` <br /> Compare r0 with r1. The result is written to the flag register, which is used in transition commands.|
|44     |`cmpi`        | RI     |Comparison with a constant.<br /> ``` cmpi r0, 0 ``` Compare r0 with zero. Set appropriate flag. |
|45     |`cmpd`        | RR     |Comparison of real numbers.<br /> ``` cmpd r1, r4, 0 ```  <br /> Compare the pair (r1,r2) with (r4,r5). Set appropriate flag.|
|46     |`jmp`         | J      |Unconditional jump.<br /> ``` jmp 2212 ``` <br /> The next executable command will be at address 2212.|
|47     |`jne`         | J      |Jump if the condition !=.<br /> ``` jne 2212 ``` <br /> If the flag register contains condition !=, the next instruction to be executed will be at address 2212.|
|48     |`jeq`         | J      |Jump if condition == is present. <br /> ``` jeq 2212``` <br /> If the flags register contains the condition ==, then the next instruction to be executed is at address 2212, otherwise the program execution is not broken.|
|49     |`jle`         | J      |Jump if condition <=. <br /> ``` jle 2212 ``` <br /> If the flag register contains the <= condition, the next instruction to be executed is at address 2212, otherwise the program execution is not broken.|
|50     |`jl`          | J      |Jump if condition <.<br /> ``` jl 2212 ``` <br /> If the flag register contains the < condition, the next instruction to be executed will be at address 2212, otherwise the program execution is not broken.|
|51     |`jge`         | J      |Jump if condition >=.<br /> ``` jge 2212 ``` <br /> If the flag register contains the >= condition, the next instruction to be executed will be at address 2212, otherwise the program execution is not disturbed. |
|52     |`jg`          | J      |Jump if condition >.<br /> ``` jg 2212 ``` <br /> If the flag register contains the > condition, the next executable command will be located at address 2212, otherwise the program execution is not disturbed. |
|64     |`load`        | RM     |Load from memory into register <br /> ``` load r0, 12345 ``` <br /> The content from the memory cell at address 12345 is copied into register r0.|
|65     |`store`       | RM     |Load from the register into the memory. <br /> ``` store r0, 12344 ``` <br /> The contents from register r0 are copied into the memory cell at address 12344.|
|66     |`load2`       | RM     |Load from memory into the pair of registers.<br /> ``` load2 r0, 12345 ``` <br /> The contents from the memory cells at address 12345 and 12346 are copied into registers r0 and r1.|
|67     |`store2`      | RM     |Load from the pair of registers into the memory. <br /> ``` store2 r0, 12345 ``` <br /> The contents from register r0 are copied to address 12345, and from cell r1 to address 12346. |
|68     |`loadr`       | RR     |Load from memory into the register. <br /> ``` loadr r0, r1, 15 ``` The contents from the memory cell at address (r1+15) are copied to register r0.|
|69     |`storer`      | RR     |Load from the register into the memory. <br /> ``` storer r0, r13, 3 ``` <br /> The contents from the register r0 is copied to the memory cell at address r13+3. |
|70     |`loadr2`      | RR     |Load from memory into a pair of registers.<br /> ``` loadr2 r0, r13, 7 ``` <br /> The contents from the memory cells at address r13+7 and r13+8 are copied to registers r0 and r1.|
|71     |`storer2`     | RR     |Load from a pair of registers into memory.<br /> ``` storer2 r0, r13, 11 ``` <br /> The contents from register r0 are copied to address r13+11, and from cell r1 to address r13+12.|


#### Table 3: Executing operating system call codes 
|Code   | Arguments     |  Description  |
|-------|--------------|-------------|
|0      |`EXIT`        |return code.    |
|100      |`SCANINT`        |   |
|101    |`SCANDOUBLE`        |  |
|102    |`PRINTINT`        |   |
|103    |`PRINTDOUBLE`        |   |
|104    |`GETCHAR`        |   |
|105    |`PUTCHAR`        |C   |

**For convenience, all command codes are collected in an enumerated code type:**
```
enum code {
	HALT = 0,
	SYSCALL = 1,
	ADD = 2,
	ADDI = 3,
	SUB = 4,
	SUBI = 5,
	MUL = 6,
	MULI = 7,
	DIV = 8,
	DIVI = 9,
	LC = 12,
	SHL = 13,
	SHLI = 14,
	SHR = 15,
	SHRI = 16,
	AND = 17,
	ANDI = 18,
	OR = 19,
	ORI = 20,
	XOR = 21,
	XORI = 22,
	NOT = 23,
	MOV = 24,
	ADDD = 32,
	SUBD = 33,
	MULD = 34,
	DIVD = 35,
	ITOD = 36,
	DTOI = 37,
	PUSH = 38,
	POP = 39,
	CALL = 40,
	CALLI = 41,
	RET = 42,
	CMP = 43,
	CMPI = 44,
	CMPD = 45,
	JMP = 46,
	JNE = 47,
	JEQ = 48,
	JLE = 49,
	JL = 50,
	JGE = 51,
	JG = 52,
	LOAD = 64,
	STORE = 65,
	LOAD2 = 66,
	STORE2 = 67,
	LOADR = 68,
	LOADR2 = 69,
	STORER = 70,
	STORER2 = 71
};
```
<a name="3"/>

## 3. T1OS executable file format
All programs for the T1 processor run under the T1OS operating system. To run a program, an executable file must be generated which contains meta-information (section descriptions) and the sections themselves. The program is stored in external memory (say, on a hard disk or SDD) as a set of bytes. The word consists of 4 bytes in the following order: 3210, that is, in LSB format. The header of an executable file contains exactly 512 bytes, the contents of which are described below. Starting from the 512 bytes, sections of code, constants and data are placed in sequence and are loaded into the virtual memory starting from address 0. Program execution starts from the address written in the header of the executable file, and the initial value of the stack is determined by the corresponding field in the header. The format of the executable file is given in Table 4.
#### Table 4: T1OS executable file format 
|Code   | Content     |
|-------|-------------|
|0..15      |The ASCII string "ThisT12Exec"    |
|16..19      |Program code size    |
|20..23      |The size of the program constants    |
|24..27      |Program data size    |
|28..31      |The starting address of the program execution    |
|32..35      |The starting address of the stack    |
|36..39      |Target processor ID    |
|512..      |Code segment    |
|      |Constant segment    |
|      |Data segment    |
<a name="4"/>

## 4. Examples of programs with comments
<a name="41"/>

### 4.1 A simple program for squaring a number
```
main:
	syscall r0, 100   ;enter a number from the standard input into the register r0
	mov r2, r0, 0     ;copying of register r0 to register r2
	mul r0, r2, 0     ;register pair (r0,r1) contains product
	syscall r0, 102   ;output of register r0 contents (low-order part)
	lc r0, 10         ;loading constant 10 ('n') into register r0
	syscall r0, 105   ;output '\n'
	lc r0, 0          ;
	syscall r0, 0     ;program exit with code 0
	end main          ;start execution with main
```
<a name="42"/>

### 4.2 A program for squaring a number using functions
```
sqr:                     ;sqr function with one argument on the stack
	loadr r0, r14, 1     ;load r0 register with the first argument
	mov r2, r0, 0        ;copy r0 into r2
	mul r0, r2, 0        ;(r0,r1) = r0*r2
	ret 1                ;return from the function, taking the argument off the stack
intout:					 ;function that prints the argument + '\n'
	load r0, r14, 1      ;loading of the first argument into r0
	syscall r0, 102      ;output r0
	lc r0, 10            ;loading '\n'
	syscall r0, 105      ;output '\n'
	ret 1                ;return
main:
	syscall r0, 100      ;reading to r0
	push r0, 0           ;put r0+0 on the stack
	calli sqr            ;call the sqr function. The function will leave the result in r0.
	push r0, 0           ;pass result to intout function
	calli intout         ;and call it
	lc r0, 0             ;
	syscall r0, 0        ;exit(0)
	end main             ;
```
<a name="43"/>

### 4.3 A program that uses a recursive factorial function
```
fact:
	loadr r0, r14, 1
	cmpi r0, 1
	jg skip0
	lc r0, 1
	ret 1
skip0:
	push r0, 0
	subi r0, 1
	push r0, 0
	calli fact
	pop r2, 0
	mul r0, r2, 0
	ret 1
main:
	syscall r0, 100
	push r0, 0
	calli fact
	syscall r0, 102
	lc r0, 10
	syscall r0, 105
	lc r0, 0
	syscall r0, 0
	end main
```
<a name="44"/>

### 4.4 A program that enters two real numbers and prints their sum, difference, product, and quotient quotient
```
fout:
	syscall r0, 103
	lc r0, 10
	syscall r0, 105
	ret 0
main:
	syscall r2, 101
	syscall r4, 101

	mov r0, r2, 0
	mov r1, r3, 0
	addd r0, r4, 0
	calli fout
	mov r0, r2, 0
	mov r1, r3, 0
	subd r0, r4, 0
	calli fout
	mov r0, r2, 0
	mov r1, r3, 0
	muld r0, r4, 0
	calli fout
	mov r0, r2, 0
	mov r1, r3, 0
	divd r0, r4, 0
	calli fout
	lc r0, 100
	itod r0, r0, 0
	calli fout
	lc r0, 0
	syscall r0, 0
	end main
```
<a name="45"/>

### 4.5 A program that brute-force calculates the number of lucky tickets lucky tickets
```
start: lc r6, 0			;counter
	lc r0, 0			;first digit i1
l0: cmpi r0, 10 		; for (i1 = 0; i1 < 10; i++) {
	jge e0
	lc r1, 0
l1: cmpi r1, 10
	jge e1
	lc r2, 0
l2: cmpi r2, 10
	jge e2
	lc r3, 0
l3: cmpi r3, 10
	jge e3
	lc r4, 0
l4: cmpi r4, 10
	jge e4
	lc r5, 0
l5: cmpi r5, 10
	jge e5
	mov r7, r0, 0 		;r7 = r0+r1+r2
	add r7, r1, 0
	add r7, r2, 0
	mov r8, r3, 0		;r8 = r3+r4+r5
	add r8, r4, 0
	add r8, r5, 0
	cmp r7, r8, 0
	jne skip
	addi r6, 1			;counter++
skip: addi r5, 1
	jmp l5
e5: addi r4, 1
	jmp l4
e4: addi r3, 1
	jmp l3
e3: addi r2, 1
	jmp l2
e2: addi r1, 1
	jmp l1
e1: addi r0, 1
	jmp l0
e0: syscall r6, 102 	; print counter, ’\n’
	lc r0, 10
	syscall r0, 105
	lc r0, 0
	syscall r0, 0		;exit(0)
	end start
```
