#include "kl_matrix.h" 
#include "kl_vsl.h"

#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE "C++ Unit Tests for kl Matrix Library"
#include <boost/test/included/unit_test.hpp>
//#include <boost/test/unit_test.hpp>

#ifdef _DEBUG
#define  KL_BOOST_TEST_VECTOR_SIZE 16
#endif

#ifndef _DEBUG
//#define  KL_BOOST_TEST_VECTOR_SIZE 4294967296
#define  KL_BOOST_TEST_VECTOR_SIZE 536870912
//#define  KL_BOOST_TEST_VECTOR_SIZE268435456
// 134217728
#endif

BOOST_AUTO_TEST_SUITE(kl_boost_test_complex_vsl_functions)
 
/* 
     This sourec module contains the unit tests for the kl wrappers to the Intel MKL
	 VSL functions. There are two test suites, one for complex and one for double precision
	 operations.  Both in place and copy operations are tested. 
*/
BOOST_AUTO_TEST_CASE(TestBoostUTF)
{
	//boost::unit_test::progress_monitor.set_stream( cout );

    BOOST_CHECK(1 == 4);
	BOOST_REQUIRE(1>2);
	BOOST_WARN(2.31<2.30);
	BOOST_REQUIRE(1<0);
	BOOST_TEST_MESSAGE("Test Mesaage");

}
//klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b);
BOOST_AUTO_TEST_CASE(testVSLComplexAdd)
{
	cout<<"Test : klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAdd( a,a,ans);
		klout(a);
		klout(ans);

		BOOST_CHECK(ans[0].real()==(2.7183f+2.7183f));
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAdd "<<endl;
	}
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAdd( a,a,ans);
		klout(a);
		klout(ans);

		BOOST_CHECK(ans[0]==complex<double>(2.7183f+2.7183f,2));
		
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAdd "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klout(b);
		klVSLAdd( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAdd inplace"<<endl;
	}
}

//klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b)
BOOST_AUTO_TEST_CASE(testVSLComplexSubtract)
{
	cout<<"Test : klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSub( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSub "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klVSLSub( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSub inplace"<<endl;
	}
}

//klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b)
BOOST_AUTO_TEST_CASE( testVSLComplexMultiply)
{
	cout<<"Test : klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLMul( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLMul "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klout(b);
		klVSLMul( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLMul inplace"<<endl;
	}
}

//klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b)
BOOST_AUTO_TEST_CASE( testVSLComplexMultiplyByConj)
{
	cout<<"Test : klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLMulByConj( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLMulByConj "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLMulByConj( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLMulByConj inplace"<<endl;
	}
}

//klVector<complex<double > > klVSLConj(klVector< complex<double > > v)
BOOST_AUTO_TEST_CASE(testVSLComplexConj)
{
	cout<<"Test : klVector<complex<double > > klVSLConj(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLConj( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLConj "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLConj( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLConj inplace"<<endl;
	}
}

////vzAbs	Computation of the absolute value of vector elements
//double klVSLAbs(klVector< complex<double > > v );
BOOST_AUTO_TEST_CASE(testVSLComplexAbs)
{
	cout<<"Test : double klVSLAbs(klVector< complex<double > > v ) ;"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAbs( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAbs"<<endl;
	}	
}

////vzArg	Computation of the argument of vector elements
//double  klVSLArg(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexArg)
{
	cout<<"Test : double  klVSLArg(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLArg( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLArg"<<endl;
	}	
}

////vzDiv	Division of elements of one vector by elements of the second vector
//klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b);
BOOST_AUTO_TEST_CASE(testVSLComplexDivide)
{
	cout<<"Test : klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLDiv( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLDiv "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klout(b);
		klVSLDiv( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLDiv inplace"<<endl;
	}
}

////vzSqrt	Computation of the square root of vector elements
//klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexSqrt)
{
	cout<<"Test : klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSqrt( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSqrt "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);
		klout(b);
		klVSLSqrt( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSqrt inplace"<<endl;
	}
}

