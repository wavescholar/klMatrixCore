#include "kl_vector.h"
#include "kl_vsl.h"
#include "kl_unit_test_wrapper.h"
#include "kl_matlab_iface.h"
#include "kl_latex_helper_fns.h"
#include <ostream>
#include <sstream>
extern const char* basefilename;

void testAddVSL(ofstream &_tex,__int64 &n);
void testAdd(ofstream &_tex,__int64 &n);
void testFreeMemoryChecker();
void testMKLMemoryManager(ofstream &_tex,__int64 &n);
double vslBenchmark(__int64 &n);

void klPerformanceTesting()
{
	testFreeMemoryChecker();

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
	sprintf(regressionFile,"%sklVSLPerformaceTest%s.tex",basefilename,testRunDateTime);
	sprintf(coutFile,"%skl_cout%s.txt",basefilename,testRunDateTime);
	sprintf(sysInfoFile,"%sklVSLPerformanceTest_SysInfo%s.txt",basefilename,testRunDateTime);


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

	ofstream _tex(regressionFile);

	ofstream _systemText(sysInfoFile);	

	klUnitTestWrapper klutw(_tex,_systemText);

	klutw.setDimension(0);
	//klutw.runTest(testMKLMemoryManager);

	__int64 n = 1073741824;  
	bool isLess =(n<16*1073741824); //careful - this is false!
	bool isLess64 = (n<17179869184); //this is true
	bool isLessLitDec = (n<16*1073741824LL);
	//for (__int64 n = 1073741824; n<17179869184;n+=1073741824)

	_tex<<"Benchmark VSL add with klMKLMemMgr "<<endl;

	bool useIntelMemMgr = true;
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

	int i=0;	
	
	__int64 numIters =1024;
	//klVector<double> speedupsKB(numIters);
	//klVector<double> GBSizeKB(numIters);
	//for (__int64 n = 64; n<=numIters*64;n+=64, i++)
	//{
	//	klutw.setDimension(n);
	//	klutw.runTest(testAddVSL);
	//	double vslDt = klutw.GetLastRuntime();
	//	klutw.runTest(testAdd);
	//	double stdDt = klutw.GetLastRuntime();
	//	assert(vslDt>0.0f);
	//	double speedup = stdDt / vslDt;
	//	cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

	//	speedupsKB[i]=speedup;
	//	GBSizeKB[i]=n;
	//}

	stringstream fileName;
	//fileName<<"VSLAdd_Versuts_Std_SpeedupsKB_MKL_MemMgr_"<<useIntelMemMgr;

	stringstream title;
	//title<<"Speedups VSL Add Versus Standard Add at KB scale : Intel MemMgr = "<<useIntelMemMgr;
	//LatexInsert2DScatterPlot(GBSizeKB,speedupsKB,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),false);

	//numIters =4096;
	//klVector<double> speedupsMB(numIters);
	//klVector<double> GBSizeMB(numIters);

	//i=0;
	//for (__int64 n = 1024; n<=numIters*1024;n+=1024, i++)
	//{
	//	klutw.setDimension(n);
	//	klutw.runTest(testAddVSL);
	//	double vslDt = klutw.GetLastRuntime();
	//	klutw.runTest(testAdd);
	//	double stdDt = klutw.GetLastRuntime();
	//	assert(vslDt>0.0f);
	//	double speedup = stdDt / vslDt;
	//	cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

	//	speedupsMB[i]=speedup;
	//	GBSizeMB[i]=n;
	//}
	//fileName.str("");
	//fileName.clear();
	//fileName<<"VSLAdd_Versuts_Std_Speedups_MB_MKL_MemMgr_"<<useIntelMemMgr;

	//title.str("");
	//title.clear();
	//title<<"Speedups VSL Add Versus Standard Add at MB scale : Intel MemMgr = "<<useIntelMemMgr;

	//LatexInsert2DScatterPlot(GBSizeMB,speedupsMB,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),false);

	////-----------------------------------------------------------------------------------------------------------------
	//_tex<<"Benchmark VSL add without klMKLMemMgr "<<endl;

	//useIntelMemMgr = false;
	//if(useIntelMemMgr)
	//{
	//	klMKLMemMgr*  mgr = new klMKLMemMgr();
	//	klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
	//}
	//else
	//{
	//	klMemMgr*  mgr = NULL;
	//	klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
	//}

	//numIters =1024;
	//i=0;
	//for (__int64 n = 64; n<=numIters*64;n+=64, i++)
	//{
	//	klutw.setDimension(n);
	//	klutw.runTest(testAddVSL);
	//	double vslDt = klutw.GetLastRuntime();
	//	klutw.runTest(testAdd);
	//	double stdDt = klutw.GetLastRuntime();
	//	assert(vslDt>0.0f);
	//	double speedup = stdDt / vslDt;
	//	cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

	//	speedupsKB[i]=speedup;
	//	GBSizeKB[i]=n;
	//}

	//fileName.str("");
	//fileName.clear();
	//fileName<<"VSLAdd_Versuts_Std_SpeedupsKB_MKL_MemMgr_"<<useIntelMemMgr;

	//title.str("");
	//title.clear();
	//title<<"Speedups VSL Add Versus Standard Add at KB scale : Intel MemMgr = "<<useIntelMemMgr;
	//LatexInsert2DScatterPlot(GBSizeKB,speedupsKB,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),false);

	//numIters =4096;
	//i=0;
	//for (__int64 n = 1024; n<=numIters*1024;n+=1024, i++)
	//{
	//	klutw.setDimension(n);
	//	klutw.runTest(testAddVSL);
	//	double vslDt = klutw.GetLastRuntime();
	//	klutw.runTest(testAdd);
	//	double stdDt = klutw.GetLastRuntime();
	//	assert(vslDt>0.0f);
	//	double speedup = stdDt / vslDt;
	//	cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

	//	speedupsMB[i]=speedup;
	//	GBSizeMB[i]=n;
	//}

	//fileName.str("");
	//fileName.clear();
	//fileName<<"VSLAdd_Versuts_Std_Speedups_MB_MKL_MemMgr_"<<useIntelMemMgr;

	//title.str("");
	//title.clear();
	//title<<"Speedups VSL Add Versus Standard Add at MB scale : Intel MemMgr = "<<useIntelMemMgr;

	//LatexInsert2DScatterPlot(GBSizeMB,speedupsMB,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),false);

	//--------------------------------------------------------------------------------------------------
	
	_tex<<"benchmark all the VSL functions testing the effect of memory aligned buffers"<<endl;

	numIters =8192;
	
	klVector<double> vslFnspeedupsKBScale(numIters);
	klVector<double> vslSizeKBScale(numIters);
	i=0;
	for (__int64 n = 1; n<=numIters;n+=1, i++)
	{
		klutw.setDimension(n);
		{
			klMemMgr*  mgr = NULL;
			klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
		}
		
		klutw.setDimension(n);
		//double vslDt = klutw.runBenchMark(vslBenchmark);
		double vslDt = vslBenchmark(n);
		
		{
			klMKLMemMgr*  mgr = new klMKLMemMgr();
			klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
		}
		
		double stdDt =vslBenchmark(n);
		//double stdDt =klutw.runBenchMark(vslBenchmark);
		
		assert(vslDt>0.0f);
		double speedup = vslDt/stdDt  ;
		cout<<"vsl functions speedup :"<<n<<" "<<speedup<<endl;

		vslFnspeedupsKBScale[i]=speedup;
		vslSizeKBScale[i]=n;
	}

	fileName.str("");
	fileName.clear();
	fileName<<"VSLSpeedupsForMemoryAlignedBufers_SmallScale"<<useIntelMemMgr;

	title.str("");
	title.clear();
	title<<"VSL Speedups using MemoryAligned Buffers for size < 8kn";
	LatexInsert2DScatterPlot(vslSizeKBScale,vslFnspeedupsKBScale,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),false);


	numIters =4096;
	klVector<double> vslFnspeedupsMBScale(numIters);
	klVector<double> vslSizeMBScale(numIters);

	i=0;
	for (__int64 n = 1024; n<=numIters*1024;n+=1024, i++)
	{
		klutw.setDimension(n);
		{
			klMemMgr*  mgr = NULL;
			klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
		}
		klutw.setDimension(n);
		//double vslDt = klutw.runBenchMark(vslBenchmark);
		double vslDt = vslBenchmark(n);
		
		{
			klMKLMemMgr*  mgr = new klMKLMemMgr();
			klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);
		}
		
		double stdDt =vslBenchmark(n);
		//double stdDt =klutw.runBenchMark(vslBenchmark);

		assert(vslDt>0.0f);
		double speedup = vslDt/stdDt;
		cout<<"vsl functions speedup :"<<n<<" "<<speedup<<endl;

		vslFnspeedupsMBScale[i]=speedup;
		vslSizeMBScale[i]=n;
	}

	fileName.str("");
	fileName.clear();
	fileName<<"VSLSpeedupsForMemoryAlignedBufers_MediumScale"<<useIntelMemMgr;

	title.str("");
	title.clear();
	title<<"VSL Speedups using MemoryAligned Buffers at MB scale";

	LatexInsert2DScatterPlot(vslSizeMBScale,vslFnspeedupsMBScale,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),false);

	//----------------------------------------------------------------------------------------------------------------

	_tex.flush();

	heapstatus = _heapchk();

	_tex.close();

	_systemText.close();
}

