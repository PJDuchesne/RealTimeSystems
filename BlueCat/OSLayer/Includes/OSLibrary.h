#ifndef OSLibrary_H
#define OSLibrary_H

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
-> Name:  OSLibrary.h
-> Date: Oct 22, 2018  (Created)
-> Author: Paul Duchesne (B00332119) -> Original Structures by Dr Hughes
-> Contact: pl332718@dal.ca
*/

#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>

#define MAX_PRIORITY (uint32_t) P_FIVE // TODO: MAGIC NUMBER

#define TRUE    1
#define FALSE   0
#define PRIVATE static

#define SVC()       __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP
#define THUMB_MODE 0x01000000

#define STACKSIZE   1024

typedef enum Priorities {
    P_ONE,
    P_TWO,
    P_THREE,
    P_FOUR,
    P_FIVE,
} priority_t;

typedef enum kernelcallcodes {
    GETID,
    NICE,
    TERMINATE
} k_call_code_t;

typedef struct kcallargs
{
    k_call_code_t kcode;
    uint32_t rtnvalue;
    uint32_t arg1;
    uint32_t arg2;
} kcallargs_t;

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
typedef struct pcb
{
    uint32_t *stack_start;
    uint32_t stack_ptr; // r13
    uint32_t pid;
    uint32_t q_count;
    priority_t priority;
    std::string name;

    /* Links to adjacent PCBs */
    pcb *next;
    pcb *prev;
} pcb_t;

#endif /* OSLibrary_H */
