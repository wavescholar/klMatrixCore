 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/
#ifndef __kl_memory__
#define __kl_memory__
/*
KL memory management module.
This module provides facilities for the following memory resource management functionality;
  Retrieve Win32 System Info.
  Global override of operator new and delete putting heap allocations on a private heap that can be managed
  by the user.  
  Private heap creation facilities.
  Virtal Memory management classes.
  An interface for implementing Adress Windowing Extensions (AWE) memory management.
  Memory Mapped File facilities.
  Memory use and debugging facilities through the Win32 imaghelp dll and or psapi.
  
  050414 - Added an Intel MKL memory manager.  This provides aligned buffers.

*/


/* Magic Numer Notes

ABABABAB Used by Microsoft's HeapAlloc() to mark "no man's land" guard bytes after allocated heap memory
 
BAADF00D Used by Microsoft's LocalAlloc(LMEM_FIXED) to mark uninitialised allocated heap memory
 
BEEFCACE Used by Microsoft .NET as a magic number in resource files

CCCCCCCC Used by Microsoft's C++ debugging runtime library to mark uninitialised stack memory
 
CDCDCDCD Used by Microsoft's C++ debugging runtime library to mark uninitialised heap memory

DEADDEAD A Microsoft Windows STOP Error code used when the user manually initiates the crash.

FDFDFDFD Used by Microsoft's C++ debugging heap to mark "no man's land" guard bytes before and after allocated heap memory

FEEEFEEE Used by Microsoft's HeapFree() to mark freed heap memory

*/
#include "kl_exception.h"
#define ANSI_INFO std::stringstream ANSI_INFO_ss (std::stringstream::in | std::stringstream::out );                            \
	ANSI_INFO_ss<<"ANSI COMPILE INFO: " <<__DATE__<<"     "<<__TIME__<<"   "<<__FILE__<<"   "<<__LINE__<<"       "<<std::endl; \
	std::string err = ANSI_INFO_ss.str();


///////////////////misc process and memory validation code /////////////////////////////
#pragma warning(disable:4786)  //truncating symbols warning
#include <iostream>
#include <map>

#include <windows.h>

using namespace std;
#include "psapi.h"

//Get Win32 system information.  System info is sent to stream if a valid stream is provided, and 
//the _SYSTEM_INFO structure is returned.
SYSTEM_INFO klWinSystemInfo(ofstream &_tex);

MEMORYSTATUSEX klWinMemoryInfo(ofstream &_tex);

bool klCheckFreeMemory(size_t size);

void klPrintModules(ofstream &_tex );


class klMemMgr{
public:
	virtual void* allocate(size_t size)=0;
	virtual bool free(void*)=0;
};


class klVMemBlock{
public:
	klVMemBlock(size_t initSize)
	{
		SYSTEM_INFO     sysinfo;
		if (!_systemPageSize)
		{
			GetSystemInfo( &sysinfo );
			_systemPageSize = sysinfo.dwPageSize;        // pagesize
		}
		_reqSize=initSize;
			_actualSize = 0;
		
		if ( _reqSize < static_cast<size_t>(_systemPageSize) ) 
		{
			_memory = new char[_reqSize];
			_virtualBlock = false;		
		}
		else
		{
			// large piece, round up to the nearest page _reqSize
			if (_reqSize % _systemPageSize)
				_actualSize = ((_reqSize / _systemPageSize) + 1) * _systemPageSize;
			else
				_actualSize = _reqSize;
			
			// perform the virtual alloc
			_memory = (void *) VirtualAlloc(NULL, _actualSize,
				MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN,
				PAGE_READWRITE );

			// if we received one, attempt to lock it down	
			if (_memory)
			{
				// if we can't, oh well, gave it our best
				VirtualLock(_memory, _actualSize);
			}
			
			// Mark as allocated using virtual alloc
			_virtualBlock = true;
		}
	}
	
	~klVMemBlock()
	{
		
		
		if (_memory)
		{
			// virtual alloc'd?
			if (!_virtualBlock)			
				delete [] _memory;
			else
			{
				VirtualUnlock(_memory, _actualSize);				
				VirtualFree(_memory, _actualSize, MEM_DECOMMIT);		
				VirtualFree(_memory, 0, MEM_RELEASE);			
			}
			
			_memory = NULL;
		}
		
	}
	
	void* memory(){return _memory;}
	size_t sizeRequested(){return _reqSize;}
	size_t sizeUsed(){return _actualSize;}
	static int      _systemPageSize;
	//other system info can be stored here in advance ...
private:
	size_t _reqSize;
	size_t _actualSize;	
	bool _virtualBlock;
	void* _memory;
};

class klVMemMgr: public klMemMgr{
public:
	klVMemMgr(unsigned long maxAllocate =0)
	{
		_maxAllocate=maxAllocate;
		_committed=0;
		_reserved=0;
		_free=0;//set to system memory free later
		
		_freeMap=new multimap<size_t,klVMemBlock*>;
		_committedMap=new 	multimap<void*,klVMemBlock*>;


	}

