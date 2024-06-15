.model FLAT, C

PUBLIC cothread_init

EXTERN setjmp : proto
EXTERN longjmp : proto

COTHREAD_ATTR_STACK			EQU		+0
COTHREAD_ATTR_STACK_SZ		EQU		+4
COTHREAD_ATTR_CALLER_OFF	EQU		+8
COTHREAD_ATTR_CALLEE_OFF	EQU		+12
COTHREAD_ATTR_USER_CB		EQU		+16

COTHREAD_CURRENT		EQU		+0

LOCAL_USER_CB			EQU		-4
LOCAL_CALLER			EQU		-8
LOCAL_CALLEE			EQU		-12

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

	;---Definitions---
	push	COTHREAD_ATTR_USER_CB[edx]			; LOCAL_USER_CB[ebp]
	;
	mov		eax, ecx
	add		eax, COTHREAD_ATTR_CALLER_OFF[edx]
	push	eax									; LOCAL_CALLER[ebp]
	;
	mov		eax, ecx
	add		eax, COTHREAD_ATTR_CALLEE_OFF[edx]
	push	eax									; LOCAL_CALLEE[ebp]

	;---Zero---
	mov		eax, LOCAL_CALLER[ebp]
	mov		COTHREAD_CURRENT[ecx], eax

	;---Initialize the callee endpoint---
cothread_init_init_callee:
	push	ecx								; save ecx.
	push	edx								; save edx.
	push	LOCAL_CALLEE[ebp]				; push setjmp arg0.
	call	setjmp							; call setjmp.
	add		esp, (1*4)						; remove arguments from the stack.
	pop		edx								; restore edx.
	pop		ecx								; restore ecx.
	cmp		eax, 0							; if setjmp returns 0 ...
	je		cothread_init_back_to_caller	; ... jump.
	; from this point, COTHREAD_ATTR_... are no more valid.

	;---Run the user callback---
cothread_init_run_user_cb:
	push	ecx								; save ecx.
	push	eax								; push user callback arg1.
	push	ecx								; push user callback arg0.
	call	dword ptr LOCAL_USER_CB[ebp]	; call the user callback.
	add		esp, (2*4)						; remove arguments from the stack.
	pop		ecx								; restore ecx.

	;---Jump to the caller---
cothread_init_longjmp_to_caller:
	mov		edx, LOCAL_CALLER[ebp]		; load edx with the caller endpoint and ...
	mov		COTHREAD_CURRENT[ecx], edx	; ... set this endpoint as the current one.
	push	eax							; push longjmp arg1.
	push	COTHREAD_CURRENT[ecx]		; push longjmp arg0.
	call	longjmp						; call longjmp and never come back.

	;---Restore the caller stack---
cothread_init_back_to_caller:
	mov		esp, esi
	mov		ebp, edi

	;---Return---
	pop		esi
	pop		edi
	ret
END
