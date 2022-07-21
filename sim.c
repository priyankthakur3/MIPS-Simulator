/*
 *Stevens Institute of Technology
 *Course: CPE 517 Digital and Computer System Architecture
 *Purpose: Lab 1 MIPS simulator
 *Programmers: Priyank Thakur (20009546), Venkata Sai Ajay Pudi (20008652)
 */
#include <stdio.h>
#include "shell.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

uint8_t op;
uint32_t instruction;
uint32_t addr_rs, addr_rt, addr_rd, addr_i, addr_i_current;
uint32_t rs, rt, rd; // to save the register value
uint8_t func;
uint16_t itemp; // for save immidiate number

// please take a loot at https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf
//				 or
//   https://web.cse.ohio-state.edu/~crawfis.3/cse675-02/Slides/MIPS%20Instruction%20Set.pdf

void fetch()
{
    // printf("fetch\n");
    instruction = mem_read_32(CURRENT_STATE.PC); // the instruction
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode()
{
    // printf("decode\n");
    op = instruction >> 26; // find the 31-26 bit

    //  printf("the op: %x ",op);
    // if op==0, it is R type
    if (op == 0)
    {
        printf("\n--------------------------------\n");
        printf("R Type Instruction\n");
        addr_rs = (instruction >> 21) & 0x1f;
        addr_rt = (instruction >> 16) & 0x1f;
        addr_rd = (instruction >> 11) & 0x1f;
        rs = CURRENT_STATE.REGS[addr_rs]; // 2^5=32
        rt = CURRENT_STATE.REGS[addr_rt];
        //  rd=CURRENT_STATE.REGS[addr_rd];
        func = instruction & 0x3f;
    }

    // Logic for I Type instruction Addi and addiu
    if (op == 8)
    { // addi:001000
        printf("\n--------------------------------\n");
        printf("I Type Instruction\n");
        printf("\naddi instruction \n");

        // decoding address of rs and rt from instruction
        addr_rs = (instruction >> 21) & 0x1f;
        addr_rt = (instruction >> 16) & 0x1f;
        rs = CURRENT_STATE.REGS[addr_rs]; // 2^5=32
        itemp = instruction & 0x0000ffff;

        if (itemp > 32767)
        { // 2^15-1=32767 is the largest number
            itemp = ~itemp + 1;
            rt = rs - itemp;
        }
        else
        {
            rt = rs + itemp;
        }
        NEXT_STATE.REGS[addr_rt] = rt; // It is fine if you just execute here instead of executing in execute() function.

        // set NEGATIVE FLAG incase if result is negative (MSB will be 1 since MIPS use 2's Complement)
        if (((rd >> 31) & 1) == 1)
        {
            CURRENT_STATE.FLAG_N = 1;
        }
        printf("\nrs: %u, itemp: %u, rt: %u", rs, itemp, rt);
    }

    if (op == 9)
    { // addiu instruction 001001
        printf("\n--------------------------------\n");
        printf("I Type Instruction\n");
        printf("\naddiu Instruction\n");

        // decoding rs and rt address from instruction
        addr_rs = (instruction >> 21) & 0x1f;
        addr_rt = (instruction >> 16) & 0x1f;
        rs = CURRENT_STATE.REGS[addr_rs]; // 2^5=32
        itemp = instruction & 0x0000ffff;

        rt = rs + itemp;
        NEXT_STATE.REGS[addr_rt] = rt;

        // set zero flag if output is zero
        if (rt == 0)
        {
            CURRENT_STATE.FLAG_Z = 1;
        }

        // if value extends 2^32 then set overflow and carry flag

        if (rd > 4294967296)
        {
            CURRENT_STATE.FLAG_V = 1;
            CURRENT_STATE.FLAG_C = 1;
        }

        printf("\nrs: %u, itemp: %u, rt: %u", rs, itemp, rt);
    }

    // Logic for J type instruction
    //  op code is 000010 or 000011 then J type instruction
    if (op == 2 || op == 3)
    {
        printf("\n--------------------------------\n");
        printf("J Type instruction \n");
        // Decode address
        addr_i = (instruction & 0x3ffffff);
        // Current State Address
        addr_i_current = CURRENT_STATE.PC & 0xf;
        // Next State address by adding current state
        // and new state address
        addr_i = addr_i_current | (addr_i << 2);

        if (op == 2)
        { // J jump instruction
            printf("\nj (Jump) Instruction");
            // assigning new address for next state
            NEXT_STATE.PC = addr_i;
            printf("\nJump to Address: %u", addr_i);
        }

        if (op == 3)
        {
            printf("\njal instruction");
            // this is hardcoded register value for MIPS jal instruction
            // https://web.cse.ohio-state.edu/~crawfis.3/cse675-02/Slides/MIPS%20Instruction%20Set.pdf
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            // assigning new address for next state
            NEXT_STATE.PC = addr_i;
            printf("\nJump to Address: %u", addr_i);
        }
    }
}

void execute()
{

    // printf("execute\n");
    if (op == 0)
    {
        switch (func)
        {
        case 32: // add:100000
            printf("\nadd instruction \n");
            rd = 0;
            //      if(rt>2147483647)

            // add operation simulation
            rd = rt + rs;
            NEXT_STATE.REGS[addr_rd] = rd;
            // set ZERO Flag incase of zero output
            if (rd == 0)
            {
                CURRENT_STATE.FLAG_Z = 1;
            }

            // if value extends 2^31 then set overflow and carry flag

            if (rd > 2147483648)
            {
                CURRENT_STATE.FLAG_V = 1;
                CURRENT_STATE.FLAG_C = 1;
            }

            // set NEGATIVE FLAG incase if result is negative (MSB will be 1 since MIPS use 2's Complement)
            if (((rd >> 31) & 1) == 1)
            {
                CURRENT_STATE.FLAG_N = 1;
            }

            printf("\nrt: %u, rs: %u, rd: %u", rt, rs, rd);

            break;

        case 33: // addu instruction 100001
            printf("\naddu instruction\n");
            rd = 0;

            // add operation simulation
            rd = rt + rs;
            NEXT_STATE.REGS[addr_rd] = rd;
            // set ZERO Flag incase of zero output
            if (rd == 0)
            {
                CURRENT_STATE.FLAG_Z = 1;
            }
            // if value extends 2^32 then set overflow and carry flag

            if (rd > 4294967296)
            {
                CURRENT_STATE.FLAG_V = 1;
                CURRENT_STATE.FLAG_C = 1;
            }
            // set NEGATIVE FLAG incase if result is negative (MSB will be 1 since MIPS use 2's Complement)
            if (((rd >> 31) & 1) == 1)
            {
                CURRENT_STATE.FLAG_N = 1;
            }
            printf("\nrt: %u, rs: %u, rd: %u", rt, rs, rd);
            break;

        case 34: // sub 100010
            printf("\nsub instruction \n");
            rd = 0;

            // sub operation simulation
            rd = rs - rt;
            NEXT_STATE.REGS[addr_rd] = rd;

            // set ZERO Flag incase of zero output
            if (rd == 0)
            {
                CURRENT_STATE.FLAG_Z = 1;
            }

            // set NEGATIVE FLAG incase if result is negative (MSB will be 1 since MIPS use 2's Complement)
            if (((rd >> 31) & 1) == 1)
            {
                CURRENT_STATE.FLAG_N = 1;
            }

            // if value is less than 2^31 then set overflow and carry flag

            if (rd > -2147483648)
            {
                CURRENT_STATE.FLAG_V = 1;
                CURRENT_STATE.FLAG_C = 1;
            }
            printf("\nrt: %u, rs: %u, rd: %u", rt, rs, rd);
            break;

        case 36: // And instruction 100100
            printf("\nand Instruction \n");
            rd = 0;

            // AND operation simulation
            rd = rs & rt;
            NEXT_STATE.REGS[addr_rd] = rd;
            printf("\nrt: %u, rs: %u, rd: %u", rt, rs, rd);
            break;

        case 12: // system call:001100
            printf("\nSystem Call instruction");
            if (CURRENT_STATE.REGS[2] == 10)
            { // v0==10 then exit
                printf("\nsystemcall: exit\n");
                RUN_BIT = FALSE;
            }
            if (CURRENT_STATE.REGS[2] == 1)
            {                               // v0==1: print int
                rt = CURRENT_STATE.REGS[4]; // a0 is 4th register
                printf("\nPrint the number: %d\n", rt);
            }
            break;
        default:
            break;
        }
    }
}
void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    fetch();
    decode();
    execute();
}
