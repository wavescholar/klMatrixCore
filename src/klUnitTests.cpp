 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
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
#include "kl_bregman_divergence.h"
#include "kl_util.h"
#include "kl_unit_tests.h"
#include "kl_matrix_facorizations.h"
#include "kl_unit_test_wrapper.h"
#include "kl_matlab_dependent_unit_tests.h"
#include "kl_matlab_iface.h"
#include "kl_arpack.h"
#include "kl_fast_gauss_transform.h"
#include "kl_latex_helper_fns.h"

const char* basefilename="D:\\klMAtrixCore\\output\\"; 

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
void MatrixOpsQuickCheck(ofstream &_tex,unsigned int  &n);
void MatrixNormDemo(ofstream &_tex,unsigned int  &n);
void MemoryManagementDemo(ofstream &_tex,unsigned int  &n);
void SemidefiniteProgrammingCheck(ofstream &_tex,unsigned int  &n);
void PrincipalComponentsDemo(ofstream &_tex,unsigned int  &n);
void IterativeKrylovCheck(ofstream &_tex,unsigned int  &n);
void GenerateTraceyWidomSample(ofstream &_tex,unsigned int  &n);
void UtilityDemo(ofstream &_tex,unsigned int  &n);
void MatrixMultiplicationCheck(ofstream &_tex,unsigned int  &n );
void LinearRegressionDemo(ofstream &_tex,unsigned int  &n);
void MatrixExponentialDemo(ofstream &_tex,unsigned int  &n);
void MutithreadedWorkflowDemo(void);
void VerifyWingerLaw(ofstream &_tex, unsigned int& n);
void GenerativeGramConsistencyCheck(ofstream &_tex,unsigned int  &n);
#include "kl_time_series.h"
#include "kl_random_number_generator.h"
void IteratedExponentialFiltering(ofstream &_tex,unsigned int &n);
void IteratedExponentialFiltering(ofstream &_tex,unsigned int &n)
{
	char* cmdString = new char[2048];
	size_t popsize=1024*2;
	klVector<double> a(popsize);
	klNormalInverseApproxRV<double> normalinv(0,0.1);
	unsigned i;
	for(i=0;i<popsize;i++)
	{
		double pi= 3.141592653589793238462643383279502;
		a[i]=normalinv()+ .5* sin(4*pi*(double(i)/popsize)) + 1* sin(7*pi*(double(i)/popsize) );
	}
	klTimeSeries<double> c(a);

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
	
	//ofstream _fileostream("klIEMA.txt");
	//for(i=0;i<popsize;i++)
	//{
	//	_fileostream<<c[i]<<", "<<ma[i]<<", "<<iema[i]<<" ,"<<ema[i]<<", "<<diff[i]<<endl;
	//}
	//_fileostream.close();	
	
	//LatexInsert1DPlot(c,_tex,basefilename,"EMA_signal","EMA Signal");

	//LatexInsert1DPlot(ma,_tex,basefilename,"MA","MA");

	//LatexInsert1DPlot(iema,_tex,basefilename,"IEMA","Iterated Exponential Moving Average");

	//LatexInsert1DPlot(ema,_tex,basefilename,"EMA","Exponential Moving Average");

	//LatexInsert1DPlot(diff,_tex,basefilename,"DIFF","Diff operator");


	LatexInsert1DPlot(c,_tex,basefilename,"EMA_signal","EMA Signal",true);

	LatexInsert1DPlot(ma,_tex,basefilename,"MA","MA",true);

	LatexInsert1DPlot(iema,_tex,basefilename,"IEMA","Iterated Exponential Moving Average",true);

	LatexInsert1DPlot(ema,_tex,basefilename,"EMA","Exponential Moving Average",true);

	LatexInsert1DPlot(diff,_tex,basefilename,"DIFF","Diff operator",false);


	//pop new data in c
	c[popsize-1024]=1237;//big shock

	//bbcsvc broken 011805 revisit - used to work  - nfg
	//double advanceEma=c.advanceEMA(popsize-1024,128,ema[popsize-1025],interp);
	//since PREVIOUS interp is used we don't see shock
	//advance again and we should start to see the effect
	//advanceEma=c.advanceEMA(popsize-1023,128,advanceEma,interp);

	//  pair<double,double> advanceIEma=c.advanceIEMA(popsize-1024,4,128,iema[popsize-1025],interp);
	//  double advanceMA=c.advanceMA(popsize-1024,4,128,interp);//bbcrevisit this call has to be consistent in second parameter as previous MA call
	//  advanceMA=c.advanceMA(popsize-1023,4,128,interp);

	_tex.flush();
	//_tex.close();
	delete cmdString;
}

