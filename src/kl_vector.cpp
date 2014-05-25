
#include "kl_vsl.h"

klMemMgr* klGlobalMemoryManager::_globalMemoryManager=NULL;
void klGlobalMemoryManager::setklVectorGlobalMemoryManager(klMemMgr* mgr)
{
	_globalMemoryManager=mgr;
}

//vzAdd	Addition of vector elements
void klVSLAdd(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const __int64 n = v.getColumns();

	vzAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzSub	Subtraction of vector elements
void klVSLSub(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const __int64 n = v.getColumns();


	vzSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzMul	Multiplication of vector elements :  elementwise
void klVSLMul(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMul";
	const __int64 n = v.getColumns();


	vzMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzMul	Multiplication of vector by conjugate of secong arg 
void klVSLMulByConj(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMulByConj";
	const __int64 n = v.getColumns();

	vzMulByConj( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzConj	Conjugation of vector elements
void klVSLConj(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzConj( n,  v.getMemory(),ans.getMemory());
}

//vzAbs	Computation of the absolute value of vector elements
void klVSLAbs(klVector< complex<double > >& v ,klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();
	
	vzAbs( n,  v.getMemory(),ans.getMemory());
}

//vzArg	Computation of the argument of vector elements
void klVSLArg(klVector< complex<double > >& v, klVector<double>& ans  )
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzArg( n,  v.getMemory(),ans.getMemory());
}

//vzDiv	Division of elements of one vector by elements of the second vector
void klVSLDiv(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLDiv";

	// Elementwise equal to the scalar
	klVector<bool> checkNonZero =( b==complex<double>(0.0f,0.0f));

	if(checkNonZero.sum()>0)
	{
		std::cerr<<"Warning : divide By zero in klVSLDiv check for Inf in result"<<endl;
		//throw "Divide By Zeroin klSVLDiv"
	}

	const __int64 n = v.getColumns();

	vzDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzSqrt	Computation of the square root of vector elements
void klVSLSqrt(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzSqrt( n,  v.getMemory(),ans.getMemory());
}

//vzPow	Raising each vector element to the specified power
void klVSLPow(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLDiv";

	const __int64 n = v.getColumns();

	vzPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzPowx	Raising each vector element to the constant power
void klVSLPowX(klVector< complex<double > >& v,complex< double> x, klVector< complex<double > >& ans )
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzPowx( n,  v.getMemory(),x,ans.getMemory());
}

//vzExp	Computation of the exponential of vector elements
void klVSLExp(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzExp( n,  v.getMemory(),ans.getMemory());
}

//vzLn	Computation of the natural logarithm of vector elements
void klVSLLn(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzLn( n,  v.getMemory(),ans.getMemory());
}

//vzLog10	Computation of the denary logarithm of vector elements
void klVSLLog10(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzLog10( n,  v.getMemory(),ans.getMemory());

}

//vzCos	Computation of the cosine of vector elements
void klVSLCos(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzCos( n,  v.getMemory(),ans.getMemory());

}

//vzSin	Computation of the sine of vector elements
void klVSLSin(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();


	vzSin( n,  v.getMemory(),ans.getMemory());
}

//vzTan	Computation of the tangent of vector elements
void klVSLTan(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzTan( n,  v.getMemory(),ans.getMemory());
}

//vzAcos	Computation of the inverse cosine of vector elements
void klVSLAcos(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzAcos( n,  v.getMemory(),ans.getMemory());
}

//vzAsin	Computation of the inverse sine of vector elements
void klVSLAsin(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzAsin( n,  v.getMemory(),ans.getMemory());
}

//vzAtan	Computation of the inverse tangent of vector elements
void klVSLAtan(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzAtan( n,  v.getMemory(),ans.getMemory());
}

//vzCosh	Computation of the hyperbolic cosine of vector elements
void klVSLCosh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzCosh( n,  v.getMemory(),ans.getMemory());
}

//vzSinh	Computation of the hyperbolic sine of vector elements
void klVSLSinh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzSinh( n,  v.getMemory(),ans.getMemory());
}

//vzTanh	Computation of the hyperbolic tangent of vector elements
void klVSLTanh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();

	vzTanh( n,  v.getMemory(),ans.getMemory());
}

//vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
void klVSLAcosh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	const __int64 n = v.getColumns();
	vzAcosh( n,  v.getMemory(),ans.getMemory());
}

//vzAsinh	Computation of the inverse hyperbolic sine of vector elements
void klVSLAsinh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();
	vzAsinh( n,  v.getMemory(),ans.getMemory());

}

//vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
void klVSLAtanh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const __int64 n = v.getColumns();
	vzAtanh( n,  v.getMemory(),ans.getMemory());
	
}

//------------------------double precision methods

