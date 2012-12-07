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

typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff [30];
}ASTAT, * PASTAT;

ASTAT Adapter;

void GetMaAddresscFromAdapter (void)
{
    NCB ncb;
    UCHAR uRetCode;
    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = 0;

    uRetCode = Netbios( &ncb );
    printf( "The NCBRESET return code is: 0x%x \n", uRetCode );

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = 0;
	
    memcpy( &ncb.ncb_callname, "*               ", 16 );
    ncb.ncb_buffer = (UCHAR*) &Adapter;
    ncb.ncb_length = sizeof(Adapter);

    uRetCode = Netbios( &ncb );
    printf( "The NCBASTAT return code is: 0x%x \n", uRetCode );
    if ( uRetCode == 0 )
    {
        printf( "The Ethernet Number is: %02x%02x%02x%02x%02x%02x\n",
                Adapter.adapt.adapter_address[0],
                Adapter.adapt.adapter_address[1],
                Adapter.adapt.adapter_address[2],
                Adapter.adapt.adapter_address[3],
                Adapter.adapt.adapter_address[4],
                Adapter.adapt.adapter_address[5] );
    }
}

//
//char* GenerateUIDFromMAC (void)
//{
//	NCB Ncb;
//	UCHAR uRetCode;
//	char NetName[50];
//	LANA_ENUM   lenum;
//	int      i;
//
//	char* UID = new char[256];
//
//	memset( &Ncb, 0, sizeof(Ncb) );
//	Ncb.ncb_command = NCBENUM;
//	Ncb.ncb_buffer = (UCHAR *)&lenum;
//	Ncb.ncb_length = sizeof(lenum);
//	uRetCode = Netbios( &Ncb );
//	printf( "The NCBENUM return code is: 0x%x \n", uRetCode );
//
//	for(i=0; i < lenum.length ;i++)
//	{
//		memset( &Ncb, 0, sizeof(Ncb) );
//		Ncb.ncb_command = NCBRESET;
//		Ncb.ncb_lana_num = lenum.lana[i];
//
//		uRetCode = Netbios( &Ncb );
//		printf( "The NCBRESET on LANA %d return code is: 0x%x \n",
//			lenum.lana[i], uRetCode );
//
//		memset( &Ncb, 0, sizeof (Ncb) );
//		Ncb.ncb_command = NCBASTAT;
//		Ncb.ncb_lana_num = lenum.lana[i];
//
//		Ncb.ncb_length = sizeof(Adapter);
//
//		uRetCode = Netbios( &Ncb );
//		printf( "The NCBASTAT on LANA %d return code is: 0x%x \n",
//			lenum.lana[i], uRetCode );
//		if ( uRetCode == 0 )
//		{
//			printf( "The Ethernet Number on LANA %d is: %02x%02x%02x%02x%02x%02x\n",
//				lenum.lana[i],
//				Adapter.adapt.adapter_address[0],
//				Adapter.adapt.adapter_address[1],
//				Adapter.adapt.adapter_address[2],
//				Adapter.adapt.adapter_address[3],
//				Adapter.adapt.adapter_address[4],
//				Adapter.adapt.adapter_address[5] );
//
//			sprintf(UID, " %02x%02x%02x%02x%02x%02x\n",      
//				Adapter.adapt.adapter_address[0],
//				Adapter.adapt.adapter_address[1],
//				Adapter.adapt.adapter_address[2],
//				Adapter.adapt.adapter_address[3],
//				Adapter.adapt.adapter_address[4],
//				Adapter.adapt.adapter_address[5] );
//		}
//	}
//	return UID;
//
//}
//







#endif __kl_HARDWARE__