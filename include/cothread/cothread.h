/**
 * @brief		This file contains the public declarations.
 * @file
 */

#ifndef __COTHREAD_COTHREAD_H__
#define __COTHREAD_COTHREAD_H__

#include <cothread/config.h>
#include <setjmp.h>
#include <stdlib.h>

//---Forward declarations---//
typedef struct _cothread_ep_t	cothread_ep_t;		///< @brief	The cothread endpoint type.
typedef struct _cothread_attr_t	cothread_attr_t;	///< @brief	The cothread attribute type.
typedef struct _cothread_t		cothread_t;			///< @brief	The cothread type.

/**
 * @brief		The cothread endpoint type.
 * @see			@ref COTHREAD_EP_INITIALIZER
 */
struct _cothread_ep_t
{
	jmp_buf		buf;	///< @brief	The execution context.
};

/**
 * @brief		The value to initialize an [endpoint](@ref _cothread_ep_t) with.
 */
#define COTHREAD_EP_INITIALIZER	{ }

/**
 * @brief		The cothread attribute type.
 */
struct _cothread_attr_t
{
	cothread_stack_t*	stack;		///< @brief	The lowest address of the callee stack.
	size_t				stack_sz;	///< @brief	The size of the callee stack, in bytes.
	cothread_ep_t*		caller;		///< @brief	Points the caller endpoint.
	cothread_ep_t*		callee;		///< @brief	Points the callee endpoint.
	int					(COTHREAD_CALL * user_cb)	(cothread_t* cothread, int user_val);	///< @brief	The callee entry point.
};

/**
 * @brief		The cothread type.
 */
struct _cothread_t
{
	cothread_ep_t*		current;	///< @brief	Points the current endpoint.
	cothread_ep_t*		caller;		///< @brief	Points the caller endpoint.
	cothread_ep_t*		callee;		///< @brief	Points the callee endpoint.
	void*				user_data;	///< @brief	Any user data.
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		Initializes the specified cothread.
 * @param		[in]	cothread	The cothread to initialize.
 * @param		[in]	attr		The attributes to initialize the cothread with.
 * @note		Modifying @e attr after calling this function has no effect on the initialized @e cothread.
 */
extern COTHREAD_LINK void		COTHREAD_CALL cothread_init	(cothread_t* cothread, const cothread_attr_t* attr);

/**
 * @brief		Stores the specified user data in the specified cothread.
 * @param		[in]	cothread	The cothread to store the user data in.
 * @param		[in]	user_data	Any user data to store in the cothread.
 */
extern COTHREAD_LINK void		COTHREAD_CALL cothread_set_user_data	(cothread_t* cothread, void* user_data);

/**
 * @brief		Returns the user data stored in the specified cothread.
 * @param		[in]	cothread	The cothread to return the user data stored in.
 * @return		Returns the user data.
 */
extern COTHREAD_LINK void*		COTHREAD_CALL cothread_get_user_data	(const cothread_t* cothread);

/**
 * @brief		Switches from the current endpoint to the other one.
 * @param		[in]	cothread	The cothread to switch.
 * @param		[in]	user_val	Any user value (except zero) to send to the other endpoint.
 * @return		Returns the @e user_val received from the other endpoint.
 */
extern COTHREAD_LINK int		COTHREAD_CALL cothread_yield	(cothread_t* cothread, int user_val);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __COTHREAD_COTHREAD_H__ */
