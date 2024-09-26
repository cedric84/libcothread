/**
 * @brief		This file contains a unittest.
 * @file
 */

#include <unittest/unittest.hxx>

/**
 * @brief		Performs the unittest.
 */
extern COTHREAD_LINK_HIDDEN	void COTHREAD_CALL
unittest3(void)
{
	struct _task1_t : public _unittest_cothread_t
	{
		protected:
			virtual int
			main(int user_val) override
			{
				assert(100	== user_val);
				assert(101	== this->yield(200));
				return 201;
			}
	};

	struct _task0_t : public _unittest_cothread_t
	{
		public:
			_task1_t*	task1;

		protected:
			virtual int
			main(int user_val) override
			{
				return this->task1->yield(user_val);
			}

		public:
			virtual
			~_task0_t(void)
			{
				delete this->task1;
			}

		public:
			_task0_t(void)
			{
				//---Zero---//
				this->task1 = new _task1_t{};
			}
	};

	_task0_t*	task0;
	assert(NULL	!= (task0 = new _task0_t{}));
	assert(200	== task0->yield(100));
	assert(201	== task0->task1->yield(101));
	delete task0;
}
