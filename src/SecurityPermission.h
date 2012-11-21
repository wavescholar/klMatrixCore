// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SECURITYDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SECURITYDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SECURITYDLL_EXPORTS
#define SECURITYDLL_API __declspec(dllexport)
#else
#define SECURITYDLL_API __declspec(dllimport)
#endif

// This class is exported from the securityDLL.dll
class SECURITYDLL_API CsecurityDLL {
public:
	CsecurityDLL(void);
	// TODO: add your methods here.
};

extern SECURITYDLL_API int nsecurityDLL;

SECURITYDLL_API int fnsecurityDLL(void);
