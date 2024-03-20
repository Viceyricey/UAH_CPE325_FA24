/*------------------------------------------------------------------------------
 * File:        Lab6CFinal.c
 * Function:    Blinks the LEDs alternately. Clicking the input switches allows one to change the Clock Frequency of the processor
 * Description: This C program turns on the LEDs and blinks them alternately according to the current
 * calibration of the clock frequency. Debouncing delay of 20ms is added for switch presses.
 * Clocks:      ACLK = 32.768kHz, MCLK = SMCLK = default DCO (~1 MHz)
 *                      MSP-EXP430F5529LP
 *                      -----------------
 *                   /|\|               |
 *                    | |               |
 *                    --|RST            |
 *                      |           P1.0|-->LED1(RED)
 *                      |           P2.1|<--SW1
 *                      |               |
 * Input:       Pressing S1 and S2
 * Output:      LED1 and LED2 are blinked alternately.
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        2/22/2024
 * ---------------------------------------------------------------------------*/
#include <msp430.h>

#define S1 P2IN&BIT1                    // switch 1 config
#define S2 P1IN&BIT1                    // switch 2 config
#define REDLED 0x01                     // Mask for BIT0 = 0000_0001b
#define GREENLED 0x80                   // Mask for BIT7 = 1000_0000b

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // watchdog stop timer


    P1DIR |= REDLED; // Set LED1 as output
    P1OUT &= ~REDLED; // Initially turn off LED1

    P4DIR |= GREENLED; // Set LED2 as output
    P4OUT |= GREENLED; // Initially turn on LED2

    P2DIR &= ~BIT1;
    P2REN |= BIT1;
    P2OUT |= BIT1;

    P1DIR &= ~BIT1; // Set P1.1 as input for SW2 Input
    P1REN |= BIT1; // Enable the pull-up resistor at P1.1
    P1OUT |= BIT1; // Required for proper IO


    /********************I know exactly what all of this does********************/

    UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx

    // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
    do
    {
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                              // Clear XT2,XT1,DCO fault flags
      SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

    __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation

    UCSCTL2 = FLLD_1 + 66;                           // Set DCO Multiplier for 2MHz
                                              // (N + 1) * FLLRef = Fdco
                                              // (66 + 1) * 32768 = 2MHz
    __bic_SR_register(SCG0);                  // Enable the FLL control loop


    unsigned int i = 0;
    unsigned int counter = 0;
    while(1)                            // Infinite loop
    {
        for (i = 0; i< 50000; i++);
        if ((S1) == 0)                  // If S1 is pressed
        {
            for (i = 2000; i > 0; i--); // Debounce ~20 ms please
            P1OUT ^= REDLED;
            P4OUT ^= GREENLED;
            counter = 0;
            __bis_SR_register(SCG0);                  // Disable the FLL control loop
            UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation
            UCSCTL2 = FLLD_1 + 249;                           // Set DCO Multiplier for 8MHz
                                                      // (N + 1) * FLLRef = Fdco
                                                      // (249 + 1) * 32768 = 8MHz
            __bic_SR_register(SCG0);                  // Enable the FLL control loop
        }
        else if ((S2) == 0)
        { // else if switch 2 is pressed
            for (i = 2000; i > 0; i--); // Debounce ~20 ms
            P1OUT ^= REDLED;
            P4OUT ^= GREENLED;
            counter++; // increment counter
            if (counter == 1) // if counter is 1
            {

                __bis_SR_register(SCG0);                  // Disable the FLL control loop
                UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation

                UCSCTL2 = FLLD_1 + 124;                          // Set DCO Multiplier for 4MHz
                                                          // (N + 1) * FLLRef = Fdco
                                                          // (124 + 1) * 32768 = 4MHz
                __bic_SR_register(SCG0);                  // Enable the FLL control loop
            } // end of counter =1
            else if (counter == 2)
            {

                __bis_SR_register(SCG0);                  // Disable the FLL control loop
                UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation

                UCSCTL2 = FLLD_1 + 66;                          // Set DCO Multiplier for 4MHz
                                                          // (N + 1) * FLLRef = Fdco
                                                          // (66 + 1) * 32768 = 2MHz
                __bic_SR_register(SCG0);                  // Enable the FLL control loop
            }
            else if (counter == 3)
            {

                __bis_SR_register(SCG0);                  // Disable the FLL control loop
                UCSCTL1 = DCORSEL_4;                      // Select DCO range 16MHz operation

                UCSCTL2 = FLLD_1 + 31;                          // Set DCO Multiplier for 1MHz
                                                          // (N + 1) * FLLRef = Fdco
                                                          // (31 + 1) * 32768 = 1MHz
                __bic_SR_register(SCG0);                  // Enable the FLL control loop
            }
        }
        else
        {

            P1OUT ^= REDLED; // toggle red LED
            P4OUT ^= GREENLED; // toggle green LED
        }
    }
}
