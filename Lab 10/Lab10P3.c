/*------------------------------------------------------------------------------
 * File:        Lab10Part3.c
 * Function:    Generates Waveforms on Oscilloscope
 * Description: Generates Sine wave and triangle wave at 30-60hz based on switch presses.
 * Input:       Switches
 * Output:      Oscilloscope waveforms
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        April 1, 2024
 *----------------------------------------------------------------------------*/

#include <msp430.h>
#include "sine_lut_256.h"


#define S1 P2IN&BIT1 // Switch 1
#define S2 P1IN&BIT1 // Switch 2


unsigned char index = 0;

unsigned int value= 127;

unsigned int i = 0;

volatile int up = 1;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // SWITCH 1 SETUP
    P2DIR &= ~BIT1; // Set P2.1 as input for S1 input
    P2REN |= BIT1; // Enable the pull-up resistor at P2.1
    P2OUT |= BIT1; // Required for proper IO

    // SWITCH 2 SETUP
    P1DIR &= ~BIT1; // Set P1.1 as input for S2 input
    P1REN |= BIT1; // Enable the pull-up resistor at P1.1
    P1OUT |= BIT1; // Required for proper IO


    // Timer B Configuration
    TB0CCTL0 = CCIE; // Timer Count Triggers Interrupt
    TB0CTL = TBSSEL_2 + MC_1; // SMCLK is source, and set to up mode
    TB0CCR0 = 135; // 30hz

    P3DIR = 0xFF; // Enable All pins on Port 3 as outputs.
    P3OUT = 0;

    __bis_SR_register(LPM0_bits + GIE);


    return 0;
}



#pragma vector=TIMERB0_VECTOR
__interrupt void timerISR2(void)
{
    // Check if S2 is pressed
    if ((S2) == 0)
    {
        TB0CCR0 = 67; // 60hz

    }
    else
    {
        TB0CCR0 = 135; // 30hz
    }

    if ((S1) == 0)
    {
        TB0CCR0 = 205; // for triangle
        // When S1 is held
        if (up)
        {
            value += 3;

            if (value == 255) // once value reaches max
            {
                up = 0; // go down
            }
        }
        else
        {
            value -= 3;

            if (value == 0) // once value reaches min
            {
                up = 1; // go up again
            }

        }

        P3OUT = value; // output value to p3
    }
    else
    {
        // If S1 is not pressed, continue using the sine lookup table
        index += 1;
        P3OUT = lut256[index % 256]; // Ensure index wraps around
    }
}
