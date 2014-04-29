#include "kl_matrix.h" 
#include "kl_vsl.h"
/*Automated generation of the test module initialization function
To automatically generate an empty test module initialization function you
need to define BOOST_TEST_MAIN before including the boost/test/unit_test.hpp
header. The value of this define is ignored. Alternatively you can define the
macro BOOST_TEST_MODULE to be equal to any string (not necessarily in quotes).
This macro causes the same result as BOOST_TEST_MAIN, and in addition the
macro value becomes the name of the master test suite. */ 

#define BOOST_TEST_MAIN

#include "boost/test/included/unit_test.hpp"

/*If you prefer to avoid the standalone library compilation, you should use
the single-header variant of the UTF. This variant is implemented, as it
follows from its name, in the single header
boost/test/included/unit_test.hpp. An inclusion of the header causes the
complete implementation of the UTF to be included as a part of a test
module's source file. The header boost/test/unit_test.hpp doesn't have to be
included anymore. You don't have to worry about disabling auto-linking
feature either. It's done in the implementation header already. This variant
can't be used with the multi-file test module. Otherwise it's almost
identical from the usage prospective to the static library variant of the
UTF. In fact the only difference is the name of the include file:
boost/test/included/unit_test.hpp instead of boost/test/unit_test.hpp.

The test runner supplied with this variant requires you to implement the test
module initialization function that matches one of the two specifications
depending on the compilation flag BOOST_TEST_ALTERNATIVE_INIT_API. If flag
isn't defined you are required to match the original specification. If you
define the flag BOOST_TEST_ALTERNATIVE_INIT_API during a test module
compilation you are required to use the alternative initialization function
specification. The UTF provides an ability to automatically generate an empty
test module initialization function with correct specification if no custom
initialization is required by a test module. */

#define BOOST_TEST_MODULE Suites
BOOST_AUTO_TEST_SUITE(klVSLFunctions)
 

//BOOST_TEST_LOG_LEVEL all
//BOOST_TEST_SHOW_PROGRESS yes

BOOST_AUTO_TEST_CASE(TestBoostUTF)
{
	boost::unit_test::progress_monitor.set_stream( cout );
	
    BOOST_CHECK(1 == 4);
	BOOST_TEST_MESSAGE("Test Mesaage");

}
//klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAdd)
{
	cout<<"Test : klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex< double> > ans =klVSLAdd( a,a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAdd inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klVector<complex< double> > ans =klVSLAdd( b,b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAdd inplace ==true"<<endl;
	}
}

//klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace)
BOOST_AUTO_TEST_CASE(testVSLComplexSubtract)
{
	cout<<"Test : klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex< double> > ans =klVSLSub( a,a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSub inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klVector<complex< double> > ans =klVSLSub( b,b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSub inplace ==true"<<endl;
	}
}

//klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace)
BOOST_AUTO_TEST_CASE( testVSLComplexMultiply)
{
	cout<<"Test : klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex< double> > ans =klVSLMul( a,a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLMul inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klVector<complex< double> > ans =klVSLMul( b,b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLMul inplace ==true"<<endl;
	}
}

//klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace)
BOOST_AUTO_TEST_CASE( testVSLComplexMultiplyByConj)
{
	cout<<"Test : klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans =klVSLMulByConj( a,a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLMulByConj inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLMulByConj( b,b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLMulByConj inplace ==true"<<endl;
	}
}

//klVector<complex<double > > klVSLConj(klVector< complex<double > > v, bool inplace)
BOOST_AUTO_TEST_CASE(testVSLComplexConj)
{
	cout<<"Test : klVector<complex<double > > klVSLConj(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans =klVSLConj( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLConj inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLConj( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLConj inplace ==true"<<endl;
	}
}

////vzAbs	Computation of the absolute value of vector elements
//double klVSLAbs(klVector< complex<double > > v );
BOOST_AUTO_TEST_CASE(testVSLComplexAbs)
{
	cout<<"Test : double klVSLAbs(klVector< complex<double > > v ) ;"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<double>  ans =klVSLAbs( a);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAbs"<<endl;
	}	
}

