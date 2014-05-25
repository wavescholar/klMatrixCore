 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *    *  
 * Bruce B Campbell 05 27 2013  *
 ********************************/
#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_time_series.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_ML_helper_fns.h"
#include "kl_divergence_functions.h"
#include "kl_util.h"
#include "kl_unit_tests.h"
#include "kl_matrix_facorizations.h"
#include "kl_unit_test_wrapper.h"
#include "kl_matlab_dependent_unit_tests.h"
#include "kl_matlab_iface.h"
#include "kl_arpack.h"
//#include "kl_fast_gauss_transform.h"
#include "kl_latex_helper_fns.h"

const char* basefilename="D:\\klMatrixCore\\output\\"; 

static klTestType klTestSize= klTestType::SMALL;

klMutex klMatlabEngineThreadMap::lock;
map<klThreadId, Engine*> klMatlabEngineThreadMap::engineMap;
DWORD gdwTlsIndex;
void __cdecl klNewHandler( )
{
	char* arg=new char[256];
	sprintf(arg,"klNewHandlerCalled.txt");
	ios_base::openmode wMode = ios_base::app;
	ofstream _tex(arg,wMode);

	klPrintModules(_tex);
	klWinMemoryInfo(_tex);
	klWinSystemInfo(_tex);

	_tex.close();
	throw bad_alloc( );
	return;
}
void MatrixOpsQuickCheck(ofstream &_tex,__int64  &n);
void MatrixNorm(ofstream &_tex,__int64  &n);
void MemoryManagement(ofstream &_tex,__int64  &n);
void SemidefiniteProgrammingCheck(ofstream &_tex,__int64  &n);
void PrincipalComponentsDemo(ofstream &_tex,__int64  &n);
void IterativeKrylovCheck(ofstream &_tex,__int64  &n,const char* fileName=NULL);
void GenerateTraceyWidomSample(ofstream &_tex,__int64  &n);
void Utility(ofstream &_tex,__int64  &n);
void MatrixMultiplicationCheck(ofstream &_tex,__int64  &n );
void LinearRegression(ofstream &_tex,__int64  &n);
void MatrixExponential(ofstream &_tex,__int64  &n);
void MutithreadedWorkflow(void);
void VerifyWingerLaw(ofstream &_tex, __int64& n);
void GenerativeGramConsistencyCheck(ofstream &_tex,__int64  &n);
void MatrixEigenSolver(ofstream &_tex,__int64  &n);
void Arpack_MKLsyevxSmokeTest(ofstream &_tex,__int64 &n,const char* fileName);
void FEATSEigensolver(ofstream &_tex,__int64 &n,const char* fileName);
void testKlBinaryOI(ofstream &_tex,__int64 &n);
void testPointCloudAndLatexPlots(ofstream &_tex,__int64 &n);
void testMatrixNorms(ofstream &_tex,__int64 &n);
#include "kl_point_cloud_generator.h"

void klFGTTest(ofstream &_tex,const klAlgorithmParameterContainer& klapc );

#include "kl_time_series.h"
#include "kl_random_number_generator.h"
void IteratedExponentialFiltering(ofstream &_tex,__int64 &n);

#include "kl_vsl.h"
void VSLFunctions(ofstream &_tex,__int64 &n);

#include <errno.h> 

void klIntegrationTest(bool useIntelMemMgr)
{
	
	klTimer klt;
	klt.tic();

	char* syscmd = new char[2048];
	sprintf(syscmd,"mkdir %s",basefilename);
	system(syscmd); 
	int heapstatus = _heapchk();
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);
	char* testRunDateTime = new char[1024];
	char* regressionFile = new char[1024];
	char* coutFile = new char[1024];
	char* sysInfoFile = new char[1024];
	heapstatus = _heapchk();

	sprintf(testRunDateTime,"%d_%d_%d_%d_%d",tm_buf->tm_mon+1,tm_buf->tm_mday+1,tm_buf->tm_hour+1,tm_buf->tm_min+1,tm_buf->tm_sec+1);
	sprintf(regressionFile,"%skl_Regression%s.tex",basefilename,testRunDateTime);
	sprintf(coutFile,"%skl_cout%s.txt",basefilename,testRunDateTime);
	sprintf(sysInfoFile,"%skl_cout%s.txt",basefilename,testRunDateTime);

	FILE *stream;

#ifndef _DEBUG
	if((stream = freopen(coutFile, "a", stdout)) == NULL)
		throw "kl: error redirecting std::cout to a file.";
	cout<<"Redirecting std::cout to"<<coutFile<<"file via freopen."<<endl;
#endif

	ofstream _tex(regressionFile);

	ofstream _systemText(sysInfoFile);
		
	startLatexDoc("Regression of KL Software Distribution   ","KL Software Libraries",asctime(tm_buf),_tex, "");

	_tex<<"\\textbf{ KL Library test output.  This LaTex file and the associated diagrams are produced by the KL software libraries.}"<<endl;
	
	klUnitTestWrapper klutw(_tex,_systemText);
	heapstatus = _heapchk();

	__int64 n = 512;
	klutw.setDimension(n);
	
	_tex.flush();
	
	heapstatus = _heapchk();
		
	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=NULL;
	if (!(matlabEngine = engOpen(NULL))) 
		throw "klWorkflow: no matlab engine available";
	
	engSetVisible(matlabEngine,false);
#ifdef _DEBUG
	engSetVisible(matlabEngine,true);
#endif

	klmtm.insert(thisThread, matlabEngine);
		

	if(useIntelMemMgr)
	{
		klMKLMemMgr*  mgr = new klMKLMemMgr();
		klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
	}
	else
	{
		klMemMgr*  mgr = NULL;
		klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
	}

	klutw.runTest(testMatrixNorms);

	klutw.runTest(IteratedExponentialFiltering);

	klutw.runTest( testKlBinaryOI);
	
	makeLatexSection("Fast Gauss Transform",_tex);
	klutw.setDimension(2);
	klutw.runTest(klFGTTest);

	klutw.runTest( testPointCloudAndLatexPlots);

	makeLatexSection("Matrix Quick Check <double>",_tex);
	klutw.runTest(MatrixOpsQuickCheck<double>);

	makeLatexSection("Matrix Quick Check <float>",_tex);
	klutw.runTest(MatrixOpsQuickCheck<float>);

	klTestSize= klTestType::SMALL;
			
	klutw.runTest(VSLFunctions);

	klutw.runTest(GenerativeGramConsistencyCheck);
	
	klutw.runTest(MatrixEigenSolver);
		
	klutw.runTest(GenerateTraceyWidomSample);
	
	klutw.runTest(VerifyWingerLaw);
	
	klmtm.insert(thisThread,matlabEngine);
	matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	
	makeLatexSection("Matrix Exponential ",_tex);
	klutw.runTest(MatrixExponential);
				
	klutw.runTest(testKLRandomNumberGeneratorMatlab<double>);
	
	makeLatexSection("Multiclass Support Vector Machine ",_tex);
	klutw.runTest(klMulticlassSVMHarnessMatlab<double>);

	makeLatexSection("Semidefinite Programming SDPA",_tex);
	klutw.runTest(SemidefiniteProgrammingCheck);

	makeLatexSection("Linear Regression 3x1",_tex);
	klutw.runTest(LinearRegression);

	makeLatexSection("Matrix Norms",_tex);
	klutw.runTest(MatrixNorm);

	makeLatexSection("Principal Components Matlab ",_tex);
	klutw.runTest(testklPrincipalComponentsMatlab<double>);

	makeLatexSection("Multi Variate Random Number Generator ",_tex);
	klutw.runTest(testKLMultiVariateRandomNumberGeneratorMatlab<double>);

	makeLatexSection("Matrix Multiply",_tex);
	klutw.runTest(MatrixMultiplicationCheck);

	makeLatexSection("Descriptive Statistics",_tex);
	klutw.runTest(testKLDescriptiveStatistics<double>);

	makeLatexSection("Time Series ",_tex);
	klutw.runTest(testKLTimeSeries2<double>);

	//makeLatexSection"Test Wavelet <double>",_tex);
	// HEAP[TestDll.exe]: Heap block at 0000000005B0A540 modified at 0000000005B0E584 past requested size of 4034
	//klutw.runTest(testKLWavelet<double>);

	heapstatus = _heapchk();
	
	n=0;
	MemoryManagement(_systemText,n);
	Utility(_systemText,n);
	klutw.HardwareConfiguration(_systemText);

	MutithreadedWorkflow();
		
	#ifdef _M_IX86
	bool isInsideVMWare= klIsInsideVMWare();
	bool isInsideVPC =klIsInsideVPC();
	#endif 

	endLatexDoc(_tex);

	_tex.close();

	delete testRunDateTime;
	delete regressionFile;
	delete coutFile;
	delete sysInfoFile;

	heapstatus = _heapchk();

	delete syscmd;

	double time = klt.toc();
	cout<<"klIntegrationTest run time " <<time<<endl;
}

