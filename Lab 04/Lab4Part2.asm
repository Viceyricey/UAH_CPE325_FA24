; ------------------------------------------------------------------------------
; File:         Lab4P2CodeFinal.asm
; Function:     Evalutes a string as a math operation
; Description:  Program traverses an input array of characters
;               to detect operands and operators, then evalutes the mathematical expression; exits when a NULL is detected
; Input:        The input string specified in myStr
; Output:       The port P2OUT displays the answer to the expression
; Author(s):    Michael Agnew, ma0133@uah.edu
; Date:         February 2nd, 2024
; Revised:      February 6th, 2024
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
;-------------------------------------------------------------------------------
        .def    RESET                   ; Export program entry-point to
                                        ; make it known to linker.
;-------------------------------------------------------------------------------
myStr:  .string "7-2+2", ''
        ; .string does not add NULL at the end of the string;
        ; '' ensures that a NULL follows the string.
        ; You can alternatively use .cstring "HELLO WORLD, I AM THE MSP430!"
        ; that adds a NULL character at the end of the string automatically.
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
; Main loop here
;-------------------------------------------------------------------------------
main:   ; bis.b   #0FFh, &P1DIR         ; Do not output the result on port pins
        mov.w   #myStr, R4              ; Load the starting address of the string into R4
        clr.b   R5                      ; Register R5 will serve as a counter
gnext:  mov.b   @R4+, R6                ; Get a new character
        cmp     #0, R6                  ; Is it a null character
        jeq     lend                    ; If yes, go to the end
 ; =======================================================================

   		cmp.b   #'0', R6                ; Check if 0
        jne     check1                  ; if not, move to check if 1
		mov.b   #0, R6					; if so, assign decimal value 0 to R6
        jmp 	op						; afterwards, jump to op branch
; ========================================= REPEAT STEPS FOR REST OF INTEGERS ===============
check1: cmp.b   #'1', R6
		jne     check2
		mov.b   #1, R6
		mov.b 	R6, R9
        jmp 	op

check2: cmp.b   #'2', R6
		jne     check3
		mov.b   #2, R6
		mov.b 	R6, R9
        jmp 	op

check3: cmp.b   #'3', R6
		jne     check4
		mov.b   #3, R6
		mov.b 	R6, R9
        jmp 	op

check4: cmp.b   #'4', R6
		jne     check5
		mov.b   #4, R6
		mov.b 	R6, R9
        jmp 	op

check5: cmp.b   #'5', R6
		jne     check6
		mov.b   #5, R6
		mov.b 	R6, R9
        jmp 	op

check6: cmp.b   #'6', R6
		jne     check7
		mov.b   #6, R6
		mov.b 	R6, R9
        jmp 	op

check7: cmp.b   #'7', R6
		jne     check8
		mov.b   #7, R6
		mov.b 	R6, R9
        jmp 	op

check8: cmp.b   #'8', R6
		jne     check9
		mov.b   #8, R6
		mov.b 	R6, R9
        jmp 	op

check9: cmp.b   #'9', R6
		jne     op
		mov.b   #9, R6
		mov.b 	R6, R9
		jmp 	op

 ; =======================================================================

op:		mov.b   @R4+, R7				; pull in next char from string
		cmp     #0, R7                  ; Is it a null character
        jeq     lend                    ; If yes, go to the end
        jmp 	src						; afterwards, jump to src branch

 ; =======================================================================

src:    mov.b   @R4+, R8
		cmp     #0, R8                  ; Is it a null character
        jeq     lend                    ; If yes, go to the end

    	cmp.b   #'0', R8                ; Is it 0 character
        jne     check11                 ; If not, go to the next check
		mov.b   #0, R8					; if it is, assign decimal value 0 to R8
		jmp 	subbr					; afterwards, jump to subbr branch

; ========================================= REPEAT STEPS FOR REST OF INTEGERS ===============

check11:cmp.b   #'1', R8
		jne     check22
		mov.b   #1, R8
		jmp 	subbr

check22:cmp.b   #'2', R8
		jne     check33
		mov.b   #2, R8
		jmp 	subbr

check33:cmp.b   #'3', R8
		jne     check44
		mov.b   #3, R8
		jmp 	subbr

check44:cmp.b   #'4', R8
		jne     check55
		mov.b   #4, R8
		jmp 	subbr

check55:cmp.b   #'5', R8
		jne     check66
		mov.b   #5, R8
		jmp 	subbr

check66:cmp.b   #'6', R8
		jne     check77
		mov.b   #6, R8
		jmp 	subbr

check77:cmp.b   #'7', R8
		jne     check88
		mov.b   #7, R8
		jmp 	subbr

check88:cmp.b   #'8', R8
		jne     check99
		mov.b   #8, R8
		jmp 	subbr

check99:cmp.b   #'9', R8
		mov.b   #9, R8
		jmp 	subbr
 ; =======================================================================

subbr:  cmp.b 	#'-', R7 ; check to see if minus sign
		jne 	addbr	 ; if not, jump to add branch
		sub.b	R8, R9   ; if so, subtract R8 from R9 and store in R9
		jmp		op		 ; aftewards, get the next operator

 ; =======================================================================

addbr:	cmp.b   #'+', R7 ; check to see if plus sign
		add.b	R8, R9	 ; if so, add the two numbers and store in R9
		jmp		op		 ; afterwards, get the next operator

 ; =======================================================================


lend:   mov.b   R9,&P2OUT               ; Write result in P2OUT (not visible on port pins)
        bis.w   #LPM4, SR               ; LPM4
        nop                             ; Required only for debugger
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
