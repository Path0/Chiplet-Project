#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#include "lcd.h"
#include "keypad.h"
#include <stdio.h>
#include "Packet.h"

void internal_clock();

void bin_to_str(char* c, uint32_t word);

int main() {

    internal_clock();

    // initialize spi display
    initialize();

    // packet demo, these are the packet variables with dummy data
    // hover over (in VS code only i guess) any fields or methods in Packet and it'll give you details, if not it's in Packet.h
    // if you have the lcd display and an STM32, hook it into GPIOA (see lcd.cpp for wiring) and watch the demo
    unsigned int length = 6;
    uint32_t data[128] = {0x123, 0x456, 0x789, 0xFFF, 0xABCDEF01, 0x23456789};
    unsigned int destination = 0x4;
    unsigned int requester = 0x1;
    unsigned int format = 0x0; // long read
    uint32_t addr = 0b01000101;


    // creates packet object
    Packet p(length, addr, destination, requester, format, data);


    // fields are public
    p.first_be = 2;
    p.last_be = 3;

    // constructs the packet bitstream
    p.construct_packet();


    // char buffer for the display
    char word[33];

    // prints the packet bitstream word by word on the display
    unsigned int packetLength = ((format == 0x0) || (format == 0x1)) ? 2 + length : 1 + length;
    for(unsigned int x = 0; x < packetLength; x++) {
        sprintf(word, "0x%08lx", *(p.packet_bitstream + x));
        display_string(word);
        spi1_display2("raw bitstream ");
        ms_wait(2 * 1000);
    }



    display_string("copied data");
    ms_wait(2000);

    // "recieves" packet data from p's bitstream
    Packet rec(p.packet_bitstream);

    // displays each field it gathered from the bitstream on the display

    // length of data
    display_int(rec.length);
    spi1_display2("Length");
    ms_wait(2000);

    // address
    display_int(rec.address);
    spi1_display2("Address");
    ms_wait(2000);

    // dest
    display_int(rec.destination);
    spi1_display2("Destination");
    ms_wait(2000);

    // first byte enable
    display_int(rec.first_be);
    spi1_display2("first be");
    ms_wait(2000);  


    // last byte enable
    display_int(rec.last_be);
    spi1_display2("last be");
    ms_wait(2000);

    // displays data array
    
    for(unsigned int x = 0; x < rec.length; x++) {
        sprintf(word, "0x%08lx", data[x]);
        display_string(word);
        spi1_display2("data array");
        ms_wait(2000);
    }



    // //  lil clock demo
    // for(int hours = 0; hours <= 12; hours++) {
    //     for(int mins = 0; mins < 60; mins++) {
    //         for(int secs = 0; secs < 60; secs++) {
    //             char string[9];
    //             sprintf(string, "%02d:%02d:%02d", hours, mins, secs);
    //             display_string(string);
    //             ms_wait(998);
    //         }
    //     }

    // }



    

    
    return 0;
}


void internal_clock()
{
    /* Disable HSE to allow use of the GPIOs */
    RCC->CR &= ~RCC_CR_HSEON;
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;
    /* PLL configuration = (HSI/2) * 12 = ~48 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1 | RCC_CFGR_PLLMUL12);
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0);
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL);
}


// turns a uint32_t into a string of its 32 bits (for lcd)
void bin_to_str(char* c, uint32_t word) {
    for(int x = 0; x < 32; x++) {
        if(word & 1) {
            *(c + 31 - x) = '1';
        } else {
            *(c + 31 - x) = '0';
        }
        word >>= 1;
    }
}