////vzArg	Computation of the argument of vector elements
//double  klVSLArg(klVector< complex<double > > v);
BOOST_AUTO_TEST_CASE(testVSLComplexArg)
{
	cout<<"Test : double  klVSLArg(klVector< complex<double > > v);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<double>  ans =klVSLArg( a);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLArg"<<endl;
	}	
}

////vzDiv	Division of elements of one vector by elements of the second vector
//klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexDivide)
{
	cout<<"Test : klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,0.0f);
		klVector<complex< double> > ans =klVSLDiv( a,a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLDiv inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),0.0f);		
		klVector<complex< double> > ans =klVSLDiv( b,b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLDiv inplace ==true"<<endl;
	}
}

////vzSqrt	Computation of the square root of vector elements
//klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexSqrt)
{
	cout<<"Test : klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans =klVSLSqrt( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSqrt inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLSqrt( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSqrt inplace ==true"<<endl;
	}
}

////vzPow	Raising each vector element to the specified power
//klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexPow)
{
	cout<<"Test :klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);;"<<endl;
	try
	{
		klVector<complex <double > > v(8);
		v=  complex<double>(2.7183f,1.0f);

		klVector<complex <double > > b(8);
		b= complex< double>(2.0f,0.0f);

		klVector<complex< double> > ans =klVSLPow( v,b,false);
		klout(v);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLPow inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex <double > > v(8);
		v=  complex<double>(std::pow( 2.7183,2),1.0f);	

		klVector<complex<double > > b(8);
		b= complex<double>(2.0f,0.0f);		
		klVector<complex< double> > ans =klVSLPow( v,b, true);

		klout(v);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLPow inplace ==true"<<endl;
	}
}

////vzPowx	Raising each vector element to the constant power
//klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x, bool inplace );
BOOST_AUTO_TEST_CASE(testVSLComplexPowX)
{
	cout<<"Test : klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x, bool inplace );"<<endl;
	try
	{
		klVector<complex <double > > v(8);
		v=  complex<double>(2.7183f,1.0f);
		complex<double> b = complex<double>(2.0f,0.0f);
		klVector<complex< double> > ans =klVSLPowX( v,b,false);
		klout(v);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLPowX inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > v(8);
		v= complex<double>(std::pow( 2.7183,2),1.0f);		
		complex<double> b =complex<double>(2.0f,0.0f);
		klVector<complex< double> > ans =klVSLPowX(v, b, true);
		klout(v);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLPowX inplace ==true"<<endl;
	}
}

////vzExp	Computation of the exponential of vector elements
//klVector<complex<double > > klVSLExp(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexExp)
{
	cout<<"Test : klVector<complex<double > > klVSLExp(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans =klVSLExp( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLExp inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLExp( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLExp inplace ==true"<<endl;
	}
}

////vzLn	Computation of the natural logarithm of vector elements
//klVector<complex<double > > klVSLLn(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexLn)
{
	cout<<"Test : klVector<complex<double > > klVSLLn(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans =klVSLLn( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLLn inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLLn( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLLn inplace ==true"<<endl;
	}
}


////vzLog10	Computation of the denary logarithm of vector elements
//klVector<complex<double > > klVSLLog10(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexLog10)
{
	cout<<"Test : klVector<complex<double > > klVSLLog10(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(2.7183f,1.0f);
		klVector<complex< double> > ans =klVSLLog10( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLLog10 inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLLog10( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLLog10 inplace ==true"<<endl;
	}
}

////vzCos	Computation of the cosine of vector elements
//klVector<complex<double > > klVSLCos(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexCos)
{
	cout<<"Test : klVector<complex<double > > klVSLCos(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,0.0f);
		klVector<complex< double> > ans =klVSLCos( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLCos inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLCos( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLCos inplace ==true"<<endl;
	}
}

////vzSin	Computation of the sine of vector elements
//klVector<complex<double > > klVSLSin(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexSin)
{
	cout<<"Test : klVector<complex<double > > klVSLSin(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLSin( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSin inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLSin( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSin inplace ==true"<<endl;
	}
}

////vzTan	Computation of the tangent of vector elements
//klVector<complex<double > > klVSLTan(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexTan)
{
	cout<<"Test : klVector<complex<double > > klVSLTan(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLSin( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLTan inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLTan( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLTan inplace ==true"<<endl;
	}
}

