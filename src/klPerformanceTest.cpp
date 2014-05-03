#include "kl_vector.h"
#include "kl_vsl.h"
#include "kl_unit_test_wrapper.h"
#include "kl_matlab_iface.h"
#include "kl_latex_helper_fns.h"

extern const char* basefilename;

#include <ostream>
void testAddVSL(ofstream &_tex,unsigned int &n);
void testAdd(ofstream &_tex,unsigned int &n);
void testFreeMemoryChecker();
void testMKLMemoryManager(ofstream &_tex,unsigned int &n);

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
	sprintf(regressionFile,"%klVSLPerformaceTest%s.tex",basefilename,testRunDateTime);
	sprintf(coutFile,"%skl_cout%s.txt",basefilename,testRunDateTime);
	sprintf(sysInfoFile,"%slVSLPerformanceTest_SysInfo%s.txt",basefilename,testRunDateTime);


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
	klutw.runTest(testMKLMemoryManager);
	
	__int64 numIters =1024;

	klVector<double> speedupsKB(numIters);
	klVector<double> GBSizeKB(numIters);

	//__int64 n = 1073741824;  
	//bool isLess =(n<16*1073741824); careful - this is false!
	//bool isLess64 = (n<17179869184); this is true

	//for (__int64 n = 1073741824; n<17179869184;n+=1073741824)
	int i=0;
	for (__int64 n = 64; n<=numIters*64;n+=64, i++)
	{
		klutw.setDimension(n);
		klutw.runTest(testAddVSL);
		double vslDt = klutw.GetLastRuntime();
		klutw.runTest(testAdd);
		double stdDt = klutw.GetLastRuntime();
		assert(vslDt>0.0f);
		double speedup = stdDt / vslDt;
		cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

		speedupsKB[i]=speedup;
		GBSizeKB[i]=n;
	}

	LatexInsert2DScatterPlot(GBSizeKB,speedupsKB,_tex,basefilename,"VSLSpeedupsKB","Speedups kb scale",false);


	numIters =4096;
	klVector<double> speedupsMB(numIters);
	klVector<double> GBSizeMB(numIters);

	i=0;
	for (__int64 n = 131072; n<=numIters*1024;n+=1024, i++)
	{
		klutw.setDimension(n);
		klutw.runTest(testAddVSL);
		double vslDt = klutw.GetLastRuntime();
		klutw.runTest(testAdd);
		double stdDt = klutw.GetLastRuntime();
		assert(vslDt>0.0f);
		double speedup = stdDt / vslDt;
		cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

		speedupsMB[i]=speedup;
		GBSizeMB[i]=n;
	}
	LatexInsert2DScatterPlot(GBSizeMB,speedupsMB,_tex,basefilename,"VSLSpeedupsMB","Speedups MB scale",false);

	numIters =64;
	klVector<double> speedupsGB(numIters);
	klVector<double> GBSizeGB(numIters);

	i=0;
	for (__int64 n = 268435456; n<=numIters*268435456;n+=268435456, i++)
	{
		klutw.setDimension(n);
		klutw.runTest(testAddVSL);
		double vslDt = klutw.GetLastRuntime();
		klutw.runTest(testAdd);
		if( i==32)
			 cout<<"32"<<endl;
		double stdDt = klutw.GetLastRuntime();
		assert(vslDt>0.0f);
		double speedup = stdDt / vslDt;
		cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

		speedupsGB[i]=speedup;
		GBSizeGB[i]=n;
	}
	LatexInsert2DScatterPlot(GBSizeGB,speedupsGB,_tex,basefilename,"VSLSpeedupsGB","Speedups GB scale",false);
	

	_tex.flush();
	
	heapstatus = _heapchk();

	_tex.close();

	_systemText.close();
}

void testFreeMemoryChecker()//(ofstream &_tex,unsigned int &n)
{
	//_tex<<"Testing the free memory checker."<<endl;
	__int64 maxAllocation=0;
	bool bigtest =klCheckFreeMemory(195290615808*195290615808);
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


void testAddVSL(ofstream &_tex,unsigned int &n)
{
	klVector<double> a(n);
	a=1;
	klVector<double> b(n);
	b=1;
	klVSLAdd(a,b,a);
}

void testAdd(ofstream &_tex,unsigned int &n)
{
	klVector<double> a(n);
	a=1;
	klVector<double> b(n);
	b=1;
	a=a+b;
}


void testMKLMemoryManager(ofstream &_tex,unsigned int &n)
{
	klMKLMemMgr*  mgr = new klMKLMemMgr();

	void* buffer =mgr->allocate(1024);
	mgr->free(buffer);

	klGlobalMemoryManager::setklVectorGlobalMemoryManager((klMemMgr*)mgr);

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

