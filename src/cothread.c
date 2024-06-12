/**
 * @brief		This file contains the C definitions.
 * @file
 */

#include <cothread/cothread.h>
#include <assert.h>
#include <stdint.h>

#if ((COTHREAD_CC_ID_CL == COTHREAD_CC_ID) && (COTHREAD_OS_ID_WINDOWS == COTHREAD_OS_ID))
	extern int	__intrinsic_setjmp	(jmp_buf, void*);
#endif

extern COTHREAD_LINK void COTHREAD_CALL
cothread_ep_init(cothread_ep_t* ep)
{
	assert(NULL	!= ep);
	// do nothing.
}

extern COTHREAD_LINK void COTHREAD_CALL
cothread_attr_init(cothread_attr_t* attr, cothread_stack_t* stack, size_t stack_sz, cothread_ep_t* caller, cothread_ep_t* callee, cothread_cb_t user_cb)
{
	//---Check arguments---//
	assert(NULL	!= attr);
	assert(NULL	!= stack);
	assert(0	!= stack_sz);
	assert(NULL	!= caller);
	assert(NULL	!= callee);
	assert(NULL	!= user_cb);

	//---Check the stack alignment---//
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & (uintptr_t)stack));
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & stack_sz));

	//---Initialize---//
	attr->stack		= stack;
	attr->stack_sz	= stack_sz;
	attr->caller	= caller;
	attr->callee	= callee;
	attr->user_cb	= user_cb;
}

extern COTHREAD_LINK void COTHREAD_CALL
cothread_set_user_data(cothread_t* cothread, void* user_data)
{
	assert(NULL	!= cothread);
	cothread->user_data	= user_data;
}

extern COTHREAD_LINK void* COTHREAD_CALL
cothread_get_user_data(const cothread_t* cothread)
{
	assert(NULL	!= cothread);
	return cothread->user_data;
}

extern COTHREAD_LINK int COTHREAD_CALL
cothread_yield(cothread_t* cothread, int user_val)
{
	//---Check arguments---//
	assert(NULL	!= cothread);

	//---Save the current endpoint---//
#if ((COTHREAD_ARCH_ID_X86_64 == COTHREAD_ARCH_ID) && (COTHREAD_OS_ID_WINDOWS == COTHREAD_OS_ID))
	#if		(COTHREAD_CC_ID_MINGW == COTHREAD_CC_ID)
		const int	ret	= _setjmp(cothread->current->buf, 0);
	#elif	(COTHREAD_CC_ID_CL == COTHREAD_CC_ID)
		const int	ret = __intrinsic_setjmp(cothread->current->buf, 0);
	#endif
#else
	const int	ret	= setjmp(cothread->current->buf);
#endif

	//---Is it the first return from setjmp ?---//
	if (0 == ret) {
		//---Switch the endpoints---//
		cothread->current	= (cothread->caller == cothread->current) ? cothread->callee : cothread->caller;
		longjmp(cothread->current->buf, user_val);
	}

	//---Return---//
	return ret;
}