void unitTestMain()
{
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
	
	//   tm_sec;     /* seconds after the minute - [0,59] */
	//   tm_min;     /* minutes after the hour - [0,59] */
	//   tm_hour;    /* hours since midnight - [0,23] */
	//   tm_mday;    /* day of the month - [1,31] */
	//   tm_mon;     /* months since January - [0,11] */
	//   tm_year;    /* years since 1900 */
	//   tm_wday;    /* days since Sunday - [0,6] */
	//   tm_yday;    /* days since January 1 - [0,365] */
	//   tm_isdst;   /* daylight savings time flag */
	//tm fields are zero based.
	sprintf(testRunDateTime,"%d_%d_%d_%d_%d",tm_buf->tm_mon+1,tm_buf->tm_mday+1,tm_buf->tm_hour+1,tm_buf->tm_min+1,tm_buf->tm_sec+1);
	
	sprintf(regressionFile,"%skl_Regression%s.tex",basefilename,testRunDateTime);
	sprintf(coutFile,"%skl_cout%s.txt",basefilename,testRunDateTime);
	sprintf(sysInfoFile,"%skl_cout%s.txt",basefilename,testRunDateTime);

	FILE *stream;

	if((stream = freopen(coutFile, "a", stdout)) == NULL)
		throw "kl: error redirecting std::cout to a file.";
	cout<<"Redirecting std::cout to"<<coutFile<<"file via freopen."<<endl;

	ofstream _tex(regressionFile);

	ofstream _sytemText(sysInfoFile);
		
	startLatexDoc("Regression of KL Software Distribution   ","KL Software Libraries",asctime(tm_buf),_tex, "");

	_tex<<"\\textbf{ KL Library test output.  This LaTex file and the associated diagrams are produced by the KL software libraries.}"<<endl;
	
	klUnitTestWrapper klutw(_tex,_sytemText);
	heapstatus = _heapchk();

	unsigned int n = 512;
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
		
	
	klTestSize= klTestType::GROW;
	unsigned int dimension;
	for(dimension =43264;dimension<131072;dimension=dimension+1024)
		IterativeKrylovCheck(_tex,dimension);

	klTestSize= klTestType::LARGE;

	makeLatexSection("Generate Tracey Widom Sample",_tex);
	klutw.runTest(GenerateTraceyWidomSample);

	makeLatexSection("Approximate Winger Distribution",_tex);
	klutw.runTest(VerifyWingerLaw);
	
	klmtm.insert(thisThread,matlabEngine);
	matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	
	makeLatexSection("Iterated Exponential Filtering ",_tex);
	klutw.runTest(IteratedExponentialFiltering);
	
	makeLatexSection("Matrix Exponential ",_tex);
	klutw.runTest(MatrixExponentialDemo);
	
	makeLatexSection("Gram Matrix Consistency Check",_tex);
	klutw.runTest(GenerativeGramConsistencyCheck);
			
	makeLatexSection("Random Number Generator ",_tex);
	klutw.runTest(testKLRandomNumberGeneratorMatlab<double>);
	
	makeLatexSection("Multiclass Support Vector Machine ",_tex);
	klutw.runTest(klMulticlassSVMHarnessMatlab<double>);

	makeLatexSection("ARPACK",_tex);
	klutw.runTest(IterativeKrylovCheck);

	makeLatexSection("Semidefinite Programming SDPA",_tex);
	klutw.runTest(SemidefiniteProgrammingCheck);

	makeLatexSection("Linear Regression 3x1",_tex);
	klutw.runTest(LinearRegressionDemo);

	makeLatexSection("Matrix Norms",_tex);
	klutw.runTest(MatrixNormDemo);

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

	makeLatexSection("Matrix",_tex);
	klutw.runTest(MatrixOpsQuickCheck<double>);
	klutw.runTest(MatrixOpsQuickCheck<float>);

	//makeLatexSection"Test Wavelet <double>",_tex);
	////HEAP[TestDll.exe]: Heap block at 0000000005B0A540 modified at 0000000005B0E584 past requested size of 4034
	//klutw.runTest(testKLWavelet<double>);

	heapstatus = _heapchk();
	
	n=0;
	MemoryManagementDemo(_sytemText,n);
	UtilityDemo(_sytemText,n);
	klutw.HardwareConfiguration(_sytemText);

	MutithreadedWorkflowDemo();
		
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

}