////vzAcos	Computation of the inverse cosine of vector elements
//klVector<complex<double > > klVSLAcos(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAcos)
{
	cout<<"Test : klVector<complex<double > > klVSLAcos(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLAcos( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAcos inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLAcos( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAcos inplace ==true"<<endl;
	}
}

////vzAsin	Computation of the inverse sine of vector elements
//klVector<complex<double > > klVSLAsin(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAsin)
{
	cout<<"Test : klVector<complex<double > > klVSLAsin(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLAsin( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAsin inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLAsin( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAsin inplace ==true"<<endl;
	}
}

////vzAtan	Computation of the inverse tangent of vector elements
//klVector<complex<double > > klVSLAtan(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAtan)
{
	cout<<"Test : klVector<complex<double > > klVSLAtan(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLAtan( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAtan inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLAtan( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAtan inplace ==true"<<endl;
	}
}

////vzCosh	Computation of the hyperbolic cosine of vector elements
//klVector<complex<double > > klVSLCosh(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexCosh)
{
	cout<<"Test : klVector<complex<double > > klVSLCosh(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLCosh( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLCosh inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLCosh( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLCosh inplace ==true"<<endl;
	}
}

////vzSinh	Computation of the hyperbolic sine of vector elements
//klVector<complex<double > > klVSLSinh(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexSinh)
{
	cout<<"Test : klVector<complex<double > > klVSLSinh(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLSinh( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSinh inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLSinh( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLSinh inplace ==true"<<endl;
	}
}

////vzTanh	Computation of the hyperbolic tangent of vector elements
//klVector<complex<double > > klVSLTanh(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexTanh)
{
	cout<<"Test : klVector<complex<double > > klVSLTanh(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLTanh( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLTanh inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLTanh( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLTanh inplace ==true"<<endl;
	}
}

////vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
//klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAcosh)
{
	cout<<"Test : klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLAcosh( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAcosh inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLAcosh( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAcosh inplace ==true"<<endl;
	}
}

////vzAsinh	Computation of the inverse hyperbolic sine of vector elements
//klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAsinh)
{
	cout<<"Test : klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLAsinh( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAsinh inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLAsinh( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAsinh inplace ==true"<<endl;
	}
}

////vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
//klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v, bool inplace);
BOOST_AUTO_TEST_CASE(testVSLComplexAtanh)
{
	cout<<"Test : klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v, bool inplace);"<<endl;
	try
	{
		klVector<complex <double > > a(8);
		a=  complex<double>(3.14159f,1.0f);
		klVector<complex< double> > ans =klVSLAtanh( a,false);
		klout(a);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAtanh inplace==false"<<endl;
	}
	//try in place 
	try
	{
		klVector<complex<double > > b(8);
		b= complex<double>(std::pow( 2.7183,2),1.0f);		
		klVector<complex< double> > ans =klVSLAtanh( b, true);
		klout(b);
		klout(ans);
	}
	catch(std::exception e)
	{
		cerr<<e.what();
	}
	catch(...)
	{
		cerr<<"Error with in place klVSLAtanh inplace ==true"<<endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()




void testklMatrix()
{
	cout<<"Test :	klMatrix<double> kld(1,1);	kld=0.0f;"<<endl;
	klMatrix<double> kld(1,1);
	kld=0.0f;
	klout(kld);	
}


