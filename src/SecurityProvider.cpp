#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_VISTA 
#endif

#include <certenroll.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <atlbase.h>

///////////////////////////enumerate crypto providers///////////////////
HRESULT enumProviders(void);
int enumerateProviders()
{
	HRESULT hr = S_OK;

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if(FAILED(hr)) return hr;

	hr = enumProviders();

	CoUninitialize();
	return hr;
}

HRESULT enumProviders(void)
{
	printf("Enumerating CryptoAPI and CNG Providers Installed");

	CComPtr<ICspInformations>     pCSPs;   // Provider collection
	CComPtr<ICspInformation>      pCSP;    // Provider instgance
	HRESULT           hr          = S_OK;  // Return value
	long              lCount      = 0;     // Count of providers
	CComBSTR          bstrName;            // Provider name
	VARIANT_BOOL      bLegacy;             // CryptoAPI or CNG

	hr = CoCreateInstance( __uuidof(CCspInformations), NULL, CLSCTX_INPROC_SERVER, __uuidof(ICspInformations), (void **) &pCSPs);
	if(FAILED(hr)) return hr;

	hr = pCSPs->AddAvailableCsps();
	if(FAILED(hr)) return hr;

	hr = pCSPs->get_Count(&lCount);
	if(FAILED(hr)) return hr;

	for (long i=0; i<lCount; i++)
	{
		hr = pCSPs->get_ItemByIndex(i, &pCSP);
		if(FAILED(hr)) return hr;

		hr = pCSP->get_Name(&bstrName);
		if(FAILED(hr)) return hr;

		hr = pCSP->get_LegacyCsp(&bLegacy);
		if(FAILED(hr)) return hr;

		if(VARIANT_TRUE == bLegacy)
			wprintf_s(L"%2d. Legacy: ", i);
		else
			wprintf_s(L"%2d. CNG: ", i);

		wprintf_s(L"%s\n", static_cast<wchar_t*>(bstrName.m_str));
		System::Console::WriteLine(gcnew System::String (bstrName.m_str ) );
		pCSP=NULL;

	}


	return hr;

}


