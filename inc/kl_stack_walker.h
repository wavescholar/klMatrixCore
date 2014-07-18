 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#ifndef __kl_SEH__
#define __kl_SEH__

#include <iostream>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <dbghelp.h>


//The stak walker was originally designed to work with structured exception handling. 
//The intent was to generate a full state report inside of termination handlers.
//Misrosoft reccomends using standard C++ exceptions, and we've stopped compiling 
//with SEH enabled. The stalk walker is still useful for collecing state info when handling exceptions. 

class klStackWalkerInternal; 

class klStackWalkBase
{
public:
  typedef enum StackWalkOptions
  {
    // No addition info will be retrived 
    // (only the address is available)
    RetrieveNone = 0,
    
    // Try to get the symbol-name
    RetrieveSymbol = 1,
    
    // Try to get the line for this symbol
    RetrieveLine = 2,
    
    // Try to retrieve the module-infos
    RetrieveModuleInfo = 4,
    
    // Also retrieve the version for the DLL/EXE
    RetrieveFileVersion = 8,
    
    // Contains all the abouve
    RetrieveVerbose = 0xF,
    
    // Generate a "good" symbol-search-path
    SymBuildPath = 0x10,
    
    // Also use the public Microsoft-Symbol-Server
    SymUseSymSrv = 0x20,
    
    // Contains all the abouve "Sym"-options
    SymAll = 0x30,
    
    // Contains all options (default)
    OptionsAll = 0x3F
  } StackWalkOptions;

  klStackWalkBase(
    int options = OptionsAll, // 'int' is by design, to combine the enum-flags
    LPCSTR szSymPath = NULL, 
    DWORD dwProcessId = GetCurrentProcessId(), 
    HANDLE hProcess = GetCurrentProcess()
    );
  klStackWalkBase(DWORD dwProcessId, HANDLE hProcess);
  
  virtual ~klStackWalkBase();

  typedef BOOL (__stdcall *PReadProcessMemoryRoutine)(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead,
    LPVOID      pUserData  // optional data, which was passed in "ShowCallstack"
    );

  BOOL LoadModules();

  BOOL ShowCallstack(
    HANDLE hThread = GetCurrentThread(), 
    const CONTEXT *context = NULL, 
    PReadProcessMemoryRoutine readMemoryFunction = NULL,
    LPVOID pUserData = NULL  // optional to identify some data in the 'readMemoryFunction'-callback
    );


#if _MSC_VER >= 1300
// due to some reasons, the "STACKWALK_MAX_NAMELEN" must be declared as "public" 
// in older compilers in order to use it... starting with VC7 we can declare it as "protected"
protected:
#endif
	enum { STACKWALK_MAX_NAMELEN = 1024 }; // max name length for found symbols

protected:
  // Entry for each Callstack-Entry
  typedef struct CallstackEntry
  {
    DWORD64 offset;  // if 0, we have no valid entry
    CHAR name[STACKWALK_MAX_NAMELEN];
    CHAR undName[STACKWALK_MAX_NAMELEN];
    CHAR undFullName[STACKWALK_MAX_NAMELEN];
    DWORD64 offsetFromSmybol;
    DWORD offsetFromLine;
    DWORD lineNumber;
    CHAR lineFileName[STACKWALK_MAX_NAMELEN];
    DWORD symType;
    LPCSTR symTypeString;
    CHAR moduleName[STACKWALK_MAX_NAMELEN];
    DWORD64 baseOfImage;
    CHAR loadedImageName[STACKWALK_MAX_NAMELEN];
  } CallstackEntry;

  typedef enum CallstackEntryType {firstEntry, nextEntry, lastEntry};

  virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
  virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion);
  virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
  virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);
  virtual void OnOutput(LPCSTR szText);

  klStackWalkerInternal *m_sw;
  HANDLE m_hProcess;
  DWORD m_dwProcessId;
  BOOL m_modulesLoaded;
  LPSTR m_szSymPath;

  int m_options;

  static BOOL __stdcall myReadProcMem(HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);
 
  friend klStackWalkerInternal;
};

class klStackWalker : public klStackWalkBase
{
public:
  klStackWalker() : klStackWalkBase() 
  {
  }

  klStackWalker(DWORD dwProcessId, HANDLE hProcess) : klStackWalkBase(dwProcessId, hProcess) {}

  virtual void OnOutput(LPCSTR szText) 
  { 
	  klStackWalkBase::OnOutput(szText); 
	  msg+=std::string(szText);
	  msg+="\r\n";

  }
  std::string msg;

};

std::string klStackWalkFn(void);

#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
// The following is not a "good" implementation, 
// because the callstack is only valid in the "__except" block...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    EXCEPTION_POINTERS *pExp = NULL; \
    __try { \
      throw 0; \
    } __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
    if (pExp != NULL) \
      memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT)); \
      c.ContextFlags = contextFlags; \
  } while(0);
#else
// The following should be enough for walking the callstack...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    __asm    call x \
    __asm x: pop eax \
    __asm    mov c.Eip, eax \
    __asm    mov c.Ebp, ebp \
    __asm    mov c.Esp, esp \
  } while(0);
#endif

#else

// The following is defined for x86 (XP and higher), x64 and IA64:
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    RtlCaptureContext(&c); \
} while(0);
#endif


/*
With SEH, you can ensure that resources such as memory blocks and files are correctly if execution unexpectedly terminates. 
You can also handle specific problems—for example, insufficient memory—by using concise structured code that does not rely on 
goto statements or elaborate testing of return codes.
The try-except and try-finally statements referred to in this article are Microsoft extensions to the C language. 
They support SEH by enabling applications to gain control of a program after events that would otherwise terminate execution. 
Although SEH works with C++ source files, it's not specifically designed for C++. If you use SEH in a C++ program that you 
compile by using the /EH option—together with certain modifiers—destructors for local objects are called but other execution 
behavior might not be what you expect. (For an illustration, see the example later in this article.) In most cases, instead of 
SEH we recommend that you use ISO-standard C++ exception handling, which Visual C++ also supports. By using C++ exception handling, 
you can ensure that your code is more portable, and you can handle exceptions of any type.
If you have C modules that use SEH, you can mix them with C++ modules that use C++ exception handling. For information, see Exception Handling Differences.
There are two SEH mechanisms:
Exception handlers, which can respond to or dismiss the exception.
Termination handlers, which are called when an exception causes termination in a block of code.
try-finally-statement:
__try  compound-statement
__finally  compound-statement
The try-finally statement is a Microsoft extension to the C and C++ languages that enables target applications to guarantee execution of cleanup code when 
execution of a block of code is interrupted. Cleanup consists of such tasks as deallocating memory, closing files, and releasing file handles. 
The try-finally statement is especially useful for routines that have several places where a check is made for an error that could cause
premature return from the routine.
For related information and a code sample, see try-except Statement. For more information on structured exception handling in general,
see Structured Exception Handling. For more information on handling exceptions in managed applications, see Exception Handling under /clr.
Note Note
Structured exception handling works with Win32 for both C and C++ source files. However, it is not specifically designed for C++. 
You can ensure that your code is more portable by using C++ exception handling. Also, C++ exception handling is more flexible, 
in that it can handle exceptions of any type. For C++ programs, it is recommended that you use the C++ exception-handling mechanism
(try, catch, and throw statements).
*/


struct klSEHException 
{
	klSEHException(const EXCEPTION_RECORD & record) : record(record) {}
	EXCEPTION_RECORD record;
};

#endif __kl_SEH__