/*
 * mutex.S
 *
 *  Created on: Feb 3, 2016
 *      Author: WillPer
 */

.syntax unified   			/* Use unified style assembly syntax */
.thumb            			/* Cortex-M3 only supports Thumb code */

.equ locked, 1
.equ unlocked, 0

.global lock_mutex
.global unlock_mutex

.text

lock_mutex:	/*todo: return a value to r0 to tell if it worked or not*/
	push {r1-r3,lr}
	mov r1, locked
	test_locked:
		ldrex r2, [r0]
		cmp r2, r1
		beq fail
		strex r3,r1,[r0]
		cmp r3, #1
		beq fail
		dmb
		mov r0, #0
		b end
	fail:
		mov r0, #1
	end:
		pop {r1-r3,pc}

unlock_mutex:
	push {r1-r2,lr}
	mov r1, unlocked
	dmb
	str r1, [r0]
	pop {r1-r2,pc}
