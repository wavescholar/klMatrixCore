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
#include "kl_image_processing_functors.h"
#include "kl_img_pca.h"
#include "kl_matlab_dependent_unit_tests.h"
#include "kl_matlab_iface.h"
#include "kl_arpack.h"
#include "kl_fast_gauss_transform.h"

#include "kl_latex_helper_fns.h"

void testKLMatrix(ofstream &_tex,unsigned int  &n);
void testMatrixNorm(ofstream &_tex,unsigned int  &n);
void testKLMemory2(ofstream &_tex,unsigned int  &n);
void klTestSDPA(ofstream &_tex,unsigned int  &n);
void testklPrincipalComponents2(ofstream &_tex,unsigned int  &n);
void testArpack(ofstream &_tex,unsigned int  &n);
void generateTraceyWidomSample(ofstream &_tex,unsigned int  &n);
void testklUtil(ofstream &_tex,unsigned int  &n);
void testklMatrixMult(ofstream &_tex,unsigned int  &n );
void testklLinearRegression3x1(ofstream &_tex,unsigned int  &n);
void testExpoKit(ofstream &_tex,unsigned int  &n);

const char* basefilename="D:\\klDll\\TestDll\\";  
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


//void startLatexDoc(string title,string author,string date,ofstream &_tex,string abs);
//void makeLatexSection(string sectionTitle,ofstream &_tex);
//void endLatexDoc(ofstream &_tex);
//void LatexInsert1DPlot( klVector<double>& vec, ofstream &_tex, string dir,string filename,string title);
//void LatexInsert3DPlot( klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title);
//void LatexInsertHistogram( klVector<double>& vec, unsigned int numBins,ofstream &_tex, string dir,string filename,string title);
//void LatexInsertHeatMap(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title);
//template<class TYPE> void LatexPrintMatrix( klMatrix<TYPE>& matrix, string name,ofstream &_tex);
//template<class TYPE> void LatexPrintVector( klVector<TYPE>& klvector, string name,ofstream &_tex);
//#include <iomanip>
//template<class TYPE> void LatexPrintMatrix( klMatrix<TYPE>& matrix, string name,ofstream &_tex)
//{
//	unsigned int row = matrix.getRows();
//	unsigned int col = matrix.getColumns();
//	_tex<<"$"<<name<<" = "<<"\\left("<<endl;
//	_tex<<"\\begin{array}{"<<endl;
//	for(int i=0;i<col;i++)
//	{
//		_tex<<"c";
//	}
//	_tex<<"}"<<endl;
//	_tex.precision(3);
//	_tex.flags( ios::showpos | ios::internal | ios::fixed );
//	_tex << setw(4);
//	for(int i=0;i<row;i++)
//	{
//		for(int j=0;j<col-1;j++)
//		{
//			_tex<<matrix[i][j]<<" & ";
//		}
//		_tex<<matrix[i][col-1]<<" \\\\"<<endl;
//	}
//	_tex<<"\\end{array}"<<endl;
//	_tex<<"\\right)$"<<endl<<endl;
//	_tex.flags();
//}
//template<class TYPE> void LatexPrintVector( klVector<TYPE>& klvector, string name,ofstream &_tex)
//{
//	unsigned int col = klvector.getColumns();
//	_tex<<"$"<<name<<" = "<<"\\left("<<endl;
//	_tex<<"\\begin{array}{"<<endl;
//	for(int i=0;i<col;i++)
//	{
//		_tex<<"c";
//	}
//	_tex<<"}"<<endl;
//	_tex.precision(5);
//	_tex.flags( ios::showpos | ios::internal | ios::fixed );
//	_tex << setw(6);
//
//	for(int j=0;j<col-1;j++)
//	{
//		_tex<<klvector[j]<<" & ";
//	}
//	_tex<<klvector[col-1]<<" \\\\"<<endl;
//
//	_tex<<"\\end{array}"<<endl;
//	_tex<<"\\right)$"<<endl<<endl;
//	_tex.flags();
//
//}

