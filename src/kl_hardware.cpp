 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/
#include "kl_memory.h"
#include <fstream>

void klPrintModules(ofstream &stream)
{
	char* msg=new char[1024];

	DWORD processID;
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	hProcess=GetCurrentProcess ();
	sprintf(msg, "\nProcess ID: %u\n", hProcess );
	stream<<msg;

	if (NULL == hProcess)
	{
		delete msg;
		return;
	}

	if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
		{
			LPTSTR szModName=NULL;

			szModName=new TCHAR[1024];

			DWORD nSize = 1024 ;
			LPTSTR lpFilename;
			
			if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,nSize))
			{
				// Print the module name and handle value.
				sprintf(msg,"(0x%08X)\n", szModName, hMods[i] );
				std::wstring name  = szModName;
				char *str = new char[1024];
				sprintf(str,"%ls",name.c_str());
				stream<<str<<"   "<<msg;
				delete str;

			}
			delete szModName;
		}
	}
	stream.flush();
	//stream.close();

	CloseHandle( hProcess );
	
	delete msg;
}

MEMORYSTATUSEX klWinMemoryInfo(ofstream &stream)
{
	MEMORYSTATUS _memoryInfo;

	GlobalMemoryStatus (&_memoryInfo);

	MEMORYSTATUSEX _memoryInfoEx;
	_memoryInfoEx.dwLength=sizeof(_MEMORYSTATUSEX);
	GlobalMemoryStatusEx (&_memoryInfoEx);
	if(stream.good())
	{
		stream<<"Memory information: \n";
		char* temp =new char[1024];

		sprintf(temp,"Relative memory load [0-100]: %u\n", _memoryInfoEx.dwMemoryLoad); 
		stream<<temp;

		sprintf(temp,"Total Physical Memory: %u\n", _memoryInfoEx.ullTotalPhys); 
		stream<<temp;

		sprintf(temp,"Available Memory: %u\n", _memoryInfoEx.ullAvailPhys); 
		stream<<temp;

		sprintf(temp,"Total PageFile: %u\n", _memoryInfoEx.ullTotalPageFile); 
		stream<<temp;

		sprintf(temp,"Available Pagefile: %u\n", _memoryInfoEx.ullAvailPageFile); 
		stream<<temp;

		sprintf(temp,"Total Virtual Memory: %u\n", _memoryInfoEx.ullTotalVirtual); 
		stream<<temp;

		sprintf(temp,"Available Virtual Memory: %u\n", _memoryInfoEx.ullAvailVirtual); 
		stream<<temp;

		sprintf(temp,"Available Virtual Memory: %u\n", _memoryInfoEx.ullAvailExtendedVirtual); 
		stream<<temp;

		delete temp;
	}
	
	stream.flush();
	//stream.close();

	return _memoryInfoEx;
} 

bool klCheckFreeMemory(size_t size)
{
	MEMORYSTATUS _memoryInfo;

	GlobalMemoryStatus (&_memoryInfo);

	MEMORYSTATUSEX _memoryInfoEx;
	_memoryInfoEx.dwLength=sizeof(_MEMORYSTATUSEX);
	GlobalMemoryStatusEx (&_memoryInfoEx);

	if  (_memoryInfoEx.ullAvailPhys < size)
		return false;
	else
		return true;
} 

SYSTEM_INFO klWinSystemInfo(ofstream &stream)
{
	//ios_base::openmode wMode = ios_base::app;
	//ofstream stream(filename, wMode);

	SYSTEM_INFO _systemInfo;

	GetSystemInfo(&_systemInfo); 

	if(stream.good())
	{

		stream<<"Hardware information: \n";
		char* temp =new char[1024] ;

		sprintf (temp,"  OEM ID: %u\n",_systemInfo.dwOemId);
		stream<<temp;

		sprintf(temp,"  Number of processors: %u\n", _systemInfo.dwNumberOfProcessors); 
		stream<<temp;

		//and the granularity of page protection and commitment for VirtualAlloc
		sprintf(temp,"Page size: %u\n", _systemInfo.dwPageSize); 
		stream<<temp;

		//Granularity with which virtual memory is allocated. 
		//For example, a VirtualAlloc request to allocate 1 byte will reserve an address space of dwAllocationGranularity bytes
		sprintf(temp,"Allocation granularity: %u\n", _systemInfo.dwAllocationGranularity); 
		stream<<temp;

		sprintf(temp,"  Processor type: %u\n", _systemInfo.dwProcessorType); 
		stream<<temp;

		sprintf(temp,"  Minimum application address: %lx\n", _systemInfo.lpMinimumApplicationAddress); 
		stream<<temp;

		sprintf(temp,"  Maximum application address: %lx\n", _systemInfo.lpMaximumApplicationAddress); 
		stream<<temp;

		sprintf(temp,"  Active processor mask: %u\n", _systemInfo.dwActiveProcessorMask); 
		stream<<temp;

		delete temp;
	}

	stream.flush();
	//stream.close();

	return _systemInfo;
} 