	~klVMemMgr()
	{
		multimap<size_t,klVMemBlock*>::iterator free_position;
		multimap<void*,klVMemBlock*>::iterator comitted_position;
		for(free_position=_freeMap->begin();free_position!=_freeMap->end();free_position++)
			delete free_position->second;
		for(comitted_position=_committedMap->begin();comitted_position!=_committedMap->end();comitted_position++)
			delete comitted_position->second;

		delete _freeMap;
		delete _committedMap;
	}

	void *allocate(size_t size)
	{
//	guard bbcrevisit put klguard

	klVMemBlock	*vptr;

	// look to see if we have an entry on the free list
	multimap<size_t,klVMemBlock*>::iterator iT = _freeMap->find( size );


	// if none available
	if( iT == _freeMap->end() )
	{
		vptr = new klVMemBlock(size);
		_committed += vptr->sizeUsed();
	}
	else
	{
		vptr = (*iT).second;
		_freeMap->erase(iT);
		_free -= vptr->sizeUsed();
	}

	// came up empty...
	if (!vptr)
		return (NULL);

	// no data ...  almost empty
	if (!vptr->memory())
	{
		delete vptr;
		return (NULL);
	}

	// place the buffer on the in use list
	multimap<void*,klVMemBlock*>::value_type element(vptr->memory(),vptr);
	_committedMap->insert(element);

	_allocatedBlocks++; // 1 more buffer allocated
	return (vptr->memory());
	}


	bool free(void *buffer)
	{
	//guard

	klVMemBlock	*vptr;
	multimap<void*,klVMemBlock*>::iterator iT = _committedMap->find(buffer);

	if (iT == _committedMap->end())
	{
		// critical error !!
		// if the buffer is in use, we should have found it on the in use list.
		// therefore the pointer does not belong to this memory manager
		return (false);
	}

	// detach from the in use list
	vptr = (*iT).second;
	_committedMap->erase(iT);
	_committed -= vptr->sizeUsed();
	
	if( _committed > _maxAllocate )
	{
		_committed -= vptr->sizeUsed();
		delete vptr;
	}
	else
	{
		multimap<size_t,klVMemBlock*>::value_type element(vptr->sizeRequested(),vptr);
		_freeMap->insert(element);
		_free += vptr->sizeUsed();
	}
	
	_allocatedBlocks--;  
	return (true); 

	}

	unsigned long committed(){return _committed;}
	unsigned long available(){return _free;}
	unsigned long reserved() {return _reserved;}

	unsigned long getMaxAllocate(){return _maxAllocate;}

	void setMaxAllocate( unsigned long maxAllocate ){_maxAllocate=maxAllocate;}

private:

	multimap<size_t,klVMemBlock*>*	_freeMap;
	multimap<void*,klVMemBlock*>*	_committedMap;
	unsigned long _committed;
	unsigned long _free;
	unsigned long _reserved;
	unsigned long _maxAllocate;
	unsigned long _allocatedBlocks;	
};


class klAWEMemBlock{
	friend class klAWEMemMgr;
public:
	klAWEMemBlock(size_t initSize)
	{
		SYSTEM_INFO     sysinfo;
		if (!_systemPageSize)
		{
			GetSystemInfo( &sysinfo );
			_systemPageSize = sysinfo.dwPageSize;        // pagesize
		}
		
		_memory=lpNextFreeAddress;
		_reqSize=initSize;
		unsigned char* as;
		unsigned char* bs;//
		as=(unsigned char*)lpNextFreeAddress+initSize;
		lpNextFreeAddress=as;
		bs=(unsigned char*)lpMemReserved;
		long overrun=( as-bs )-NumberOfPagesInitial*_systemPageSize;
		
		if(overrun>0)
			_actualSize=initSize-overrun;
		else
			_actualSize=initSize;
		
		if(_actualSize!=initSize)//we've run out of room and haven't mapped the full amount of requested memory
		{
			ANSI_INFO; throw klError(err + "klAWEMemMgr ran out of physical memory.");
		}		
	}	
	~klAWEMemBlock()
	{
		
		
	}
	
	void* memory(){return _memory;}
	size_t sizeRequested(){return _reqSize;}
	size_t sizeUsed(){return _actualSize;}
	static int      _systemPageSize;
private:
	size_t _reqSize;
	size_t _actualSize;	
	void* _memory;
protected:
	static ULONG_PTR NumberOfPagesInitial; // initial number of pages requested
	static ULONG_PTR *aPFNs;               // page info; holds opaque data
	static PVOID lpMemReserved;            // AWE window
	static PVOID lpNextFreeAddress;            // AWE window

};

//This manager was in development prior to porting to x64, it works, but the windowing needs to be implemented.
class klAWEMemMgr: public klMemMgr{
	
public:

