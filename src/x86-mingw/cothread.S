.equ	COTHREAD_ATTR_STACK,		+0
.equ	COTHREAD_ATTR_STACK_SZ,		+4

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

	//---Initialize the cothread---//
	push	%edx			# push cothread_core arg1.
	push	%ecx			# push cothread_core arg0.
	call	_cothread_core	# call cothread_core.
	add		$(2*4), %esp	# remove arguments from the stack.

	//---Restore the caller stack---//
	mov		%esi, %esp
	mov		%edi, %ebp

	//---Return---//
	pop		%esi
	pop		%edi
	ret
