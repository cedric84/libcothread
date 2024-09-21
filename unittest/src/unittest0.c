/**
 * @brief		This file contains a unittest.
 * @file
 */

#include <unittest/unittest.h>

/// @cond
static cothread_t	cothread_g;
/// @endcond

/**
 * @brief		The callee entry point.
 * @param		[in]	cothread	The cothread.
 * @param		[in]	user_val	Any user value (except zero) received from the other endpoint.
 * @return		Returns any user value (except zero) to send to the other endpoint.
 */
static int COTHREAD_CALL
user_cb(cothread_t* cothread, int user_val)
{
	//---Check arguments---//
	assert(NULL					!= cothread);
	assert(&cothread_g			== cothread);
	assert(&(cothread->callee)	== cothread->current);

	//---Check user data---//
	{
		size_t*	ctr	= (size_t*)cothread_get_user_data(cothread);
		assert(1234	== ctr[0]++);
	}

	//---Switch from callee to caller---//
	assert(100					== user_val);
	assert(102					== cothread_yield(cothread, 101));
	assert(&cothread_g			== cothread);
	assert(&(cothread->callee)	== cothread->current);

	//---Check user data---//
	{
		size_t*	ctr	= (size_t*)cothread_get_user_data(cothread);
		assert(1235	== ctr[0]++);
	}

	//---Return to caller---//
	return 103;
}

/**
 * @brief		Performs the unittest.
 */
extern COTHREAD_LINK_HIDDEN	void COTHREAD_CALL
unittest0(void)
{
	//---Set the cothread attributes---//
	static cothread_stack_t	stack[8 * 1024 * 1024 / sizeof(cothread_stack_t)];
	cothread_attr_t			attr;
	cothread_attr_init(&attr, stack, sizeof(stack), user_cb);

	//---Check stack alignment---//
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & (uintptr_t)attr.stack));
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & attr.stack_sz));

	//---Initialize the cothread---//
	size_t	ctr	= 1234;
	cothread_init(&cothread_g, &attr);
	cothread_set_user_data(&cothread_g, &ctr);
	assert(&(cothread_g.caller)	== cothread_g.current);
	assert(&ctr					== cothread_g.user_data);

	//---Clear the attributes which are now useless---//
	memset(&attr, 0, sizeof(attr));

	//---Switch from caller to callee---//
	assert(101					== cothread_yield(&cothread_g, 100));
	assert(&(cothread_g.caller)	== cothread_g.current);

	//---Switch from caller to callee---//
	assert(103					== cothread_yield(&cothread_g, 102));
	assert(&(cothread_g.caller)	== cothread_g.current);

	//---Check the counter---//
	assert(1236	== ctr);
}
