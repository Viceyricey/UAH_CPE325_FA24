; ------------------------------------------------------------------------------
; File:         SMUL.asm
; Function:     Finds the sum of squares of an integer array
; Description:  This program is a subroutine that uses shift-add algorithm to sum the squares of an array
; Output:       The output is handled by the caller function
; Author(s):    Michael Agnew, ma0133@uah.edu
; Date:         February 12, 2024
; Revised:      February 13, 2024
; ------------------------------------------------------------------------------

		.cdecls C,LIST,"msp430.h" ; Include device header file12

		.def	SMUL

		.text

SMUL:	push	R7				; Make register 7 temporal sum of multiplication
		push	R6				; Make register 6 array length
		push	R4				; Make register 4 pointer to array
		clr.w	R12				; Clear register 8 so you can do math operations on it
		mov.w	10(SP), R6		; Retrieve array length
		mov.w	12(SP), R4		; Retrieve the first element of the array

lnext:	mov.w 	@R4+, R9		; Extract the first element of the array
		mov.w	R9, R8			; Copy the element of the array into R8


check:	and.w 	#0x7FFF, R9 	; clear the carry bit
		cmp.w	#0x00,R9		; Check if 0
		jeq		end				; if 0, break because we're done
		mov.w	#0x01, R10 		; Initialize Register 10 to 1
		and.w	R9, R10			; AND R9 and R10 Together to see if the LSB is 1
		cmp.w	#0x01, R10		; Comparison
		jeq		sum				; Add this number to the current sum if LSB = 1

rot:	cmp.w 	#0x01, R9		; If R9 is 1, then branch to 0 block
		jeq 	zero			; if equal to 1, jump to zero
		rlc.w	R8				; Rotate R8
		dec.w 	R8				; Decrement R8 (why do I have to do this? Idk)
		rrc.w	R9				; Rotate R9
		jmp		check	     	; jump to check

sum:	add.w	R8, R12			; sum R8
		jmp		rot				; jump to rot

zero: 	mov.w 	#0x00, R9 		; set register to 0 so I don't have to worry about garbage
		rlc.w	R8				; Rotate R8
		jmp		check			; jump back to check


end:	dec.w	R6				; decrement array length
		jnz 	lnext			; if not zero, continue

lend:
		mov.w	R12, 8(SP)		; assign R8 to the stack pointer
		pop		R4				; Restore R4
		pop		R6				; Restore R6
		pop		R7				; Restore R7
		ret

		.end
