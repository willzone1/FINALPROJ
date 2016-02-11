/*
 * POTstuff.h
 *
 *  Created on: Feb 10, 2016
 *      Author: WillPer
 */
#pragma once

#ifndef POTSTUFF_H_
#define POTSTUFF_H_


void sendHEX(unsigned int);
unsigned char intHEX(unsigned int);
void POT_init(void);
unsigned int POT_sample(void);


#endif /* POTSTUFF_H_ */
