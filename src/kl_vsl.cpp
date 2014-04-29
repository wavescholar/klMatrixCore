#include "kl_vsl.h"

/*  Some VML Functions To Be Aware Of

VML Service Functions
vmlSetMode	Sets the VML mode
vmlGetMode	Gets the VML mode
MKLFreeTls	Frees allocated VML/VSL thread local storage memory from within DllMain routine (Windows* OS only)
vmlSetErrStatus	Sets the VML Error Status
vmlGetErrStatus	Gets the VML Error Status
vmlClearErrStatus	Clears the VML Error Status
vmlSetErrorCallBack	Sets the additional error handler callback function
vmlGetErrorCallBack	Gets the additional error handler callback function
vmlClearErrorCallBack	Deletes the additional error handler callback function
*/


/*Modes for the VSL
Values of the mode Parameter
Value of mode	Description
Accuracy Control
VML_HA	high accuracy versions of VML functions
VML_LA	low accuracy versions of VML functions
VML_EP	enhanced performance accuracy versions of VML functions
Denormalized Numbers Handling Control
VML_FTZDAZ_ON	Faster processing of denormalized inputs is enabled.
VML_FTZDAZ_OFF	Faster processing of denormalized inputs is disabled.
Error Mode Control
VML_ERRMODE_IGNORE	No action is set for computation errors.
VML_ERRMODE_ERRNO	On error, the errno variable is set.
VML_ERRMODE_STDERR	On error, the error text information is written to stderr.
VML_ERRMODE_EXCEPT	On error, an exception is raised.
VML_ERRMODE_CALLBACK	On error, an additional error handler function is called.
VML_ERRMODE_DEFAULT	On error, the errno variable is set, an exception is raised, and an additional error handler function is called.
*/



/*  Complex VSL Functions we  would like to support
Check vzAdd	s, d, c, z	Addition of vector elements
Check vzSub	s, d, c, z	Subtraction of vector elements
Check vzMul	s, d, c, z	Multiplication of vector elements
Check vzMulByConj	c, z	Multiplication of elements of one vector by conjugated elements of the second vector
Check vzConj	c, z	Conjugation of vector elements
Check vzAbs	s, d, c, z	Computation of the absolute value of vector elements
Check vzArg	c, z	Computation of the argument of vector elements
Check vzDiv	s, d, c, z	Division of elements of one vector by elements of the second vector
Check vzSqrt	s, d, c, z	Computation of the square root of vector elements
Check vzPow	s, d, c, z	Raising each vector element to the specified power
Check vzPowx	s, d, c, z	Raising each vector element to the constant power
Check vzExp	s, d, c, z	Computation of the exponential of vector elements
Check vzLn	s, d, c, z	Computation of the natural logarithm of vector elements
vzLog10	s, d, c, z	Computation of the denary logarithm of vector elements
vzCos	s, d, c, z	Computation of the cosine of vector elements
vzSin	s, d, c, z	Computation of the sine of vector elements
vzTan	s, d, c, z	Computation of the tangent of vector elements
vzAcos	s, d, c, z	Computation of the inverse cosine of vector elements
vzAsin	s, d, c, z	Computation of the inverse sine of vector elements
vzAtan	s, d, c, z	Computation of the inverse tangent of vector elements
vzCosh	s, d, c, z	Computation of the hyperbolic cosine of vector elements
vzSinh	s, d, c, z	Computation of the hyperbolic sine of vector elements
vzTanh	s, d, c, z	Computation of the hyperbolic tangent of vector elements
vzAcosh	s, d, c, z	Computation of the inverse hyperbolic cosine of vector elements
vzAsinh	s, d, c, z	Computation of the inverse hyperbolic sine of vector elements
vzAtanh	s, d, c, z	Computation of the inverse hyperbolic tangent of vector elements
*/

//vzAdd	Addition of vector elements
klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLAdd";
	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAdd( n,  v.getMemory(),b.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAdd( n, v.getMemory(),b.getMemory(), v.getMemory());
		return v;
	}
}

//vzSub	Subtraction of vector elements
klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLSub";
	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzSub( n,  v.getMemory(),b.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzSub( n, v.getMemory(),b.getMemory(), v.getMemory());
		return v;
	}
}

