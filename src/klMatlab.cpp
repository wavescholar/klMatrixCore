// kldll.cpp : Defines the entry point for the DLL application.
#include "windows.h"
#include "kl_memory.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#define KL_OPERATOR_NEW

#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_time_series.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_large_dataset.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_util.h"
#include "kl_unit_tests.h"
#include "kl_matlab_iface.h"
#include "kl_matlab_dependent_unit_tests.h"
#include "kl_image_processing_functors.h"


klMutex klMatlabEngineThreadMap::lock;
map<klThreadId, Engine*> klMatlabEngineThreadMap::engineMap;

#include <iostream>
#include <ostream>
#include <fstream>
using namespace std;

#pragma warning (once : 4244)
#pragma warning (once : 4305)

//const char* kl_base_dir="D://KL";
//const char* basefilename="//output//UnitTestOutput//";
DWORD gdwTlsIndex;
void __cdecl klNewHandler( );
//{
//	cout << "The new_handler is called:" << endl;
//	char* arg=new char[256];
//	sprintf(arg,"%s/ErrorDumpModules.txt",kl_base_dir);
//	klPrintModules(arg);
//	sprintf(arg,"%s/ErrorDumpMemoryInfo.txt",kl_base_dir);
//	klWinMemoryInfo(arg);
//	sprintf(arg,"%s/ErrorDumpSystemInfo.txt",kl_base_dir);
//	klWinSystemInfo(arg);
//	throw bad_alloc( );
//	return;
//}

extern DWORD gdwTlsIndex;

extern "C" void  klRunMatlabWorkFlow(void)
{
	klFlotingPointHelper mFPH;
	char* FP_errStatus= new char [2048];
	mFPH.getAndClearStatus(FP_errStatus);

	gdwTlsIndex = TlsAlloc();

	set_new_handler (klNewHandler);

	char* evalString=new char[256];

	sprintf(evalString,"%skl__matlab_unit_test_performance.txt",kl_base_dir);

	//Performance Data 
	ofstream performance(evalString );
	
	FILE *stream ;
	sprintf(evalString,"%skl_std_out.txt",kl_base_dir);
	if((stream = freopen(evalString, "w", stdout)) == NULL)
		throw "klMain: error redirecting std::cout to a file.";
	printf("  KL stdout\n");

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
	performance<<"QueryPerformanceFrequency  =  "<<freq->QuadPart<<endl;   // current frequency in counts/sec
	performance<<"QueryPerformanceCounter init =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	HANDLE	 hProcess=GetCurrentProcess ();

	sprintf(evalString,"%skl_dll_modules.txt",kl_base_dir);
	klPrintModules(evalString);

	sprintf(evalString,"%skl_system_info.txt",kl_base_dir);
	klWinSystemInfo( evalString );

	sprintf(evalString,"%skl_memory_info.txt",kl_base_dir);
	klWinMemoryInfo(evalString );

	klCreateLowMemoryResourceNotification();

	int* answer=new int;

	bool status=klQueryLowMemoryResourceNotification(answer);

	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=NULL;
    if (!(matlabEngine = engOpen(NULL))) 
        throw "klWorkflow: no matlab engine available";

	klmtm.insert(thisThread,matlabEngine);

	testklPrincipalComponentsMatlab<double> ("");
	mFPH.getAndClearStatus(FP_errStatus);	
	cout<<"testklPrincipalComponentsMatlab<double>  FP Status"<<FP_errStatus<<endl;

	testKLMultiVariateRandomNumberGeneratorMatlab<double>("");
	mFPH.getAndClearStatus(FP_errStatus);	
	cout<<"testKLMultiVariateRandomNumberGeneratorMatlab FP Status"<<FP_errStatus<<endl;
	
	testKLRandomNumberGeneratorMatlab<double>("");
	mFPH.getAndClearStatus(FP_errStatus);
	cout<<"testKLRandomNumberGeneratorMatlab FP ststus "<<FP_errStatus<<endl;

	klMulticlassSVMHarnessMatlab<double>("");
	mFPH.getAndClearStatus(FP_errStatus);
	cout<<"klMulticlassSVMHarnessMatlab  FP status \n"<<FP_errStatus<<endl;
	
	flushall();

	delete FP_errStatus;
}

