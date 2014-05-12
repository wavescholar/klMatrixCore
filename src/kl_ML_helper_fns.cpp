 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/

#include "kl_ML_helper_fns.h"
#include "kl_divergence_functions.h"
#include "kl_arpack.h"

//Let A \in G_1(n,n) =X : X_{ii} =_d N(0,1) X_{ij} =_d N(0,frac{1}{2}
//frac{A+A^dag} {2} \in GOE 
klMatrix<double> SampleGOE( __int64 n , unsigned  seed )
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
//the multivariate normal distribution
klMatrix<double> SampleWishart( __int64 n , unsigned  seed )
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

klMatrix<double> SampleSymmetricStandardNormalRM( __int64 n , unsigned  seed )
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


