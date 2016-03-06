#pragma once

#include "stdint.h"

/*
 * Some useful definitions for controlling the clock enable registers,
 * configuring GPIO and turning LEDs on and off.
 */

/* GPIO Registers, see the reference manual sections 7.4.X */
volatile typedef struct
{
	uint32_t MODER;                /* Mode register                    - offset 0x00 */
	uint32_t OTYPER;               /* Output type register             - offset 0x04 */
	uint32_t OSPEEDR;              /* Output speed register            - offset 0x08 */
	uint32_t PUPDR;                /* Port pull-up/pull-down register  - offset 0x0C */
	uint32_t IDR;                  /* Input data register              - offset 0x10 */
	uint32_t ODR;                  /* Output data register             - offset 0x14 */
	uint32_t BSRR;                 /* Bit set-reset register           - offset 0x18 */
	uint32_t LCKR;                 /* Port configuration lock register - offset 0x1C */
	uint32_t AFRL;                 /* Alternate function low register  - offset 0x20 */
	uint32_t AFRH;                 /* Alternate function high register - offset 0x24 */
} GPIO_TypeDef;

/* Reset and Clock Control Registers, see User's Manual section 6.3.X */
volatile typedef struct
{
	uint32_t CR;                   /* Clock control register           - offset 0x00 */
	uint32_t PLLCFGR;              /* PLL configuration register       - offset 0x04 */
	uint32_t CFGR;                 /* RCC clock configuration register - offset 0x08 */
	uint32_t CIR;                  /* Clock configuration register     - offset 0x0C */
	uint32_t AHB1RSTR;             /* AHB1 peripheral reset register   - offset 0x10 */
	uint32_t AHB2RSTR;             /* AHB2 peripheral reset register   - offset 0x14 */
	uint32_t AHB3RSTR;             /* AHB3 peripheral reset register   - offset 0x18 */
	uint32_t RES0;                 /* Reserved                         - offset 0x1C */
	uint32_t APB1RSTR;             /* APB1 peripheral reset register   - offset 0x20 */
	uint32_t APB2RSTR;             /* APB2 peripheral reset register   - offset 0x24 */
	uint32_t RES1;                 /* Reserved                         - offset 0x28 */
	uint32_t RES2;                 /* Reserved                         - offset 0x2C */
	uint32_t AHB1ENR;              /* AHB1 peripheral clock enable     - offset 0x30 */
	uint32_t RES3;                 /* Reserved                         - offset 0x34 */
	uint32_t RES4;                 /* Reserved                         - offset 0x38 */
	uint32_t RES5;                 /* Reserved                         - offset 0x3C */
	uint32_t APB1ENR;              /* APB1 peripheral clock enable     - offset 0x40 */
	uint32_t APB2ENR;              /* APB2 peripheral clock enable     - offset 0x44 */
	/* Rest of registers not utilized in lab 3 */
} RCC_TypeDef;

/* SYSTICK timer control registers, see the reference manual sections 6.3.X */
volatile typedef struct
{
	uint32_t CTRL;              	/* Clock control register           - offset 0x00 */
	uint32_t LOAD;              	/* PLL configuration register       - offset 0x04 */

} STK_TypeDef;

volatile typedef struct
{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMCR;
	uint32_t DIER;
	uint32_t SR;
	uint32_t EGR;
	uint32_t CCMR1;
	uint32_t CCMR2;
	uint32_t CCER;
	uint32_t CNT;
	uint32_t PSC;
	uint32_t ARR;
	uint32_t RCR;
	uint32_t CCR1;
	uint32_t CCR2;
	uint32_t CCR3;
	uint32_t CCR4;
	uint32_t BDTR;
	uint32_t DCR;
	uint32_t DMAR;

}TIM_TypeDef;

volatile typedef struct
{
	uint32_t SR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMPR1;
	uint32_t SMPR2;
	uint32_t JOFR1;
	uint32_t JOFR2;
	uint32_t JOFR3;
	uint32_t JOFR4;
	uint32_t HTR;
	uint32_t LTR;
	uint32_t SQR1;
	uint32_t SQR2;
	uint32_t SQR3;
	uint32_t JSQR;
	uint32_t JDR1;
	uint32_t JDR2;
	uint32_t JDR3;
	uint32_t JDR4;
	uint32_t DR;
	uint32_t CSR;
	uint32_t CCR;
	uint32_t CDR;


}ADC_TypeDef;