	klAWEMemMgr(unsigned long maxAllocate)
	{
		_maxAllocate=maxAllocate;
		_committed=0;
		_reserved=0;
		_free=0;//set to system memory free later
		_freeMap=new multimap<size_t,klAWEMemBlock*>;
		_committedMap=new 	multimap<void*,klAWEMemBlock*>;
		
		GetSystemInfo(&sSysInfo);  // fill the system information structure
		
		NumberOfPages = maxAllocate/sSysInfo.dwPageSize;
		
		PFNArraySize = NumberOfPages * sizeof (unsigned long*);
		
		aPFNs = (ULONG_PTR *) HeapAlloc (GetProcessHeap (), 0, PFNArraySize);
		
		if (aPFNs == NULL) {
			return;
		}
		
		if( ! setLockPagesPrivilege( GetCurrentProcess(), true ) ) 
		{
			ANSI_INFO; throw klError(err + "klAWEMemMgr needs the lock pages security attribute;Computer Configuration\\Windows Settings\\Security Settings\\Local Policies\\User Rights Assignment\\Lock Pages in Memory");
		}
		
		NumberOfPagesInitial = NumberOfPages;
		bResult = AllocateUserPhysicalPages( GetCurrentProcess(), &NumberOfPages,  aPFNs );
		
		if( bResult != true ) 
		{
			return;
		}
		
		if( NumberOfPagesInitial != NumberOfPages ) 
		{
			return;
		}
		lpMemReserved = VirtualAlloc( NULL, maxAllocate,MEM_RESERVE | MEM_PHYSICAL,PAGE_READWRITE );

		
		klAWEMemBlock::NumberOfPagesInitial=NumberOfPagesInitial; // initial number of pages requested
		klAWEMemBlock::aPFNs=aPFNs;               // page info; holds opaque data
		klAWEMemBlock::lpMemReserved=lpMemReserved;            // AWE window
		klAWEMemBlock::lpNextFreeAddress=lpMemReserved;
		if( lpMemReserved == NULL ) 
		{
			return;
		}
		
		//map pages
		bResult = MapUserPhysicalPages( lpMemReserved,NumberOfPages,aPFNs );
		
		if( bResult != TRUE ) 
		{
			return;
		}
		
		//unmap
		
		
		
	}

	~klAWEMemMgr()
	{
		bResult = MapUserPhysicalPages( lpMemReserved,NumberOfPages,NULL );
		
		if( bResult != TRUE ) 
		{
			return;
		}
		bResult = FreeUserPhysicalPages( GetCurrentProcess(),&NumberOfPages,aPFNs );
		
		if( bResult != TRUE ) 
		{
			return;
		}
		
		multimap<size_t,klAWEMemBlock*>::iterator free_position;
		multimap<void*,klAWEMemBlock*>::iterator comitted_position;
		for(free_position=_freeMap->begin();free_position!=_freeMap->end();free_position++)
			delete free_position->second;
		for(comitted_position=_committedMap->begin();comitted_position!=_committedMap->end();comitted_position++)
			delete comitted_position->second;
		
		
		
		// Free virtual memory.
		
		bResult = VirtualFree( lpMemReserved,0,MEM_RELEASE );
		delete _freeMap;
		delete _committedMap;
		HeapFree (GetProcessHeap (), 0, aPFNs);
	}
	static bool setLockPagesPrivilege ( HANDLE hProcess,bool bEnable)
	{
		struct {
			DWORD Count;
			LUID_AND_ATTRIBUTES Privilege [1];
		} Info;
		
		HANDLE Token;
		BOOL Result;
		
		// Open the token.
		
		Result = OpenProcessToken ( hProcess, TOKEN_ADJUST_PRIVILEGES, & Token);
		
		if( !Result  ) 
		{
			return FALSE;
		}
		Info.Count = 1;
		if( bEnable ) 
		{
			Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
		} 
		else 
		{
			Info.Privilege[0].Attributes = 0;
		}
		Result = LookupPrivilegeValue ( NULL, SE_LOCK_MEMORY_NAME,&(Info.Privilege[0].Luid));
		
		if( Result != TRUE ) 
		{
			return FALSE;
		}
		
		Result = AdjustTokenPrivileges ( Token, FALSE, (PTOKEN_PRIVILEGES) &Info, NULL, NULL, NULL);
		
		if( Result != TRUE ) 
		{
			return FALSE;
		} 
		else 
		{
			unsigned long err=GetLastError();

			//  Not all privileges referenced are assigned to the caller.
			if(err==ERROR_NOT_ALL_ASSIGNED)
			{
				return false;
			}
		
		}
		CloseHandle( Token );
		return true;
	}

	void *allocate(size_t size)
	{
//	guard bbcrevisit put klguard here


	klAWEMemBlock	*vptr;

	// look to see if we have an entry on the free list
	multimap<size_t,klAWEMemBlock*>::iterator iT = _freeMap->find( size );


	// if none available
	if( iT == _freeMap->end() )
	{
		vptr = new klAWEMemBlock(size);
		_committed += vptr->sizeUsed();
	}
	else
	{
		vptr = (*iT).second;
		_freeMap->erase(iT);
		_free -= vptr->sizeUsed();
	}

	if (!vptr)
		return (NULL);

	if (!vptr->memory())
	{
		delete vptr;
		return (NULL);
	}

	// place the buffer on the in use list
	multimap<void*,klAWEMemBlock*>::value_type element(vptr->memory(),vptr);
	_committedMap->insert(element);

	_allocatedBlocks++; // 1 more buffer allocated
	return (vptr->memory());

	}
	bool free(void *buffer)
	{
	//guard

	klAWEMemBlock	*vptr;
	multimap<void*,klAWEMemBlock*>::iterator iT = _committedMap->find(buffer);

	if (iT == _committedMap->end())
	{
		// critical error !!
		// if the buffer is in use, we should have found it on the in use list.
		// therefore the pointer does not belong to this memory manager
		return (false);
	}

	// detach from the in use list
	vptr = (*iT).second;
	_committedMap->erase(iT);
	_committed -= vptr->sizeUsed();
	
	if( _committed > _maxAllocate )
	{
		_committed -= vptr->sizeUsed();
		delete vptr;
	}
	else
	{
		multimap<size_t,klAWEMemBlock*>::value_type element(vptr->sizeRequested(),vptr);
		_freeMap->insert(element);
		_free += vptr->sizeUsed();
	}
	
	_allocatedBlocks--;  
	return (true); 

	}

