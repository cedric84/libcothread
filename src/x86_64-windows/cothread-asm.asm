PUBLIC cothread_init

EXTERN __intrinsic_setjmp : proto
EXTERN longjmp : proto

COTHREAD_ATTR_STACK			EQU		+0
COTHREAD_ATTR_STACK_SZ		EQU		+8
COTHREAD_ATTR_CALLER_OFF	EQU		+16
COTHREAD_ATTR_CALLEE_OFF	EQU		+24
COTHREAD_ATTR_USER_CB		EQU		+32

COTHREAD_CURRENT		EQU		+0

LOCAL_USER_CB			EQU		-8
LOCAL_CALLER			EQU		-16
LOCAL_CALLEE			EQU		-24

; https://stackoverflow.com/questions/33751509/external-assembly-file-in-visual-studio
; https://developer.ibm.com/articles/l-gas-nasm/
;
; calling conventions:
; https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170
;
; rcx is used as arg0.
; rdx is used as arg1.
;
; r12-r15, rdi, rsi, rbx, rbp, rsp belong to the calling function and shall be preserved by the called one.
; the stack shall be 16-byte aligned immediately before the call instruction is executed.
; r9 home, r8 home, rdx home & rcx home are always pushed (in this order) before the call instruction is executed.
;
; mov	dst, src
;
.code
cothread_init:
	;---Save registers---
	push	r12
	push	r13

	;---Save the caller stack---
	mov		r12, rbp
	mov		r13, rsp

	;---Setup the callee stack---
	; Compute the past-the-end stack address
	mov		rax, COTHREAD_ATTR_STACK[rdx]		; store the lowest stack address in rax.
	add		rax, COTHREAD_ATTR_STACK_SZ[rdx]	; rax points the past-the-end stack address.
	; Setup the callee stack frame
	mov		rbp, rax							; load the stack base pointer with the past-the-end stack address.
	mov		rsp, rbp							; empty the stack.
	; from this point, stack is aligned on a 16-byte boundary (if the provided stack is well defined.)

	;---Definitions---
	push	COTHREAD_ATTR_USER_CB[rdx]			; LOCAL_USER_CB[rbp]
	;
	mov		rax, rcx
	add		rax, COTHREAD_ATTR_CALLER_OFF[rdx]
	push	rax									; LOCAL_CALLER[rbp]
	;
	mov		rax, rcx
	add		rax, COTHREAD_ATTR_CALLEE_OFF[rdx]
	push	rax									; LOCAL_CALLEE[rbp]
	;
	push	0
	; from this point, stack is aligned on a 16-byte boundary

	;---Zero---
	mov		rax, LOCAL_CALLER[rbp]
	mov		COTHREAD_CURRENT[rcx], rax

	;---Initialize the callee endpoint---
cothread_init_init_callee:
	push	rcx								; save rcx.
	push	rdx								; save rdx.
	; from this point, stack is aligned on a 16-byte boundary
	mov		rcx, LOCAL_CALLEE[rbp]			; rcx is used as setjmp arg0.
	mov		rdx, 0							; rdx is used as setjmp arg1 (yes, MSVCRT has two arguments, see the setjmp macro. Placing zero prevent the CRT to unwind the stack.)
	push	r9								; push r9 home.
	push	r8								; push r8 home.
	push	rdx								; push rdx home.
	push	rcx								; push rcx home.
	call	__intrinsic_setjmp				; call setjmp.
	add		rsp, (4*8)						; remove home registers from the stack.
	pop		rdx								; restore rdx.
	pop		rcx								; restore rcx.
	cmp		rax, 0							; if setjmp returns 0 ...
	je		cothread_init_back_to_caller	; ... jump.
	; from this point, COTHREAD_ATTR_... are no more valid.

	;---Run the user callback---;
cothread_init_run_user_cb:
	push	rcx								; save rcx.
	push	0
	; from this point, stack is aligned on a 16-byte boundary
	mov		rdx, rax						; rdx is used as user callback arg1.
;	mov		rcx, rcx						; rcx is used as user callback arg0 (already set.)
	push	r9								; push r9 home.
	push	r8								; push r8 home.
	push	rdx								; push rdx home.
	push	rcx								; push rcx home.
	call	qword ptr LOCAL_USER_CB[rbp]	; call the user callback.
	add		rsp, (4*8)						; remove home registers from the stack.
	add		rsp, 8
	pop		rcx								; restore rcx.

	;---Jump to the caller---
cothread_init_longjmp_to_caller:
	mov		rdx, LOCAL_CALLER[rbp]			; load rdx with the caller endpoint and ...
	mov		COTHREAD_CURRENT[rcx], rdx		; ... set this endpoint as the current one.
	mov		rdx, rax						; rdx is used as longjmp arg1.
	mov		rcx, COTHREAD_CURRENT[rcx]		; rcx is used as longjmp arg0.
	push	r9								; push r9 home.
	push	r8								; push r8 home.
	push	rdx								; push rdx home.
	push	rcx								; push rcx home.
	call	longjmp							; call longjmp and never come back.

	;---Restore the caller stack---
cothread_init_back_to_caller:
	mov		rsp, r13
	mov		rbp, r12

	;---Return---
	pop		r13
	pop		r12
	ret
END