//////////////////////////enumerate Certificates in store.
#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <cryptuiapi.h>
#include <tchar.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
void HandleCertEnumError(char *s);
void enumerateCertificatesInStore(const wchar_t* pszStoreNameIn)
{
	HCERTSTORE       hCertStore;        
	PCCERT_CONTEXT   pCertContext=NULL;      
	wchar_t pszNameString[256];
	wchar_t pszStoreName[256];
	void*            pvData;
	DWORD            cbData;
	DWORD            dwPropId = 0; 
	//These are the stores 
	//CERT_SYSTEM_STORE_CURRENT_USER
	//CERT_SYSTEM_STORE_CURRENT_SERVICE
	//CERT_SYSTEM_STORE_LOCAL_MACHINE
	//CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY
	//CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY
	//CERT_SYSTEM_STORE_SERVICES
	//CERT_SYSTEM_STORE_USERS
	//CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE

	wsprintf(pszStoreName,L"%s",pszStoreNameIn);
	if ( hCertStore = CertOpenSystemStore(  NULL,  pszStoreName))
	{
		System::Console::WriteLine(gcnew System::String(pszStoreName) + " store has been opened. \n");
	}
	else
	{
		HandleCertEnumError("The store was not opened.");
	}

	while(pCertContext= CertEnumCertificatesInStore(
		hCertStore,
		pCertContext))
	{

		if ( CryptUIDlgViewContext(
			CERT_STORE_CERTIFICATE_CONTEXT,
			pCertContext,
			NULL,
			NULL,
			0,
			NULL))
		{
		}
		else
		{
			HandleCertEnumError("UI failed.");
		}

		if(CertGetNameString( pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, pszNameString, 128))
		{
			System::Console::WriteLine(gcnew System::String(pszNameString) + "  Certificate for %s \n");
		}
		else
			fprintf(stderr,"CertGetName failed. \n");

		while(dwPropId = CertEnumCertificateContextProperties(
			pCertContext, // The context whose properties are to be listed.
			dwPropId))    // Number of the last property found.  
			
		{
			//System::Console::WriteLine(gcnew System::String(dwPropId)+ "   Property # %d found->" );
			switch(dwPropId)
			{
			case CERT_FRIENDLY_NAME_PROP_ID:
				{
					System::Console::WriteLine("Display name: ");
					break;
				}
			case CERT_SIGNATURE_HASH_PROP_ID:
				{
					System::Console::WriteLine("Signature hash identifier ");
					break;
				}
			case CERT_KEY_PROV_HANDLE_PROP_ID:
				{
					System::Console::WriteLine("KEY PROVE HANDLE");
					break;
				}
			case CERT_KEY_PROV_INFO_PROP_ID:
				{
					System::Console::WriteLine("KEY PROV INFO PROP ID ");
					break;
				}
			case CERT_SHA1_HASH_PROP_ID:
				{
					System::Console::WriteLine("SHA1 HASH identifier");
					break;
				}
			case CERT_MD5_HASH_PROP_ID:
				{
					System::Console::WriteLine("md5 hash identifier ");
					break;
				}
			case CERT_KEY_CONTEXT_PROP_ID:
				{
					System::Console::WriteLine("KEY CONTEXT PROP identifier");
					break;
				}
			case CERT_KEY_SPEC_PROP_ID:
				{
					System::Console::WriteLine("KEY SPEC PROP identifier");
					break;
				}
			case CERT_ENHKEY_USAGE_PROP_ID:
				{
					System::Console::WriteLine("ENHKEY USAGE PROP identifier");
					break;
				}
			case CERT_NEXT_UPDATE_LOCATION_PROP_ID:
				{
					System::Console::WriteLine("NEXT UPDATE LOCATION PROP identifier");
					break;
				}
			case CERT_PVK_FILE_PROP_ID:
				{
					System::Console::WriteLine("PVK FILE PROP identifier ");
					break;
				}
			case CERT_DESCRIPTION_PROP_ID:
				{
					System::Console::WriteLine("DESCRIPTION PROP identifier ");
					break;
				}
			case CERT_ACCESS_STATE_PROP_ID:
				{
					System::Console::WriteLine("ACCESS STATE PROP identifier ");
					break;
				}
			case CERT_SMART_CARD_DATA_PROP_ID:
				{
					System::Console::WriteLine("SMART_CARD DATA PROP identifier ");
					break;
				}
			case CERT_EFS_PROP_ID:
				{
					System::Console::WriteLine("EFS PROP identifier ");
					break;
				}
			case CERT_FORTEZZA_DATA_PROP_ID:
				{
					System::Console::WriteLine("FORTEZZA DATA PROP identifier ");
					break;
				}
			case CERT_ARCHIVED_PROP_ID:
				{
					System::Console::WriteLine("ARCHIVED PROP identifier ");
					break;
				}
			case CERT_KEY_IDENTIFIER_PROP_ID:
				{
					System::Console::WriteLine("KEY IDENTIFIER PROP identifier ");
					break;
				}
			case CERT_AUTO_ENROLL_PROP_ID:
				{
					System::Console::WriteLine("AUTO ENROLL identifier.");
					break;
				}
			} 
			if(CertGetCertificateContextProperty(pCertContext,  dwPropId ,  NULL, &cbData))
			{
				
			}
			else
			{  

				HandleCertEnumError("Call #1 to GetCertContextProperty failed.");
			}

			if(pvData = (void*)malloc(cbData))
			{
				// Memory is allocated. Continue.
			}
			else
			{
				// If memory allocation failed, exit to an error routine.
				HandleCertEnumError("Memory allocation failed.");
			}
			//----------------------------------------------------------------
			// Allocation succeeded. Retrieve the property data.

			if(CertGetCertificateContextProperty(
				pCertContext,
				dwPropId,
				pvData, 
				&cbData))
			{
			}
			else
			{
				HandleCertEnumError("Call #2 failed.");
			}
			//   System::Console::WriteLine(gcnew System::String(pvData) +  "  Property Content \n" );
			free(pvData);
		} 
	}
	/*if(!(pCertContext = CryptUIDlgSelectCertificateFromStore(
		hCertStore,
		NULL,
		NULL,
		NULL,
		CRYPTUI_SELECT_LOCATION_COLUMN,
		0,
		NULL)))
	{
		HandleCertEnumError("Select UI failed." );
	}*/
	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hCertStore,0);
} 


void HandleCertEnumError(char* psz)
{
	_ftprintf(stderr, TEXT("An error occurred in the program. \n"));
	_ftprintf(stderr, TEXT("%s\n"), psz);
	_ftprintf(stderr, TEXT("Error number %x.\n"), GetLastError());
	_ftprintf(stderr, TEXT("Program terminating. \n"));

} 