void VerifyWingerLaw(ofstream &_tex, unsigned int &n)
{
	makeLatexSection("Verfy Winger Law.",_tex);

	_tex<<"Let $M_n = [X_{ij} ]$ a symmetric n x n matrix with Random entries such \
		  that $X_{i,j} = X_{j,i}$, and $X_{i,j}$ are iid $\\forall i < j,$ and $Xjj$ are iid $\\forall j \\st  \
		  E[X^2_{ij} ] = 1, & E[X_{ij}] = 0$ and that all moments exists for each of the entries. \
		  The eigenvector of this random matrix; $ \\lamda_1 \\lteq ... \\lteq \\lamda_n$ depends continuously on $Mn$."<<endl;
	
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

//BBCTODO The output needs to be put in the Latex output.  Convergence data is in the cout stream redirect file.
void IterativeKrylovCheck(ofstream &_tex,unsigned int  &n)
{
	if (klTestSize!=klTestType::GROW)
	{
		n=128;
	}
	
	_tex<<"Running Arnoldi Krylov algorithm on $GOE$ matrix and comparing to Intel MKL."<<endl;


	klArpackFunctor klaf;

	LARGE_INTEGER* freq;
	_LARGE_INTEGER* prefCountStart;
	_LARGE_INTEGER* prefCountEnd;
	freq=new _LARGE_INTEGER;
	prefCountStart=new _LARGE_INTEGER;
	prefCountEnd=new _LARGE_INTEGER;
	QueryPerformanceFrequency(freq);

	QueryPerformanceCounter(prefCountStart);
	klMatrix<double> A_GOE = SampleGOE(n);
	QueryPerformanceCounter(prefCountEnd);

	cerr<<"SampleGOE(n)="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	_tex<<"SampleGOE(n)="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   
	
	A_GOE /=n;

	unsigned int numEigenvalues= 16;
	
	QueryPerformanceCounter(prefCountStart);

	klVector<complex<double> > eigsAP = klaf.run( A_GOE,numEigenvalues);

	QueryPerformanceCounter(prefCountEnd);
	cout<<"TicToc ARPACK  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

	cerr<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;
	_tex<<"Iterative Krylov dim="<<n<<" dt="<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;
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

void GenerateTraceyWidomSample(ofstream &_tex,unsigned int  &n)
{
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
		
		ofstream fileostreamobj("TraceyWidom_re.txt" );
		fileostreamobj<<L_re<<endl;
		fileostreamobj.close();

		fileostreamobj.open("TraceyWidom_im.txt" );
		fileostreamobj<<L_im<<endl;
		fileostreamobj.close();
		 
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
		
		ofstream fileostreamobj("Winger_re.txt" );
		fileostreamobj<<L_re<<endl;
		fileostreamobj.close();

		fileostreamobj.open("Winger_im.txt" );
		fileostreamobj<<L_im<<endl;
		fileostreamobj.close();

		LatexInsertHistogram(L_re,30,_tex, basefilename,"Re_Winger","Histogram of Re(\\lambda_1) A \\in GOE(1024)");
		LatexInsertHistogram(L_im,30,_tex, basefilename,"Im_Winger","Histogram of Im(\\lambda_1) A \\in GOE(1024) ");
		_tex.flush();
	}



}

#include "testmatgenunit.h"
bool testmatgen(bool silent);
klMatrix<double> real_2d_array_to_klMatrix(ap::real_2d_array a);
void MatrixNormDemo(ofstream &_tex,unsigned int  &n)
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

	_tex<<"$L_1$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.getConditionNumber(1)<<endl<<endl;
	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$L_{\\infty}$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.getConditionNumber()<<endl<<endl;

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
	_tex<<"$L_1$ condition number of Wishart Matrix "<<AW.getConditionNumber(true)<<endl;
	_tex<<"$L_\infty$ condition number of Wishart Matrix "<<AW.getConditionNumber()<<endl;

	makeLatexSection("Gaussian Orthogonal Ensemble $A \\in GOE(n)$",_tex);

	klMatrix<double> A_GOE = SampleGOE(n);
	klMatrix<double> Ainv=A_GOE.inverse();
	klMatrix<double> Id_goe=Ainv * A_GOE;
	Id_goe.threshold(0.001f,+0.0f);
	Id_goe = Id_goe + 0.0;
	klVector<double> x=Id_goe[0]; 
	klVector<double> y=Id_goe[2];

	_tex<<"$L_1$ condition number of GOE Matrix "<<A_GOE.getConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of GOE Matrix "<<A_GOE.getConditionNumber(true)<<endl;

	makeLatexSection("The Identity Matrix $I \\in M(n)$",_tex);

	klMatrix<double> Id(n,n);
	Id= IdentityMatrix<double>(n);
	_tex<<"$L_1$ condition number of $I$ = "<<Id.getConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of $I$ = "<<Id.getConditionNumber()<<endl;

	_tex.flush();
	
}

