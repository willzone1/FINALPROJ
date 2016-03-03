/*
 * USART3.h
 *
 *  Created on: Mar 2, 2016
 *      Author: WillPer
 */

#ifndef USART3_H_
#define USART3_H_


void USART3_init(void);
void USART3_send(unsigned char);
unsigned char USART3_recv(void);
void init_wifly();
void reboot_wifly();
void exit_wifly();


#endif /* USART3_H_ */
