/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "usart2.h"
#include "usart3.h"
#include "button.h"
#include "POTstuff.h"
#include "servo.h"
#include "print.h"

uint16_t count=0;
uint8_t ledLoad = 0;
uint8_t state=1;
//volatile uint8_t toPing = 0;
uint8_t button_pressed = 0;
uint32_t pot=0;
//update_request_t rx_packet = 0;

ping_t ping;

static int SYSTICK_enable(void);
int shortCount = 0;
void sendPing(ping_t ping);
void sendData(update_request_t ping);
void printData(char data);
unsigned char asciiIntForHex(unsigned char hex);
void printINT(int16_t toPrint);

static volatile uint32_t tx_timer_expired = 0;
static volatile uint32_t rx_timer_expired = 0;
#define RX_PACKET_SIZE (256)
static volatile uint8_t rx_packet[RX_PACKET_SIZE];
static volatile uint16_t rx_packet_index = 0;


/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	/* happens at 10Hz */
	count+=1;

	// every half-second
	if (count%5==0){
		LED_toggle(LED_GREEN);
	}

	// every second
	if (count%10==0){
		//toPing = 1;
		tx_timer_expired = 1;
		rx_timer_expired = 1;
	}
}

void __attribute__ ((interrupt)) USART2_handler(void){
	unsigned char input = USART2_recv();
	USART3_send(input);						//relay input to wifi
}

void __attribute__ ((interrupt)) USART3_handler(void){
	unsigned char byte = USART3_recv();
	// in configuration mode, forward data from wifly to terminal
	if (state == 1) {
		 USART2_send(byte);
	} else if (state == 2) {
		 //print_str("ID: ");
		 //unsigned char toSend = asciiIntForHex(byte);
		 //USART2_send(toSend); // so we can see what's going on.
		 //USART2_send(10);
		 //USART2_send(13);
		if (rx_packet_index < (RX_PACKET_SIZE - 1)) {
			   rx_packet[rx_packet_index] = byte;
			   rx_packet_index++;
		 }
	}
	// otherwise collect RX data in rx_packet buffer if space
	else if (state == 3) {
		 if (rx_packet_index < (RX_PACKET_SIZE - 1)) {
			   rx_packet[rx_packet_index] = byte;
			   rx_packet_index++;
		 }
	}
}

void __attribute__ ((interrupt)) EXTI0_handler(void){

	__asm ("cpsid i \n");
	unsigned int b = 0;
	button_clear();		/* clear pending exti0 interrupt */
	button_pressed = 1;
	while(b < 200000){
		if ((GPIOA->IDR & 0x01) == 0){b += 1;}
		else{b = 0;}
	}
	__asm ("cpsie i");
}


void get_server_response() {
   // serialize rx_packet into the data structure
   update_response_t *update_response;
   update_response = (update_response_t *) rx_packet;
   USART2_send(10);
   USART2_send(13);
   print_str("ID: ");
   printINT(update_response->id);
   USART2_send(10);
   USART2_send(13);
   print_str("Type: ");
   printINT(update_response->type);
   USART2_send(10);
   USART2_send(13);
   print_str("Average: ");
   printINT(update_response->average);
   USART2_send(10);
   USART2_send(13);
   print_str("Values: ");
   USART2_send(10);
   USART2_send(13);
   for (char i = 0; i < 31; i++) {
         printINT(update_response->values[i]);
         USART2_send(32);
   }
   USART2_send(10);
   USART2_send(13);
}

void returnPing() {
   // serialize rx_packet into the data structure
   ping_t *ping;
   ping = (ping_t *) rx_packet;
   USART2_send(10);
   USART2_send(13);
   print_str("ID: ");
   printINT(ping->id);
   USART2_send(10);
   USART2_send(13);

}
int main()
{
	__asm ("  cpsie i \n" );	//interrupt enable
	/* ENABLERS */
	LED_init();
	LED_update(LED_BLUE_OFF|LED_RED_OFF|LED_GREEN_OFF|LED_ORANGE_OFF);
	SYSTICK_enable();
	USART2_init();
	USART3_init();	// wi-fly module
	button_init();
	POT_init();
	servoInit();
	ping.type = TYPE_PING;
	ping.id = 14;

	init_wifly();	// enter command mode
	//reboot_wifly();	// connect to network
	while(1){
		if (button_pressed) {
			switch(state) {
			 case 1:
				exit_wifly();
				state = 2;
				break;
			 case 2:
				rx_packet_index = 0;
				state = 3;
				break;
			 case 3:
				rx_packet_index = 0;
				state = 1;
				init_wifly();
				break;
			}
			button_pressed = 0;
		}
		if(state==1){
			LED_update(LED_BLUE_ON);
			LED_update(LED_ORANGE_OFF);
		}
		if (state==2){
			LED_update(LED_BLUE_OFF);
			LED_update(LED_ORANGE_ON);
			if(tx_timer_expired == 1){
				sendPing(ping);
				tx_timer_expired = 0;
			} else if(rx_timer_expired == 1){
				returnPing();
				rx_timer_expired = 0;
			}

		}
		if (state==3){
			LED_update(LED_BLUE_ON);
			LED_update(LED_ORANGE_ON);
			pot = servoScale();
			servoUpdate(pot);

			update_request_t update;
			update.type = TYPE_UPDATE;
			update.id = 14;
			update.value = pot;

			if (tx_timer_expired == 1) {
				update.value = pot;
				sendData(update);
				tx_timer_expired = 0;
			} else if (rx_timer_expired == 1) {
				 get_server_response();
				 rx_timer_expired = 0;
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

void sendPing(ping_t ping){
	char* byte = (char*)&ping;
	for (int i=0; i<sizeof(ping); i++){
		USART3_send(byte[i]);
	}
}

void sendData(update_request_t data){
	char* byte = (char*)&data;
	for (int i=0; i<sizeof(data); i++){
		USART3_send(byte[i]);
	}
}

void printData(char data){
	char* byte = (char*)&data;
	for (int i=0; i<sizeof(data); i++){
		USART2_send(byte[i]);
	}
}

void printINT(int16_t toPrint){
	int digit = 1000;
	int print = 0;
	if(toPrint<0){USART2_send(45);}
	//else{USART2_send(32);}
	while(digit>0){
		toPrint = abs(toPrint);		//ignore the warning, abs function is in GNU library
		print = (toPrint/digit) % 10;
		print += 48;	/* ascii values start at 48 */
		digit /= 10;
		USART2_send(print);
	}
}


