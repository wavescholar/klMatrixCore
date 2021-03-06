 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#include "windows.h"
#define KL_OPERATOR_NEW

#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_util.h"
#include "kl_thread_workflow.h"
#include "kl_matrix_helper_fns.h"
extern DWORD gdwTlsIndex;
//Globals for thread functionality
klMutex klThreadMap::lock;
map<klThreadId, unsigned long> klThreadMap::threadMap;

void work0(void)
{
	//Thread Local Storage
	LPVOID lpvBuffer;
	lpvBuffer = (LPVOID) LocalAlloc(LPTR, 256);
	TlsSetValue(gdwTlsIndex, lpvBuffer);

	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();

	unsigned int i;
	for ( i=0;i<512;i++)
	{
		//Suspends the current thread until the specified condition is met. Execution resumes when one of the following occurs:
		//An I/O completion callback function is called. An asynchronous procedure call (APC) is queued to the thread. 
		//The minimum time-out interval elapses 
		//SleepEx(100,false);
		cout<<"Runing workflow unit 0"<<" TID= "<<thisThread<<"i = "<<i<<endl;
	}	
	

	lpvBuffer = TlsGetValue(gdwTlsIndex);
	LocalFree((HLOCAL) lpvBuffer);
}
void work1(void)
{
	//Thread Local Storage
	LPVOID lpvBuffer;
	lpvBuffer = (LPVOID) LocalAlloc(LPTR, 256);
	TlsSetValue(gdwTlsIndex, lpvBuffer);

	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();

	unsigned int i;
	for ( i=0;i<512;i++)
	{
		//SleepEx(100,false);
		cout<<"Runing workflow unit 1"<<" TID= "<<thisThread<<"i = "<<i<<endl;
	}
	

	lpvBuffer = TlsGetValue(gdwTlsIndex);
	LocalFree((HLOCAL) lpvBuffer);


}
void work2(void)
{
	//Thread Local Storage
	LPVOID lpvBuffer;
	lpvBuffer = (LPVOID) LocalAlloc(LPTR, 256);
	TlsSetValue(gdwTlsIndex, lpvBuffer);
	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	
	unsigned int i;
	for ( i=0;i<512;i++)
	{
		//SleepEx(100,false);
		cout<<"Runing workflow unit 2"<<" TID= "<<thisThread<<"i = "<<i<<endl;
	}
	////klMatlabEngineThreadMap klmtm;
	//char* evalString=new char[256];
	//char* arg=new char[256];
	//char* ofilename=new char[256];
	//char* texFileName=new char[512];
	//sprintf(texFileName,"klPCA.tex");
	//ofstream _tex(texFileName);
	////Do a pca on 3d rv
	//klMatrix<double> covarianceMatrix(3,3);

	//covarianceMatrix[0][0]=1; covarianceMatrix[0][1]=.2;covarianceMatrix[0][2]=.1;
	//covarianceMatrix[1][0]=.2; covarianceMatrix[1][1]=2;covarianceMatrix[1][2]=.1;
	//covarianceMatrix[2][0]=.1; covarianceMatrix[2][1]=.1;covarianceMatrix[2][2]=3;
	//klVector<double> meanVector(3);
	//meanVector[0]=0;meanVector[1]=0;meanVector[2]=0;

	//klNormalMultiVariate<double> X(meanVector, covarianceMatrix);

	//unsigned int numTestPoints=1024;

	//klSamplePopulation<double> sample(numTestPoints,3);
	//unsigned int j;
	//for(j=0;j<numTestPoints;j++)
	//{
	//	klVector<double> rv=X();
	//	sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
	//}
	//sprintf(ofilename,"//testPoints.eps");

	//klPrincipalComponents<double> pca=sample;
	//unsigned int numComponents=2;
	//klMatrix<double> V=pca(numComponents);
	//_tex<<"The "<<numComponents<<" eigenvectors:"<<endl;
	//_tex<<V<<endl;
	//klVector<complex<double> > e=covarianceMatrix.eigenvalues();
	//_tex<<"All of the eigenvalues of the covariance matrix:"<<endl;
	//_tex<<e<<endl;
	//klMatrix<double> dist=sample.calcDistributionTests();
	//lpvBuffer = TlsGetValue(gdwTlsIndex);
	//LocalFree((HLOCAL) lpvBuffer);
}

void MutithreadedWorkflow(void)
{ 
	//Run multiple of the same work element
	{
		klThreadWorkflow<int,klUnitTestThreadPtr> workflow;
		void (*pf1)() =work0;
		void (*pf2)() =work1;
		unsigned int noThreads=2;
		unsigned int i;

		klSmartPtr<klUnitTestThread> element1=new klUnitTestThread(pf1);
		workflow.insertWorkElement(1, element1);
		klSmartPtr<klUnitTestThread> element2=new klUnitTestThread(pf2);
		workflow.insertWorkElement(2, element2);
		workflow.doWork();
	}
}

