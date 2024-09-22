/**
 * @brief		This file contains a unittest.
 * @file
 */

#include <unittest/unittest.h>

/**
 * @brief		Performs the unittest.
 */
extern COTHREAD_LINK_HIDDEN	void COTHREAD_CALL
unittest2(void)
{
	struct _task_t
	{
		private:
			cothread_stack_t	stack[8 * 1024 * 1024 / sizeof(cothread_stack_t)];
			cothread_t			cothread;
			int					initd;

		protected:	virtual int	main	(int user_val) = 0;

		public:
			virtual int
			yield(int user_val)
			{
				//---Initialize the cothread if not already---//
				if (0 == this->initd) {
					cothread_attr_t	attr;
					cothread_attr_init(&attr, this->stack, sizeof(this->stack), _task_t::user_cb);
					cothread_init(&(this->cothread), &attr);
					cothread_set_user_data(&(this->cothread), this);
					this->initd	= !0;
				}

				return cothread_yield(&(this->cothread), user_val);
			}

		private:
			static int COTHREAD_CALL
			user_cb(cothread_t* cothread, int user_val)
			{
				//---Check arguments---//
				assert(NULL	!= cothread);

				//---Run the main---//
				_task_t*	task	= (_task_t*)cothread_get_user_data(cothread);
				return task->main(user_val);
			}

		public:
			virtual
			~_task_t(void)
			{
			}

		public:
			_task_t(void)
			{
				//---Zero---//
				this->initd	= 0;
			}
	};

	struct _task1_t : public _task_t
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

	struct _task0_t : public _task_t
	{
		private:
			using _base_t	= _task_t;
		public:
			_task1_t*	task1;

		protected:
			virtual int
			main(int user_val) override
			{
				return this->task1->yield(user_val);
			}

		public:
			virtual int
			yield(int user_val) override
			{
				if (NULL == this->task1) this->task1 = new _task1_t{};
				return this->_base_t::yield(user_val);
			}

		public:
			virtual
			~_task0_t(void)
			{
				assert(NULL	!= this->task1);
				delete this->task1;
			}

		public:
			_task0_t(void)
			{
				//---Zero---//
				this->task1	= NULL;
			}
	};

	_task0_t*	task0;
	assert(NULL	!= (task0 = new _task0_t{}));
	assert(200	== task0->yield(100));
	assert(201	== task0->task1->yield(101));
	delete task0;
}
