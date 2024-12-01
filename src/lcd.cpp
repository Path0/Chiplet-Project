#include "stm32f0xx.h"
#include "stm32f0xx_hal_spi.h"
#include <stdio.h>

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void ms_wait(int n) {
    for(int x = 0; x <= n; x++) {
        nano_wait(1000000);
    }
}

void init_spi1() {
    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;

    // set ports 15, 7, and 5 to alternate function 0
    GPIOA -> MODER |= GPIO_MODER_MODER15_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER5_1;
    
    // disable spi
    SPI1 -> CR1 &= ~SPI_CR1_SPE;

    // set max divisable baud rate
    SPI1 -> CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

    // set data size to 10 bit
    SPI1 -> CR2 |= SPI_CR2_DS_3 | SPI_CR2_DS_0;
    SPI1 -> CR2 &= ~SPI_CR2_DS_2;
    SPI1 -> CR2 &= ~SPI_CR2_DS_1;



    // master config
    SPI1 -> CR1 |= SPI_CR1_MSTR;

    // enable SSOE, NSSP, DMA Transfer
    SPI1 -> CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_TXDMAEN;

    // enable port
    SPI1 -> CR1 |= SPI_CR1_SPE;
}
void spi_cmd(unsigned int data) {
    while((SPI1 -> SR & (1 << 1)) == 0) {
        
    }
    SPI1-> DR = data;
}

void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
    
}
void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}
void spi1_display1(const char *string) {
    spi_cmd(0x02);
    int index = 0;
    while(*(string + index) != '\0') {
        spi_data(*(string + index));
        index++;
    }

    
}
void spi1_display2(const char *string) {
    spi_cmd(0xc0);
    int index = 0;
    while(*(string + index) != '\0') {
        spi_data(*(string + index));
        index++;
    }
}



void display_string(const char* string) {
    spi_cmd(0x1);
    ms_wait(2);

    // set pointer to home pos
    spi_cmd(0x02);
    int index = 0;

    // stop if null terminator OR if the index is 16, signaling go to next line
    while((*(string + index) != '\0') && (index != 16)) {
        spi_data(*(string + index));
        index++;
    }


    spi_cmd(0xc0);
    ms_wait(1);
    while((*(string + index) != '\0')) {
        spi_data(*(string + index));
        index++;
    }


}

void display_int(int x) {
    char string[16];

    sprintf(string, "%d", x);
    display_string(string);
    
}

void initialize() {
    init_spi1();
    spi1_init_oled();
}