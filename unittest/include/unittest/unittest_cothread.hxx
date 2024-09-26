/**
 * @brief		This file contains the unittest cothread class declarations.
 * @file
 */

#ifndef __UNITTEST_UNITTEST_COTHREAD_HXX__
#define __UNITTEST_UNITTEST_COTHREAD_HXX__

/**
 * @brief		The unittest cothread class.
 */
struct _unittest_cothread_t
{
	protected:
		virtual	~_unittest_cothread_t	(void);
	protected:
				 _unittest_cothread_t	(void);

	private:
		cothread_stack_t	stack[sizeof(void*) * 1024 * 1024 / sizeof(cothread_stack_t)];	///< @brief	The cothread's stack.
		cothread_t			cothread;	///< @brief	The cothread.
	private:
		static int	COTHREAD_CALL user_cb	(cothread_t* cothread, int user_val);
	protected:
		/**
		 * @brief		The entry point.
		 * @param		[in]	user_val	Any user value (except zero) received from the other endpoint.
		 * @return		Returns any user value (except zero) to send to the other endpoint.
		 */
		virtual	int	main	(int user_val) = 0;
	public:
		int			yield	(int user_val);
};

#endif /* __UNITTEST_UNITTEST_COTHREAD_HXX__ */
