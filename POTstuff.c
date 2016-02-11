/*
 * POTstuff.c
 *
 *  Created on: Feb 10, 2016
 *      Author: WillPer
 */

#include "POTstuff.h"

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


}

unsigned char intHEX(unsigned int toConvert){
	if (toConvert < 10){
		return (toConvert + 48);
	}
	else{
		return (toConvert + 55);
	}
}
