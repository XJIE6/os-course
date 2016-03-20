	.code64
	.global empty, run_repeat_me, err
	.extern repeat_me

run_repeat_me:
	push %rax
	push %rcx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11

	call repeat_me
	
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rcx
	pop %rax
	
	iretq
 
empty:
	iretq

err:
	add $8, %rsp
	iretq