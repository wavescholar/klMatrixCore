 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#ifndef __kl_util__
#define __kl_util__

#include <windows.h>
#include <map>
#include <assert.h>
#include "float.h"


#define GetBit(var, bit) ((var & (1 << bit)) != 0) // Returns true / false if bit is set 
#define SetBit(var, bit) (var |= (1 << bit)) 
#define FlipBit(var, bit) (var ^= (1 << bit)) 


//Compiler intrinsics for atomic increment and decrement of refrence count
#include <intrin.h>
#pragma intrinsic(__getcallerseflags)
#pragma intrinsic(__readeflags)
#pragma intrinsic(__readeflags)

#include "kl_exception.h"

using namespace std;

//A safe way to use a  mutex or other lock object.  
//The lock object must implement lock() and unlock().
//A guard object is a simple object, its constructor takes 
//a lock as parameter and locks it right in the constructor. 
//The lock is unlocked in the destructor of the guard object. 
//Since in C++ the destructor is automatically called as soon as the object 
//goes out of scope, the lock will always be unlocked as soon as the program 
//leaves the protected code region
template<class TYPE>
class klGuard
{
public:
	klGuard(TYPE &lock) :
	  lockObj(&lock)
	  {
		  lockObj->lock();
	  }
	  ~klGuard(void)
	  {
		  lockObj->unlock();
	  }

private:
	TYPE *lockObj;
};


template <class TYPE_MUTEX>
class klRefCount
{
public:
	inline klRefCount(void) :
	theUseCount(0)
	{ 
	}

	~klRefCount(void)
	{ 
		assert(theUseCount==0);
		theUseCount = -1;
	}
	void addRef(void)
	{ 

#ifdef _M_X64
		long incremented =InterlockedIncrement(&theUseCount);

		//BBC revisit 
		//unsigned int getcallerseflagsVAL= __getcallerseflags();
		//unsigned int readeflagsVAL= __readeflags();
		//unsigned __int64 readeflagsx64VAL = __readeflags();  
#else
		kl_GUARD(TYPE_MUTEX,theMutexLock);
		theUseCount++;
#endif
	}

	bool deleteRef(void)
	{ 
#ifdef _M_X64 //_M_IX86
		bool status;

		long decremented =InterlockedDecrement(&theUseCount);

		//unsigned int getcallerseflagsVAL= __getcallerseflags();
		//unsigned int readeflagsVAL= __readeflags();
		//unsigned __int64 readeflagsx64VAL = __readeflags();  

		status = (theUseCount == 0);

		return status;
#else
		kl_GUARD(TYPE_MUTEX,theMutexLock);
		theUseCount--;
		return theUseCount == 0;
#endif
	}

	klRefCount(const klRefCount &obj) :
	theUseCount(0)      
	{
	}

	klRefCount &operator=(const klRefCount &obj)
	{                                
		return *this;
	}

	long useCount() const {return theUseCount;};

protected:

private:
	long volatile theUseCount; //Added volatile fpr 64 bit mods

#ifndef _M_X64 //_M_IX86
	TYPE_MUTEX theMutexLock;
#endif
};

//mutual exclusion lock for thread synchronization
class klMutex
{
public:

	inline klMutex (void);

	inline ~klMutex (void);

	inline void lock (void);

	inline void unlock (void);
protected:

	CRITICAL_SECTION _lock;


};

inline klMutex::klMutex( void ) 
{ 
	InitializeCriticalSection( &_lock ); 
}

inline klMutex::~klMutex (void)
{
	DeleteCriticalSection( &_lock );
}

inline void klMutex::lock (void)	
{
	EnterCriticalSection( &_lock );
}

inline void klMutex::unlock (void)
{
	LeaveCriticalSection( &_lock );
}


#include <process.h>
typedef unsigned long klThreadId;

class klFlotingPointHelper