///////////////////////////////////   enumerates_system_certificate_store_locations          /////////////////
#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
void MyHandleError(char *s);
#pragma comment(lib, "crypt32.lib")
typedef struct _ENUM_ARG {
	BOOL        fAll;
	BOOL        fVerbose;
	DWORD       dwFlags;
	const void  *pvStoreLocationPara;
	HKEY        hKeyBase;
} ENUM_ARG, *PENUM_ARG;
static BOOL WINAPI EnumPhyCallback(const void *pvSystemStore,DWORD dwFlags, LPCWSTR pwszStoreName, PCERT_PHYSICAL_STORE_INFO pStoreInfo, void *pvReserved, void *pvArg);
static BOOL WINAPI EnumSysCallback(const void *pvSystemStore, DWORD dwFlags, PCERT_SYSTEM_STORE_INFO pStoreInfo, void *pvReserved, void *pvArg);
static BOOL WINAPI EnumLocCallback(LPCWSTR pwszStoreLocation, DWORD dwFlags, void *pvReserved,  void *pvArg);



void enumerates_system_certificate_store_locations(void) 
{
	DWORD dwExpectedError = 0;
	DWORD dwLocationID = CERT_SYSTEM_STORE_CURRENT_USER_ID;
	DWORD dwFlags = 0;
	CERT_PHYSICAL_STORE_INFO PhyStoreInfo;
	ENUM_ARG EnumArg;
	LPSTR pszStoreParameters = NULL;          
	LPWSTR pwszStoreParameters = NULL;
	LPWSTR pwszSystemName = NULL;
	LPWSTR pwszPhysicalName = NULL;
	LPWSTR pwszStoreLocationPara = NULL;
	void *pvSystemName;                   
	void *pvStoreLocationPara;              
	DWORD dwNameCnt = 0;
	LPCSTR pszTestName;
	HKEY hKeyRelocate = HKEY_CURRENT_USER;
	LPSTR pszRelocate = NULL;               
	HKEY hKeyBase = NULL;

	memset(&PhyStoreInfo, 0, sizeof(PhyStoreInfo));
	PhyStoreInfo.cbSize = sizeof(PhyStoreInfo);
	PhyStoreInfo.pszOpenStoreProvider = sz_CERT_STORE_PROV_SYSTEM_W;
	pszTestName = "Enum";  
	pvSystemName = pwszSystemName;
	pvStoreLocationPara = pwszStoreLocationPara;

	memset(&EnumArg, 0, sizeof(EnumArg));
	EnumArg.dwFlags = dwFlags;
	EnumArg.hKeyBase = hKeyBase;

	EnumArg.pvStoreLocationPara = pvStoreLocationPara;
	EnumArg.fAll = TRUE;
	dwFlags &= ~CERT_SYSTEM_STORE_LOCATION_MASK;
	dwFlags |= (dwLocationID << CERT_SYSTEM_STORE_LOCATION_SHIFT) &
		CERT_SYSTEM_STORE_LOCATION_MASK;

	System::Console::WriteLine("Begin enumeration of store locations. \n");
	if(CertEnumSystemStoreLocation( dwFlags,&EnumArg,EnumLocCallback ))
	{
		System::Console::WriteLine("\nFinished enumerating locations. \n");
	}
	else
	{
		MyHandleError("Enumeration of locations failed.");
	}
	System::Console::WriteLine("\nBegin enumeration of system stores. \n");

	if(CertEnumSystemStore(dwFlags, pvStoreLocationPara, &EnumArg,  EnumSysCallback    ))
	{
		System::Console::WriteLine("\nFinished enumerating system stores. \n");
	}
	else
	{
		MyHandleError("Enumeration of system stores failed.");
	}

	System::Console::WriteLine("\n\nEnumerate the physical stores for the MY system store. \n");
	if(CertEnumPhysicalStore(L"MY",dwFlags,&EnumArg, EnumPhyCallback ))
	{
		System::Console::WriteLine("Finished enumeration of the physical stores. \n");
	}
	else
	{
		MyHandleError("Enumeration of physical stores failed.");
	}
}    


