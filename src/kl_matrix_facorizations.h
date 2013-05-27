 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
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

template<class TYPE> class klSYEVX
{
private :
	bool _resultsCalculated;
	klSmartPtr<klMatrix<TYPE> > _A;
	unsigned int _numEigen;


	klSmartPtr<klMatrix<TYPE> > _U;
	klSmartPtr<klMatrix<TYPE> > _V;
	klSmartPtr<klVector<TYPE> > _Sigma;

public:	
	klSYEVX(klMatrix<TYPE>& A,unsigned int numEigen) : _resultsCalculated(false)
	{
		_A = new klMatrix<TYPE>(A);
	}

	klSmartPtr<klVector<TYPE> > operator()()
	{
		return new klVector<TYPE>(0,0);
	}

	
};

//BBC TODO 052013
template<> klSmartPtr<klVector<float> > klSYEVX<float>::operator()()
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

template<> klSmartPtr<klVector<double> > klSYEVX<double>::operator()()
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
	
	{
		const char jobz=' ';
		const char range=' ';
		const char uplo=' ';
		const MKL_INT n=0;
		double* a;
		const MKL_INT lda=0;
		const double vl=0.0f;
		const double vu=0.0f;
		const MKL_INT il=0;
		const MKL_INT iu=0;
		const double abstol=0.0f;
		MKL_INT* m;
		double* w;
		double* z;
		const MKL_INT ldz=0;
		double* work;
		const MKL_INT lwork=0;
		MKL_INT* iwork;
		MKL_INT* ifail;
		MKL_INT* info;
		dsyevx( &jobz, &range, &uplo,
			&n, a, &lda,&vl,
			&vu, &il, &iu,
			&abstol, m,  w, z,
			&ldz,  work, &lwork,
			iwork,ifail,  info );
	}

	return _Sigma;
}


#endif