{	public:

static unsigned int status_x86();
static unsigned int status_SSE();

static unsigned int Clear87( void );
static unsigned int Clear( void );

static int* fpError();
inline const char* get_SSE_ErrorStringFromStatus(unsigned int status)
{ 
	char* ans=new char[1024];
	string ser;
	if(status & _SW_INEXACT) ser+= "|  inexact (precision)" ;
	if(status & _SW_UNDERFLOW) ser+=  "|  underflow ";
	if(status & _SW_OVERFLOW)  ser+=  "| overflow ";
	if(status & _SW_ZERODIVIDE )ser+=  "| zero divide";
	if(status & _SW_INVALID) ser+=     "|  invalid";
	if(status & _SW_DENORMAL) ser+=     "| denormal status bit";
	if(status &  _EM_AMBIGUIOUS) ser+=     "| for backwards compatibility old spelling ";
	if(status &  _EM_AMBIGUOUS ) ser+=     "| ";
	if(status &  _MCW_EM  ) ser+=     "|  interrupt Exception Masks ";
	if(status & _EM_INEXACT ) ser+=     "|   inexact (precision)";
	if(status &  _EM_UNDERFLOW ) ser+=     "|  underflow ";
	if(status &  _EM_OVERFLOW ) ser+=     "|    overflow ";
	if(status &  _EM_ZERODIVIDE ) ser+=     "|   zero divide ";
	if(status &  _EM_INVALID ) ser+=     "|    invalid ";
	if(status &  _EM_DENORMAL  ) ser+=     "| denormal exception mask (_control87 only) ";
	if(status &  _MCW_RC  ) ser+=     "|  Rounding Control ";
	if(status &  _RC_NEAR ) ser+=     "|    near ";
	if(status &  _RC_DOWN   ) ser+=     "|   down ";
	if(status & _RC_UP  ) ser+=     "|   up ";
	if(status &  _RC_CHOP ) ser+=     "|  chop ";
	if(status & _MCW_PC ) ser+=     "| Precision Control ";
	if(status &  _PC_64   ) ser+=     "|    64 bits ";
	if(status &  _PC_53  ) ser+=     "|    53 bits ";
	if(status &  _PC_24  ) ser+=     "|    24 bits ";
	if(status &  _MCW_IC  ) ser+=     "|  Infinity Control ";
	if(status &  _IC_AFFINE  ) ser+=     "|  affine ";
	if(status &  _MCW_DN  ) ser+=     "|  Denormal Control ";
	if(status &  _DN_SAVE   ) ser+=     "|   save denormal results and operands";
	if(status &  _DN_FLUSH  ) ser+=     "|  flush denormal results and operands to zero ";
	if(status &  _DN_FLUSH_OPERANDS_SAVE_RESULTS  ) ser+=     "|   flush operands to zero and save results ";
	if(status &  _DN_SAVE_OPERANDS_FLUSH_RESULTS ) ser+=     "|    save operands and flush results to zero ";
	sprintf(ans ,"%s",ser.c_str());
	return ans;
}
//user deletes string mem
inline const char* get_x86_ErrorStringFromStatus(unsigned int status)
{ char* ans=new char[1024];
string ser;

if(status & _SW_INEXACT) ser+= "|  inexact (precision)" ;
if(status & _SW_UNDERFLOW) ser+=  "|  underflow ";
if(status & _SW_OVERFLOW)  ser+=  "| overflow ";
if(status & _SW_ZERODIVIDE )ser+=  "| zero divide";
if(status & _SW_INVALID) ser+=     "|  invalid";
if(status & _SW_DENORMAL) ser+=     "| denormal status bit";
sprintf(ans ,"%s",ser.c_str());
return ans;
//if(status & __EM_AMBIGUIOUS return "__EM_AMBIGUIOUS";
//if(status & __EM_AMBIGUOUS) return "__EM_AMBIGUIOUS";
}

inline const char* get_fp_ErrorStringFromStatus(int* status)
{char* ans=new char[1024];
string ser;

if(*status & _SW_UNEMULATED)  ser+=  "| unemulated instruction ";
if(*status & _SW_SQRTNEG)   ser+=  "| square root of a neg number ";
if(*status & _SW_STACKOVERFLOW)       ser+=  "| FP stack overflow ";
if(*status & _SW_STACKUNDERFLOW)       ser+=  "| FP stack underflow ";
if(*status & _FPE_INVALID)  ser+=  "| _FPE_INVALID";
if(*status & _FPE_DENORMAL)  ser+=  "| denorml";
if(*status & _FPE_ZERODIVIDE)  ser+=  "| divide by zero";
if(*status & _FPE_OVERFLOW )  ser+=  "| overflow";
if(*status & _FPE_UNDERFLOW )  ser+=  "| underflow";
if(*status & _FPE_INEXACT )  ser+=  "| inexact";
if(*status & _FPE_UNEMULATED )  ser+=  "| unemulated";
if(*status & _FPE_SQRTNEG   )  ser+=  "| sqrt neg number";
if(*status & _FPE_STACKOVERFLOW )  ser+=  "| stack overflow";
if(*status & _FPE_STACKUNDERFLOW)  ser+=  "| stack underflow";
if(*status & _FPE_EXPLICITGEN )  ser+=  "| raise( SIGFPE )";
sprintf(ans ,"%s",ser.c_str());
return ans;
}

void getAndClearStatus( char* erws)
{
	int* er=fpError();
	unsigned int st86=status_x86();
	unsigned int stSSE=status_SSE();
	const char* x86Status= get_x86_ErrorStringFromStatus(st86);
	string sx86Status=x86Status;
	const char*  fpERr= get_fp_ErrorStringFromStatus(er);
	string sfpERr=fpERr;
	const char*  sseERr= get_SSE_ErrorStringFromStatus(stSSE);
	string ssseERr=sseERr;
	unsigned int clV87=Clear87();
	unsigned int clV=Clear();
	string stat;
	stat+="\n X86 = " ;
	stat+=sx86Status;
	stat+= " \n fp err = ";
	stat+= sfpERr;
	stat+="\n SSE status =";
	stat+=  ssseERr;
	delete x86Status;
	delete fpERr;
	delete sseERr;
	sprintf(erws, "%s \n ",stat.c_str());

}

};

