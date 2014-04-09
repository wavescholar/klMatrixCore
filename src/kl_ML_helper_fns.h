 /*******************************
 * WSCMP [2003] - [2014] WSCMP  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/

#ifndef __kl_ML_helperFns__
#define __kl_ML_helperFns__

#include "kl_memory.h"
#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_util.h"
#include "kl_unit_tests.h"

//Let A \in G_1(n,n) =X : X_{ii} =_d N(0,1) X_{ij} =_d N(0,frac{1}{2}
//frac{A+A^dag} {2} \in GOE 
klMatrix<double> SampleGOE( unsigned  int n , unsigned  seed= 4357 );

//\beta = 1 flavor - This function samples from the space of covariance matrices for
//the multivariate normal distributuion
klMatrix<double> SampleWishart( unsigned  int n , unsigned  seed= 4357 );

klMatrix<double> SampleSymmetricStandardNormalRM( unsigned  int n , unsigned  seed = 4357);

//Generate a random permutation of integer types over specified range.
template<class TYPE> klVector<TYPE> kl_random_permutation( unsigned  range, unsigned  seed= 4357 )
{
    unsigned i;
    unsigned j;

    klUniformRV<TYPE> RV;
    RV.reset(seed);

    klVector<TYPE> permutation(range);

    for ( i = 0; i < range; i++ )
    {
        permutation[i] = i + 1;
    }

    for ( i = 1; i <= range; i++ )
    {
        RV.setRange(i,range);//generate a uniform variate between i,and range
        TYPE temp=RV(); //swap i-1 and temp-1
        unsigned j=temp;  //allows instantiation for floating point types
        TYPE swapval=permutation[i-1];
        permutation[i-1]=permutation[j-1];
        permutation[j-1]=swapval;
    }

    return permutation;
}

//Returns an identity matrix
template<class TYPE> klMatrix<TYPE> IdentityMatrix(unsigned int n)
{
	klMatrix<TYPE> Id(n,n);
	unsigned int i=0;
	unsigned int j=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			Id[i][j]=0.0f;
			if(i==j)
				Id[i][j]=1.0f;
		}
	}
	return Id;
}

#endif //__kl_ML_helperFns__