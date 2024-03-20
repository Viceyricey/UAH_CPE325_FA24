;-------------------------------------------------------------------------------
; File:         Lab6final.asm
; Function:     Handles switches and LEDs with interrupts
; Description:  When pressing switch 1, toggle Green LED, when pressing switch 2, flash red LED 4 times then toggle green LED.
; Input:        Switches 1 and 2
; Output:       LED1 and LED2
; Author(s):    Michael Agnew, ma0133@uah.edu
; Date:         February 19, 2024
; Revised:      Febryary 21, 2024
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
            .def	SW2_INT
            .def	SW1_INT
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET:      mov.w   #__STACK_END,SP         ; Initialize stack pointer
StopWDT:    mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer
;--------------------------------------------------------------------------
Setup:
	        bis.b   #001h, &P1DIR   ; Setting up LED1
            bic.b   #001h, &P1OUT	; Setting up Output to LED1
            bic.b	#002h, &P1DIR	; Setting up SW2 Direction
            bis.b	#002h, &P1REN	; Setting up SW2 Ren
            bis.b	#002h, &P1OUT 	; Setting up SW2 Output

            bis.b	#0x80, &P4DIR	; Setting up LED2
            bic.b	#0x80, &P4OUT	; Setting up Output to LED2
            bic.b	#002h, &P2DIR	; Setting up SW1 Direction
            bis.b	#002h, &P2REN	; Setting up SW1 Ren
            bis.b	#002h, &P2OUT 	; Setting up SW1 Output

            bis.w	#GIE, SR		; Enabling Global Interrupts
            bis.b	#002h, &P1IE	; Enabling Switch 2 Interrupts
            bis.b	#002h, &P1IES
            bic.b	#002h, &P1IFG

            bis.b	#002h, &P2IE	; Enabling Switch 1 Interrupts
            bis.b	#002h, &P2IES
            bic.b	#002h, &P2IFG

InfLoop:
            jmp     $                       ; Loop here until interrupt

;----------------------------------------------------------------------------
; Interrupt service routine. Caused by one of two switches being pressed
;----------------------------------------------------------------------------
SW2_INT:
			mov.w	#4, R10
			bic.b	#002h, &P1IFG 	; clear switch 2 interrupt flag
			bit.b	#02h, &P1IN 	; check if switch 2 is pressed
			jnz 	SW2Exit			; if switch 2 is not pressed, then exit

		    mov.w   #2000, R15              ; Set to (2000 * 10 cc )
SW2Deb:     dec.w   R15                     ; Decrement R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jnz     SW2Deb                    ; Is the delay over?
            bit.b   #00000010b, &P1IN         ; Make sure that SW2 is still pressed
            jnz     SW2Exit                   ; If SW2 is not pressed, exit interrupt

sw2pres:	bis.b 	#001h, &P1OUT 		; Turn on the red LED
            call	#Delay 				; 250ms Delay
            bic.b	#001h, &P1OUT       ; Turn off the red LED
            call	#Delay   			; 250ms Delay
            dec.w   R10					; Decrement the loop counter
            jnz     sw2pres				; if the loop counter is not 0, then loop again

LED2on: 	xor.b	#0x80, &P4OUT       ; Turn on the green LED

SW2Exit:	reti 						; Return from the interrupt

SW1_INT:

            bic.b   #002h, &P2IFG           ; Clear switch 1 interrupt flag
ChkSW1:     bit.b   #02h, &P2IN             ; Check if SW1 is pressed
                                            ; (0000_0010 on P1IN)
            jnz     SW1Exit                   ; If not zero, SW is not pressed
                                            ; loop and check again
		    mov.w   #2000, R15              ; Set to (2000 * 10 cc )
SW1Deb:     dec.w   R15                     ; Decrement R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jnz     SW1Deb                 ; Delay over?
            bit.b   #00000010b,&P2IN        ; Verify SW2 is still pressed
            jnz     SW1Exit                   ; If not, wait for SW2 press

	        mov.b	&P4OUT, R6             ; check if LED1 on
           	and.b 	#0x80, R6
            jnz		LEDoff
            jmp 	LEDon
LEDon:      bis.b   #0x80, &P4OUT            ; Turn on LED1
			jmp 	SW1Exit


Delay:
            mov.w   #50000, R12         ; 50k iteration loop makes 500ms which is a half second. 2hz = 0.5s
            nop
            nop
            nop
            nop
            nop
            nop
            nop

loopy_boy:  dec.w   R12					; decrement counter register
            jnz     loopy_boy			; if counter is not 0, jump back to the top of the loop
			ret							; return from subroutine

LEDoff:     bic.b  #0x80, &P4OUT            ; Turn off LED1
SW1Exit:    reti                            ; Return from interrupt
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
            .sect   ".int42"
            .short	SW1_INT
            .sect	".int47"
            .short	SW2_INT
            .end
