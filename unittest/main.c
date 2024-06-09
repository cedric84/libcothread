/**
 * @brief		This file contains the unittest application.
 * @file
 */

#include <cothread/cothread.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

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
	assert(NULL				!= cothread);
	assert(&cothread_g		== cothread);
	assert(cothread->callee	== cothread->current);

	//---Check user data---//
	{
		size_t*	ctr	= (size_t*)cothread_get_user_data(cothread);
		assert(1234	== ctr[0]++);
	}

	//---Switch from callee to caller---//
	assert(100				== user_val);
	assert(102				== cothread_yield(cothread, 101));
	assert(&cothread_g		== cothread);
	assert(cothread->callee	== cothread->current);

	//---Check user data---//
	{
		size_t*	ctr	= (size_t*)cothread_get_user_data(cothread);
		assert(1235	== ctr[0]++);
	}

	//---Return to caller---//
	return 103;
}

/**
 * @brief		The application entry point.
 * @param		[in]	argc	The number of arguments.
 * @param		[in]	argv	The arguments values.
 * @return		Returns zero on success.
 */
extern int
main(int argc, char* argv[])
{
	//---Log---//
	printf("%s started\n", __func__);

	//---Check structure member offsets---//
	if (COTHREAD_ARCH_ID_X86 == COTHREAD_ARCH_ID) {
		assert(0	== (uintptr_t)&(((cothread_ep_t*)0)->buf));

		assert(0	== (uintptr_t)&(((cothread_attr_t*)0)->stack));
		assert(4	== (uintptr_t)&(((cothread_attr_t*)0)->stack_sz));
		assert(8	== (uintptr_t)&(((cothread_attr_t*)0)->caller));
		assert(12	== (uintptr_t)&(((cothread_attr_t*)0)->callee));
		assert(16	== (uintptr_t)&(((cothread_attr_t*)0)->user_cb));

		assert(0	== (uintptr_t)&(((cothread_t*)0)->current));
		assert(4	== (uintptr_t)&(((cothread_t*)0)->caller));
		assert(8	== (uintptr_t)&(((cothread_t*)0)->callee));
	} else if (COTHREAD_ARCH_ID_X86_64 == COTHREAD_ARCH_ID) {
		assert(0	== (uintptr_t)&(((cothread_ep_t*)0)->buf));

		assert(0	== (uintptr_t)&(((cothread_attr_t*)0)->stack));
		assert(8	== (uintptr_t)&(((cothread_attr_t*)0)->stack_sz));
		assert(16	== (uintptr_t)&(((cothread_attr_t*)0)->caller));
		assert(24	== (uintptr_t)&(((cothread_attr_t*)0)->callee));
		assert(32	== (uintptr_t)&(((cothread_attr_t*)0)->user_cb));

		assert(0	== (uintptr_t)&(((cothread_t*)0)->current));
		assert(8	== (uintptr_t)&(((cothread_t*)0)->caller));
		assert(16	== (uintptr_t)&(((cothread_t*)0)->callee));
	} else {
		assert(0);
	}

	//---Set the cothread attributes---//
	static cothread_ep_t	caller_ep;
	static cothread_ep_t	callee_ep;
	static cothread_stack_t	stack[8 * 1024 * 1024 / sizeof(cothread_stack_t)];
	cothread_attr_t			attr	= {
		.stack		= stack,
		.stack_sz	= sizeof(stack),
		.caller		= &caller_ep,
		.callee		= &callee_ep,
		.user_cb	= user_cb,
	};

	//---Check stack alignment---//
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & (uintptr_t)attr.stack));
	assert(0	== ((COTHREAD_STACK_ALIGN - 1) & attr.stack_sz));

	//---Initialize the cothread---//
	size_t	ctr	= 1234;
	cothread_init(&cothread_g, &attr);
	cothread_set_user_data(&cothread_g, &ctr);
	assert(&caller_ep	== cothread_g.current);
	assert(&caller_ep	== cothread_g.caller);
	assert(&callee_ep	== cothread_g.callee);
	assert(&ctr			== cothread_g.user_data);

	//---Clear the attributes which are now useless---//
	memset(&attr, 0, sizeof(attr));

	//---Switch from caller to callee---//
	assert(101			== cothread_yield(&cothread_g, 100));
	assert(&caller_ep	== cothread_g.current);

	//---Switch from caller to callee---//
	assert(103			== cothread_yield(&cothread_g, 102));
	assert(&caller_ep	== cothread_g.current);

	//---Check the counter---//
	assert(1236	== ctr);

	//---Return---//
	printf("%s terminated\n", __func__);
	return 0;
}
