/*------------------------------------------------------------------------------
 * File:        Lab2BonusCode.c
 * Function:    Multiplies Matrices together
 * Description: Takes two matrices that are given, and multiplies them together before outputting
 * the result
 * Input:       None
 * Output:      Outputs the given matrices A and B and the resulting product matrix C
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        Jan 23, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>
#include <math.h>


int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop watchdog timer


    const int rows = 8; // setting number of rows equal to 8
    const int cols = 8; // setting the number of columns equal to 8

    // Initializing matrix A, B, and initializing matrix C to hold the result
    int mA[8][8] = {{1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}};
    int mB[8][8] = {{1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}, {1,2,3,4,1,2,3,4}};
    int mC[8][8];

    // initializing loop counters
    int i = 0;
    int j = 0;
    int k = 0;

    printf("The matrix A is:\n");
    printf("-----------------------------------------------\n");
    for (i = 0; i < rows; i++) // loop while i is less than rows
    {
        for (j = 0; j < cols; j++) // loop while j is less than cols
        {
            printf("| %d | ", mA[i][j]); // print out the current index of the matrix
            if (j == cols - 1) // format manipulation for output
            {
                printf("\n");
                printf("-----------------------------------------------\n");
            }
        }
    }


    printf("The matrix B is:\n");
    printf("-----------------------------------------------\n");
    for (i = 0; i < rows; i++) // loop while i is less than rows
    {
        for (j = 0; j < cols; j++) // loop while j is less than cols
        {
            printf("| %d | ", mA[i][j]); // print out the current index of the matrix
            if (j == cols - 1) // format manipulation for output
            {
                printf("\n");
                printf("-----------------------------------------------\n");
            }
        }
    }



    for (i = 0; i < rows; i++) // loop while i is less than rows
    {
        for (j = 0; j < cols; j++) // loop while j is less than cols
        {
            mC[i][j] = 0; // initialize the current index of matrix C to 0 to prevent garbage data

            for (k = 0; k < rows; k++) // loop while k is less than the size of the matrix
            {
                mC[i][j] += mA[i][k]*mB[k][j]; // add the product of the current index of A multiplied by the current index of B
            }

        }

    }


    printf("The product matrix of matrix A and B is:\n");
    printf("-------------------------------------------------------\n");
    for (i = 0; i < rows; i++) // loop while i is less than rows
    {
        for (j = 0; j < cols; j++) // loop while j is less than cols
        {
            printf("| %d | ", mC[i][j]); // print the current index of the matrix
            if (j == cols - 1)
            {
                printf("\n");
                printf("-------------------------------------------------------\n");
            }
        }
    }


    return 0;
}

