/*
 File:         AgnewMichaelFinalproj.c
 Function:     Allows the user to play hangman on MobaXTerm
 Description:  Prompts the user with a menu that can be navigated with switches before finally entering the game. Once the game starts, the user
               will be able to fully play hangman with the buzzer and red LED coming on when an incorrect guess is made, and the green LED
               coming on when a correct guess is made. The game ends when the user correctly guesses the word or when the user runs out of guesses.
 Input:        Switches 1,2; and typing in UART console.
 Output:       Red LED, Green LED, Buzzer, UART console.
 Author(s):    Michael Agnew, ma0133@uah.edu
 Date:         April 4th, 2024
 Revised:      April 15th, 2024  */

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// COLORS YAYAYAYAY
#define ANSI_COLOR_RESET   "\x1b[0m" // for restting the color
#define ANSI_COLOR_USER    "\x1b[34m"  // Makes a blue color for user
#define correct    "\x1b[32m" // green
#define incorrect    "\x1b[31m" // red
#define warning    "\x1b[33m" // yellow


#define S2 P1IN&BIT1 // bitmask for switch 2
#define S1 P2IN&BIT1 // bitmask for switch 1
#define GREENLED 0x80             // Mask for BIT7 = 1000_0000b
#define REDLED 0x01               // Mask for BIT0 = 0000_0001b

volatile unsigned int buzzerToggle = 0;
unsigned int i = 0;
bool playing = 1;

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

