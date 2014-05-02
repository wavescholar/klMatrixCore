#include "kl_vsl.h"

//vzAdd	Addition of vector elements
void klVSLAdd(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const MKL_INT n = v.getColumns();

	vzAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzSub	Subtraction of vector elements
void klVSLSub(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const MKL_INT n = v.getColumns();


	vzSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzMul	Multiplication of vector elements :  elementwise
void klVSLMul(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMul";
	const MKL_INT n = v.getColumns();


	vzMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzMul	Multiplication of vector by conjugate of secong arg 
void klVSLMulByConj(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMulByConj";
	const MKL_INT n = v.getColumns();

	vzMulByConj( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzConj	Conjugation of vector elements
void klVSLConj(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzConj( n,  v.getMemory(),ans.getMemory());
}

//vzAbs	Computation of the absolute value of vector elements
void klVSLAbs(klVector< complex<double > >& v ,klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	vzAbs( n,  v.getMemory(),ans.getMemory());
}

//vzArg	Computation of the argument of vector elements
void klVSLArg(klVector< complex<double > >& v, klVector<double>& ans  )
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

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

	const MKL_INT n = v.getColumns();

	vzDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzSqrt	Computation of the square root of vector elements
void klVSLSqrt(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzSqrt( n,  v.getMemory(),ans.getMemory());
}

//vzPow	Raising each vector element to the specified power
void klVSLPow(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLDiv";

	const MKL_INT n = v.getColumns();

	vzPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());
}

//vzPowx	Raising each vector element to the constant power
void klVSLPowX(klVector< complex<double > >& v,complex< double> x, klVector< complex<double > >& ans )
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzPowx( n,  v.getMemory(),x,ans.getMemory());
}

//vzExp	Computation of the exponential of vector elements
void klVSLExp(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzExp( n,  v.getMemory(),ans.getMemory());
}

//vzLn	Computation of the natural logarithm of vector elements
void klVSLLn(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzLn( n,  v.getMemory(),ans.getMemory());
}

//vzLog10	Computation of the denary logarithm of vector elements
void klVSLLog10(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzLog10( n,  v.getMemory(),ans.getMemory());

}

//vzCos	Computation of the cosine of vector elements
void klVSLCos(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzCos( n,  v.getMemory(),ans.getMemory());

}

//vzSin	Computation of the sine of vector elements
void klVSLSin(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();


	vzSin( n,  v.getMemory(),ans.getMemory());
}

//vzTan	Computation of the tangent of vector elements
void klVSLTan(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzTan( n,  v.getMemory(),ans.getMemory());
}

//vzAcos	Computation of the inverse cosine of vector elements
void klVSLAcos(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzAcos( n,  v.getMemory(),ans.getMemory());
}

//vzAsin	Computation of the inverse sine of vector elements
void klVSLAsin(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzAsin( n,  v.getMemory(),ans.getMemory());
}

//vzAtan	Computation of the inverse tangent of vector elements
void klVSLAtan(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzAtan( n,  v.getMemory(),ans.getMemory());
}

//vzCosh	Computation of the hyperbolic cosine of vector elements
void klVSLCosh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzCosh( n,  v.getMemory(),ans.getMemory());
}

//vzSinh	Computation of the hyperbolic sine of vector elements
void klVSLSinh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzSinh( n,  v.getMemory(),ans.getMemory());
}

//vzTanh	Computation of the hyperbolic tangent of vector elements
void klVSLTanh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();

	vzTanh( n,  v.getMemory(),ans.getMemory());
}

//vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
void klVSLAcosh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	const MKL_INT n = v.getColumns();
	vzAcosh( n,  v.getMemory(),ans.getMemory());
}

//vzAsinh	Computation of the inverse hyperbolic sine of vector elements
void klVSLAsinh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	vzAsinh( n,  v.getMemory(),ans.getMemory());

}

//vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
void klVSLAtanh(klVector< complex<double > >& v, klVector< complex<double > >& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	vzAtanh( n,  v.getMemory(),ans.getMemory());
	
}

//------------------------double precision methods