//void startLatexDoc(string title,string author,string date,ofstream &_tex,string abs)
//{
//	_tex<<"\\documentclass[12pt]{article}"<<endl;
//	_tex<<"\\usepackage[english]{babel}"<<endl;
//	_tex<<"\\usepackage{amsmath,amsthm}"<<endl;
//	_tex<<"\\usepackage{amsfonts}"<<endl;
//	_tex<<"\\usepackage{graphicx}"<<endl;
//	_tex<<"\\newtheorem{thm}{Theorem}[section]"<<endl;
//	_tex<<"\\newtheorem{cor}[thm]{Corollary}"<<endl;
//	_tex<<"\\newtheorem{lem}[thm]{Lemma}"<<endl;
//	_tex<<"\\newtheorem{prop}[thm]{Proposition}"<<endl;
//	_tex<<"\\theoremstyle{definition}"<<endl;
//	_tex<<"\\newtheorem{defn}[thm]{Definition}"<<endl;
//	_tex<<"\\theoremstyle{remark}"<<endl;
//	_tex<<"\\newtheorem{rem}[thm]{Remark}"<<endl;
//	_tex<<"\\numberwithin{equation}{section}"<<endl;
//	_tex<<"\\begin{document}"<<endl;
//	_tex<<"\\title{"<<title<<"}"<<endl;
//	_tex<<"\\author{"<<author<<"}"<<endl;
//	_tex<<"\\date{"<<date<<"}"<<endl;
//	//_tex<<"\\begin{abstract}"<<endl;
//	//_tex<<abs<<endl;
//	//_tex<<"\\end{abstract}"<<endl;
//	_tex<<"\\maketitle"<<endl;
//
//}
//void makeLatexSection(string sectionTitle,ofstream &_tex)
//{
//	_tex<<"\\subsubsection{"<<sectionTitle<<"}"<<endl;
//}
//void endLatexDoc(ofstream &_tex)
//{
//	_tex<<"\\end{document}"<<endl;
//}
//void LatexInsert3DPlot(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title)
//{
//	klMatlabEngineThreadMap klmtm;
//	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
//	char* arg = new char[512];
//	char* evalString = new char[512];
//	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
//	const char* xAxis=NULL;const char* yAxis=NULL;const char* zAxis=NULL;
//	bool useExtents=true;bool holdOn=false;const char* marker=NULL;
//	klScatterPlot3D(mat,filename.c_str(),title.c_str(), xAxis, yAxis,zAxis, useExtents, holdOn, marker);
//	sprintf(evalString,"print -r1200 -depsc %s;",arg);
//	engEvalString(matlabEngine, evalString);
//	engEvalString(matlabEngine, "hold off;close(gcf);");
//	sprintf(evalString,"epstopdf   %s",arg);
//	system(evalString);
//	sprintf(arg,"%s.pdf",filename.c_str());
//	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
//	delete arg;
//	delete evalString;
//}
//void LatexInsert1DPlot(klVector<double>& vec, ofstream &_tex, string dir,string filename,string title)
//{
//	klMatlabEngineThreadMap klmtm;
//
//	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
//
//	char* arg = new char[512];
//	char* evalString = new char[512];
//	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
//	klPlot1D<double>(vec,arg,title.c_str());
//	sprintf(evalString,"print -r1200 -depsc %s;",arg);
//	engEvalString(matlabEngine, evalString);
//	engEvalString(matlabEngine, "hold off;close(gcf);");
//	sprintf(evalString,"epstopdf   %s",arg);
//	system(evalString);
//	sprintf(arg,"%s.pdf",filename.c_str());
//	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
//	delete arg;
//	delete evalString;
//}
//void LatexInsertHeatMap(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title)
//{
//	klMatlabEngineThreadMap klmtm;
//
//	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
//
//	char* arg = new char[512];
//	char* evalString = new char[512];
//	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
//	//klMatrix<TYPE>  c,const char* filename,  const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,const char* zAxis=NULL,bool useExtents=true,bool holdOn=false,const char* marker=NULL
//	kl2DPlot<double>(mat,arg,title.c_str());
//	sprintf(evalString,"print -r1200 -depsc %s;",arg);
//	engEvalString(matlabEngine, evalString);
//	engEvalString(matlabEngine, "hold off;close(gcf);");
//	sprintf(evalString,"epstopdf   %s",arg);
//	system(evalString);
//	sprintf(arg,"%s.pdf",filename.c_str());
//	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
//	delete arg;
//	delete evalString;
//}
//void LatexInsertHistogram(klVector<double>& vec, unsigned int numBins,ofstream &_tex, string dir,string filename,string title)
//{
//	vec.setupRange();
//	klVector<double> hist=vec.histogram(numBins,vec.y0,vec.y1);
//	hist.setupRange();
//	hist.setupDomain(vec.y0,vec.y1);
//	klMatlabEngineThreadMap klmtm;
//	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
//	char* arg = new char[512];
//	char* evalString = new char[512];
//	sprintf(arg,"%s\\%s.eps",dir.c_str(),filename.c_str());
//	klPlot1D<double>(hist,arg,title.c_str());
//	sprintf(evalString,"print -r1200 -depsc %s;",arg);
//	engEvalString(matlabEngine, evalString);
//	engEvalString(matlabEngine, "hold off;close(gcf);");
//	sprintf(evalString,"epstopdf   %s",arg);
//	system(evalString);
//	sprintf(arg,"%s.pdf",filename.c_str());
//	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
//	delete arg;
//	delete evalString;
//}


