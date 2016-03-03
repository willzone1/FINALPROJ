/*
 * USART3.c
 *
 *  Created on: Mar 1, 2016
 *      Author: don
 */

#include "stdint.h"
#include "stm32f4xx.h"
#include "USART2.h"
#include "print.h"


void USART3_init() {
   RCC->AHB1ENR |= 0x8;
   RCC->APB1ENR  |= 0x40000; // bit 18 - clock enable USART3
   // Now go to GPIOD so we can control the USART3
   // Configure GPIOD Pin 8 as as AF7, push-pull, no pull-up/pull-down, 2MHz drive speed
   GPIOD->MODER  |= 0xA0000; // bit 17, 19 - AF mode for MODER8/9 (ref man 198)
   GPIOD->AFRH   |= 0x77;    //0b 0111 0111 for AF7 on pin 8 and 9
   USART3->CR1   |= 0x2020;  // enable the USART peripheral
   USART3->BRR    = 1667;    // 9600 baud
   NVIC->ISER1   |= 0xC0;    // enables position 38,39 in the EXTI table. (ref man p249, prog man p205)
   USART3->CR1   |= 0xC;     // Enable transmitter and receiver
}

void USART3_send(unsigned char input) {
   // wait for USART transmit shift register to be empty
   while((USART3->SR & 0x80) == 0); // busywait for Tx available
   // load byte to send into the transmit shift register
   USART3->DR = input;
}

unsigned char USART3_recv() {
   // return the data sitting in USART3's data register
   unsigned char character = USART3->DR;
   return character;
}

void init_wifly() {
	for (int i = 0; i<2000000; i++){}

	//print_str("$$$");
	USART3_send(36);
	for (int i = 0; i<200000; i++){}
	USART3_send(36);
	for (int i = 0; i<200000; i++){}
	USART3_send(36);

	for (int i = 0; i<2000000; i++){}
	// no carriage return after

}

void reboot_wifly() {
	for (int i = 0; i<2000000; i++){}
	USART3_send(10);
	USART3_send(13);
	for (int i = 0; i<200000; i++){}
	print_str("reboot");
	USART3_send(10);
	USART3_send(13);

}
