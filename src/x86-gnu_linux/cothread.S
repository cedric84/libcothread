.section	.note.GNU-stack, "", @progbits

.equ	COTHREAD_ATTR_STACK,		+0
.equ	COTHREAD_ATTR_STACK_SZ,		+4
.equ	COTHREAD_ATTR_CALLER_OFF,	+8
.equ	COTHREAD_ATTR_CALLEE_OFF,	+12
.equ	COTHREAD_ATTR_USER_CB,		+16

.equ	COTHREAD_CURRENT,	+0

.equ	LOCAL_USER_CB,		-4
.equ	LOCAL_CALLER,		-8
.equ	LOCAL_CALLEE,		-12

.text

// calling conventions:
// https://en.wikipedia.org/wiki/X86_calling_conventions
//
// Parameters are pushed onto the stack from right to left.
//
// %edi, %esi, %ebx, %ebp, %esp belong to the calling function and shall be preserved by the called one.
// the stack shall be 16-byte aligned immediately before the call instruction is executed.
//
// mov	src, dst
//
.global	cothread_init
.type	cothread_init, @function
cothread_init:
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
	//
	mov		%ecx, %eax
	add		COTHREAD_ATTR_CALLER_OFF(%edx), %eax
	push	%eax								# LOCAL_CALLER(%ebp)
	//
	mov		%ecx, %eax
	add		COTHREAD_ATTR_CALLEE_OFF(%edx), %eax
	push	%eax								# LOCAL_CALLEE(%ebp)
	//
	sub		$(1*4), %esp
	// from this point, stack is aligned on a 16-byte boundary

	//---Zero---//
	mov		LOCAL_CALLER(%ebp), %eax
	mov		%eax, COTHREAD_CURRENT(%ecx)

	//---Initialize the callee endpoint---//
cothread_init_init_callee:
	sub		$(1*4), %esp					# pad the stack to make it 16-byte aligned before the call.
	push	%ecx							# save %ecx.
	push	%edx							# save %edx.
	push	LOCAL_CALLEE(%ebp)				# push setjmp arg0.
	call	setjmp@plt						# call setjmp.
	add		$(1*4), %esp					# remove arguments from the stack.
	pop		%edx							# restore %edx.
	pop		%ecx							# restore %ecx.
	add		$(1*4), %esp					# remove the padding from the stack.
	cmp		$0, %eax						# if setjmp returns 0 ...
	je		cothread_init_back_to_caller	# ... jump.
	// from this point, COTHREAD_ATTR_... are no more valid.

	//---Run the user callback---//
cothread_init_run_user_cb:
	sub		$(1*4), %esp			# pad the stack to make it 16-byte aligned before the call.
	push	%ecx					# save %ecx.
	push	%eax					# push user callback arg1.
	push	%ecx					# push user callback arg0.
	call	*LOCAL_USER_CB(%ebp)	# call the user callback.
	add		$(2*4), %esp			# remove arguments from the stack.
	pop		%ecx					# restore %ecx.
	add		$(1*4), %esp			# remove the padding from the stack.

	//---Jump to the caller---//
cothread_init_longjmp_to_caller:
	mov		LOCAL_CALLER(%ebp), %edx		# load %edx with the caller endpoint and ...
	mov		%edx, COTHREAD_CURRENT(%ecx)	# ... set this endpoint as the current one.
	sub		$(2*4), %esp					# pad the stack to make it 16-byte aligned before the call.
	push	%eax							# push longjmp arg1.
	push	COTHREAD_CURRENT(%ecx)			# push longjmp arg0.
	call	longjmp@plt						# call longjmp and never come back.

	//---Restore the caller stack---//
cothread_init_back_to_caller:
	mov		%esi, %esp
	mov		%edi, %ebp

	//---Return---//
	pop		%esi
	pop		%edi
	ret
