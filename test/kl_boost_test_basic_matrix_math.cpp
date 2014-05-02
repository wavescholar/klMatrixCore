#include "kl_matrix.h"

//#include <boost/test/unit_test.hpp>

//#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE "C++ Unit Tests for kl Matrix Library"
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(kl_boost_test_basic_matrix_math)
 
//BOOST_TEST_LOG_LEVEL all
//BOOST_TEST_SHOW_PROGRESS yes

BOOST_AUTO_TEST_CASE(TestBoostUTF)
{
	//boost::unit_test::progress_monitor.set_stream( cout );
	
    BOOST_CHECK(1 == 4);
	BOOST_TEST_MESSAGE("Test Mesaage");

}

BOOST_AUTO_TEST_CASE(testklMatrix)
{
	cout<<"Test :	klMatrix<double> kld(1,1);	kld=0.0f;"<<endl;
	klMatrix<double> kld(1,1);
	kld=0.0f;
	klout(kld);	
}

BOOST_AUTO_TEST_SUITE_END()