	unsigned long committed()     {return _committed;}
	unsigned long available() {return _free;}
	unsigned long reserved() {return _reserved;}

	unsigned long getMaxAllocate(){return _maxAllocate;}

	void setMaxAllocate( unsigned long maxAllocate ){_maxAllocate=maxAllocate;}

private:

	multimap<size_t,klAWEMemBlock*>*	_freeMap;
	multimap<void*,klAWEMemBlock*>*	_committedMap;
	unsigned long _committed;
	unsigned long _free;
	unsigned long _reserved;
	unsigned long _maxAllocate;
	unsigned long _allocatedBlocks;	
	bool bResult;                   
	ULONG_PTR NumberOfPages;        // number of pages to request
	ULONG_PTR NumberOfPagesInitial; // initial number of pages requested
	ULONG_PTR *aPFNs;               // page info; holds opaque data
	PVOID lpMemReserved;            // AWE window
	SYSTEM_INFO sSysInfo;           // useful system information
	int PFNArraySize;               // memory to request for PFN array
};


#include <windows.h>
#pragma warning(disable: 4800)
#pragma warning(disable: 4101)
#include <assert.h>
#include <new>
#include <crtdbg.h>
#include <string>
#include <stdio.h>
#include <malloc.h>
#define KLMEM_HEAD_STRSIZE 256
#define KLMEM_HEAD_TIMESIZE 256
#define KLMEM_HEAD_GUARD_BYTES 0xABABABABABABABAB
#define KLMEM_FOOT_GUARD_BYTES 0xEFEFEFEFEFEFEFEF

#define KLMEM_MAGIC_NUMBER 0x4005bf0a8b14576a  //hex representation of e
using namespace std;

inline char* ConvertToHex( unsigned char* Ptr, char* buffer, __int64 size )
{
	char	*bPtr = buffer ;
    char	*HexChars = "0123456789ABCDEF" ;

	*bPtr = '\0' ;
	long	loop ;
	
	if( size > 0 )
	{
		for( loop = 0 ; loop < size ; loop++ )
		{
			int		c1 = ((*Ptr) & 0xF0) >> 4 ;
			int		c2 = ((*Ptr) & 0x0F) ;
			*bPtr++ = HexChars[c1] ;
			*bPtr++ = HexChars[c2] ;
			*bPtr++ = ' ' ;
			Ptr++ ;
		}
	}
	*bPtr = 0 ;
	return( buffer ) ;
}
struct klHeader
{
	char		fileName[KLMEM_HEAD_STRSIZE];		// filename where allocation occurs
	int 		lineNum;						// line number where allocation occurs
	__int64		bufferLen;						// length of memory allocation
	__int64		magicNumber;					// special magic number for safety
	__int64		guardBytes;	// guard bytes for safety
//  ... for fututre functionality	
//	char		className[KLMEM_HEAD_STRSIZE];	// RTTI class name (if available)
//	__int64		numItems;						// number of items (if available)
//	char		timeStamp[KLMEM_HEAD_TIMESIZE];	// time stamp of new allocation
//	void		*memBlockPtr;					// pointer to static list of allocations
//	int			listIndex;						// location in static list of allocations
	
};

struct klFooter{
	__int64	guardBytes;		
	//
};
class klHeapMgr;
void* klHeapAlloc(size_t inSize,void* theHeap);
bool klHeapFree(void* inPtr,void* theHeap);
void* klRedZoneHeapAlloc(size_t inSize,void* theHeap, const char *inFile, int inLineNum);  
void* klRedZoneHeapAlloc(size_t inSize,void* theHeap) ; 
bool klRedZoneHeapFree(void *inPtr, void* theHeap,const char *inFile, int inLineNum);
bool klRedZoneHeapFree(void *inPtr, void* theHeap);

class klHeapMgr: public klMemMgr{
	
public:

	klHeapMgr() {
		//if _DEBUG is not define, the preprocessor removes the _RPT0 and _CrtSetDbgFlag calls
		//_RPT0( _CRT_WARN, "klMemory: setting _CRTDBG_CHECK_ALWAYS_DF and _CRTDBG_DELAY_FREE_MEM_DF\n");

		//Setting this flag enables the C Run Time Library to check it's own safety zones it puts at the beginning and end of memory blocks.
		//_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);

		//Setting this flag delays freeing memory.  This facilitates checking for errant memory writes.
		//_CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF);

		pHeap=NULL;
	}

	~klHeapMgr(){
		//_RPT0( _CRT_WARN, "klMemory: setting _CRTDBG_LEAK_CHECK_DF\n");
		//_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
	} 

