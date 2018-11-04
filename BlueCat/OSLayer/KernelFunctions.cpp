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
-> Name:  KernelFunctions.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Dr. Larry Hughes, Modified by Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/KernelFunctions.h"
#include <ISRLayer/Includes/GlobalConfig.h>

#include <iostream>
#include <string>

// Forward declaration
class OperatingSystem;
class PostOffice;

// Hughes version
// void SVCall()
// {

//     /* Supervisor call (trap) entry point
//        Using MSP - trapping process either MSP or PSP (specified in LR)
//        Source is specified in LR: F9 (MSP) or FD (PSP)
//        Save r4-r11 on trapping process stack (MSP or PSP)
//        Restore r4-r11 from trapping process stack to CPU
//        SVCHandler is called with r0 equal to MSP or PSP to access any arguments
//     */

//     /* Save LR for return via MSP or PSP */
//     __asm("   PUSH  {LR}");

//     /* Trapping source: MSP or PSP? */
//     __asm("   TST   LR,#4");   // Bit #4 indicates MSP (0) or PSP (1) 
//     __asm("   BNE   RtnViaPSP");

//      /* Trapping source is MSP - save r4-r11 on stack (default, so just push)  */
//     __asm("   PUSH  {r4-r11}");
//     __asm("   MRS r0,msp");
//     __asm("   BL  SVCHandler"); /* r0 is MSP */
//     __asm("   POP {r4-r11}");
//     __asm("   POP   {PC}");

//     /* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
//     __asm("RtnViaPSP:");
//     __asm("   mrs   r0,psp");
//     __asm("   stmdb   r0!,{r4-r11}"); /* Store multiple, decrement before */
//     __asm("   msr psp,r0");
//     __asm("   BL  SVCHandler"); /* r0 Is PSP */

//     /* Restore r4..r11 from trapping process stack  */
//     __asm("   mrs   r0,psp");
//     __asm("   ldmia   r0!,{r4-r11}"); /* Load multiple, increment after */
//     __asm("   msr psp,r0");

//     __asm("   POP   {PC}");

// }

// Edited Version (In case that is ever relevant?)
void SVCall()
{

    static bool firstSVCcall = true;

    /* Supervisor call (trap) entry point
       Using MSP - trapping process either MSP or PSP (specified in LR)
       Source is specified in LR: F9 (MSP) or FD (PSP)
       Save r4-r11 on trapping process stack (MSP or PSP)
       Restore r4-r11 from trapping process stack to CPU
       SVCHandler is called with r0 equal to MSP or PSP to access any arguments
    */

    /* Save LR for return via MSP or PSP */
    __asm("   PUSH  {LR}");

    // Not letting SVCall decide where it came from, I know where it came from
    /* Trapping source: MSP or PSP? */
    // __asm("   TST   LR,#4");   // Bit #4 indicates MSP (0) or PSP (1) 
    // __asm("   BNE   RtnViaPSP");

    if (firstSVCcall) {
        firstSVCcall = false;
        /* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
        __asm("   PUSH  {r4-r11}");
        __asm("   MRS r0,msp");
        __asm("   BL  SVCHandler"); /* r0 is MSP */
        __asm("   POP {r4-r11}");
        __asm("   POP   {PC}");
    }
    else {
        /* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
        __asm("RtnViaPSP:");
        __asm("   mrs   r0,psp");
        __asm("   stmdb   r0!,{r4-r11}"); /* Store multiple, decrement before */
        __asm("   msr psp,r0");
        __asm("   BL  SVCHandler"); /* r0 Is PSP */

        /* Restore r4..r11 from trapping process stack  */
        __asm("   mrs   r0,psp");
        __asm("   ldmia   r0!,{r4-r11}"); /* Load multiple, increment after */
        __asm("   msr psp,r0");

        // Force a PSP return??
        // __asm("   POP {r7}"); /* R7 is already clobbered */
        // __asm(" movw  LR,#0xFFFD"); //  Lower 16 [and clear top 16] 
        // __asm(" movt  LR,#0xFFFF");  /* Upper 16 only */
        // __asm(" bx  LR");            /* Force return to PSP */

        __asm("   POP   {PC}");
    }

}

