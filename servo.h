/*
 * servo.h
 *
 *  Created on: Feb 16, 2016
 *      Author: WillPer
 */

#ifndef SERVO_H_
#define SERVO_H_


void servoInit(void);
void servoUpdate(uint32_t us);
unsigned int servoScale();

#endif /* SERVO_H_ */
