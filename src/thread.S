.code64
.section .text
.globl switch_context

switch_context:
	push %rbp
	mov %rsp,%rbp
	sub $0x10,%rsp
	mov %rdi,-0x8(%rbp)
	mov %rsi,-0x10(%rbp)
	mov -0x10(%rbp),%rax
	mov (%rax),%rax
	test %rax,%rax
	je switch_context_load
	push %rbp
	push %rbx
	push %r12
	push %r13
	push %r14
	push %r15
	pushfq
switch_context_load:
	mov -0x10(%rbp),%rax
	mov (%rax),%rax
	mov %rsp,%rdx
	mov %rdx,(%rax)
	mov -0x8(%rbp),%rax
	mov (%rax),%rax
	mov %rax,%rsp
	mov -0x10(%rbp),%rax
	mov -0x8(%rbp),%rdx
	mov %rdx,(%rax)
	mov -0x10(%rbp),%rax
	mov (%rax),%rax
	mov %rax,%rdi
	callq try_wrapper_entry
	popfq
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbx
	pop %rbp
	leaveq
	retq

