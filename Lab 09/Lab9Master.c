/*------------------------------------------------------------------------------
 * File:        Master.c
 * Function:    Implements a serial user interface with UART communication. Also uses SPI communication with a slave chip and informs
 *              it of what rate to blink an LED.
 * Description: This code performs LED blinking at a rate given by the user.
 * Input:       User input through serial communication
 * Output:      LEDs on the circuit board.
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        March 18th, 2024
 *----------------------------------------------------------------------------*/

#include <msp430.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


char BeaconArray[] = "Beacon pause: "; // output the prompt for beacon pause rate
char PauseBuffArray[5]; // make an array to store the pausey boy
char PauseHolder; // hold the pause close hold it to your heart

void main(void)
{

    WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer
    unsigned char pause;    // make a pause char

    UART_setup(); // call uart setup function
    SPI_setup(); // call spi setup function

    while(1) // infinite loop
    {
        UART_putStr(BeaconArray); // Send prompt
        UART_getStr(PauseBuffArray,5); // get the value of the pause with a limit of 5 digits

        if (!(strcmp(PauseBuffArray,"?"))) // if the prompt is a question mark then we get the state
        {
            SPI_getState(); // call get state function so that we can get the state lol
        }
        else if (!(strcmp(PauseBuffArray,"-"))) // if minus then we need to return 255 to tell the subroutine to turn off the LED
        {
            pause = 0xFF;  // set pause equal to 255
            SPI_putChar(pause); // send pause to the subroutine
        }
        else
        {
            pause = (unsigned char)atoi(PauseBuffArray); // convert pause from character to integer
            if ((pause>0) && (pause<101)) // check the range of the pause that was given
            {
                SPI_setState(pause); // if it is in the correct range then send it to the subroutine
            }
            else
            {
                UART_putStr("\r\nInvalid pause entered"); // if it outside of the correct bounds then inform the user that their decision sucks
            }
        }
        UART_putStr("\r\n"); // make a newline afterwards
    }
}

/* getChar function definition to take in chars from spi communication */
char SPI_getChar(void)
{
    while(!(IFG2 & UCB0RXIFG)); // Wait until character is ready to be received
    IFG2 &= ~UCB0RXIFG;
    UCB0TXBUF = 0x80;     // Dummy write to start SPI
    while (!(IFG2 & UCB0RXIFG));   // USCI_B0 TX buffer ready?
    return UCB0RXBUF;
}

/* getState function definition (for getting the current pause) */
void SPI_getState(void)
{
    unsigned int k = 0;
    char pauseStr[20];
    SPI_putChar(0x00);
    for (k=0;k < 1000; k++); // Give slave time to load transmit register
    PauseHolder = SPI_getChar();
    sprintf(pauseStr,"\r\nCurrent pause: %u",PauseHolder);
    UART_putStr(pauseStr);
}

/* putChar function definition, waits for character to transmit and then puts the new one into the tx buffer */
void SPI_putChar(char yay)
{
    while (!(IFG2 & UCB0TXIFG));  // Wait for previous character to transmit
    UCB0TXBUF = yay;                // Put character into tx buffer
}

/* setState function that literally just sends the pause value to the subroutine */
void SPI_setState(char state)
{
    SPI_putChar(state);
}

/* spi setup function that sets up the spi communication */
void SPI_setup(void)
{
    UCB0CTL0 = UCMSB + UCMST + UCSYNC;// Sync. mode, 3-pin SPI, Master mode, 8-bit data
    UCB0CTL1 = UCSSEL_2 + UCSWRST; // SMCLK and Software reset
    UCB0BR0 = 0x02;                // Data rate = SMCLK/2 ~= 500kHz
    UCB0BR1 = 0x00;
    P3SEL |= BIT1 + BIT2 + BIT3;   // P3.1,P3.2,P3.3 option select
    UCB0CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
}

/* getChar function that waits for things to be ready and then sends character */
char UART_getChar(void)
{
    while (!(IFG2 & UCA0RXIFG)); // Wait until a character is ready to be read from Rx buffer
    IFG2 &= ~UCA0RXIFG;
    return UCA0RXBUF;
}

/* getStr function that gets the string from UART and puts it into array */
void UART_getStr(char* ReceiveArray, int limit)
{
    char yay;
    unsigned int h = 0;

    yay = UART_getChar();
    while ((yay != '\r') & (h < limit-1))
    {
        ReceiveArray[h++] = yay; // Store received character in receive buffer
        UART_putChar(yay); // Echo character back
        yay = UART_getChar(); // Get next character
    }
    ReceiveArray[h] = (char)0x00; // Terminate string with null character
}

void UART_putChar(char yay)
{
    while (!(IFG2 & UCA0TXIFG));  // Wait for previous character to transmit
    UCA0TXBUF = yay;                // Put character into tx buffer
}

void UART_putStr(char* message)
{
    int u;
    for(u = 0; message[u] != 0; u++)
    {
        UART_putChar(message[u]);
    }
}

void UART_setup(void)
{
    P2SEL |= BIT4 + BIT5;          // Set UC0TXD and UC0RXD to transmit and receive data
    UCA0CTL1 |= UCSWRST;           // Software reset
    UCA0CTL0 = 0;                  // USCI_A0 control register
    UCA0CTL1 |= UCSSEL_2;          // Clock source SMCLK - 1048576 Hz
    UCA0BR0 = 54;                  // Baud rate - 1048576 Hz / 19200
    UCA0BR1 = 0;
    UCA0MCTL = 0x0A;               // remainder 10
    UCA0CTL1 &= ~UCSWRST;          // Clear software reset
}


/* FUNCTION PROTOTYPES */

void SPI_setup(void);

char SPI_getChar(void);

void SPI_putChar(char);

void SPI_getState(void);

void SPI_setState(char);

char UART_getChar(void);

void UART_getStr(char*, int);

void UART_putChar(char);

void UART_putStr(char*);

void UART_setup(void);
