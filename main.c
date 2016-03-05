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
#include "server.h"
#include "accel.h"

uint16_t count=0;
uint8_t ledLoad = 0;
uint8_t state=1;
uint8_t button_pressed = 0;
uint8_t accel_flag=0;
uint32_t pot=0;
int toSend = 0;
ping_t ping;
ping_t *returnedPing;

update_request_t sensorData;
update_request_t accelY;
update_response_t *returnedData;


static int SYSTICK_enable(void);

static volatile uint32_t tx_timer_expired = 0;
static volatile uint32_t rx_timer_expired = 0;
#define RX_PACKET_SIZE (256)
volatile uint8_t rx_packet[RX_PACKET_SIZE];
volatile uint16_t rx_packet_index = 0;


/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	/* happens at 10Hz */
	count+=1;
	accel_flag=1;

	// every half-second
	if (count%5==0){
		LED_toggle(LED_GREEN);
	}

	// transmit at 5Hz!
	if (count%2==0){
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
	button_pressed = 1;	// THE ONLY LINE THAT MATTERS LOL
	while(b < 200000){
		if ((GPIOA->IDR & 0x01) == 0){b += 1;}
		else{b = 0;}
	}
	__asm ("cpsie i");
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
	accel_init();
	servoInit();
	ping.type = TYPE_PING;
	ping.id = 14;
	sensorData.type = TYPE_UPDATE;
	sensorData.id = 1;
	accelY.type = TYPE_UPDATE;
	accelY.id = 3;
	/* Accel settup */
	accel_write(0x20, 0x67); // 100Hz data update rate, x/y/z enabled
	accel_write(0x24, 0xC0); // anti-aliasing filter bandwidth of 50Hz

	uint8_t MSB,LSB;
	int16_t xg,yg,zg;

	init_wifly();	// enter command mode
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
				returnedPing = returnPing(rx_packet);
				rx_timer_expired = 0;
			}

		}
		if (state==3){
			LED_update(LED_BLUE_ON);
			LED_update(LED_ORANGE_ON);
			pot = servoScale();
			//servoUpdate(pot);
			sensorData.value = pot;
			 if (accel_flag == 1) {
				pot = servoScale();
				servoUpdate(pot);
				/*
				MSB = accel_read(0x29); // x-axis MSB
				LSB = accel_read(0x28); // x-axis LSB
				xg = (MSB << 8) | (LSB);
				xg = accel_scale(xg);
				*/
				MSB = accel_read(0x2b); // y-axis MSB
				LSB = accel_read(0x2a); // y-axis LSB
				yg = (MSB << 8) | (LSB);
				yg = accel_scale(yg);
				/*
				MSB = accel_read(0x2d); // z-axis MSB
				LSB = accel_read(0x2c); // z-axis LSB
				zg = (MSB << 8) | (LSB);
				zg = accel_scale(zg);
				*/
				accel_flag = 0;
			 }
			if (tx_timer_expired == 1) {
				toSend = (pot <<16);
				toSend += (yg);
				sensorData.value = toSend;
				sendData(sensorData);
				tx_timer_expired = 0;
				rx_packet_index = 0;	// buffer transmitted, reset index
			}
			else if (rx_timer_expired == 1) {
				 returnedData = returnData(rx_packet);
				 printINT((*returnedData).values[1]>>16);
				 USART2_send(32);
				 printINT(((*returnedData).values[1] & 0xFFFF));
				 USART2_send(10);
				 USART2_send(13);
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

