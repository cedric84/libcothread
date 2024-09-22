/**
 * @brief		This file contains the unittest application.
 * @file
 */

#include <unittest/unittest.h>

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

	//---Check stack size rounding macro---//
	assert((0 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(0 * COTHREAD_STACK_ALIGN + 0));
	assert((1 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(0 * COTHREAD_STACK_ALIGN + 1));
	assert((1 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(1 * COTHREAD_STACK_ALIGN - 1));
	assert((1 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(1 * COTHREAD_STACK_ALIGN + 0));
	assert((2 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(1 * COTHREAD_STACK_ALIGN + 1));
	assert((2 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(2 * COTHREAD_STACK_ALIGN - 1));
	assert((2 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(2 * COTHREAD_STACK_ALIGN + 0));
	assert((3 * COTHREAD_STACK_ALIGN)	== COTHREAD_ROUND_STACK_SZ(2 * COTHREAD_STACK_ALIGN + 1));

	//---Performs the unittests---//
	unittest0();
	unittest1();
	unittest2();

	//---Return---//
	printf("%s terminated\n", __func__);
	return 0;
}