void VerifyWingerLaw(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Approximate Winger Distribution",_tex);
	
	makeLatexSection("Verfy Winger Law.",_tex);

	_tex<<"Let $M_n = [X_{ij} ]$ a symmetric n x n matrix with Random entries such that $X_{i,j} = X_{j,i}$, \
		  and $X_{i,j}$ are iid $\forall i < j,$ and $Xjj$ are iid $\forall j  :  \; E[X^2_{ij} ] = 1, \& E[X_{ij}] = 0$ \
		  and that all moments exists for each of the entries.  \
		  The eigenvector of this random matrix; $ \lambda_1 \leq ... \leq \lambda_n$ depends continuously on $Mn$."<<endl;
	
	if (klTestSize==klTestType::VERYLARGE)
	{
     n=16384;
	}
	
	if (klTestSize==klTestType::LARGE)
	{
     n=4096;
	}

	if (klTestSize==klTestType::MEDIUM)
	{
     n=1024;
	}

	if (klTestSize==klTestType::SMALL)
	{
     n=512;
	}



	_tex<<"Dimension $n = "<<n<<"$"<<endl<<endl;
	
	klMatrix<double> A = SampleSymmetricStandardNormalRM(n);
	
	klVector< complex<double> >  eigs = A.eigenvalues();
	
	LatexInsertHistogram(RE(eigs),100,_tex, basefilename,"Re_lambda_n","Histogram of Re(\\lambda_i) for $X$");
	LatexInsertHistogram(IM(eigs),100,_tex, basefilename,"Im_lambda_n","Histogram of Im(\\lambda_i) for $X$");

	_tex.flush();

}

//I got a zero finally out of my Merssene Twistor (MT) random variate generator.  
//The range of the one I use is [0, 2^32-1].  It happened after 1,171,079,842 calls.
//I wrote a Normal RV  based on the  MT that uses the approximation to the inverse CDF of the normal distribution by 
//Peter J. Acklam.  See http://home.online.no/~pjacklam/notes/invnorm/index.html
//This method of generating normal variates avoids the undesirable side effects 
//found by Neave in H.R. Neave "On using the Box-Muller transformation with multiplicative congurential 
//psuedo-random number generators"  Applied Statistics. 22 pp92, 1973
//The uniform vairates produced by this generator are suitable for use with low discrepancy numbers and quasi-Monte Carlo simulations.
//Inside the routine to generate the next variate, I asserted that the underlying uniform would be in (0,1).
//I hit the assert when the MT returned zero.  The assert was there to avoid calling log(0) in the tail part.
//So, I changed the code 
//assert( 0 <= u && u <= 1 );
//if (u==0)
//{
//      u=DBL_MIN; //So we don't take log of 0 down below.
//}
//So that I'm taking log ( 2.2250738585072014e-308)
//For N(0,1/2) the zero in the MT then gives a value of z = -26.530234309983367.
//Then I calculate for z \approx N(0,1/2)
//CDF(-26.530234309983367) = P( z <= -26.530234309983367 ) =2.1775825702786199e-155
//I'm unappy with that
//My plan is to change the undelrying uniform generator (based on MT) of N(0,1/2)  to have a range of [1/(2^32-1),1] insatead of [0,1]
//And compare with Intel MKL RV.
void TestMerssenePeriodIssue()
{
	unsigned int seed = 4357;

	klMersenneTwister badNumber(seed);
	klNormalInverseApproxRV<double> N_0_frac_1_2(0,0.5,false,seed);
	unsigned int max = 1171079842;
	for(int i=0;i<= max;i++)
	{
		unsigned int a =0;
		double d =0;

		if(i==max-1 || i==max)
		{
			a = badNumber.nextValue();
			d= N_0_frac_1_2();
			double prob_X_lt_d =  N_0_frac_1_2.NormalCDF(d);

		}
		else
		{
			a = badNumber.nextValue();
			d= N_0_frac_1_2();
		}
		if(a==0)
		{
			std::cout<< "umm...WE HAVE A PROBLEM HOUSTON"<<endl;
		}
	}


	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=NULL;
	if (!(matlabEngine = engOpen(NULL))) 
		throw "klWorkflow: no matlab engine available";

	klmtm.insert(thisThread,matlabEngine);
	matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	unsigned int n= 4096*10;
	klMatrix<double> A_GOE =SampleGOE(n);// SampleSymmetricStandardNormalRM(n);

	A_GOE /=n;
	unsigned int numEigenvalues= 16;//floor(n*.10);
	klArpackFunctor klaf;
	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);
	QueryPerformanceCounter(prefCountStart);

	QueryPerformanceCounter(prefCountStart);
	klVector<complex<double> > eigsAP = klaf.run( A_GOE,numEigenvalues);

	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc ARPACK  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	exit(42);
}

#include "limits.h"
#include <limits>
void IterativeKrylovCheck(ofstream &_tex,__int64 &n,const char* fileName)
{
	if (klTestSize!=klTestType::GROW && fileName==NULL)
	{
		//Then we're just doing simple test.
		n=128;
	}	

	_tex<<"Running Arnoldi Krylov algorithm on $GOE$ matrix and comparing to Lapack DGEES (via Intel MKL)."<<endl;
	
	klArpackFunctor klaf;

	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);

	klMatrix<double> A_GOE;

	if(fileName==NULL)
	{
		QueryPerformanceCounter(prefCountStart);
		A_GOE = SampleGOE(n);
		QueryPerformanceCounter(prefCountEnd);

		cerr<<"SampleGOE(n)="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
		_tex<<"SampleGOE(n)="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

		A_GOE /=n;
	}
	else
	{
		fstream _fileistream;
		QueryPerformanceCounter(prefCountStart);
		_fileistream.open(fileName);
		A_GOE.setup(n,n);
		_fileistream>>A_GOE;

		QueryPerformanceCounter(prefCountEnd);

		cerr<<"tic toc fileistream read dim n="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
		_tex<<"tic toc fileistream read dim n="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	
	}

	unsigned int numEigenvalues= 16;
	
	QueryPerformanceCounter(prefCountStart);

	klVector<complex<double> > eigsAP = klaf.run( A_GOE,numEigenvalues);
	
	//Write the eigenvectors
	char* arg = new char[128];
	for(int j=0;j<numEigenvalues;j++)
	{
		sprintf(arg,"%sEigenVector%d.txt",basefilename,j);	

		ofstream _fileostream(arg);
		_fileostream<<RE(klaf.EigenVectors[j])<<endl;
		_fileostream.close();
	}
	delete arg;
	
	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc ARPACK  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	cerr<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;
	_tex<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;

	QueryPerformanceCounter(prefCountStart);
	{
		klSYEVX<double> SYEVX(A_GOE,3);

		klDoubleVectorPtr ans = SYEVX();

		klVector<double> spectrum = *(ans.ptr());

		cout<<spectrum;

		klDoubleMatrixPtr E = SYEVX.Eigenvectors();

		cout<<*(E.ptr());
	}
	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc SYEVX  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   



	//QueryPerformanceCounter(prefCountStart);

	//klVector<complex<double> > eigs = A_GOE.eigenvalues();

	//QueryPerformanceCounter(prefCountEnd);
	//cout<<"TicToc MKL  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	////LatexPrintMatrix(A_GOE, "GOE(16)" ,_tex);

	//if (klTestSize!=klTestType::GROW)
	//{

	//	LatexPrintVector(eigsAP, "Eigenvalues via Arpack" , _tex);

	//	LatexPrintVector(eigs, "Eigenvalues via Intel MKL DGEES" , _tex);

	//	_tex.flush();
	//}
	//klVector<double> eigDiffs(numEigenvalues);

	//unsigned int i =0;
	//for(i =0;i<numEigenvalues;i++)
	//{
	//	complex<double> dt =eigsAP[i]-eigs[i];
	//	double eps = abs<double>(dt);
	//	eigDiffs[i]=eps;
	//}

	//double ell2dist = std::sqrt(eigDiffs.dotBLAS(eigDiffs));

	//cerr<<"Iterative Krylov dim="<<n<<" ell2 eig diff="<<ell2dist<<endl;
}