//vdAdd	Addition of vector elements
void klVSLAdd(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const __int64 n = v.getColumns();


	vdAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdSub		Subtraction of vector elements
void klVSLSub(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const __int64 n = v.getColumns();

	vdSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdMul		Multiplication of vector elements
void klVSLMul(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMul";
	const __int64 n = v.getColumns();

	vdMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdDiv		Division of elements of one vector by elements of the second vector
void klVSLDiv(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const __int64 n = v.getColumns();

	vdDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdPow		Raising each vector element to the specified power
void klVSLPow(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLPow";
	const __int64 n = v.getColumns();


	vdPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}
//vdSqr		Squaring of vector elements
void klVSLSqr(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdSqr( n,  v.getMemory(),ans.getMemory());

}

//vdAbs		Computation of the absolute value of vector elements
void klVSLAbs(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdAbs( n,  v.getMemory(),ans.getMemory());

}

//vdInv		Inversion of vector elements
void klVSLInv(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdInv( n,  v.getMemory(),ans.getMemory());

}

//vdSqrt Computation of the square root of vector elements
void klVSLSqrt(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdSqrt( n,  v.getMemory(),ans.getMemory());

}

//vdPowx		Raising each vector element to the constant power
void klVSLPowx(klVector<double>& v,double x,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdPowx( n,  v.getMemory(),x,ans.getMemory());

}

//vdHypot		Computation of the square root of sum of squares
void klVSLHypot(klVector<double>& v,klVector<double  >& b,klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const __int64 n = v.getColumns();

	vdHypot( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdExp	Computation of the exponential of vector elements
void klVSLExp(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdExp( n,  v.getMemory(),ans.getMemory());

}

//vdExpm1		Computation of the exponential of vector elements decreased by 1
void klVSLExpm1(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdExpm1( n,  v.getMemory(),ans.getMemory());

}

//vdLn	Computation of the natural logarithm of vector elements
void klVSLLn(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdLn( n,  v.getMemory(),ans.getMemory());

}

//vdLog10		Computation of the denary logarithm of vector elements
void klVSLLog10(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdLog10( n,  v.getMemory(),ans.getMemory());

}

//vdCos		Computation of the cosine of vector elements
void klVSLCos(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdCos( n,  v.getMemory(),ans.getMemory());

}

//vdSin		Computation of the sine of vector elements
void klVSLSin(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdSin( n,  v.getMemory(),ans.getMemory());

}

//vdTan		Computation of the tangent of vector elements
void klVSLTan(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdTan( n,  v.getMemory(),ans.getMemory());

}

//vdAcos		Computation of the inverse cosine of vector elements
void klVSLAcos(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdAcos( n,  v.getMemory(),ans.getMemory());

}

//vdAsin		Computation of the inverse sine of vector elements
void klVSLAsin(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdAsin( n,  v.getMemory(),ans.getMemory());

}

//vdAtan		Computation of the inverse tangent of vector elements
void klVSLAtan(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdAtan( n,  v.getMemory(),ans.getMemory());

}

//vdCosh		Computation of the hyperbolic cosine of vector elements
void klVSLCosh(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdCosh( n,  v.getMemory(),ans.getMemory());

}

//vdSinh		Computation of the hyperbolic sine of vector elements
void klVSLSinh(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdSinh( n,  v.getMemory(),ans.getMemory());

}

//vdTanh		Computation of the hyperbolic tangent of vector elements
void klVSLTanh(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdTanh( n,  v.getMemory(),ans.getMemory());

}

//vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
void klVSLAcosh(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdAcosh( n,  v.getMemory(),ans.getMemory());

}

//vdAsinh		Computation of the inverse hyperbolic sine of vector elements
void klVSLAsinh(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdAsinh( n,  v.getMemory(),ans.getMemory());

}

//vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
void klVSLAtanh(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdAtanh( n,  v.getMemory(),ans.getMemory());

}

//vdErf		Computation of the error function value of vector elements
void klVSLErf(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdErf( n,  v.getMemory(),ans.getMemory());

}

//vdErfc		Computation of the complementary error function value of vector elements
void klVSLErfc(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdErfc( n,  v.getMemory(),ans.getMemory());

}

//vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
void klVSLCdfNorm(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdCdfNorm( n,  v.getMemory(),ans.getMemory());

}

//vdErfInv		Computation of the inverse error function value of vector elements
void klVSLErfInv(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdErfInv( n,  v.getMemory(),ans.getMemory());

}

//vdErfcInv		Computation of the inverse complementary error function value of vector elements
void klVSLErfcInv(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdErfcInv( n,  v.getMemory(),ans.getMemory());

}

//vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
void klVSLCdfNormInv(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdCdfNormInv( n,  v.getMemory(),ans.getMemory());

}

//vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
void klVSLLGamma(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdLGamma( n,  v.getMemory(),ans.getMemory());

}

//vdTGamma		Computation of the gamma function of vector elements 
void klVSLTGamma(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();
	vdTGamma( n,  v.getMemory(),ans.getMemory());

}

//vdFloor		Rounding towards minus infinity
void klVSLFloor(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdFloor( n,  v.getMemory(),ans.getMemory());

}

//vdCeil		Rounding towards plus infinity
void klVSLCeil(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();


	vdCeil( n,  v.getMemory(),ans.getMemory());

}

//vdTrunc		Rounding towards zero infinity
void klVSLTrunc(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();

	vdTrunc( n,  v.getMemory(),ans.getMemory());

}
//vdRound		Rounding to nearest integer
void klVSLRound(klVector<double>& v,klVector<double>& ans)
{
	const __int64 n = v.getColumns();
	vdRound( n,  v.getMemory(),ans.getMemory());
}

////vzAdd	Addition of vector elements.  Smart Pointer Version. This design needs to be solidified.  
//klComplexDoubleVectorPtr klVSLAdd(klComplexDoubleVectorPtr v,klComplexDoubleVectorPtr b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v->getColumns() != b->getColumns() )
//		throw "Range Argument Exception in klVSLAdd";
//	const __int64 n = v->getColumns();
//	
//	if(!inplace)
//	{
//		klComplexDoubleVectorPtr ans= new klVector<complex<double>  >  (v->getColumns() );		
//
//		vzAdd( n,  v->getMemory(),b->getMemory(),ans->getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAdd( n, v->getMemory(),b->getMemory(), v->getMemory());
//		return v;
//	}
//}


//vdNearbyInt		Rounding according to current mode

//vdRint		Rounding according to current mode and raising inexact result exception

//vdModf		Computation of the integer and fractional parts
