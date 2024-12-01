#include "keypad.h"
#include "lcd.h"

// initializes GPIO
void initc() {
  
  RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;

  // sets pins 4-7 to output
  GPIOC -> MODER |= 0x55 << 8;

  // sets pins 0-3 to have internal pull down resistors
  GPIOC -> PUPDR |= 0xAA << 0;

}


/**
 * Returns a byte containing col,row information.
 * bit pattern is as follows:
 * col_4 | col_3 | col_2 | col_1 | row_4 | row_3 | row_2 | row_1
 * 
 */
uint8_t keypad() {
  int col_4, col_3, col_2, col_1, row_4, row_3, row_2, row_1 = 0x0;

  for(int x = 0; x < 4; x++) {
    // clear all columns
    GPIOC -> BSRR |= 0xF << (4 + 16);
    // set the xth column to 1, starting at col4
    GPIOC -> BSRR |= 1 << (x + 4);

    nano_wait(1000000);

    uint8_t input = (GPIOC -> IDR)& 0xFF;

    if(input & 0b1) {
      row_4 = 1;
    } else {
      row_4 = 0;
    }

    if(input & 0b10) {
      row_3 = 1;
    } else {
      row_3 = 0;
    }

    if(input & 0b100) {
      row_2 = 1;
    } else {
      row_2 = 0;
    }

    if(input & 0b1000) {
      row_1 = 1;
    } else {
      row_1 = 0;
    }

    if(input & 0b10000) {
      col_4 = 1;
    } else {
      col_4 = 0;
    }

    if(input & 0b100000) {
      col_3 = 1;
    } else {
      col_3 = 0;
    }

    if(input & 0b1000000) {
      col_2 = 1;
    } else {
      col_2 = 0;
    }

    if(input & 0b10000000) {
      col_1 = 1;
    } else {
      col_1 = 0;
    }

    //row 4 is gpio0
    // row 3 is 1
    // 2 is 2
    //1 is 3

    //col 4 is gpio4, x=0
    // col3 x=1
    // col2 x=2
    // col1 x=3


    
  }
  return (uint8_t) ((~~col_4 << 7) | (~~col_3 << 6) | (~~col_2 << 5) | (~~col_1 << 4) | (~~row_4 << 3) | (~~row_3 << 2) | (~~row_2 << 1) | (~~row_1 << 0));
}

/*
* calling this makes the keypad listen for a command, and doesn't do anything until a keypress is detected.
*/
char get_key() {
  int cont = 0;
  uint8_t result = 0x0;
  while(~cont) {
    result = keypad();
    if((result & 0xF) != 0) {
      cont = 1;
    }
    // wait around until there's a keypress
  }

  uint8_t r1c1 = 0b10001000;
  uint8_t r1c2 = 0b01001000;
  uint8_t r1c3 = 0b00101000;
  uint8_t r1c4 = 0b00011000;

  uint8_t r2c1 = 0b10000100;
  uint8_t r2c2 = 0b01000100;
  uint8_t r2c3 = 0b00100100;
  uint8_t r2c4 = 0b00010100;

  uint8_t r3c1 = 0b10000010;
  uint8_t r3c2 = 0b01000010;
  uint8_t r3c3 = 0b00100010;
  uint8_t r3c4 = 0b00010010;

  uint8_t r4c1 = 0b10000001;
  uint8_t r4c2 = 0b01000001;
  uint8_t r4c3 = 0b00100001;
  uint8_t r4c4 = 0b00010001;


  if(result == r1c1) return '1';
  else if(result == r1c2) return '2';
  else if(result == r1c3) return '3';
  else if(result == r1c4) return 'A';
  else if(result == r2c1) return '4';
  else if(result == r2c2) return '5';
  else if(result == r2c3) return '6';
  else if(result == r2c4) return 'B';
  else if(result == r3c1) return '7';
  else if(result == r3c2) return '8';
  else if(result == r3c3) return '9';
  else if(result == r3c4) return 'C';
  else if(result == r4c1) return '*';
  else if(result == r4c2) return '0';
  else if(result == r4c3) return '#';
  else if(result == r4c4) return 'D';
  else return 'X';

}



/*
* gets the letter from two chars representing numbers from the keypad.
* valid inputs are from '0','1' to '2', '6' for the two character inputs.
* a - 01
* b - 02
* c - 03
* ...
* z - 26
*/
char letter_from_number(char tens, char ones) {
  for(int tens_digit = 0; tens_digit <= 2; tens_digit++) {

    for(int ones_digit = 0; ones_digit <= 9; ones_digit++) {
      //skip "00" and anything above "26"
      if((tens_digit == 0) && (ones_digit == 0)) continue;
      if((tens_digit == 2) && (ones_digit >= 7)) continue;

      if((tens_digit == (tens - 48) && (ones_digit == (ones - 48)))) {
        int ascii_code = tens_digit + ones_digit;
        return (char) (ascii_code + 97);
      }






    }
    return 'X';
  }
  return 'X';
}



// does the sequence to get player's 3-character name
/*
* fills the empty pointer with 3 chars representing the player's name, takes the next 6 keypad inputs
* and translates them into letters.
*/
void get_player_name(char* empty_3item_pointer) {
  char key1 = get_key();
  char key2 = get_key();

  char letter1 = letter_from_number(key1, key2);

  key1 = get_key();
  key2 = get_key();

  char letter2 = letter_from_number(key1, key2);

  key1 = get_key();
  key2 = get_key();

  char letter3 = letter_from_number(key1, key2);

  *empty_3item_pointer = letter1;
  *(empty_3item_pointer + 1) = letter2;
  *(empty_3item_pointer + 2) = letter3;

  return;
}




