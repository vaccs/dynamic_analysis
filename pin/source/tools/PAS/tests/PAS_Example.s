	.file	"PAS_Example.c"
	.comm	a,40,32
	.text
	.globl	bar
	.type	bar, @function
bar:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, -52(%rbp)
	jmp	.L2
.L3:
	movl	-52(%rbp), %eax
	cltq
	movl	-52(%rbp), %edx
	movl	%edx, -48(%rbp,%rax,4)
	movl	-52(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	leal	1(%rax), %edx
	movl	-52(%rbp), %eax
	cltq
	movl	%edx, a(,%rax,4)
	addl	$1, -52(%rbp)
.L2:
	cmpl	$9, -52(%rbp)
	jle	.L3
	nop
	movq	-8(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L4
	call	__stack_chk_fail
.L4:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	bar, .-bar
	.globl	foo
	.type	foo, @function
foo:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	foo, .-foo
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, %eax
	call	foo
	movl	$0, %eax
	call	bar
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
