	.intel_syntax noprefix
.message:
	.string	"%d\n"
	.globl	main
main:
	push	rbp
	mov	rbp, rsp
	mov	esi, 10
	lea	rdi, .message[rip]
	mov	eax, 0
	call	printf@PLT
	mov	eax, 0
	pop	rbp
	ret
