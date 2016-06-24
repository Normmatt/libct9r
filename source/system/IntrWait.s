	.arch	armv5te
	.cpu	arm946e-s

	.text
	.arm

	.global IntrWait
	.type IntrWait STT_FUNC

IntrWait:
	@ savedIME = REG_IME, REG_IME = 1
	mov r2, #1
	mov	r12, #0x10000000
	add	r12, #0x00001000		@ REG_BASE
	ldrb r3, [r12, #0x8]
	strb r2, [r12, #0x8]
	
	@ Wait for IRQ
	mov r2, #0
	mcr 15, 0, r2, c7, c0, 4
	
	@ REG_IME = savedIME
	strb r3, [r12, #0x8]
	
	@ return
	bx lr
