 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
#ifndef __kl_principal_components__
#define __kl_principal_components__
#include "kl_sample_population.h"
#include "mkl.h"


//Calculates the best linear approximation of a data set provided by the 
//klsamplePopulation base class.  The data matrix X  \in is modeled
template<class TYPE> class klPrincipalComponents :public klSamplePopulation<TYPE>
{
public:

	klPrincipalComponents() : _pcaCalculated(false)
	{

	}

	klPrincipalComponents(const klSamplePopulation<TYPE>& src)
		:	klSamplePopulation<TYPE>(src), _pcaCalculated(false)// ,klMatrix<TYPE> (src)
	{

	}

	//Specializations exist for float and double.
	//This class should not be instantiated for any other type.
	klMatrix<TYPE> klPCACore()
	{
	}

	//Calculates the eigenvalues and eigenvectors of the covariance matrix for the sample population.
	klMatrix<TYPE> operator()()
	{
		return klPCACore();

	}
	//returns the eigenvalues of the covariance matrix
	klVector<TYPE> eigenvalues()
	{ 
		if(! _pcaCalculated)
		{	
			operator()();
		}
		return _eigenvalues;
	}

	unsigned int getk() { return _k;}
	
private:
	bool _pcaCalculated;

	klVector<TYPE> _eigenvalues;

};
template<> klMatrix<float> klPrincipalComponents<float>::klPCACore()
{klMatrix<float> _A=covarianceMatrix();

	/*                     mkl svd documentation
	call sgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	call dgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	
	This routine computes the singular value decomposition (SVD) of a
	real/complex m-by-n matrix A, optionally computing the left and/or right
	singular vectors. The SVD is written A = U S VH

	?gesvd Computes the singular value decomposition of a general rectangular matrix.
	?gesdd Computes the singular value decomposition of a general rectangular matrix using a divide and conquer method.
	?ggsvd Computes the generalized singular value decomposition of a pair of general rectangular matrices.

	where S is an m-by-n matrix which is zero except for its min(m,n) diagonal
	elements, U is an m-by-m orthogonal/unitary matrix, and V is an n-by-n
	orthogonal/unitary matrix. The diagonal elements of S are the singular
	values of A; they are real and non-negative, and are returned in descending
	order. The first min(m,n) columns of U and V are the left and right singular
	vectors of A. Note that the routine returns V^t, not V.
	Input Parameters

	*/
	int m=getRows();
	int n=getColumns();
	klMatrix<float> trA=transpose();
	int lda=getRows();
	float rcond=getConditionNumber();
	int min=(m<n? m :n);
	int max=(m>n? m :n);
	int max2=2*min>max ? 2*min : max;
	int lwork=10*max;//use 5 time the recommended workspace - MKL says be generous
	float* work=new float[lwork];
	int info=0;
	//options for computing U		
	char jobu='A';//  A=all are returned in u   S = min (m,n)  goto u     O=min(m,n) goto a    N=no signular values are computed
	char jobv='A';// A= all rows of V^t are computed and returned in vt

	klVector<float> S(n);
	klMatrix<float> U(m,m);
	klMatrix<float> V(n,n);

	int ldu=U.getColumns();
	int ldv=V.getColumns() ;
	sgesvd ( &jobu, &jobv, &m, &n, trA.getMemory(), &lda, S.getMemory(), U.getMemory(),&ldu , V.getMemory(),&ldv ,work, &lwork, &info);
	//dgesvd ( &jobu, &jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, &lwork, &info);

	//Store the eigenvalues of the covariance matrix in the member variable 

	//allocate the _eigenvlaues member variable
	_eigenvalues.setup(n);

	unsigned int i;
	for(i=0;i<n;i++)
		_eigenvalues[i]=S[i];

	U=U.transpose();
	V=V.transpose();
	delete work;
	_pcaCalculated=true;
	
	return V;

}

template<> klMatrix<double> klPrincipalComponents<double>::klPCACore()
{
	klMatrix<double> _A=covarianceMatrix();

	/*                     mkl svd documentation
	call sgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	call dgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)

	This routine computes the singular value decomposition (SVD) of a
	real/complex m-by-n matrix A, optionally computing the left and/or right
	singular vectors. The SVD is written A = U S VH

	?gesvd Computes the singular value decomposition of a general rectangular matrix.
	?gesdd Computes the singular value decomposition of a general rectangular matrix using a divide and conquer method.
	?ggsvd Computes the generalized singular value decomposition of a pair of general rectangular matrices.

	where S is an m-by-n matrix which is zero except for its min(m,n) diagonal
	elements, U is an m-by-m orthogonal/unitary matrix, and V is an n-by-n
	orthogonal/unitary matrix. The diagonal elements of S are the singular
	values of A; they are real and non-negative, and are returned in descending
	order. The first min(m,n) columns of U and V are the left and right singular
	vectors of A. Note that the routine returns V^t, not V.
	Input Parameters

	*/
	klMatrix<double> trA=_A.transpose();
	int m=trA.getRows();
	int n=trA.getColumns();
	int lda=trA.getRows();
	float rcond=getConditionNumber();
	int min=(m<n? m :n);
	int max=(m>n? m :n);
	int max2=2*min>max ? 2*min : max;
	int lwork=20*max;//use 5 time the recommended workspace - MKL says be generous
	double* work=new double[lwork];
	int info=0;
	//options for computing U		
	char jobu='A';//  A=all are returned in u   S = min (m,n)  goto u     O=min(m,n) goto a    N=no signular values are computed
	char jobv='A';// A= all rows of V^t are computed and returned in vt
		
	klVector<double> S(n);//S(m,n,FORTAN); 

	klMatrix<double> U(m,m);//
	klMatrix<double> V(n,n);

	int ldu=U.getColumns();
	int ldv=V.getColumns() ;
	dgesvd ( &jobu, &jobv, &m, &n, trA.getMemory(), &lda, S.getMemory(), U.getMemory(),&ldu , V.getMemory(),&ldv ,work, &lwork, &info);
	//dgesvd ( &jobu, &jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, &lwork, &info);

	//Store the eigenvalues of the covariance matrix in the member variable 

	//First allocate the _eigenvlaues member variable
	_eigenvalues.setup(n);

	unsigned int i;
	for(i=0;i<n;i++)
		_eigenvalues[i]=S[i];

	V=V.transpose();
	delete work;
	_pcaCalculated=true;
	return V;
}
#endif //__kl_principal_components__