void unitTestMain()
{
	int heapstatus = _heapchk();
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);

	char* testRunDateTime = new char[1024];
	char* testFile = new char[1024];
	heapstatus = _heapchk();
	
	sprintf(testRunDateTime,"%d_%d_%d_%d.tex",tm_buf->tm_mon,tm_buf->tm_mday,tm_buf->tm_hour,tm_buf->tm_min);
	
	sprintf(testFile,"kl_Regression%s",testRunDateTime);
	//ofstream _tex(testFile);
	ofstream _tex("kl_Regression.tex");

	ofstream _sytemText("kl_RegressionSysInfo.txt");
	
	startLatexDoc("Regression of KL Software Distribution   ","KL Software Libraries",asctime(tm_buf),_tex, "");

	_tex<<"\\textbf{ KL Libraryt unit test ouput.  This LaTex file and the associated diagrams \
		are produced by the KL software libraries.}"<<endl;

	//FILE *stream ;
	//if((stream = freopen(testFile, "a", stdout)) == NULL)
	//	throw "kl: error redirecting std::cout to a file.";
	//_tex<<"Redirecting std::cout to this file via freopen."<<endl;

	_tex.flush();

	heapstatus = _heapchk();

	klUnitTestWrapper klutw(_tex,_sytemText);
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

	klmtm.insert(thisThread,matlabEngine);
	matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
		
	unsigned int dimension = 512;
	klutw.setDimension(dimension);
	
	makeLatexSection("Random Number Generator ",_tex);
	klutw.runTest(testKLRandomNumberGeneratorMatlab<double>);

	makeLatexSection("Matrix Exponential ",_tex);
	klutw.runTest(testExpoKit);

	makeLatexSection("Multiclass Support Vector Machine ",_tex);
	klutw.runTest(klMulticlassSVMHarnessMatlab<double>);

	makeLatexSection("ARPACK",_tex);
	klutw.runTest(testArpack);

	makeLatexSection("Semidefinite Programming SDPA",_tex);
	klutw.runTest(klTestSDPA);

	makeLatexSection("Linear Regression 3x1",_tex);
	klutw.runTest(testklLinearRegression3x1);

	makeLatexSection("Matrix Norms",_tex);
	klutw.runTest(testMatrixNorm);

	makeLatexSection("Generate Tracey Widom Sample",_tex);
	klutw.runTest(generateTraceyWidomSample);

	makeLatexSection("Principal Components Matlab ",_tex);
	klutw.runTest(testklPrincipalComponentsMatlab<double>);

	makeLatexSection("Multi Variate Random Number Generator ",_tex);
	klutw.runTest(testKLMultiVariateRandomNumberGeneratorMatlab<double>);

	makeLatexSection("Matrix Multiply",_tex);
	klutw.runTest(testklMatrixMult);

	makeLatexSection("Descriptive Statistics",_tex);
	klutw.runTest(testKLDescriptiveStatistics<double>);

	makeLatexSection("Time Series ",_tex);
	klutw.runTest(testKLTimeSeries2<double>);

	//makeLatexSection"Test Wavelet <double>",_tex);
	////HEAP[TestDll.exe]: Heap block at 0000000005B0A540 modified at 0000000005B0E584 past requested size of 4034
	//klutw.runTest(testKLWavelet<double>);

	//heapstatus = _heapchk();
	//_tex<<"Testing Memory Managers"<<endl;
	//klutw.runTest(testKLMemory2);
	//heapstatus = _heapchk();

	unsigned int n;
	n=0;
	testKLMemory2(_sytemText,n);
	testklUtil(_sytemText,n);
	klutw.HardwareConfiguration(_sytemText);


	endLatexDoc(_tex);

	_tex.close();

}