void Arpack_MKLsyevxSmokeTest(ofstream &_tex,__int64  &n,const char* fileName)
{
	makeLatexSection("ARPACK versus Lapack SYEVX",_tex);
	
	_tex<<"Running Arnoldi Krylov algorithm on affinity matrix and comparing to Lapack SYEVX (via Intel MKL)."<<endl;
	
	klArpackFunctor klaf;

	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);

	klMatrix<double> A_GOE;

	fstream _fileistream;
	QueryPerformanceCounter(prefCountStart);
	_fileistream.open(fileName);
	A_GOE.setup(n,n);
	_fileistream>>A_GOE;

	QueryPerformanceCounter(prefCountEnd);

	cerr<<"tic toc fileistream read dim n="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	_tex<<"tic toc fileistream read dim n="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	
	unsigned int numEigenvalues= 16;
	
	QueryPerformanceCounter(prefCountStart);

	klVector<complex<double> > eigsAP = klaf.run( A_GOE.transpose(),numEigenvalues);
	
	//Write the eigenvectors
	char* arg = new char[128];
	for(int j=0;j<numEigenvalues;j++)
	{
		sprintf(arg,"%sARPACK_EigenVector%d.txt",basefilename,j);	

		ofstream _fileostream(arg);
		_fileostream<<RE(klaf.EigenVectors[j])<<endl;
		_fileostream.close();
	}
	delete arg;
	
	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc ARPACK  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	cerr<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;
	_tex<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;

	QueryPerformanceCounter(prefCountStart);
	
	klSYEVX<double> SYEVX(A_GOE,3);

	klDoubleVectorPtr ans = SYEVX();

	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc SYEVX  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	klVector<double> spectrum = *(ans.ptr());

	//klDoubleMatrixPtr E = SYEVX.Eigenvectors();

	klMatrix<double> E = *(SYEVX.Eigenvectors().ptr());

	klMatrix<double> Etr= E.transpose();
	for(int j=0;j<numEigenvalues;j++)
	{
		sprintf(arg,"%sSYEVX_EigenVector%d.txt",basefilename,j);	

		ofstream _fileostream(arg);
		_fileostream<<Etr[j]<<endl;
		_fileostream.close();
	}
	delete arg;


	cout<<spectrum<<endl;
	cout<<eigsAP<<endl;

}

void FEATSEigensolver(ofstream &_tex,unsigned int  &n,const char* fileName)
{
	makeLatexSection("FEATS Eigensolver",_tex);
	
	_tex<<"Running FEATS Eigensolver algorithm on affinity matrix and comparing to ARPACK and Lapack SYEVX (via Intel MKL)."<<endl;
	
	klArpackFunctor klaf;

	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);
	//Write the eigenvectors
	char* arg = new char[128];

	klMatrix<double> A_GOE;

	fstream _fileistream;
	QueryPerformanceCounter(prefCountStart);
	_fileistream.open(fileName);
	A_GOE.setup(n,n);
	_fileistream>>A_GOE;

	QueryPerformanceCounter(prefCountEnd);

	cerr<<"tic toc fileistream read dim n="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	_tex<<"tic toc fileistream read dim n="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	
	unsigned int numEigenvalues= 16;
	
	QueryPerformanceCounter(prefCountStart);

	klVector<complex<double> > eigsAP = klaf.run( A_GOE.transpose(),numEigenvalues);
	
	for(int j=0;j<numEigenvalues;j++)
	{
		sprintf(arg,"%sARPACK_EigenVector%d.txt",basefilename,j);	

		ofstream _fileostream(arg);
		_fileostream<<RE(klaf.EigenVectors[j])<<endl;
		_fileostream.close();
	}
	
	
	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc ARPACK  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	cerr<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;
	_tex<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;

	{
		QueryPerformanceCounter(prefCountStart);

		klFEATS<double> FEATS(A_GOE,3);

		klDoubleVectorPtr ans = FEATS();

		QueryPerformanceCounter(prefCountEnd);
		cout<<"TicToc FEATS  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

		klVector<double> spectrum = *(ans.ptr());

		klMatrix<double> E = *(FEATS.Eigenvectors().ptr());

		klMatrix<double> Etr= E.transpose();
		for(int j=0;j<numEigenvalues;j++)
		{
			sprintf(arg,"%sFEATS_EigenVector%d.txt",basefilename,j);	

			ofstream _fileostream(arg);
			_fileostream<<Etr[j]<<endl;
			_fileostream.close();
		}
		
		cout<<spectrum<<endl;
		cout<<eigsAP<<endl;
	}

	{
		QueryPerformanceCounter(prefCountStart);

		klSYEVX<double> SYEVX(A_GOE,3);

		klDoubleVectorPtr ans = SYEVX();

		QueryPerformanceCounter(prefCountEnd);
		cout<<"TicToc SYEVX  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

		klVector<double> spectrum = *(ans.ptr());

		klMatrix<double> E = *(SYEVX.Eigenvectors().ptr());

		klMatrix<double> Etr= E.transpose();
		for(int j=0;j<numEigenvalues;j++)
		{
			sprintf(arg,"%sSYEVX_EigenVector%d.txt",basefilename,j);	

			ofstream _fileostream(arg);
			_fileostream<<Etr[j]<<endl;
			_fileostream.close();
		}
		

		cout<<spectrum<<endl;
		cout<<eigsAP<<endl;
		
		delete arg;
	}




}

void GenerateTraceyWidomSample(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Generate Tracey Widom Sample",_tex);

	//n is the size of the matrix that will be sampled
	unsigned int m; //Number or samples to generate 
	if (klTestSize==klTestType::VERYLARGE)
	{
     	n=1024*2;
		m=1024*4;
	}
	
	if (klTestSize==klTestType::LARGE)
	{
		//With n = 1024 m= 1024 this test should take about five hours on a 12 core 3Gz 96GB RAM workstation
		n=1024;
		m=1024;
	}

	if (klTestSize==klTestType::MEDIUM)
	{
		n=512;
		m=512;
	}

	if (klTestSize==klTestType::SMALL)
	{
		n=128;
		m=32;
	}

	makeLatexSection("Sample from $W_n m$ times and calculate empirical PDF of the first eig",_tex);
	_tex<<"Here we generate histograms of $\\lambda_1$ for GOE (Gaussian Orthogonal Ensemble), and W (Wishart) \
		 distributed of random matrices"<<endl;
	_tex<<"These should approximate the celebrated Tracy Widom distribution."<<endl;

	_tex<<"Dimension $n = "<<n<<"$"<<endl<<endl;
	_tex<<"Sample size $m = "<<m<<"$"<<endl<<endl;
	klTimer dti;
	{	
		klVector<complex<double> > lambda_1_Hist(m);
		unsigned int i;
		//#pragma omp parallel num_threads(4)
		for(i=0;i<m;i++)
		{
			dti.tic();
			klMatrix<double> A(n,n);
			A=SampleWishart(n,i);
			klVector<complex<double> > eigs=A.eigenvalues();
			lambda_1_Hist[i]=eigs[0];
			double dt = dti.toc();
			cerr<<"dt("<<i<<") = "<<dt<<endl;
		}

		//Real part of first eig
		klVector<double> L_re(m);
		//Imaginary part of first eig 
		klVector<double> L_im(m);

		unsigned int j=0;
		for(j=0;j<m;j++)
		{
			L_re[j]=lambda_1_Hist[j].real();
			L_im[j]=lambda_1_Hist[j].imag();
		}
		
		char* fileNameTW= new char[1024];
		sprintf(fileNameTW,"%sTraceyWidom_re.txt",basefilename);
		ofstream fileostreamobj(fileNameTW );
		fileostreamobj<<L_re<<endl;
		fileostreamobj.close();

		sprintf(fileNameTW,"%sTraceyWidom_im.txt",basefilename);
		fileostreamobj.open(fileNameTW);
		fileostreamobj<<L_im<<endl;
		fileostreamobj.close();
		delete fileNameTW;
		 
		LatexInsertHistogram(L_re,30,_tex, basefilename,"Re_TraceyWidom","Histogram of Re(\\lambda_1) A \\in W");
		LatexInsertHistogram(L_im,30,_tex, basefilename,"Im_TraceyWidom","Histogram of Im(\\lambda_1) A \\in W ");
		_tex.flush();
	}

	{			
		klVector<complex<double> > lambda_1_Hist(m);
		unsigned int i;
		//#pragma omp parallel num_threads(4)
		for(i=0;i<m;i++)
		{
			klMatrix<double> A(n,n);
			A=SampleGOE(n,i);
			klVector<complex<double> > eigs=A.eigenvalues();
			lambda_1_Hist[i]=eigs[0];
		}

		//Real part of first eig
		klVector<double> L_re(m);
		//Imaginary part of first eig 
		klVector<double> L_im(m);

		unsigned int j=0;
		for(j=0;j<m;j++)
		{
			L_re[j]=lambda_1_Hist[j].real();
			L_im[j]=lambda_1_Hist[j].imag();
		}
		
		char* fileName= new char[1024];
		sprintf(fileName,"%sWinger_re.txt",basefilename);
		ofstream fileostreamobj(fileName );
		fileostreamobj<<L_re<<endl;
		fileostreamobj.close();

		sprintf(fileName,"%sWinger_im.txt",basefilename);
		fileostreamobj.open(fileName);
		fileostreamobj<<L_im<<endl;
		fileostreamobj.close();

		LatexInsertHistogram(L_re,30,_tex, basefilename,"Re_Winger","Histogram of Re(\\lambda_1) A \\in GOE(1024)");
		LatexInsertHistogram(L_im,30,_tex, basefilename,"Im_Winger","Histogram of Im(\\lambda_1) A \\in GOE(1024) ");
		_tex.flush();

		delete fileName;
	}



}

