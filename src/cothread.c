/**
 * @brief		This file contains the C definitions.
 * @file
 */

#ifdef _FORTIFY_SOURCE
	// prevent GCC to raise a "longjmp causes uninitialized stackframe" error.
	// see https://www.gnu.org/software/libc/manual/html_node/Source-Fortification.html
	#undef _FORTIFY_SOURCE
#endif

#include <cothread/cothread.h>
#include <assert.h>
#include <stdint.h>

#if ((COTHREAD_ARCH_ID_X86_64 == COTHREAD_ARCH_ID) && (COTHREAD_OS_ID_WINDOWS == COTHREAD_OS_ID))
	#if		(COTHREAD_CC_ID_MINGW == COTHREAD_CC_ID)
		#define COTHREAD_SETJMP(_buf)	_setjmp((_buf), 0)
	#elif	(COTHREAD_CC_ID_CL == COTHREAD_CC_ID)
		extern int	__intrinsic_setjmp	(jmp_buf, void*);
		#define COTHREAD_SETJMP(_buf)	__intrinsic_setjmp((_buf), 0)
	#endif
#else
	#define COTHREAD_SETJMP(_buf)	setjmp((_buf))
#endif

extern COTHREAD_LINK void COTHREAD_CALL
cothread_attr_init(cothread_attr_t* attr, cothread_stack_t* stack, size_t stack_sz, cothread_cb_t user_cb)
{
	//---Check arguments---//
	assert(NULL	!= attr);
	assert(NULL	!= stack);
	assert(0	!= stack_sz);
	assert(NULL	!= user_cb);

	//---Check the stack alignment---//
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & (uintptr_t)stack));
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & stack_sz));

	//---Initialize---//
	attr->stack		= stack;
	attr->stack_sz	= stack_sz;
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
	const int	ret	= COTHREAD_SETJMP(cothread->current->buf);

	//---Is it the first return from setjmp ?---//
	if (0 == ret) {
		//---Switch the endpoints---//
		cothread->current	= (&(cothread->caller) == cothread->current) ? &(cothread->callee) : &(cothread->caller);
		longjmp(cothread->current->buf, user_val);
	}

	//---Return---//
	return ret;
}

/**
 * @brief		Initializes and runs the specified cothread.
 * @param		[in]	cothread	The cothread to initialize.
 * @param		[in]	attr		The attributes to initialize the cothread with.
 */
extern COTHREAD_LINK_HIDDEN void COTHREAD_CALL
cothread_core(cothread_t* cothread, const cothread_attr_t* attr)
{
	//---Check arguments---//
	assert(NULL	!= cothread);
	assert(NULL	!= attr);

	//---Init---//
	cothread->current	= &(cothread->callee);

	//---Initialize the callee endpoint---//
	cothread_cb_t	user_cb	= attr->user_cb;
	int	user_val	= COTHREAD_SETJMP(cothread->current->buf);
	if (0 != user_val) {
		//---Forget the attributes which are not valid during the 2nd return---//
		attr	= NULL;

		//---Run the user callback---//
		user_val	= user_cb(cothread, user_val);

		//---Jump to the caller---//
		cothread->current	= &(cothread->caller);
		longjmp(cothread->current->buf, user_val);
	}

	//---Return to caller---//
	cothread->current	= &(cothread->caller);
}
