/*------------------------------------------------------------------------------
 * File:        Lab7Part1Code.c
 * Function:    Increases and Decreases the duty cycle of an LED based on user input.
 * Description: This program initializes LED1 at 50% brightness and allows the user to press SW1 and SW2 to change the brightness.
 * Input:       Pressing S1 and S2
 * Output:      LED1 (At different variants of brightness)
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        2/26/2024
 * ---------------------------------------------------------------------------*/

#include <msp430.h>

#define SW1 (P2IN & BIT1) // defining switch 1
#define SW2 (P1IN & BIT1) // defining switch 2

void main(void)
{ // start of main
    WDTCTL = WDTPW + WDTHOLD; // Stop dat boy the watchdog
    _EINT();                        // Enable interrupts

    // Configuring Ports and Bits
    P1DIR |= BIT2; // setting up p1.2 as direction
    P1SEL |= BIT2; // stuff dont worry about it


   // Configuring switch 1
    P2DIR &= ~BIT1;
    P2REN |= BIT1;
    P2OUT |= BIT1;

    // switch 1 ISR
    P2IE |= BIT1;                   // P1IE.BIT1 interrupt enabled
    P2IES |= BIT1;                  // P1IES.BIT1 hi/low edge
    P2IFG &= ~BIT1;                 // P1IFG.BIT1 is cleared

   // Configuring switch 2
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1OUT |= BIT1;

    // switch 2 ISR
    P1IE |= BIT1;                   // P1IE.BIT1 interrupt enabled
    P1IES |= BIT1;                  // P1IES.BIT1 hi/low edge
    P1IFG &= ~BIT1;                 // P1IFG.BIT1 is cleared


    TA0CTL = TASSEL_2 + MC_1; // Setting SMCLK to up mode
    TA0CCR0 = 1000; // Initializing the PWM period
    TA0CCTL1 = OUTMOD_7; // Setting the output to reset/set
    TA0CCR1 = 500; // initializing the duty cycle to 50%

    _BIS_SR(LPM0_bits + GIE);       // Enter LPM0(CPU is off); Enable interrupts


} // end of main


#pragma vector = PORT1_VECTOR // switch 2 interrupt vector
__interrupt void Port1_ISR (void)
{
    unsigned int i = 0;
    if ((SW2 == 0) && (TA0CCR1 > 0))
    {
        for (i = 0; i < 20000; i++); // debounce
        P1IFG &= ~BIT1; // P1IFG.BIT0 is cleared
        TA0CCR1 -= 250; // decrease the duty cycle by 25%
        for (i = 0; i < 20000; i++); // debounce

    }
    else if ((SW1 == 0) && (TA0CCR1 < 1000))
    {
        for (i = 0; i < 20000; i++); // debounce
        P1IFG &= ~BIT1; // P1IFG.BIT0 is cleared
        TA0CCR1 += 250; // increase the duty cycle by 25%
        for (i = 0; i < 20000; i++); // debounce

    }
    return;}

#pragma vector = PORT2_VECTOR // switch 1 interrupt vector
__interrupt void Port2_ISR (void)
{
    unsigned int i = 0; // loop counter
    if ((SW1 == 0) && (TA0CCR1 < 1000))
    {
        for (i = 0; i < 20000; i++); // debounce
        P1IFG &= ~BIT1; // P1IFG.BIT0 is cleared
        TA0CCR1 += 250; // increase the duty cycle by 25%
        for (i = 0; i < 20000; i++); // debounce

    }
    else if ((SW2 == 0) && (TA0CCR1 > 0))
    {
        for (i = 0; i < 20000; i++); // debounce
        P1IFG &= ~BIT1; // P1IFG.BIT0 is cleared
        TA0CCR1 -= 250; // decrease the duty cycle by 25%
        for (i = 0; i < 20000; i++); // debounce

    }
    return;}