	void* klHeapInit(long initSize) {
		if(!klHeapIsCreated())
			pHeap=HeapCreate(HEAP_GENERATE_EXCEPTIONS,initSize,0); 
		 return pHeap;}

	
	//User calls this 
	bool klHeapCheck() {return HeapValidate(pHeap,0,0);}
	
	//User can call this on any pointer returned by our operator new
	bool klHeapCheckBlock(void* mem){  return HeapValidate(pHeap,0,reinterpret_cast<void*>(int(mem)));	
	}
	long klHeapCompact() { return HeapCompact(pHeap,0);}

	void* allocate(size_t size)
	{ 	
		if(!klHeapIsCreated())
			klHeapInit(0);
		return klHeapAlloc(size,pHeap);
	}

	bool klHeapDestroy(){if(pHeap)return HeapDestroy(pHeap);else return false;}

	void* klHeapHandle(){return pHeap;}

	bool free(void* pMem) {return klHeapFree(pMem, pHeap); }

	bool klHeapIsCreated() {return pHeap?true:false;} 

	void klHeapDump(char* file){
	char HexData[1024];
	char HdrData[1024];
	char	DumpHdr[1024] ;
	char	DumpTlr[1024] ;
	PROCESS_HEAP_ENTRY hinfo;
	int heapstatus;
	hinfo.lpData  = NULL;
	FILE* fp = fopen(file, "a");
	if( ! fp ) return ;
	fprintf(fp, "===========================================================================\n");
	fprintf(fp, "===========================================================================\n");
	long	count = 0 ;
	while( ( heapstatus = HeapWalk( pHeap, &hinfo ) ) )
	{
		long	 isBlock  = hinfo.wFlags & PROCESS_HEAP_ENTRY_BUSY ;
		*HdrData = '\0';
		if( isBlock )
		{
			char	*theMem = (char *) hinfo.lpData ;
			sprintf( HdrData, "%6ld %1s %Fp %6.6ld ", count++, "U", hinfo.lpData, hinfo.cbData ) ;
		}
		else
		{
			fprintf( fp, "Not an allocated block.  May have been freed already see about _CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF) %s\n", HdrData ) ;
		}
		switch( heapstatus )
		{
		case _HEAPEMPTY:	fprintf( fp, "OK - empty klMemory heap\n\n\n" );				break;
		case _HEAPEND:		fprintf( fp, "OK - end of klMemory heap\n\n\n" );				break;
		case _HEAPBADPTR:	fprintf( fp, "ERROR - bad pointer to klMemory heap\n\n\n" );	break;
		case _HEAPBADBEGIN:	fprintf( fp, "ERROR - bad start of klMemory heap\n\n\n" );		break;
		case _HEAPBADNODE:	fprintf( fp, "ERROR - bad node in klMmory heap\n\n\n" );		break;
		}
	}
	fclose( fp ) ;
}
	
protected:
	void* pHeap;
};



//This heap puts a klHeader at the beginning of each allocated block, and a klFooter at the end of each allocated block.
//The information in the header will depend on the build configuration.  Debug builds will have line and file number information 
//indicating where an allocation request originated.

class klRedZoneHeapMgr: public klHeapMgr
{
public:
	klRedZoneHeapMgr() : klHeapMgr()
	{
	}

	~klRedZoneHeapMgr()
	{
	} 

	//User can call this on any pointer returned by our operator new
	bool klHeapCheckBlock(void* mem)
	{
		 return HeapValidate(pHeap,0,reinterpret_cast<void*>(int(mem)-sizeof(klHeader) ) ) ;
	}

	void* allocate(size_t size)
	{ 
		if(!klHeapIsCreated())
			klHeapInit(0);
		return klRedZoneHeapAlloc(size,pHeap) ;
	}

	void* allocate(size_t size, const char *inFile, int inLineNum) 
	{
		if(!klHeapIsCreated())
			klHeapInit(0);
		return  klRedZoneHeapAlloc(size,pHeap,inFile,inLineNum); 
	}

	//This should only be called by our operator delete override
	bool free(void* pMem) {  return klRedZoneHeapFree(pMem,pHeap); }

	bool free(void* pMem, const char *inFile, int inLineNum)  {  return klRedZoneHeapFree(pMem,pHeap,inFile,inLineNum); }

