#ifndef KernelFunctions_H
#define KernelFunctions_H

/*
__/\\\\\\\\\\\\\_____/\\\\\\\\\\\__/\\\\\\\\\\\\____        
 _\/\\\/////////\\\__\/////\\\///__\/\\\////////\\\__       
  _\/\\\_______\/\\\______\/\\\_____\/\\\______\//\\\_      
   _\/\\\\\\\\\\\\\/_______\/\\\_____\/\\\_______\/\\\_     
    _\/\\\/////////_________\/\\\_____\/\\\_______\/\\\_    
     _\/\\\__________________\/\\\_____\/\\\_______\/\\\_   
      _\/\\\___________/\\\___\/\\\_____\/\\\_______/\\\__  
       _\/\\\__________\//\\\\\\\\\______\/\\\\\\\\\\\\/___
        _\///____________\/////////_______\////////////_____
-> Name:  KernelFunctions.h
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <cstdint>

#define TRUE    1
#define FALSE   0
#define PRIVATE static

#define SVC()   __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP

#define STACKSIZE   1024

enum kernelcallcodes {
    GETID,
    NICE,
    TERMINATE
};

struct kcallargs
{
    uint32_t code;
    uint32_t rtnvalue;
    uint32_t arg1;
    uint32_t arg2;
};

void set_LR(volatile uint32_t);
void set_PSP(volatile uint32_t);
void set_MSP(volatile uint32_t);

uint32_t get_PSP();
uint32_t get_MSP();
uint32_t get_SP();

void volatile save_registers();
void volatile restore_registers();

void assignR7(volatile uint32_t data);

/* Cortex default stack frame */
typedef struct stack_frame
{
    /* Registers saved by s/w (explicit) */
    /* There is no actual need to reserve space for R4-R11, other than
     * for initialization purposes.  Note that r0 is the h/w top-of-stack.
     */
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    /* Stacked by hardware (implicit)*/
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} stack_frame_t;

/* Process control block */
struct pcb
{
    /* Stack pointer - r13 (PSP) */
    uint32_t sp;
    /* Links to adjacent PCBs */
    struct pcb *next;
    struct pcb *prev;
};

#endif /* KernelFunctions_H */
