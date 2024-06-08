/**
 * @brief		This file contains the current configuration.
 * @file
 */

#ifndef __COTHREAD_CONFIG_H__
#define __COTHREAD_CONFIG_H__

#define COTHREAD_CC_ID_GCC			1	///< @brief	The GNU C compiler identifier.
#define COTHREAD_CC_ID_CLANG		2	///< @brief	The LLVM Clang compiler identifier.
#define COTHREAD_CC_ID_MINGW		3	///< @brief	The MinGW compiler identifier.
#define COTHREAD_CC_ID_CL			4	///< @brief	The Microsoft Visual Studio compiler identifier.

#define COTHREAD_ARCH_ID_X86_64		1	///< @brief	The x86_64 architecture identifier.

#define COTHREAD_OS_ID_GNU_LINUX	1	///< @brief	The GNU/Linux operating system identifier.
#define COTHREAD_OS_ID_FREEBSD		2	///< @brief	The FreeBSD operating system identifier.
#define COTHREAD_OS_ID_MACOS		3	///< @brief	The macOS operating system identifier.
#define COTHREAD_OS_ID_WINDOWS		4	///< @brief	The Windows operating system identifier.

#if		(defined(__GNUC__) && !defined(__clang__) && !defined(__MINGW32__))
	#define	COTHREAD_CC_ID	COTHREAD_CC_ID_GCC
#elif	(defined(__clang__))
	#define	COTHREAD_CC_ID	COTHREAD_CC_ID_CLANG
#elif	(defined(__MINGW32__))
	#define	COTHREAD_CC_ID	COTHREAD_CC_ID_MINGW
#elif	(defined(_MSC_VER))
	#define	COTHREAD_CC_ID	COTHREAD_CC_ID_CL
#else
	#error	"compiler is not detected."
#endif

#if		((COTHREAD_CC_ID_GCC == COTHREAD_CC_ID) && defined(__x86_64__))
	#define	COTHREAD_ARCH_ID	COTHREAD_ARCH_ID_X86_64
#elif	((COTHREAD_CC_ID_CLANG == COTHREAD_CC_ID) && defined(__x86_64__))
	#define	COTHREAD_ARCH_ID	COTHREAD_ARCH_ID_X86_64
#elif	((COTHREAD_CC_ID_MINGW == COTHREAD_CC_ID) && defined(__x86_64__))
	#define	COTHREAD_ARCH_ID	COTHREAD_ARCH_ID_X86_64
#elif	((COTHREAD_CC_ID_CL == COTHREAD_CC_ID) && defined(_M_AMD64))
	#define	COTHREAD_ARCH_ID	COTHREAD_ARCH_ID_X86_64
#else
	#error	"architecture is not detected."
#endif

#if		((COTHREAD_CC_ID_GCC == COTHREAD_CC_ID) && defined(__gnu_linux__))
	#define	COTHREAD_OS_ID		COTHREAD_OS_ID_GNU_LINUX
#elif	((COTHREAD_CC_ID_CLANG == COTHREAD_CC_ID) && defined(__FreeBSD__))
	#define	COTHREAD_OS_ID		COTHREAD_OS_ID_FREEBSD
#elif	((COTHREAD_CC_ID_CLANG == COTHREAD_CC_ID) && defined(__APPLE__) && defined(__MACH__) && defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__))
	#define	COTHREAD_OS_ID		COTHREAD_OS_ID_MACOS
#elif	((COTHREAD_CC_ID_MINGW == COTHREAD_CC_ID) && defined(_WIN32))
	#define	COTHREAD_OS_ID		COTHREAD_OS_ID_WINDOWS
#elif	((COTHREAD_CC_ID_CL == COTHREAD_CC_ID) && defined(_WIN32))
	#define	COTHREAD_OS_ID		COTHREAD_OS_ID_WINDOWS
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
#elif	(!0	\
		&& (COTHREAD_CC_ID_CLANG		== COTHREAD_CC_ID)		\
		&& (COTHREAD_ARCH_ID_X86_64		== COTHREAD_ARCH_ID)	\
		&& (COTHREAD_OS_ID_FREEBSD		== COTHREAD_OS_ID)		\
		)
	#define	COTHREAD_LINK_HIDDEN	__attribute__ ((visibility ("hidden")))
	#define COTHREAD_CALL			__attribute__ ((sysv_abi))
	typedef	__uint128_t				cothread_stack_t;
#elif	(!0	\
		&& (COTHREAD_CC_ID_CLANG		== COTHREAD_CC_ID)		\
		&& (COTHREAD_ARCH_ID_X86_64		== COTHREAD_ARCH_ID)	\
		&& (COTHREAD_OS_ID_MACOS		== COTHREAD_OS_ID)		\
		)
	#define	COTHREAD_LINK_HIDDEN	__attribute__ ((visibility ("hidden")))
	#define COTHREAD_CALL			__attribute__ ((sysv_abi))
	typedef	__uint128_t				cothread_stack_t;
#elif	(!0	\
		&& (COTHREAD_CC_ID_MINGW		== COTHREAD_CC_ID)		\
		&& (COTHREAD_ARCH_ID_X86_64		== COTHREAD_ARCH_ID)	\
		&& (COTHREAD_OS_ID_WINDOWS		== COTHREAD_OS_ID)		\
		)
	#define	COTHREAD_LINK_HIDDEN
	#define COTHREAD_CALL			__attribute__ ((ms_abi))
	typedef	__uint128_t				cothread_stack_t;
#elif	(!0	\
		&& (COTHREAD_CC_ID_CL			== COTHREAD_CC_ID)		\
		&& (COTHREAD_ARCH_ID_X86_64		== COTHREAD_ARCH_ID)	\
		&& (COTHREAD_OS_ID_WINDOWS		== COTHREAD_OS_ID)		\
		)
	#define	COTHREAD_LINK_HIDDEN
	#define COTHREAD_CALL
	typedef	__declspec(align(16)) struct _cothread_stack_t { char buf; }	cothread_stack_t;
#else
	#error	"configuration is not supported."
#endif

#ifndef COTHREAD_LINK
	#define	COTHREAD_LINK	COTHREAD_LINK_HIDDEN
#endif

#endif /* __COTHREAD_CONFIG_H__ */
