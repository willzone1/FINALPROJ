/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "usart2.h"
#include "usart3.h"
#include "button.h"

uint16_t count=0;
uint8_t ledLoad = 0;
uint8_t state=1;
uint8_t toPing = 0;
uint8_t button_pressed = 0;
uint32_t pot=0;
uint32_t buffC = 0;
char buffer[132];

static int SYSTICK_enable(void);
void sendPing(ping_t ping);
unsigned char asciiIntForHex(unsigned char hex);



/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	/* happens at 10Hz */
	count+=1;

	if (count%5==0){LED_toggle(LED_GREEN);}
	if (count%10==0){toPing = 1;}
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
	unsigned char toSend = asciiIntForHex(input);
	USART2_send(toSend);
	//buffer[buffC] = input;
}

void __attribute__ ((interrupt)) EXTI0_handler(void){

	button_clear();		/* clear pending EXTI0 interrupt */
	button_pressed = 1;
	switch(state) {
	 case 1:
		 USART3_send(10);
		 USART3_send('E');
		 USART3_send('X');
		 USART3_send('I');
		 USART3_send('T');
		 USART3_send(10);
		 state = 2;
		break;
	 case 2:
		state = 3;
		break;
	 case 3:
		state = 1;
		init_wifly();
		break;
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
	USART3_init();	//wi-fly module
	init_wifly();
	button_init();

	while(1){
		if (button_pressed) { button_pressed = 0; }
		while(state==1){
			LED_update(LED_BLUE_ON);
			LED_update(LED_ORANGE_OFF);
		}
		while (state==2){
			LED_update(LED_BLUE_OFF);
			LED_update(LED_ORANGE_ON);
			ping_t ping;
			ping.type = TYPE_PING;
			ping.id = 14;
			if(toPing == 1){
				sendPing(ping);
				toPing = 0;
			}

		}
		while (state==3){
			LED_update(LED_BLUE_ON);
			LED_update(LED_ORANGE_ON);
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

void sendPing(ping_t ping){
	char* byte = (char*)&ping;
	for (int i=0; i<sizeof(ping); i++){
		USART3_send(byte[i]);
	}
}

void getData(char data){
	char* byteIn = (char*)&data;
	for (int i=0; i<sizeof(data); i++){
		USART2_send(byteIn[i]);
	}
}

unsigned char asciiIntForHex(unsigned char hex)
{
   if (hex < 10) {
         return hex + 48;
   } else {
         return hex + 55;
   }
}


