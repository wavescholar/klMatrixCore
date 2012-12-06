 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#ifndef __kl_thread_workflow__
#define __kl_thread_workflow__

#include "windows.h"
#define KL_OPERATOR_NEW
#include "kl_memory.h"
#include "kl_matrix.h"
#include "kl_util.h"

//These are in main.cpp or kl_thread_workflow.cpp depending on distribution
//DWORD gdwTlsIndex;
//klMutex klThreadMap::lock;
//map<klThreadId, unsigned long> klThreadMap::threadMap;

class klUnitTestThread :public klThread<klMutex>
{
public:
    klUnitTestThread(void (*pf)(void) )
    {
        fPtr=pf;
    }
    void run()
    {
        fPtr();
    }
    void (*fPtr)(void);
	
	string arg;
};

typedef klSmartPtr<klUnitTestThread >	         klUnitTestThreadPtr;
extern DWORD gdwTlsIndex;

//The first parameter is used in a map for keeping track of thread objects.
//The second parameter is of type klThread or a derivative of klThread
template <class TYPE_INDEX,class TYPE_THREAD> class klThreadWorkflow//<TYPE_INDEX,TYPE_THREAD>
{
public:
    klThreadWorkflow() : _noWait(false) {};

    void insertWorkElement(TYPE_INDEX index,TYPE_THREAD thread)
    {
        map<TYPE_INDEX,TYPE_THREAD>::value_type element(index, thread);
        _workflow.insert(element);
    }

    //If set to false, doWork waits for all threads to complete before returning
    void setNoWait(bool noWait)
    { 
        _noWait=noWait;
    }

    virtual void doWork()
    {
        map<TYPE_INDEX,TYPE_THREAD>::iterator val;
        for(val=_workflow.begin();val!=_workflow.end();++val)
        {
            (*val).second->launch();
        }

        //Wait here for all threads unless user sets noWait
        if(!_noWait)
        {
            for(val=_workflow.begin();val!=_workflow.end();++val)
            {
                klThreadId current=(*val).second->getCurrentThreadId();
                cout<<"waiting for thread: "<<current<<endl;//<<val<<endl;
                //klUnitTestThreadPtr v=(*val).second; 
				TYPE_THREAD v = (*val).second; 
                klThreadId theid=v->getThreadId();
                v->waitForThread(theid);
            }
        }
    }
private:

    bool _noWait;

    //Create function pointer map of workflow
    map<TYPE_INDEX,TYPE_THREAD> _workflow;
};

#endif //__kl_thread_workflow__