#include "windows.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#define KL_OPERATOR_NEW
#include "kl_memory.h"
#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_util.h"
#include "kl_unit_tests.h"

#include <iostream>
#include <ostream>
#include <fstream>
using namespace std;

#pragma warning (once : 4244)
#pragma warning (once : 4305)

extern const char* kl_base_dir;//="";
extern const char* basefilename;//="";

void  klRunWorkFlow(void );

void runKLLargeDataTests(void );

//DWORD gdwTlsIndex;

void __cdecl klNewHandler( )
{
	cout << "The new_handler is called:" << endl;
	char* arg=new char[256];
	sprintf(arg,"%s/ErrorDumpModules.txt",kl_base_dir);
	klPrintModules(arg);
	sprintf(arg,"%s/ErrorDumpMemoryInfo.txt",kl_base_dir);
	klWinMemoryInfo(arg);
	sprintf(arg,"%s/ErrorDumpSystemInfo.txt",kl_base_dir);
	klWinSystemInfo(arg);
	delete arg;
	throw bad_alloc( );
}

extern DWORD gdwTlsIndex;

//These are in main.cpp or kl_thread_workflow.cpp depending on distribution
//klMutex klThreadMap::lock;
//map<klThreadId, unsigned long> klThreadMap::threadMap;

#include "kl_thread_workflow.h"

void work1l(void)
{
	//Thread Local Storage
	LPVOID lpvBuffer;
	lpvBuffer = (LPVOID) LocalAlloc(LPTR, 256);
	TlsSetValue(gdwTlsIndex, lpvBuffer);

	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();

	//Work for this thread 
	testKLMultiVariateRandomNumberGenerator2<double>("klDemo.tex");	

	lpvBuffer = TlsGetValue(gdwTlsIndex);
	LocalFree((HLOCAL) lpvBuffer);
}
void work2l(void)
{
	//THread Local Storage
	LPVOID lpvBuffer;
	lpvBuffer = (LPVOID) LocalAlloc(LPTR, 256);
	TlsSetValue(gdwTlsIndex, lpvBuffer);
	unsigned int i;

	//Work for this thread
	testKLMultiVariateRandomNumberGenerator2<double>("klDemo.tex");	

	lpvBuffer = TlsGetValue(gdwTlsIndex);
	LocalFree((HLOCAL) lpvBuffer);
}

void klRunWorkFlow(void)
{
	klFlotingPointHelper mFPH;
	char* FP_errStatus= new char [2048];
	mFPH.getAndClearStatus(FP_errStatus);

	gdwTlsIndex = TlsAlloc();

	set_new_handler (klNewHandler);

	mFPH.getAndClearStatus(FP_errStatus);
	printf(FP_errStatus);

	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);
	QueryPerformanceCounter(prefCountStart);

	QueryPerformanceCounter(prefCountEnd);
	cout<<"QueryPerformanceFrequency  =  "<<freq->QuadPart<<endl;   // current frequency in counts/sec
	cout<<"QueryPerformanceCounter init =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	HANDLE	 hProcess=GetCurrentProcess ();

	klCreateLowMemoryResourceNotification();
	int* answer =new int;
	bool status=klQueryLowMemoryResourceNotification(answer);

	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();

	mFPH.getAndClearStatus(FP_errStatus);	

	klThreadWorkflow<int,klUnitTestThreadPtr> workflow;
	void (*pf)() =work1l;
	klSmartPtr<klUnitTestThread> element=new klUnitTestThread(pf);
	workflow.insertWorkElement(1, element);
	pf=work2l;
	element=new klUnitTestThread(pf);
	workflow.insertWorkElement(2, element);
	workflow.doWork();

	flushall();

	delete FP_errStatus;
	delete answer;
}

