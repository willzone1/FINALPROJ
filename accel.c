/*
 * accel.c
 *
 *  Created on: Feb 20, 2016
 *      Author: WillPer
 */

#include "stm32f4xx.h"

void accelInit(void){
	RCC->AHB1ENR |= 0x11;         /* enable GPIOA and GPIOE clock */
	RCC->APB2ENR |= 0x1000;       /* enable SPI1 clock (bit 12) */
	GPIOA->MODER |= 0xA800;       // enable ports 5,6,7 as AF (ref man p198)
	GPIOA->AFRL |= 0x55500000;    // set GPIOA pin 5,6,7 as AF5
	GPIOE->MODER |= 0x40;         // enable GPIOE port 3 as general purpose IO
	// clear SPE in config register one to disable the peripheral before making config changes
	SPI1->CR1 &= 0xFFCF;              // clear CR1
	/*
	* Set up CR1 as follows:
	* - Data format = 16 bits
	* - Enable software slave management and force NSS to '1' (we use GPIO for CS)
	* - Set clock divider for a 1 MHz SCLK (recall SYSCLK = 16MHz)
	* - Configure for master mode operation
	* - Configure CPOL/CPHA (see LIS302DL datasheet)
	*
	* Then set SPE in configuration register to 1 to enable the SPI peripheral
	*/

	// data format to 16 bits: set DFF to 1
	SPI1->CR1 |= 0x800;
	// enable software slave management (bit 9) and SSI
	SPI1->CR1 |= 0x300;
	// set clock divider for a 1MHz SCLK
	SPI1->CR1 |= 0x18;
	// configure for master mode operation
	SPI1->CR1 |= 0x4;
	// we want clock polarity to be high when idle (going off documentation on the accelerometer). CPOL=1
	SPI1->CR1 |= 0x2;
	// we want to capture data on the second clock transition. CPHA=1
	SPI1->CR1 |= 0x1;
	// transmission starts with MSB. LSBFIRST=0 (bit 7)
	unsigned int msb_mask = 0xFF7F;
	SPI1->CR1 &= msb_mask;
	// Set FRF to Motorola mode
	SPI1->CR2 &= 0xFFEF;
	// enable peripheral
	SPI1->CR1 |= 0x40;
	// set chip select pin to logic high
	accel_cs_high();

}


void accel_cs_high(){
   GPIOE->BSRR = 0x8; // set bit 3 high
   while(1) {
		 unsigned int isDone = GPIOE->IDR & 0x8;
		 if (isDone) {
			   return;
		 }
   }
}

void accel_cs_low(){
   GPIOE->BSRR = 0x80000; // reset bit 3 to 0
   while(1) {
		 unsigned int isDone = GPIOE->IDR & 0x8;
		 if (isDone == 0) {
			   return;
		 }
   }
}

unsigned short accel_read(uint8_t accel_reg) {
   /**
    * Assemble 16-bit write value to send from master to slave as described
    *  in 5.2.1 of acc datasheet.
    *
    * First we have to do some configuration settings.
    */
   while(1) {
      // check the status of SPI1 TXE bit in status register
      unsigned int checkTXE = SPI1->SR & 0x2; // bit 1
      if (checkTXE == 2) {
            // select accelerometer by setting chip select signal to 0
            accel_cs_low();
            // issue read transaction by writing to SPI1 data register
            // bit 16: 0
            // then address of indexed register
            unsigned int toSend = 1;
            toSend = toSend<<7;
            toSend = toSend + accel_reg;
            // now shift by 8 more bits, since MSB.
            toSend = toSend<<8;
            SPI1->DR = toSend;
            // now poll the BSY flag in SR. When clear, finish.
            while(1) {
                  unsigned int BSY = SPI1->SR & 0x80;
                  if (BSY == 0) {
                        // flag has cleared, SP1 peripheral is finished.
                        accel_cs_high();
                        // now read the data
                        uint16_t data = SPI1->DR;
                        data = data & 0xFF; // just lower 8 bits
                        return data;
                  }
            }
      }
   }
}

void accel_write(uint8_t accel_reg, uint8_t data) {
   while (SPI1->SR & 0x2 == 0);
   // now the TXE bit is cleared and we can coordinate the
   // chip select signal and write data to SPI data register
   accel_cs_low();
   //toSend = toSend << 7;
   uint16_t toSend = accel_reg;
   // now shift by 8 more bits, since MSB.
   toSend = toSend<<8;

   toSend = toSend + data;

   SPI1->DR = toSend;
   while ((SPI1->SR & 0x80) != 0); // now BSY flag is cleared.
   accel_cs_high();

//      SPI1->DR = data;
//      while (BSY != 0); // now the BSY flag is cleared and data has been written

}
