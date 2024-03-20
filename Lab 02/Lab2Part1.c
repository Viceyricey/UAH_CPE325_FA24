/*------------------------------------------------------------------------------
 * File:        Lab2P1Code.c
 * Function:    Prints the size and range of data types
 * Description: Finds the size and range of various data types and outputs them to the console
 * Input:       None
 * Output:      Size and range of different data types
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        Jan 23, 2024
 *----------------------------------------------------------------------------*/
#include <msp430.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop watchdog timer
 // lots of string manipulation for formatting, simply just printing the size of a data type and using float.h and limits.h to get the max and min value
    printf("-----------------------------------------------------------------------------------------\n");
    printf("| Data Type             | Size (in bytes) | Minimum              | Maximum              |\n");
    printf("-----------------------------------------------------------------------------------------\n");

    printf("| CHAR                  | %-15d | %-20d | %-20u |\n", sizeof(char), CHAR_MIN, CHAR_MAX);
    printf("| FLOAT                 | %-15d | %-20e | %-20e |\n", sizeof(float), FLT_MIN, FLT_MAX);
    printf("| INT                   | %-15d | %-20d | %-20d |\n", sizeof(int), INT_MIN, INT_MAX);
    printf("| LONG INT              | %-15d | %-20ld | %-20ld |\n", sizeof(long int), LONG_MIN, LONG_MAX);
    printf("| UNSIGNED LONG INT     | %-15d | %-20u | %-20lu |\n", sizeof(unsigned long int), 0, ULONG_MAX);
    printf("| UNSIGNED CHAR         | %-15d | %-20d | %-20u |\n", sizeof(unsigned char), 0, UCHAR_MAX);
    printf("| UNSIGNED SHORT INT    | %-15d | %-20d | %-20hu |\n", sizeof(unsigned short int), 0, USHRT_MAX);
    printf("| UNSIGNED INT          | %-15d | %-20d | %-20u |\n", sizeof(unsigned int), 0, UINT_MAX);
    printf("| LONG LONG INT         | %-15d | %-20lld | %-20lld |\n", sizeof(long long int), LLONG_MIN, LLONG_MAX);
    printf("| UNSIGNED LONG LONG INT| %-15d | %-20u | %-20llu |\n", sizeof(unsigned long long int), 0, ULLONG_MAX);
    printf("| SHORT INT             | %-15d | %-20d | %-20d |\n", sizeof(short int), SHRT_MIN, SHRT_MAX);
    printf("| DOUBLE                | %-15d | %-20e | %-20e |\n", sizeof(double), DBL_MIN, DBL_MAX);

    printf("-----------------------------------------------------------------------------------------\n");

    return 0;
}

