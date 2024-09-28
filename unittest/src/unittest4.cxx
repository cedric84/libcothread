/**
 * @brief		This file contains a unittest.
 * @file
 */

#include <unittest/unittest.hxx>
#include <exception>

/**
 * @brief		Performs the unittest.
 */
extern COTHREAD_LINK_HIDDEN	void COTHREAD_CALL
unittest4(void)
{
	//---Define a task---//
	struct _task_t : public _unittest_cothread_t
	{
		public:
			size_t	catch_ctr;
			size_t	unwound_ctr;

		public:
			virtual
			~_task_t(void)
			{
			}

		public:
			_task_t(void)
			{
				//---Zero---//
				this->catch_ctr		= 0;
				this->unwound_ctr	= 0;
			}

		private:
			void
			fct0(void)
			{
				try { throw ::std::exception{}; }
				catch (::std::exception&) { assert(2	== this->catch_ctr++); }
			}

		private:
			void
			fct1(void)
			{
				//---Define an object to call the destructor of when the exception will be thrown---//
				struct _dummy_t
				{
					private:
						size_t*	unwound_ctr;
					public:
						virtual
						~_dummy_t(void)
						{ assert(0	== this->unwound_ctr[0]++); }
					public:
						_dummy_t(size_t* unwound_ctr)
						{ this->unwound_ctr	= unwound_ctr; }
				} dummy{&(this->unwound_ctr)};
				(void)dummy;

				//---Throw an exception---//
				throw ::std::exception{};
			}

		protected:
			virtual	int
			main(int user_val) override
			{
				//---Check arguments---//
				assert(100	== user_val);

				//---Catch an exception---//
				try { throw ::std::exception{}; }
				catch (::std::exception&) { assert(1	== this->catch_ctr++); }

				//---Call a method that catches an exception---//
				this->fct0();

				//---Catch an exception thrown by a method---//
				assert(0	== this->unwound_ctr);
				try { this->fct1(); }
				catch (::std::exception&) { assert(3	== this->catch_ctr++); }
				assert(1	== this->unwound_ctr);

				//---Return---//
				return 200;
			}
	};

	//---Create---//
	_task_t*	task;
	assert(NULL	!= (task = new _task_t{}));

	//---Catch an exception---//
	try { throw ::std::exception{}; }
	catch (::std::exception&) { assert(0	== task->catch_ctr++); }

	//---Run the task---//
	assert(200	== task->yield(100));
	assert(4	== task->catch_ctr);

	//---Release memory---//
	delete task;
}
