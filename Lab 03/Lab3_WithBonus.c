/*------------------------------------------------------------------------------
 * File:        Lab3Code.c
 * Function:    Blinks LED lights on the MSP430 in the ways assigned.
 * Description: This code performs LED blinking and alternation based off the given requirements of the assignment.
 * Input:       None
 * Output:      LEDs on the circuit board.
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        Jan 29, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h>

#define S2 P1IN&BIT1
#define S1 P2IN&BIT1
#define GREENLED 0x80             // Mask for BIT7 = 1000_0000b
#define REDLED 0x01               // Mask for BIT0 = 0000_0001b

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop watchdog timer
    P4DIR |= GREENLED;          // Set P4.7 to output direction
    P4OUT |= GREENLED;          // Set P4OUT to 1000_0000b (LED1 is ON)

    P1DIR |= REDLED;  // assign red led
    P1OUT &= ~REDLED; // make sure that it's off

    P2DIR &= ~BIT1;    // Set P2.1 as input for S1 input
    P2REN |= BIT1;     // Enable the pull-up resistor at P2.1
    P2OUT |= BIT1;     // Required for proper IO


    P1DIR &= ~BIT1;    // Set P1.1 as input for S2 input
    P1REN |= BIT1;     // Enable the pull-up resistor at P1.1
    P1OUT |= BIT1;     // Required for proper IO


    unsigned int i = 0;

    while(1) // infinite loop
    {
        if (((S1) == 0) && ((S2) == 0)) // if switch 1 and 2 are pressed
        {

            for (i = 2000; i > 0; i--) // debounce

            if (((S1) == 0) && ((S2) == 0))
            {
                __delay_cycles(200000); // 5 hz delay

                P1OUT ^= REDLED;                // Toggle LED1
                P4OUT ^= GREENLED;              // Toggle LED2

            }

        }
        else if (((S1) == 0) && !(((S1) == 0) && ((S2) == 0))) // if the switch is pressed enter
        {
            for (i = 2000; i > 0; i--) // debounce ~20 ms

            P4OUT &= ~GREENLED; // turn LED2 off
            __delay_cycles(500000); // delay of 2 hz
            P1OUT ^= REDLED; // blink red LED

        }
        else if (((S2) == 0) && !(((S1) == 0) && ((S2) == 0))) // if switch 2 is pressed
        {

            for (i = 2000; i > 0; i--) // debounce

            P1OUT |= REDLED; // turn on red LED
            __delay_cycles(250000); // delay for 4 hz
            P4OUT ^= GREENLED; // blink green LED

        }

        else // if no buttons pressed (default state)
        {
            P4OUT |= GREENLED; // turn LED2 on
            P1OUT &= ~REDLED; // make sure that LED1 is off
        }
    }


}


