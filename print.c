/*
 * print.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "USART2.h"
#include "button.h"

unsigned char asciiIntForHex(unsigned char hex)
{
   if (hex < 10) {
         return hex + 48;
   } else {
         return hex + 55;
   }
}

void print_32bit_value(unsigned int input)
{
   unsigned char remaining = 32;
   unsigned int mask = 0xF0000000;
   for (unsigned char i = 0; i < 8; i++) {
         // isolate left-most four bits
         unsigned char fourBits = (input & mask)>>(remaining - 4);
         // get ascii-equivalent decimal number for the four bits
         unsigned char hexToPrint = asciiIntForHex(fourBits);
         USART2_send(hexToPrint);
         remaining = remaining - 4;
         mask = mask>>4;
   }
   USART2_send(10);
   USART2_send(13);
}

/**
 * Print 16-bit value in decimal format
 * @param input to print
 */
void print_16bit_value(int16_t input) {
   int mod = 10000;
   if (input < 0) {
         USART2_send('-');
         input = input * -1;
         mod = mod / 10;
   }
   while(mod>0) {
         int16_t out = input / mod;
         // print out
         USART2_send(out + 48); // to decimal
         input = input % mod;
         mod = mod / 10;
   }
}

void print_32bit_dec(uint32_t input) {
   int mod = 1000000000;
   while(mod>0) {
         int16_t out = input / mod;
         // print out
         USART2_send(out + 48); // to decimal
         input = input % mod;
         mod = mod / 10;
   }
}

void print_register(uint32_t input) {
   int mod = 1000000000;
   while(mod>0) {
         uint32_t out = input / mod;
         // print out
         USART2_send(out + 48); // to decimal
         input = input % mod;
         mod = mod / 10;
   }
}

void print_str(char s[]) {
   char i = 0;
   while(s[i] != 0) {
         USART2_send(s[i]);
         i++;
   }
}

void print_str_dl(char s[]) {
   char i = 0;
   while(s[i] != 0) {
         USART2_send(s[i]);
         i++;
         for (int i = 0; i<200000; i++){}
   }
}

void printINT(int16_t toPrint){
	int digit = 1000;
	int print = 0;
	if(toPrint<0){USART2_send(45);}
	//else{USART2_send(32);}
	while(digit>0){
		toPrint = abs(toPrint);		//ignore the warning, abs function is in GNU library
		print = (toPrint/digit) % 10;
		print += 48;	/* ascii values start at 48 */
		digit /= 10;
		USART2_send(print);
	}
}