void testFreeMemoryChecker()//(ofstream &_tex,unsigned int &n)
{
	//_tex<<"Testing the free memory checker."<<endl;
	__int64 maxAllocation=0;
	bool bigtest =klCheckFreeMemory(195290615808LL*195290615808LL);
	for(__int64 n=4294967296;n<195290615808*512;n+=4294967296)
	{
		cout<<n<<endl;
		bool okToAllocate = klCheckFreeMemory(n);
		if (!okToAllocate)
		{
			maxAllocation = n-4294967296;
			break;
		}
	}
	//_tex<<"Max Allocation : "<<maxAllocation<<endl;
	cout<<"Max Allocation : "<<maxAllocation<<endl;
}

void testAddVSL(ofstream &_tex,__int64 &n)
{
	klVector<double> a(n);
	a=1;
	klVector<double> b(n);
	b=1;
	klVSLAdd(a,b,a);
}

void testAdd(ofstream &_tex,__int64 &n)
{
	klVector<double> a(n);
	a=1;
	klVector<double> b(n);
	b=1;
	a=a+b;
}

void testMKLMemoryManager(ofstream &_tex,__int64 &n)
{
	klMKLMemMgr*  mgr = new klMKLMemMgr();
	klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);

	try
	{
		void* buffer =mgr->allocate(1024);
		mgr->free(buffer);
	}
	catch(...)
	{
		cerr<<"Problem allocating emmory with Intel MKL Memory Manager";
	}

	int numAllocs = 128;

	for(__int64 i=1048576;i<1048576 *numAllocs;i+=1048576)
	{
		klVector<int> kli(i); kli=1;

		klVector<double> kld(i);kld=3.14159l;

		klVector<float> klf(i);klf=3.14159f;

		klVector<__int64> kli64(i);kli64=4294967296LL;

		//size_t peakUse =mgr->getPeakMemoryUse();

		//cout<<peakUSe<<endl;
	}
}

