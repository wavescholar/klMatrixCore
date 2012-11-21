#include "kl_memory.h"

#include <fstream>

//class klHeapMgr theKLOperatorNewHeap;
//class klHeapMgr aPrivateHeap;

int klVMemBlock::_systemPageSize=0;
int klAWEMemBlock::_systemPageSize=0;
ULONG_PTR klAWEMemBlock::NumberOfPagesInitial=0; // initial number of pages requested
ULONG_PTR* klAWEMemBlock::aPFNs=0;               // page info; holds opaque data
PVOID klAWEMemBlock::lpMemReserved=0;            // AWE window
PVOID klAWEMemBlock::lpNextFreeAddress=0;            



#include "winbase.h"
HANDLE klLowMemoryResourceNotificationObject;
void klCreateLowMemoryResourceNotification(void)
{
	klLowMemoryResourceNotificationObject=CreateMemoryResourceNotification( LowMemoryResourceNotification );
}

bool klQueryLowMemoryResourceNotification(int* answer)
{
	BOOL success=QueryMemoryResourceNotification(klLowMemoryResourceNotificationObject,answer);
	return success;
}