#include "testmatgenunit.h"
bool testmatgen(bool silent);
klMatrix<double> real_2d_array_to_klMatrix(ap::real_2d_array a);
void MatrixNorm(ofstream &_tex,__int64  &n)
{	
	n= 12;

	makeLatexSection("Haar Distributed Random Orthogonal Matrix $A \\in O(n)$",_tex);
	_tex<<" Testing Operator Norm"<<endl<<"Number of Dimensions: "<<n<<endl<<endl; 

	testmatgen(false);
	ap::real_2d_array a;
	rmatrixrndorthogonal(n, a);
	klMatrix<double> Op = real_2d_array_to_klMatrix(a);
	klLU<double> LU(Op);
	klMatrix<double> u =LU();
	klMatrix<double> l =LU.L();

	klMatrix<double> lu = l*u;

	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$Det(A) :   A \\in O(n)$ = "<<Op.det()<<endl<<endl;
	LatexPrintMatrix(l, "L",_tex);
	LatexPrintMatrix(u, "U",_tex);
	LatexPrintMatrix(lu, "L * U ",_tex);
	_tex<<"$Det(L) :    = "<<l.det()<<"    "<<" Det(U) :    = "<<u.det()<<"    "<<" Det(LU) :    = "<<lu.det()<<"$"<<endl<<endl;
	_tex<<"$||A||_{L_1}$  = "<<Op.norm()<<endl<<endl;

	_tex<<"$||A||_{L_{\\infty}}$ = "<<Op.norm(true)<<endl<<endl;

	_tex<<"$||A^{-1}||_{L_1}$  = "<<Op.inverse().norm()<<endl<<endl;

	_tex<<"$||A^{-1}||_{L_{\\infty}}$ = "<<Op.inverse().norm(true)<<endl<<endl;

	_tex<<"$||A||_{L_{\\infty}} * ||A^{-1}||_{L_{\\infty}} = "<<Op.norm(true) * Op.inverse().norm(true)<<"$"<<endl<<endl;

	_tex<<"$||A||_{L_1} * ||A^{-1}||_{L_1} = "<< Op.norm() * Op.inverse().norm()<<"$"<<endl<<endl;

	double A_f =0;
	for(int i=0;i<n;i++)
	{
		A_f+=Op[i].pow_alpha(2).sum();		
	}
	A_f = sqrt(A_f);
	_tex<<"Frobenious Norm  $||A||_{\\textit{F}}$ via $\\sum\\limits_{i,j =0}^{n} \\|A_{i,j}|$   of  $A \\in O(n)$  "<<A_f<<endl<<endl;

	_tex<<"$L_1$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.ConditionNumber(1)<<endl<<endl;
	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$L_{\\infty}$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.ConditionNumber()<<endl<<endl;

	klVector<complex<double> > eigen =Op.eigenvalues();
	_tex<<"Eigenvalues of $A \\in O(n)$"<<endl<<endl;
	_tex<<eigen<<endl;

	klVector<double> argEigenvalues(n);
	for(int i=0;i<n;i++)
	{
		argEigenvalues[i] = std::abs(eigen[i]);
	}
	_tex<<" $|\\lambda | : \\lambda \\in \\sigma(A) , A \\in O(n)$"<<endl<<endl;
	_tex<<argEigenvalues<<endl<<endl;


	_tex<<"Calculating $A^{\\dag} A,$  we expect $A^{\\dag} A \\approx I$"<<endl<<endl;
	klMatrix<double> ortho(n,n);
	int i=0;
	int j=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			ortho[i][j] = Op.getColumn(i).dotBLAS(Op.getColumn(j));
		}
	}
	LatexPrintMatrix(ortho,"A^{\\dag} A",_tex);

	//test \alpha A x + \beta y
	//klMatrix<float> mvpBLAS(float alpha,  klMatrix<float> a,klVector<float> x, float beta,klVector<float> yi);
	klVector<double> Axbpy = mvpBLAS(1.0,  ortho,ortho[0], 2.0,ortho[1]);
	//_tex<<ortho[0]<<endl;
	//_tex<<ortho[1]<<endl;
	//_tex<<Axbpy<<endl;

	ortho = Op;

	_tex<<"Calculating $A^{-1} ,  A \\in O(n)$."<<endl<<endl;
	klMatrix<double> invOrtho= Op.inverse();
	LatexPrintMatrix(invOrtho,"A^{-1}",_tex);

	_tex<<"Calculating $A^{-1} *A  ,  A \\in O(n)$.   We expect $A^{-1} *A  \\approx I$. "<<endl<<endl;
	LatexPrintMatrix(invOrtho*Op,"A^{-1} *A",_tex);

	_tex<<"Calculating SVD of  $A \\in O(n)$"<<endl<<endl;

	{
		klSVD<double> SVD(Op);
		klDoubleVectorPtr Sigma = SVD();
		klMatrix<double> S = diag(*Sigma);
		klDoubleMatrixPtr U = SVD.U();
		klDoubleMatrixPtr V = SVD.V();

		LatexPrintMatrix(*U,"U",_tex);
		LatexPrintMatrix(S,"S",_tex);
		LatexPrintMatrix(*V,"V",_tex);

		LatexPrintMatrix( *(U) * (S) * *(V),"U S V",_tex);
	}

	makeLatexSection("Wishart Matrix $A \\in W(n)$",_tex);

	klMatrix<double> AW=     SampleWishart(n);
	_tex<<"$L_1$ condition number of Wishart Matrix "<<AW.ConditionNumber(true)<<endl;
	_tex<<"$L_\infty$ condition number of Wishart Matrix "<<AW.ConditionNumber()<<endl;

	makeLatexSection("Gaussian Orthogonal Ensemble $A \\in GOE(n)$",_tex);

	klMatrix<double> A_GOE = SampleGOE(n);
	klMatrix<double> Ainv=A_GOE.inverse();
	klMatrix<double> Id_goe=Ainv * A_GOE;
	Id_goe.threshold(0.001f,+0.0f);
	Id_goe = Id_goe + 0.0;
	klVector<double> x=Id_goe[0]; 
	klVector<double> y=Id_goe[2];

	_tex<<"$L_1$ condition number of GOE Matrix "<<A_GOE.ConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of GOE Matrix "<<A_GOE.ConditionNumber(true)<<endl;

	makeLatexSection("The Identity Matrix $I \\in M(n)$",_tex);

	klMatrix<double> Id(n,n);
	Id= IdentityMatrix<double>(n);
	_tex<<"$L_1$ condition number of $I$ = "<<Id.ConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of $I$ = "<<Id.ConditionNumber()<<endl;

	_tex.flush();
	
}

