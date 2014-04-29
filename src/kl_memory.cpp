 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
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