void VerifyWingerLaw(ofstream &_tex)
{
	makeLatexSection("Verfy Winger Law.",_tex);

	_tex<<"Let $M_n = [X_{ij} ]$ a symmetric n�n matrix with Random entries such "<<endl;
		  //that $X_{i,j} = X_{j,i}$, and X_{i,j} are iid $\\forall i < j,$ and $Xjj$ are iid $\\forall j \\st  \
		  //E[X^2_{ij} ] = 1, & E[X_{ij}] = 0$ and that All moments exists for each entries. \
		  //The eigenvector of this random matrix; $ \lamda_1 \\lteq ... \\lteq \lamda_n$ depends continuously on Mn."<<endl.*/
	unsigned int n=4096;

	_tex<<"Dimension $n = "<<n<<"$"<<endl<<endl;
	
	klMatrix<double> A = SampleSymmetricStandardNormalRM(n);
	
	klVector< complex<double> >  eigs = A.eigenvalues();
	
	LatexInsertHistogram(RE(eigs),100,_tex, basefilename,"Re_lambda_n","Histogram of Re(\\lambda_n)");
	LatexInsertHistogram(IM(eigs),100,_tex, basefilename,"Im_lambda_n","Histogram of Im(\\lambda_n)");

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

void testArpack(ofstream &_tex,unsigned int  &n)
{
	{
		_tex<<"Running Arnoldi Krylov algorithm on an $n=64$ dimensional $GOE$ matrix."<<endl;

		//unsigned int n= 16;
		klMatrix<double> A_GOE = SampleGOE(n);
		
		A_GOE /=n;

		unsigned int numEigenvalues= 16;

		klArpackFunctor klaf;

		LARGE_INTEGER* freq;
		_LARGE_INTEGER* prefCountStart;
		_LARGE_INTEGER* prefCountEnd;
		freq=new _LARGE_INTEGER;
		prefCountStart=new _LARGE_INTEGER;
		prefCountEnd=new _LARGE_INTEGER;
		QueryPerformanceFrequency(freq);
		QueryPerformanceCounter(prefCountStart);

		klVector<complex<double> > eigsAP = klaf.run( A_GOE,numEigenvalues);
		
		QueryPerformanceCounter(prefCountEnd);
		cout<<"TicToc ARPACK  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

		QueryPerformanceCounter(prefCountStart);
		
		klVector<complex<double> > eigs = A_GOE.eigenvalues();
		
		QueryPerformanceCounter(prefCountEnd);
		cout<<"TicToc MKL  =  "<<double(prefCountEnd->QuadPart-prefCountStart->QuadPart)/double(freq->QuadPart)<<endl;   

		LatexPrintMatrix(A_GOE, "GOE(16)" ,_tex);

		LatexPrintVector(eigsAP, "Eigenvalues via Arpack" , _tex);

		LatexPrintVector(eigs, "Eigenvalues via Intel MKL DGEES" , _tex);

	}
	_tex.flush();
}


void generateTraceyWidomSample(ofstream &_tex,unsigned int  &n)
{
		
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);

	makeLatexSection("Sample from $W_n m$ times and calculate empirical PDF of the first eig",_tex);
	_tex<<"This test of the KL libraries will generate histograms of \
		  $\\lambda_1$ for GOE (Gaussian Orthogonal Ensemble), and W (Wishart) \
		  distribution of random matrices"<<endl<<endl;
	_tex<<"This should approximate the celebrated Tracy Widom distribution."<<endl;
	//unsigned int n=64;
	unsigned int m=n*2;
	unsigned int i;
	_tex<<"Dimension $n = "<<n<<"$"<<endl<<endl;
	_tex<<"Sample size $m = "<<m<<"$"<<endl<<endl;

	klVector<complex<double> > lambda_1_Hist(m);
#pragma omp parallel num_threads(4)
	for(i=0;i<m;i++)
	{
		klMatrix<double> A(n,n);
		A=SampleWishart(n,i);
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

	LatexInsertHistogram(L_re,30,_tex, basefilename,"Re_TraceyWidom","Histogram of Re(\\lambda_1)");
	LatexInsertHistogram(L_im,30,_tex, basefilename,"Im_TraceyWidom","Histogram of Im(\\lambda_1)");
	_tex.flush();

}


#include "testmatgenunit.h"
bool testmatgen(bool silent);
klMatrix<double> real_2d_array_to_klMatrix(ap::real_2d_array a);
void testMatrixNorm(ofstream &_tex,unsigned int  &n)
{	
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);
	//startLatexDoc("Testing Operator Norm","KL Software Libraries",asctime(tm_buf),_tex,"");
	//unsigned int n= 3;

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


void testklUtil(ofstream &_tex,unsigned int  &n )
{
	//This works
	//try
	//{
	//	throw klError("Testing kl Stack Walking and Memory Dump");
	//}
	//catch(klError e)
	//{
	//	e.
	//	std::stringstream ANSI_INFO_ss (std::stringstream::in | std::stringstream::out );
	//	ANSI_INFO_ss<<"ANSI COMPILE INFO: " <<__DATE__<<"     "<<__TIME__<<"   "<<__FILE__<<"   "<<__LINE__<<"       "<<std::endl;
	//	std::string err = ANSI_INFO_ss.str();

	//}
	
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

void testklMatrixMult(ofstream &_tex,unsigned int  &n  )
{		
	{
		//unsigned int n=2;
		
		unsigned int m=n+3;
		unsigned int p=n+6;

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
		_tex<<A<<endl;

		klMatrix<double> B(m,p);
		for(i=0;i<m;i++)
		{
			for(j=0;j<p;j++)
			{
				B[i][j]=i+j;
			}
		}
		_tex<<B<<endl;

		klMatrix<double> C = A*B;

		_tex<<C<<endl;
		klMatrix<double> Cp(n,p);
		Cp[0]=1;
		Cp[1]=2;
		_tex<<Cp<<endl;
		klMatrix<double> Ap =mmBLAS(1.0, A, B, 1.0,Cp);
		_tex<<Cp<<endl;
		_tex<<Ap<<endl;

	}

	{
		unsigned int n=2;
		unsigned int m=3;
		unsigned int p=6;
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
		_tex<<A<<endl;

		klMatrix<float> B(m,p);
		for(i=0;i<m;i++)
		{
			for(j=0;j<p;j++)
			{
				B[i][j]=i+j;
			}
		}
		_tex<<B<<endl;

		klMatrix<float> C = A*B;

		_tex<<C<<endl;
		klMatrix<float> Cp(n,p);
		Cp[0]=1;
		Cp[1]=2;
		_tex<<Cp<<endl;
		klMatrix<float> Ap =mmBLAS(1.0, A, B, 1.0,Cp);
		_tex<<Cp<<endl;
		_tex<<Ap<<endl;
	}
	_tex.flush();
}

void testklLinearRegression3x1(ofstream &_tex,unsigned int  &n)
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
