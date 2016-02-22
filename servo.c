/*
 * servo.c
 *
 *  Created on: Feb 16, 2016
 *      Author: WillPer
 */

#include "POTstuff.h"
#include "stm32f4xx.h"


void servoInit(void){
	/* configure port e pin 9 for tim1_ch1 */
	RCC->AHB1ENR |= 0x10;		/* enable GPIOE clock */
	RCC->APB2ENR |= 0x01;		/* enable TIM1 clock */
	GPIOE->MODER |= 0x80000;	/* Set pin 9 as alternate function */
	GPIOE->AFRH	 |= 0x10;		/* set gpioe pin 9 as af1 */
	TIM1->CCMR1	 = 0x60;		/* OCM1 - PWM mode 1 */

	TIM1->PSC	 = 15;			/* 319999 */
	TIM1->ARR	 = 20000;		/* 20,000 */

	TIM1->CCR1 	 = 1500;		/* configure 7.5% duty cycle */
	TIM1->CCER	 |=0x01;		/* CC1 output enable */

	TIM1->BDTR 	 |= 0x80000;	/*enable all outputs */
	TIM1->CR1	 |= 0x01;		/* enable counter CEN bit set */

	//CPACR->REG = 0xF00000;



}