//vzMul	Multiplication of vector elements :  elementwise
klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMul";
	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzMul( n,  v.getMemory(),b.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzMul( n, v.getMemory(),b.getMemory(), v.getMemory());
		return v;
	}
}

//vzMul	Multiplication of vector by conjugate of secong arg 
klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLMulByConj";
	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzMulByConj( n,  v.getMemory(),b.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzMulByConj( n, v.getMemory(),b.getMemory(), v.getMemory());
		return v;
	}
}

//vzConj	Conjugation of vector elements
klVector<complex<double > > klVSLConj(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzConj( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzConj( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAbs	Computation of the absolute value of vector elements
klVector<double> klVSLAbs(klVector< complex<double > > v )
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	klVector<double> ans(n);		

	vzAbs( n,  v.getMemory(),ans.getMemory());
	return ans;
}

//vzArg	Computation of the argument of vector elements
klVector<double>  klVSLArg(klVector< complex<double > > v)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
		
	klVector<double> ans(n);		

	vzArg( n,  v.getMemory(),ans.getMemory());
	return ans;
}

//vzDiv	Division of elements of one vector by elements of the second vector
klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
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

	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzDiv( n,  v.getMemory(),b.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzDiv( n, v.getMemory(),b.getMemory(), v.getMemory());
		return v;
	}
}

//vzSqrt	Computation of the square root of vector elements
klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzSqrt( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzSqrt( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzPow	Raising each vector element to the specified power
klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace=false)
{
		vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );
	if(v.getColumns() != b.getColumns() )
		throw "Range Argument Exception in klVSLDiv";

	const MKL_INT n = v.getColumns();

	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzPow( n,  v.getMemory(),b.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzPow( n, v.getMemory(),b.getMemory(), v.getMemory());
		return v;
	}
}

//vzPowx	Raising each vector element to the constant power
klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x, bool inplace=false )
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzPowx( n,  v.getMemory(),x,ans.getMemory());
		return ans;
	}
	else
	{
		vzPowx( n, v.getMemory(),x, v.getMemory());
		return v;
	}
}