// In order to wait for thread completion, under Win32, we need the
// handle which was returned by beginthread.  Unfortunately, given 
// a thread id, there is no way to get that handle, so we need to 
// maintain the mapping from thread id to handle ourselves.
// This class maintains that mapping for all instantiations of any
// klThread<>.
class klThreadMap 
{

public:

	static void insert (klThreadId id, unsigned long threadHandle)
	{
		klGuard<klMutex> guard(lock);

		threadMap[id] = threadHandle;
	}
	static void remove (klThreadId id)
	{
		klGuard<klMutex> guard(lock);

		map<klThreadId, unsigned long>::iterator i = threadMap.find(id);
		if (i != threadMap.end())
			threadMap.erase (i);
	}
	static unsigned long find (klThreadId id)
	{
		klGuard<klMutex> guard(lock);

		map<klThreadId, unsigned long>::iterator i = threadMap.find(id);
		if (i == threadMap.end())
			return 0;

		return (*i).second;
	}
private:

	static klMutex lock;

	static map<klThreadId, unsigned long> threadMap;
};

template <class TYPE_MUTEX>
class klThread: public klRefCount <TYPE_MUTEX>
{

public:

	enum klThreadState { NOT_STARTED, RUNNING, TERMINATED };


	klThread () : klRefCount <TYPE_MUTEX> (), state(NOT_STARTED), threadId(0) {}

	virtual ~klThread (void)
	{
	}

	//klErrp getError (void) {return runError;};

	klThreadState getState (void) {return state;};

	klThreadId getThreadId (void) {return threadId;};

