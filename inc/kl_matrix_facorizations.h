 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/
#ifndef __kl_linear_equation__
#define __kl_linear_equation__

#include "mkl.h"
#include "kl_matrix.h"
template<class TYPE> class klSVD
{
private :
	bool _resultsCalculated;
	klSmartPtr<klMatrix<TYPE> > _A;
	klSmartPtr<klMatrix<TYPE> > _U;
	klSmartPtr<klMatrix<TYPE> > _V;
	klSmartPtr<klVector<TYPE> > _Sigma;

public:	
	klSVD(klMatrix<TYPE>& A) : _resultsCalculated(false)
	{
		_A = new klMatrix<TYPE>(A);
	}

	klSmartPtr<klVector<TYPE> > operator()()
	{
		return new klVector<TYPE>(0,0);
	}

	klSmartPtr<klMatrix<TYPE> > U()
	{
		if(_resultsCalculated==true)
			return _U;
	}

	klSmartPtr<klMatrix<TYPE> > V()
	{	
		if(_resultsCalculated==true)
			return _V;
	}

};

template<> klSmartPtr<klVector<float> > klSVD<float>::operator()()
{
	//Note that the routine returns vt = V' (for real flavors) or vt = conjg(V') (for complex flavors), not V.

	//If jobz = 'A', all m columns of U and all n rows of V'/conjg(V') are returned in the arrays u and vt; 
	//if jobz = 'O', then 
		//if m ?  n, the first n columns of U are overwritten in the array a and all rows of V'/conjg(V') are returned in the array vt; 
		//if m < n, all columns of U are returned in the array u and the first m rows of V'/conjg(V') are overwritten in the array a;
	char jobz='A';
	int m=_A->getRows();
	int n=_A->getColumns();

	//If jobz = 'O', then if m? n, a is overwritten with the first n columns of U 
	//(the left singular vectors, stored columnwise). 
	//If m < n, a is overwritten with the first m rows of VT (the right singular vectors, stored rowwise);
	//If jobz?'O', the contents of a are destroyed.
	
	float* a = _A->transpose().getMemory();  //BBC revisit we need to pass in transpose!

	int lda= _A->getRows();

	_U = new klMatrix<float>(m,m);
	_V = new  klMatrix<float>(n,n);
	_Sigma = new  klVector<float>(min(m,n));
	float* s=_Sigma->getMemory();

	//u(ldu,*); the second dimension of u must be at least max(1, m)if jobz = 'A' or jobz = 'O' and m < n. 
	//If jobz = 'A'or jobz = 'O' and m < n, u contains the m-by-m orthogonal/unitary matrix U.

	//vt(ldvt,*); the second dimension of vt must be at least max(1, n).
	//If jobz = 'O' and m < n, or jobz = 'N', vt is not referenced.

	float* u=_U->getMemory();
	float* vt=_V->getMemory();


	int ldu = m;
	int ldvt = n;
	int minv=(m<n? m :n);
	int maxv=(m>n? m :n);

	MKL_INT lwork =  5 *   3*minv* 2 + maxv *maxv * 4*2 + 4;
	float* work= new float[lwork];
	int iworkSize = max(1, 8*min(m, n));
	MKL_INT* iwork = new int[iworkSize];
	MKL_INT info;
	sgesdd( &jobz,  &m,  &n,  a,  &lda,  s,  u,  &ldu,  vt,  &ldvt,  work,  &lwork,  iwork,  &info );

	_resultsCalculated = true;
	return _Sigma;
}

template<> klSmartPtr<klVector<double> > klSVD<double>::operator()()
{
	char jobz='A';
	int m=_A->getRows();
	int n=_A->getColumns();
	double* a = _A->getMemory();
	int lda= _A->getRows();

	_U = new klMatrix<double>(m,m);
	_V =new  klMatrix<double>(n,n);

	//_Sigma =Array, DIMENSION at least max(1, min(m,n)). 
	//Contains the singular values of A sorted so that s(i) ? s(i+1).

	_Sigma = new  klVector<double>(min(m,n));
	double* s=_Sigma->getMemory();
	double* u=_U->getMemory();
	double* vt=_V->getMemory();
	int ldu = m;
	int ldvt = n;
	int minv=(m<n? m :n);
	int maxv=(m>n? m :n);

	MKL_INT lwork =  5 *   3*minv* 2 + maxv *maxv * 4*2 + 4;
	double* work= new double[lwork];
	int iworkSize = max(1, 8*min(m, n));
	MKL_INT* iwork = new int[iworkSize];
	MKL_INT info;
	dgesdd( &jobz,  &m,  &n,  a,  &lda,  s,  u,  &ldu,  vt,  &ldvt,  work,  &lwork,  iwork,  &info );

	_resultsCalculated = true;
	return _Sigma;

}

