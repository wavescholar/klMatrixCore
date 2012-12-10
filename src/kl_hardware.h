 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/
#ifndef __kl_HARDWARE__
#define __kl_HARDWARE__
#include <windows.h>
#include <wincon.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "nb30.h"  //This header contains the definitions for portable NetBIOS 3.0
#include <iostream>
using namespace std;
#pragma comment(lib, "netapi32.lib")
unsigned  GetOSLogicalProcessorCount();
unsigned  GetSysProcessorPackageCount();
unsigned  GetSysProcessorPackageCount();
unsigned  GetSysProcessorPackageCount();
unsigned  GetProcessorCoreCount();
unsigned  GetLogicalProcessorCount();
unsigned  GetCoresPerPackageProcessorCount();
unsigned  GetProcessorPackageCount();
unsigned  GetLogicalPerCoreProcessorCount();
unsigned  getAPICID(unsigned  processor);

class klCPUTopology
{

public:
	unsigned int  _GetOSLogicalProcessorCount(){return GetOSLogicalProcessorCount();}
	unsigned int  _GetSysProcessorPackageCount(){return GetSysProcessorPackageCount();}
	unsigned int  _GetProcessorCoreCount(){return GetProcessorCoreCount();}
	unsigned int  _GetLogicalProcessorCount(){return GetLogicalProcessorCount();}
	unsigned int  _GetCoresPerPackageProcessorCount(){return GetCoresPerPackageProcessorCount();}
	unsigned int  _GetProcessorPackageCount(){return GetProcessorPackageCount();}
	unsigned int  _GetLogicalPerCoreProcessorCount(){return GetLogicalPerCoreProcessorCount();}

//
//unsigned  GetCoreCount(unsigned long package_ordinal)
//unsigned  GetThreadCount(unsigned long package_ordinal, unsigned long core_ordinal);
//
//unsigned  getAPICID(unsigned  processor);
};

static ostream& operator<<(ostream& str, klCPUTopology& v)
{
	
	str<<"GetOSLogicalProcessorCount = "<<v._GetOSLogicalProcessorCount()<<endl;
	str<<"GetSysProcessorPackageCount = "<<v._GetSysProcessorPackageCount() <<endl;
	str<<"GetProcessorCoreCount = "<<v._GetProcessorCoreCount() <<endl;
	str<<"GetLogicalProcessorCount = "<<v._GetLogicalProcessorCount() <<endl;
	str<<"GetCoresPerPackageProcessorCount = "<<v._GetCoresPerPackageProcessorCount() <<endl;
	str<<"GetProcessorPackageCount = "<<v._GetProcessorPackageCount() <<endl;
	str<<"GetLogicalPerCoreProcessorCount = "<<v._GetLogicalPerCoreProcessorCount() <<endl;

	return str<<"\n";
}


#endif __kl_HARDWARE__