/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */

/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	LED_toggle(LED_GREEN);
}

void __attribute__ ((interrupt)) USART2_handler(void){
	USART2_recv();
	USART2_send();
}

void __attribute__ ((interrupt)) EXTI0_handler(void){
	__asm ("cpsid i \n");
	unsigned int b = 0;
	button_clear();		/* clear pending exti0 interrput */
	button_disable();
	__asm ("mov r0, #88 \n");
	USART2_send();
	while(b < 200000){
		unsigned int a = GPIOA->IDR & GPIOA_0;
		if (a == 0){
			b += 1;
		}
		else{
			b = 0;
		}
	}
	button_clear();
	__asm ("cpsie i");
	button_init();

}

int main()
{
	/*
	 * Demonstrate use of LED.h/LED.c - modifies hardware registers using C
	 */
	LED_init();
	LED_update(LED_BLUE_ON);
	LED_update(LED_RED_ON );
	LED_update(LED_GREEN_ON );
	LED_update(LED_ORANGE_ON );
	LED_update(LED_BLUE_OFF|LED_RED_OFF|LED_GREEN_OFF|LED_ORANGE_OFF);

	LED_toggle(LED_ORANGE);
	LED_toggle(LED_ORANGE);

	/*
	 * Demonstrate use of in-line assembly - enable interrupts
	 */
	__asm ("  cpsie i \n" );

	SYSTICK_enable();
	USART2_init();
	button_init();

	sendHEX(3551376191);		/* send D3ADB33F */
	/* Wait here forever */
	while(1);

	/* We'll never reach this line */
	return 0;
}

int SYSTICK_enable(void){
	/* Turn on SYSTICK */
	SYSTICK->LOAD = STK_LOAD_sec;
	SYSTICK->CTRL |= SYSTICK->CTRL | STK_CTRL_init;

	return 0;
}