////vzPow	Raising each vector element to the specified power
//klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b);
BOOST_AUTO_TEST_CASE(testVSLComplexPow)
{
	cout<<"Test :klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b);;"<<endl;
	try
	{
		klVector<complex <double > > v(KL_BOOST_TEST_VECTOR_SIZE);
		v=  complex<double>(2.7183f,1.0f);

		klVector<complex <double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex< double>(2.0f,0.0f);

		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);

		klVSLPow( v,b,ans);
		klout(v);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPow "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex <double > > v(KL_BOOST_TEST_VECTOR_SIZE);
		v=  complex<double>(std::pow( 2.7183,2),1.0f);	

		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		klout(v);		
		klVSLPow( v,b, v);
		klout(v);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPow inplace"<<endl;
	}
}

////vzPowx	Raising each vector element to the constant power
//klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x );
BOOST_AUTO_TEST_CASE(testVSLComplexPowX)
{
	cout<<"Test : klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x );"<<endl;
	try
	{
		klVector<complex <double > > v(KL_BOOST_TEST_VECTOR_SIZE);
		v=  complex<double>(2.7183f,1.0f);
		complex<double> b = complex<double>(2.0f,0.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLPowX( v,b,ans);
		klout(v);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPowX "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > v(KL_BOOST_TEST_VECTOR_SIZE);
		v= complex<double>(std::pow( 2.7183,2),1.0f);		
		complex<double> b =complex<double>(2.0f,0.0f);
		klout(b);
		klVSLPowX(v, b, v);
		klout(v);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPowX inplace"<<endl;
	}
}

////vzExp	Computation of the exponential of vector elements
//klVector<complex<double > > klVSLExp(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexExp)
{
	cout<<"Test : klVector<complex<double > > klVSLExp(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLExp( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLExp "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLExp( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLExp inplace"<<endl;
	}
}

////vzLn	Computation of the natural logarithm of vector elements
//klVector<complex<double > > klVSLLn(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexLn)
{
	cout<<"Test : klVector<complex<double > > klVSLLn(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLLn( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLn "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLLn( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLn inplace"<<endl;
	}
}


////vzLog10	Computation of the denary logarithm of vector elements
//klVector<complex<double > > klVSLLog10(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexLog10)
{
	cout<<"Test : klVector<complex<double > > klVSLLog10(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLLog10( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLog10 "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLLog10( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLog10 inplace"<<endl;
	}
}

////vzCos	Computation of the cosine of vector elements
//klVector<complex<double > > klVSLCos(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexCos)
{
	cout<<"Test : klVector<complex<double > > klVSLCos(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,0.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCos( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCos "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLCos( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCos inplace"<<endl;
	}
}

////vzSin	Computation of the sine of vector elements
//klVector<complex<double > > klVSLSin(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexSin)
{
	cout<<"Test : klVector<complex<double > > klVSLSin(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSin( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSin "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLSin( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSin inplace"<<endl;
	}
}

////vzTan	Computation of the tangent of vector elements
//klVector<complex<double > > klVSLTan(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexTan)
{
	cout<<"Test : klVector<complex<double > > klVSLTan(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSin( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTan "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLTan( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTan inplace"<<endl;
	}
}

////vzAcos	Computation of the inverse cosine of vector elements
//klVector<complex<double > > klVSLAcos(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexAcos)
{
	cout<<"Test : klVector<complex<double > > klVSLAcos(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAcos( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcos "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLAcos( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcos inplace"<<endl;
	}
}

////vzAsin	Computation of the inverse sine of vector elements
//klVector<complex<double > > klVSLAsin(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexAsin)
{
	cout<<"Test : klVector<complex<double > > klVSLAsin(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAsin( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsin "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLAsin( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsin inplace"<<endl;
	}
}

////vzAtan	Computation of the inverse tangent of vector elements
//klVector<complex<double > > klVSLAtan(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexAtan)
{
	cout<<"Test : klVector<complex<double > > klVSLAtan(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAtan( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtan "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLAtan( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtan inplace"<<endl;
	}
}

////vzCosh	Computation of the hyperbolic cosine of vector elements
//klVector<complex<double > > klVSLCosh(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexCosh)
{
	cout<<"Test : klVector<complex<double > > klVSLCosh(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCosh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCosh "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLCosh( b, b);
		klout(b);
		
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCosh inplace"<<endl;
	}
}

////vzSinh	Computation of the hyperbolic sine of vector elements
//klVector<complex<double > > klVSLSinh(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexSinh)
{
	cout<<"Test : klVector<complex<double > > klVSLSinh(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSinh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSinh "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLSinh( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSinh inplace"<<endl;
	}
}

