#include <msp430.h>

#define REDLED 0x01                     // Mask for BIT0 = 0000_0001b

volatile unsigned int buzzerToggle = 0;


void main(void)
{
    WDTCTL = WDT_ADLY_1000; // Set watchdog timer to interval mode (1 second)
    SFRIE1 |= WDTIE; // Enable watchdog timer interrupt

    _BIS_SR(LPM0_bits + GIE); // Enter low-power mode 0 and enable global interrupts
}

#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    static unsigned int buzz = 0;

    if (buzz == 0)
    {
        P2DIR |= BIT2; // buzzer configuration at 2.4
        P2SEL |= BIT2; // more buzzer configuration at 2.4


        P1DIR |= REDLED; // Set LED1 as output
        P1REN |= BIT1; // Enable the pull-up resistor at P1.1
        P1OUT &= ~REDLED; // turn on Red LED


        TB0CCR0 = 1000;  // Set Timer B0 period to produce 1 KHz sound
        TB0CCTL0 = CCIE;  // Enable Timer B0 CCR0 interrupt
        TB0CTL = TBSSEL_2 + MC_1;  // Use SMCLK, Up mode for Timer B0

        TBCCR1 = 524; // Set the duty cycle for the buzzer (50%)

        TBCCTL1 = OUTMOD_7; // reset/set mode
        TBCTL = TBSSEL_2 + MC_1 + ID_0; // SMCLK, up mode, no division

        buzz = 1; // change the state of the buzzer after the timer
    }
    else
    {
        P2SEL &= ~BIT2; // Turn off the buzzer
        P1OUT ^= REDLED; // turn off red LED

        buzz = 0;
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B0_ISR(void) {

    if (buzzerToggle == 0)
    {
        P2OUT |= BIT2;  // Set buzzer to on
        buzzerToggle = 1;

    } else
    {
        P2OUT &= ~BIT2;  // Set buzzer to off
        buzzerToggle = 0;
    }
}