	void klHeapDump(char* file){
		char HexData[1024];
		char HdrData[1024];
		char	DumpHdr[1024] ;
		char	DumpTlr[1024] ;
		PROCESS_HEAP_ENTRY hinfo;
		int heapstatus;
		hinfo.lpData  = NULL;
		FILE* fp = fopen(file, "a");
		if( ! fp ) return ;
		fprintf(fp, "===========================================================================\n");
		fprintf(fp, "===========================================================================\n");
		long	count = 0 ;
		while( ( heapstatus = HeapWalk( pHeap, &hinfo ) ) )
		{
			long	 isBlock  = hinfo.wFlags & PROCESS_HEAP_ENTRY_BUSY ;
			*HdrData = '\0';
			if( isBlock )
			{
				char	*theMem = (char *) hinfo.lpData ;
				sprintf( HdrData, "%6ld %1s %Fp %6.6ld ", count++, "U", hinfo.lpData, hinfo.cbData ) ;
				klHeader*	theHdr = (klHeader *)  theMem ;
				char	*DataStart = theMem + sizeof(klHeader) ;
				__int64	 DataSize  = theHdr->bufferLen ;
				klFooter* theTlr =reinterpret_cast<klFooter*>((int)theMem+sizeof(klHeader)+DataSize); ;
				sprintf( DumpHdr, ("Header: %I64X, %I64d,  %s,  %ld"),theHdr->guardBytes, theHdr->bufferLen, theHdr->fileName,theHdr->lineNum) ;
				sprintf( DumpTlr, ("Footer: %I64X"), theTlr->guardBytes) ;
				if( theHdr->magicNumber == KLMEM_MAGIC_NUMBER )
				{
					sprintf( DumpHdr, ("Header: %I64X, %I64d,  %s,  %ld"),theHdr->guardBytes, theHdr->bufferLen, theHdr->fileName,theHdr->lineNum) ;
					sprintf( DumpTlr, ("Footer: %I64X"), theTlr->guardBytes) ;
					fprintf( fp, "%s", HdrData);
					fprintf( fp, "[%s]\t",ConvertToHex( (unsigned char *)(DataStart), HexData,(DataSize > 256) ? 256 : DataSize )) ;
					fprintf( fp, "\t%s\t", DumpHdr) ;
					fprintf( fp, "\t%s\n", DumpTlr) ;
				}
				else
				{
					fprintf( fp, "\t%s\t", DumpHdr) ;
					fprintf( fp, "\t%s\n", DumpTlr) ;
					fprintf(fp,"We have a problem, walking the fwHeap and found a block without a magic number!");
				}
			}
			else
			{
				fprintf( fp, "Not an allocated block.%s\n", HdrData ) ;
			}
			switch( heapstatus )
			{
			case _HEAPEMPTY:	fprintf( fp, "OK - empty fwMemory heap\n\n\n" );				break;
			case _HEAPEND:		fprintf( fp, "OK - end of fwMemory heap\n\n\n" );				break;
			case _HEAPBADPTR:	fprintf( fp, "ERROR - bad pointer to fwMemory heap\n\n\n" );	break;
			case _HEAPBADBEGIN:	fprintf( fp, "ERROR - bad start of fwMemory heap\n\n\n" );		break;
			case _HEAPBADNODE:	fprintf( fp, "ERROR - bad node in fwMmory heap\n\n\n" );		break;
			}
		}
		
		fclose( fp ) ;
		
}
};




//////////////////////////////////////////KL Operator new overload implementation////////////////////////////
/*
Description:
	The code below allows one to override the global operator new and delete and put heap allocations on a private heap.
	This heap can be managed throught the interface provided by klHeap.  Use the instructions below to instrument the source files
	and then manage through the global static variable - extern class klHeap theKLOperatorNewHeap;

Instructions: 
	#define KL_OPERATOR_NEW in your build settings for all source files that you want to have heap allocations put on  the private heap.
	Include this header before any others
	Don't mix this with MFC - ie you can't have a source file including MFC headers and this one as well.
*/
//#ifdef KL_OPERATOR_NEW

	//extern class klHeapMgr theKLOperatorNewHeap;


class klMemError: public exception{
public:
	klMemError(string Err): msg(Err){;}
	const char* what(){return (const char*)(msg.c_str());}
	string msg;
};



/*
// standard new overload
inline void *operator new(size_t inSize)
{
	return klStdNewHeader(inSize); 
}

// standard new [] overload
inline void *operator new [](size_t inSize)
{ 	
	return klStdNewHeader(inSize);
}

//standard delete overload
inline void operator delete(void *inPtr)
{ 	
	klStdDeleteHeader(inPtr);		
}

// standard delete [] overload
inline void operator delete [](void *inPtr)
{
	klStdDeleteHeader(inPtr);
}
*/
/* //These overloads of operator new are used in a debug build 
   //They conflict with overloads in MFC for debug builds, so there is some funny
   //buisiness that one must go through in order to instrument a module with this new
   //that must also include MFC headers.  Unedfine new. and then include the MFC header.
   //That means the module will be instumented with the operator new MFC uses, and will
   //consequently have all of its heap memory allocated on the global heap.
inline void *operator new(size_t inSize, const char *inFile, int inLineNum)
{
 	return klNewHeader(inSize,inFile,inLineNum); 
}

// special operator new [] overload
inline void *operator new [](size_t inSize, const char *inFile, int inLineNum)
{
 	return klNewHeader(inSize,inFile,inLineNum); 
}


// special operator delete overload
inline void operator delete(void *inPtr, const char *inFile, int inLineNum)
{
	klDeleteHeader(inPtr,inFile,inLineNum);
}

// special operator delete[] overload
inline void operator delete[](void *inPtr, const char *inFile, int inLineNum)
{
	klDeleteHeader(inPtr,inFile,inLineNum);
}



// this macro does the magic that picks up the right operator new taking line number and module name as parameters.
#define new		new (__FILE__, __LINE__)
*/