template<class TYPE> class klLU
{
private :
	bool _resultsCalculated;
	klSmartPtr<klMatrix<TYPE> > _A;
	klMatrix<TYPE>  _U;
	klMatrix<TYPE>  _L;

public:	
	klLU(klMatrix<TYPE>& A) : _resultsCalculated(false)
	{
		_A = new klMatrix<TYPE>(A);
	}

	klMatrix<TYPE>  operator()()
	{
		return klMatrix<TYPE>(0,0);
	}


	klMatrix<TYPE>  U()
	{
		if(_resultsCalculated==true)
			return _U;
	}

	klMatrix<TYPE> L()
	{	
		if(_resultsCalculated==true)
			return _L;
	}

};


template<> klMatrix<double> klLU<double>::operator()()
{
	//transpose to get into FORTRAN column major storage format
	klMatrix<double> tr=_A->transpose();
	//allocate memory for returned pivot indices
	size_t size=_A->getRows()  >_A->getColumns() ? _A->getRows() : _A->getColumns();
	int* ipiv=new int[2*size];
	int info=0;
	int n=_A->getRows();
	int m=_A->getColumns();
	dgetrf(&n,&m,tr.getMemory(),&n,ipiv,&info);
	//tr is now LU factored
	 
	_U = tr.upper();
	_L = tr.lower();
	for(int i=0;i<min(n,m);i++)
	{
		_L[i][i] = 1.0f;  //From MKL doc : The unit diagonal elements of L are not stored.
	}
	_resultsCalculated=true;
	return _U;
}


//This computes all eigenvalues and, optionally, eigenvectors of an n-by-n real symmetric matrix A
template<class TYPE> class klSYEVX
{
private :
	bool _resultsCalculated;
	klSmartPtr<klMatrix<TYPE> > _A;
	klSmartPtr<klMatrix<TYPE> > _B;
	klSmartPtr<klVector<TYPE> > _Sigma;

public:	
	klSYEVX(klMatrix<TYPE>& A) : _resultsCalculated(false)
	{
		if( A.getRows() != A.getColumns())
		{
			throw klError("klSYEVX(klMatrix<TYPE>& A,unsigned int numEigen) bad dimension.  Matrix must be square");
		}
		_A = new klMatrix<TYPE>(A);
	}

	klSmartPtr<klMatrix<TYPE> > Eigenvectors()
	{
		if (_resultsCalculated)
		{
			return _B;
		}
		else 
		{
			 operator()();
			 return _B;
		}

	}

	klSmartPtr<klVector<TYPE> > operator()()
	{
		return new klVector<TYPE>(0,0);
	}
};

#include "mkl_lapacke.h"
template<> klSmartPtr<klVector<double> > klSYEVX<double>::operator()()
{
	int info;

	int m=_A->getRows();

	int n=_A->getColumns();

    //Make a deep copy of _A.  The call will replace the matrix with the basis of eigenvectors
	_B= new klMatrix<double>(*(_A.ptr() )) ;

	double* a = _A->getMemory();

	double* b = _B->getMemory();

	int lda= _A->getRows();

	_Sigma = new  klVector<double>(n);

	double* s=_Sigma->getMemory();

	info = LAPACKE_dsyev( LAPACK_ROW_MAJOR, 'V', 'U', n, b, lda, s );

	_resultsCalculated = true;
		
	return _Sigma;
}