void MatrixEigenSolver(ofstream &_tex,__int64  &n)
{	
	makeLatexSection("Eigen Solver Checks",_tex);
	
	n= 8;

	makeLatexSection("Haar Distributed Random Orthogonal Matrix $A \\in O(n)$",_tex);
	_tex<<" Testing Operator Norm"<<endl<<"Number of Dimensions: "<<n<<endl<<endl; 

	testmatgen(false);
	ap::real_2d_array a;
	rmatrixrndorthogonal(n, a);
	klMatrix<double> Op = real_2d_array_to_klMatrix(a);
	{
		//Exercise symmetric EV solver

		klMatrix<double> F = Op*Op.transpose();
		
		cout<<F;
		
		klSYEVX<double> SYEVX(F,3);

		klDoubleVectorPtr ans = SYEVX();

		klVector<double> spectrum = *(ans.ptr());

		cout<<spectrum;

		klDoubleMatrixPtr E = SYEVX.Eigenvectors();

		cout<<*(E.ptr());

	}

	klLU<double> LU(Op);
	klMatrix<double> u =LU();
	klMatrix<double> l =LU.L();

	klMatrix<double> lu = l*u;

	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$Det(A) :   A \\in O(n)$ = "<<Op.det()<<endl<<endl;
	LatexPrintMatrix(l, "L",_tex);
	LatexPrintMatrix(u, "U",_tex);
	LatexPrintMatrix(lu, "L * U ",_tex);
	_tex<<"$Det(L) :    = "<<l.det()<<"    "<<" Det(U) :    = "<<u.det()<<"    "<<" Det(LU) :    = "<<lu.det()<<"$"<<endl<<endl;
	_tex<<"$||A||_{L_1}$  = "<<Op.norm()<<endl<<endl;

	_tex<<"$||A||_{L_{\\infty}}$ = "<<Op.norm(true)<<endl<<endl;

	_tex<<"$||A^{-1}||_{L_1}$  = "<<Op.inverse().norm()<<endl<<endl;

	_tex<<"$||A^{-1}||_{L_{\\infty}}$ = "<<Op.inverse().norm(true)<<endl<<endl;

	_tex<<"$||A||_{L_{\\infty}} * ||A^{-1}||_{L_{\\infty}} = "<<Op.norm(true) * Op.inverse().norm(true)<<"$"<<endl<<endl;

	_tex<<"$||A||_{L_1} * ||A^{-1}||_{L_1} = "<< Op.norm() * Op.inverse().norm()<<"$"<<endl<<endl;

	double A_f =0;
	for(int i=0;i<n;i++)
	{
		A_f+=Op[i].pow_alpha(2).sum();		
	}
	A_f = sqrt(A_f);
	_tex<<"Frobenious Norm  $||A||_{\\textit{F}}$ via $\\sum\\limits_{i,j =0}^{n} \\|A_{i,j}|$   of  $A \\in O(n)$  "<<A_f<<endl<<endl;

	_tex<<"$L_1$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.ConditionNumber(1)<<endl<<endl;
	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$L_{\\infty}$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.ConditionNumber()<<endl<<endl;

	klVector<complex<double> > eigen =Op.eigenvalues();
	_tex<<"Eigenvalues of $A \\in O(n)$"<<endl<<endl;
	_tex<<eigen<<endl;

	klVector<double> argEigenvalues(n);
	for(int i=0;i<n;i++)
	{
		argEigenvalues[i] = std::abs(eigen[i]);
	}
	_tex<<" $|\\lambda | : \\lambda \\in \\sigma(A) , A \\in O(n)$"<<endl<<endl;
	_tex<<argEigenvalues<<endl<<endl;


	_tex<<"Calculating $A^{\\dag} A,$  we expect $A^{\\dag} A \\approx I$"<<endl<<endl;
	klMatrix<double> ortho(n,n);
	int i=0;
	int j=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			ortho[i][j] = Op.getColumn(i).dotBLAS(Op.getColumn(j));
		}
	}
	LatexPrintMatrix(ortho,"A^{\\dag} A",_tex);

	//test \alpha A x + \beta y
	//klMatrix<float> mvpBLAS(float alpha,  klMatrix<float> a,klVector<float> x, float beta,klVector<float> yi);
	klVector<double> Axbpy = mvpBLAS(1.0,  ortho,ortho[0], 2.0,ortho[1]);
	//_tex<<ortho[0]<<endl;
	//_tex<<ortho[1]<<endl;
	//_tex<<Axbpy<<endl;

	ortho = Op;

	_tex<<"Calculating $A^{-1} ,  A \\in O(n)$."<<endl<<endl;
	klMatrix<double> invOrtho= Op.inverse();
	LatexPrintMatrix(invOrtho,"A^{-1}",_tex);

	_tex<<"Calculating $A^{-1} *A  ,  A \\in O(n)$.   We expect $A^{-1} *A  \\approx I$. "<<endl<<endl;
	LatexPrintMatrix(invOrtho*Op,"A^{-1} *A",_tex);

	_tex<<"Calculating SVD of  $A \\in O(n)$"<<endl<<endl;

	{
		klSVD<double> SVD(Op);
		klDoubleVectorPtr Sigma = SVD();
		klMatrix<double> S = diag(*Sigma);
		klDoubleMatrixPtr U = SVD.U();
		klDoubleMatrixPtr V = SVD.V();

		LatexPrintMatrix(*U,"U",_tex);
		LatexPrintMatrix(S,"S",_tex);
		LatexPrintMatrix(*V,"V",_tex);

		LatexPrintMatrix( *(U) * (S) * *(V),"U S V",_tex);
	}


	_tex<<"Calculating first few eigenvectors of $A \\in O(n)$ using LAPACK syevx"<<endl<<endl;

	makeLatexSection("Wishart Matrix $A \\in W(n)$",_tex);

	klMatrix<double> AW=     SampleWishart(n);
	_tex<<"$L_1$ condition number of Wishart Matrix "<<AW.ConditionNumber(true)<<endl;
	_tex<<"$L_\infty$ condition number of Wishart Matrix "<<AW.ConditionNumber()<<endl;

	makeLatexSection("Gaussian Orthogonal Ensemble $A \\in GOE(n)$",_tex);

	klMatrix<double> A_GOE = SampleGOE(n);
	klMatrix<double> Ainv=A_GOE.inverse();
	klMatrix<double> Id_goe=Ainv * A_GOE;
	Id_goe.threshold(0.001f,+0.0f);
	Id_goe = Id_goe + 0.0;
	klVector<double> x=Id_goe[0]; 
	klVector<double> y=Id_goe[2];

	_tex<<"$L_1$ condition number of GOE Matrix "<<A_GOE.ConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of GOE Matrix "<<A_GOE.ConditionNumber(true)<<endl;

	makeLatexSection("The Identity Matrix $I \\in M(n)$",_tex);

	klMatrix<double> Id(n,n);
	Id= IdentityMatrix<double>(n);
	_tex<<"$L_1$ condition number of $I$ = "<<Id.ConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of $I$ = "<<Id.ConditionNumber()<<endl;

	_tex.flush();
	
}

void Utility(ofstream &_tex,__int64  &n )
{
	//This works
	try
	{
		throw klError("Testing kl Stack Walking and Memory Dump");
	}
	catch(klError e)
	{
		_tex<<"Caught klError with message  : ";
		_tex<<e.what()<<endl;
		std::stringstream ANSI_INFO_ss (std::stringstream::in | std::stringstream::out );
		ANSI_INFO_ss<<"ANSI COMPILE INFO: " <<__DATE__<<"     "<<__TIME__<<"   "<<__FILE__<<"   "<<__LINE__<<"       "<<std::endl;
		std::string err = ANSI_INFO_ss.str();
		_tex<<err;
	}
	
	makeLatexSection("Testing kl Floating Point and Memory Utilities",_tex);

	klFlotingPointHelper myFPH;
	int* er=myFPH.fpError();
	unsigned int st86=myFPH.status_x86();
	unsigned int stSSE=myFPH.status_SSE();
	const char* x86Status= myFPH.get_x86_ErrorStringFromStatus(st86);
	const char*  fpERr= myFPH.get_fp_ErrorStringFromStatus(er);

	unsigned int clV87=myFPH.Clear87();

	unsigned int clV=myFPH.Clear();

	delete x86Status;
	delete fpERr;

#ifdef _M_IX86
	bool isInsideVM = klIsInsideVPC();
	_tex<<"klIsInsideVPC returned"<<isInsideVM<<endl;
#endif

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
	_CrtMemState * _State;
	_CrtMemState s1, s2, s3;

	_CrtMemCheckpoint( &s1 );
	_CrtMemDumpStatistics( &s1 );
	int* p=new int[1024];
	_CrtMemCheckpoint( &s2 );
	_CrtMemDumpStatistics( &s2 );

	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	delete p;
	_CrtDumpMemoryLeaks();

#endif
	_tex.flush();

}

