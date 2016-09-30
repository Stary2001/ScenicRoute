.macro SVC_BEGIN name
	.section .text.\name, "ax", %progbits
	.global \name
	.type \name, %function
	.align 2
\name:
.endm

SVC_BEGIN svcGetThreadList
	push {r0}
	svc 0x66
	ldr r3, [sp, #0]
	str r1, [r3]
	add sp, sp, #4
	bx lr

SVC_BEGIN svcGetDebugThreadContext
	svc 0x67
	bx  lr