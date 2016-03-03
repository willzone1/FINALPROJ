/*
 * USART3.c
 *
 *  Created on: Mar 2, 2016
 *      Author: WillPer
 */

#include "stm32f4xx.h"
#include "stdint.h"
#include "USART3.h"

/* configure GPIOD:
 * pin 8 tx
 * pin 9 rx
 */
void USART3_init(void){
	RCC->AHB1ENR |= 0x8;		// enable GPIOD clock
	RCC->APB1ENR |= 0x40000;		// enable USART3 clock
	GPIOD->MODER |= 0xA0000;	// set port d pins 8 & 9 as alternate function
	GPIOD->AFRH  |= 0x77;		// set pins 8 & 9 as AF7
	USART3->CR1  |= 0x2020;	// enable USART 3
	USART3->BRR  = 1667;		// set baud rate to 9600
	NVIC->ISER1	 |= 0xC0;		// tell NVIC to pass USART 2 & 3 interrupts to processor
	USART3->CR1	 |= 0xC;	// enable transmitter & receiver
}

void USART3_send(unsigned char input){
	while ((USART3->SR & 0x80) == 0){/*wait*/}
	input = USART3->DR;
	return;
}

unsigned char USART3_recv(void){
	while ((USART3->SR & 0x10) == 0){/*wait*/}
	return USART3->DR;
}

