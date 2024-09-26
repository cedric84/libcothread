/**
 * @brief		This file contains the unittest cothread class definitions.
 * @file
 */

#include <unittest/unittest.hxx>

/**
 * @brief		The destructor.
 */
_unittest_cothread_t::~_unittest_cothread_t(void)
{
}

/**
 * @brief		The main constructor.
 */
_unittest_cothread_t::_unittest_cothread_t(void)
{
	//---Initialize the cothread---//
	cothread_attr_t	attr;
	cothread_attr_init(&attr, this->stack, sizeof(this->stack), _unittest_cothread_t::user_cb);
	cothread_init(&(this->cothread), &attr);
	cothread_set_user_data(&(this->cothread), this);
}

/**
 * @brief		The callee entry point.
 * @param		[in]	cothread	The cothread.
 * @param		[in]	user_val	Any user value (except zero) received from the other endpoint.
 * @return		Returns any user value (except zero) to send to the other endpoint.
 */
int COTHREAD_CALL
_unittest_cothread_t::user_cb(cothread_t* cothread, int user_val)
{
	//---Check arguments---//
	assert(NULL	!= cothread);

	//---Run the entry point---//
	return ((_unittest_cothread_t*)cothread_get_user_data(cothread))->main(user_val);
}

/**
 * @brief		Switches from the current endpoint to the other one.
 * @param		[in]	user_val	Any user value (except zero) to send to the other endpoint.
 * @return		Returns the @e user_val received from the other endpoint.
 */
int
_unittest_cothread_t::yield(int user_val)
{
	return cothread_yield(&(this->cothread), user_val);
}
