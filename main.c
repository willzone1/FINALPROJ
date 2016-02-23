/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */

unsigned int history[4]={0,0,0,0};
int my_mutex=0;

/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	LED_toggle(LED_GREEN);
}

void __attribute__ ((interrupt)) USART2_handler(void){
	int fail = 0;
	fail = lock_mutex(my_mutex);
	if (fail == 1){
		return;
	}
	unsigned char input = USART2_recv();
	int i=0;
	unsigned int sum = 0;
	unsigned int average = 0;
	switch(input){
	case 'a':
		/* print average of 4 history values */
		sum = (history[0]+history[1]+history[2]+history[3]);
		average = (unsigned int)sum/4;
		USART2_send(10);
		sendHEX(average);
		USART2_send(10);
		break;
	case 'h':
		/* print history values (don't re-sample) */
		USART2_send(10);
		while (i<=3){
			sendHEX(history[i]);
			i++;
		}
		USART2_send(10);
		break;
	case 'c':
		/* clear history values */
		history[0]=0;
		history[1]=0;
		history[2]=0;
		history[3]=0;
		USART2_send(10);
		break;
	default:break;
	}
	unlock_mutex(my_mutex);
//	USART2_send();
}

void __attribute__ ((interrupt)) EXTI0_handler(void){
	__asm ("cpsid i \n");
	unsigned int b = 0;
	unsigned int latest = 0;
	button_clear();		/* clear pending EXTI0 interrupt */
	button_disable();
//	__asm ("mov r0, #88 \n");
//	USART2_send();
	int j=0;
	latest = POT_sample();
	history[0] = history[1];
	history[1] = history[2];
	history[2] = history[3];
	history[3] = latest;
	while (j<=3){
		sendHEX(history[j]);
		j++;
	}
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
	POT_init();
	servoInit();

	sendHEX(3551376191);		/* send D3ADB33F */




	/* Wait here forever */
	while(1){
		servoScale();	/* update pot/servo */

	}

	/* We'll never reach this line */
	return 0;
}

int SYSTICK_enable(void){
	/* Turn on SYSTICK */
	SYSTICK->LOAD = STK_LOAD_sec;
	SYSTICK->CTRL |= SYSTICK->CTRL | STK_CTRL_init;

	return 0;
}
