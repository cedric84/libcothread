.section	.note.GNU-stack, "", @progbits

.equ	COTHREAD_ATTR_STACK,		+0
.equ	COTHREAD_ATTR_STACK_SZ,		+4
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
	// from this point, stack is aligned on a 16-byte boundary

	//---Initialize the cothread---//
	sub		$(2*4), %esp	# pad the stack to make it 16-byte aligned before the call.
	push	%edx			# push cothread_core arg1.
	push	%ecx			# push cothread_core arg0.
	call	cothread_core	# call cothread_core.
	add		$(2*4), %esp	# remove arguments from the stack.
	add		$(2*4), %esp	# remove the padding from the stack.

	//---Restore the caller stack---//
	mov		%esi, %esp
	mov		%edi, %ebp

	//---Return---//
	pop		%esi
	pop		%edi
	ret