	//int \in [-2,2]
	bool setPriority (int relative_priority)
	{
		HANDLE id   = (HANDLE)klThreadMap::find(threadId);
		BOOL result = false;

		switch (relative_priority)
		{
		case -2: result = SetThreadPriority (id, THREAD_PRIORITY_LOWEST);
			break;
		case -1: result = SetThreadPriority (id, THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case  0: result = SetThreadPriority (id, THREAD_PRIORITY_NORMAL);
			break;
		case  1: result = SetThreadPriority (id, THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case  2: result = SetThreadPriority (id, THREAD_PRIORITY_HIGHEST);
			break;
		default: 
			break;
		};

		return (result ? true : false);
#ifndef kl_THREADS_OK
		return false;
#endif
	}

	static klThreadId getCurrentThreadId( void )
	{
		// static method returns the thread id of the current thread.
		return GetCurrentThreadId();
	}

	static bool waitForThread (klThreadId id)
	{
		// blocks current thread of execution until specified
		// thread has completed.  the function returns true, 
		// if the wait was successfully accomplished.
		HANDLE hEvent = (HANDLE)klThreadMap::find(id);

		if (!hEvent)
			return true;

		while (true)
		{
			DWORD dwReturn = ::MsgWaitForMultipleObjects(1, &hEvent,
				false, INFINITE, QS_ALLINPUT) ;
			if (dwReturn == WAIT_OBJECT_0)
				return true ;	// Our event happened.
			else if (dwReturn == WAIT_OBJECT_0 + 1)
			{	// Handle message to keep client alive.
				MSG msg ;
				while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					::DispatchMessage(&msg) ;
			}
			else
				return false;
		}

#ifndef kl_THREADS_OK
		return false;
#endif
	}

	inline void launch (void)
	{
		int error = 0;
		addRef();

		unsigned long threadHandle = _beginthreadex((void *)NULL, 0, (unsigned( __stdcall*)(void*))klThread::main, (void *)this, 0, (unsigned *)&threadId);
		if (threadHandle == 0) 
			error = (errno ? errno : -1);
		else
			klThreadMap::insert (threadId, threadHandle);

		if (error)
		{
			// Since the thread did not get created properly, decrement
			// the ref count here so that the thread object will get
			// cleaned up.
			deleteRef();

			state = TERMINATED;

			throw "klThread:launch problem launching posix thread .";
		}
		else
		{
			state = RUNNING;
		}
	}

	//Implemented in derivatives
	virtual void run (void) = 0;


protected:

	// Calls the run() method on its argument.
	static unsigned __stdcall main (void * thread)
	{
		// Create a new smart pointer for the thread object,
		// then decrement the ref count to counter the add ref
		// done in the klThread::launch method (which got the
		// thread object safely across the thread boundary).
		//
		klSmartPtr<klThread> threadPtr =
			reinterpret_cast<klThread*>(thread);

		// The parent thread sets the member variable 'threadId' but if the
		// sets it after launching the child thread then its possible that
		// the child could query its 'threadId' and not have it set.  The
		// simple check below will correct this race condition.
		if (threadPtr->threadId == 0)
			threadPtr->threadId = getCurrentThreadId();

		klThreadId the_thread_id = threadPtr->threadId;

		threadPtr->deleteRef();

		threadPtr->run();

		threadPtr->state = TERMINATED;

		// Zero out the thread pointer so that the thread object
		// can safely go away now that it is no longer in use.
		threadPtr = 0;

		HANDLE threadHandle = (HANDLE)klThreadMap::find(the_thread_id);
		// get a handle to the thread

		klThreadMap::remove(the_thread_id);
		// this could introduce a race condition, whereby our
		// user may think the thread has terminated before we
		// actually execute the endthreadex() call.  hopefully
		// this will not be a problem. for anyone.

		if (threadHandle != (HANDLE)0)
			CloseHandle(threadHandle);

		return 0;

	}

	//klErrp runError;
	klThreadState state;
	klThreadId threadId;
};


//klSmartPtr  should never be used to point at a stack based
// object (otherwise a double delete will happen).  Similarly, it
// should never be used to point at a static object, for it will
// attempt to call "delete" on that object.
//
// Note that klConstSmartPtr is a base class for klSmartPtr
// this is so that proper constness symantics can be followed.  Just
// like a char * can be converted to a const char * a klSmartPtr<foo>
// can be converted to a klConstSmartPtr<foo> (the constness refers to
// the object being pointed at not the klSmartPtr object).
// The template argument for klSmartPtr and klConstSmartPtr 
// must be a class that provides the following member functions:
//     void addRef(void) - add one to the use count for the object.
//     bool deleteRef(void) - subtract one from the use count for the object.  
//                            If the object should be deleted return non-zero.
//  This requirement may be satisfied by basing the class on klRefCount
template <class TYPE> class klConstSmartPtr 
{
public:
	klConstSmartPtr(void): thePtr(0)
	{}

	~klConstSmartPtr(void)
	{
		if (thePtr && thePtr->deleteRef())
		{
			delete thePtr;	
		}
		thePtr = 0;
	}

	klConstSmartPtr(const klConstSmartPtr<TYPE> &smartPtr): thePtr(smartPtr.thePtr)
	{
		if(thePtr)
			thePtr->addRef();
	}

	klConstSmartPtr(const TYPE* smartPtr): thePtr(const_cast<TYPE*>(smartPtr)) 
	{
		if(thePtr)
			thePtr->addRef();
	}


	klConstSmartPtr<TYPE> &operator=(const klConstSmartPtr<TYPE> &smartPtr)
	{
		if (thePtr != smartPtr.thePtr)   
		{
			TYPE* oldPtr = thePtr;  
			thePtr = smartPtr.thePtr;   
			if (thePtr)
				thePtr->addRef();

			if (oldPtr && oldPtr->deleteRef()) 
				delete oldPtr;
		}
		return *this;
	}
	klConstSmartPtr<TYPE> &operator=(const TYPE* smartPtr){
		if (thePtr != smartPtr)       
		{
			TYPE* oldPtr = thePtr;   

			thePtr = const_cast<TYPE*>(smartPtr); 
			if (thePtr)
				thePtr->addRef();

			if (oldPtr && oldPtr->deleteRef()) 
				delete oldPtr;
		}
		return *this;
	}

	const TYPE* operator->(void) const
	{
		return thePtr;
	}

	const TYPE* ptr(void) const
	{
		return thePtr;
	}


	const TYPE& operator*(void) const
	{
		assert(thePtr);             
		return *thePtr;
	}

	bool operator==(const klConstSmartPtr<TYPE> &smartPtr) const
	{
		return thePtr == smartPtr.thePtr;
	}
	bool operator==(const TYPE* smartPtr) const 
	{
		return thePtr == smartPtr;
	}
	bool operator!=(const klConstSmartPtr<TYPE> &smartPtr) const
	{
		return thePtr != smartPtr.thePtr;
	}
	bool operator!=(const TYPE* smartPtr) const
	{
		return thePtr != smartPtr;
	}
	bool operator<(const klConstSmartPtr<TYPE>& smartPtr) const
	{
		return thePtr < smartPtr.thePtr;
	}
	bool operator<(const TYPE* smartPtr) const
	{
		return thePtr < smartPtr;
	}

	operator bool(void) const
	{
		return thePtr != 0;
	}

protected:
	TYPE* thePtr;   
};


template <class TYPE> inline bool operator==(const TYPE* optr, const klConstSmartPtr<TYPE>& sp)
{
	return optr == sp.ptr();
}

template <class TYPE> inline bool operator!=(const TYPE* optr, const klConstSmartPtr<TYPE>& sp)
{
	return optr != sp.ptr();
}

template <class TYPE> inline bool operator<(const TYPE* optr, const klConstSmartPtr<TYPE>& sp)
{
	return optr < sp.ptr();
}

template <class TYPE> class klSmartPtr : public klConstSmartPtr<TYPE>
{
public:
	klSmartPtr(void) : klConstSmartPtr<TYPE>() {};

	klSmartPtr(const klSmartPtr<TYPE> &smartPtr) : klConstSmartPtr<TYPE>(smartPtr){};
	klSmartPtr(TYPE* smartPtr) : klConstSmartPtr<TYPE>(smartPtr){};

	klSmartPtr<TYPE> &operator=(const klSmartPtr<TYPE> &smartPtr)
	{
		klConstSmartPtr<TYPE>::operator=(smartPtr);
		return *this;
	}
	klSmartPtr<TYPE> &operator=(TYPE* smartPtr)
	{
		klConstSmartPtr<TYPE>::operator=(smartPtr);
		return *this;
	}

	TYPE* operator->(void) const
	{
		return thePtr;
	}
	TYPE* ptr(void) const
	{
		return thePtr;
	}
	TYPE& operator*(void) const
	{
		assert(thePtr);             
		return *thePtr;
	}
};

#endif  //#ifndef __kl_util__