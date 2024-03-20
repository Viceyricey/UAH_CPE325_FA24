; ------------------------------------------------------------------------------
; File:         Lab5MainFinal.asm
; Function:     Finds a sum of squares of two integer arrays using two subroutines HMUL and SMUL
; Description:  This program calls SMUL and HMUL as subroutines in order to find the sum of squares of the given arrays
; Input:        The input arrays are signed 16-bit integers in arr1, arr2, arr3, and arr4
; Output:       P1OUT, P2OUT, P3OUT, P4OUT
; Author(s):    Michael Agnew, ma0133@uah.edu
; Date:         February 12, 2024
; Revised:      Febryary 13, 2024
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
;-------------------------------------------------------------------------------
        .def    RESET                   ; Export program entry-point to
                                        ; make it known to linker.
        .ref    HMUL

        .ref 	SMUL
;-------------------------------------------------------------------------------
        .text                           ; Assemble into program memory.
        .retain                         ; Override ELF conditional linking
                                        ; and retain current section.
        .retainrefs                     ; And retain any sections that have
                                        ; references to current section.
;-------------------------------------------------------------------------------
RESET:  mov.w   #__STACK_END,SP         ; Initialize stack pointer
        mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer
;-------------------------------------------------------------------------------
; Main code here
;-------------------------------------------------------------------------------
main:
		bis.b	#0xFF, &P1DIR ; port 1
		bis.b	#0xFF, &P2DIR ; port 2
		bis.b	#0xFF, &P3DIR ; port 3
		bis.b	#0xFF, &P4DIR ; port 4
		clr.w 	P1OUT
		clr.w   P3OUT
		clr.w 	P5OUT
		clr.w   P7OUT
        push    #arr1                   ; Push the address of arr1
        push    #8                      ; Push the number of elements
        sub.w   #2, SP                  ; Allocate space for the sum
        call    #SMUL
        mov.w   @SP, P1OUT             ; Store the sum in P2OUT&P1OUT
        add.w   #6, SP                  ; Collapse the stack

        push    #arr3                   ; Push the address of arr1
        push    #8                      ; Push the number of elements
        sub.w   #2, SP                  ; Allocate space for the sum
		call 	#HMUL
		mov.w 	@SP, P3OUT
		add.w	#6, SP

        push    #arr2                   ; Push the address of arr1
        push    #7                      ; Push the number of elements
        sub     #2, SP                  ; Allocate space for the sum
        call    #SMUL
        mov.w   @SP, P5OUT             ; Store the sume in P4OUT&P3OUT
        mov.w 	P5OUT, R12
        add.w   #6, SP                  ; Collapse the stack

        push    #arr4                   ; Push the address of arr1
        push    #7                      ; Push the number of elements
        sub.w   #2, SP                  ; Allocate space for the sum
		call 	#HMUL
		mov.w 	@SP, P7OUT
		add.w	#6, SP

        jmp     $

arr1:   .int    1, 7, -5, 4, 2, -3, 9, 6
arr2:   .int    2, 4, 1, 6, -1, -1, -1
arr3:   .int    1, 7, -5, 4, 2, -3, 9, 6
arr4:   .int    2, 4, 1, 6, -1, -1, -1

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
        .global __STACK_END
        .sect   .stack
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
        .sect   ".reset"                ; MSP430 RESET Vector
        .short  RESET
        .end
