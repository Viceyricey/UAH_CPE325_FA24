/*------------------------------------------------------------------------------
 * File:        Lab2P3Code.c
 * Function:    Finds the dot product and sum of two arrays
 * Description: Finds the dot product and sum of two arrays
 * Input:       None
 * Output:      Sum and dot product of 2 given arrays
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        Jan 23, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>
#include <math.h>


int main(void)
{

    WDTCTL = WDTPW + WDTHOLD;   // Stop watchdog timer

    int x[4] = {9,0,0,4}; // initialize array x
    int y[4] = {3,6,1,-2}; // initialize array y
    int sum[4]; // initialize the sum array
    int i = 0; // initialize loop counter i

    for(i = 0; i < 4; i++) // loop while i is less than the size of the array
    {
        sum[i] = x[i] + y[i]; // add the current index of x and y to the sum array
    }

    printf("Sum of the arrays: [");

    for(i = 0; i < 4; i++) // loop while i is less than the size of the array
    {
        printf("%d", sum[i]); // print the current index of the sum array
        if (i == 3)
        {
            break;
        }
        printf(" ");
    }

    printf("]");
    printf("\n");

    int dot = 0; // initialize the integer dot to hold the value of the dot product

    for(i = 0; i < 4; i++) // loop while i is less than the size of the array
    {
        dot += x[i]*y[i]; // add the product of the index of x and y to the variable dot
    }

    printf("Dot Product of the arrays: [%d]", dot); // print the value of the dot product
    printf("\n");

    return 0;
}
