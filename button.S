.syntax unified   			/* Use unified style assembly syntax */
.thumb            			/* Cortex-M3 only supports Thumb code */
	
.global button_clear
.global button_disable
.global button_init

.text	

.equ GPIOD_MODER,0x40020C00
.equ GPIOD_IDR, 0x40020C10
.equ GPIOD_ODR,0x40020C14
.equ GPIOD_BSRR,0x40020C18
.equ RCC_AHB1ENR,0x40023830 
.equ STK_CTRL, 0xE000E010
.equ STK_LOAD, 0xE000E014
/* EXTI */
.equ EXTI_IMR,0x40013C00
.equ EXTI_RTSR,0x40013C08
.equ EXTI_PR,0x40013C14
/* NVIC */
.equ NVIC_ISER0,0xE000E100

button_clear:
	PUSH {r1-r2,LR}
	ldr r1, =EXTI_PR
	mov r2, #0x1
	orr r2, r1, r2
	str r2, [r1]
	POP {r1-r2,PC}

button_disable:
	PUSH {R0-R3,LR}
	/* Configure EXTI0 interrupt mask to signal NVIC for EXTI0 events */
	MOVW R0, #0xFFFE
	MOVT R0, #0xFFFF
	LDR R1, =EXTI_IMR
	LDR R2, [R1] /* Read */
	AND R2, R2, R0 /* Modify */
	STR R2, [R1] /* Write */

	LDR R1, =RCC_AHB1ENR
  	LDR R2, [R1] /* Read */
  	AND R2, R2, R0 /* Modify */
  	STR R2, [R1] /* Write */

	POP {R0-R3,PC}

button_init:
  PUSH {R0-R3,LR}
  /* Button is on GPIOA Pin 0 - Enable GPIOA */
  MOV R0, #1
  LDR R1, =RCC_AHB1ENR
  LDR R2, [R1] /* Read */
  ORR R2, R2, R0 /* Modify */
  STR R2, [R1] /* Write */
  /* PA0 default configuration is general-purpose input */
  /* EXTI0 default mapping is to PA0 */
  /* Configure EXTI0 for rising-edge event detection */
  MOV R0, #1
  LDR R1, =EXTI_RTSR
  LDR R2, [R1] /* Read */
  ORR R2, R2, R0 /* Modify */
  STR R2, [R1] /* Write */
  /* Configure EXTI0 interrupt mask to signal NVIC for EXTI0 events */
  MOV R0, #1
  LDR R1, =EXTI_IMR
  LDR R2, [R1] /* Read */
  ORR R2, R2, R0 /* Modify */
  STR R2, [R1] /* Write */
  /* Enable Interrupt ID6 (EXTI0) in the NVIC */
  MOV R0, #1
  MOV R1, R0, LSL #6
  LDR R2, =NVIC_ISER0
  STR R1, [R2]
  /* Return */
  POP {R0-R3,PC}