static BOOL GetSystemName( const void *pvSystemStore,DWORD dwFlags, PENUM_ARG pEnumArg, LPCWSTR *ppwszSystemName )
{
	*ppwszSystemName = NULL;
	if (pEnumArg->hKeyBase && 0 == (dwFlags &  CERT_SYSTEM_STORE_RELOCATE_FLAG)) 
	{
		return FALSE;
	} 
	else 
	{
		if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG) 
		{
			PCERT_SYSTEM_STORE_RELOCATE_PARA pRelocatePara;
			if (!pEnumArg->hKeyBase) 
			{
				MyHandleError("Failed => RELOCATE_FLAG is set in callback");
			}
			pRelocatePara = (PCERT_SYSTEM_STORE_RELOCATE_PARA) 
				pvSystemStore;
			if (pRelocatePara->hKeyBase != pEnumArg->hKeyBase) 
			{
				MyHandleError("Wrong hKeyBase passed to callback");
			}
			*ppwszSystemName = pRelocatePara->pwszSystemStore;
		} 
		else
		{
			*ppwszSystemName = (LPCWSTR) pvSystemStore;
		}
	}
	return TRUE;
}
static BOOL WINAPI EnumPhyCallback(  const void *pvSystemStore, DWORD dwFlags,   LPCWSTR pwszStoreName,    PCERT_PHYSICAL_STORE_INFO pStoreInfo,      void *pvReserved,  void *pvArg )
{
	PENUM_ARG pEnumArg = (PENUM_ARG) pvArg;
	LPCWSTR pwszSystemStore;
	if (GetSystemName(pvSystemStore, dwFlags, pEnumArg,  &pwszSystemStore))
	{
		System::Console::WriteLine(gcnew System::String(pwszStoreName));
	}
	else
	{
		MyHandleError("GetSystemName failed.");
	}
	if (pEnumArg->fVerbose &&   (dwFlags & CERT_PHYSICAL_STORE_PREDEFINED_ENUM_FLAG))
		return TRUE;
}

static BOOL WINAPI EnumSysCallback(  const void *pvSystemStore,  DWORD dwFlags,  PCERT_SYSTEM_STORE_INFO pStoreInfo, void *pvReserved, void *pvArg)
{
	PENUM_ARG pEnumArg = (PENUM_ARG) pvArg;
	LPCWSTR pwszSystemStore;
	static int line_counter=0;
	char x;
	
	if (GetSystemName(pvSystemStore, dwFlags, pEnumArg, &pwszSystemStore))
	{
		System::Console::WriteLine(pwszSystemStore);
	}
	else
	{
		MyHandleError("GetSystemName failed.");
	}
	if (pEnumArg->fAll || pEnumArg->fVerbose) 
	{
		dwFlags &= CERT_SYSTEM_STORE_MASK;
		dwFlags |= pEnumArg->dwFlags & ~CERT_SYSTEM_STORE_MASK;
		if (!CertEnumPhysicalStore( pvSystemStore,dwFlags, pEnumArg, EnumPhyCallback  )) 
		{
			DWORD dwErr = GetLastError();
			if (!(ERROR_FILE_NOT_FOUND == dwErr ||  ERROR_NOT_SUPPORTED == dwErr))
			{
				System::Console::WriteLine("    CertEnumPhysicalStore");
			}
		}
	}
	return TRUE;
}


static BOOL WINAPI EnumLocCallback(LPCWSTR pwszStoreLocation, DWORD dwFlags, void *pvReserved,void *pvArg)
{
	PENUM_ARG pEnumArg = (PENUM_ARG) pvArg;
	DWORD dwLocationID = (dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK) >>
		CERT_SYSTEM_STORE_LOCATION_SHIFT;
	static int linecount=0;
	char x;
	
	System::Console::WriteLine(gcnew System::String(pwszStoreLocation));
	if (pEnumArg->fAll) 
	{
		dwFlags &= CERT_SYSTEM_STORE_MASK;
		dwFlags |= pEnumArg->dwFlags & ~CERT_SYSTEM_STORE_LOCATION_MASK;
		CertEnumSystemStore(dwFlags, (void *) pEnumArg->pvStoreLocationPara, pEnumArg, EnumSysCallback ); 
	}
	return TRUE;
}


void MyHandleError(char *s)
{
	fprintf(stderr,"An error occurred in running the program. \n");
	fprintf(stderr,"%s\n",s);
	fprintf(stderr, "Error number %x.\n", GetLastError());
} 