typedef BOOL (WINAPI *LPFN_GLPI)(   PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,    PDWORD);
// Helper function to count set bits in the processor mask.
DWORD CountSetBits(ULONG_PTR bitMask)
{
	DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
	DWORD i;

	for (i = 0; i <= LSHIFT; ++i)
	{
		bitSetCount += ((bitMask & bitTest)?1:0);
		bitTest/=2;
	}

	return bitSetCount;
}
void klGetLogicalProcessorInformation(ofstream &stream)
{
	char* message = new char[2048];
	LPFN_GLPI glpi;
	BOOL done = FALSE;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	DWORD logicalProcessorCount = 0;
	DWORD numaNodeCount = 0;
	DWORD processorCoreCount = 0;
	DWORD processorL1CacheCount = 0;
	DWORD processorL2CacheCount = 0;
	DWORD processorL3CacheCount = 0;
	DWORD processorPackageCount = 0;
	DWORD byteOffset = 0;
	PCACHE_DESCRIPTOR Cache;

	glpi = (LPFN_GLPI) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"GetLogicalProcessorInformation");
	if (NULL == glpi) 
	{
		sprintf(message,("\nGetLogicalProcessorInformation is not supported.\n"));
		stream<<message;

		delete message;
		return;
	}

	while (!done)
	{
		DWORD rc = glpi(buffer, &returnLength);

		if (FALSE == rc) 
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
			{
				if (buffer) 
					free(buffer);

				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
					returnLength);

				if (NULL == buffer) 
				{
					sprintf(message,("\nError: Allocation failure\n"));
					stream<<message;
					return;
				}
			} 
			else 
			{
				sprintf(message,("\nError %d\n"), GetLastError());
				stream<<message;
				return;
			}
		} 
		else
		{
			done = TRUE;
		}
	}

	ptr = buffer;

	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
	{
		switch (ptr->Relationship) 
		{
		case RelationNumaNode:
			// Non-NUMA systems report a single record of this type.
			numaNodeCount++;
			break;

		case RelationProcessorCore:
			processorCoreCount++;

			// A hyperthreaded core supplies more than one logical processor.
			logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
			break;

		case RelationCache:
			// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
			Cache = &ptr->Cache;
			if (Cache->Level == 1)
			{
				processorL1CacheCount++;
			}
			else if (Cache->Level == 2)
			{
				processorL2CacheCount++;
			}
			else if (Cache->Level == 3)
			{
				processorL3CacheCount++;
			}
			break;

		case RelationProcessorPackage:
			// Logical processors share a physical package.
			processorPackageCount++;
			break;

		default:
			sprintf(message,("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
			stream<<message;
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}

	sprintf(message,("\nGetLogicalProcessorInformation results:\n"));stream<<message;
	sprintf(message,("Number of NUMA nodes: %d\n"), 	numaNodeCount);stream<<message;
	sprintf(message,("Number of physical processor packages: %d\n"), processorPackageCount);stream<<message;
	sprintf(message,("Number of processor cores: %d\n"), 	processorCoreCount);stream<<message;
	sprintf(message,("Number of logical processors: %d\n"), 		logicalProcessorCount);stream<<message;
	sprintf(message,("Number of processor L1/L2/L3 caches: %d/%d/%d\n"), 	processorL1CacheCount,	processorL2CacheCount,	processorL3CacheCount);stream<<message;
	
	free(buffer);
	delete message;
}

// Use the __cpuid intrinsic to get information about a CPU
#include <stdio.h>
#include <string.h>
#include <intrin.h>

const char* szFeatures[] =
{
    "x87 FPU On Chip",
    "Virtual-8086 Mode Enhancement",
    "Debugging Extensions",
    "Page Size Extensions",
    "Time Stamp Counter",
    "RDMSR and WRMSR Support",
    "Physical Address Extensions",
    "Machine Check Exception",
    "CMPXCHG8B Instruction",
    "APIC On Chip",
    "Unknown1",
    "SYSENTER and SYSEXIT",
    "Memory Type Range Registers",
    "PTE Global Bit",
    "Machine Check Architecture",
    "Conditional Move/Compare Instruction",
    "Page Attribute Table",
    "36-bit Page Size Extension",
    "Processor Serial Number",
    "CFLUSH Extension",
    "Unknown2",
    "Debug Store",
    "Thermal Monitor and Clock Ctrl",
    "MMX Technology",
    "FXSAVE/FXRSTOR",
    "SSE Extensions",
    "SSE2 Extensions",
    "Self Snoop",
    "Multithreading Technology",
    "Thermal Monitor",
    "Unknown4",
    "Pending Break Enable"
};

void klGetCPUID(ofstream &stream)
{
	char* message = new char[2048];
    char CPUString[0x20];
    char CPUBrandString[0x40];
    int CPUInfo[4] = {-1};
    int nSteppingID = 0;
    int nModel = 0;
    int nFamily = 0;
    int nProcessorType = 0;
    int nExtendedmodel = 0;
    int nExtendedfamily = 0;
    int nBrandIndex = 0;
    int nCLFLUSHcachelinesize = 0;
    int nLogicalProcessors = 0;
    int nAPICPhysicalID = 0;
    int nFeatureInfo = 0;
    int nCacheLineSize = 0;
    int nL2Associativity = 0;
    int nCacheSizeK = 0;
    int nPhysicalAddress = 0;
    int nVirtualAddress = 0;
    int nRet = 0;

    int nCores = 0;
    int nCacheType = 0;
    int nCacheLevel = 0;
    int nMaxThread = 0;
    int nSysLineSize = 0;
    int nPhysicalLinePartitions = 0;
    int nWaysAssociativity = 0;
    int nNumberSets = 0;

    unsigned    nIds, nExIds, i;

    bool    bSSE3Instructions = false;
    bool    bMONITOR_MWAIT = false;
    bool    bCPLQualifiedDebugStore = false;
    bool    bVirtualMachineExtensions = false;
    bool    bEnhancedIntelSpeedStepTechnology = false;
    bool    bThermalMonitor2 = false;
    bool    bSupplementalSSE3 = false;
    bool    bL1ContextID = false;
    bool    bCMPXCHG16B = false;
    bool    bxTPRUpdateControl = false;
    bool    bPerfDebugCapabilityMSR = false;
    bool    bSSE41Extensions = false;
    bool    bSSE42Extensions = false;
    bool    bPOPCNT = false;

    bool    bMultithreading = false;

    bool    bLAHF_SAHFAvailable = false;
    bool    bCmpLegacy = false;
    bool    bSVM = false;
    bool    bExtApicSpace = false;
    bool    bAltMovCr8 = false;
    bool    bLZCNT = false;
    bool    bSSE4A = false;
    bool    bMisalignedSSE = false;
    bool    bPREFETCH = false;
    bool    bSKINITandDEV = false;
    bool    bSYSCALL_SYSRETAvailable = false;
    bool    bExecuteDisableBitAvailable = false;
    bool    bMMXExtensions = false;
    bool    bFFXSR = false;
    bool    b1GBSupport = false;
    bool    bRDTSCP = false;
    bool    b64Available = false;
    bool    b3DNowExt = false;
    bool    b3DNow = false;
    bool    bNestedPaging = false;
    bool    bLBRVisualization = false;
    bool    bFP128 = false;
    bool    bMOVOptimization = false;

    bool    bSelfInit = false;
    bool    bFullyAssociative = false;

    // __cpuid with an InfoType argument of 0 returns the number of
    // valid Ids in CPUInfo[0] and the CPU identification string in
    // the other three array elements. The CPU identification string is
    // not in linear order. The code below arranges the information 
    // in a human readable form.
    __cpuid(CPUInfo, 0);
    nIds = CPUInfo[0];
    memset(CPUString, 0, sizeof(CPUString));
    *((int*)CPUString) = CPUInfo[1];
    *((int*)(CPUString+4)) = CPUInfo[3];
    *((int*)(CPUString+8)) = CPUInfo[2];

    // Get the information associated with each valid Id
    for (i=0; i<=nIds; ++i)
    {
        __cpuid(CPUInfo, i);
        sprintf(message,"\nFor InfoType %d\n", i); stream<<message;
        sprintf(message,"CPUInfo[0] = 0x%x\n", CPUInfo[0]);stream<<message;
        sprintf(message,"CPUInfo[1] = 0x%x\n", CPUInfo[1]);stream<<message;
        sprintf(message,"CPUInfo[2] = 0x%x\n", CPUInfo[2]);stream<<message;
        sprintf(message,"CPUInfo[3] = 0x%x\n", CPUInfo[3]);stream<<message;

        // Interpret CPU feature information.
        if  (i == 1)
        {
            nSteppingID = CPUInfo[0] & 0xf;
            nModel = (CPUInfo[0] >> 4) & 0xf;
            nFamily = (CPUInfo[0] >> 8) & 0xf;
            nProcessorType = (CPUInfo[0] >> 12) & 0x3;
            nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
            nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
            nBrandIndex = CPUInfo[1] & 0xff;
            nCLFLUSHcachelinesize = ((CPUInfo[1] >> 8) & 0xff) * 8;
            nLogicalProcessors = ((CPUInfo[1] >> 16) & 0xff);
            nAPICPhysicalID = (CPUInfo[1] >> 24) & 0xff;
            bSSE3Instructions = (CPUInfo[2] & 0x1) || false;
            bMONITOR_MWAIT = (CPUInfo[2] & 0x8) || false;
            bCPLQualifiedDebugStore = (CPUInfo[2] & 0x10) || false;
            bVirtualMachineExtensions = (CPUInfo[2] & 0x20) || false;
            bEnhancedIntelSpeedStepTechnology = (CPUInfo[2] & 0x80) || false;
            bThermalMonitor2 = (CPUInfo[2] & 0x100) || false;
            bSupplementalSSE3 = (CPUInfo[2] & 0x200) || false;
            bL1ContextID = (CPUInfo[2] & 0x300) || false;
            bCMPXCHG16B= (CPUInfo[2] & 0x2000) || false;
            bxTPRUpdateControl = (CPUInfo[2] & 0x4000) || false;
            bPerfDebugCapabilityMSR = (CPUInfo[2] & 0x8000) || false;
            bSSE41Extensions = (CPUInfo[2] & 0x80000) || false;
            bSSE42Extensions = (CPUInfo[2] & 0x100000) || false;
            bPOPCNT= (CPUInfo[2] & 0x800000) || false;
            nFeatureInfo = CPUInfo[3];
            bMultithreading = (nFeatureInfo & (1 << 28)) || false;
        }
    }

    // Calling __cpuid with 0x80000000 as the InfoType argument
    // gets the number of valid extended IDs.
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    // Get the information associated with each extended ID.
    for (i=0x80000000; i<=nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        sprintf(message,"\nFor InfoType %x\n", i); stream<<message;
        sprintf(message,"CPUInfo[0] = 0x%x\n", CPUInfo[0]);stream<<message;
        sprintf(message,"CPUInfo[1] = 0x%x\n", CPUInfo[1]);stream<<message;
        sprintf(message,"CPUInfo[2] = 0x%x\n", CPUInfo[2]);stream<<message;
        sprintf(message,"CPUInfo[3] = 0x%x\n", CPUInfo[3]);stream<<message;

        if  (i == 0x80000001)
        {
            bLAHF_SAHFAvailable = (CPUInfo[2] & 0x1) || false;
            bCmpLegacy = (CPUInfo[2] & 0x2) || false;
            bSVM = (CPUInfo[2] & 0x4) || false;
            bExtApicSpace = (CPUInfo[2] & 0x8) || false;
            bAltMovCr8 = (CPUInfo[2] & 0x10) || false;
            bLZCNT = (CPUInfo[2] & 0x20) || false;
            bSSE4A = (CPUInfo[2] & 0x40) || false;
            bMisalignedSSE = (CPUInfo[2] & 0x80) || false;
            bPREFETCH = (CPUInfo[2] & 0x100) || false;
            bSKINITandDEV = (CPUInfo[2] & 0x1000) || false;
            bSYSCALL_SYSRETAvailable = (CPUInfo[3] & 0x800) || false;
            bExecuteDisableBitAvailable = (CPUInfo[3] & 0x10000) || false;
            bMMXExtensions = (CPUInfo[3] & 0x40000) || false;
            bFFXSR = (CPUInfo[3] & 0x200000) || false;
            b1GBSupport = (CPUInfo[3] & 0x400000) || false;
            bRDTSCP = (CPUInfo[3] & 0x8000000) || false;
            b64Available = (CPUInfo[3] & 0x20000000) || false;
            b3DNowExt = (CPUInfo[3] & 0x40000000) || false;
            b3DNow = (CPUInfo[3] & 0x80000000) || false;
        }

        // Interpret CPU brand string and cache information.
        if  (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000006)
        {
            nCacheLineSize = CPUInfo[2] & 0xff;
            nL2Associativity = (CPUInfo[2] >> 12) & 0xf;
            nCacheSizeK = (CPUInfo[2] >> 16) & 0xffff;
        }
        else if  (i == 0x80000008)
        {
           nPhysicalAddress = CPUInfo[0] & 0xff;
           nVirtualAddress = (CPUInfo[0] >> 8) & 0xff;
        }
        else if  (i == 0x8000000A)
        {
            bNestedPaging = (CPUInfo[3] & 0x1) || false;
            bLBRVisualization = (CPUInfo[3] & 0x2) || false;
        }
        else if  (i == 0x8000001A)
        {
            bFP128 = (CPUInfo[0] & 0x1) || false;
            bMOVOptimization = (CPUInfo[0] & 0x2) || false;
        }
    }

    // Display all the information in user-friendly format.

    sprintf(message,"\n\nCPU String: %s\n", CPUString);stream<<message;

    if  (nIds >= 1)
    {
        if  (nSteppingID)
            sprintf(message,"Stepping ID = %d\n", nSteppingID);stream<<message;
        if  (nModel)
            sprintf(message,"Model = %d\n", nModel);stream<<message;
        if  (nFamily)
            sprintf(message,"Family = %d\n", nFamily);stream<<message;
        if  (nProcessorType)
            sprintf(message,"Processor Type = %d\n", nProcessorType);stream<<message;
        if  (nExtendedmodel)
            sprintf(message,"Extended model = %d\n", nExtendedmodel);stream<<message;
        if  (nExtendedfamily)
            sprintf(message,"Extended family = %d\n", nExtendedfamily);stream<<message;
        if  (nBrandIndex)
            sprintf(message,"Brand Index = %d\n", nBrandIndex);stream<<message;
        if  (nCLFLUSHcachelinesize)
            sprintf(message,"CLFLUSH cache line size = %d\n",nCLFLUSHcachelinesize);stream<<message;
        if (bMultithreading && (nLogicalProcessors > 0))
           sprintf(message,"Logical Processor Count = %d\n", nLogicalProcessors);stream<<message;
        if  (nAPICPhysicalID)
            sprintf(message,"APIC Physical ID = %d\n", nAPICPhysicalID);stream<<message;

        if  (nFeatureInfo || bSSE3Instructions ||
             bMONITOR_MWAIT || bCPLQualifiedDebugStore ||
             bVirtualMachineExtensions || bEnhancedIntelSpeedStepTechnology ||
             bThermalMonitor2 || bSupplementalSSE3 || bL1ContextID || 
             bCMPXCHG16B || bxTPRUpdateControl || bPerfDebugCapabilityMSR || 
             bSSE41Extensions || bSSE42Extensions || bPOPCNT || 
             bLAHF_SAHFAvailable || bCmpLegacy || bSVM ||
             bExtApicSpace || bAltMovCr8 ||
             bLZCNT || bSSE4A || bMisalignedSSE ||
             bPREFETCH || bSKINITandDEV || bSYSCALL_SYSRETAvailable || 
             bExecuteDisableBitAvailable || bMMXExtensions || bFFXSR || b1GBSupport ||
             bRDTSCP || b64Available || b3DNowExt || b3DNow || bNestedPaging || 
             bLBRVisualization || bFP128 || bMOVOptimization )
        {
            sprintf(message,"\nThe following features are supported:\n");stream<<message;

            if  (bSSE3Instructions)
                sprintf(message,"\tSSE3\n");stream<<message;
            if  (bMONITOR_MWAIT)
                sprintf(message,"\tMONITOR/MWAIT\n");stream<<message;
            if  (bCPLQualifiedDebugStore)
                sprintf(message,"\tCPL Qualified Debug Store\n");stream<<message;
            if  (bVirtualMachineExtensions)
                sprintf(message,"\tVirtual Machine Extensions\n");stream<<message;
            if  (bEnhancedIntelSpeedStepTechnology)
                sprintf(message,"\tEnhanced Intel SpeedStep Technology\n");stream<<message;
            if  (bThermalMonitor2)
                sprintf(message,"\tThermal Monitor 2\n");stream<<message;
            if  (bSupplementalSSE3)
                sprintf(message,"\tSupplemental Streaming SIMD Extensions 3\n");stream<<message;
            if  (bL1ContextID)
                sprintf(message,"\tL1 Context ID\n");stream<<message;
            if  (bCMPXCHG16B)
                sprintf(message,"\tCMPXCHG16B Instruction\n");stream<<message;
            if  (bxTPRUpdateControl)
                sprintf(message,"\txTPR Update Control\n");stream<<message;
            if  (bPerfDebugCapabilityMSR)
                sprintf(message,"\tPerf\\Debug Capability MSR\n");stream<<message;
            if  (bSSE41Extensions)
                sprintf(message,"\tSSE4.1 Extensions\n");stream<<message;
            if  (bSSE42Extensions)
                sprintf(message,"\tSSE4.2 Extensions\n");stream<<message;
            if  (bPOPCNT)
                sprintf(message,"\tPPOPCNT Instruction\n");stream<<message;

            i = 0;
            nIds = 1;
            while (i < (sizeof(szFeatures)/sizeof(const char*)))
            {
                if  (nFeatureInfo & nIds)
                {
                    sprintf(message,"\t");
                    sprintf(message,szFeatures[i]);
                    sprintf(message,"\n");
                }

                nIds <<= 1;
                ++i;
            }
            if (bLAHF_SAHFAvailable)
                sprintf(message,"\tLAHF/SAHF in 64-bit mode\n");stream<<message;
            if (bCmpLegacy)
                sprintf(message,"\tCore multi-processing legacy mode\n");stream<<message;
            if (bSVM)
                sprintf(message,"\tSecure Virtual Machine\n");stream<<message;
            if (bExtApicSpace)
                sprintf(message,"\tExtended APIC Register Space\n");stream<<message;
            if (bAltMovCr8)
                sprintf(message,"\tAltMovCr8\n");stream<<message;
            if (bLZCNT)
                sprintf(message,"\tLZCNT instruction\n");stream<<message;
            if (bSSE4A)
                sprintf(message,"\tSSE4A (EXTRQ, INSERTQ, MOVNTSD, MOVNTSS)\n");stream<<message;
            if (bMisalignedSSE)
                sprintf(message,"\tMisaligned SSE mode\n");stream<<message;
            if (bPREFETCH)
                sprintf(message,"\tPREFETCH and PREFETCHW Instructions\n");stream<<message;
            if (bSKINITandDEV)
                sprintf(message,"\tSKINIT and DEV support\n");stream<<message;
            if (bSYSCALL_SYSRETAvailable)
                sprintf(message,"\tSYSCALL/SYSRET in 64-bit mode\n");stream<<message;
            if (bExecuteDisableBitAvailable)
                sprintf(message,"\tExecute Disable Bit\n");stream<<message;
            if (bMMXExtensions)
                sprintf(message,"\tExtensions to MMX Instructions\n");stream<<message;
            if (bFFXSR)
                sprintf(message,"\tFFXSR\n");stream<<message;
            if (b1GBSupport)
                sprintf(message,"\t1GB page support\n");stream<<message;
            if (bRDTSCP)
                sprintf(message,"\tRDTSCP instruction\n");stream<<message;
            if (b64Available)
                sprintf(message,"\t64 bit Technology\n");stream<<message;
            if (b3DNowExt)
                sprintf(message,"\t3Dnow Ext\n");stream<<message;
            if (b3DNow)
                sprintf(message,"\t3Dnow! instructions\n");stream<<message;
            if (bNestedPaging)
                sprintf(message,"\tNested Paging\n");stream<<message;
            if (bLBRVisualization)
                sprintf(message,"\tLBR Visualization\n");stream<<message;
            if (bFP128)
                sprintf(message,"\tFP128 optimization\n");stream<<message;
            if (bMOVOptimization)
                sprintf(message,"\tMOVU Optimization\n");stream<<message;
        }
    }

    if  (nExIds >= 0x80000004)
        sprintf(message,"\nCPU Brand String: %s\n", CPUBrandString);stream<<message;

    if  (nExIds >= 0x80000006)
    {
        sprintf(message,"Cache Line Size = %d\n", nCacheLineSize);stream<<message;
        sprintf(message,"L2 Associativity = %d\n", nL2Associativity);stream<<message;
        sprintf(message,"Cache Size = %dK\n", nCacheSizeK);stream<<message;
    }


    for (i=0;;i++)
    {
        __cpuidex(CPUInfo, 0x4, i);
        if(!(CPUInfo[0] & 0xf0)) break;

        if(i == 0)
        {
            nCores = CPUInfo[0] >> 26;
            sprintf(message,"\n\nNumber of Cores = %d\n", nCores + 1);stream<<message;
        }

        nCacheType = (CPUInfo[0] & 0x1f);
        nCacheLevel = (CPUInfo[0] & 0xe0) >> 5;
        bSelfInit = (CPUInfo[0] & 0x100) >> 8;
        bFullyAssociative = (CPUInfo[0] & 0x200) >> 9;
        nMaxThread = (CPUInfo[0] & 0x03ffc000) >> 14;
        nSysLineSize = (CPUInfo[1] & 0x0fff);
        nPhysicalLinePartitions = (CPUInfo[1] & 0x03ff000) >> 12;
        nWaysAssociativity = (CPUInfo[1]) >> 22;
        nNumberSets = CPUInfo[2];

        sprintf(message,"\n");stream<<message;

        sprintf(message,"ECX Index %d\n", i);stream<<message;
        switch (nCacheType)
        {
            case 0:
                sprintf(message,"   Type: Null\n");stream<<message;
                break;
            case 1:
                sprintf(message,"   Type: Data Cache\n");stream<<message;
                break;
            case 2:
                sprintf(message,"   Type: Instruction Cache\n");stream<<message;
                break;
            case 3:
                sprintf(message,"   Type: Unified Cache\n");stream<<message;
                break;
            default:
                 sprintf(message,"   Type: Unknown\n");stream<<message;
        }

        sprintf(message,"   Level = %d\n", nCacheLevel + 1); 
        if (bSelfInit)
        {
            sprintf(message,"   Self Initializing\n");stream<<message;
        }
        else
        {
            sprintf(message,"   Not Self Initializing\n");stream<<message;
        }
        if (bFullyAssociative)
        {
            sprintf(message,"   Is Fully Associatve\n");stream<<message;
        }
        else
        {
            sprintf(message,"   Is Not Fully Associatve\n");stream<<message;
        }
        sprintf(message,"   Max Threads = %d\n", nMaxThread+1);stream<<message;
        sprintf(message,"   System Line Size = %d\n", nSysLineSize+1);stream<<message;
        sprintf(message,"   Physical Line Partions = %d\n", nPhysicalLinePartitions+1);stream<<message;
        sprintf(message,"   Ways of Associativity = %d\n",  nWaysAssociativity+1);stream<<message;
        sprintf(message,"   Number of Sets = %d\n",nNumberSets+1);stream<<message;
    }
	delete message;
}

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include "tchar.h"
# pragma comment(lib, "wbemuuid.lib")
void klWMI(ofstream &stream)
{
	CoInitialize(NULL);
	if (CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0) != S_OK)
		return;

	IWbemLocator* pIWbemLocator = NULL;
	IWbemServices* pWbemServices = NULL;
	IEnumWbemClassObject* pEnumObject = NULL;

	BSTR bstrNamespace = (L"root\\cimv2");

	if (CoCreateInstance (
		CLSID_WbemAdministrativeLocator,
		NULL ,
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER ,
		IID_IUnknown ,
		( void ** ) & pIWbemLocator) != S_OK)
		return;

	if (pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, NULL, 0,
		NULL, NULL,&pWbemServices) != S_OK)
		return;

	HRESULT hRes;
	BSTR strQuery = (L"Select * from win32_Processor");
	BSTR strQL = (L"WQL");
	hRes = pWbemServices->ExecQuery(strQL, strQuery,
		WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumObject);
	if (hRes != S_OK)
	{
		printf("Could not execute Query");
		return;
	}

	hRes = pEnumObject->Reset();
	if (hRes != S_OK)
	{
		printf("Could not Enumerate");
		return;
	}

	ULONG uCount = 1, uReturned;
	IWbemClassObject* pClassObject = NULL;
	hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
	if(hRes != S_OK)
	{
		printf("Could not Enumerate");
		return;
	}

	VARIANT v;
	unsigned int nItems= 24;
	WCHAR** items=new WCHAR*[nItems];
	unsigned int ij=0;
	for (ij=0;ij<nItems;ij++)
	{
		items[ij]=new WCHAR[128];
	}
	wsprintf(items[0],L"Description");
	wsprintf(items[1],L"AddressWidth");
	wsprintf(items[2],L"Architecture");
	wsprintf(items[3],L"Availability");
	wsprintf(items[4],L"Caption");
	wsprintf(items[5],L"CpuStatus");
	wsprintf(items[6],L"CreationClassName");
	wsprintf(items[7],L"CurrentClockSpeed");
	wsprintf(items[8],L"CurrentVoltage");
	wsprintf(items[9],L"DataWidth");
	wsprintf(items[10],L"Description");
	wsprintf(items[11],L"DeviceID");
	wsprintf(items[12],L"ExtClock");
	wsprintf(items[13],L"Family");
	wsprintf(items[14],L"L2CacheSize");
	wsprintf(items[15],L"L2CacheSpeed");
	wsprintf(items[16],L"L3CacheSize");
	wsprintf(items[17],L"Level");
	wsprintf(items[18],L"LoadPercentage");
	wsprintf(items[19],L"Manufacturer");
	wsprintf(items[20],L"MaxClockSpeed");
	wsprintf(items[21],L"Name");
	wsprintf(items[22],L"NumberOfCores");
	wsprintf(items[23],L"NumberOfLogicalProcessors");


	//Dumb as hell
	char** itemsCHAR=new char*[nItems];
	
	for (ij=0;ij<nItems;ij++)
	{
		itemsCHAR[ij]=new char[128];
	}
	sprintf(itemsCHAR[0],"Description");
	sprintf(itemsCHAR[1],"AddressWidth");
	sprintf(itemsCHAR[2],"Architecture");
	sprintf(itemsCHAR[3],"Availability");
	sprintf(itemsCHAR[4],"Caption");
	sprintf(itemsCHAR[5],"CpuStatus");
	sprintf(itemsCHAR[6],"CreationClassName");
	sprintf(itemsCHAR[7],"CurrentClockSpeed");
	sprintf(itemsCHAR[8],"CurrentVoltage");
	sprintf(itemsCHAR[9],"DataWidth");
	sprintf(itemsCHAR[10],"Description");
	sprintf(itemsCHAR[11],"DeviceID");
	sprintf(itemsCHAR[12],"ExtClock");
	sprintf(itemsCHAR[13],"Family");
	sprintf(itemsCHAR[14],"L2CacheSize");
	sprintf(itemsCHAR[15],"L2CacheSpeed");
	sprintf(itemsCHAR[16],"L3CacheSize");
	sprintf(itemsCHAR[17],"Level");
	sprintf(itemsCHAR[18],"LoadPercentage");
	sprintf(itemsCHAR[19],"Manufacturer");
	sprintf(itemsCHAR[20],"MaxClockSpeed");
	sprintf(itemsCHAR[21],"Name");
	sprintf(itemsCHAR[22],"NumberOfCores");
	sprintf(itemsCHAR[23],"NumberOfLogicalProcessors");
	for(ij=0;ij<nItems;ij++)
	{
		BSTR strClassProp = SysAllocString(items[ij]);
		hRes = pClassObject->Get(strClassProp, 0, &v, 0, 0);
		if (hRes != S_OK)
		{
			printf("Could not Get Value");
			return;
		}
		SysFreeString(strClassProp);
		if(v.vt  ==VT_NULL)
		{
			
			stream<<"Could Not Get : "<<itemsCHAR[ij]<<endl;
			continue;
		}
		_bstr_t bstrPath = &v; //Just to convert BSTR to ANSI
		char* strPath=(char*)bstrPath;
		if (SUCCEEDED(hRes))
		{
			stream<<strPath<<"    "<<itemsCHAR[ij]<<endl;
		}
		else
			printf("Error in getting object");

		VariantClear(&v);
	}//end Loop over items
	pIWbemLocator->Release();
	pWbemServices->Release();
	pEnumObject->Release();
	pClassObject->Release();
	CoUninitialize();
	for (ij=0;ij<nItems;ij++)
	{
		delete items[ij];
	}
	for (ij=0;ij<nItems;ij++)
	{
		delete itemsCHAR[ij];
	}
}

#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")

void klGetMACaddress(ofstream &stream)
{
	char* macADDRESS = new char[1024];
	IP_ADAPTER_INFO AdapterInfo[16];			// Allocate information for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);		// Save the memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);								
	assert(dwStatus == ERROR_SUCCESS);		


	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
	do {


		BYTE* MACData =(pAdapterInfo->Address);
		sprintf(macADDRESS, "\nMAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n", MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
		stream<<macADDRESS<<endl;

		stream<<pAdapterInfo->AdapterName<<endl;
		stream<<pAdapterInfo->Description<<endl;
		stream<<pAdapterInfo->CurrentIpAddress<<endl;
		stream<<pAdapterInfo->IpAddressList.IpAddress.String<<endl;
		stream<<pAdapterInfo->GatewayList.IpAddress.String<<endl;
		stream<<pAdapterInfo->DhcpServer.IpAddress.String<<endl;
        stream<<pAdapterInfo->PrimaryWinsServer.IpAddress.String<<endl;
		stream<<pAdapterInfo->SecondaryWinsServer.IpAddress.String<<endl;
		
		pAdapterInfo = pAdapterInfo->Next;		// Progress through linked list
	}
	while(pAdapterInfo);						// Terminate if last adapter

	delete macADDRESS;
}







