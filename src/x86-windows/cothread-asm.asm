.model FLAT, C

PUBLIC cothread_init

EXTERN cothread_core : proto

COTHREAD_ATTR_STACK			EQU		+0
COTHREAD_ATTR_STACK_SZ		EQU		+4

; calling conventions:
; https://learn.microsoft.com/en-us/cpp/cpp/argument-passing-and-naming-conventions?view=msvc-170
;
; Parameters are pushed onto the stack from right to left.
;
; edi, esi, ebx, ebp, esp belong to the calling function and shall be preserved by the called one.
; the stack shall be 4-byte aligned immediately before the call instruction is executed.
;
; mov	dst, src
;
.code
cothread_init:
	;---Move arguments from stack to registers---
	mov		ecx, 4[esp]		; save arg0 in ecx.
	mov		edx, 8[esp]		; save arg1 in edx.

	;---Save registers---
	push	edi
	push	esi

	;---Save the caller stack---
	mov		edi, ebp
	mov		esi, esp

	;---Setup the callee stack---
	; Compute the past-the-end stack address
	mov		eax, COTHREAD_ATTR_STACK[edx]		; store the lowest stack address in eax.
	add		eax, COTHREAD_ATTR_STACK_SZ[edx]	; eax points the past-the-end stack address.
	; Setup the callee stack frame
	mov		ebp, eax							; load the stack base pointer with the past-the-end stack address.
	mov		esp, ebp							; empty the stack.

	;---Initialize the cothread---
	push	edx				; push cothread_core arg1.
	push	ecx				; push cothread_core arg0.
	call	cothread_core	; call cothread_core.
	add		esp, (2*4)		; remove arguments from the stack.

	;---Restore the caller stack---
	mov		esp, esi
	mov		ebp, edi

	;---Return---
	pop		esi
	pop		edi
	ret
END
