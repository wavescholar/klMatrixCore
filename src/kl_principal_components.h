 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
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

	klPrincipalComponents() : _pcaCalculated(false), k(0)
	{

	}

	klPrincipalComponents(const klSamplePopulation<TYPE>& src)
		:	klSamplePopulation<TYPE>(src), _pcaCalculated(false)// ,klMatrix<TYPE> (src)
	{

	}

	//Specializations exist for float and double.
	//This class should not be instantiated for any other type.
	klMatrix<TYPE> klPCACore(unsigned int k)
	{
	}


	//Calculates the eigenvalues and eigenvectors of the covariance matrix for the sample population.
	//bbcrevisit - this method needs to be factored and or specialized
	klMatrix<TYPE> operator()(unsigned int k)
	{
		return klPCACore(k);

	}
	//returns the eigenvalues of the covariance matrix
	klVector<TYPE> eigenvalues()
	{ 
		if(! _pcaCalculated)
		{
			if(_k==0 || _k>getColumns() )
				throw "klPrincipalComponents ERROR:  too many components for this data matrix";

			operator()(_k);
		}
		return _eigenvalues;
	}


	unsigned int getk() { return _k;}



private:
	bool _pcaCalculated;

	unsigned int _k;

	klVector<TYPE> _eigenvalues;

};
template<> klMatrix<float> klPrincipalComponents<float>::klPCACore(unsigned int k)
{

	_k=k;//bbcrevisit we are not using k yet
	klMatrix<float> _A=covarianceMatrix();

	/*                     mkl svd documentation
	call sgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	call dgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	call cgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, rwork, info)
	call zgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, rwork, info)

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

	klMatrix<float> S(n,m);//S(m,n,FORTAN); 
	klMatrix<float> U(m,m);//
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
		_eigenvalues[i]=S[i][i];

	U=U.transpose();
	V=V.transpose();
	delete work;
	_pcaCalculated=true;

	//Apply the transform to the sample poplulation and return 

	return V;

}

template<> klMatrix<double> klPrincipalComponents<double>::klPCACore(unsigned int k)
{
	_k=k;
	klMatrix<double> _A=covarianceMatrix();

	/*                     mkl svd documentation
	call sgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	call dgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, info)
	call cgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, rwork, info)
	call zgesvd ( jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, lwork, rwork, info)

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

	klMatrix<double> S(n,m);//S(m,n,FORTAN); 
	klMatrix<double> U(m,m);//
	klMatrix<double> V(n,n);

	int ldu=U.getColumns();
	int ldv=V.getColumns() ;
	dgesvd ( &jobu, &jobv, &m, &n, trA.getMemory(), &lda, S.getMemory(), U.getMemory(),&ldu , V.getMemory(),&ldv ,work, &lwork, &info);
	//dgesvd ( &jobu, &jobvt, m, n, a, lda, s, u, ldu, vt, ldvt,work, &lwork, &info);

	//unnecessary since s is diag
	//klMatrix<float> trS=S.transpose();

	//Store the eigenvalues of the covariance matrix in the member variable 

	//First allocate the _eigenvlaues member variable
	_eigenvalues.setup(n);

	unsigned int i;
	for(i=0;i<n;i++)
		_eigenvalues[i]=S[i][i];

	//U=U.transpose();//bbcrevisit see what this means in term of the copy constructors
	V=V.transpose();
	delete work;
	_pcaCalculated=true;
	return V;
}
#endif //__kl_principal_components__

