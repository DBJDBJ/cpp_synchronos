#ifndef DBJ_SYNHRO_FM
#define DBJ_SYNHRO_FM 0xFF

/*
(c) DBJ.ORG
As far as code in this file is concerned, MIT licence applies for everybody, everywhere and all the time.

Copyright (C) 2012-2013 by Dusan B. Jovanovic (dbj@dbj.org)
 
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

*/
/*
#if WINVER > _WIN32_WINNT_WIN7
#include <synchapi.h>
#else
#include <winbase.h>
#endif
*/
#include <Windows.h>
#include <queue>

#ifdef _DEBUG
#include <crtdbg.h>
#define ASSERT(expression) _ASSERTE(expression)
#define VERIFY(expression) ASSERT(expression)
#define VERIFY_(expected, expression) ASSERT(expected == expression)
#else
#define ASSERT(expression) ((void)0)
#define VERIFY(expression) (expression)
#define VERIFY_(expected, expression) (expression)
#endif
namespace dbj {
/*
What is FM ? FM is Foundation mechanisma
Why FM ? FM is between library and framework. 
Not low enough to be in the library, and not high to be a framework.
*/
	namespace fm {
		class CSLock
			{
				CRITICAL_SECTION h;
				/* no-copying measures */
				CSLock(CSLock const &);
				CSLock const & operator=(CSLock const &);
			public:
				CSLock()				{					InitializeCriticalSection(&h);			}
				~CSLock()				{					DeleteCriticalSection(&h);				}
				void enter()			{					EnterCriticalSection(&h);				}
				bool try_enter()		{					return 0 != TryEnterCriticalSection(&h);}
				void exit()				{					LeaveCriticalSection(&h);				}
				std::auto_ptr<CRITICAL_SECTION> handle()	{	return std::auto_ptr<CRITICAL_SECTION>(&h);		}
		};
		/*
		*/
			class MULock	{	  
				HANDLE h;	  
				MULock(MULock const &);	  
				MULock const & operator=(MULock const &);	
			public:	  
				MULock() :	    
					h(CreateMutex(nullptr, false, nullptr))	  {	   
						ASSERT(h);	  }	 
				~MULock()	  {	   
					VERIFY(CloseHandle(h));	  }	 
				void enter()	  {	    
					VERIFY_(WAIT_OBJECT_0, WaitForSingleObject(h, INFINITE));	  
				}	  
				bool try_enter()	  {	    
					return WAIT_OBJECT_0 == WaitForSingleObject(h, 0);	   
				}	  
				void exit()	  {	    VERIFY(ReleaseMutex(h));	  }	 
				std::auto_ptr<HANDLE> handle()	  {	    return std::auto_ptr<HANDLE>(&h);	  }	
			};
			/**/
			class event
			{
				HANDLE h;
				event(event const &);
				event const & operator=(event const &);
			public:
				explicit event(bool manual = false) :
				h(CreateEvent(nullptr, manual, false, nullptr))
							  {	    ASSERT(h);				  }	  
				~event()	  {	 	VERIFY(CloseHandle(h));	  }	  
				void set()	  {	    VERIFY(SetEvent(h));	  }	  
				void clear()  {	    VERIFY(ResetEvent(h));	  }	  
				void wait()	  {	    VERIFY_(WAIT_OBJECT_0, WaitForSingleObject(h, INFINITE));	  }	
				std::auto_ptr<HANDLE> handle () {  return std::auto_ptr<HANDLE>(&h); }
			};
			/*
			The recommended locking primitive
			*/
			class SRWLock		{		  
				SRWLOCK h;		  
				SRWLock(SRWLock const &);		  
				SRWLock const & operator=(SRWLock const &);		
			public:		  
				SRWLock()		  {		    InitializeSRWLock(&h);		  }			  
				void enter()	  {		    AcquireSRWLockExclusive(&h);  }			  
				bool try_enter()  {		    return 	 0 != TryAcquireSRWLockExclusive(&h);  }			  
				void exit()		  {		    ReleaseSRWLockExclusive(&h);  }			  
				std::auto_ptr<SRWLOCK> handle() {  return std::auto_ptr<SRWLOCK>(&h); }			
			};
			/*
			*/
		
			template <class LT> class lock_block ; 

		// template<class LT = SRWLock>
		class condition_variable	{
		public:	  
			
			//typedef LT lock_type ;
			//typedef lock_block<lock_type> lock_block_type ;

			condition_variable()	  {	    InitializeConditionVariable(&h);	  
			}	  
			
			template <typename T, typename locker_type>	  
			void wait_while( locker_type & locker , T & predicate)	  
			{	    
				while (predicate())	    
				{
					VERIFY(
						SleepConditionVariableSRW(
						&h,
						reinterpret_cast<PSRWLOCK>(locker.handle().get()), 
						INFINITE, 0)
						);	    
				}	  
			}	  
			void wake_one()	  {	    WakeConditionVariable(&h);	  }	  
			void wake_all()	  {	    WakeAllConditionVariable(&h);	  }	
		private:
			CONDITION_VARIABLE h;	  
			condition_variable(condition_variable const &);	  
			condition_variable const & operator=(condition_variable const &);	
		};

		/*
		Deterministic Locking helper
		*/
		template <class LT>
		class lock_block {
		public :
			typedef LT lock_type ; 
			lock_block () 
			{ 
				lock_.enter(); 
			}
			~lock_block () {
				lock_.exit() ;
			}
			lock_type & locker () { return lock_ ; }
		private:
			lock_type lock_ ;
			lock_block( const lock_block & ) ;
		    lock_block const & operator=(lock_block const &);	
		};
		
/*
Condition Variable Blocking Queue
*/

		template <class T>	class blocking_queue	
		{
		std::deque<T> q;	  
		
		condition_variable cv;

		typedef lock_block<SRWLock> locker_type ;

		blocking_queue(blocking_queue const &);	  
		blocking_queue const & operator=(blocking_queue const &);	
	public:	  
		blocking_queue()	  {	  }	  
		void push(T const & value)	  {	    
			locker_type block ;	    
			q.push_back(value);	    
			cv.wake_one();	  
		}	  
		T pop()	  {	    
			locker_type block ;	    
			cv.wait_while( block.locker(), [&]() {   return q.empty(); });	    
			T v = q.front();	    
			q.pop_front();	    
			return v;	  
		}	
	}
	;
	/*eof FM */
	}
	/*eof DBJ */
}
#endif