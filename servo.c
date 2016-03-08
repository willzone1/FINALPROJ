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
	//RCC->AHB1ENR |= 0x10;		/* enable GPIOE clock */
	//RCC->APB2ENR |= 0x01;		/* enable TIM1 clock */
	//GPIOE->MODER |= 0x80000;	/* Set pin 9 as alternate function */
	//GPIOE->AFRH	 |= 0x10;		/* set GPIOE pin 9 as AF1 */
	//TIM1->CCMR1	 = 0x60;		/* OCM1 - PWM mode 1 */

	//TIM1->PSC	 = 15;			/* Timer prescaler -1 */
	//TIM1->ARR	 = 20000;		/* 20,000 */

	//TIM1->CCR1 	 = 1500;		/* configure 7.5% duty cycle */
	//TIM1->CCER	 |=0x01;		/* CC1 output enable */

	//TIM1->BDTR 	 |= 0x8000;	/*enable all outputs */
	//TIM1->CR1	 |= 0x01;		/* enable counter; CEN bit set */

	CPACR->REG = 0xF00000;
}

void servoUpdate(uint32_t us){
	/* us ranges from 1 to 2 (ms) for duty cycles of 5-10% */
	/* CCR1 = 1000 @ 5% */
	/* CCR1 = 2000 @10% */
	/* CCR1 is in us too though, so straight pass-thru */
	/* hard servo LIMITS */
	if (us<1000){us=1000;}
	if (us>2000){us=2000;}
	TIM1->CCR1 = (us);
}

uint32_t servoScale(){
	int pot = POT_sample();
	pot = pot * 1000;
	pot = pot / 4095; //4094 is max value observed
	pot += 1000;
	return pot;
}
