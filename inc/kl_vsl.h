#ifndef __kl_vsl__
#define __kl_vsl__


#include "kl_vector.h"
/*
Using Complex Types in C/C++
As described in the documentation for the Intel® Fortran Compiler XE, C/C++ does not directly implement the 
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

//vdln Take ln of elements

klVector<double> klVSLLog(klVector<double> v, bool inplace);

//vzAdd	Addition of vector elements
klVector<complex<double > > klVSLAdd(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);

//vzSub	Subtraction of vector elements
klVector<complex<double > > klVSLSub(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);

//vzMul	Multiplication of vector elements :  elementwise
klVector<complex<double > > klVSLMul(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);

//vzMul	Multiplication of vector by conjugate of secong arg 
klVector<complex<double > > klVSLMulByConj(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);

//vzConj	Conjugation of vector elements
klVector<complex<double > > klVSLConj(klVector< complex<double > > v, bool inplace);

//vzAbs	Computation of the absolute value of vector elements
klVector<double> klVSLAbs(klVector< complex<double > > v );

//vzArg	Computation of the argument of vector elements
klVector<double> klVSLArg(klVector< complex<double > > v);

//vzDiv	Division of elements of one vector by elements of the second vector
klVector<complex<double > > klVSLDiv(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);
//vzSqrt	Computation of the square root of vector elements
klVector<complex<double > > klVSLSqrt(klVector< complex<double > > v, bool inplace);

//vzPow	Raising each vector element to the specified power
klVector<complex<double > > klVSLPow(klVector< complex<double > > v,klVector< complex<double> > b, bool inplace);

//vzPowx	Raising each vector element to the constant power
klVector<complex<double > > klVSLPowX(klVector< complex<double > > v,complex< double> x, bool inplace );

//vzExp	Computation of the exponential of vector elements
klVector<complex<double > > klVSLExp(klVector< complex<double > > v, bool inplace);

//vzLn	Computation of the natural logarithm of vector elements
klVector<complex<double > > klVSLLn(klVector< complex<double > > v, bool inplace);

//vzLog10	Computation of the denary logarithm of vector elements
klVector<complex<double > > klVSLLog10(klVector< complex<double > > v, bool inplace);

//vzCos	Computation of the cosine of vector elements
klVector<complex<double > > klVSLCos(klVector< complex<double > > v, bool inplace);

//vzSin	Computation of the sine of vector elements
klVector<complex<double > > klVSLSin(klVector< complex<double > > v, bool inplace);

//vzTan	Computation of the tangent of vector elements
klVector<complex<double > > klVSLTan(klVector< complex<double > > v, bool inplace);

//vzAcos	Computation of the inverse cosine of vector elements
klVector<complex<double > > klVSLAcos(klVector< complex<double > > v, bool inplace);

//vzAsin	Computation of the inverse sine of vector elements
klVector<complex<double > > klVSLAsin(klVector< complex<double > > v, bool inplace);

//vzAtan	Computation of the inverse tangent of vector elements
klVector<complex<double > > klVSLAtan(klVector< complex<double > > v, bool inplace);

//vzCosh	Computation of the hyperbolic cosine of vector elements
klVector<complex<double > > klVSLCosh(klVector< complex<double > > v, bool inplace);

//vzSinh	Computation of the hyperbolic sine of vector elements
klVector<complex<double > > klVSLSinh(klVector< complex<double > > v, bool inplace);

//vzTanh	Computation of the hyperbolic tangent of vector elements
klVector<complex<double > > klVSLTanh(klVector< complex<double > > v, bool inplace);

//vzAcosh	Computation of the inverse hyperbolic cosine of vector elements
klVector<complex<double > > klVSLAcosh(klVector< complex<double > > v, bool inplace);

//vzAsinh	Computation of the inverse hyperbolic sine of vector elements
klVector<complex<double > > klVSLAsinh(klVector< complex<double > > v, bool inplace);

//vzAtanh	Computation of the inverse hyperbolic tangent of vector elements
klVector<complex<double > > klVSLAtanh(klVector< complex<double > > v, bool inplace);


#endif