volatile typedef struct
{
	uint32_t REG;

}CPACR_TypeDef;

volatile typedef struct
{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SR;
	uint32_t DR;
	uint32_t CRCPR;
	uint32_t RXCRCR;
	uint32_t TXCRCR;
	uint32_t I2SCFGR;
	uint32_t I2SPR;

}SPI_TypeDef;

volatile typedef struct
{
	uint32_t SR;
	uint32_t DR;
	uint32_t BRR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t GTPR;

}USART_TypeDef;

volatile typedef struct
{
	uint32_t ISER0;
	uint32_t ISER1;

}NVIC_TypeDef;

#define NVIC_BASE	(0xE000E100)
#define NVIC		((NVIC_TypeDef*)NVIC_BASE)

#define USART2_BASE	(0x40004400)
#define USART2		((USART_TypeDef*)USART3_BASE)
#define USART3_BASE	(0x40004800)
#define USART3		((USART_TypeDef*)USART3_BASE)

/* See datasheet for memory map / base addresses */

#define CPACR_BASE 	(0xE000ED88)
#define CPACR		((CPACR_TypeDef*)CPACR_BASE)

#define SPI1_BASE	(0x40013000)
#define SPI1		((SPI_TypeDef*)SPI1_BASE)

/*
 * GPIOA & GPIOD
 */
#define GPIOA_BASE   (0x40020000)  /* Base address of GPIOA peripheral */
#define GPIOA        ((GPIO_TypeDef*)GPIOA_BASE)
#define GPIOD_BASE   (0x40020C00)  /* Base address of GPIOD peripheral */
#define GPIOD        ((GPIO_TypeDef*)GPIOD_BASE)
#define GPIOE_BASE	 (0x40021000)  /* Base address of GPIOE peripheral */
#define GPIOE		 ((GPIO_TypeDef*)GPIOE_BASE)

#define GPIOx_MODER_PIN12_MASK  (0xfcffffff)
#define GPIOx_MODER_PIN13_MASK  (0xf3ffffff)
#define GPIOx_MODER_PIN14_MASK  (0xcfffffff)
#define GPIOx_MODER_PIN15_MASK  (0x3fffffff)
#define GPIOx_MODER_PIN12_GPOUT (0x01000000)
#define GPIOx_MODER_PIN13_GPOUT (0x04000000)
#define GPIOx_MODER_PIN14_GPOUT (0x10000000)
#define GPIOx_MODER_PIN15_GPOUT (0x40000000)

#define RCC_BASE     (0x40023800)
#define RCC          ((RCC_TypeDef*)RCC_BASE)

#define RCC_AHB1ENR_GPIOAEN     (0x01)
#define RCC_AHB1ENR_GPIODEN     (0x08)
#define ADC1_clockon			(0x0100)

#define TIM1_BASE	(0x40010000)
#define TIM1		((TIM_TypeDef*)TIM1_BASE)

#define POT_GPIOmask			(0x0C)

#define ADC_BASE	(0x40012000)
#define ADC1		((ADC_TypeDef*)ADC_BASE)
#define ADC1_on		(0x01)

#define STK_BASE (0xE000E010)
#define STK_CTRL (0xE000E010)
//#define STK_LOAD (0x00186A00)		//10Hz
//#define STK_LOAD (0x00013380)		//200Hz
#define STK_LOAD (0x1EC0)			//2kHz
#define STK_CTRL_init (0x07)
#define SYSTICK   ((STK_TypeDef*)STK_BASE)

#define TYPE_PING 1

typedef struct {
 int type;
 int id;
} ping_t;

#define TYPE_UPDATE 2

typedef struct {
 int type;
 int id;
 int value;
} update_request_t;

typedef struct {
 int type;
 int id;
 int average;
 int values[30];
} update_response_t;

