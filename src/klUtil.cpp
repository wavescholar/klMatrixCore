 /*******************************
 * WSCMP [2003] - [2014] WSCMP  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/

#include "windows.h"
#include "kl_util.h"

#ifdef _M_IX86
//////////////////////////Fn's to detect VM environment
bool klIsInsideVMWare()
{
	bool rc = true;

	__try
	{
		__asm
		{
			push   edx
				push   ecx
				push   ebx

				mov    eax, 'VMXh'
				mov    ebx, 0 // any value but not the MAGIC VALUE
				mov    ecx, 10 // get VMWare version
				mov    edx, 'VX' // port number

				in     eax, dx // read port
				// on return EAX returns the VERSION
				cmp    ebx, 'VMXh' // is it a reply from VMWare?
				setz   [rc] // set return value

			pop    ebx
				pop    ecx
				pop    edx
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}

	return rc;
}
// IsInsideVPC's exception filter
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
	PCONTEXT ctx = ep->ContextRecord;

	ctx->Ebx = -1; // Not running VPC
	ctx->Eip += 4; // skip past the "call VPC" opcodes
	return EXCEPTION_CONTINUE_EXECUTION;
	// we can safely resume execution since we skipped faulty instruction
}

// High level language friendly version of IsInsideVPC()
bool klIsInsideVPC()
{
	bool rc = false;

	__try
	{
		_asm push ebx
			_asm mov  ebx, 0 // It will stay ZERO if VPC is running
			_asm mov  eax, 1 // VPC function number

			// call VPC 
			_asm __emit 0Fh
			_asm __emit 3Fh
			_asm __emit 07h
			_asm __emit 0Bh

			_asm test ebx, ebx
			_asm setz [rc]
		_asm pop ebx
	}
	// The except block shouldn't get triggered if VPC is running!!
	__except(IsInsideVPC_exceptionFilter(GetExceptionInformation()))
	{
	}

	return rc;
}
#endif

unsigned int klFlotingPointHelper::status_x86()
{
	return _status87();
}
_CRT_MANAGED_FP_DEPRECATE _CRTIMP void __cdecl _statusfp2(_Out_opt_ unsigned int *_X86_status, _Out_opt_ unsigned int *_SSE2_status);
unsigned int klFlotingPointHelper::status_SSE()
{   
	//#define _SW_INEXACT     0x00000001              /* inexact (precision) */
	//#define _SW_UNDERFLOW   0x00000002              /* underflow */
	//#define _SW_OVERFLOW    0x00000004              /* overflow */
	//#define _SW_ZERODIVIDE  0x00000008              /* zero divide */
	//#define _SW_INVALID     0x00000010              /* invalid */
	//#define _SW_DENORMAL    0x00080000              /* denormal status bit */
	//#define _EM_AMBIGUIOUS  0x80000000				/* for backwards compatibility old spelling */
	//#define _EM_AMBIGUOUS   0x80000000
	//#define _MCW_EM         0x0008001f              /* interrupt Exception Masks */
	//#define _EM_INEXACT     0x00000001              /*   inexact (precision) */
	//#define _EM_UNDERFLOW   0x00000002              /*   underflow */
	//#define _EM_OVERFLOW    0x00000004              /*   overflow */
	//#define _EM_ZERODIVIDE  0x00000008              /*   zero divide */
	//#define _EM_INVALID     0x00000010              /*   invalid */
	//#define _EM_DENORMAL    0x00080000              /* denormal exception mask (_control87 only) */
	//#define _MCW_RC         0x00000300              /* Rounding Control */
	//#define _RC_NEAR        0x00000000              /*   near */
	//#define _RC_DOWN        0x00000100              /*   down */
	//#define _RC_UP          0x00000200              /*   up */
	//#define _RC_CHOP        0x00000300              /*   chop */
	//#define _MCW_PC         0x00030000              /* Precision Control */
	//#define _PC_64          0x00000000              /*    64 bits */
	//#define _PC_53          0x00010000              /*    53 bits */
	//#define _PC_24          0x00020000              /*    24 bits */
	//#define _MCW_IC         0x00040000              /* Infinity Control */
	//#define _IC_AFFINE      0x00040000              /*   affine */
	//#define _MCW_DN         0x03000000              /* Denormal Control */
	//#define _DN_SAVE        0x00000000              /*   save denormal results and operands */
	//#define _DN_FLUSH       0x01000000              /*   flush denormal results and operands to zero */
	//#define _DN_FLUSH_OPERANDS_SAVE_RESULTS 0x02000000  /*   flush operands to zero and save results */
	//#define _DN_SAVE_OPERANDS_FLUSH_RESULTS 0x03000000  /*   save operands and flush results to zero */
	unsigned int  floatingPointStatus87=0;
	unsigned int  _SSEStatus=0;
	//	_statusfp2(&floatingPointStatus87,&_SSEStatus);  //BBC REVISIT - I lost this in the x64 port - revisit should be there but MSFT dodes not have 64 C standard library.
	floatingPointStatus87 =_statusfp();
	return _SSEStatus; 
}
int* klFlotingPointHelper::fpError()
{
	//#define _SW_UNEMULATED          0x0040  /* unemulated instruction */
	//#define _SW_SQRTNEG             0x0080  /* square root of a neg number */
	//#define _SW_STACKOVERFLOW       0x0200  /* FP stack overflow */
	//#define _SW_STACKUNDERFLOW      0x0400  /* FP stack underflow */
	//#define _FPE_INVALID            0x81
	//#define _FPE_DENORMAL           0x82
	//#define _FPE_ZERODIVIDE         0x83
	//#define _FPE_OVERFLOW           0x84
	//#define _FPE_UNDERFLOW          0x85
	//#define _FPE_INEXACT            0x86
	//#define _FPE_UNEMULATED         0x87
	//#define _FPE_SQRTNEG            0x88
	//#define _FPE_STACKOVERFLOW      0x8a
	//#define _FPE_STACKUNDERFLOW     0x8b
	//#define _FPE_EXPLICITGEN =       0x8c    /* raise( SIGFPE ); */
	return __fpecode();

}

unsigned int klFlotingPointHelper::Clear87( )
{
	return _clear87();
}
unsigned int klFlotingPointHelper::Clear (void ){
	return _clearfp();
}



