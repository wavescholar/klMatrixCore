 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#include "kl_ML_helper_fns.h"
#include "kl_bregman_divergence.h"
#include "kl_arpack.h"

//Let A \in G_1(n,n) =X : X_{ii} =_d N(0,1) X_{ij} =_d N(0,frac{1}{2}
//frac{A+A^dag} {2} \in GOE 
klMatrix<double> SampleGOE( unsigned  int n , unsigned  seed )
{
	klMatrix<double> A(n,n);
	unsigned int i;
	unsigned int j;
	klNormalInverseApproxRV<double> N_0_1(0,1,false,seed);
	klNormalInverseApproxRV<double> N_0_frac_1_2(0,0.5,false,seed);
//#pragma omp parallel num_threads(4)
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			if(i==j)
			{
				//N(0,1)
				A[i][j]=N_0_1();
			}
			else
			{
				//N(0,1/1)  
				if(i== 28591 && j== 21073)
				{
					//Deal With Assertion failed: 0 < u && u < 1, file ..\..\src\kl_random_number_generator.h, line 255
					A[i][j]=N_0_frac_1_2();
				}
				A[i][j]=N_0_frac_1_2();
			}
		}
	}
	A=(A.transpose()+A)/2.0; //bbcrevisit inefficient
	return A;
}

//\beta = 1 flavor - This function samples from the space of covariance matrices for
//the multivariate normal distributuion
klMatrix<double> SampleWishart( unsigned  int n , unsigned  seed )
{
	klMatrix<double> A=SampleGOE(n,seed);

	klMatrix<double> Ap =mmBLAS(1.0, A.transpose(), A, 0.0,A);

	A= A.transpose() * A;

	return A;
}

#include "testmatgenunit.h"
extern bool testmatgen(bool silent);
#include "kl_matrix.h"
klMatrix<double> real_2d_array_to_klMatrix(ap::real_2d_array a)
{
	int n=a.gethighbound(0)+1;
	int m=a.gethighbound(1)+1;

	klMatrix<double> matv(n,m);
	unsigned int i=0;
	unsigned int j=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<m;j++)
		{
			matv[i][j] = a(i,j);
		}
	}
	return matv;

}

klMatrix<double> SampleSymmetricStandardNormalRM( unsigned  int n , unsigned  seed )
{
	klMatrix<double> A(n,n);
	unsigned int i;
	unsigned int j;
	klNormalInverseApproxRV<double> N_0_1(0,1,false,seed);
	double* mem = A.getMemory();

	//#pragma omp parallel num_threads(4)
	for(i=0;i<n;i++)
	{		
		for(j=0;j<n;j++)
		{
			if(i<j)
				continue;
			//if(i!=j)
			//{
			//	//N(0,1/1)
			//	A[i][j]=N_0_1();
			//	A[j][i] = A[i][j];
			//}
			//else if (i==j)
			//{
			//	A[i][j]=N_0_1();
			//}

			if(i!=j)
			{
				//N(0,1/1)
				*(mem+ j*n +i)=N_0_1();
				*(mem+ i*n +j) = *(mem+ j*n +i);
			}
			else if (i==j)
			{
				*(mem+ i*n +i)=N_0_1();
			}

		}
	}

	return A;
}


