/*------------------------------------------------------------------------------
 * File:        Slave.c
 * Function:    Implements LED blinking based on parameters passed from the master code.
 * Description: This code performs LED blinking at a rate given by the user.
 * Input:       None
 * Output:      LEDs on the circuit board.
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        March 18th, 2024
 *----------------------------------------------------------------------------*/

#include "msp430x20x3.h"

#define LED_ON()       P1OUT |= 0x01
#define LED_OFF()     P1OUT &= ~0x01

unsigned int juan;

unsigned char pause = 50;

unsigned char PauseHolder;

void main(void)
{
    sysInit();        // System Initialize function call (sets up the system for our purposes)
    SPI_setup();      // SPI_Setup function call (sets up spi communication)
    initLED();        // initialize the LED with the correct parameters and initialize it to be on

    while(1)
    {

        if (PauseHolder == 0)
        {
            USISRL = pause;
        }
        else if (PauseHolder <= 100)
        {
            pause = PauseHolder;
        }
        else if (PauseHolder == 0xff)
        {
            pause = 0xff;
            LED_OFF();
        }
        _BIS_SR(LPM0_bits + GIE); // LPM0 w/ Interrupt
    }
}

void initLED(void)
{
    P1DIR |= BIT0;                // P1.0 as output - LED3
    LED_ON();
}

void SPI_setup(void)
{
    USICTL0 |= USISWRST;          // Set UCSWRST -- needed for re-configuration process
    USICTL0 |= USIPE5 + USIPE6 + USIPE7 + USIOE; // SCLK-SDO-SDI port enable,MSB first
    USICTL1 = USIIE;              // USI Counter Interrupt enable
    USICTL0 &= ~USISWRST;         // **Initialize USCI state machine**
    USICNT = 8;                   // Load bit counter, clears IFG
}

void sysInit(void) // system initialization (from demo code)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    WDTCTL = WDT_MDLY_32;     // 32 ms interval mode
    IE1 |= WDTIE;                // enable watchdog interrupts
    BCSCTL1 = CALBC1_1MHZ;        // Set DCO
    DCOCTL = CALDCO_1MHZ;
}



#pragma vector = USI_VECTOR // this is also from demo code
__interrupt void USI_ISR(void)
{
    PauseHolder = USISRL;           // Read new command
    USICNT = 8;                   // Load bit counter for next TX
    _BIC_SR_IRQ(LPM0_bits);       // Exit from LPM0 on RETI
}


#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    if (juan == 1) // juan
    {
        LED_OFF(); // turn off led
        juan++;
    }
    else if (juan > pause)
    {
        LED_ON(); // turn on led
        juan = 0; // reset counter
    }
    else if (pause == 0xff)
    {
        LED_OFF();
    }
    else
    {
        juan++; // increment counter

    }
}

/* FUNCTION PROTOTYPES */

void initLED(void);

void SPI_setup(void);

void sysInit(void);

