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

// Turns debugging printouts on with 1 or 2 and off otherwise (At 0)
#define DEBUGGING_TRAIN 0

#define MAX_PRIORITY (uint32_t) P_FIVE

#define TRUE    1
#define FALSE   0
#define PRIVATE static

#define SVC()       __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9    //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD    //LR value: exception return using PSP as SP
#define THUMB_MODE 0x01000000

#define STACKSIZE   512
// #define STACKSIZE   2048

#define INVALID_NUM UINT32_MAX

#define MAX_MAILBOXES_PER_PROCESS 3

#define MIN(x, y) ((x < y) ? x : y )
#define MAX(x, y) ((x > y) ? x : y )

typedef enum kernel_responses {
    FAILURE_KR,
    NO_MSG_KR,
    SUCCESS_KR
} kernel_responses_t;

typedef enum priorities {
    P_ONE,
    P_TWO,
    P_THREE,
    P_FOUR,
    P_FIVE,
} priority_t;

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
    uint8_t mailbox_numbers[MAX_MAILBOXES_PER_PROCESS] = { 0, 0, 0 }; // [3]

    /* Links to adjacent PCBs */
    pcb *next;
    pcb *prev;
} pcb_t;

// Size of the mailboxes
typedef enum letter_size {
    ZERO_CHAR    = 0,
    ONE_CHAR     = 1,
    SMALL_LETTER = 16,
    BIG_LETTER   = 256,
} letter_size_t;

// Letter sizes
typedef struct empty_msg { // ZERO_CHAR
    empty_msg() : msg_size(ZERO_CHAR) {}
    uint8_t msg_size = 0;
    uint8_t msg_src;
} empty_msg_t;

typedef struct one_char_msg { // ONE_CHAR
    uint8_t msg_size = 1;
    uint8_t msg_src = 0;
    char msg[ONE_CHAR];
} one_char_msg_t;


typedef struct small_letter_msg { // BIG_LETTER
    uint8_t msg_size;
    uint8_t msg_src;
    char msg[SMALL_LETTER];
} small_letter_msg_t;

typedef struct big_letter_msg { // BIG_LETTER
    uint8_t msg_size;
    uint8_t msg_src;
    char msg[BIG_LETTER];
} big_letter_msg_t;

typedef enum kernelcallcodes {
    GETID,
    NICE,
    TERMINATE,
    SEND,
    RECV,
    BIND
} k_call_code_t;

typedef struct kcallargs
{
    k_call_code_t kcode;
    uint32_t rtnvalue;
    union {
        // Priority Argument
        priority_t priority; // 4 Bytes
        // Send & Recv Arguments
        struct  // 8 Bytes
        {
            uint8_t src_q;
            uint8_t dst_q;
            void* msg_ptr;
            uint16_t msg_len;
            bool enable_sleep;
        }; 
        // Bind Arguments
        struct 
        {
            uint8_t req_q;
            letter_size_t q_size;
            uint8_t mailbox_size;
        };
    };
} kcallargs_t;

typedef struct mailbox {
    mailbox() : currently_owned(false), currently_blocked(false), mailbox_ptr(0) {}
    bool currently_owned;
    bool currently_blocked;
    pcb_t* owner_pcb;
    letter_size_t letter_size;
    void* mailbox_ptr;
    // kcallargs_t mailbox_kca;
    kcallargs_t* kcaptr; // TODO: THIS IS MAYBE A PROBLEM
} mailbox_t;

#endif /* OSLibrary_H */
