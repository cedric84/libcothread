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
	// MSVCRT's setjmp has two arguments, see the setjmp macro.
	// Placing zero in the second one prevent the CRT to unwind the stack during a longjmp.
	// (This Microsoft specific feature seems to be used to handle the "finally" statement of a try-catch-finally.)

	#if		(COTHREAD_CC_ID_MINGW == COTHREAD_CC_ID)
		#define COTHREAD_SETJMP(_buf)	_setjmp((_buf), 0)
	#elif	(COTHREAD_CC_ID_CL == COTHREAD_CC_ID)
		extern int	__intrinsic_setjmp	(jmp_buf, void*);
		#define COTHREAD_SETJMP(_buf)	__intrinsic_setjmp((_buf), 0)
	#endif
#else
	#define COTHREAD_SETJMP(_buf)	setjmp((_buf))
#endif

#define COTHREAD_LOGF(_cothread, _fmt, ...)	{													\
	const cothread_t*	_cothd	= (_cothread);													\
	if (NULL != _cothd->dbg_strm) {																\
		fprintf(_cothd->dbg_strm, "%s: " _fmt "\n", _cothd->current->dbg_name, __VA_ARGS__);	\
	}																							\
}

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
	attr->stack				= stack;
	attr->stack_sz			= stack_sz;
	attr->user_cb			= user_cb;
	//
	attr->dbg_caller_name	= NULL;
	attr->dbg_callee_name	= NULL;
	attr->dbg_strm			= NULL;
}

extern COTHREAD_LINK void COTHREAD_CALL
cothread_attr_set_dbg_caller_name(cothread_attr_t* attr, const char* name)
{
	assert(NULL	!= attr);
	attr->dbg_caller_name	= name;
}

extern COTHREAD_LINK void COTHREAD_CALL
cothread_attr_set_dbg_callee_name(cothread_attr_t* attr, const char* name)
{
	assert(NULL	!= attr);
	attr->dbg_callee_name	= name;
}

extern COTHREAD_LINK void COTHREAD_CALL
cothread_attr_set_dbg_strm(cothread_attr_t* attr, FILE* strm)
{
	assert(NULL	!= attr);
	attr->dbg_strm	= strm;
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
	COTHREAD_LOGF(cothread, "%s", "saving endpoint");
	const int	ret	= COTHREAD_SETJMP(cothread->current->buf);

	//---Is it the first return from setjmp ?---//
	if (0 == ret) {
		//---Switch the endpoints---//
		COTHREAD_LOGF(cothread, "%s", "yielding");
		cothread->current	= (&(cothread->caller) == cothread->current) ? &(cothread->callee) : &(cothread->caller);
		COTHREAD_LOGF(cothread, "%s", "resuming");
		longjmp(cothread->current->buf, user_val);
	}

	//---Return---//
	COTHREAD_LOGF(cothread, "%s", "endpoint restored");
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
	//---Definitions---//
	static const char	dbg_caller_name_default[]	= "caller";
	static const char	dbg_callee_name_default[]	= "callee";

	//---Check arguments---//
	assert(NULL	!= cothread);
	assert(NULL	!= attr);

	//---Init---//
	cothread->current			= &(cothread->callee);
	cothread->caller.dbg_name	= NULL != attr->dbg_caller_name ? attr->dbg_caller_name : dbg_caller_name_default;
	cothread->callee.dbg_name	= NULL != attr->dbg_callee_name ? attr->dbg_callee_name : dbg_callee_name_default;
	cothread->dbg_strm			= attr->dbg_strm;

	//---Initialize the callee endpoint---//
	cothread_cb_t	user_cb	= attr->user_cb;
	COTHREAD_LOGF(cothread, "%s", "initializing endpoint");
	int	user_val	= COTHREAD_SETJMP(cothread->current->buf);
	if (0 != user_val) {
		//---Forget the attributes which are not valid during the 2nd return---//
		attr	= NULL;

		//---Run the user callback---//
		COTHREAD_LOGF(cothread, "%s", "starting user callback");
		user_val	= user_cb(cothread, user_val);
		COTHREAD_LOGF(cothread, "%s", "user callback returned");

		//---Jump to the caller---//
		COTHREAD_LOGF(cothread, "complete, returning to %s", cothread->caller.dbg_name);
		cothread->current	= &(cothread->caller);
		longjmp(cothread->current->buf, user_val);
	}

	//---Return to caller---//
	COTHREAD_LOGF(cothread, "%s", "initialized");
	cothread->current	= &(cothread->caller);
	COTHREAD_LOGF(cothread, "%s", "initialized");
}
