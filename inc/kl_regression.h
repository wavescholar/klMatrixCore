 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#ifndef __kl_regression__
#define __kl_regression__
#include "kl_matrix.h"

#include "mkl.h"

template <class TYPE> class klLinearRegression
{
public:
	//Minimizes ||B-AX|| in L2 norm.  If A is full rank, and A.row()>=A.col() a QR factorization is used 
	//to solve the system - A=(Q1,Q2) tr(R,0) and RX=tr(Q1)B give the unique solution.  
	//If A.row()<A.col() the LQ factorization is used - A=(L,0) tr(Q1,Q2) and X=tr(Q1) L^(-1) B.
	//If A.col()<A.row() and rank(A)=A.col() the minimum norm solution 
	//of the underdetermined system is calculated.  If A does not have full rank, a full orthogonal 
	//factorization is performed to solve the system.  
	klLinearRegression(const klMatrix<TYPE>& A,const klMatrix<TYPE>& B) 
		: _A((TYPE*)A.getMemory(),A.getRows(),A.getColumns(),false),
		_B((TYPE*)B.getMemory(),B.getRows(),B.getColumns(),false)

	{
	}


	klMatrix<TYPE> operator()()
	{
		return klLinearRegressionCore();
	}

	//Specializations exist for float and double.
	//This class should not be instantiated for any other type.
	klMatrix<TYPE> klLinearRegressionCore()
	{
		return klMatrix<TYPE>(0,0);
	}
	
	//	klMatrix<TYPE> zscore():
private:
	klMatrix<TYPE> _A;
	klMatrix<TYPE> _B;
};


template<> klMatrix<float> klLinearRegression<float>::klLinearRegressionCore()
{
	int m=_A.getRows();
	int n=_A.getColumns();
	int nrhs=_B.getColumns();
	klMatrix<float> trA=_A.transpose();
	klMatrix<float> trB=_B.transpose();
	int lda=_A.getRows();
	int ldb=_B.getRows();
	float rcond=_A.ConditionNumber();
	int min=(m<n? m :n);
	int max=(m>n? m :n);
	int max2=2*min>max ? 2*min : max;
	int max3=max2>nrhs ? max2 : nrhs;
	int lwork=5*max3;//use 5 time the recommended workspace - MKL says be generous
	float* work=new float[lwork];
	int eigensize=m<n ? m:n;
	float* s=new float[eigensize];
	int rank=0;//ouput param - effective rank, the number of eigenvalues > rcond * s[0]
	int info=0;

	sgelss(&m,&n,&nrhs,trA.getMemory(),&lda,trB.getMemory(),&ldb,s,&rcond,&rank,work,&lwork,&info);

	//trA has first min(m,n) eigenvectors stored row-wise
	//trB has n,nrhs solution matrix 
	//if m>=n and rankA=n, _RSS can be obtained by summing squares of elements n+1:m in that column
	//Pull the solution vectors out of trB and store RSS in _RSS if we have them.

	//bbs revisit - partial impl for now
	//bbcrevisit revisit what is partial - the options for A,B or the implementation of this 
	//part?

	delete s;
	delete work;
	return trB.transpose();

}
template<> klMatrix<double> klLinearRegression<double>::klLinearRegressionCore()
{	
	int m=_A.getRows();
	int n=_A.getColumns();
	int nrhs=_B.getColumns();
	klMatrix<double> trA=_A.transpose();
	klMatrix<double> trB=_B.transpose();
	int lda=_A.getRows();
	int ldb=_B.getRows();

	//rcond is used to determine the effective rank of A. Singular
	//values s(i) ? rcond *s(1) are treated as zero.
	//If rcond <0, machine precision is used instead.
	double rcond= -1;//_A.ConditionNumber();
	//rcond = trA.ConditionNumber();
	int min=(m<n? m :n);
	int max=(m>n? m :n);
	int max2=2*min>max ? 2*min : max;
	int max3=max2>nrhs ? max2 : nrhs;
	int lwork=5*max3;//use 5 time the recommended workspace - MKL says be generous
	double* work=new double[lwork];
	int eigensize=m<n ? m:n;
	double* s=new double[eigensize];
	int rank=0;//ouput param - effective rank, the number of eigenvalues > rcond * s[0]
	int info=0;

	
	dgelss(&m,&n,&nrhs,trA.getMemory(),&lda,trB.getMemory(),&ldb,s,&rcond,&rank,work,&lwork,&info);

    //dgelss(&m,&n,&nrhs,_A.getMemory(),&lda,_B.getMemory(),&ldb,s,&rcond,&rank,work,&lwork,&info);

	delete s;
	delete work;
	return trB.transpose();
}








#endif //__kl_regression__








