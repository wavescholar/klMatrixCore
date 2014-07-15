 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#ifndef __kl_independent_components__
#define __kl_independent_components__
#include "kl_sample_population.h"
#include "mkl.h"

//Used to use the it++ fast ica 
#include <itpp/itbase.h>
#include <cstdio>
#include <itpp/base/fastica.h>//from the it library

using namespace itpp;
using namespace std;


template<class TYPE> class klIndependentComponents :public klSamplePopulation<TYPE>
{
public:

	klIndependentComponents() : _icaCalculated(false), _k(0)
	{

	}

	klIndependentComponents(const klSamplePopulation<TYPE>& src)
		:	klSamplePopulation<TYPE>(src), _icaCalculated(false), _k(0)  // ,klMatrix<TYPE> (src)
	{

	}
	  klMatrix<TYPE> klICACore(unsigned int k)
    {
    }



	//Calculates the eigenvalues and eigenvectors of the covariance matrix for the sample population.
	klMatrix<TYPE> operator()(unsigned int k)
	{

   return klICACore(k);


	}
	//returns the eigenvalues of the covariance matrix
	klVector<TYPE> eigenvalues()
	{ 
		if(! _icaCalculated)
		{
			if(_k==0 || _k>getColumns() )
				throw "klIndependentComponents:  too many components for this data matrix";
		
			operator()(_k);
		}
		return _eigenvalues;
	}


	unsigned int getk() { return _k;}



private:
	bool _icaCalculated;

	unsigned int _k;

	klVector<TYPE> _eigenvalues;

};

template<> klMatrix<float> klIndependentComponents<float>::klICACore(unsigned int k)
{
	
	_k=k;
	//
	//      By default the matrix is stored as a RowMajor matrix (i.e. listing elements in sequence
	//      beginning with the first column).
	//      Mat(Num_T *c_array, int rows, int cols, bool RowMajor = true, const Factory &f = DEFAULT_FACTORY);
	//
	klMatrix<float> tr=transpose();

	itpp::Mat<double> ma_mixed_sig(getRows(),getColumns());
	
	unsigned int i,j;
	
	for(i=0;i<getRows();i++)
	{
		for(j=0;j<getColumns();j++)
		{
			ma_mixed_sig(i,j)=(double)tr[i][j];
		}
	}
	
	
	Fast_ICA fastica(ma_mixed_sig);

	fastica.set_nrof_independent_components(ma_mixed_sig.rows());

	fastica.set_non_linearity(  FICA_NONLIN_TANH );

	fastica.set_approach( FICA_APPROACH_DEFL );

	fastica.separate();   //run the algoritm

	Mat<double> ICs = fastica.get_independent_components();

	klMatrix<float> ICklMat;

	for(i=0;i<getRows();i++)
	{
		for(j=0;j<getColumns();j++)
		{
			ICklMat[i][j]=(float)ICs(i,j);
		}
	}
	return ICklMat;
	//bbcrevisit beware the it++ mat will try and delete it's memory.
	//we should see how much use it++ gets and revisit integrating a kl-it factory
	//object for cooperative memory management.
	
}

template<> klMatrix<double> klIndependentComponents<double>::klICACore(unsigned int k)
{
		//FastICA fastica(sources);
		//fastica.set_nrof_independent_components(sources.rows());
		//fastica.set_non_linearity(  FICA_NONLIN_TANH );
		//fastica.set_approach( FICA_APPROACH_DEFL );
		//fastica.separate();
		//mat ICs = fastica.get_independent_components();
		 
	
	_k=k;
		//
	//      By default the matrix is stored as a RowMajor matrix (i.e. listing elements in sequence
	//      beginning with the first column).
	//      Mat(Num_T *c_array, int rows, int cols, bool RowMajor = true, const Factory &f = DEFAULT_FACTORY);
	klMatrix<double> tr=transpose();

	itpp::Mat<double> ma_mixed_sig(tr.getMemory(),getRows(),getColumns(),true);
	//fill up with our data 
	Fast_ICA fastica(ma_mixed_sig);

	//bbcrevisit if(k>getColumns())
		// throw "

	fastica.set_nrof_independent_components(ma_mixed_sig.cols());
	//bbcrevisit set the number to k?
	
	fastica.set_nrof_independent_components(3);
	
	fastica.set_non_linearity(  FICA_NONLIN_TANH );
	fastica.set_approach( FICA_APPROACH_DEFL );
	
	
	fastica.separate();   //run the algoritm


	itpp::Mat<double> ICs = fastica.get_independent_components();
	klMatrix<double> ICklMat(getRows(),getColumns() );
	unsigned int i,j;
	for(i=0;i<getRows();i++)
	{
		for(j=0;j<getColumns();j++)
		{
			ICklMat[i][j]=ICs(i,j);
		}
	}
	return ICklMat;
	//bbcrevisit beware the it++ mat will try and delete it's memory.
	//we should see how much use it++ gets and revisit integrating a kl-it factory
	//object for cooperative memory management.

	
	

}

#endif //__kl_independent_components__

