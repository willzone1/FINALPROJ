/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "servo.h"
#include "usart2.h"
#include "usart3.h"
#include "POTstuff.h"
#include "button.h"

uint16_t count=0;
uint8_t ledLoad = 0;
uint32_t pot=0;
static int SYSTICK_enable(void);


/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	/* happens at 10Hz */
	count+=1;

	if (count%5==0){LED_toggle(LED_GREEN);}
	/*
	if (ledLoad==1){LED_toggle(LED_ORANGE);}
	if (ledLoad==2){LED_toggle(LED_RED);}
	if (ledLoad==3){LED_toggle(LED_BLUE);}
	if (ledLoad==5){ledLoad=0;}
	ledLoad += 1;
	*/

}

void __attribute__ ((interrupt)) USART2_handler(void){
	unsigned char input = USART2_recv();
	USART3_send(input);						//relay input to wifi
}

void __attribute__ ((interrupt)) USART3_handler(void){
	unsigned char input = USART3_recv();
	USART2_send(input);
}

void __attribute__ ((interrupt)) EXTI0_handler(void){

	button_clear();		/* clear pending EXTI0 interrupt */

}

int main()
{
	__asm ("  cpsie i \n" );	//interrupt enable
	/* ENABLERS */
	LED_init();
	LED_update(LED_BLUE_OFF|LED_RED_OFF|LED_GREEN_OFF|LED_ORANGE_OFF);
	SYSTICK_enable();
	USART2_init();
	USART3_init();	//wi-fly module
	button_init();
	POT_init();
	servoInit();

	while(1);
	/* We'll never reach this line */
	return 0;
}

static int SYSTICK_enable(void){
	/* Turn on SYSTICK -- see programming manual 4.5 */
	SYSTICK->LOAD = STK_LOAD;	//value we count to (currently @16MHz)
	SYSTICK->CTRL |= STK_CTRL_init;

	return 0;
}
