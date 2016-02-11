/*
 * POTstuff.c
 *
 *  Created on: Feb 10, 2016
 *      Author: WillPer
 */

#include "POTstuff.h"
#include "stm32f4xx.h"

unsigned char intHEX(unsigned int toConvert);

void sendHEX(unsigned int toSend){
	unsigned int chunk1 = (toSend & 0xF0000000)>>28;
	unsigned int chunk2 = (toSend & 0x0F000000)>>24;
	unsigned int chunk3 = (toSend & 0x00F00000)>>20;
	unsigned int chunk4 = (toSend & 0x000F0000)>>16;
	unsigned int chunk5 = (toSend & 0x0000F000)>>12;
	unsigned int chunk6 = (toSend & 0x00000F00)>>8;
	unsigned int chunk7 = (toSend & 0x000000F0)>>4;
	unsigned int chunk8 = (toSend & 0x0000000F);
	unsigned char one = intHEX(chunk1);
	unsigned char two = intHEX(chunk2);
	unsigned char three = intHEX(chunk3);
	unsigned char four = intHEX(chunk4);
	unsigned char five = intHEX(chunk5);
	unsigned char six = intHEX(chunk6);
	unsigned char seven = intHEX(chunk7);
	unsigned char eight = intHEX(chunk8);
	USART2_send(one);
	USART2_send(two);
	USART2_send(three);
	USART2_send(four);
	USART2_send(five);
	USART2_send(six);
	USART2_send(seven);
	USART2_send(eight);
	unsigned char newline = 13;
	USART2_send(newline);


}

unsigned char intHEX(unsigned int toConvert){
	if (toConvert < 10){
		return (toConvert + 48);
	}
	else{
		return (toConvert + 55);
	}
}

void POT_init(){
	RCC->APB2ENR = RCC->APB2ENR | ADC1_clockon;	/* turn ADC clock on */
	RCC->AHB1ENR |= 0x01;
	GPIOA->MODER = GPIOA->MODER | POT_GPIOmask;	/* Set pot pin as analog in */
	ADC1->SQR1 = 0xFF0FFFFF;					/* One conversion */
	ADC1->SQR3 = ADC1->SQR3 | 0x01;	/* 1st conversion is  potentiometer ADC channel (1) */
	ADC1->SMPR2 = 0x08;				/* take 56 cycles for the reading - equation datasheet p.125 */
	ADC1->CR2 = ADC1->CR2 | ADC1_on;			/* turn ADC on */

}

unsigned int POT_sample(){
	ADC1->CR2 = ADC1->CR2 | 0x40000000;	/* start sampling */

	while (1){
		int regi = ADC1->SR;
		int status = (ADC1->SR & 0x02)>>1;
		if (status == 1){
			break;
		}
	}
	return ADC1->DR;
}
