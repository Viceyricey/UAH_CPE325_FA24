/*------------------------------------------------------------------------------
 * File:        lab8part2.c
 * Function:    Send floating data to Serial port
 * Description: UAH serial app expects lower byte first so send each byte at a
 *              time sending Lowest byte first
 * Clocks:      ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO
 *
 * Instructions: Set the following parameters in putty
 * Port: COM1
 * Baud rate: 115200
 * Data bits: 8
 * Parity: None
 * Stop bits: 1
 * Flow Control: None
 *
 *        MSP430F5529
 *     -----------------
 * /|\ |            XIN|-
 *  |  |               | 32kHz
 *  |--|RST        XOUT|-
 *     |               |
 *     |   P2.4/UCA0TXD|------------>
 *     |               | 115200 - 8N1
 *     |   P2.5/UCA0RXD|<------------
 *     |               |
 * Input:     None
 * Output:    Ramp signal in UAH Serial app
 * Author:    Michael Agnew, ma0133@uah.edu
 * Date:      March 6th, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

volatile float myData = 0.0; // data
int i = 0; // loop counter
bool peak = 1; // boolean that keeps track of whether or not the waveform is rising or falling

void UART_setup(void)
{
    P3SEL |= BIT3 | BIT4;       // Make sure that these are capable of transmitting and receiving data
    UCA0CTL1 |= BIT0;           // I think this is for software reset
    UCA0CTL0 = 0;               // Control register
    UCA0CTL1 |= UCSSEL_2;       // SMCLK
    UCA0BR0 = 18;               // 1048576 Hz/57,600 lower byte
    UCA0BR1 = 0x00;             // This is for the upper byte
    UCA0MCTL = 0x02;            // do the mod thing
    UCA0CTL1 &= ~BIT0;          // UCSWRST software reset
}

void sendChar(char c)
{
    while (!(UCA0IFG & UCTXIFG)); // Wait for previous character to transmit
    UCA0TXBUF = c;               // Put character into tx buffer
}

int main()
{
    WDTCTL = WDT_MDLY_32; // 32 becuase 1000 sucks, also MDLY
    UART_setup();                // Initialize USCI_A0 module in UART mode
    SFRIE1 |= WDTIE;                // Enable watchdog interrupts

    myData = 0.0;
    __bis_SR_register(LPM0_bits + GIE);
}

// Sends a ramp signal; amplitude of one period ranges from 0.0 to 9.9
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    char *pntr = (char*)&myData;
    char next = 0;
    sendChar(0x55);
    for (next = 0; next < 4; next++) // iterate 4 times essentially
    {
        sendChar(pntr[next]); // send character
    }
    if (myData < 0.0) // while the data is 0, move it move it
    {
        peak = 1; // set the peak to true
        i += 1; // inc
    }
    else if (myData >= 9.0)
    {
        peak = 0; // set peak to false
        i += 1; // incing time
    }
    if (peak) // if peak is true ( which means it's like going to the top or something )
    {
        myData += 0.1; // increase
    }
    else
    {
        myData -= 0.1; // if peak is not true, then it must be false duh so go down
    }
    if (i == 12) // once it reaches 12, you know that you have 6 triangles and we only want 6 triangles please dont have more triangles than me
    {
        SFRIE1 &= ~ WDTIE; // disable watchdog interrupts
    }
}

