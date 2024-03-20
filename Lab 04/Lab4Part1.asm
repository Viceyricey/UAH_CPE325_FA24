; ------------------------------------------------------------------------------
; File:         Lab4P1Final.asm
; Function:     Counts the number of Digits and Capital Letters in a String and Displays the number to P1OUT and P2OUT Respectively
; Description:  Program traverses an input array of characters
;               to detect capital letters or digits; exits when a NULL is detected
; Input:        The input string specified in myStr
; Output:       The port P1OUT and P2OUT display the number of digits and capital letters in the string
; Author(s):    Michael Agnew, ma0133@uah.edu
; Date:        	February 2nd, 2024
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
;-------------------------------------------------------------------------------
        .def    RESET                   ; Export program entry-point to
                                        ; make it known to linker.
;-------------------------------------------------------------------------------
myStr:  .string "Welcome To MSP430F5529 Assembly Programming!", ''
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
        clr.b   R7						; Register R7 will serve as a counter
gnext:  mov.b   @R4+, R6                ; Get a new character
        cmp     #0, R6                  ; Is it a null character
        jeq     lend                    ; If yes, go to the end


        cmp.b   #'0', R6                ; Is the character 0?
        jl		gnext					; if it's ascii value is lower, move on
        cmp.b 	#0x3A, R6				; compare it to the ascii value above 9
        jge		capital					; if it's greater than ascii value of 9, then it must be a letter
        inc.w	R5						; if it is in this range, increment the counter
        jmp 	gnext					; at the end, move to the next character


capital:cmp.b	#'A', R6				; compare character to ascii value A
		jl		gnext					; if it's less, then jump to next character
		cmp.b	#0x5B, R6				; compare to ascii value above Z
		jge 	gnext					; if it's greater, jump to next character
		inc.w	R7						; if it is in the range, then increment capital letter counter
		jmp 	gnext					; afterwards, move to the next character

lend:   mov.b   R5,&P1OUT               ; Write result in P1OUT (not visible on port pins)
		mov.b   R7,&P2OUT				; Write result in P2OUT (not visible on port pins)
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
