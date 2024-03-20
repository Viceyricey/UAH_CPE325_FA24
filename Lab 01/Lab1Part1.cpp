#include <msp430.h>
#include <stdio.h>

int main()
{

    WDTCTL = WDTPW + WDTHOLD;   // Stop watchdog timer
    int base = 2; // initialize base
    const int exp = 13; // initialize exponent
    int value = base; // set value equal to the base

    int i = 1; // initialize the loop counter

    for (i = 1; i < exp; i++) // for loop until i reaches the value of the exponent
    {
        base = base * value; // setting base equal to base multiplied by value
    }

    // string manipulation in order to properly format the output

    printf("%d raised to the power ", value);
    printf("%d is ", exp);
    printf("%d", base);
    printf("\n");


    return 0;
}