extern "C" void SVCHandler(struct stack_frame *argptr)
{
    /*
      Supervisor call handler
      Handle startup of initial process
      Handle all other SVCs such as getid, terminate, etc.
      Assumes first call is from startup code
      Argptr points to (i.e., has the value of) either:
      - the top of the MSP stack (startup initial process)
      - the top of the PSP stack (all subsequent calls)
     Argptr points to the full stack consisting of both hardware and software 
     register pushes (i.e., R0..xPSR and R4..R10); this is defined in type 
     stack_frame
     Argptr is actually R0 -- setup in SVCall(), above.
     Since this has been called as a trap (Cortex exception), the code is in 
     Handler mode and uses the MSP
    */
    static int firstSVCcall = TRUE;

    kcallargs_t *kcaptr;

    if (firstSVCcall)
    {
        std::cout << "[KernelFunctions] SVCHandler: First Call\n";

        // Set reference to OSInstance for subsequent calls, stored as a global within Kernel Space
        OSInstance = OperatingSystem::GetOperatingSystem();
        PostOfficeInstance = PostOffice::GetPostOffice();

        /*
        * Force a return using PSP 
        * This will be the first process to run, so the eight "soft pulled" registers 
        (R4..R11) must be ignored otherwise PSP will be pointing to the wrong 
        location; the PSP should be pointing to the registers R0..xPSR, which will 
        be "hard pulled"by the BX LR instruction.
        * To do this, it is necessary to ensure that the PSP points to (i.e., has) the 
        address of R0; at this moment, it points to R4.
        * Since there are eight registers (R4..R11) to skip, the value of the sp 
        should be increased by 8 * sizeof(unsigned int).
        * sp is increased because the stack runs from low to high memory.
        */

        set_PSP(OSInstance->GetCurrentPCB()->stack_ptr + 8*sizeof(uint32_t));

        firstSVCcall = FALSE;

        // Start SysTick
        InterruptEnable(INT_VEC_UART0);  // Allow UART0 interrupts
        InterruptMasterEnable();         // Enable global interrupts
        SingletonSetup();                // Instantiates all singletons

        /*
        - Change the current LR to indicate return to Thread mode using the PSP
        - Assembler required to change LR to FFFF.FFFD (Thread/PSP)
        - BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
        */
        __asm(" movw  LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
        __asm(" movt  LR,#0xFFFF");  /* Upper 16 only */
        __asm(" bx  LR");            /* Force return to PSP */
    }
    else /* Subsequent SVCs */
    {
        /* 
        * kcaptr points to the arguments associated with this kernel call
        * argptr is the value of the PSP (passed in R0 and pointing to the TOS)
        * the TOS is the complete stack_frame (R4-R10, R0-xPSR)
        * in this example, R7 contains the address of the structure supplied by 
        the process - the structure is assumed to hold the arguments to the 
        kernel function.
        * to get the address and store it in kcaptr, it is simply a matter of 
        assigning the value of R7 (arptr -> r7) to kcaptr
        */

        kcaptr = (struct kcallargs *) argptr -> r7;
        switch(kcaptr -> kcode)
        {
            case GETID:
                kcaptr->rtnvalue = OSInstance->GetCurrentPCB()->pid;
                break;
            case NICE:
                KNice(kcaptr->priority);
                break;
            case TERMINATE:
                KTerminateProcess();
                break;
            case SEND:
                kcaptr->rtnvalue = (uint32_t) KSend(kcaptr);
                break;
            case RECV:
                kcaptr->rtnvalue = (uint32_t) KRecv(kcaptr);
                break;
            case BIND:
                kcaptr->rtnvalue = (uint32_t) KBind(kcaptr);
                break;
            default:
                // TODO: Some sort of real error handling here
                kcaptr -> rtnvalue = 0;
        }

    }

}

uint32_t get_PSP(void)
{
    /* Returns contents of PSP (current process stack */
    __asm(" mrs     r0, psp");
    __asm(" bx  lr");
    return 0;   /***** Not executed -- shuts compiler up */ 
            /***** If used, will clobber 'r0' */
}

uint32_t get_MSP(void)
{
    /* Returns contents of MSP (main stack) */
    __asm(" mrs     r0, msp");
    __asm(" bx  lr");
    return 0;
}

void set_PSP(volatile uint32_t ProcessStack)       
{
    /* set PSP to ProcessStack */
    __asm(" msr psp, r0");
}

void set_MSP(volatile uint32_t MainStack)
{
    /* Set MSP to MainStack */
    __asm(" msr msp, r0");
}

void volatile save_registers()
{
    /* Save r4..r11 on process stack */
    __asm(" mrs     r0,psp");
    /* Store multiple, decrement before; '!' - update R0 after each store */
    __asm(" stmdb   r0!,{r4-r11}"); 
    __asm(" msr psp,r0");
}

void volatile restore_registers()
{
    /* Restore r4..r11 from stack to CPU */
    __asm(" mrs r0,psp");
    /* Load multiple, increment after; '!' - update R0 */
    __asm(" ldmia   r0!,{r4-r11}"); 
    __asm(" msr psp,r0");
}

uint32_t get_SP()
{
    /**** Leading space required -- for label ****/         
    __asm("     mov     r0,SP"); 
    __asm(" bx  lr");
    return 0;
}

void assignR7(volatile uint32_t data)
{
    /* Assign 'data' to R7; since the first argument is R0, this is
    * simply a MOV from R0 to R7
    */
    __asm(" mov r7,r0");
}


void KNice(priority_t new_priority) {
    pcb_t* CurrentPCB = OSInstance->GetCurrentPCB();

    // Deattach PCB
    OSInstance->DeleteCurrentPCB();

    // Update priority
    assert(new_priority <= P_FIVE); // For debugging
    CurrentPCB->priority = new_priority;

    // Store current PSP
    CurrentPCB->stack_ptr = get_PSP();

    // Move PCB
    OSInstance->QueuePCB(CurrentPCB);

    // Set PCB to next process
    set_PSP(OSInstance->GetNextPCB()->stack_ptr);
}

void KTerminateProcess() {
    pcb_t* CurrentPCB = OSInstance->GetCurrentPCB();

    // Deattach PCB
    OSInstance->DeleteCurrentPCB();

    // Deallocate Process Stack
    // Deallocate PCB
    delete[] CurrentPCB->stack_start;
    delete CurrentPCB;

    // Set PCB to next process
    set_PSP(OSInstance->GetNextPCB()->stack_ptr);
}

bool KSend(kcallargs_t *kcaptr) {
    // Get requested mailbox
    mailbox_t* requested_mailbox = PostOfficeInstance->GetMailBox(kcaptr->dst_q);

    // Check if it is in use
    if (requested_mailbox->currently_owned == false) return false;

    // Check if the current PCB owns owns the "SRC_Q" mailbox (For replies) and is in use
    // TODO: Let this be optional! So processes can send anonymous messages
    mailbox_t* owners_mailbox = PostOfficeInstance->GetMailBox(kcaptr->src_q);
    if (owners_mailbox->currently_owned == false) return false;
    if (OSInstance->GetCurrentPCB() != owners_mailbox->owner_pcb) return false;

    // Add message to mailbox!
    uint16_t actual_msg_len = MIN(kcaptr->msg_len, (uint16_t)(requested_mailbox->letter_size));

    // Define a structure of the largest size and use it
    big_letter_msg_t input_msg;
    input_msg.msg_size = actual_msg_len;
    input_msg.msg_src = kcaptr->src_q;

    // If the receiver is currently blocked, then wake it up and directly pass messages
    if (requested_mailbox->currently_blocked) {
        // Wake up PCB by adding to correct queue
        OSInstance->QueuePCB(requested_mailbox->owner_pcb);

        // Update mailbox flag
        requested_mailbox->currently_blocked = false;

        // Manually copy message to destination!
        requested_mailbox->kcaptr->msg_len = actual_msg_len;
        requested_mailbox->kcaptr->src_q = kcaptr->src_q;

        switch(requested_mailbox->letter_size) {
            case ZERO_CHAR: // Not forgotten, just empty. This will be optimized away
                break;
            case ONE_CHAR:
                ((char *)(requested_mailbox->kcaptr->msg_ptr))[0] = ((char *)(kcaptr->msg_ptr))[0];
                break;
            case BIG_LETTER:
                memcpy(requested_mailbox->kcaptr->msg_ptr, kcaptr->msg_ptr, actual_msg_len);
                break;
            default:
                // TODO: Some sort of error msg here
                break;
        }
    }
    // Else, pass message into the correct buffer
    else {
        switch (requested_mailbox->letter_size) {
            case ZERO_CHAR:
                if (((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Full()) return false;
                ((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Add(&input_msg);
                break;
            case ONE_CHAR:
                if (((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Full()) return false;
                (input_msg.msg)[0] = ((char*)(kcaptr->msg_ptr))[0];
                ((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Add(&input_msg);
                break;
            case BIG_LETTER:
                if (((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Full()) return false;
                // TODO: Fix the fact that this is copying twice by adding a new "Add" function that directly copies it into the
                memcpy(input_msg.msg, kcaptr->msg_ptr, actual_msg_len);
                // std::memcpy(kcaptr->msg_ptr, tmp, actual_msg_len);
                ((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Add(&input_msg);
                break;
            default:
                // TODO: Some sort of error msg here
                break;
        }
    }
    return true;
}

bool KRecv(kcallargs_t *kcaptr) {
    // Get requested mailbox
    mailbox_t* requested_mailbox = PostOfficeInstance->GetMailBox(kcaptr->dst_q);

    // Check if it is in use
    if (requested_mailbox->currently_owned == false) return false;

    // Check if this is your mailbox!
    if (OSInstance->GetCurrentPCB() != requested_mailbox->owner_pcb) return false;

    // Fetch mail! (Using different structures)
    static empty_msg_t recv_msg0;
    static one_char_msg_t recv_msg1;
    static big_letter_msg_t recv_msg256;

    bool put_to_sleep = false;
    switch (requested_mailbox->letter_size) {
    case ZERO_CHAR:
        // Get for the sake of getting, but ignore result
        if (((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Empty()) put_to_sleep = true;
        else {
            recv_msg0 = ((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Get();
            kcaptr->msg_len = recv_msg0.msg_size;
            kcaptr->src_q = recv_msg0.msg_src;
        }
        break;
    case ONE_CHAR:
        if (((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Empty()) put_to_sleep = true;
        else {
            recv_msg1 = ((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Get();
            kcaptr->msg_len = recv_msg1.msg_size;
            kcaptr->src_q = recv_msg1.msg_src;
            ((char *)kcaptr->msg_ptr)[0] = recv_msg1.msg[0];
        }
        break;
    case BIG_LETTER:
        if (((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Empty()) put_to_sleep = true;
        else {
            recv_msg256 = ((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Get();
            kcaptr->msg_len = recv_msg256.msg_size;
            kcaptr->src_q = recv_msg256.msg_src;
            memcpy(kcaptr->msg_ptr, recv_msg256.msg, recv_msg256.msg_size);
        }
        break;
    default:
        // TODO: Some sort of error msg here
        break;
    }

    if (put_to_sleep) {
        // Update mailbox state
        requested_mailbox->currently_blocked = true;
        requested_mailbox->kcaptr = kcaptr;

        // Deattach PCB
        OSInstance->DeleteCurrentPCB();

        // Store current PSP
        requested_mailbox->owner_pcb->stack_ptr = get_PSP();

        // Set PCB to next process
        set_PSP(OSInstance->GetNextPCB()->stack_ptr);
    }

    return true;
}

bool KBind(kcallargs_t *kcaptr) {
    // Get requested mailbox
    mailbox_t* requested_mailbox = PostOfficeInstance->GetMailBox(kcaptr->dst_q);

    // Check if it is in use
    if (requested_mailbox->currently_owned == false) return false;

    // Buy a mailbox!
    return PostOfficeInstance->BuyMailbox(kcaptr->req_q, kcaptr->q_size, OSInstance->GetCurrentPCB());
}

// The following are performed within PSP Space and are not actually kernel calls,
// they pass into the kernel by directly making an SVC call

uint32_t PGetID() {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = GETID;
    assignR7((uint32_t) &KernelArgs);
    SVC();
    return KernelArgs.rtnvalue;
}

void PNice(priority_t priority) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = NICE;
    KernelArgs.priority = priority;
    assignR7((uint32_t) &KernelArgs);
    SVC();
}

void PTerminateProcess() {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = TERMINATE;
    assignR7((uint32_t) &KernelArgs);
    SVC();
}

bool PSend(uint8_t src_q, uint8_t dst_q, void* msg_ptr, uint32_t msg_len) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = SEND;
    KernelArgs.src_q = src_q;
    KernelArgs.dst_q = dst_q;
    KernelArgs.msg_ptr = msg_ptr;
    KernelArgs.msg_len = msg_len;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    return KernelArgs.rtnvalue;
}

bool PRecv(uint8_t& src_q, uint8_t dst_q, void* msg_ptr, uint32_t& msg_len) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = RECV;
    KernelArgs.dst_q = dst_q;
    KernelArgs.msg_ptr = msg_ptr;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    if (KernelArgs.rtnvalue != 0) {
        src_q = KernelArgs.src_q;
        msg_len = KernelArgs.msg_len;
    }

    return KernelArgs.rtnvalue;
}

bool PBind(uint8_t req_q, letter_size_t size) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = BIND;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    return KernelArgs.rtnvalue;
}