void MatrixMultiplicationCheck(ofstream &_tex,__int64  &n  )
{		
	{
		n=2048;
		_tex<<"Comparing naive matrix multiply verus Intel MKL dgemm for matrix of size "<<n<<"."<<endl;
		_tex<<"This is for type double (hence the d in dgemm)."<<endl;

		unsigned int m=n+128;
		unsigned int p=n+64;

		klMatrix<double> A(n,m);

		unsigned int i=0;
		unsigned int j=0;
		for(i=0;i<n;i++)
		{
			for(j=0;j<m;j++)
			{
				A[i][j]=i+j;
			}
		}

		klMatrix<double> B(m,p);
		for(i=0;i<m;i++)
		{
			for(j=0;j<p;j++)
			{
				B[i][j]=i+j;
			}
		}

		//bbcrevisit make this easier - like tic toc !  Will requier a file scope or global static object.
		LARGE_INTEGER* freq;
		_LARGE_INTEGER* prefCountStart;
		_LARGE_INTEGER* prefCountEnd;
		freq=new _LARGE_INTEGER;
		prefCountStart=new _LARGE_INTEGER;
		prefCountEnd=new _LARGE_INTEGER;
		QueryPerformanceFrequency(freq);

		QueryPerformanceCounter(prefCountStart);
		klMatrix<double> C = A*B;
		QueryPerformanceCounter(prefCountEnd);
		_tex<<"Naive type double matrix multiply tic toc  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
		klMatrix<double> Cp(n,p);

		QueryPerformanceCounter(prefCountStart);
		klMatrix<double> Ap =mmBLAS(1.0, A, B, 1.0,Cp);	
		QueryPerformanceCounter(prefCountEnd);
		_tex<<"dgemm plus row to column major transpose operation tic toc  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	}

	{
		n=2048;
		_tex<<"Comparing naive matrix multiply verus Intel MKL sgemm for matrix of size "<<n<<"."<<endl;
		_tex<<"This is for type float (hence the s in dgemm)."<<endl;

		unsigned int m=n+128;
		unsigned int p=n+64;

		klMatrix<float> A(n,m);

		unsigned int i=0;
		unsigned int j=0;
		for(i=0;i<n;i++)
		{
			for(j=0;j<m;j++)
			{
				A[i][j]=i+j;
			}
		}

		klMatrix<float> B(m,p);
		for(i=0;i<m;i++)
		{
			for(j=0;j<p;j++)
			{
				B[i][j]=i+j;
			}
		}

		LARGE_INTEGER* freq;
		_LARGE_INTEGER* prefCountStart;
		_LARGE_INTEGER* prefCountEnd;
		freq=new _LARGE_INTEGER;
		prefCountStart=new _LARGE_INTEGER;
		prefCountEnd=new _LARGE_INTEGER;
		QueryPerformanceFrequency(freq);

		QueryPerformanceCounter(prefCountStart);
		klMatrix<float> C = A*B;
		QueryPerformanceCounter(prefCountEnd);
		_tex<<"Naive type float matrix multiply tic toc  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

		klMatrix<float> Cp(n,p);	

		QueryPerformanceCounter(prefCountStart);
		klMatrix<float> Ap =mmBLAS(1.0, A, B, 1.0,Cp);
		QueryPerformanceCounter(prefCountEnd);
		_tex<<"sgemm plus row to column major transpose operation tic toc  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	}
	_tex.flush();
}

void LinearRegression(ofstream &_tex,__int64 &n)
{
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);
	makeLatexSection("3 x 1 Linear Regression",_tex);

	unsigned int sampleSize=64;
	_tex<<"Sample size = "<<sampleSize<<endl<<endl;
	unsigned int numFeatures=3;
	_tex<<"Number of features = "<<numFeatures<<endl<<endl;

	klMatrix<double> X(sampleSize,numFeatures); X=0;
	klMatrix<double> Y(sampleSize,1);

	klUniformRV<double> uniformRV(0,1);
	klNormalInverseApproxRV<double> N_0_eps(0,0.05,false,42);
	unsigned int i;
	unsigned int j;

	klVector<double> mean(numFeatures);
	klVector<double> beta(numFeatures);
	klVector<double> zero(numFeatures);

	mean = 0;
	zero =0;
	for(i=0;i<numFeatures;i++)
	{
		beta[i]=uniformRV();
	}

	klMatrix<double> Sigma = klGenerateRandomSymmetricPositiveDefiniteMatrix<double> (numFeatures);
	LatexPrintMatrix(Sigma, "\\sigma",_tex);
	klNormalMultiVariate<double> features(zero,Sigma);

	//We could add niose to the features Y = \beta \dot \(X+\epsilon}
	//But for now we add noise to the response [check usage of term] 
	// Y = \beta \dot X + \epsion.
	for(j=0;j<sampleSize;j++)
	{
		klVector<double> sample =features();
		cout<<sample<<endl;
		klVector<double > rowj =X[j];
		X.setRow(j, sample);
		double response = X[j].dot(beta);
		Y[j] =  response;  //This works because Y is 1 dimensional, otherwise use setRow
	}
	//bbcrevisit expand model to multiple linear regression and [standardize notation ]
	//Model Y=BX + \epsilon where \epsilon =_d N(\mathbf{0},mathbf{\Sigma})
	//Usually E[Y_i]=\mu_i=x_i^T \beta where Y_i =_d N(\mu_i,\sigma);

	LatexInsert3DPlot(X, _tex, basefilename,"regression_features","Features");

	_tex<<"Beta"<<endl;

	_tex<<beta<<endl;
	_tex<<"Response"<<endl;
	_tex<<Y;
	klLinearRegression<double> R(X,Y);
	klMatrix<double> betahat=R();

	klMatrix<double> betaEst=betahat.getSubBlock(0,0,2,0);
	_tex<<"Estimate for Beta"<<endl;
	_tex<<betaEst;

	_tex<<"Error:"<<endl;
	klVector<double> error(numFeatures);
	for(int j =0 ; j<numFeatures;j++)
	{
		double b1=betahat[j][0];
		double b2 =beta[j];
		error[j] = b1-b2;
	}
	_tex<<error<<endl<<endl;
	_tex.flush();
}

void GenerativeGramConsistencyCheck(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Gram Matrix Consistency Check",_tex);
	
	//Notes from On the Nystrom Method for Approximating a Gram Matrix for Improved Kernel-Based Learning by 
	//Petros Drineas DRINEP@CS.RPI.EDU Department of Computer Science Rensselaer Polytechnic Institute

	//Given a collection X of data points, which are often but not necessarily elements of R^m, techniques
	//such as PCA, SVM, GP and SVD can be used to identify structure from X by computing linear functions.
	//PCA calculates the subspace spanned by the first k eigenvectors and is used to give a k dimensional
	//model of the data with minimal residual.  This type of  spectral analysis has a long theoretical history and forms the basis 
	//of many modern machine learning techniques. In real world applications however there is typically nonlinear structure in the data.
	//Some domains sucha as Natural Language processing (NLP) data may not be amenable to the definition of meaningful linear operations.
	//It is for such situations that kernel learning methods were developed.  Kernel methods map data in to high 
	//dimensional space and use inner products designed to capture positional information.  Kernel methods use 
	//the inner product distances for classification and regression.Kernel methods exploit the information encoded in the inner product
	//between all pairs of data items and are successful because there is often an efficient method to
	//compute inner products between very complex infinite dimensional vectors. Kernel
	//algorithms provide a way to deal with nonlinear structure by reducing nonlinear algorithms
	//to algorithms that are linear in some feature space F.  F is nonlinearly related to the original input
	//space.  
	//Let x_i \ in \dblr^m X = [  ...x_i ... ] row wise. Here we test the linear structure of a genreative data set against the
	//Gream kernel. In kernel-based methods, a set of features is chosen that define a space F , where it is hoped relevant structure will be 
	//revealed, the data X are then mapped to the feature space F using a mapping F : X  -> F , and then classification, regression, or
	//clustering is performed in F using traditional methods such as linear SVMs, GPs, or PCA. If F
	//is chosen to be a dot product space and if one defines the kernel matrix, also known as the Gram
	//matrix, G \in  \dlbr^{n×n} as G_ij = k(x_i, x_j) = (F(xi),F(xj)), then any algorithm whose operations can be
	//expressed in the input space in terms of dot products can be generalized to an algorithm which
	//operates in the feature space by substituting a kernel function for the inner product. In practice, this
	//means presenting the Gram matrix G in place of the input covariance matrix X^\dag X. Relatedly, using
	//the kernel k instead of a dot product in the input space corresponds to mapping the data set into a
	//(usually) high-dimensional dot product space F by a (usually nonlinear) mapping \Phi : \dblr^m -> F , and
	//taking dot products there, i.e., k(x_i, x_j) = (F(x_i),F(x_j)). Note that for the commonly-used Mercer
	//kernels, G is a symmetric positive semidefinite (SPSD) matrix.
	//The generality of this framework should be emphasized. For example, there has been much
	//work recently on dimensionality reduction for nonlinear manifolds in high-dimensional spaces. 
	//Ie Isomap, LLE, and graph Laplacian eigenmap, and Hessian eigenmaps.  These methods first induce a local neighborhood 
	//structure on the data and then use this local structure to find a global embedding of the manifold in a lower dimensional space.
	
	unsigned int numFeatures=3;
	unsigned int sampleSize=4096;
	//Is this PSD? If not then we will not get a good Cholesky factorization? 
	//One way to find out is to find eigs, and see is \sigma(W) \in [0,\inflty)
	klMatrix<double> SigmaW=SampleWishart(numFeatures);

	_tex<<"Sample Size = "<<sampleSize<<endl;

	_tex<<"Feature dim = "<<numFeatures<<endl<<endl;

	LatexPrintMatrix<double>(SigmaW,"$\Sigma$",_tex);

	unsigned int i;
	unsigned int j;
	klVector<double> meanVector(numFeatures);
	meanVector=1;
	
	klNormalMultiVariate<double> T(meanVector,SigmaW );
	klSamplePopulation<double> X(sampleSize,numFeatures);
	for(j=0;j<sampleSize;j++)
	{
		X.setRow(j,T());
	}

	klMatrix<double> SampleCovariance = X.covarianceMatrix();

	LatexPrintMatrix<double>(SampleCovariance,"Sample Covariance",_tex);
	
	LatexPrintVector<double>(X.sampleMean(),"Sample Mean",_tex);

	//Git ISSUE #10 ( Makes extra Copy ?)
	klMatrix<double> D  = SampleCovariance-SigmaW;                           
	LatexPrintMatrix<double>(D,"Sample Covariance-$\Omega$",_tex);
	
	klPrincipalComponents<double> pca=X;
	klMatrix<double> VC =pca();
	klVector<complex<double> > covarianceEigs=SampleCovariance.eigenvalues();
	klVector<double > pcaEigs = pca.eigenvalues();
	LatexPrintVector<complex<double>  >(covarianceEigs,"Sample Covariance Eigs",_tex);
		
	klout(SigmaW);
	klout(SampleCovariance);
	klout(D);
	klout(VC);
	klout(covarianceEigs);
	klout(pcaEigs);

	//How close is the sample covariance to a PSD matrix?
	//First we'll need a measure of matrix closeness and a way to find the 
	//nearest PSD matrix in that measure. 

	//The Gram matrix G of a set of vectors x_1, ...,x_i,...x_n in an inner product space is the Hermitian matrix of inner products, whose entries are given by G_{ij} = <x_i,x_j>
	//An important application is to compute linear independence: a set of vectors is linearly independent if and only if the determinant of the Gram matrix is non-zero.

	klMatrix<double> G(sampleSize,sampleSize);
	G.makeNanFriendly();
	for(i=0;i<sampleSize;i++)
	{
		for(j=0;j<sampleSize;j++)
		{
			G[i][j]=( X[i]).dotBLAS(X[j]);
		}
	}

	klMatrix<double> centered = X.centeredData();
	klSamplePopulation<double> normalizedSample(centered);
	klVector<double> normedMean = normalizedSample.sampleMean();

	LatexPrintVector<double>(normalizedSample.sampleMean(),"Centered Mean",_tex);

	LatexPrintMatrix<double>(normalizedSample.covarianceMatrix(),"Centered Covariance",_tex);
	
	klMatrix<double> Gf(numFeatures,numFeatures);
	for(i=0;i<numFeatures;i++)
	{
		for(j=0;j<numFeatures;j++)
		{
			Gf[i][j]=(centered.getColumn(i)).dotBLAS(centered.getColumn(j));
		}
	}

	LatexPrintMatrix<double>( Gf,"Gram Matrix Gf Not scaled by sample size",_tex);

	//bbcreivist Git ISSUE #10 ( Makes extra Copy ?)
	klMatrix<double> scalesGf = Gf / (double) sampleSize;
	LatexPrintMatrix<double>(scalesGf ,"Gram Matrix Gf  scaled by sample size",_tex);

	klMatrix<double> diff = SampleCovariance / (double) sampleSize;

	LatexPrintMatrix<double>(diff ,"SampleCovariance - Scaled Gf",_tex);

	LatexPrintMatrix<double>(VC ,"EigenDecomp of SampleCovariance",_tex);
	
	klPrincipalComponents<double> pcaGram=Gf;
	klMatrix<double> VCGram =pcaGram();

	LatexPrintMatrix<double>(VCGram ,"EigenDecomp of Gram Matrix",_tex);

	}

void IteratedExponentialFiltering(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Iterated Exponential Filtering ",_tex);
	
	size_t popsize=1024*2;
	klTimeSeries<double> c(popsize);
	klNormalInverseApproxRV<double> normalinv(0,0.1);
	unsigned i;
	for(i=0;i<popsize;i++)
	{
		double pi= 3.141592653589793238462643383279502;
		c[i]=normalinv()+ .5* sin(4*pi*(double(i)/popsize)) + 1* sin(7*pi*(double(i)/popsize) );
	}
	

	_tex<<"$\\mu_1 =" <<c.mean()<<"$"<<endl;
	_tex<<"$\\mu_2 =" <<c.variance()<<"$"<<endl;
	_tex<<"$\\mu_3 =" <<c.skewness()<<"$"<<endl;
	_tex<<"$\\mu_4 =" <<c.kurtosis()<<"$"<<endl;

	klTimeSeries<double>::klTimeSeriesInterpolation interp=klTimeSeries<double>::klTimeSeriesInterpolation::PREVIOUS;

	double tau=12.0;

	klTimeSeries<double> ema=c.EMA(popsize,tau,interp);

	klTimeSeries<double> iema=c.IEMA(popsize,6,tau,interp);

	klTimeSeries<double> ma=c.MA(popsize,6,tau,interp);

	double gamma=1.22208;
	double beta=0.65;
	double alpha=1/(gamma*(8* beta - 3));

	klTimeSeries<double> diff=c.DIFF(popsize,gamma,beta,alpha,64,interp);
	
	LatexInsert1DPlot(c,_tex,basefilename,"EMA_signal","EMA Signal",klHoldOnStatus::NoHold);

	LatexInsert1DPlot(ma,_tex,basefilename,"MA","MA",klHoldOnStatus::NoHold);

	LatexInsert1DPlot(iema,_tex,basefilename,"IEMA","Iterated Exponential Moving Average",klHoldOnStatus::NoHold);

	LatexInsert1DPlot(ema,_tex,basefilename,"EMA","Exponential Moving Average",klHoldOnStatus::NoHold);

	LatexInsert1DPlot(diff,_tex,basefilename,"DIFF","Diff operator",klHoldOnStatus::NoHold);

	//Put everything in onle plot
	{
		char* markerSpec = "'r'";
		LatexInsert1DPlot(c,_tex,basefilename,"EMA_signal","EMA Signal",klHoldOnStatus::FirstPlot,markerSpec);

		markerSpec = "'g'";
		LatexInsert1DPlot(ma,_tex,basefilename,"MA","MA",klHoldOnStatus::HoldOn,markerSpec);

		markerSpec = "'b'";
		LatexInsert1DPlot(iema,_tex,basefilename,"IEMA","Iterated Exponential Moving Average",klHoldOnStatus::HoldOn,markerSpec);

		markerSpec = "'c'";
		LatexInsert1DPlot(ema,_tex,basefilename,"EMA","Exponential Moving Average",klHoldOnStatus::HoldOn,markerSpec);
		
		markerSpec = "'m'";
		LatexInsert1DPlot(diff,_tex,basefilename,"IteratedExponentailOperators","Iterated Exponentail Operators",klHoldOnStatus::LastPlot,markerSpec);
					
	}


	//Put everything in onle plot
	{
		char* markerSpec = "'r'";
		LatexInsert1DPlot(c,_tex,basefilename,"EMA_signal","EMA Signal",klHoldOnStatus::FirstPlot,markerSpec);

		markerSpec = "'g'";
		LatexInsert1DPlot(ma,_tex,basefilename,"MA","MA",klHoldOnStatus::HoldOn,markerSpec);

		markerSpec = "'b'";
		LatexInsert1DPlot(iema,_tex,basefilename,"IEMA","Iterated Exponential Moving Average",klHoldOnStatus::HoldOn,markerSpec);

		markerSpec = "'c'";
		LatexInsert1DPlot(ema,_tex,basefilename,"EMA","Exponential Moving Average",klHoldOnStatus::HoldOn,markerSpec);
		
		markerSpec = "'m'";
		LatexInsert1DPlot(diff,_tex,basefilename,"IteratedExponentailOperators","Iterated Exponentail Operators",klHoldOnStatus::HoldOn,markerSpec);

		LatexInsertLegend("'Signal','MA','IEMA','EMA','Diff'");

		WritePlot(_tex, basefilename,"IteratedExponentailOperators","Iterated Exponentail Operators");
		
	}

	c[popsize-1024]=1237;//big shock

	_tex.flush();

}

void VSLFunctions(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Intel VSL Function Check",_tex);
	
	//vdInv		Inversion of vector elements
//void klVSLInv(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLInv(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLInv","Inversion of vector elements",klHoldOnStatus::NoHold);
	}
	
////vdSqrt Computation of the square root of vector elements
//void klVSLSqrt(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLSqrt(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLSqrt","sqrt of vector elements",klHoldOnStatus::NoHold);
	}
