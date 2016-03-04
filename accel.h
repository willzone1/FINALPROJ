#pragma once
/*
 * accel.h
 *
 *  Created on: Feb 22, 2016
 *      Author: WillPer
 */

#ifndef ACCEL_H_
#define ACCEL_H_

void accel_write(uint8_t accel_reg, uint8_t data);
unsigned short accel_read(uint8_t accel_reg);
void accel_cs_low();
void accel_cs_high();
void accelInit(void);




#endif /* ACCEL_H_ */
