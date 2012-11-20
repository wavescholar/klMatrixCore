#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#include "tchar.h"
# pragma comment(lib, "wbemuuid.lib")
int klWMI(const char* filename)
{
	CoInitialize(NULL);
	if (CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0) != S_OK)
		return 0;

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
		_bstr_t bstrPath = &v; //Just to convert BSTR to ANSI
		char* strPath=(char*)bstrPath;
		if (SUCCEEDED(hRes))
		{printf(strPath);printf("    "); wprintf(items[ij]);printf("   \n");}
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
}