////vdExp	Computation of the exponential of vector elements
//void klVSLExp(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLExp(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLExp","exponentail of vector elements",klHoldOnStatus::NoHold);
	}
////vdExpm1		Computation of the exponential of vector elements decreased by 1
//void klVSLExpm1(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLExpm1(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLExpm1","exponential of vector elements decreased by 1",klHoldOnStatus::NoHold);
	}
////vdLn	Computation of the natural logarithm of vector elements
//void klVSLLn(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLLn(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLLn","natural logarithm of vector elements",klHoldOnStatus::NoHold);
	}
////vdLog10		Computation of the denary logarithm of vector elements
//void klVSLLog10(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLLog10(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLLog10","denary lograrithm of vector elements",klHoldOnStatus::NoHold);
	}
////vdCos		Computation of the cosine of vector elements
//void klVSLCos(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLCos(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLCos","cosine of vector elements",klHoldOnStatus::NoHold);
	}
////vdSin		Computation of the sine of vector elements
//void klVSLSin(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLSin(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLSin","sine of vector elements",klHoldOnStatus::NoHold);
	}
////vdTan		Computation of the tangent of vector elements
//void klVSLTan(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLTan(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLTan","tangent of vector elements",klHoldOnStatus::NoHold);
	}
////vdAcos		Computation of the inverse cosine of vector elements
//void klVSLAcos(klVector<double>&  v,klVector<double>& ans);
	//{
	//klVector<double> a(0.01,0.01,1);
	//klVSLAcos(a,a);
	//LatexInsert1DPlot(a,_tex,basefilename,"klVSLAcos","inverse cosine of vector elements decreased by 1",false);
	//}