//This computes all eigenvalues and, optionally, eigenvectors of an n-by-n real symmetric matrix A
//Intel® MKL 11.0 Update 2 introduced a new component called Extended Eigensolver routines. 
//These routines solve standard and generalized Eigenvalue problems for symmetric/Hermitian and symmetric/Hermitian positive definite sparse matrices. Specifically, these routines computes all the Eigenvalues and the corresponding Eigenvectors within a given search interval [?min, ?max]:
//$$Ax = ?x$$ and
//$$Ax = ?Bx$$
//where A is a real symmetric or complex Hermitian matrix and B is a real symmetric positive definite or complex Hermitian positive definite matrix. Eigenvalue problems for dense and band matrices are also supported, although solvers for these matrices have long existed in LAPACK. The Extended Eigensolver is intended to complement the Eigenvalue solvers in LAPACK, which lacks support for sparse matrices. 
//The Extended Eigensolver is an implementation of the FEAST Eigenvalue Solver. The interfaces are compatible with FEAST Eigenvalue Solver v2.0. The FEAST algorithm is fundamentally different than the traditional Krylov subspace iteration based algorithms. It uses a numerically efficient contour integration technique to obtain Eigenpairs within the search interval. It has advantages such as the ability to capture all multiplicities, fast convergence, being insensitive to spectrum structures, and so on.
template<class TYPE> class klFEATS
{
private :

	bool _resultsCalculated;
	klSmartPtr<klMatrix<TYPE> > _A;
	unsigned int _numEigen;
	klSmartPtr<klMatrix<TYPE> > _B;
	klSmartPtr<klVector<TYPE> > _Sigma;

public:	
	klFEATS(klMatrix<TYPE>& A,unsigned int numEigen) : _resultsCalculated(false)
	{
		if( A.getRows() != A.getColumns())
		{
			throw klError("klFEATS(klMatrix<TYPE>& A,unsigned int numEigen) bad dimension.  Matrix must be square");
		}
		_A = new klMatrix<TYPE>(A);
	}

	klSmartPtr<klMatrix<TYPE> > Eigenvectors()
	{
		if (_resultsCalculated)
		{
			return _B;
		}
		else 
		{
			 operator()();
			 return _B;
		}
	}

	klSmartPtr<klVector<TYPE> > operator()()
	{
		return new klVector<TYPE>(0,0);
	}
};

#include "mkl_lapacke.h"
template<> klSmartPtr<klVector<double> > klFEATS<double>::operator()()
{
	int n=_A->getColumns();

    //Make a deep copy of _A.  The call will replace the matrix with the basis of eigenvectors
	_B= new klMatrix<double>(*(_A.ptr() )) ;

	double* a = _A->getMemory();

	int lda= _A->getRows();

	//Must be 'U' or 'L' or 'F' .If uplo = 'U', a stores the upper triangular parts of A. If uplo = 'L', a stores the lower triangular parts of A. If uplo= 'F' , a stores the full matrix A.
	const char uplo = 'F';

	int* fpm = new int[128];
	//fpm INTEGER Array, dimension of 128. This array is used to pass various parameters to Extended Eigensolver routines. See Extended Eigensolver Input Parameters for a complete description of the parameters and their default values.
	
	double epsout; 
	int loop;

	double emin = -1.7976931348623157e+307;

	double emax = +1.7976931348623157e+307;

	int m0 =n;//On entry, specifies the initial guess for subspace dimension to be used. m0 ? m where m is the total number of eigenvalues located in the interval [emin, emax]. If the initial guess is wrong, Extended Eigensolver routines return info=3
	
	double * e= new double[n];

	double * x = new double[n*n];

	int m ; //The total number of eigenvalues found in the interval [emin, emax]: 0 ? m ? m0.

	double* res = new double[m0];//DOUBLE PRECISION for dfeast_syev and zfeast_heev Array of length m0. On exit, the first m components contain the relative residual vector

	int info;
	//const char * uplo, const MKL_INT * n, const double * a, const MKL_INT * lda, MKL_INT * fpm, double * epsout, MKL_INT * loop, const double * emin, const double * emax, MKL_INT * m0, double * e, double * x, MKL_INT * m, double * res, MKL_INT * info
	//dfeast_syev ( &uplo, &n, a, &lda, fpm, & epsout, &loop,&emin, & emax, & m0,  e,  x,  &m, res, &info);
	
	_resultsCalculated = true;
		
	return _Sigma;
}

#endif