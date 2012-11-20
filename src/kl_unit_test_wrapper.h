#ifndef __kl_unit_test_wrapper__
#define __kl_unit_test_wrapper__
#include "windows.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#define KL_OPERATOR_NEW
#include "kl_memory.h"
#include "kl_matrix.h"
#include "kl_util.h"
#include "kl_exception.h"
#include "kl_hardware.h"
#include <iostream>
#include <ostream>
#include <fstream>


using namespace std;

#pragma warning (once : 4244)
#pragma warning (once : 4305)

void klWMI(ofstream &stream);
void klGetLogicalProcessorInformation(ofstream &stream);
void klGetCPUID(ofstream &stream);
void klGetMACaddress(ofstream &stream);


class klUnitTestWrapper
{

public:

	klUnitTestWrapper(ofstream &mstream, ofstream &sysstream) : stream(mstream),system_stream(sysstream)
	{
		
	}

	void HardwareConfiguration(ofstream &mstream)
	{
		char* evalString=new char[1024];

		mstream<<"------------------klPrintModules------------------"<<endl;
		klPrintModules(mstream);

		mstream<<"------------------klWinSystemInfo------------------"<<endl;
		klWinSystemInfo( mstream);

		mstream<<"------------------klWinMemoryInfo------------------"<<endl;
		klWinMemoryInfo(mstream);

		mstream<<"------------------klGetLogicalProcessorInformation------------------"<<endl;
		klGetLogicalProcessorInformation(mstream);

		mstream<<"------------------klGetMACaddress------------------"<<endl;
		klGetMACaddress(mstream);

		mstream<<"------------------klGetCPUID------------------"<<endl;
		klGetCPUID(mstream);

		mstream<<"------------------klWMI------------------"<<endl;
		klWMI(mstream);

		flushall();

		delete evalString;
	}

	void setDimension(unsigned int n)
	{
		_n = n;
	}


public:
	void MemoryPreCheck()
	{
		int heapstatus = _heapchk();
		_CrtDumpMemoryLeaks();
		_CrtMemCheckpoint( &s1 );
		_CrtMemDumpStatistics( &s1 );
	}

	void MemoryPostCheck()
	{
		int heapstatus = _heapchk();
		_CrtMemCheckpoint( &s2 );
		_CrtMemDumpStatistics( &s2 );
		if ( _CrtMemDifference( &s3, &s1, &s2) )
		{
			system_stream<<"klUnitTestWrapper:: MemoryPostCheck() detected memory state difference.";
			_CrtMemDumpStatistics( &s3 );
		}
		_CrtDumpMemoryLeaks();
	}
private:	
	_CrtMemState s1, s2, s3;


	ofstream& system_stream;
	ofstream& stream;
	unsigned int _n;  //Dimension (or some measure of it) for the test problem

	klFlotingPointHelper mFPH;
public:
	void checkFloatingPointStatus(ofstream &stream)
	{
		char* FP_errStatus= new char [2048];
		mFPH.getAndClearStatus(FP_errStatus);
		stream<<"Floating Point Error Status : "<<FP_errStatus<<endl;
		delete FP_errStatus;
	}

	void runTest(void (*pf)(ofstream &, unsigned int &))
	{
		try
		{
			MemoryPreCheck();

			LARGE_INTEGER* freq;
			_LARGE_INTEGER* prefCountStart;
			_LARGE_INTEGER* prefCountEnd;
			freq=new _LARGE_INTEGER;
			prefCountStart=new _LARGE_INTEGER;
			prefCountEnd=new _LARGE_INTEGER;
			
			QueryPerformanceFrequency(freq);
			
			QueryPerformanceCounter(prefCountStart);
			pf(stream, _n);
			QueryPerformanceCounter(prefCountEnd);

			stream<<"QueryPerformanceCounter  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

			MemoryPostCheck();

			checkFloatingPointStatus(system_stream);

		}
		catch(...)
		{
			std::stringstream ANSI_INFO_ss (std::stringstream::in | std::stringstream::out );
			ANSI_INFO_ss<<"ANSI COMPILE INFO: " <<__DATE__<<"     "<<__TIME__<<"   "<<__FILE__<<"   "<<__LINE__<<"       "<<std::endl;
			std::string err = ANSI_INFO_ss.str();		
			throw klError(err);
		}

	}

};


#endif //__kl_unit_test_wrapper__