void UtilityDemo(ofstream &_tex,unsigned int  &n )
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

void MatrixMultiplicationCheck(ofstream &_tex,unsigned int  &n  )
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

void LinearRegressionDemo(ofstream &_tex,unsigned int  &n)
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

void GenerativeGramConsistencyCheck(ofstream &_tex,unsigned int  &n)
{
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

	LatexPrintMatrix<double>(SigmaW,"W",_tex);

	klPrincipalComponents<double> pcaWishart=SigmaW;
	klMatrix<double> V=pcaWishart(2);
	klVector<double> wishartEigs=pcaWishart.eigenvalues();
	LatexPrintVector<double>(wishartEigs,"W eigs",_tex);

	unsigned int i;
	unsigned int j;
	klVector<double> meanVector(numFeatures);
	meanVector=1;
	
	klNormalMultiVariate<double> T(meanVector,SigmaW );
	klSamplePopulation<double> X(sampleSize,numFeatures);
	for(j=0;j<sampleSize;j++)
	{
		klVector<double> tv=T();
		X.setRow(j,tv);
	}

	klMatrix<double> SampleCovariance = X.covarianceMatrix();

	LatexPrintMatrix<double>(SampleCovariance,"Sample Covariance",_tex);
	
	LatexPrintVector<double>(X.sampleMean(),"Sample Mean",_tex);

	//bbcreivist Git ISSUE #10 ( Makes extra Copy ?)
	klMatrix<double> D  = SampleCovariance-SigmaW; 
	LatexPrintMatrix<double>(D,"SampleCovariance-W",_tex);
	
	klPrincipalComponents<double> pcaCovariance=SampleCovariance;
	klMatrix<double> VC =pcaCovariance(2);
	klVector<double> covarianceEigs=pcaCovariance.eigenvalues();
	
	LatexPrintVector<double>(covarianceEigs,"Sample Covariance Eigs",_tex);

	//How close is the sample covariance to a PSD matrix?
	//First we'll need a measure of matrix closeness and a way to find the 
	//nearest PSD matrix in that measure. 

	//The Gram matrix G of a set of vectors x_1, ...,x_i,...x_n in an inner product space is the Hermitian matrix of inner products, whose entries are given by G_{ij} = <x_i,x_j>
	//An important application is to compute linear independence: a set of vectors is linearly independent if and only if the determinant of the Gram matrix is non-zero.

	klMatrix<double> G(sampleSize,sampleSize);
	G.makeNanFriendly();
	for(i=0;i<sampleSize;i++)
	{
		klVector<double> x_i= X[i];
		for(j=0;j<sampleSize;j++)
		{
			klVector<double> x_j= X[j];
			G[i][j]=(x_i).dotBLAS(x_j);
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
		klVector<double> x_i= centered.getColumn(i);
		for(j=0;j<numFeatures;j++)
		{
			klVector<double> x_j= centered.getColumn(j);
			Gf[i][j]=(x_i).dotBLAS(x_j);
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
	klMatrix<double> VCGram =pcaGram(2);

	LatexPrintMatrix<double>(VCGram ,"EigenDecomp of Gram Matrix",_tex);

	}

