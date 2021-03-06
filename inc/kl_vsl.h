#ifndef __kl_vsl__
#define __kl_vsl__


#include "kl_vector.h"
/*
Using Complex Types in C/C++
As described in the documentation for the Intel� Fortran Compiler XE, C/C++ does not directly implement the 
Fortran types COMPLEX(4) and COMPLEX(8). However, you can write equivalent structures. The type COMPLEX(4)
consists of two 4-byte floating-point numbers. The first of them is the real-number component, and the 
second one is the imaginary-number component. The type COMPLEX(8) is similar to COMPLEX(4) except that it
contains two 8-byte floating-point numbers.

Intel MKL provides complex types MKL_Complex8 and MKL_Complex16, which are structures equivalent to the 
Fortran complex types COMPLEX(4) and COMPLEX(8), respectively. The MKL_Complex8 and MKL_Complex16 types 
are defined in the mkl_types.h header file. You can use these types to define complex data. You can also 
redefine the types with your own types before including the mkl_types.h header file. The only requirement 
is that the types must be compatible with the Fortran complex layout, that is, the complex type must be a 
pair of real numbers for the values of real and imaginary parts.  */



//vzAdd	Addition of vector elements
void klVSLAdd(klVector< complex<double > >& v,klVector< complex<double> >& b, klVector< complex<double > >& ans);

//vzSub	Subtraction of vector elements
void klVSLSub(klVector< complex<double > >& v,klVector< complex<double > >& b, klVector< complex<double > >& ans);

//vzMul	Multiplication of vector elements :  elementwise
void klVSLMul(klVector< complex<double > >& v,klVector< complex<double > >& b, klVector< complex<double > >& ans);

//vzMul	Multiplication of vector by conjugate of second arg 
void klVSLMulByConj(klVector< complex<double > >& v,klVector< complex<double > >& b, klVector< complex<double > >& ans);