double vslBenchmark(__int64 &n)
{
	klTimer klt;
	klt.tic();

	////klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b);
	//std::cout<<"Test : klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAdd( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAdd "<<endl;
	//}
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAdd( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAdd "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),0.0f);		
	//	klVSLAdd( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAdd inplace"<<endl;
	//}
	////klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b)
	//std::cout<<"Test : klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLSub( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSub "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),0.0f);		
	//	klVSLSub( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSub inplace"<<endl;
	//}
	////klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b)
	//std::cout<<"Test : klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLMul( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLMul "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),0.0f);		
	//	klVSLMul( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLMul inplace"<<endl;
	//}
	////klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b)
	//std::cout<<"Test : klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLMulByConj( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLMulByConj "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLMulByConj( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLMulByConj inplace"<<endl;
	//}
	////klVector<complex<double > > klVSLConj(klVector< complex<double > > v)
	//std::cout<<"Test : klVector<complex<double > > klVSLConj(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLConj( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLConj "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLConj( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLConj inplace"<<endl;
	//}
	//////vzAbs	Computation of the absolute value of vector elements
	////double klVSLAbs(klVector< complex<double > > v );
	//std::cout<<"Test : double klVSLAbs(klVector< complex<double > > v ) ;"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<double> ans(n);
	//	klVSLAbs( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAbs"<<endl;
	//}	
	//////vzArg	Computation of the argument of vector elements
	////double  klVSLArg(klVector< complex<double > > v);
	//std::cout<<"Test : double  klVSLArg(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<double> ans(n);
	//	klVSLArg( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLArg"<<endl;
	//}	
	//////vzDiv	Division of elements of one vector by elements of the second vector
	////klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b);
	//std::cout<<"Test : klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLDiv( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLDiv "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),0.0f);		
	//	klVSLDiv( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLDiv inplace"<<endl;
	//}
	//////vzSqrt	Computation of the square root of vector elements
	////klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLSqrt( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSqrt "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);
	//	klVSLSqrt( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSqrt inplace"<<endl;
	//}
	//////vzPow	Raising each vector element to the specified power
	////klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b);
	//std::cout<<"Test :klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b);;"<<endl;
	//try
	//{
	//	klVector<complex <double > > v(n);
	//	v=  complex<double>(2.7183f,1.0f);
	//	klVector<complex <double > > b(n);
	//	b= complex< double>(2.0f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLPow( v,b,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPow "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex <double > > v(n);
	//	v=  complex<double>(std::pow( 2.7183,2),1.0f);	
	//	klVector<complex<double > > b(n);
	//	klVSLPow( v,b, v);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPow inplace"<<endl;
	//}
	//////vzPowx	Raising each vector element to the constant power
	////klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x );
	//std::cout<<"Test : klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x );"<<endl;
	//try
	//{
	//	klVector<complex <double > > v(n);
	//	v=  complex<double>(2.7183f,1.0f);
	//	complex<double> b = complex<double>(2.0f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLPowX( v,b,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPowX "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > v(n);
	//	v= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	complex<double> b =complex<double>(2.0f,0.0f);
	//	klVSLPowX(v, b, v);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPowX inplace"<<endl;
	//}
	//////vzExp	Computation of the exponential of vector elements
	////klVector<complex<double > > klVSLExp(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLExp(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex< double> > ans(n);
	//	klVSLExp( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLExp "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLExp( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLExp inplace"<<endl;
	//}
	//////vzLn	Computation of the natural logarithm of vector elements
	////klVector<complex<double > > klVSLLn(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLLn(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLLn( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLn "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLLn( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLn inplace"<<endl;
	//}
	//////vzLog10	Computation of the denary logarithm of vector elements
	////klVector<complex<double > > klVSLLog10(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLLog10(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(2.7183f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLLog10( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLog10 "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLLog10( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLog10 inplace"<<endl;
	//}
	//////vzCos	Computation of the cosine of vector elements
	////klVector<complex<double > > klVSLCos(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLCos(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,0.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLCos( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCos "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLCos( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCos inplace"<<endl;
	//}
	////vzSin	Computation of the sine of vector elements
	//klVector<complex<double > > klVSLSin(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLSin(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(n);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(n);
		klVSLSin( a,ans);
	}
	catch(std::exception e)
	{
		std::cerr<<e.what();
	}
	catch(...)
	{
		std::cerr<<"Error with klVSLSin "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(n);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVSLSin( b, b);
	}
	catch(std::exception e)
	{
		std::cerr<<e.what();
	}
	catch(...)
	{
		std::cerr<<"Error with klVSLSin inplace"<<endl;
	}
	//////vzTan	Computation of the tangent of vector elements
	////klVector<complex<double > > klVSLTan(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLTan(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLSin( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTan "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLTan( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTan inplace"<<endl;
	//}
	//////vzAcos	Computation of the inverse cosine of vector elements
	////klVector<complex<double > > klVSLAcos(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLAcos(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAcos( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcos "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLAcos( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcos inplace"<<endl;
	//}
	//////vzAsin	Computation of the inverse sine of vector elements
	////klVector<complex<double > > klVSLAsin(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLAsin(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAsin( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsin "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLAsin( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsin inplace"<<endl;
	//}
	//////vzAtan	Computation of the inverse tangent of vector elements
	////klVector<complex<double > > klVSLAtan(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLAtan(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAtan( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtan "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLAtan( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtan inplace"<<endl;
	//}
	//////vzCosh	Computation of the hyperbolic cosine of vector elements
	////klVector<complex<double > > klVSLCosh(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLCosh(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLCosh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCosh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLCosh( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCosh inplace"<<endl;
	//}
	//////vzSinh	Computation of the hyperbolic sine of vector elements
	////klVector<complex<double > > klVSLSinh(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLSinh(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLSinh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSinh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLSinh( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSinh inplace"<<endl;
	//}
	//////vzTanh	Computation of the hyperbolic tangent of vector elements
	////klVector<complex<double > > klVSLTanh(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLTanh(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLTanh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTanh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLTanh( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTanh inplace"<<endl;
	//}
	//////vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
	////klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAcosh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcosh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLAcosh( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcosh inplace"<<endl;
	//}
	//////vzAsinh	Computation of the inverse hyperbolic sine of vector elements
	////klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAsinh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsinh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLAsinh( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsinh inplace"<<endl;
	//}
	//////vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
	////klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v);
	//std::cout<<"Test : klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v);"<<endl;
	//try
	//{
	//	klVector<complex <double > > a(n);
	//	a=  complex<double>(3.14159f,1.0f);
	//	klVector<complex<double> > ans(n);
	//	klVSLAtanh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtanh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<complex<double > > b(n);
	//	b= complex<double>(std::pow( 2.7183,2),1.0f);		
	//	klVSLAtanh( b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtanh inplace"<<endl;
	//}
	////vdAdd	Addition of vector elements
	////klVector<double > klVSLAdd(klVector<double  > v,klVector<double > b=false);
	//std::cout<<"Test : klVector<double  > klVSLAdd(klVector<<double >  v,klVector<double>  b);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);klVSLAdd( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAdd "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAdd( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAdd inplace"<<endl;
	//}
	////vdSub		Subtraction of vector elements
	////klVector<double > klVSLSub(klVector<double  > v,klVector<double > b);
	//std::cout<<"Test : klVector<double  > klVSLSub(klVector<<double >  v,klVector<double>  b);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);klVSLSub( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSub "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLSub( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSub inplace"<<endl;
	//}
	////vdMul		Multiplication of vector elements
	////klVector<double > klVSLMul(klVector<double  > v,klVector<double > b);
	//std::cout<<"Test : klVector<double  > klVSLMul(klVector<<double >  v,klVector<double>  b);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLMul( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLMul "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLMul( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLMul inplace"<<endl;
	//}
	////vdDiv		Division of elements of one vector by elements of the second vector
	////klVector<double > klVSLDiv(klVector<double  > v,klVector<double > b);
	//std::cout<<"Test : klVector<double  > klVSLDiv(klVector<<double >  v,klVector<double>  b);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLDiv( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLDiv "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLDiv( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLDiv inplace"<<endl;
	//}
	////vdHypot		Computation of the square root of sum of squares
	////klVector<double > klVSLHypot(klVector<double  > v,klVector<double  > b);
	//std::cout<<"Test : klVector<double  > klVSLHypot(klVector<<double >  v,klVector<double>  b);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLHypot( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLHypot "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLHypot( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLHypot inplace"<<endl;
	//}
	////vdPow		Raising each vector element to the specified power
	////klVector<double > klVSLPow(klVector<double  > v,klVector<double > b);
	//std::cout<<"Test : klVector<double  > klVSLPow(klVector<<double >  v,klVector<double>  b);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLPow( a,a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPow "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLPow( b,b, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPow inplace"<<endl;
	//}
	////vdPowx		Raising each vector element to the constant power
	////klVector<double > klVSLPowx(klVector<double  > v,double x);
	//std::cout<<"Test : klVector<double  > klVSLPowx(klVector<<double >  v,double> x);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	double x=2.0f;
	//	klVector< double> ans(n);
	//	klVSLPowx( a,x,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPowx "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));	
	//	double x=2.0f;
	//	klVSLPowx( b,x, b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLPowx inplace"<<endl;
	//}
	////vdSqr		Squaring of vector elements
	////klVector<double > klVSLSqr(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLSqr(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLSqr( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSL "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLSqr( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSqr inplace"<<endl;
	//}
	////vdAbs		Computation of the absolute value of vector elements
	////klVector<double > klVSLAbs(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAbs(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLAbs( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAbs "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAbs( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAbs inplace"<<endl;
	//}
	////vdInv		Inversion of vector elements
	////klVector<double > klVSLInv(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLInv(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLInv( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLInv "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLInv( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLInv inplace"<<endl;
	//}
	////vdSqrt Computation of the square root of vector elements
	////klVector<double > klVSLSqrt(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLSqrt(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLSqrt( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSqrt "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLSqrt( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSqrt inplace"<<endl;
	//}
	////vdExp	Computation of the exponential of vector elements
	////klVector<double > klVSLExp(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLExp(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLExp( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLExp "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLExp( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLExp inplace"<<endl;
	//}
	////vdExpm1		Computation of the exponential of vector elements decreased by 1
	////klVector<double > klVSLExpm1(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLExpm1(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLExpm1( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLExpm1 "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLExpm1( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLExpm1 inplace"<<endl;
	//}
	////vdLn	Computation of the natural logarithm of vector elements
	////klVector<double > klVSLLn(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLLn(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLLn( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLn "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLLn( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLn inplace"<<endl;
	//}
	////vdLog10		Computation of the denary logarithm of vector elements
	////klVector<double > klVSLLog10(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLLog10(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLLog10( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLog10 "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLLog10( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLog10 inplace"<<endl;
	//}
	////vdCos		Computation of the cosine of vector elements
	////klVector<double > klVSLCos(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLCos(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLCos( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCos "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLCos( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCos inplace"<<endl;
	//}
	////vdSin		Computation of the sine of vector elements
	////klVector<double > klVSLSin(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLSin(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLSin( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSin "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLSin( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSin inplace"<<endl;
	//}
	////vdTan		Computation of the tangent of vector elements
	////klVector<double > klVSLTan(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLTan(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLTan( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTan "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLTan( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTan inplace"<<endl;
	//}
	////vdAcos		Computation of the inverse cosine of vector elements
	////klVector<double > klVSLAcos(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAcos(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLAcos( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcos "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAcos( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcos inplace"<<endl;
	//}
	////vdAsin		Computation of the inverse sine of vector elements
	////klVector<double > klVSLAsin(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAsin(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLAsin( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsin "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAsin( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsin inplace"<<endl;
	//}
	////vdAtan		Computation of the inverse tangent of vector elements
	////klVector<double > klVSLAtan(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAtan(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLAtan( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtan "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAtan( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtan inplace"<<endl;
	//}
	////vdCosh		Computation of the hyperbolic cosine of vector elements
	////klVector<double > klVSLCosh(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLCosh(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLCosh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCosh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLCosh( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCosh inplace"<<endl;
	//}
	////vdSinh		Computation of the hyperbolic sine of vector elements
	////klVector<double > klVSLSqrtSinh(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLSinh(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLSinh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSinh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLSinh( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLSinh inplace"<<endl;
	//}
	////vdTanh		Computation of the hyperbolic tangent of vector elements
	////klVector<double > klVSLTanh(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLTanh(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLTanh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTanh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLTanh( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTanh inplace"<<endl;
	//}
	////vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
	////klVector<double > klVSLAcosh(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAcosh(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLAcosh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSAcoshL "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAcosh( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAcosh inplace"<<endl;
	//}
	////vdAsinh		Computation of the inverse hyperbolic sine of vector elements
	////klVector<double > klVSLAsinh(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAsinh(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);klVSLAsinh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsinh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAsinh( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAsinh inplace"<<endl;
	//}
	////vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
	////klVector<double > klVSLAtanh(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLAtanh(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLAtanh( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtanh "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLAtanh( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLAtanh inplace"<<endl;
	//}
	////vdErf		Computation of the error function value of vector elements
	////klVector<double > klVSLErf(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLErf	(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLErf	( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErf	 "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLErf	( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErf	 inplace"<<endl;
	//}
	////vdErfc		Computation of the complementary error function value of vector elements
	////klVector<double > klVSLErfc(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLErfc(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLErfc( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErfc "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLErfc( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErfc inplace"<<endl;
	//}
	////vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
	////klVector<double > klVSLCdfNorm(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLCdfNorm(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLCdfNorm( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCdfNorm "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLCdfNorm( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCdfNorm inplace"<<endl;
	//}
	////vdErfInv		Computation of the inverse error function value of vector elements
	////klVector<double > klVSLErfInv(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLErfInv(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLErfInv( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErfInv "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLErfInv( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErfInv inplace"<<endl;
	//}
	////vdErfcInv		Computation of the inverse complementary error function value of vector elements
	////klVector<double > klVSLErfcInv(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLErfcInv(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLErfcInv( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErfcInv "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLErfcInv( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLErfcInv inplace"<<endl;
	//}
	////vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
	////klVector<double > klVSLCdfNormInv(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLCdfNormInv(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLCdfNormInv( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCdfNormInv "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLCdfNormInv( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCdfNormInv inplace"<<endl;
	//}
	////vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
	////klVector<double > klVSLLGamma(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLLGamma(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLLGamma( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLGamma "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLLGamma( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLLGamma inplace"<<endl;
	//}
	////vdTGamma		Computation of the gamma function of vector elements 
	////klVector<double > klVSLTGamma(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLTGamma(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLTGamma( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTGamma "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLTGamma( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTGamma inplace"<<endl;
	//}
	////vdFloor		Rounding towards minus infinity
	////klVector<double > klVSLFloor(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLFloor(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLFloor( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLFloor "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLFloor( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLFloor inplace"<<endl;
	//}
	////vdCeil		Rounding towards plus infinity
	////klVector<double > klVSLCeil(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLCeil(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLCeil( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCeil "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLCeil( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLCeil inplace"<<endl;
	//}
	////vdTrunc		Rounding towards zero infinity
	////klVector<double > klVSLTrunc(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLTrunc(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLTrunc( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTrunc "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLTrunc( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLTrunc inplace"<<endl;
	//}
	////vdRound		Rounding to nearest integer
	////klVector<double > klVSLRound(klVector<double  > v);
	//std::cout<<"Test : klVector<double  > klVSLRound(klVector<<double >  v);"<<endl;
	//try
	//{
	//	klVector <double  > a(n);
	//	a=  double(2.7183f);
	//	klVector< double> ans(n);
	//	klVSLRound( a,ans);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLRound "<<endl;
	//}
	////try in place 
	//try
	//{
	//	klVector<double> b(n);
	//	b= double(std::pow( 2.7183,2));		
	//	klVSLRound( b,b);
	//}
	//catch(std::exception e)
	//{
	//	std::cerr<<e.what();
	//}
	//catch(...)
	//{
	//	std::cerr<<"Error with klVSLRound inplace"<<endl;
	//}

	return klt.toc();	
}



