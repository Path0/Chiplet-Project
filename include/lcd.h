#include "stm32f0xx.h"

void nano_wait(unsigned int n);

void ms_wait(int n);

void init_spi1();

void spi_cmd(unsigned int data);

void spi_data(unsigned int data);

void spi1_init_oled();

void spi1_display1(const char* string);

void spi1_display2(const char* string);

void display_string(const char* string);

void display_int(int x);

void initialize();