//vzConj	Conjugation of vector elements
void klVSLConj(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAbs	Computation of the absolute value of vector elements
void klVSLAbs(klVector< complex<double > >& v ,klVector<double>& ans);

//vzArg	Computation of the argument of vector elements
void klVSLArg(klVector< complex<double > >& v, klVector<double>& ans );

//vzDiv	Division of elements of one vector by elements of the second vector
void klVSLDiv(klVector< complex<double > >& v,klVector< complex<double > >& b, klVector< complex<double > >& ans);
//vzSqrt	Computation of the square root of vector elements
void klVSLSqrt(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzPow	Raising each vector element to the specified power
void klVSLPow(klVector< complex<double > >& v,klVector< complex<double > >& b, klVector< complex<double > >& ans);

//vzPowx	Raising each vector element to the constant power
void klVSLPowX(klVector< complex<double > >& v,complex< double> x, klVector< complex<double > >& ans );

//vzExp	Computation of the exponential of vector elements
void klVSLExp(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzLn	Computation of the natural logarithm of vector elements
void klVSLLn(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzLog10	Computation of the denary logarithm of vector elements
void klVSLLog10(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzCos	Computation of the cosine of vector elements
void klVSLCos(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzSin	Computation of the sine of vector elements
void klVSLSin(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzTan	Computation of the tangent of vector elements
void klVSLTan(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAcos	Computation of the inverse cosine of vector elements
void klVSLAcos(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAsin	Computation of the inverse sine of vector elements
void klVSLAsin(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAtan	Computation of the inverse tangent of vector elements
void klVSLAtan(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzCosh	Computation of the hyperbolic cosine of vector elements
void klVSLCosh(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzSinh	Computation of the hyperbolic sine of vector elements
void klVSLSinh(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzTanh	Computation of the hyperbolic tangent of vector elements
void klVSLTanh(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
void klVSLAcosh(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAsinh	Computation of the inverse hyperbolic sine of vector elements
void klVSLAsinh(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
void klVSLAtanh(klVector< complex<double > >& v, klVector< complex<double > >& ans);

//----------------------double precision methods---------------------------------

//vdAdd	Addition of vector elements
void klVSLAdd(klVector<double>&  v,klVector<double>&  b, klVector<double>& ans);

//vdSub		Subtraction of vector elements
void klVSLSub(klVector<double>&  v,klVector<double>&  b, klVector<double>& ans);

//vdMul		Multiplication of vector elements
void klVSLMul(klVector<double>&  v,klVector<double>&  b, klVector<double>& ans);

//vdDiv		Division of elements of one vector by elements of the second vector
void klVSLDiv(klVector<double>&  v,klVector<double>&  b, klVector<double>& ans);

//vdHypot		Computation of the square root of sum of squares
void klVSLHypot(klVector<double>&  v,klVector<double>&  b,klVector<double>& ans);

//vdPow		Raising each vector element to the specified power
void klVSLPow(klVector<double>&  v,klVector<double>&  b, klVector<double>& ans);

//vdPowx		Raising each vector element to the constant power
void klVSLPowx(klVector<double>&  v,double x,klVector<double>& ans);

//vdSqr		Squaring of vector elements
void klVSLSqr(klVector<double>&  v,klVector<double>& ans);

//vdAbs		Computation of the absolute value of vector elements
void klVSLAbs(klVector<double>&  v,klVector<double>& ans);

//vdInv		Inversion of vector elements
void klVSLInv(klVector<double>&  v,klVector<double>& ans);

//vdSqrt Computation of the square root of vector elements
void klVSLSqrt(klVector<double>&  v,klVector<double>& ans);

//vdExp	Computation of the exponential of vector elements
void klVSLExp(klVector<double>&  v,klVector<double>& ans);

//vdExpm1		Computation of the exponential of vector elements decreased by 1
void klVSLExpm1(klVector<double>&  v,klVector<double>& ans);

//vdLn	Computation of the natural logarithm of vector elements
void klVSLLn(klVector<double>&  v,klVector<double>& ans);

//vdLog10		Computation of the denary logarithm of vector elements
void klVSLLog10(klVector<double>&  v,klVector<double>& ans);

//vdCos		Computation of the cosine of vector elements
void klVSLCos(klVector<double>&  v,klVector<double>& ans);

//vdSin		Computation of the sine of vector elements
void klVSLSin(klVector<double>&  v,klVector<double>& ans);

//vdTan		Computation of the tangent of vector elements
void klVSLTan(klVector<double>&  v,klVector<double>& ans);

//vdAcos		Computation of the inverse cosine of vector elements
void klVSLAcos(klVector<double>&  v,klVector<double>& ans);

//vdAsin		Computation of the inverse sine of vector elements
void klVSLAsin(klVector<double>&  v,klVector<double>& ans);

//vdAtan		Computation of the inverse tangent of vector elements
void klVSLAtan(klVector<double>&  v,klVector<double>& ans);

//vdCosh		Computation of the hyperbolic cosine of vector elements
void klVSLCosh(klVector<double>&  v,klVector<double>& ans);

//vdSinh		Computation of the hyperbolic sine of vector elements
void klVSLSinh(klVector<double>&  v,klVector<double>& ans);

//vdTanh		Computation of the hyperbolic tangent of vector elements
void klVSLTanh(klVector<double>&  v,klVector<double>& ans);

//vdAcosh		Computation of the inverse hyperbolic cosine of vector elements
void klVSLAcosh(klVector<double>&  v,klVector<double>& ans);

//vdAsinh		Computation of the inverse hyperbolic sine of vector elements
void klVSLAsinh(klVector<double>&  v,klVector<double>& ans);

//vdAtanh		Computation of the inverse hyperbolic tangent of vector elements
void klVSLAtanh(klVector<double>&  v,klVector<double>& ans);

//vdErf		Computation of the error function value of vector elements
void klVSLErf(klVector<double>&  v,klVector<double>& ans);

//vdErfc		Computation of the complementary error function value of vector elements
void klVSLErfc(klVector<double>&  v,klVector<double>& ans);

//vdCdfNorm		Computation of the cumulative normal distribution function value of vector elements
void klVSLCdfNorm(klVector<double>&  v,klVector<double>& ans);

//vdErfInv		Computation of the inverse error function value of vector elements
void klVSLErfInv(klVector<double>&  v,klVector<double>& ans);

//vdErfcInv		Computation of the inverse complementary error function value of vector elements
void klVSLErfcInv(klVector<double>&  v,klVector<double>& ans);

//vdCdfNormInv		Computation of the inverse cumulative normal distribution function value of vector elements
void klVSLCdfNormInv(klVector<double>&  v,klVector<double>& ans);

//vdLGamma		Computation of the natural logarithm for the absolute value of the gamma function of vector elements
void klVSLLGamma(klVector<double>&  v,klVector<double>& ans);

//vdTGamma		Computation of the gamma function of vector elements 
void klVSLTGamma(klVector<double>&  v,klVector<double>& ans);

//vdFloor		Rounding towards minus infinity
void klVSLFloor(klVector<double>&  v,klVector<double>& ans);

//vdCeil		Rounding towards plus infinity
void klVSLCeil(klVector<double>&  v,klVector<double>& ans);

//vdTrunc		Rounding towards zero infinity
void klVSLTrunc(klVector<double>&  v,klVector<double>& ans);

//vdRound		Rounding to nearest integer
void klVSLRound(klVector<double>&  v,klVector<double>& ans);

//vdFrac		Computation of the fractional part
//Must be new : 1>src\kl_vsl.cpp(1383): error C3861: '


////enum klVSLZTypes{
////	klVSLAdd,
////	klVSLSub,
////	klVSLMul,
////	klVSLMulByConj,
////	klVSLConj,
////	klVSLAbs,
////	klVSLArg,
////	klVSLDiv,
////	klVSLSqrt,
////	klVSLPow,
////	klVSLPowx,
////	klVSLExp,
////	klVSLLn,
////	klVSLLog10,
////	klVSLCos,
////	klVSLSin,
////	klVSLTan,
////	klVSLAcos,
////	klVSLAsin,
////	klVSLAtan,
////	klVSLCosh,
////	klVSLSinh,
////	klVSLTanh,
////	klVSLAcosh,
////	klVSLAsinh,
////	klVSLAtanh	};

//
////vdFrac		Computation of the fractional part
////Must be new : 1>src\kl_vsl.cpp(1383): error C3861: 'vdFrac': identifier not found
////klVector<double > klVSLFract(klVector<double  > v,bool inplace);

#endif