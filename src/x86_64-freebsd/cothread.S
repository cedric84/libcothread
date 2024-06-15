.equ	COTHREAD_ATTR_STACK,		+0
.equ	COTHREAD_ATTR_STACK_SZ,		+8
.equ	COTHREAD_ATTR_CALLER_OFF,	+16
.equ	COTHREAD_ATTR_CALLEE_OFF,	+24
.equ	COTHREAD_ATTR_USER_CB,		+32

.equ	COTHREAD_CURRENT,	+0

.equ	LOCAL_USER_CB,		-8
.equ	LOCAL_CALLER,		-16
.equ	LOCAL_CALLEE,		-24

.text

// calling conventions:
// https://gitlab.com/x86-psABIs/x86-64-ABI
//
// %rdi is used as arg0.
// %rsi is used as arg1.
//
// %rbp, %rbx, %r12-%r15 belong to the calling function and shall be preserved by the called one.
// the stack shall be 16-byte aligned immediately before the call instruction is executed.
//
// mov	src, dst
//
.global	cothread_init
.type	cothread_init, @function
cothread_init:
	//---Save registers---//
	push	%r12
	push	%r13

	//---Save the caller stack---//
	mov		%rbp, %r12
	mov		%rsp, %r13

	//---Setup the callee stack---//
	// Compute the past-the-end stack address
	mov		COTHREAD_ATTR_STACK(%rsi), %rax		# store the lowest stack address in %rax.
	add		COTHREAD_ATTR_STACK_SZ(%rsi), %rax	# %rax points the past-the-end stack address.
	// Setup the callee stack frame
	mov		%rax, %rbp							# load the stack base pointer with the past-the-end stack address.
	mov		%rbp, %rsp							# empty the stack.
	// from this point, stack is aligned on a 16-byte boundary (if the provided stack is well defined.)

	//---Definitions---//
	push	COTHREAD_ATTR_USER_CB(%rsi)			# LOCAL_USER_CB(%rbp)
	//
	mov		%rdi, %rax
	add		COTHREAD_ATTR_CALLER_OFF(%rsi), %rax
	push	%rax								# LOCAL_CALLER(%rbp)
	//
	mov		%rdi, %rax
	add		COTHREAD_ATTR_CALLEE_OFF(%rsi), %rax
	push	%rax								# LOCAL_CALLEE(%rbp)
	//
	pushq	$0
	// from this point, stack is aligned on a 16-byte boundary

	//---Zero---//
	mov		LOCAL_CALLER(%rbp), %rax
	mov		%rax, COTHREAD_CURRENT(%rdi)

	//---Initialize the callee endpoint---//
cothread_init_init_callee:
	push	%rdi							# save %rdi.
	push	%rsi							# save %rsi.
	// from this point, stack is aligned on a 16-byte boundary
	mov		LOCAL_CALLEE(%rbp), %rdi		# %rdi is used as setjmp arg0.
	call	setjmp							# call setjmp.
	pop		%rsi							# restore %rsi.
	pop		%rdi							# restore %rdi.
	cmp		$0, %rax						# if setjmp returns 0 ...
	je		cothread_init_back_to_caller	# ... jump.
	// from this point, COTHREAD_ATTR_... are no more valid.

	//---Run the user callback---//
cothread_init_run_user_cb:
	push	%rdi					# save %rdi.
	pushq	$0
	// from this point, stack is aligned on a 16-byte boundary
	mov		%rax, %rsi				# %rsi is used as user callback arg1.
//	mov		%rdi, %rdi				# %rdi is used as user callback arg0 (already set.)
	call	*LOCAL_USER_CB(%rbp)	# call the user callback.
	add		$8, %rsp
	pop		%rdi					# restore %rdi.

	//---Jump to the caller---//
cothread_init_longjmp_to_caller:
	mov		LOCAL_CALLER(%rbp), %rsi		# load %rsi with the caller endpoint and ...
	mov		%rsi, COTHREAD_CURRENT(%rdi)	# ... set this endpoint as the current one.
	mov		%rax, %rsi						# %rsi is used as longjmp arg1.
	mov		COTHREAD_CURRENT(%rdi), %rdi	# %rdi is used as longjmp arg0.
	call	longjmp							# call longjmp and never come back.

	//---Restore the caller stack---//
cothread_init_back_to_caller:
	mov		%r13, %rsp
	mov		%r12, %rbp

	//---Return---//
	pop		%r13
	pop		%r12
	ret