////vdAsin		Computation of the inverse sine of vector elements
//void klVSLAsin(klVector<double>&  v,klVector<double>& ans);
//
////vdAtan		Computation of the inverse tangent of vector elements
//void klVSLAtan(klVector<double>&  v,klVector<double>& ans);
//
////vdCosh		Computation of the hyperbolic cosine of vector elements
//void klVSLCosh(klVector<double>&  v,klVector<double>& ans);
//
////vdSinh		Computation of the hyperbolic sine of vector elements
//void klVSLSinh(klVector<double>&  v,klVector<double>& ans);
//
////vdTanh		Computation of the hyperbolic tangent of vector elements
//void klVSLTanh(klVector<double>&  v,klVector<double>& ans);
//
////vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
//void klVSLAcosh(klVector<double>&  v,klVector<double>& ans);
//
////vdAsinh		Computation of the inverse hyperbolic sine of vector elements
//void klVSLAsinh(klVector<double>&  v,klVector<double>& ans);
//
////vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
//void klVSLAtanh(klVector<double>&  v,klVector<double>& ans);
//
////vdErf		Computation of the error function value of vector elements
//void klVSLErf(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLErf(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLErf","error function of vector elements",klHoldOnStatus::NoHold);
	}
////vdErfc		Computation of the complementary error function value of vector elements
//void klVSLErfc(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLErfc(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLErfc","complementary error function of vector elements",klHoldOnStatus::NoHold);
	}
////vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
//void klVSLCdfNorm(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLCdfNorm(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLCdfNorm","cumulative normal distribution function of vector elements",klHoldOnStatus::NoHold);
	}
////vdErfInv		Computation of the inverse error function value of vector elements
//void klVSLErfInv(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLErfInv(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLErfInv","inverse error function of vector elements",klHoldOnStatus::NoHold);
	}
////vdErfcInv		Computation of the inverse complementary error function value of vector elements
//void klVSLErfcInv(klVector<double>&  v,klVector<double>& ans);
//
////vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
//void klVSLCdfNormInv(klVector<double>&  v,klVector<double>& ans);
//
////vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
//void klVSLLGamma(klVector<double>&  v,klVector<double>& ans);
	{
	klVector<double> a(0.01,0.01,1);
	klVSLLGamma(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLLGamma","logarithm for the absolute value of the gamma function of vector elements",klHoldOnStatus::NoHold);
	}
////vdTGamma		Computation of the gamma function of vector elements 
//void klVSLTGamma(klVector<double>&  v,klVector<double>& ans);
		{
	klVector<double> a(0.01,0.01,1);
	klVSLTGamma(a,a);
	LatexInsert1DPlot(a,_tex,basefilename,"klVSLTGamma","gamma function of vector elements",klHoldOnStatus::NoHold);
	}
}

void testKlBinaryOI(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Testing binary writer",_tex);
	double pi= 3.141592653589793238462643383279502;
	klTimer klt;
	{
		__int64 GBWorthOfDoubles = __int64(1073741824LL/sizeof(double));
		__int64 rzG = __int64 (std::sqrt ((double)GBWorthOfDoubles)) ;

		//klMatrix<double> klmd (rzG,rzG);
		klMatrix<double> klmd (362,362);
		klmd =pi;
		stringstream ss;
		klt.tic();
		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestMatrix.klmd";
		klBinaryIO::WriteWinx64( klmd, ss.str() );
		double bwtoc=klt.toc();

		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestMatrix.txt";
		ofstream fileostreamobj(ss.str() );
		klt.tic();
        fileostreamobj<<klmd<<endl;
        fileostreamobj.close();
		double swtoc = klt.toc();

		_tex<<"Binary writer Speedup 1GB Double Matrix "<< swtoc/bwtoc<<endl<<endl;

		__int64 rows,cols;

		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestMatrix.klmd";	
		klBinaryIO::QueryWinx64(ss.str(),rows,cols);

		klMatrix<double> klmdMat(rows,cols);
		klt.tic();
		klBinaryIO::MatReadWinx64(ss.str(),klmdMat);
		bwtoc =klt.toc();

		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestMatrix.txt";
		ifstream fileistreamobj(ss.str() );
		klt.tic();
        fileistreamobj>>klmdMat;
        fileistreamobj.close();
		swtoc = klt.toc();
		_tex<<"Binary reader Speedup 1GB Double Matrix "<< swtoc/bwtoc<<endl<<endl;

		
	}

	{
		__int64 GBWorthOfDoubles = __int64(1073741824LL/sizeof(double));

		//klVector<double> klvd (GBWorthOfDoubles);
		klVector<double> klvd (131072);
		klvd =pi;

		stringstream ss;
		klt.tic();
		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestVector.klvd";
		klBinaryIO::WriteWinx64( klvd, ss.str() );
		double bwtoc=klt.toc();

		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestVector.txt";
		ofstream fileostreamobj(ss.str() );
		klt.tic();
        fileostreamobj<<klvd<<endl;
        fileostreamobj.close();
		double swtoc = klt.toc();

		_tex<<"Binary writer Speedup 1GB Double vector "<< swtoc/bwtoc<<endl<<endl;

		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestVector.klvd";
		__int64 rows,cols;
		klBinaryIO::QueryWinx64(ss.str(),rows,cols);

		if (rows!=0)
			throw "klBinaryIO::QueryWinx64(fileName ,rows,cols) returned non zero rows for vector"; 
		klVector<double> readklvd(cols);
		klt.tic();
		klBinaryIO::VecReadWinx64(ss.str(),readklvd);
		bwtoc =klt.toc();

		ss.str("");ss.clear();
		ss<<basefilename<<"//WriterTestVector.txt";
		ifstream fileistreamobj(ss.str() );
		klt.tic();
        fileistreamobj>>readklvd;
        fileistreamobj.close();
		swtoc = klt.toc();
		_tex<<"Binary reader Speedup 1GB Double Matrix "<< swtoc/bwtoc<<endl<<endl;
	}
}

void testPointCloudAndLatexPlots(ofstream &_tex,__int64 &n)
{
	makeLatexSection("Testing Gaussian Mixture Point Cloud and Latex Plotting Capabilities.",_tex);

	{	
		unsigned int numPoints = 800;
		unsigned int numSources=numPoints;
		unsigned int numCenters = 2;
		int dimension =3;

		//__int64 numPointsPerCenter, __int64 numCenters,__int64 dimension ,double scale
		klGaussianMixture X(numPoints/numCenters,numCenters,dimension,1.0f /950.0f);
		klGaussianMixture Y(numPoints/numCenters,numCenters,dimension,1.0f /950.0f);
		klGaussianMixture Z(numPoints/numCenters,numCenters,dimension,1.0f /950.0f);

		stringstream fileName;stringstream title;
		fileName.str("");fileName.clear();
		title.str(""); title.clear();
		fileName<<"GaussianMixture_Dim_3"<<"_Centers"<<numCenters;
		title<<"3 Gaussian Mixtures"<<numCenters<<"_Centers";
		char* color="'r.'";
		LatexInsert3DPlot(X.getData(),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::FirstPlot, color);
		color ="'g.'";
		LatexInsert3DPlot(Y.getData(),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::HoldOn, color);
		color= "'b.'";
		LatexInsert3DPlot(Z.getData(),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::LastPlot, color);
	}
	
	{	
		unsigned int numPoints = 800;
		unsigned int numSources=numPoints;
		unsigned int numCenters = 2;
		int dimension =1;

		//__int64 numPointsPerCenter, __int64 numCenters,__int64 dimension ,double scale
		klGaussianMixture X(numPoints/numCenters,numCenters,dimension,1.0f /950.0f);
		klGaussianMixture Y(numPoints/numCenters,numCenters,dimension,1.0f /950.0f);
		klGaussianMixture Z(numPoints/numCenters,numCenters,dimension,1.0f /950.0f);
						
		stringstream fileName;stringstream title;
		fileName.str("");fileName.clear();
		title.str(""); title.clear();
		fileName<<"GaussianMixture_Dim_1"<<"_Centers"<<numCenters;
		title<<"Gaussian Mixtures"<<numCenters<<"_Centers";
		char* color="'r.'";
		LatexInsert1DPlot(X.getData().getColumn(0),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::FirstPlot, color);
		color ="'g.'";
		LatexInsert1DPlot(Y.getData().getColumn(0),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::HoldOn, color);
		color= "'b.'";
		LatexInsert1DPlot(Z.getData().getColumn(0),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::LastPlot, color);
	}
}

void testMatrixNorms(ofstream &_tex,__int64 &n)
{
	
	{
		klMatrix<double> G(4,3);

	G[0][0]= 1, G[0][1]=2, G[0][2]=3,
	G[1][0]= 4, G[1][1]=5, G[1][2]=6,
	G[2][0]= 7, G[2][1]=8, G[2][2]=9,
	G[3][0]= 10, G[3][1]=11, G[3][2]=12;

	double n1=G.norm(false);

	double n2=G.norm(true);

	double c1 = G.ConditionNumber(false);

	double c2 = G.ConditionNumber(true);
	}

	
	
	{
		klMatrix<double> G =SampleGOE(n);
	
	double n1=G.norm(false);

	double n2=G.norm(true);

	double c1 = G.ConditionNumber(false);

	double c2 = G.ConditionNumber(true);
	}

		{
		klMatrix<double> G =HilbertMatrix(n);
		
	double n1=G.norm(false);

	double n2=G.norm(true);

	double c1 = G.ConditionNumber(false);

	double c2 = G.ConditionNumber(true);
	}


}