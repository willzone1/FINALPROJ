/*
 * print.h
 *
 *  Created on: Mar 3, 2016
 *      Author: WillPer
 */

#ifndef PRINT_H_
#define PRINT_H_


unsigned char asciiIntForHex(unsigned char hex);
void print_32bit_value(unsigned int input);
void print_16bit_value(int16_t input);
void print_32bit_dec(uint32_t input);
void print_register(uint32_t input);
void print_str(char s[]);


#endif /* PRINT_H_ */
