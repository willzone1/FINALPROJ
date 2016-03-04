/*
 * server.c
 *
 *  Created on: Mar 4, 2016
 *      Author: WillPer
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

/*
 * Ping server (via wifly)
 */
void sendPing(ping_t ping){
	char* byte = (char*)&ping;
	for (int i=0; i<sizeof(ping); i++){
		USART3_send(byte[i]);
	}
}

/*
 * Send data to server (via wifly)
 */
void sendData(update_request_t data){
	char* byte = (char*)&data;
	for (int i=0; i<sizeof(data); i++){
		USART3_send(byte[i]);
	}
}

/*
 * Read return ping from server (via wifly)
 */
ping_t* returnPing(volatile uint8_t rx_packet[]) {
   // serialize rx_packet into the data structure
   ping_t *ping;
   ping = (ping_t *) rx_packet;
   // send the ping response to the terminal
   USART2_send(10);
   USART2_send(13);
   print_str("ID: ");
   printINT(ping->id);
   USART2_send(10);
   USART2_send(13);
   return ping;

}

/*
 * Read return data from server (via wifly)
 */
update_response_t * returnData(volatile uint8_t rx_packet[]) {
   // serialize rx_packet into the data structure
   update_response_t *update_response;
   update_response = (update_response_t *) rx_packet;
   //printReturnData(update_response);
   return update_response;
}

/*
 * Optional send raw return data to terminal
 */
void printReturnData(update_response_t update_response){

	   USART2_send(10);
	   USART2_send(13);
	   print_str("ID: ");
	   printINT(update_response.id);
	   USART2_send(10);
	   USART2_send(13);
	   print_str("Type: ");
	   printINT(update_response.type);
	   USART2_send(10);
	   USART2_send(13);
	   print_str("Average: ");
	   printINT(update_response.average);
	   USART2_send(10);
	   USART2_send(13);
	   print_str("Values: ");
	   USART2_send(10);
	   USART2_send(13);
	   for (char i = 0; i < 31; i++) {
	         printINT(update_response.values[i]);
	         USART2_send(32);
	   }
	   USART2_send(10);
	   USART2_send(13);

}