void klMainUnitTestDriver()
{
	gdwTlsIndex = TlsAlloc();

	set_new_handler (klNewHandler);

	char* evalString=new char[256];

	sprintf(evalString,"%skl_unit_test_performance.txt",kl_base_dir);

	//Performance Data 
	ofstream performance(evalString );
	//
	//Redirecting stdout to a file
	FILE *stream ;
	sprintf(evalString,"%skl_std_out.txt",kl_base_dir);
	if((stream = freopen(evalString, "w", stdout)) == NULL)
		throw "klMain: error redirecting std::cout to a file.";
	printf("  KL stdout\n");

	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);
	QueryPerformanceCounter(prefCountStart);

	QueryPerformanceCounter(prefCountEnd);
	performance<<"QueryPerformanceFrequency  =  "<<freq->QuadPart<<endl;   // current frequency in counts/sec
	performance<<"QueryPerformanceCounter init =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	HANDLE	 hProcess=GetCurrentProcess ();

	int* answer;
	bool status;
	try
	{
		sprintf(evalString,"%skl_dll_modules.txt",kl_base_dir);
		klPrintModules(evalString);

		sprintf(evalString,"%skl_system_info.txt",kl_base_dir);
		klWinSystemInfo( evalString );

		sprintf(evalString,"%skl_memory_info.txt",kl_base_dir);
		klWinMemoryInfo(evalString );

		klCreateLowMemoryResourceNotification();
		answer=new int;
		status=klQueryLowMemoryResourceNotification(answer);
		QueryPerformanceCounter(prefCountStart);
		klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	} 
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in klPrintModules klWinSystemInfo klWinMemoryInfo klCreateLowMemoryResourceNotification"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in klPrintModules klWinSystemInfo klWinMemoryInfo klCreateLowMemoryResourceNotification"<<endl;

	}


	try
	{
		testKLMatrix<double>("testKLMatrix_double.txt");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testKLMatrix"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testKLMatrix"<<endl;

	}


	try
	{
		testKLWavelet<float>("klDemo.txt");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testKLWavelet"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testKLWavelet"<<endl;

	}


	try
	{
		testklPrincipalComponents2<double>("klDemo.tex");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testklPrincipalComponents2"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testklPrincipalComponents2"<<endl;

	}


	try
	{
		testKLMultiVariateRandomNumberGenerator2<double>("klDemo.tex");		
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testKLMultiVariateRandomNumberGenerator2"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testKLMultiVariateRandomNumberGenerator2"<<endl;

	}


	try
	{
		testKLRandomNumberGenerator2<double>("klDemo.tex");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testKLRandomNumberGenerator2"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testKLRandomNumberGenerator2"<<endl;

	}


	try
	{
		klMulticlassSVMHarness2<double>("klDemo.tex");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in klMulticlassSVMHarness2"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in klMulticlassSVMHarness2"<<endl;

	}


	try
	{
		testKLMultiVariateRandomNumberGenerator2<float>("klDemo.tex");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error "<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error"<<endl;

	}



	try
	{
		testSamplePopulation2<double>("klDemo.tex");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testSamplePopulation2"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testSamplePopulation2"<<endl;

	}


	try
	{
		testKLDescriptiveStatistics<double>("klDemo.tex");

		testKLDescriptiveStatistics<float>("klDemo.tex");
	}
	catch(std::exception e)
	{
		cout<<"Caught An STL Error in testKLDescriptiveStatistics"<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"Caught An unknown Error in testKLDescriptiveStatistics"<<endl;

	}



	QueryPerformanceCounter(prefCountEnd);

	performance<<"QueryPerformanceCounter  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	delete answer;
	delete evalString;
	//Delete index for thread local storage.
	TlsFree(gdwTlsIndex);
}


void  testKLMemory()
{
	klVMemMgr vmemmgr(1000000);

	//klAWEMemMgr awemgr(1024*1024*128);

	klHeapMgr heapmgr;
	klRedZoneHeapMgr redheapmgr;
	heapmgr.klHeapInit(256);
	void* some_mem;

	int a;
	int total=0;
	for(a=1;a<255;a++)
	{
		unsigned long b=a*sizeof(double) * a*sizeof(double);
		some_mem=vmemmgr.allocate(b);
		klNormalBoxMullerRV<double> normal;
		klMatrix<double> rand((double*)some_mem,a,a);
		int j,k;
		for(j=0;j<a;j++)
			for(k=0;k<a;k++)
				rand[j][k]=normal();
		vmemmgr.free(some_mem);
	}
	for(a=1;a<255;a++)
	{
		unsigned long b=a*sizeof(double) * a*sizeof(double);
		some_mem=heapmgr.allocate(b);
		klNormalBoxMullerRV<double> normal;
		klMatrix<double> rand((double*)some_mem,a,a);
		int j,k;
		for(j=0;j<a;j++)
			for(k=0;k<a;k++)
				rand[j][k]=normal();
		heapmgr.free(some_mem);

	}

	for(a=1;a<255;a++)
	{
		unsigned long b=a*sizeof(double) * a*sizeof(double);
		some_mem=redheapmgr.allocate(b);
		klNormalBoxMullerRV<double> normal;
		klMatrix<double> rand((double*)some_mem,a,a);
		int j,k;
		for(j=0;j<a;j++)
			for(k=0;k<a;k++)
				rand[j][k]=normal();
		redheapmgr.free(some_mem);

	}

	//for(a=1;a<128;a++)
	//{
	//	unsigned long b=a*sizeof(double) * a*sizeof(double);
	//	total+=b;
	//	some_mem=awemgr.allocate(b);
	//	klNormalBoxMullerRV<double> normal;
	//	klMatrix<double> rand((double*)some_mem,a,a);
	//	int j,k;
	//	for(j=0;j<a;j++)
	//		for(k=0;k<a;k++)
	//			rand[j][k]=normal();
	//	awemgr.free(some_mem);
	//}
}

