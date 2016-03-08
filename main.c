/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "usart3.h"
#include "button.h"
#include "POTstuff.h"
#include "servo.h"
#include "server.h"
#include "accel.h"

uint16_t count=0;
uint8_t ledLoad = 0;
uint8_t state=3;
uint8_t button_pressed = 1;
uint8_t accel_flag=0;
uint8_t accelIdx = 0;
uint32_t pot=0;
int toSend = 0;
ping_t ping;
ping_t *returnedPing;
static volatile uint32_t tx_timer_expired = 0;
static volatile uint32_t rx_timer_expired = 0;
#define RX_PACKET_SIZE (256)
volatile uint8_t rx_packet[RX_PACKET_SIZE];
volatile uint16_t rx_packet_index = 0;
int accelVals[4];

update_request_t sensorData;
update_response_t *returnedData;

/*
 * SET CONTROLLER ID HERE
 */
uint8_t ID = 0;	//0,1,2



static int SYSTICK_enable(void);




/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	/* happens at 200Hz */
	count+=1;


	// every half-second
	if (count%1000==0){
		LED_toggle(LED_GREEN);
	}

	// sample accel at 100 Hz
	if (count%20==0){
		accel_flag=1;
	}

	// 80 is transmit at 25Hz!
	if (count%80==0){
		tx_timer_expired = 1;
		rx_timer_expired = 1;
	}
}
/*
void __attribute__ ((interrupt)) USART2_handler(void){
	unsigned char input = USART2_recv();
	USART3_send(input);						//relay input to wifi
}
*/
void __attribute__ ((interrupt)) USART3_handler(void){
	unsigned char byte = USART3_recv();
	// in configuration mode, forward data from wifly to terminal
	 if (state == 2) {
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

	//__asm ("cpsid i \n");
	if(state != 3){
		unsigned int b = 0;
		button_clear();		/* clear pending exti0 interrupt */
		if (state != 3){
			button_pressed = 1;	// THE ONLY LINE THAT MATTERS LOL
		}
		while(b < 4000){
			if ((GPIOA->IDR & 0x01) == 0){b += 1;}
			else{b = 0;}
		}
	}
	//__asm ("cpsie i \n");
}


int main()
{
	__asm ("  cpsie i \n" );	//interrupt enable
	/* ENABLERS */
	LED_init();
	LED_update(LED_BLUE_OFF|LED_RED_OFF|LED_GREEN_OFF|LED_ORANGE_OFF);
	SYSTICK_enable();
	USART3_init();	// wi-fly module
	button_init();
	accel_init();
	servoInit();
	ping.type = TYPE_PING;
	sensorData.type = TYPE_UPDATE;

	switch (ID){
	case 0:
		ping.id = 0;
		sensorData.id = 0;
		POT_init();
		break;
	case 1:
		ping.id = 1;
		sensorData.id = 1;
		POT_init();
		break;
	case 2:
		ping.id = 2;
		sensorData.id = 2;
		break;
	}


	/* Accel settup */
	accel_write(0x20, 0x67); // 100Hz data update rate, x/y/z enabled
	accel_write(0x24, 0xC0); // anti-aliasing filter bandwidth of 50Hz

	uint8_t MSB,LSB=0;
	int16_t xg,yg,zg=0;
	int16_t sum=0;
	int16_t average=0;

	//init_wifly();	// enter command mode
	while(1){

		if (button_pressed) {
			switch(state) {
			 case 1:
				exit_wifly();
				LED_update(LED_BLUE_OFF);
				LED_update(LED_ORANGE_ON);
				state = 2;
				break;
			 case 2:
				rx_packet_index = 0;
				LED_update(LED_BLUE_ON);
				LED_update(LED_ORANGE_ON);
				state = 3;
				break;
			 case 3:
				//rx_packet_index = 0;
				//state = 1;
				//init_wifly();
				break;
			}
			button_pressed = 0;
		}
		if(state==1){	// command mode
			LED_update(LED_BLUE_ON);
			LED_update(LED_ORANGE_OFF);
		}
		if (state==2){	// ping mode
			if(tx_timer_expired == 1){
				sendPing(ping);
				tx_timer_expired = 0;
			} else if(rx_timer_expired == 1){
				returnPing(rx_packet);
				rx_timer_expired = 0;
			}
		}
		if (state==3){
			 if (accel_flag == 1) {
				switch (ID){
				case 0:
					pot = servoScale();
					break;
				case 1:
					pot = servoScale();
					break;
				}

				MSB = accel_read(0x2b); // y-axis MSB
				LSB = accel_read(0x2a); // y-axis LSB
				yg = (MSB << 8) | (LSB);
				yg = accel_scale(yg);
				accel_flag = 0;
			 }
			/*
			* ** high = shifted up 16 bits, low = bottom value
			* ID 0 = wrist (high), control grip (low)
			* ID 1 = rotational base (high), shoulder (low)
			* ID 2 = elbow (low)
			*/
			if (tx_timer_expired == 1) {
				switch (ID){
				case 0:
					toSend = (yg <<16);
					toSend += pot;
					sensorData.value = toSend;
					break;
				case 1:
					toSend = (pot <<16);
					toSend += yg;
					sensorData.value = toSend;
					break;
				case 2:
					sensorData.value = yg;
					break;
				}

				sendData(sensorData);
				tx_timer_expired = 0;
				rx_packet_index = 0;	// buffer transmitted, reset index
				sum = 0;
			}
			else if (rx_timer_expired == 1) {
				 returnedData = returnData(rx_packet);
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

