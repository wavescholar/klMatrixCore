#include "kl_multivariate_random_variable.h"

template<  > void klNormalMultiVariate<float>::setupSpec()
{
    //The covariance matrix has to be factored; ie find a s.t. Sigma=transpose(A) * A.  
    //For Positive definite matrices, a Cholesky factorization can be done.
    unsigned int row=_covarianceMatrix.getRows();
    unsigned int col=_covarianceMatrix.getColumns();
    if(row!=col || row !=_meanVector.getRowSize() )
        throw "klNormalMultiVariate ERROR: non-square covariance matrix, or inconsistent mean vector size";
  
    klMatrix<float> temp;
    temp=_covarianceMatrix;
    //Mkl factor
    unsigned int size=0;
    unsigned int i,j;
    unsigned int index=0;
    char uplo='L';
    int info=0;
    int n=row;

    //Normally we'd transpose before passing in factor memory to get layout into column major
    //format used by FORTRAN routine, but _covarianceMatrix should be symmetric - so this step is unncessary.
    // decl from mkl_lapack32.h 
    spotrf(&uplo,&n, temp.getMemory(),&n,&info);
    if(info<0)
        throw "klNormalMultiVariate ERROR: parameter error in MKL call to factor covariance matrix.";
    if(info>0)
        throw "klNormalMultiVariate ERROR: covariance matrix is not positive definite.";

    _factor=temp.transpose(); 

    //Only the lower triangular part of _factor is written to by MKL, 
    //so we have to write 0's in the lower part.
    for(i=0;i<row;i++)
    {
        for(j=0;j<row;j++)
        {
            if(i<j)
                continue;
            else if(i!=j)
                _factor[j][i]=0;
        }
    }

}


template<  > void klNormalMultiVariate<double>::setupSpec()
{	//The covariance matrix has to be factored; ie find a s.t. Sigma=transpose(A) * A.  
    //For Positive definite matrices, a Cholesky factorization can be done.
    unsigned int row=_covarianceMatrix.getRows();
    unsigned int col=_covarianceMatrix.getColumns();
    if(row!=col || row!=_meanVector.getRowSize() )
        throw "klNormalMultiVariate ERROR: non-square covariance matrix, or inconsistent mean vector size";
    double* mem=new double[row*col];
    memset(mem,0,row+col);

    klMatrix<double> temp;
    temp=_covarianceMatrix;

    unsigned int size=0;
    unsigned int i,j;
    unsigned int index=0;
   
    char uplo='L';
    MKL_INT info=0;
    MKL_INT n=row;

    //Normally we'd transpose before passing in factor memory to get layout into column major
    //format used by FORTRAN routine, but _covarianceMatrix should be symmetric - so this step is unncessary.
    // decl from mkl_lapack32.h 
//	void dpotrf( char* uplo, MKL_INT* n, double* a, MKL_INT* lda, MKL_INT* info );
    dpotrf(&uplo,&n, temp.getMemory(),&n,&info);
    if(info<0)
        throw "klNormalMultiVariate ERROR: parameter error in MKL call to factor covariance matrix.";
    if(info>0)
        throw "klNormalMultiVariate ERROR: covariance matrix is not positive definite.";

    _factor=temp.transpose();  
    //Only the lower triangular part of _factor is written to by MKL, 
    //so we have to write 0's in the lower part.
    for(i=0;i<row;i++)
    {
        for(j=0;j<row;j++)
        {
            if(i<j)
                continue;
            else if(i!=j)
                _factor[j][i]=0;
        }
    }
   
}









