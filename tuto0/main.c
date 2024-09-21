/**
 * @brief		This file contains an example of application that uses the cothread library.
 * @file
 */

#include <cothread/cothread.h>
#include <assert.h>

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
	printf("\n->%s yields to %s...\n\n", cothread->callee.dbg_name, cothread->caller.dbg_name);
	cothread_yield(cothread, 456);

	//---Return to caller---//
	printf("\n->%s returns to %s...\n\n", cothread->callee.dbg_name, cothread->caller.dbg_name);
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

	//---Initialize the attributes---//
	static cothread_stack_t	stack[8 * 1024 * 1024 / sizeof(cothread_stack_t)];
	cothread_attr_t			attr;
	cothread_attr_init(&attr, stack, sizeof(stack), user_cb);
	cothread_attr_set_dbg_strm(&attr, stdout);	// optional.

	//---Initialize the cothread---//
	static cothread_t	cothread;
	cothread_init(&cothread, &attr);

	//---Yield from caller to callee---//
	printf("\n->%s yields to %s...\n\n", cothread.caller.dbg_name, cothread.callee.dbg_name);
	cothread_yield(&cothread, 123);

	//---Yield from caller to callee---//
	printf("\n->%s yields again to %s...\n\n", cothread.caller.dbg_name, cothread.callee.dbg_name);
	cothread_yield(&cothread, 789);

	//---Return---//
	printf("%s terminated\n", __func__);
	return 0;
}
