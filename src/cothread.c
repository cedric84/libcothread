/**
 * @brief		This file contains the C definitions.
 * @file
 */

#include <cothread/cothread.h>
#include <assert.h>
#include <stdint.h>

#if		(COTHREAD_CC_ID_CLANG != COTHREAD_CC_ID)
	_Static_assert(0	== (uintptr_t)&(((cothread_ep_t*)0)->buf)			, "invalid offset");

	_Static_assert(0	== (uintptr_t)&(((cothread_attr_t*)0)->stack)		, "invalid offset");
	_Static_assert(8	== (uintptr_t)&(((cothread_attr_t*)0)->stack_sz)	, "invalid offset");
	_Static_assert(16	== (uintptr_t)&(((cothread_attr_t*)0)->caller)		, "invalid offset");
	_Static_assert(24	== (uintptr_t)&(((cothread_attr_t*)0)->callee)		, "invalid offset");
	_Static_assert(32	== (uintptr_t)&(((cothread_attr_t*)0)->user_cb)		, "invalid offset");

	_Static_assert(0	== (uintptr_t)&(((cothread_t*)0)->current)			, "invalid offset");
	_Static_assert(8	== (uintptr_t)&(((cothread_t*)0)->caller)			, "invalid offset");
	_Static_assert(16	== (uintptr_t)&(((cothread_t*)0)->callee)			, "invalid offset");
#endif

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
#if ((COTHREAD_CC_ID_MINGW == COTHREAD_CC_ID) && (COTHREAD_OS_ID_WINDOWS == COTHREAD_OS_ID))
	const int	ret	= _setjmp(cothread->current->buf, 0);
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