void controlBuzzer(bool on) // buzzer controlling function
{
    if (on)
    {
        P2OUT |= BIT4; // set buzzer on
    }
    else
    {
        P2OUT &= ~BIT4; // set buzzer off
    }
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

void chooseMode() // print header for choosing the mode
{
    UART_sendString("***************************************************************\r\n");
    UART_sendString("You may choose between two different game modes. One of which\r\n");
    UART_sendString("plays with nouns as the words, the other that plays with verbs.\r\n");
    UART_sendString("Press [SW1] to play with nouns, press [SW2] to play with verbs.\r\n");
    UART_sendString("***************************************************************\r\n");
}

void nounLength() // printing header for choosing nouns
{
    UART_sendString("*********************************************************************************************\r\n");
    UART_sendString("You have chosen to play with nouns; now please choose the length of words to play with.\r\n");
    UART_sendString("For words of 3-6 letters in length, press [SW1]. For words 7+ letters in length, press [SW2].\r\n");
    UART_sendString("*********************************************************************************************\r\n");
}

void verbLength() // print header for choosing verbs
{
    UART_sendString("*********************************************************************************************\r\n");
    UART_sendString("You have chosen to play with verbs; now please choose the length of words to play with.\r\n");
    UART_sendString("For words of 3-6 letters in length, press [SW1]. For words 7+ letters in length, press [SW2].\r\n");
    UART_sendString("*********************************************************************************************\r\n");
}

void word36() // print header for choosing words of 3-6 letters
{
    UART_sendString("***********************************************************************************\r\n");
    UART_sendString("You have chosen to play with words of 3-6 letters in length; now please choose the\r\n");
    UART_sendString("difficulty of the game. Press [SW1] for EASY; and press [SW2] for HARD.\r\n");
    UART_sendString("***********************************************************************************\r\n");

}

void plus7() // print header for choosing 7+ letters
{
    UART_sendString("**********************************************************************************\r\n");
    UART_sendString("You have chosen to play with words of 7+ letters in length; now please choose the.\r\n");
    UART_sendString("difficulty of the game. Press [SW1] for EASY; and press [SW2] for HARD.\r\n");
    UART_sendString("**********************************************************************************\r\n");
}

void easyDiff() // print header for choosing easy difficulty
{
    UART_sendString("***********************************************************************************\r\n");
    UART_sendString("You have chosen to play on EASY difficulty. This means that the words will be\r\n");
    UART_sendString("simpler and more common which should make it easier. Enjoy!\r\n");
    UART_sendString("***********************************************************************************\r\n");
}

void hardDiff() // print header for choosing hard difficulty
{
    UART_sendString("***********************************************************************************\r\n");
    UART_sendString("You have chosen to play on HARD difficulty. This means that the words will be\r\n");
    UART_sendString("more complex and less common which should make it harder. Enjoy!\r\n");
    UART_sendString("***********************************************************************************\r\n");
}

void rules() // print rules function
{
    UART_sendString("***********************************************************************************\r\n");
    UART_sendString("If you don't already know, hangman consists of guessing letters based on the number\r\n");
    UART_sendString("of spaces that are given at the beginning of the game. The player gets 5 chances to\r\n");
    UART_sendString("guess the word, which each wrong guess adding a limb to their character. When all\r\n");
    UART_sendString("the letters are guessed before the player loses, that is when the player wins.\r\n");
    UART_sendString("If the player guesses a letter incorrectly, the buzzer will sound and the red LED\r\n");
    UART_sendString("will come on, as well as updating the current diagram. When the player guesses\r\n");
    UART_sendString("correctly, the green LED will come on and the diagram will update accordingly.\r\n");
    UART_sendString("That's about it! Good luck!\r\n");
    UART_sendString("***********************************************************************************\r\n");
}

bool checkGuess(const char* word, char guess)
{
    for (i = 0; i < strlen(word); i++)
    {
        if (word[i] == guess)
        {
            return true; // found character
        }
    }
    return false; // not found LOL
}

void updateWordDisplay(const char* word, char guess, char* display)
{
    for (i = 0; i < strlen(word); i++)
    {
        if (word[i] == guess)
        {
            display[i * 2] = guess; // update thing
        }
    }
}

void printTree(int limbs)
{
    // Print the hangman tree with the appropriate number of limbs

    UART_sendString("   ______\r\n");
    UART_sendString("  |      |\r\n");
    UART_sendString("  |      |\r\n");

    // Print the head if limbs are 1 or more
    if (limbs > 0)
    {
        UART_sendString("  |      O\r\n");
    }
    else
    {
        UART_sendString("  |       \r\n");
    }

    // Print the body if limbs are 2 or more
    if (limbs > 1)
    {
        UART_sendString("  |      |\r\n");
        UART_sendString("  |     / ");
    }
    else
    {
        UART_sendString("  |       \r\n");
    }

    // Print the left arm if limbs are 3 or more
    if (limbs > 2)
    {
        UART_sendString("\\\r\n");
    }
    else
    {
        UART_sendString("\r\n  |        ");
    }

    // Print the right arm if limbs are 4 or more
    if (limbs > 3) {
        UART_sendString("  |      | \r\n");
        UART_sendString("  |     / ");
    } else {
        UART_sendString("\r\n");
    }

    // Print the left leg if limbs are 5 or more
    if (limbs > 4) {
        UART_sendString("\\\r\n");
    } else {
        UART_sendString("\r\n  |        \r\n");
    }

    // Print the right leg if limbs are 6 or more
    UART_sendString("  |        \r\n");


    UART_sendString("  |____\r\n");
    UART_sendString(" |______|\r\n");

}

void playHangman(const char* word) // heart and soul of the hangman code
{
    char userInput[100];
    bool playing = true;
    int attempts = 5;
    int limbs = 0;
    int uniqueLettersRemaining = 0; // Variable to track remaining unique letters
    char wordDisplay[100];
    bool guessedLetters[26] = {false}; // Array to track guessed letters, initialized to false
    bool checkLetters[26] = {false}; // Array to track if a letter has been guessed before

    // this loop counts the number of unique letters in the word
    for (i = 0; i < strlen(word); i++)
    {
        if (!guessedLetters[word[i] - 'a']) // converts character to a 0-based index value by subtracting the ascii value of a
        {
            guessedLetters[word[i] - 'a'] = true; // if the character has not been guessed, it sets it to true
            uniqueLettersRemaining++; // increase the number of unique characters, which is the number of characters left in the word to guess
        }
    }

    // print the word display with underlines
    for (i = 0; i < strlen(word) * 2; i += 2)
    {
        wordDisplay[i] = '_';
        wordDisplay[i + 1] = ' ';
    }
    wordDisplay[strlen(word) * 2] = '\0'; // terminate with null character

    // print the initial hangman tree as well as what the current looking word is
    printTree(limbs); // print the tree with the parameter of number of limbs
    UART_sendString("\n\r");
    UART_sendString("Word: ");
    UART_sendString(wordDisplay); // print the display word
    UART_sendString("\r\n");

    while (playing && attempts > 0 && uniqueLettersRemaining > 0) // update the game condition
    {
        UART_sendString("What is your guess?\r\n"); // prompt user for their guess
        UART_getLine(userInput, 100, "User"); // print User:

        // check if the letter has already been guessed
        if (checkLetters[userInput[0] - 'a'])
        {
            UART_sendString(warning);
            UART_sendString("\r\nYou have already guessed this letter. Please guess a different letter.\r\n"); // inform user that they have already guessed that word
            UART_sendString(ANSI_COLOR_RESET);
            continue; // Reprompt for a different guess
        }

        // Check if the guessed character is in the word
        if (checkGuess(word, userInput[0]))
        {
            checkLetters[userInput[0] - 'a'] = true; // Mark the letter as guessed
            UART_sendString(correct);
            UART_sendString("\r\nCorrect guess!\r\n"); // inform the user they guessed correctly
            UART_sendString(ANSI_COLOR_RESET);
            updateWordDisplay(word, userInput[0], wordDisplay);
            uniqueLettersRemaining--; // Decrement unique letters remaining if guess is correct
            P4OUT |= GREENLED; // turn on green LED to show that the guess was correct
            __delay_cycles(1200000); // delay for 1.2s
            P4OUT &= ~GREENLED; // turn off green LED after the delay

        }
        else
        {
            checkLetters[userInput[0] - 'a'] = true; // mark the letter if it has been guessed
            UART_sendString(incorrect);
            UART_sendString("\r\nIncorrect guess!\r\n"); // tell the user that their guess was wrong
            UART_sendString(ANSI_COLOR_RESET);
            limbs++; // increase the number of limbs
            P1OUT |= REDLED; // turn on the red LED to show the user their guess was wrong
            TA2CCTL1 = OUTMOD_7; // configure the mode to reset/set
            TA2CTL = TASSEL_2 + MC_1; // configure SMCLK
            TA2CCR0 = 1048; // count up to 1048 (1khz)
            TA2CCR1 = 524; // 50% duty cycle
            controlBuzzer(true); // turn on buzzer
            __delay_cycles(1200000); // delay for 1.2s
            P1OUT &= ~REDLED; // turn off the red LED after delay
            controlBuzzer(false); // turn off buzzer
            TA2CCR0 = 0; // turn off buzzer
            TA2CCR1 = 0; // turn off buzzer

        }

        // print all of the letters that have been guessed so far
        UART_sendString("\n\rGuessed letters: ");
        for (i = 0; i < 26; i++) // 26 because alphabet lol
        {
            if (checkLetters[i]) // current element
            {
                UART_sendChar('a' + i);
                UART_sendChar(' ');
            }
        }
        UART_sendString("\r\n");

        // update the display
        UART_sendString("Word: ");
        UART_sendString(wordDisplay);
        UART_sendString("\r\n");


        // print new tree
        printTree(limbs);

        // make sure that all of the letters have been guessed
        if (uniqueLettersRemaining == 0) // Update condition for ending the game
        {
            UART_sendString(correct);
            UART_sendString("Congratulations! You won!\r\n"); // telkl player that they won
            UART_sendString(ANSI_COLOR_RESET);
            playing = false; // set playing to false when this is over
        }

        // if the player has reached the max amount of guesses, end them
        if (limbs == attempts)
        {
            UART_sendString(incorrect);
            UART_sendString("Sorry, you ran out of attempts. Game over!\r\n"); // tell user they suck
            UART_sendString(ANSI_COLOR_RESET);
            playing = false; // set playing to false because the game is over lol
        }
    }
}

int main(void)
{

    // ARRAYS OF NOUNS AND VERBS FOR DIFFERENT GAMEMODES
    const char* easy_nouns_3_6[] = {"cat", "dog", "bird", "fish", "lion", "door", "brain"};

    const char* hard_nouns_3_6[] = {"abide", "corbel", "desk", "champ", "curdle"};

    const char* easy_nouns_7_plus[] = {"triangle", "midwest", "postage", "blanket"};

    const char* hard_nouns_7_plus[] = {"rhombus", "hickory", "copyist", "bowties"};

    const char* easy_verbs_3_6[] = {"run", "jump", "talk", "sing", "read"};

    const char* hard_verbs_3_6[] = {"faze", "jilt", "quip", "snub", "vex"};

    const char* easy_verbs_7_plus[] = {"hitting", "crumpling", "networking", "discover"};

    const char* hard_verbs_7_plus[] = {"waltzed", "baptise", "blunder", "foresaw"};


    WDTCTL = WDTPW + WDTHOLD; // stop the watchdog timer

    // SETTING UP GREEN LED
    P4DIR |= GREENLED;          // Set P4.7 to output direction
    P4OUT &= ~GREENLED;          // Set P4OUT to 1000_0000b (LED1 is ON)

    // SETTING UP RED LED
    P1DIR |= REDLED;  // assign red led
    P1OUT &= ~REDLED; // make sure that it's off

    // SETTING UP SWITCH 1
    P2DIR &= ~BIT1;    // Set P2.1 as input for S1 input
    P2REN |= BIT1;     // Enable the pull-up resistor at P2.1
    P2OUT |= BIT1;     // Required for proper IO

    // SETTING UP SWITCH 2
    P1DIR &= ~BIT1;    // Set P1.1 as input for S2 input
    P1REN |= BIT1;     // Enable the pull-up resistor at P1.1
    P1OUT |= BIT1;     // Required for proper IO

    // SETTING UP BUZZER
    P2DIR |= BIT4;
    P2SEL |= BIT4;

    UART_setup(); // set up the UART

    char userInput[100]; // make an array to store the user input
    char userName[100]; // make an array to store the user name


    UART_sendString("Shall we play a game?\r\n"); // ask user if they would like to play a game
    UART_sendString("[SW1] - y | [SW2] - n\r\n"); // give user choice of yes or no through the switches on the board
    while (playing) // infinite loop waiting for switch press
    { // while 1

        if ((S1) == 0) // if switch 1 is pressed, enter this conditional
        { // if sw1 pressed
            for (i = 0; i < 20000; i++); // debounce

            UART_sendString("***********************************************\r\n"); // output market bot in pink
            UART_sendString("Hello and welcome to Hangman, here is the menu\r\n"); // output the market bot greeting
            UART_sendString("where you can navigate and choose what game and\r\n"); // output the market bot greeting
            UART_sendString("mode you would like to play\r\n"); // output the market bot greeting
            UART_sendString("***********************************************\r\n");
            UART_sendString("\r\n"); // Use this to make a new space

            while (playing) // infinite loop
            { // while 1 start

                chooseMode(); // prompt user for whether they want to play with nouns or verbs

                while (playing) // infinite loop while waiting for the switches to be pressed
                { // while 1 start

                    // PLAYING WITH NOUNS
                    if ((S1) == 0) // if switch 1 is pressed, then enter this conditional
                    {
                        for (i = 0; i < 20000; i++); // debounce

                        nounLength(); // chose to play with nouns, find length of said nouns

                        while (playing)
                        {
                            // PLAYING WITH 3-6 LENGTH WORDS
                            if ((S1) == 0)
                            {
                                for (i = 0; i < 20000; i++); // debounce

                                word36(); // chose 3-6 length words, now choose difficulty

                                while (playing) // infinite loop after difficulty choice
                                {
                                    if ((S1) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        easyDiff(); // chosen easy difficulty, now play the game
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(easy_nouns_3_6)/sizeof(easy_nouns_3_6[0]);
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = easy_nouns_3_6[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }


                                    }
                                    else if ((S2) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        hardDiff(); // chosen difficulty, now freaking play the game
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(hard_nouns_3_6)/sizeof(hard_nouns_3_6[0]);
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = hard_nouns_3_6[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }

                                }
                            }
                            // PLAYING WITH 7+ LENGTH WORDS
                            else if ((S2) == 0)
                            {
                                for (i = 0; i < 20000; i++); // debounce

                                plus7(); // chosen 7+, now choose difficulty

                                while (playing) // infinite loop after difficulty choice
                                {
                                    if ((S1) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        easyDiff(); // oh my gosh play the game
                                        rules(); // print rules of the game



                                        while (playing)
                                        {
                                            int numWords = sizeof(easy_nouns_7_plus)/sizeof(easy_nouns_7_plus[0]); // set up random parameter
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = easy_nouns_7_plus[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }
                                    else if ((S2) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        hardDiff(); // play. the. game.
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(hard_nouns_7_plus)/sizeof(hard_nouns_7_plus[0]);
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = hard_nouns_7_plus[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }

                                }
                            }
                        }

                    }
                    // PLAYING WITH VERBS
                    else if ((S2) == 0)
                    {
                        for (i = 0; i < 20000; i++); // debounce

                        verbLength(); // chose verbs, now choose length of said verbs

                        while (playing)
                        {
                            // PLAYING WITH 3-6 LENGTH WORDS
                            if ((S1) == 0)
                            {
                                for (i = 0; i < 20000; i++); // debounce

                                word36(); // chose 3-6 length words, now choose difficulty

                                while (playing) // infinite loop after difficulty choice
                                {
                                    if ((S1) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        easyDiff(); // chosen difficulty, now play the game
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(easy_verbs_3_6)/sizeof(easy_verbs_3_6[0]);
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = easy_verbs_3_6[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }
                                    else if ((S2) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        hardDiff(); // chosen difficulty, now play the game
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(hard_verbs_3_6)/sizeof(hard_verbs_3_6[0]); // set up parameter for finding random word
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = hard_verbs_3_6[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }

                                }
                            }
                            // PLAYING WITH 7+ LENGTH WORDS
                            else if ((S2) == 0)
                            {
                                for (i = 0; i < 20000; i++); // debounce

                                plus7(); // chosen 7+, now choose difficulty

                                while (playing) // infinite loop after difficulty choice
                                {
                                    if ((S1) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        easyDiff(); // chosen difficulty, now play the game
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(easy_verbs_7_plus)/sizeof(easy_verbs_7_plus[0]);
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = easy_verbs_7_plus[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }
                                    else if ((S2) == 0)
                                    {
                                        for (i = 0; i < 20000; i++); // debounce

                                        hardDiff(); // chosen difficulty, now play the game
                                        rules(); // print rules of the game

                                        while (playing)
                                        {
                                            int numWords = sizeof(hard_verbs_7_plus)/sizeof(hard_verbs_7_plus[0]);
                                            int index = rand() % numWords; // get the random index

                                            const char* randomWord = hard_verbs_7_plus[index]; // get the random word
                                            playHangman(randomWord); // play hangman with this new random word

                                            UART_sendString("\r\nThank you for playing! Would you like to play again? (y/n)\r\n");
                                            UART_getLine(userInput, 100, "User"); // output the name of the user
                                            if (userInput[0] == 'n')
                                            {
                                                playing = 0;
                                            }
                                            UART_sendString("\r\n");
                                        }
                                    }

                                }
                            }
                        }
                    }
                } // while 1 end
            } // while 1 end
        } // if sw1 pressed
        else if ((S2) == 0) // if switch 2 is pressed, then enter this here conditional
        { // if sw2 pressed
            for (i = 0; i < 20000; i++); // debounce
            UART_sendString("Awww man\r\n"); // creeper, awww man
            break; // break out of this because im literally just testing it right now dude
        } // if sw2 pressed
    }

    UART_sendString("\r\nHave a great time!\r\n");
}

