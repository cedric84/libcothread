/**
 * @brief		This file contains the current configuration.
 * @file
 */

#ifndef __COTHREAD_CONFIG_H__
#define __COTHREAD_CONFIG_H__

#define COTHREAD_CC_ID_GCC			1	///< @brief	The GNU C compiler identifier.

#define COTHREAD_ARCH_ID_X86_64		1	///< @brief	The x86_64 architecture identifier.

#define COTHREAD_OS_ID_GNU_LINUX	1	///< @brief	The GNU/Linux operating system identifier.

#if		(defined(__GNUC__))
	#define	COTHREAD_CC_ID	COTHREAD_CC_ID_GCC
#else
	#error	"compiler is not detected."
#endif

#if		((COTHREAD_CC_ID_GCC == COTHREAD_CC_ID) && defined(__x86_64__))
	#define	COTHREAD_ARCH_ID	COTHREAD_ARCH_ID_X86_64
#else
	#error	"architecture is not detected."
#endif

#if		((COTHREAD_CC_ID_GCC == COTHREAD_CC_ID) && defined(__gnu_linux__))
	#define	COTHREAD_OS_ID		COTHREAD_OS_ID_GNU_LINUX
#else
	#error	"operating system is not detected."
#endif







#if		(!0	\
		&& (COTHREAD_CC_ID_GCC			== COTHREAD_CC_ID)		\
		&& (COTHREAD_ARCH_ID_X86_64		== COTHREAD_ARCH_ID)	\
		&& (COTHREAD_OS_ID_GNU_LINUX	== COTHREAD_OS_ID)		\
		)
	#define	COTHREAD_LINK_HIDDEN	__attribute__ ((visibility ("hidden")))
	#define COTHREAD_CALL			__attribute__ ((sysv_abi))
	typedef	__uint128_t				cothread_stack_t;
#else
	#error	"configuration is not supported."
#endif

#ifndef COTHREAD_LINK
	#define	COTHREAD_LINK	COTHREAD_LINK_HIDDEN
#endif

#endif /* __COTHREAD_CONFIG_H__ */
