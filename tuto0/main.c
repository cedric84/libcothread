/**
 * @brief		This file contains an example of application that uses the cothread library.
 * @file
 */

#include <cothread/cothread.h>
#include <assert.h>
#include <stdio.h>

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
	assert(NULL		!= cothread);

	//---Yield from callee to caller---//
	printf("callee yields to caller...\n");
	cothread_yield(cothread, 456);

	//---Return to caller---//
	printf("callee returns to caller...\n");
	return 0;
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

	//---Initialize the cothread---//
	static cothread_ep_t	caller_ep	= COTHREAD_EP_INITIALIZER;
	static cothread_ep_t	callee_ep	= COTHREAD_EP_INITIALIZER;
	static cothread_stack_t	stack[8 * 1024 * 1024 / sizeof(cothread_stack_t)];
	cothread_attr_t			attr	= {
		.stack		= stack,
		.stack_sz	= sizeof(stack),
		.caller		= &caller_ep,
		.callee		= &callee_ep,
		.user_cb	= user_cb,
	};
	cothread_t	cothread;
	cothread_init(&cothread, &attr);

	//---Yield from caller to callee---//
	printf("caller yields to callee...\n");
	cothread_yield(&cothread, 123);

	//---Yield from caller to callee---//
	printf("caller yields again to callee...\n");
	cothread_yield(&cothread, 789);

	//---Return---//
	printf("%s terminated\n", __func__);
	return 0;
}
