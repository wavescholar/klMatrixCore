#include "kl_vector.h"
#include "kl_vsl.h"
#include "kl_unit_test_wrapper.h"

#include "kl_latex_helper_fns.h"

#include <ostream>
void testAddVSL(ofstream &_tex,unsigned int &n);
void testAdd(ofstream &_tex,unsigned int &n);
void klVSLPerformaceTest()
{
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);

	ofstream _tex("klVSLPerformaceTest.tex");

	ofstream _systemText("klVSLPerformanceTest_SysInfo.txt");
	
		
	startLatexDoc("Regression of KL Software Distribution   ","KL Software Libraries",asctime(tm_buf),_tex, "");

	_tex<<"\\textbf{ KL Library test output.  This LaTex file and the associated diagrams are produced by the KL software libraries.}"<<endl;
	
	klUnitTestWrapper klutw(_tex,_systemText);
	int heapstatus = _heapchk();
	
	//for (unsigned int n = 536870912; n< 1073741824;n+=53687091)
	for (unsigned int n = 512; n< 1073741824;n+=512)
	{
		klutw.setDimension(n);
		klutw.runTest(testAddVSL);
		double vslDt = klutw.GetLastRuntime();
		klutw.runTest(testAdd);
		double stdDt = klutw.GetLastRuntime();
		assert(vslDt>0.0f);
		double speedup = stdDt / vslDt;
		cout<<"vsl speedup :"<<n<<" "<<speedup<<endl;

	}

	_tex.flush();
	
	heapstatus = _heapchk();

	_tex.close();

	_systemText.close();
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