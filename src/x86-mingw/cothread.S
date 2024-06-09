.equ	COTHREAD_ATTR_STACK,		+0
.equ	COTHREAD_ATTR_STACK_SZ,		+4
.equ	COTHREAD_ATTR_CALLER,		+8
.equ	COTHREAD_ATTR_CALLEE,		+12
.equ	COTHREAD_ATTR_USER_CB,		+16

.equ	COTHREAD_CURRENT,	+0
.equ	COTHREAD_CALLER,	+4
.equ	COTHREAD_CALLEE,	+8

.equ	LOCAL_USER_CB,		-4

.text

// calling conventions:
// https://learn.microsoft.com/en-us/cpp/cpp/argument-passing-and-naming-conventions?view=msvc-170
//
// Parameters are pushed onto the stack from right to left.
//
// %edi, %esi, %ebx, %ebp, %esp belong to the calling function and shall be preserved by the called one.
// the stack shall be 4-byte aligned immediately before the call instruction is executed.
//
// mov	src, dst
//
.global	_cothread_init
_cothread_init:
	//---Move arguments from stack to registers---//
	mov		4(%esp), %ecx	# save arg0 in %ecx.
	mov		8(%esp), %edx	# save arg1 in %edx.

	//---Save registers---//
	push	%edi
	push	%esi

	//---Save the caller stack---//
	mov		%ebp, %edi
	mov		%esp, %esi

	//---Setup the callee stack---//
	// Compute the past-the-end stack address
	mov		COTHREAD_ATTR_STACK(%edx), %eax		# store the lowest stack address in %eax.
	add		COTHREAD_ATTR_STACK_SZ(%edx), %eax	# %eax points the past-the-end stack address.
	// Setup the callee stack frame
	mov		%eax, %ebp							# load the stack base pointer with the past-the-end stack address.
	mov		%ebp, %esp							# empty the stack.

	//---Definitions---//
	push	COTHREAD_ATTR_USER_CB(%edx)			# LOCAL_USER_CB(%ebp)

	//---Initialize the callee endpoint---//
cothread_init_init_callee:
	push	%ecx							# save %ecx.
	push	%edx							# save %edx.
	push	COTHREAD_ATTR_CALLEE(%edx)		# push setjmp arg0.
	call	__setjmp						# call setjmp.
	add		$(1*4), %esp					# remove arguments from the stack.
	pop		%edx							# restore %edx.
	pop		%ecx							# restore %ecx.
	cmp		$0, %eax						# if setjmp returns 0 ...
	je		cothread_init_back_to_caller	# ... jump.
	// from this point, COTHREAD_ATTR_... are no more valid.

	//---Run the user callback---//
cothread_init_run_user_cb:
	push	%ecx					# save %ecx.
	push	%eax					# push user callback arg1.
	push	%ecx					# push user callback arg0.
	call	*LOCAL_USER_CB(%ebp)	# call the user callback.
	add		$(2*4), %esp			# remove arguments from the stack.
	pop		%ecx					# restore %ecx.

	//---Jump to the caller---//
cothread_init_longjmp_to_caller:
	mov		COTHREAD_CALLER(%ecx), %edx		# load %edx with the caller endpoint and ...
	mov		%edx, COTHREAD_CURRENT(%ecx)	# ... set this endpoint as the current one.
	push	%eax							# push longjmp arg1.
	push	COTHREAD_CURRENT(%ecx)			# push longjmp arg0.
	call	_longjmp						# call longjmp and never come back.

	//---Restore the caller stack---//
cothread_init_back_to_caller:
	mov		%esi, %esp
	mov		%edi, %ebp

	//---Return---//
	mov		COTHREAD_ATTR_CALLER(%edx), %eax
	mov		%eax, COTHREAD_CURRENT(%ecx)
	mov		%eax, COTHREAD_CALLER(%ecx)
	mov		COTHREAD_ATTR_CALLEE(%edx), %eax
	mov		%eax, COTHREAD_CALLEE(%ecx)
	pop		%esi
	pop		%edi
	ret
