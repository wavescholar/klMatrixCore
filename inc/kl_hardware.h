 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
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