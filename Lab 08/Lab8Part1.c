/*------------------------------------------------------------------------------
 * File:        Lab8CodeFinal.c
 * Function:    Makes a market bot that allows the user the buy fruits from it
 * Description: Market Bot sells fruits to user
 * Input:       User Input from MobaXTerm
 * Output:      MobaXTerm Terminal
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        3/6/2024
 * ---------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>
#include <string.h>

// ANSI escape codes for colors
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_AUTHOR1 "\x1b[35m"  // Makes a pink color for Market Bot
#define ANSI_COLOR_AUTHOR2 "\x1b[36m"
#define ANSI_COLOR_USER    "\x1b[34m"  // Makes a blue color for user

void UART_sendChar(char thing) // function for sending character to terminal
{
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = thing;
}

void UART_sendString(const char* str) // sends a string to the terminal
{
    while (*str != '\0') // while not at the end of the string
    {
        UART_sendChar(*str++); // output the string to the terminal
    }
}

char UART_getChar(void) // gets the character from the terminal
{
    while (!(UCA0IFG & UCRXIFG));
    return UCA0RXBUF;
}

void UART_getLine(char* bufferArray, int limit, const char* inputName) // get the line input from the terminal
{

    int i = 0;
    char c;

    // Display the input name prompt before waiting for user input
    UART_sendString(ANSI_COLOR_USER);
    UART_sendString(inputName);
    UART_sendString(": " ANSI_COLOR_RESET);

    while (1)
    {
        c = UART_getChar(); // initialize c as a character from getChar


        if (c == '\b' && i > 0)
        {
            UART_sendChar('\b');
            UART_sendChar(' '); // Use this to overwrite the character
            UART_sendChar('\b');
            i -= 1; // decrement the length of the buffer array
            bufferArray[i] = '\0'; // get rid of the character from the buffer array
            continue;

        }

        if ((c == '\r') || (i == limit - 1))
        {
            break; // bweak hehe
        }

        bufferArray[i++] = c; // set the new character in the buffer array to c

        UART_sendChar(c); // send c to the terminal

    }
    bufferArray[i] = '\0'; // set the end of the buffer array to the null character

}


void UART_setup(void) // setup for the serial connection
{
    P3SEL |= BIT3 + BIT4;   // Set USCI_A0 RXD/TXD to receive/transmit data
    UCA0CTL1 |= UCSWRST;    // Set software reset during initialization
    UCA0CTL1 |= UCSSEL_2;   // Clock source SMCLK
    UCA0BR0 = 0x09;         // 1048576 Hz  / 115200 lower byte
    UCA0BR1 = 0x00;         // upper byte
    UCA0MCTL |= UCBRS_1 + UCBRF_0;        // Modulation (UCBRS0=0x01, UCOS16=0)
    UCA0CTL1 &= ~UCSWRST;   // Clear software reset to initialize USCI state machine
}


int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // stop the watchdog timer
    UART_setup(); // set up the UART

    char userInput[100]; // make an array to store the user input
    char userName[100]; // make an array to store the user name

    while (1)
    {

        UART_sendString(ANSI_COLOR_AUTHOR1 "Market Bot: " ANSI_COLOR_RESET); // output market bot in pink
        UART_sendString("Hi, I am Market Bot! What is your name?\r\n"); // output the market bot greeting
        UART_getLine(userName, 100, "User"); // output the name of the user
        UART_sendString("\r\n"); // Use this to make a new space

        while (1)
        {

            UART_sendString(ANSI_COLOR_AUTHOR1 "Market Bot: " ANSI_COLOR_RESET); // output the market bot in pink
            UART_sendString("Today we have apples, bananas, and oranges. What would you like to order?\r\n"); // output the options for fruit
            UART_getLine(userInput, 100, userName); // output the name of the user
            UART_sendString("\r\n"); // add an extra line


            if (strcmp(userInput, "apples") == 0 || strcmp(userInput, "bananas") == 0 || strcmp(userInput, "oranges") == 0) // see if the fruit match
            {
                break; // bnweak out heuueh
            }


            UART_sendString(ANSI_COLOR_AUTHOR1 "Market Bot: " ANSI_COLOR_RESET); // output the market bot in pink
            UART_sendString(userInput); // output what the user said
            UART_sendString(" is not available today.\r\n"); // tell the user that the fruit is not available
        }


        int fruitAmount;
        int price;
        if (strcmp(userInput, "apples") == 0)
        {
            price = 2; // if apples, price is 2
        }
        else if (strcmp(userInput, "bananas") == 0)
        {
            price = 1; // if banana, price is 1
        }
        else if (strcmp(userInput, "oranges") == 0)
        {
            price = 3; // orange is 3 bruh
        }

        char costArray[100]; // make an array for the cost
        sprintf(costArray, "%d", price); // print the price


        UART_sendString(ANSI_COLOR_AUTHOR1 "Market Bot: " ANSI_COLOR_RESET); // print market bot in pink
        UART_sendString(userInput); // print user input
        UART_sendString(" cost $"); // print the cost
        UART_sendString(costArray); // print the cost but like for real this time
        UART_sendString(" each. How many would you like?\r\n"); // asking how many FRUIT BOYS the user wants
        UART_getLine(userInput, 100, userName); // get the user's input

        sscanf(userInput, "%d", &fruitAmount); // get fruit or something
        int total = price * fruitAmount; // I remember when the MSP430 said "it's mathing time" and then it math'd all over the place
        char total_str[10]; // string time
        sprintf(total_str, "%d", total); // print total


        UART_sendString("\r\n"); // Add new line for spacing
        UART_sendString(ANSI_COLOR_AUTHOR1 "Market Bot: " ANSI_COLOR_RESET); // print name of market bot in pink
        UART_sendString("The total for "); // total
        UART_sendString(userInput); // user stuff
        UART_sendString(" is $"); // money money
        UART_sendString(total_str); // total_str dont ask
        UART_sendString(". Thank you for shopping with me!\r\n"); // thanks man you too
        UART_sendString("\r\n"); // new line new me
    }
}
