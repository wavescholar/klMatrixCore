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
#include "kl_ML_helper_fns.h"
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
	for ( i=0;i<1024*1024;i++)
	{
		//Suspends the current thread until the specified condition is met. Execution resumes when one of the following occurs:
		//An I/O completion callback function is called 
		//An asynchronous procedure call (APC) is queued to the thread. 
		//The minimum time-out interval elapses 
		//DWORD WINAPI SleepEx(
		//  __in          DWORD dwMilliseconds,
		//  __in          BOOL bAlertable
		//);
		//SleepEx(100,false);

		cout<<thisThread<<" :  "<< i<<"  "<<i%1024<<endl;
		if(i%1024)
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
	for ( i=0;i<1024*1024;i++)
	{
		SleepEx(100,false);

		if(i%1024)
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
	cout<<"Runing workflow unit 2"<<" TID= "<<thisThread<<endl;
	
	//klMatlabEngineThreadMap klmtm;
	char* evalString=new char[256];
	char* arg=new char[256];
	char* ofilename=new char[256];
	char* texFileName=new char[512];
	sprintf(texFileName,"klPCA.tex");
	ofstream _tex(texFileName);
	//Do a pca on 3d rv
	klMatrix<double> covarianceMatrix(3,3);

	covarianceMatrix[0][0]=1; covarianceMatrix[0][1]=.2;covarianceMatrix[0][2]=.1;
	covarianceMatrix[1][0]=.2; covarianceMatrix[1][1]=2;covarianceMatrix[1][2]=.1;
	covarianceMatrix[2][0]=.1; covarianceMatrix[2][1]=.1;covarianceMatrix[2][2]=3;
	klVector<double> meanVector(3);
	meanVector[0]=0;meanVector[1]=0;meanVector[2]=0;

	klNormalMultiVariate<double> X(meanVector, covarianceMatrix);

	unsigned int numTestPoints=1024;

	klSamplePopulation<double> sample(numTestPoints,3);
	unsigned int j;
	for(j=0;j<numTestPoints;j++)
	{
		klVector<double> rv=X();
		sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
	}
	sprintf(ofilename,"//testPoints.eps");

	klPrincipalComponents<double> pca=sample;
	unsigned int numComponents=2;
	klMatrix<double> V=pca(numComponents);
	_tex<<"The "<<numComponents<<" eigenvectors:"<<endl;
	_tex<<V<<endl;
	klVector<complex<double> > e=covarianceMatrix.eigenvalues();
	_tex<<"All of the eigenvalues of the covariance matrix:"<<endl;
	_tex<<e<<endl;
	klMatrix<double> dist=sample.calcDistributionTests();
	lpvBuffer = TlsGetValue(gdwTlsIndex);
	LocalFree((HLOCAL) lpvBuffer);



}

void kl_SMP_SM_MT_WorkFlow(void)
{ 

	//Run multiple of the same work elelement
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
	unsigned int numFeatures=8;

	klMatrix<double> Sigma=SampleGOE(numFeatures);
	klMatrix<double> SigmaW=SampleWishart(numFeatures);
	unsigned int i;
	unsigned int j;
	klVector<double> meanVector(numFeatures);
	meanVector=0;
	unsigned int sampleSize=32;
	klNormalMultiVariate<double> T(meanVector,SigmaW );
	klSamplePopulation<double> TS(sampleSize,numFeatures);
	for(j=0;j<sampleSize;j++)
	{
		klVector<double> tv=T();
		TS.setRow(j,tv);
	}
	klMatrix<double> SampleCovariance = TS.covarianceMatrix();
	klMatrix<double> gram_sdot(numFeatures,numFeatures);
	gram_sdot.makeNanFriendly();//Make gram matrix
	double* MV=TS.getMeanVector();
	klVector<double> MVW(MV,numFeatures,false);

	for(i=0;i<sampleSize;i++)
	{
		klVector<double> CenteredDataPoint= TS[i]-MVW;
		double ddot=(CenteredDataPoint).dotBLAS(CenteredDataPoint);

	}
	{
		klMatrix<double> diff = SampleCovariance - gram_sdot;cout<<diff;
	}




}