//vdAdd	Addition of vector elements
void klVSLAdd(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const MKL_INT n = v.getColumns();


	vdAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdSub		Subtraction of vector elements
void klVSLSub(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const MKL_INT n = v.getColumns();

	vdSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdMul		Multiplication of vector elements
void klVSLMul(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMul";
	const MKL_INT n = v.getColumns();

	vdMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdDiv		Division of elements of one vector by elements of the second vector
void klVSLDiv(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const MKL_INT n = v.getColumns();

	vdDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdPow		Raising each vector element to the specified power
void klVSLPow(klVector<double>& v,klVector<double>& b, klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLPow";
	const MKL_INT n = v.getColumns();


	vdPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}
//vdSqr		Squaring of vector elements
void klVSLSqr(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdSqr( n,  v.getMemory(),ans.getMemory());

}

//vdAbs		Computation of the absolute value of vector elements
void klVSLAbs(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdAbs( n,  v.getMemory(),ans.getMemory());

}

//vdInv		Inversion of vector elements
void klVSLInv(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdInv( n,  v.getMemory(),ans.getMemory());

}

//vdSqrt Computation of the square root of vector elements
void klVSLSqrt(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdSqrt( n,  v.getMemory(),ans.getMemory());

}

//vdPowx		Raising each vector element to the constant power
void klVSLPowx(klVector<double>& v,double x,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdPowx( n,  v.getMemory(),x,ans.getMemory());

}

//vdHypot		Computation of the square root of sum of squares
void klVSLHypot(klVector<double>& v,klVector<double  >& b,klVector<double>& ans)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const MKL_INT n = v.getColumns();

	vdHypot( n,  v.getMemory(),b.getMemory(),ans.getMemory());

}

//vdExp	Computation of the exponential of vector elements
void klVSLExp(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdExp( n,  v.getMemory(),ans.getMemory());

}

//vdExpm1		Computation of the exponential of vector elements decreased by 1
void klVSLExpm1(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdExpm1( n,  v.getMemory(),ans.getMemory());

}

//vdLn	Computation of the natural logarithm of vector elements
void klVSLLn(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdLn( n,  v.getMemory(),ans.getMemory());

}

//vdLog10		Computation of the denary logarithm of vector elements
void klVSLLog10(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdLog10( n,  v.getMemory(),ans.getMemory());

}

//vdCos		Computation of the cosine of vector elements
void klVSLCos(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdCos( n,  v.getMemory(),ans.getMemory());

}

//vdSin		Computation of the sine of vector elements
void klVSLSin(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdSin( n,  v.getMemory(),ans.getMemory());

}

//vdTan		Computation of the tangent of vector elements
void klVSLTan(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdTan( n,  v.getMemory(),ans.getMemory());

}

//vdAcos		Computation of the inverse cosine of vector elements
void klVSLAcos(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdAcos( n,  v.getMemory(),ans.getMemory());

}

//vdAsin		Computation of the inverse sine of vector elements
void klVSLAsin(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdAsin( n,  v.getMemory(),ans.getMemory());

}

//vdAtan		Computation of the inverse tangent of vector elements
void klVSLAtan(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdAtan( n,  v.getMemory(),ans.getMemory());

}

//vdCosh		Computation of the hyperbolic cosine of vector elements
void klVSLCosh(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdCosh( n,  v.getMemory(),ans.getMemory());

}

//vdSinh		Computation of the hyperbolic sine of vector elements
void klVSLSinh(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdSinh( n,  v.getMemory(),ans.getMemory());

}

//vdTanh		Computation of the hyperbolic tangent of vector elements
void klVSLTanh(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdTanh( n,  v.getMemory(),ans.getMemory());

}

//vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
void klVSLAcosh(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdAcosh( n,  v.getMemory(),ans.getMemory());

}

//vdAsinh		Computation of the inverse hyperbolic sine of vector elements
void klVSLAsinh(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdAsinh( n,  v.getMemory(),ans.getMemory());

}

//vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
void klVSLAtanh(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdAtanh( n,  v.getMemory(),ans.getMemory());

}

//vdErf		Computation of the error function value of vector elements
void klVSLErf(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdErf( n,  v.getMemory(),ans.getMemory());

}

//vdErfc		Computation of the complementary error function value of vector elements
void klVSLErfc(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdErfc( n,  v.getMemory(),ans.getMemory());

}

//vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
void klVSLCdfNorm(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdCdfNorm( n,  v.getMemory(),ans.getMemory());

}

//vdErfInv		Computation of the inverse error function value of vector elements
void klVSLErfInv(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdErfInv( n,  v.getMemory(),ans.getMemory());

}

//vdErfcInv		Computation of the inverse complementary error function value of vector elements
void klVSLErfcInv(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdErfcInv( n,  v.getMemory(),ans.getMemory());

}

//vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
void klVSLCdfNormInv(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdCdfNormInv( n,  v.getMemory(),ans.getMemory());

}

//vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
void klVSLLGamma(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdLGamma( n,  v.getMemory(),ans.getMemory());

}

//vdTGamma		Computation of the gamma function of vector elements 
void klVSLTGamma(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();
	vdTGamma( n,  v.getMemory(),ans.getMemory());

}

//vdFloor		Rounding towards minus infinity
void klVSLFloor(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdFloor( n,  v.getMemory(),ans.getMemory());

}

//vdCeil		Rounding towards plus infinity
void klVSLCeil(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();


	vdCeil( n,  v.getMemory(),ans.getMemory());

}

//vdTrunc		Rounding towards zero infinity
void klVSLTrunc(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();

	vdTrunc( n,  v.getMemory(),ans.getMemory());

}
//vdRound		Rounding to nearest integer
void klVSLRound(klVector<double>& v,klVector<double>& ans)
{
	const MKL_INT n = v.getColumns();
	vdRound( n,  v.getMemory(),ans.getMemory());
}

////vzAdd	Addition of vector elements.  Smart Pointer Version. This design needs to be solidified.  
//klCDVP klVSLAdd(klCDVP v,klCDVP b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v->getColumns() != b->getColumns() )
//		throw "Range Argument Exception in klVSLAdd";
//	const MKL_INT n = v->getColumns();
//	
//	if(!inplace)
//	{
//		klCDVP ans= new klVector<complex<double>  >  (v->getColumns() );		
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




//
//
////vzAdd	Addition of vector elements
//klVector<complex<double > > klVSLAdd(klVector< complex<double > >& v,klVector< complex<double> >& b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLAdd";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAdd( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzSub	Subtraction of vector elements
//klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLSub";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzSub( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzMul	Multiplication of vector elements :  elementwise
//klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLMul";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzMul( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzMul	Multiplication of vector by conjugate of secong arg 
//klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLMulByConj";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzMulByConj( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzMulByConj( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzConj	Conjugation of vector elements
//klVector<complex<double > > klVSLConj(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzConj( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzConj( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAbs	Computation of the absolute value of vector elements
//klVector<double> klVSLAbs(klVector< complex<double > > v )
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	klVector<double> ans(n);		
//
//	vzAbs( n,  v.getMemory(),ans.getMemory());
//	return ans;
//}
//
////vzArg	Computation of the argument of vector elements
//klVector<double>  klVSLArg(klVector< complex<double > > v)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//		
//	klVector<double> ans(n);		
//
//	vzArg( n,  v.getMemory(),ans.getMemory());
//	return ans;
//}
//
////vzDiv	Division of elements of one vector by elements of the second vector
//klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLDiv";
//
//	// Elementwise equal to the scalar
//	klVector<bool> checkNonZero =( b==complex<double>(0.0f,0.0f));
//
//	if(checkNonZero.sum()>0)
//	{
//		std::cerr<<"Warning : divide By zero in klVSLDiv check for Inf in result"<<endl;
//		//throw "Divide By Zeroin klSVLDiv"
//	}
//
//	const MKL_INT n = v.getColumns();
//
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzDiv( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzSqrt	Computation of the square root of vector elements
//klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzSqrt( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzSqrt( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzPow	Raising each vector element to the specified power
//klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
//{
//		vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLDiv";
//
//	const MKL_INT n = v.getColumns();
//
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzPow( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzPowx	Raising each vector element to the constant power
//klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x, bool inplace=false )
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzPowx( n,  v.getMemory(),x,ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzPowx( n, v.getMemory(),x, v.getMemory());
//		return v;
//	}
//}
//
////vzExp	Computation of the exponential of vector elements
//klVector<complex<double > > klVSLExp(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzExp( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzExp( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzLn	Computation of the natural logarithm of vector elements
//klVector<complex<double > > klVSLLn(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzLn( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzLn( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzLog10	Computation of the denary logarithm of vector elements
//klVector<complex<double > > klVSLLog10(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzLog10( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzLog10( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzCos	Computation of the cosine of vector elements
//klVector<complex<double > > klVSLCos(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzCos( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzCos( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzSin	Computation of the sine of vector elements
//klVector<complex<double > > klVSLSin(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzSin( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzSin( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzTan	Computation of the tangent of vector elements
//klVector<complex<double > > klVSLTan(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzTan( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzTan( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAcos	Computation of the inverse cosine of vector elements
//klVector<complex<double > > klVSLAcos(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAcos( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAcos( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAsin	Computation of the inverse sine of vector elements
//klVector<complex<double > > klVSLAsin(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAsin( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAsin( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAtan	Computation of the inverse tangent of vector elements
//klVector<complex<double > > klVSLAtan(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAtan( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAtan( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzCosh	Computation of the hyperbolic cosine of vector elements
//klVector<complex<double > > klVSLCosh(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzCosh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzCosh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzSinh	Computation of the hyperbolic sine of vector elements
//klVector<complex<double > > klVSLSinh(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzSinh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzSinh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzTanh	Computation of the hyperbolic tangent of vector elements
//klVector<complex<double > > klVSLTanh(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzTanh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzTanh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
//klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAcosh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAcosh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAsinh	Computation of the inverse hyperbolic sine of vector elements
//klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAsinh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAsinh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
//klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<complex<double>  > ans (v.getColumns() );		
//
//		vzAtanh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vzAtanh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
//
//
///* Double Precision VSL Functions we would like to support
//
//vdAdd	s, d, c, z	Addition of vector elements
//vdSub	s, d, c, z	Subtraction of vector elements
//vdSqr	s, d	Squaring of vector elements
//vdMul	s, d, c, z	Multiplication of vector elements
//vdMulByConj	c, z	Multiplication of elements of one vector by conjugated elements of the second vector
//vdConj	c, z	Conjugation of vector elements
//vdAbs	s, d, c, z	Computation of the absolute value of vector elements
//vdArg	c, z	Computation of the argument of vector elements
//vdLinearFrac	s, d	Linear fraction transformation of vectors
//vdInv	s, d	Inversion of vector elements
//vdDiv	s, d, c, z	Division of elements of one vector by elements of the second vector
//vdSqrt	s, d, c, z	Computation of the square root of vector elements
//vdInvSqrt	s, d	Computation of the inverse square root of vector elements
//vdCbrt	s, d	Computation of the cube root of vector elements
//vdInvCbrt	s, d	Computation of the inverse cube root of vector elements
//vdPow2o3	s, d	Raising each vector element to the power of 2/3
//vdPow3o2	s, d	Raising each vector element to the power of 3/2
//vdPow	s, d, c, z	Raising each vector element to the specified power
//vdPowx	s, d, c, z	Raising each vector element to the constant power
//vdHypot	s, d	Computation of the square root of sum of squares
//vdExp	s, d, c, z	Computation of the exponential of vector elements
//vdExpm1	s, d	Computation of the exponential of vector elements decreased by 1
//vdLn	s, d, c, z	Computation of the natural logarithm of vector elements
//vdLog10	s, d, c, z	Computation of the denary logarithm of vector elements
//vdLog1p	s, d	Computation of the natural logarithm of vector elements that are increased by 1
//vdCos	s, d, c, z	Computation of the cosine of vector elements
//vdSin	s, d, c, z	Computation of the sine of vector elements
//vdSinCos	s, d	Computation of the sine and cosine of vector elements
//vdCIS	c, z	Computation of the complex exponent of vector elements (cosine and sine combined to complex value)
//vdTan	s, d, c, z	Computation of the tangent of vector elements
//vdAcos	s, d, c, z	Computation of the inverse cosine of vector elements
//vdAsin	s, d, c, z	Computation of the inverse sine of vector elements
//vdAtan	s, d, c, z	Computation of the inverse tangent of vector elements
//vdAtan2	s, d	Computation of the four-quadrant inverse tangent of elements of two vectors
//vdCosh	s, d, c, z	Computation of the hyperbolic cosine of vector elements
//vdSinh	s, d, c, z	Computation of the hyperbolic sine of vector elements
//vdTanh	s, d, c, z	Computation of the hyperbolic tangent of vector elements
//vdAcosh	s, d, c, z	Computation of the inverse hyperbolic cosine of vector elements
//vdAsinh	s, d, c, z	Computation of the inverse hyperbolic sine of vector elements
//vdAtanh	s, d, c, z	Computation of the inverse hyperbolic tangent of vector elements.
//vdErf	s, d	Computation of the error function value of vector elements
//vdErfc	s, d	Computation of the complementary error function value of vector elements
//vdCdfNorm	s, d	Computation of the cumulative normal distribution function value of vector elements
//vdErfInv	s, d	Computation of the inverse error function value of vector elements
//vdErfcInv	s, d	Computation of the inverse complementary error function value of vector elements
//vdCdfNormInv	s, d	Computation of the inverse cumulative normal distribution function value of vector elements
//vdLGamma	s, d	Computation of the natural logarithm for the absolute value of the gamma function of vector elements
//vdTGamma	s, d	Computation of the gamma function of vector elements 
//vdFloor	s, d	Rounding towards minus infinity
//vdCeil	s, d	Rounding towards plus infinity
//vdTrunc	s, d	Rounding towards zero infinity
//vdRound	s, d	Rounding to nearest integer
//vdNearbyInt	s, d	Rounding according to current mode
//vdRint	s, d	Rounding according to current mode and raising inexact result exception
//vdModf	s, d	Computation of the integer and fractional parts
//vdFrac	s, d	Computation of the fractional part
//*/
//
////vdAdd	Addition of vector elements
//klVector<double > klVSLAdd(klVector<double  > v,klVector<double > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLAdd";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAdd( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdSub		Subtraction of vector elements
//klVector<double > klVSLSub(klVector<double  > v,klVector<double > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLSub";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdSub( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdMul		Multiplication of vector elements
//klVector<double > klVSLMul(klVector<double  > v,klVector<double > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLMul";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdMul( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdDiv		Division of elements of one vector by elements of the second vector
//klVector<double > klVSLDiv(klVector<double  > v,klVector<double > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLSub";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdDiv( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdPow		Raising each vector element to the specified power
//klVector<double > klVSLPow(klVector<double  > v,klVector<double > b, bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLPow";
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdPow( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
////vdSqr		Squaring of vector elements
//klVector<double > klVSLSqr(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdSqr( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdSqr( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAbs		Computation of the absolute value of vector elements
//klVector<double > klVSLAbs(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAbs( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAbs( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdInv		Inversion of vector elements
//klVector<double > klVSLInv(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdInv( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdInv( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdSqrt Computation of the square root of vector elements
//klVector<double > klVSLSqrt(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdSqrt( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdSqrt( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdPowx		Raising each vector element to the constant power
//klVector<double > klVSLPowx(klVector<double  > v,double x,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdPowx( n,  v.getMemory(),x,ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdPowx( n, v.getMemory(),x, v.getMemory());
//		return v;
//	}
//}
//
////vdHypot		Computation of the square root of sum of squares
//klVector<double > klVSLHypot(klVector<double  > v,klVector<double  > b,bool inplace=false)
//{
//	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
//	if(v.getColumns() != b.getColumns() )
//		throw "Range Argument Exception in klVSLAdd";
//	const MKL_INT n = v.getColumns();
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdHypot( n,  v.getMemory(),b.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdHypot( n, v.getMemory(),b.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdExp	Computation of the exponential of vector elements
//klVector<double > klVSLExp(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdExp( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdExp( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdExpm1		Computation of the exponential of vector elements decreased by 1
//klVector<double > klVSLExpm1(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdExpm1( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdExpm1( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdLn	Computation of the natural logarithm of vector elements
//klVector<double > klVSLLn(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdLn( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdLn( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdLog10		Computation of the denary logarithm of vector elements
//klVector<double > klVSLLog10(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdLog10( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdLog10( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdCos		Computation of the cosine of vector elements
//klVector<double > klVSLCos(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdCos( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdCos( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdSin		Computation of the sine of vector elements
//klVector<double > klVSLSin(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdSin( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdSin( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdTan		Computation of the tangent of vector elements
//klVector<double > klVSLTan(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdTan( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdTan( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAcos		Computation of the inverse cosine of vector elements
//klVector<double > klVSLAcos(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAcos( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAcos( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAsin		Computation of the inverse sine of vector elements
//klVector<double > klVSLAsin(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAsin( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAsin( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAtan		Computation of the inverse tangent of vector elements
//klVector<double > klVSLAtan(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAtan( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAtan( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdCosh		Computation of the hyperbolic cosine of vector elements
//klVector<double > klVSLCosh(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdCosh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdCosh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdSinh		Computation of the hyperbolic sine of vector elements
//klVector<double > klVSLSinh(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdSinh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdSinh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdTanh		Computation of the hyperbolic tangent of vector elements
//klVector<double > klVSLTanh(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdTanh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdTanh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
//klVector<double > klVSLAcosh(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAcosh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAcosh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAsinh		Computation of the inverse hyperbolic sine of vector elements
//klVector<double > klVSLAsinh(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAsinh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAsinh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
//klVector<double > klVSLAtanh(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdAtanh( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdAtanh( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdErf		Computation of the error function value of vector elements
//klVector<double > klVSLErf(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdErf( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdErf( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdErfc		Computation of the complementary error function value of vector elements
//klVector<double > klVSLErfc(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdErfc( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdErfc( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
//klVector<double > klVSLCdfNorm(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdCdfNorm( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdCdfNorm( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdErfInv		Computation of the inverse error function value of vector elements
//klVector<double > klVSLErfInv(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdErfInv( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdErfInv( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdErfcInv		Computation of the inverse complementary error function value of vector elements
//klVector<double > klVSLErfcInv(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdErfcInv( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdErfcInv( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
//klVector<double > klVSLCdfNormInv(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdCdfNormInv( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdCdfNormInv( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
//klVector<double > klVSLLGamma(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdLGamma( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdLGamma( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdTGamma		Computation of the gamma function of vector elements 
//klVector<double > klVSLTGamma(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdTGamma( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdTGamma( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdFloor		Rounding towards minus infinity
//klVector<double > klVSLFloor(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdFloor( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdFloor( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdCeil		Rounding towards plus infinity
//klVector<double > klVSLCeil(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdCeil( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdCeil( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
//
////vdTrunc		Rounding towards zero infinity
//klVector<double > klVSLTrunc(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdTrunc( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdTrunc( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}
////vdRound		Rounding to nearest integer
//klVector<double > klVSLRound(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdRound( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdRound( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}

//vdFrac		Computation of the fractional part
//Must be new : 1>src\kl_vsl.cpp(1383): error C3861: 'vdFrac': identifier not found
//klVector<double > klVSLFract(klVector<double  > v,bool inplace=false)
//{
//	const MKL_INT n = v.getColumns();
//	
//	if(!inplace)
//	{
//		klVector<double > ans (v.getColumns() );		
//
//		vdFrac( n,  v.getMemory(),ans.getMemory());
//		return ans;
//	}
//	else
//	{
//		vdFrac( n, v.getMemory(), v.getMemory());
//		return v;
//	}
//}

//vdNearbyInt		Rounding according to current mode

//vdRint		Rounding according to current mode and raising inexact result exception

//vdModf		Computation of the integer and fractional parts