inline void* klRedZoneHeapAlloc(size_t inSize,void* theHeap)  
//This version gets called in a release configuration
{
	void* theMem = 	NULL;
	theMem= HeapAlloc(theHeap,0,int(inSize)+sizeof(klHeader)+sizeof(klFooter));
	if(!theMem)
	{
		MEMORYSTATUS _memoryInfo;

		GlobalMemoryStatus (&_memoryInfo);

		MEMORYSTATUSEX _memoryInfoEx;
		_memoryInfoEx.dwLength=sizeof(_MEMORYSTATUSEX);
		GlobalMemoryStatusEx (&_memoryInfoEx);

		unsigned int RelativeMemoryLoad=_memoryInfoEx.dwMemoryLoad; 
		unsigned int TotalPhysicalMemory=  _memoryInfoEx.ullTotalPhys; 
		unsigned int memAvail =  _memoryInfoEx.ullAvailPhys; 
		unsigned int TotalPageFile= _memoryInfoEx.ullTotalPageFile; 
		unsigned int AvailablePagefile= _memoryInfoEx.ullAvailPageFile; 
		unsigned int TotalVirtualMemory= _memoryInfoEx.ullTotalVirtual; 
		unsigned int AvailableVirtualMemory= _memoryInfoEx.ullAvailVirtual; 
		unsigned int AvailableExtendedVirtualMemory= _memoryInfoEx.ullAvailExtendedVirtual;

		//This checks the globak heap status
		//int heapstatus = _heapchk();
		
		long compactResult  = HeapCompact(theHeap,0);
		//retry
		theMem= HeapAlloc(theHeap,0,int(inSize)+sizeof(klHeader)+sizeof(klFooter));
		if(!theMem)
		{
			ANSI_INFO; throw klError(err + "Bad heap alloc in klRedZoneHeapAlloc(size_t inSize,void* theHeap)");
		}
		
	}
	klHeader* pHeader= (klHeader*)theMem;
	pHeader->magicNumber=KLMEM_MAGIC_NUMBER;
	pHeader->guardBytes=KLMEM_HEAD_GUARD_BYTES;
	pHeader->bufferLen=inSize;
	klFooter* pFooter=reinterpret_cast<klFooter*>((int)theMem+sizeof(klHeader)+inSize);

	pFooter->guardBytes=KLMEM_FOOT_GUARD_BYTES;
	return reinterpret_cast<void*>((int)theMem+sizeof(klHeader));
}
inline void* klRedZoneHeapAlloc(size_t inSize,void* theHeap, const char *inFile, int inLineNum)  
//This version gets called in a debug configuration
{
	void* theMem = NULL;
	theMem=HeapAlloc(theHeap,0,int(inSize)+sizeof(klHeader)+sizeof(klFooter));
	if(!theMem)
	{
		//compact
		//retry
		//throw error if fail
	}
	klHeader* pHeader= (klHeader*)theMem;
	pHeader->magicNumber=KLMEM_MAGIC_NUMBER;
	pHeader->guardBytes=KLMEM_HEAD_GUARD_BYTES;
	pHeader->bufferLen=inSize;
	int ccount=0;
	do{

		if( ccount==KLMEM_HEAD_STRSIZE-1)
			pHeader->fileName[ccount]='\0';
		else
			pHeader->fileName[ccount]=*(inFile+ccount);
		ccount++;
	}while(*(inFile+ccount-1)!='\0' && ccount!=KLMEM_HEAD_STRSIZE);
    pHeader->lineNum=inLineNum;
	klFooter* pFooter=reinterpret_cast<klFooter*>((int)theMem+sizeof(klHeader)+inSize);
	pFooter->guardBytes=KLMEM_FOOT_GUARD_BYTES;
	return reinterpret_cast<void*>((int)theMem+sizeof(klHeader));

}

	
inline bool klRedZoneHeapFree(void* inPtr,void* theHeap)
//This version gets called in a release configuration.
{
	if(inPtr==NULL)
		return false;
	klHeader* pHeader=reinterpret_cast<klHeader*>(int(inPtr)-sizeof(klHeader));
	klFooter* pFooter= reinterpret_cast<klFooter*>(int(inPtr)+pHeader->bufferLen);
	if(pHeader->magicNumber==KLMEM_MAGIC_NUMBER)
	{
		if(pHeader->guardBytes==KLMEM_HEAD_GUARD_BYTES && pFooter->guardBytes==KLMEM_FOOT_GUARD_BYTES)
		{
			return HeapFree(theHeap,0,pHeader);
		}
			
		else
		{
			string msg("klMemmory error in ");
			msg+=pHeader->fileName;  
			string df(pHeader->fileName);
			char ln[20];itoa(pHeader->lineNum,ln,10);
			msg+=ln;
			msg+="\n No info on file and line number of free attempt.\n ";
			klMemError e(msg);
			throw e;
		}
	}

	else
	{
		free(inPtr);//last ditch attempt...should not get here in a perfect world.
		return false;
	}

}

