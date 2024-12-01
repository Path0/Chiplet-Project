#include "stm32f0xx.h"

// initialize GPIO port
void initc();

/**
 * Returns a byte containing col,row information.
 * bit pattern is as follows:
 * col_4 | col_3 | col_2 | col_1 | row_4 | row_3 | row_2 | row_1
 * 
 */
uint8_t keypad();



/*
* calling this makes the keypad listen for a command, and doesn't do anything until a keypress is detected.
*/
char get_key();

/*
* gets the letter from two chars representing numbers from the keypad.
* valid inputs are from '0','1' to '2', '6' for the two character inputs.
* a - 01
* b - 02
* c - 03
* ...
* z - 26
*/
char letter_from_number(char tens, char ones);


// does the sequence to get player's 3-character name
/*
* fills the empty pointer with 3 chars representing the player's name, takes the next 6 keypad inputs
* and translates them into letters.
*/
void get_player_name(char* empty_3item_pointer);


