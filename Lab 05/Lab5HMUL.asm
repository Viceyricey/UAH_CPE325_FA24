; ------------------------------------------------------------------------------
; File:         HMUL.asm
; Function:     Finds the sum of squares of an integer array
; Description:  This program is a subroutine that uses hardware multiplication to sum the squares of an array
; Output:       The output is handled by the caller function
; Author(s):    Michael Agnew, ma0133@uah.edu
; Date:         February 12, 2024
; Revised:      February 13, 2024
; ------------------------------------------------------------------------------
        .cdecls C, LIST, "msp430.h"     ; Include device header file
; ------------------------------------------------------------------------------
        .def    HMUL
        .text
HMUL:                                ; Save the registers on the stack
        push    R13                     ; Save R7, temporal sum
        push    R6                      ; Save R6, array length
        push    R4                      ; Save R5, pointer to array
        clr.w   R7                      ; Clear R7
        mov.w   10(SP), R6              ; Retrieve array length
        mov.w   12(SP), R4              ; Retrieve starting address
        clr.w	R13

lnext:  mov.w	@R4+, R7	; get the next element
		mov.w 	R7, &MPY	; load in the value in R7
		mov.w 	R7, &OP2	; multiply it by itself
		nop  ; delay
		nop	 ; delay
		nop  ; delay

		mov.w 	RESLO, R8 ; store the result in R8
		add.w 	R8, R13

        dec.w   R6                      ; Decrement array length
        jnz     lnext                   ; Repeat if not done
        mov.w   R13, 8(SP)              ; Store the sum on the stack

lend:   pop     R4                      ; Restore R4
        pop     R6                      ; Restore R6
        pop     R7                      ; Restore R7
        ret                             ; Return
        .end
