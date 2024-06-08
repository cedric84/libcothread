.equ	COTHREAD_ATTR_STACK,		+0
.equ	COTHREAD_ATTR_STACK_SZ,		+8
.equ	COTHREAD_ATTR_CALLER,		+16
.equ	COTHREAD_ATTR_CALLEE,		+24
.equ	COTHREAD_ATTR_USER_CB,		+32

.equ	COTHREAD_CURRENT,	+0
.equ	COTHREAD_CALLER,	+8
.equ	COTHREAD_CALLEE,	+16

.equ	LOCAL_USER_CB,		-8

.text

// calling conventions:
// https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170
//
// %rcx is used as arg0.
// %rdx is used as arg1.
//
// %r12-%r15, %rdi, %rsi, %rbx, %rbp, %rsp belong to the calling function and shall be preserved by the called one.
// the stack shall be 16-byte aligned immediately before the call instruction is executed.
// %r9 home, %r8 home, %rdx home & %rcx home are always pushed (in this order) before the call instruction is executed.
//
// mov	src, dst
//
.global	cothread_init
cothread_init:
	//---Save registers---//
	push	%r12
	push	%r13

	//---Save the caller stack---//
	mov		%rbp, %r12
	mov		%rsp, %r13

	//---Setup the callee stack---//
	// Compute the past-the-end stack address
	mov		COTHREAD_ATTR_STACK(%rdx), %rax		# store the lowest stack address in %rax.
	add		COTHREAD_ATTR_STACK_SZ(%rdx), %rax	# %rax points the past-the-end stack address.
	// Setup the callee stack frame
	mov		%rax, %rbp							# load the stack base pointer with the past-the-end stack address.
	mov		%rbp, %rsp							# empty the stack.
	// from this point, stack is aligned on a 16-byte boundary (if the provided stack is well defined.)

	//---Definitions---//
	push	COTHREAD_ATTR_USER_CB(%rdx)			# LOCAL_USER_CB(%rbp)
	pushq	$0
	// from this point, stack is aligned on a 16-byte boundary

	//---Initialize the callee endpoint---//
cothread_init_init_callee:
	push	%rcx								# save %rcx.
	push	%rdx								# save %rdx.
	// from this point, stack is aligned on a 16-byte boundary
	mov		COTHREAD_ATTR_CALLEE(%rdx), %rcx	# %rcx is used as setjmp arg0.
	mov		$0, %rdx							# %rdx is used as setjmp arg1 (yes, MSVCRT has two arguments, see the setjmp macro. Placing zero prevent the CRT to unwind the stack.)
	push	%r9									# push %r9 home.
	push	%r8									# push %r8 home.
	push	%rdx								# push %rdx home.
	push	%rcx								# push %rcx home.
	call	setjmp								# call setjmp.
	add		$(4*8), %rsp						# remove home registers from the stack.
	pop		%rdx								# restore %rdx.
	pop		%rcx								# restore %rcx.
	cmp		$0, %rax							# if setjmp returns 0 ...
	je		cothread_init_back_to_caller		# ... jump.
	// from this point, COTHREAD_ATTR_... are no more valid.

	//---Run the user callback---//
cothread_init_run_user_cb:
	push	%rcx					# save %rcx.
	pushq	$0
	// from this point, stack is aligned on a 16-byte boundary
	mov		%rax, %rdx				# %rdx is used as user callback arg1.
//	mov		%rcx, %rcx				# %rcx is used as user callback arg0 (already set.)
	push	%r9						# push %r9 home.
	push	%r8						# push %r8 home.
	push	%rdx					# push %rdx home.
	push	%rcx					# push %rcx home.
	call	*LOCAL_USER_CB(%rbp)	# call the user callback.
	add		$(4*8), %rsp			# remove home registers from the stack.
	add		$8, %rsp
	pop		%rcx					# restore %rcx.

	//---Jump to the caller---//
cothread_init_longjmp_to_caller:
	mov		COTHREAD_CALLER(%rcx), %rdx		# load %rdx with the caller endpoint and ...
	mov		%rdx, COTHREAD_CURRENT(%rcx)	# ... set this endpoint as the current one.
	mov		%rax, %rdx						# %rdx is used as longjmp arg1.
	mov		COTHREAD_CURRENT(%rcx), %rcx	# %rcx is used as longjmp arg0.
	push	%r9								# push %r9 home.
	push	%r8								# push %r8 home.
	push	%rdx							# push %rdx home.
	push	%rcx							# push %rcx home.
	call	longjmp							# call longjmp and never come back.

	//---Restore the caller stack---//
cothread_init_back_to_caller:
	mov		%r13, %rsp
	mov		%r12, %rbp

	//---Return---//
	mov		COTHREAD_ATTR_CALLER(%rdx), %rax
	mov		%rax, COTHREAD_CURRENT(%rcx)
	mov		%rax, COTHREAD_CALLER(%rcx)
	mov		COTHREAD_ATTR_CALLEE(%rdx), %rax
	mov		%rax, COTHREAD_CALLEE(%rcx)
	pop		%r13
	pop		%r12
	ret