inline bool klRedZoneHeapFree(void *inPtr, void* theHeap,const char *inFile, int inLineNum)
// This version gets called in a debug configuration
{
	if(inPtr==NULL)
		return false;
	klHeader* pHeader=reinterpret_cast<klHeader*>(int(inPtr)-sizeof(klHeader));
	klFooter* pFooter= reinterpret_cast<klFooter*>(int(inPtr)+pHeader->bufferLen);
	if(pHeader->magicNumber==KLMEM_MAGIC_NUMBER)
	{
		if(pHeader->guardBytes==KLMEM_HEAD_GUARD_BYTES && pFooter->guardBytes==KLMEM_FOOT_GUARD_BYTES)
			return HeapFree(theHeap,0,inPtr);
		else
		{
			string msg("klMemmory error in block allocated at:");
			msg+=pHeader->fileName;  
			msg+=" line number ";
			char ln[20];itoa(pHeader->lineNum,ln,10);
			msg+=ln;
			msg+="\n Block freed at: ";
			msg+=inFile; itoa(inLineNum,ln,10); msg+=ln;
			klMemError e(msg);
			throw e;
		}
	}	
	else
	{
		free(inPtr);	
		return false;
	}

}


inline void* klHeapAlloc(size_t inSize,void* theHeap)
{
	void* theMem = NULL;
	theMem= HeapAlloc(theHeap,0,inSize);
	if(!theMem)
	{
		//compact
		//retry
		//throw error if fail
	}
	return theMem;
}
	
inline bool klHeapFree(void* inPtr,void* theHeap)
{
	if(inPtr==NULL)
		return false;
	bool status= HeapFree(theHeap,0,inPtr);
	if(!status)
	{
		//GetLastError
		//Throw or store mem error?
	}
	return status;

}


///////////////////////////////////////////////////Low Memory Warning Handler//////////////
//The CreateMemoryResourceNotification function creates a memory resource notification object.

/* From MSDN 
Parameters
NotificationType 
[in] Memory condition under which the object is to be signaled. 
This parameter can be one of the following values. Value Meaning 
LowMemoryResourceNotification Available physical memory is running low. 
HighMemoryResourceNotification Available physical memory is high. 

Return Values
If the function succeeds, the return value is a handle to a memory resource notification object.
If the function fails, the return value is NULL. To get extended error information, call GetLastError.

Remarks
Applications can use memory resource notification events to scale their memory usage as appropriate. 
If available memory is low, the application can reduce its working set. If available memory is high, 
the application can allocate more memory.
Any thread of the calling process can specify the memory resource notification handle in a call to the 
QueryMemoryResourceNotification function or one of the wait functions. The state of the object is signaled 
when the specified memory condition exists. This is a system-wide event, so all applications receive notification 
when the object is signaled. Note that there is a range of memory availability where neither the LowMemoryResourceNotification 
or HighMemoryResourceNotification object is signaled. In this case, applications should attempt to keep their memory use constant.

The default level of available memory that signals a low-memory-resource notification event is approximately 32 MB per 4 GB, 
to a maximum of 64 MB. The default level that signals a high-memory-resource notification event is three times the default low-memory value.

Use the CloseHandle function to close the handle. The system closes the handle automatically when the process terminates. The memory resource notification object is destroyed when its last handle has been closed.

To compile an application that uses this function, define the _WIN32_WINNT macro as 0x0501 or later. For more information, see Using the SDK Headers.
*/
//////////////////////////////////////////////////////////////////////////////////////////

void klCreateLowMemoryResourceNotification(void);

bool klQueryLowMemoryResourceNotification(int* answer);

#include "mkl.h"

/*
Best Buffer Aligment Per Intel 
	128-bit register is in SSE2= require 16-byte alignment
	256-bit register is in AVX, AVX2= require 32-byte alignment
	512-bit register for MIC, = require 64-byte alignment
*/

class klMKLMemMgr{
public:
	virtual void* allocate(size_t size)
	{
		//void* mkl_malloc (size_t alloc_size, int alignment);
		void* ptr=NULL;
		ptr = mkl_malloc(size,64);
		return ptr;
	};
	virtual bool free(void* ptr)
	{
		mkl_free(ptr);
		return true;
	};
	
	//Frees unused memory allocated by the Intel MKL Memory Allocator.
	void freeAllBuffers()
	{
			mkl_free_buffers();
	}

	 //Frees unused memory allocated by the Intel MKL Memory Allocator in the current thread.
	void freeCurrentThreadBuffers()
	{
		mkl_thread_free_buffers();
	}

	//mkl_disable_fast_mm //Turns off the Intel MKL Memory Allocator for Intel MKL functions to directly use the system malloc/free functions.
	
	 //Reports the status of the Intel MKL Memory Allocator.
	__int64 allocatorStatus()
	{
		int allocatedBytes=0;//bbcrevisit
		__int64 buffers =mkl_mem_stat(&allocatedBytes);
		return buffers;
	}	

	//This may be new
	//Reports the peak memory allocated by the Intel MKL Memory Allocator.
	//__int64 getPeakMemoryUse()
	//{
	//	//MKL_PEAK_MEM_ENABLE - start gathering the peak memory data
	//	//MKL_PEAK_MEM_DISABLE - stop gathering the peak memory data
	//	//MKL_PEAK_MEM - return the peak memory
	//	//MKL_PEAK_MEM_RESET
	//	int mode =MKL_PEAK_MEM;
	//	__int64 peakMem=0;
	//	peakMem=mkl_peak_mem_usage(mode);
	//	return peakMem;
	//}
//mkl_calloc //Allocates and initializes an aligned memory buffer.
//mkl_realloc //Changes the size of memory buffer allocated by mkl_malloc/mkl_calloc.

};

#endif //__kl_memory__        