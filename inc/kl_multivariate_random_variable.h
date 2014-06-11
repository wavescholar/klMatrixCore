 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/

#ifndef __kl_multivariate_random_number__
#define __kl_multivariate_random_number__

#include "kl_random_number_generator.h"

#include "kl_matrix.h"
#include "kl_principal_components.h"
#include "mkl.h"

using namespace std;
#include <vector>

//bbcrevisit - add more structure to this functionality,  specify variance, allow bounds on off diag components, etc. 
template<class TYPE> klMatrix<TYPE> klGenerateRandomSymmetricPositiveDefiniteMatrix(unsigned int size, int seed=0)
{
	/*  Recipe for SPD
	 The diagonal entries of the matrix have the form (0.5 + rand)*dim; 
	 the lower-diagonal entries take the form rand — 0.5; 
	 the upper-diagonal entries equal their lower-diagonal counterparts.
	 Here, dim is the number of equations and rand a random-number generator with values in the interval [0, 1].
	 This construction yields a symmetric and positive definite matrix.
	 */
	
    klUniformRV<double> U(0,1);
	U.reset(seed);
    
    klMatrix<TYPE> covarianceMatrix(size,size);
	unsigned int i,j;
	
    for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			if(i==j)
			{
				covarianceMatrix[i][j]=(0.5+U())*size;
				continue;
			}
			if(i<j)
			{
				covarianceMatrix[i][j]=(0.5-U());
				continue;
			}

			
		}
	}
	//now fill the upper half with the values in the lower half.
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			if(i>j)
			{
				covarianceMatrix[i][j]=covarianceMatrix[j][i];
				continue;
			}

			
		}
	}
    return covarianceMatrix;



}

//A multivariate normal random variate generator.  
template<class TYPE> class klNormalMultiVariate
{
public:
	klNormalMultiVariate(klVector<TYPE> meanVector, klMatrix<TYPE> covarianceMatrix ,bool seedFromClock=true,klVector<unsigned int> seedVector=0)
	{
		_meanVector=meanVector;
		_covarianceMatrix=covarianceMatrix;
		if(!seedFromClock)
			_seedVector=seedVector;
		setup(seedFromClock);

	}
	void setup(bool seedFromClock)
	{		unsigned int row=_covarianceMatrix.getRows();


			if(sizeof(TYPE)==sizeof(float))  //bbcrevisit this is not the way to code this - need a core template specialization member that's called
			{
			}
			else if(sizeof(TYPE)==sizeof(double))
			{
			}
			else
				throw "klNormalMultiVariate ERROR: only float and double template parameters supported at this time.";


			setupSpec();

			unsigned int i;
			if(seedFromClock)
			{
				clock_t processMilisec = clock(); //CLOCKS_PER_SEC = 1000 
				klMersenneTwister seedRV(processMilisec); 
				for(i=0;i<row;i++)
					_normals.push_back( klNormalBoxMullerRV<TYPE> (false,seedRV.nextValue() ) );//klNormalInverseApproxRV<TYPE> (0,1) ); bbcrevisit inverse method broken
			}
			else
			{
				for(i=0;i<row;i++)
					_normals.push_back( klNormalBoxMullerRV<TYPE> (false,_seedVector[i]) );//klNormalInverseApproxRV<TYPE> (0,1,false,_seedVector[i]) );
			}
		}

		klVector<TYPE> operator()()
		{
			unsigned int n=_meanVector.getRowSize();
			klVector<TYPE> nextRV;
			//Use a memory manager if the parameters both used the same one.
			if(_meanVector.getMemMgr()!=0 && _meanVector.getMemMgr()==_covarianceMatrix.getMemMgr() )
				nextRV.setup(n,_meanVector.getMemMgr());
			else
				nextRV.setup(n );

			//Now use factor to generate RV
			//First get n N(0,1) normals
			klVector<TYPE> v(n);
			unsigned int i;
			for(i=0;i<n;i++)
			{
				v[i]=_normals[i]();
			}
			
			int k;
			int j;			
			for(i=0;i<n;i++)
			{
				nextRV[i]=_meanVector[i];
				for(j=0;j<=i;j++)
				{
					nextRV[i]+=v[j]* _factor[i][j]; 
				}
			}

			for(i=0;i<n;i++)
				if(nextRV[i]>1000)
					cout<<"bbcrevisit we have a problem houston";

			return nextRV;
		}

		//Returns the entropy measured in nats calculated on a grid [a,b]^dim(covarianceMatrix) dx
		//where a,b = -n \sigma , +n \sigma and \sigma is the larges eigenvalue of the covariance matrix
		double entropy(double dx, unsigned int n)
		{
			//bbctodo
			//H(p(x))= - \int\limits_{[a,b]^n} p(x) ln( p(x) ) dx
			
		}

		//returns the value of the probability distribution at x
		TYPE p( TYPE x)
		{	
			//bbctodo
			//p(x) = \fract{1}{2 \pi | \Sigma |^ \fract{1}{2} } exp [ - \fract{1}{2} ( x\ \mu) \Sigma^{-1} (x-\mu) ]
			//Ref Duda andf Hart 2.5 pp 33
			return 0;

		}
		//let \Phi be the ON eigenvectors of the covariance matrix and \Lambda the diagonal 
		//matrix of eigenvalues.  Then the whitening transform \Phi \Lambda^{fract{1}{2}} applied to the coordiantes
		//of a data vector from this distribution transforms it to one drawn from a shperically symmetric distribution; 
		//ie the covariance matrix of the transformed data is I.
		klMatrix<TYPE> whiteningTransform()
		{
			return klMatrix<TYPE>(0,0);
		}

		//This returns a meshgrid of the n-1 dimensional manifold of constant probability for this distribution.
		//This is the loci of points which comprise the hyperellipsoid of constant density.  The quadratic form
		//( x-\ mu) \Sigma^{-1} (x- \mu) determines this surface; for the 90% confidence we solve 
		//( x-\ mu) \Sigma^{-1} (x- \mu)=0.90.  The principal axis of the hyperellipsoids are given by the
		// eigenvectors of the covariance matrix. 
		klMatrix<TYPE> lociOfContantProbability(double dx)
		{
			//bbctodo
			return klMatrix(0,0);
		}
		
		//Returns the orthonormal matrix of eigenvectors of the covariance matrix which are guaranteed to exist
		//by the requierment that the covariance matrix is positive definite.
		klMatrix<TYPE> eigenvectors()
		{
			//bbctodo calculate this in setup and save it for retrieval later
			return klMatrix<TYPE>(0,0);
		}

		//Returns the Mahalanobis distance from x \in \mathbb{R}^n to \mu \in \mathbb{R}^n 
		//The contours of contant density returned by lociOfConstantDensity() have constant Mahalanobis density.
		//This returns \sqrt{ ( x-\ mu) \Sigma^{-1} (x- \mu) }.   
		TYPE mahalanobisDistance(klVector<TYPE> x)
		{
			//bbctodo
			return 0;
		}

		void setupSpec();

		
private:
	klVector<TYPE> _meanVector;
	klMatrix<TYPE> _covarianceMatrix;
	klVector<unsigned int> _seedVector;
	klMatrix<TYPE> _factor;
//	vector< klNormalInverseApproxRV<TYPE> > _normals;
	vector< klNormalBoxMullerRV<TYPE> > _normals;


};


#endif