////vzTanh	Computation of the hyperbolic tangent of vector elements
//klVector<complex<double > > klVSLTanh(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexTanh)
{
	cout<<"Test : klVector<complex<double > > klVSLTanh(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLTanh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTanh "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLTanh( b, b);
		klout(b);
		
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTanh inplace"<<endl;
	}
}

////vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
//klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexAcosh)
{
	cout<<"Test : klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);

		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAcosh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcosh "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLAcosh( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcosh inplace"<<endl;
	}
}

////vzAsinh	Computation of the inverse hyperbolic sine of vector elements
//klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexAsinh)
{
	cout<<"Test : klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAsinh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsinh "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLAsinh( b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsinh inplace"<<endl;
	}
}

////vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
//klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexAtanh)
{
	cout<<"Test : klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex<double> > ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAtanh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtanh "<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(KL_BOOST_TEST_VECTOR_SIZE);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klout(b);
		klVSLAtanh( b, b);
		klout(b);
		
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtanh inplace"<<endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(kl_boost_test_double_vsl_functions)
 
//vdAdd	Addition of vector elements
//klVector<double > klVSLAdd(klVector<double  > v,klVector<double > b=false);
BOOST_AUTO_TEST_CASE(testVSLDoubleAdd)
{
	cout<<"Test : klVector<double  > klVSLAdd(klVector<<double >  v,klVector<double>  b);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);klVSLAdd( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAdd "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAdd( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAdd inplace"<<endl;
	}
}

//vdSub		Subtraction of vector elements
//klVector<double > klVSLSub(klVector<double  > v,klVector<double > b);
BOOST_AUTO_TEST_CASE(testVSLDoubleSub)
{
	cout<<"Test : klVector<double  > klVSLSub(klVector<<double >  v,klVector<double>  b);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);klVSLSub( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSub "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLSub( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSub inplace"<<endl;
	}
}

//vdMul		Multiplication of vector elements
//klVector<double > klVSLMul(klVector<double  > v,klVector<double > b);
BOOST_AUTO_TEST_CASE(testVSLDoubleMul)
{
	cout<<"Test : klVector<double  > klVSLMul(klVector<<double >  v,klVector<double>  b);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLMul( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLMul "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLMul( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLMul inplace"<<endl;
	}
}

//vdDiv		Division of elements of one vector by elements of the second vector
//klVector<double > klVSLDiv(klVector<double  > v,klVector<double > b);
BOOST_AUTO_TEST_CASE(testVSLDoubleDiv)
{
	cout<<"Test : klVector<double  > klVSLDiv(klVector<<double >  v,klVector<double>  b);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLDiv( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLDiv "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLDiv( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLDiv inplace"<<endl;
	}
}


//vdHypot		Computation of the square root of sum of squares
//klVector<double > klVSLHypot(klVector<double  > v,klVector<double  > b);
BOOST_AUTO_TEST_CASE(testVSLDoubleHypot)
{
	cout<<"Test : klVector<double  > klVSLHypot(klVector<<double >  v,klVector<double>  b);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLHypot( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLHypot "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLHypot( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLHypot inplace"<<endl;
	}
}

//vdPow		Raising each vector element to the specified power
//klVector<double > klVSLPow(klVector<double  > v,klVector<double > b);
BOOST_AUTO_TEST_CASE(testVSLDoublePow)
{
	cout<<"Test : klVector<double  > klVSLPow(klVector<<double >  v,klVector<double>  b);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLPow( a,a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPow "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLPow( b,b, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPow inplace"<<endl;
	}
}

//vdPowx		Raising each vector element to the constant power
//klVector<double > klVSLPowx(klVector<double  > v,double x);
BOOST_AUTO_TEST_CASE(testVSLDoublePowx)
{
	cout<<"Test : klVector<double  > klVSLPowx(klVector<<double >  v,double> x);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		double x=2.0f;
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLPowx( a,x,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPowx "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));	
		double x=2.0f;
		
		klout(b);
		klVSLPowx( b,x, b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLPowx inplace"<<endl;
	}
}

//vdSqr		Squaring of vector elements
//klVector<double > klVSLSqr(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleSqr)
{
	cout<<"Test : klVector<double  > klVSLSqr(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSqr( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSL "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLSqr( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSqr inplace"<<endl;
	}
}
//vdAbs		Computation of the absolute value of vector elements
//klVector<double > klVSLAbs(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAbs)
{
	cout<<"Test : klVector<double  > klVSLAbs(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAbs( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAbs "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAbs( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAbs inplace"<<endl;
	}
}

//vdInv		Inversion of vector elements
//klVector<double > klVSLInv(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleInv)
{
	cout<<"Test : klVector<double  > klVSLInv(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLInv( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLInv "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLInv( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLInv inplace"<<endl;
	}
}


//vdSqrt Computation of the square root of vector elements
//klVector<double > klVSLSqrt(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleSqrt)
{
	cout<<"Test : klVector<double  > klVSLSqrt(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSqrt( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSqrt "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLSqrt( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSqrt inplace"<<endl;
	}
}
//vdExp	Computation of the exponential of vector elements
//klVector<double > klVSLExp(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleExp)
{
	cout<<"Test : klVector<double  > klVSLExp(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLExp( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLExp "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLExp( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLExp inplace"<<endl;
	}
}

//vdExpm1		Computation of the exponential of vector elements decreased by 1
//klVector<double > klVSLExpm1(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleExpm1)
{
	cout<<"Test : klVector<double  > klVSLExpm1(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLExpm1( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLExpm1 "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLExpm1( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLExpm1 inplace"<<endl;
	}
}

//vdLn	Computation of the natural logarithm of vector elements
//klVector<double > klVSLLn(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleLn)
{
	cout<<"Test : klVector<double  > klVSLLn(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLLn( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLn "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLLn( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLn inplace"<<endl;
	}
}

//vdLog10		Computation of the denary logarithm of vector elements
//klVector<double > klVSLLog10(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleLog10)
{
	cout<<"Test : klVector<double  > klVSLLog10(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLLog10( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLog10 "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLLog10( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLog10 inplace"<<endl;
	}
}

//vdCos		Computation of the cosine of vector elements
//klVector<double > klVSLCos(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleCos)
{
	cout<<"Test : klVector<double  > klVSLCos(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCos( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCos "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLCos( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCos inplace"<<endl;
	}
}

//vdSin		Computation of the sine of vector elements
//klVector<double > klVSLSin(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleSin)
{
	cout<<"Test : klVector<double  > klVSLSin(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSin( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSin "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLSin( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSin inplace"<<endl;
	}
}

//vdTan		Computation of the tangent of vector elements
//klVector<double > klVSLTan(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleTan)
{
	cout<<"Test : klVector<double  > klVSLTan(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLTan( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTan "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLTan( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTan inplace"<<endl;
	}
}

//vdAcos		Computation of the inverse cosine of vector elements
//klVector<double > klVSLAcos(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAcos)
{
	cout<<"Test : klVector<double  > klVSLAcos(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAcos( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcos "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAcos( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcos inplace"<<endl;
	}
}

//vdAsin		Computation of the inverse sine of vector elements
//klVector<double > klVSLAsin(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAsin)
{
	cout<<"Test : klVector<double  > klVSLAsin(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAsin( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsin "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAsin( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsin inplace"<<endl;
	}
}

//vdAtan		Computation of the inverse tangent of vector elements
//klVector<double > klVSLAtan(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAtan)
{
	cout<<"Test : klVector<double  > klVSLAtan(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAtan( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtan "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAtan( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtan inplace"<<endl;
	}
}

//vdCosh		Computation of the hyperbolic cosine of vector elements
//klVector<double > klVSLCosh(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleCosh)
{
	cout<<"Test : klVector<double  > klVSLCosh(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCosh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCosh "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLCosh( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCosh inplace"<<endl;
	}
}

//vdSinh		Computation of the hyperbolic sine of vector elements
//klVector<double > klVSLSqrtSinh(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleSinh)
{
	cout<<"Test : klVector<double  > klVSLSinh(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLSinh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSinh "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLSinh( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLSinh inplace"<<endl;
	}
}

//vdTanh		Computation of the hyperbolic tangent of vector elements
//klVector<double > klVSLTanh(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleTanh)
{
	cout<<"Test : klVector<double  > klVSLTanh(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLTanh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTanh "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLTanh( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTanh inplace"<<endl;
	}
}

//vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
//klVector<double > klVSLAcosh(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAcosh)
{
	cout<<"Test : klVector<double  > klVSLAcosh(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAcosh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSAcoshL "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAcosh( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAcosh inplace"<<endl;
	}
}

//vdAsinh		Computation of the inverse hyperbolic sine of vector elements
//klVector<double > klVSLAsinh(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAsinh)
{
	cout<<"Test : klVector<double  > klVSLAsinh(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);klVSLAsinh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsinh "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAsinh( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAsinh inplace"<<endl;
	}
}

//vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
//klVector<double > klVSLAtanh(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleAtanh)
{
	cout<<"Test : klVector<double  > klVSLAtanh(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLAtanh( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtanh "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLAtanh( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLAtanh inplace"<<endl;
	}
}

//vdErf		Computation of the error function value of vector elements
//klVector<double > klVSLErf(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleErf	)
{
	cout<<"Test : klVector<double  > klVSLErf	(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLErf	( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErf	 "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLErf	( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErf	 inplace"<<endl;
	}
}

//vdErfc		Computation of the complementary error function value of vector elements
//klVector<double > klVSLErfc(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleErfc)
{
	cout<<"Test : klVector<double  > klVSLErfc(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLErfc( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErfc "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLErfc( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErfc inplace"<<endl;
	}
}

//vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
//klVector<double > klVSLCdfNorm(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleCdfNorm)
{
	cout<<"Test : klVector<double  > klVSLCdfNorm(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCdfNorm( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCdfNorm "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLCdfNorm( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCdfNorm inplace"<<endl;
	}
}

//vdErfInv		Computation of the inverse error function value of vector elements
//klVector<double > klVSLErfInv(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleErfInv)
{
	cout<<"Test : klVector<double  > klVSLErfInv(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLErfInv( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErfInv "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLErfInv( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErfInv inplace"<<endl;
	}
}

//vdErfcInv		Computation of the inverse complementary error function value of vector elements
//klVector<double > klVSLErfcInv(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleErfcInv)
{
	cout<<"Test : klVector<double  > klVSLErfcInv(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLErfcInv( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErfcInv "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLErfcInv( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLErfcInv inplace"<<endl;
	}
}

//vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
//klVector<double > klVSLCdfNormInv(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleCdfNormInv)
{
	cout<<"Test : klVector<double  > klVSLCdfNormInv(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCdfNormInv( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCdfNormInv "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLCdfNormInv( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCdfNormInv inplace"<<endl;
	}
}

//vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
//klVector<double > klVSLLGamma(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleLGamma)
{
	cout<<"Test : klVector<double  > klVSLLGamma(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLLGamma( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLGamma "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLLGamma( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLLGamma inplace"<<endl;
	}
}

//vdTGamma		Computation of the gamma function of vector elements 
//klVector<double > klVSLTGamma(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleTGamma)
{
	cout<<"Test : klVector<double  > klVSLTGamma(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLTGamma( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTGamma "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLTGamma( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTGamma inplace"<<endl;
	}
}

//vdFloor		Rounding towards minus infinity
//klVector<double > klVSLFloor(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleFloor)
{
	cout<<"Test : klVector<double  > klVSLFloor(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLFloor( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLFloor "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLFloor( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLFloor inplace"<<endl;
	}
}

//vdCeil		Rounding towards plus infinity
//klVector<double > klVSLCeil(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoublevdCeil)
{
	cout<<"Test : klVector<double  > klVSLCeil(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLCeil( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCeil "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLCeil( b,b);
		klout(b);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLCeil inplace"<<endl;
	}
}

//vdTrunc		Rounding towards zero infinity
//klVector<double > klVSLTrunc(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleTrunc)
{
	cout<<"Test : klVector<double  > klVSLTrunc(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLTrunc( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTrunc "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLTrunc( b,b);
		klout(b);
		
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLTrunc inplace"<<endl;
	}
}

//vdRound		Rounding to nearest integer
//klVector<double > klVSLRound(klVector<double  > v);
BOOST_AUTO_TEST_CASE(testVSLDoubleRound)
{
	cout<<"Test : klVector<double  > klVSLRound(klVector<<double >  v);"<<endl;
	try
	{
		klVector <double  > a(KL_BOOST_TEST_VECTOR_SIZE);
		a=  double(2.7183f);
		klVector< double> ans(KL_BOOST_TEST_VECTOR_SIZE);
		klVSLRound( a,ans);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLRound "<<endl;
	}
	//try in place 
	try
	{
		klVector<double> b(KL_BOOST_TEST_VECTOR_SIZE);
		b= double(std::pow( 2.7183,2));		
		klVSLRound( b,b);
		klout(b);
		
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with klVSLRound inplace"<<endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()



