/*------------------------------------------------------------------------------
 * File:        Lab10P1P2.c
 * Function:    Accelerometer Telemetry Processor
 * Description: This code performs calculations on data passed from accelerometer and turns on LEDs based on angular deviation.
 * Input:       None
 * Output:      LEDs on the circuit board and waveform in serial app.
 * Author(s):   Michael Agnew, ma0133@uah.edu
 * Date:        April 1, 2024
 *----------------------------------------------------------------------------*/

#include <msp430.h>
#include <math.h>
#include <stdint.h>


#define samplerate 10 // sample rate
#define threshhold 15 // threshold for angular deviation
#define grav 10 // gravity


volatile uint16_t ADCXval, ADCYval, ADCZval;

void TimerA_setup(void)
{
    TA0CCR0 = 3277 / samplerate; // Calculate CCR0 for desired sampling rate
    TA0CTL = TASSEL_1 + MC_1; // ACLK, up mode
    TA0CCTL0 = CCIE; // Enabled interrupt
}

void ADC_setup(void)
{
    P6DIR &= ~(BIT3 + BIT4 + BIT5); // Configure P6.3, P6.4, P6.5 as input pins
    P6SEL |= BIT3 + BIT4 + BIT5; // Configure P6.3, P6.4, P6.5 as analog pins

    ADC12CTL0 = ADC12ON + ADC12SHT0_8 + ADC12MSC; // Configure ADC converter
    ADC12CTL1 = ADC12SHP + ADC12CONSEQ_1; // Use sample timer, single sequence
    ADC12MCTL0 = ADC12INCH_3; // ADC A3 pin - X-axis
    ADC12MCTL1 = ADC12INCH_4; // ADC A4 pin - Y-axis
    ADC12MCTL2 = ADC12INCH_5 + ADC12EOS; // ADC A5 pin - Z-axis, End of Sequence
    ADC12IE |= ADC12IE2; // Enable ADC12IFG.2
    ADC12CTL0 |= ADC12ENC; // Enable conversions
}

void UART_putCharacter(uint8_t c)
{
    while (!(UCA0IFG & UCTXIFG)); // Wait for previous character to be sent
    UCA0TXBUF = c; // Send byte to the buffer for transmitting
}

void UART_setup(void)
{
    P3SEL |= BIT3 + BIT4;               // Set up Rx and Tx bits
    UCA0CTL0 = 0;                       // Set up default RS-232 protocol
    UCA0CTL1 |= BIT0 + UCSSEL_2;        // Disable device, set clock
    UCA0BR0 = 27;                       // 1048576 Hz / 38400
    UCA0BR1 = 0;
    UCA0MCTL = 0x94;
    UCA0CTL1 &= ~BIT0;                  // Start UART device
}

void sendData(void)
{
    int i;

    // grav CALCULATIONS

    float yGrav = (((float)ADCYval * 3.0 / 4095.0 * 100.0 / 3.0) / 10.0 - 2.5);

    float zGrav = (((float)ADCZval * 3.0 / 4095.0 * 100.0 / 3.0) / 10.0 - 5.0);

    float xGrav = (((float)ADCXval * 3.0 / 4095.0 * 100.0 / 3.0) / 10.0 - 5.0);



    UART_putCharacter(0x55);

    uint8_t *y_pointer = (uint8_t *)&yGrav;
    uint8_t *x_pointer = (uint8_t *)&xGrav;
    uint8_t *z_pointer = (uint8_t *)&zGrav;

    for (i = 0; i < 4; i++)
    { // Send x acceleration - one byte at a time
        UART_putCharacter(x_pointer[i]);
    }

    for (i = 0; i < 4; i++)
    { // Send y acceleration - one byte at a time
        UART_putCharacter(y_pointer[i]);
    }

    for (i = 0; i < 4; i++)
    { // Send z acceleration - one byte at a time
        UART_putCharacter(z_pointer[i]);
    }
}

void sendDataDebug(float data)
{
    int i;
    uint8_t *data_pointer = (uint8_t *)&data;
    for (i = 0; i < 4; i++)
    {
        UART_putCharacter(data_pointer[i]);
    }
}

void bubbleLevel(void) {
    float xGrav = (((float)ADCXval * 3.0 / 4095.0 * 100.0 / 3.0) / 10.0 - 5.0);
    float yGrav = (((float)ADCYval * 3.0 / 4095.0 * 100.0 / 3.0) / 10.0 - 5.0);
    float zGrav = (((float)ADCZval * 3.0 / 4095.0 * 100.0 / 3.0) / 10.0 - 5.0);


    // CALCULATING ANGULAR DEVIATION OF X AND Y WITH RESPECT TO Z
    float x_deviation = atan2(xGrav, zGrav) * 180 / M_PI;

    float y_deviation = atan2(yGrav, zGrav) * 180 / M_PI;

    // X AXIS LED
    if (x_deviation > threshhold)
    {
        P1OUT |= BIT0; // activate red LED
    } else if (x_deviation < -threshhold)
    {
        P1OUT &= ~BIT0;
    } else {
        P1OUT &= ~BIT0;
    }

    // Y AXIS LED
    if (y_deviation > threshhold)
    {
        P4OUT |= BIT7; // turn on green LED
    }
    else if (y_deviation < -threshhold)
    {
        P4OUT &= ~BIT7;
    }
    else
    {
        P4OUT &= ~BIT7;
    }
}



void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    TimerA_setup(); // Setup timer to send ADC data
    ADC_setup(); // Setup ADC
    UART_setup(); // Setup UART for RS-232
    P1DIR |= BIT0 + BIT1; // Set P1.0 and P1.1 as output for LEDs
    P4DIR |= BIT7;
    __enable_interrupt(); // Enable global interrupt

    while (1)
    {
        ADC12CTL0 |= ADC12SC; // Start conversions
        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0
        sendData(); // Send data to serial app
        bubbleLevel(); // Check bubble level and control LEDs
    }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void)
{
    ADCXval = ADC12MEM0; // X-axis
    ADCYval = ADC12MEM1; // Y-axis
    ADCZval = ADC12MEM2; // Z-axis
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerA_isr()
{
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}
