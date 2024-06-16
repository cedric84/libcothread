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

	//---Log configuration---//
	static const char*	cc_name		= COTHREAD_CC_ID_GCC			== COTHREAD_CC_ID ? "gcc"
									: COTHREAD_CC_ID_CLANG			== COTHREAD_CC_ID ? "clang"
									: COTHREAD_CC_ID_MINGW			== COTHREAD_CC_ID ? "mingw"
									: COTHREAD_CC_ID_CL				== COTHREAD_CC_ID ? "cl"
									: "???";

	static const char*	arch_name	= COTHREAD_ARCH_ID_X86			== COTHREAD_ARCH_ID ? "x86"
									: COTHREAD_ARCH_ID_X86_64		== COTHREAD_ARCH_ID ? "x86_64"
									: "???";

	static const char*	os_name		= COTHREAD_OS_ID_GNU_LINUX		== COTHREAD_OS_ID ? "gnu_linux"
									: COTHREAD_OS_ID_FREEBSD		== COTHREAD_OS_ID ? "freebsd"
									: COTHREAD_OS_ID_MACOS			== COTHREAD_OS_ID ? "macos"
									: COTHREAD_OS_ID_WINDOWS		== COTHREAD_OS_ID ? "windows"
									: "???";
	printf("%*s: %s\n", 16, "compiler", cc_name);
	printf("%*s: %s\n", 16, "architecture", arch_name);
	printf("%*s: %s\n", 16, "operating system", os_name);

	//---Check structure member offsets used in assembly code---//
	if (COTHREAD_ARCH_ID_X86 == COTHREAD_ARCH_ID) {
		assert(0	== (uintptr_t)&(((cothread_attr_t*)0)->stack));
		assert(4	== (uintptr_t)&(((cothread_attr_t*)0)->stack_sz));
	} else if (COTHREAD_ARCH_ID_X86_64 == COTHREAD_ARCH_ID) {
		assert(0	== (uintptr_t)&(((cothread_attr_t*)0)->stack));
		assert(8	== (uintptr_t)&(((cothread_attr_t*)0)->stack_sz));
	} else {
		assert(0);
	}

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

	//---Return---//
	printf("%s terminated\n", __func__);
	return 0;
}
