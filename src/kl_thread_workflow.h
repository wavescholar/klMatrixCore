 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
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