//vzExp	Computation of the exponential of vector elements
klVector<complex<double > > klVSLExp(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzExp( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzExp( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzLn	Computation of the natural logarithm of vector elements
klVector<complex<double > > klVSLLn(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzLn( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzLn( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzLog10	Computation of the denary logarithm of vector elements
klVector<complex<double > > klVSLLog10(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzLog10( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzLog10( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzCos	Computation of the cosine of vector elements
klVector<complex<double > > klVSLCos(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzCos( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzCos( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzSin	Computation of the sine of vector elements
klVector<complex<double > > klVSLSin(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzSin( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzSin( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzTan	Computation of the tangent of vector elements
klVector<complex<double > > klVSLTan(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzTan( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzTan( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAcos	Computation of the inverse cosine of vector elements
klVector<complex<double > > klVSLAcos(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAcos( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAcos( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAsin	Computation of the inverse sine of vector elements
klVector<complex<double > > klVSLAsin(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAsin( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAsin( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAtan	Computation of the inverse tangent of vector elements
klVector<complex<double > > klVSLAtan(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAtan( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAtan( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzCosh	Computation of the hyperbolic cosine of vector elements
klVector<complex<double > > klVSLCosh(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzCosh( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzCosh( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzSinh	Computation of the hyperbolic sine of vector elements
klVector<complex<double > > klVSLSinh(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzSinh( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzSinh( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzTanh	Computation of the hyperbolic tangent of vector elements
klVector<complex<double > > klVSLTanh(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzTanh( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzTanh( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAcosh( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAcosh( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAsinh	Computation of the inverse hyperbolic sine of vector elements
klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAsinh( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAsinh( n, v.getMemory(), v.getMemory());
		return v;
	}
}

//vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v, bool inplace=false)
{
	vmlSetMode( VML_LA | VML_FTZDAZ_ON | VML_ERRMODE_ERRNO );

	const MKL_INT n = v.getColumns();
	
	if(!inplace)
	{
		klVector<complex<double>  > ans (v.getColumns() );		

		vzAtanh( n,  v.getMemory(),ans.getMemory());
		return ans;
	}
	else
	{
		vzAtanh( n, v.getMemory(), v.getMemory());
		return v;
	}
}



/* Double Precision VSL Functions we would like to support

vdAdd	s, d, c, z	Addition of vector elements
vdSub	s, d, c, z	Subtraction of vector elements
vdSqr	s, d	Squaring of vector elements
vdMul	s, d, c, z	Multiplication of vector elements
vdMulByConj	c, z	Multiplication of elements of one vector by conjugated elements of the second vector
vdConj	c, z	Conjugation of vector elements
vdAbs	s, d, c, z	Computation of the absolute value of vector elements
vdArg	c, z	Computation of the argument of vector elements
vdLinearFrac	s, d	Linear fraction transformation of vectors
vdInv	s, d	Inversion of vector elements
vdDiv	s, d, c, z	Division of elements of one vector by elements of the second vector
vdSqrt	s, d, c, z	Computation of the square root of vector elements
vdInvSqrt	s, d	Computation of the inverse square root of vector elements
vdCbrt	s, d	Computation of the cube root of vector elements
vdInvCbrt	s, d	Computation of the inverse cube root of vector elements
vdPow2o3	s, d	Raising each vector element to the power of 2/3
vdPow3o2	s, d	Raising each vector element to the power of 3/2
vdPow	s, d, c, z	Raising each vector element to the specified power
vdPowx	s, d, c, z	Raising each vector element to the constant power
vdHypot	s, d	Computation of the square root of sum of squares
vdExp	s, d, c, z	Computation of the exponential of vector elements
vdExpm1	s, d	Computation of the exponential of vector elements decreased by 1
vdLn	s, d, c, z	Computation of the natural logarithm of vector elements
vdLog10	s, d, c, z	Computation of the denary logarithm of vector elements
vdLog1p	s, d	Computation of the natural logarithm of vector elements that are increased by 1
vdCos	s, d, c, z	Computation of the cosine of vector elements
vdSin	s, d, c, z	Computation of the sine of vector elements
vdSinCos	s, d	Computation of the sine and cosine of vector elements
vdCIS	c, z	Computation of the complex exponent of vector elements (cosine and sine combined to complex value)
vdTan	s, d, c, z	Computation of the tangent of vector elements
vdAcos	s, d, c, z	Computation of the inverse cosine of vector elements
vdAsin	s, d, c, z	Computation of the inverse sine of vector elements
vdAtan	s, d, c, z	Computation of the inverse tangent of vector elements
vdAtan2	s, d	Computation of the four-quadrant inverse tangent of elements of two vectors
vdCosh	s, d, c, z	Computation of the hyperbolic cosine of vector elements
vdSinh	s, d, c, z	Computation of the hyperbolic sine of vector elements
vdTanh	s, d, c, z	Computation of the hyperbolic tangent of vector elements
vdAcosh	s, d, c, z	Computation of the inverse hyperbolic cosine of vector elements
vdAsinh	s, d, c, z	Computation of the inverse hyperbolic sine of vector elements
vdAtanh	s, d, c, z	Computation of the inverse hyperbolic tangent of vector elements.
vdErf	s, d	Computation of the error function value of vector elements
vdErfc	s, d	Computation of the complementary error function value of vector elements
vdCdfNorm	s, d	Computation of the cumulative normal distribution function value of vector elements
vdErfInv	s, d	Computation of the inverse error function value of vector elements
vdErfcInv	s, d	Computation of the inverse complementary error function value of vector elements
vdCdfNormInv	s, d	Computation of the inverse cumulative normal distribution function value of vector elements
vdLGamma	s, d	Computation of the natural logarithm for the absolute value of the gamma function of vector elements
vdTGamma	s, d	Computation of the gamma function of vector elements 
vdFloor	s, d	Rounding towards minus infinity
vdCeil	s, d	Rounding towards plus infinity
vdTrunc	s, d	Rounding towards zero infinity
vdRound	s, d	Rounding to nearest integer
vdNearbyInt	s, d	Rounding according to current mode
vdRint	s, d	Rounding according to current mode and raising inexact result exception
vdModf	s, d	Computation of the integer and fractional parts
vdFrac	s, d	Computation of the fractional part
*/