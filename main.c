/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "accel.h"
#include "servo.h"
#include "mutex.h"
#include "usart2.h"
#include "POTstuff.h"
#include "button.h"

unsigned int history[4]={0,0,0,0};
int my_mutex=0;
uint16_t count=0;
uint8_t ledLoad = 0;
uint32_t pot=0;
int accel_flag = 0;
static int SYSTICK_enable(void);


/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	/* happens at 10Hz */
	count+=1;

	if (count%5==0){
		LED_toggle(LED_GREEN);
	}

	if (ledLoad==1){LED_toggle(LED_ORANGE);}
	if (ledLoad==2){LED_toggle(LED_RED);}
	if (ledLoad==3){LED_toggle(LED_BLUE);}
	if (ledLoad==5){ledLoad=0;}
	ledLoad += 1;

	/* read the accelerometer, print everything, etc! */
	accel_flag=1;
}

void __attribute__ ((interrupt)) USART2_handler(void){
	int fail = 0;
	fail = lock_mutex(my_mutex);
	if (fail == 1){
		return;
	}
	unsigned char input = USART2_recv();
	switch(input){
	case 'a':
		break;
	default:break;
	}
	unlock_mutex(my_mutex);
}

void __attribute__ ((interrupt)) EXTI0_handler(void){

	button_clear();		/* clear pending EXTI0 interrupt */

}

static int16_t GRAVscl(int16_t in) {
   float scaleFactor = 1000.0/32767;  // range of accel outputs a signed int16
   float scaledAccel = (float)in * scaleFactor;
   return (int16_t) scaledAccel;
}

void printINT(int16_t toPrint){
	int digit = 10000;
	int print = 0;
	if(toPrint<0){USART2_send(45);}
	else{USART2_send(32);}
	while(digit>0){
		toPrint = abs(toPrint);		//ignore the warning, abs function is in GNU library
		print = (toPrint/digit) % 10;
		print += 48;	/* ascii values start at 48 */
		digit /= 10;
		USART2_send(print);
	}
}

int main()
{
	__asm ("  cpsie i \n" );	//interrupt enable
	/* ENABLERS */
	LED_init();
	LED_update(LED_BLUE_OFF|LED_RED_OFF|LED_GREEN_OFF|LED_ORANGE_OFF);
	SYSTICK_enable();
	USART2_init();
	button_init();
	POT_init();
	servoInit();
	accelInit();

	/* Accel settup */
	accel_write(0x20, 0x67); // 100Hz data update rate, x/y/z enabled
	accel_write(0x24, 0xC0); // anti-aliasing filter bandwidth of 50Hz

	uint8_t MSB,LSB;
	int16_t xg,yg,zg;

	/* Wait here forever */
	while(1){
		pot = servoScale();
		servoUpdate(pot);
		/*USART2_send(accel_read(0x0F));	//read whois register, should read  */
		while(1) {

			 if (accel_flag == 1) {
				pot = servoScale();
				servoUpdate(pot);
				MSB = accel_read(0x29); // x-axis MSB
				LSB = accel_read(0x28); // x-axis LSB
				xg = (MSB << 8) | (LSB);
				xg = GRAVscl(xg);
				MSB = accel_read(0x2b); // y-axis MSB
				LSB = accel_read(0x2a); // y-axis LSB
				yg = (MSB << 8) | (LSB);
				yg = GRAVscl(yg);
				MSB = accel_read(0x2d); // z-axis MSB
				LSB = accel_read(0x2c); // z-axis LSB
				zg = (MSB << 8) | (LSB);
				zg = GRAVscl(zg);


				/* LET'S PRINT */
				printINT(count);
				USART2_send(',');
				USART2_send(' ');

				printINT((uint16_t)pot);
				USART2_send(',');
				USART2_send(' ');

				printINT(xg);
				USART2_send(',');
				USART2_send(' ');

				printINT(yg);
				USART2_send(',');
				USART2_send(' ');

				printINT(zg);
				USART2_send(10);
				USART2_send(13);

				accel_flag = 0;	//we done!
			 }
		}
	}

	/* We'll never reach this line */
	return 0;
}

static int SYSTICK_enable(void){
	/* Turn on SYSTICK -- see programming manual 4.5 */
	SYSTICK->LOAD = STK_LOAD;	//value we count to (currently @16MHz)
	SYSTICK->CTRL |= STK_CTRL_init;

	return 0;
}
