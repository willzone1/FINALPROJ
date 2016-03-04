/*
 * server.h
 *
 *  Created on: Mar 4, 2016
 *      Author: WillPer
 */

#ifndef SERVER_H_
#define SERVER_H_

void sendPing(ping_t ping);
void sendData(update_request_t data);
ping_t * returnPing(volatile uint8_t rx_packet[]);
update_response_t * returnData(volatile uint8_t rx_packet[]);
void printReturnData(update_response_t update_response);


#endif /* SERVER_H_ */
