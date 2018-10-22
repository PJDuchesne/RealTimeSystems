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

void SVCall()
{
    /* Supervisor call (trap) entry point
       Using MSP - trapping process either MSP or PSP (specified in LR)
       Source is specified in LR: F9 (MSP) or FD (PSP)
       Save r4-r11 on trapping process stack (MSP or PSP)
       Restore r4-r11 from trapping process stack to CPU
       SVCHandler is called with r0 equal to MSP or PSP to access any arguments
    */

    /* Save LR for return via MSP or PSP */
    __asm("   PUSH  {LR}");

    /* Trapping source: MSP or PSP? */
    __asm("   TST   LR,#4");  /* Bit #4 indicates MSP (0) or PSP (1) */
    __asm("   BNE   RtnViaPSP");

    /* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
    __asm("   PUSH  {r4-r11}");
    __asm("   MRS r0,msp");
    __asm("   BL  SVCHandler"); /* r0 is MSP */
    __asm("   POP {r4-r11}");
    __asm("   POP   {PC}");

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
    __asm("   POP   {PC}");

}

void SVCHandler(struct stack_frame *argptr)
{
    // To allow "BL SVCHandler" to call from above
    __asm("SVCHandler:");
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
    struct kcallargs *kcaptr;

    if (firstSVCcall)
    {
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

        // TODO: Make current process, not this running->sp idea
        set_PSP(OperatingSystem::GetOperatingSystem()->GetNextPCB()->stack_ptr + 8 * sizeof(uint32_t));

        firstSVCcall = FALSE;

        /* Start SysTick -> DONE IN MONITOR */
        // systick_init();

        /*
        - Change the current LR to indicate return to Thread mode using the PSP
        - Assembler required to change LR to FFFF.FFFD (Thread/PSP)
        - BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
        */
        __asm(" movw  LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
        __asm(" movt  LR,#0xFFFF");  /* Upper 16 only */
        __asm(" bx  LR");          /* Force return to PSP */
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

        #ifdef FOR_KERNEL_ARGS // TODO: Define this

        kcaptr = (struct kcallargs *) argptr -> r7;
        switch(kcaptr -> code)
        {
            case GETID:
                // TODO: GETID
                break;
            case NICE:
                // TODO: NICE
                break;
            case TERMINATE:
                // TODO: TERMINATE
                break;
            default:
                kcaptr -> rtnvalue = -1;
        }

        #endif

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