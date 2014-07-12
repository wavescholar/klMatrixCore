 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/
#ifndef __kl_arpack__
#define __kl_arpack__
#include <math.h>
#include <fstream>
#include <iostream>
using namespace std;
#include "kl_matrix.h"

extern void callAprpackNonSymmetric( int n,double* matrix, unsigned int  numEgs, complex<double>* eigenValues, complex<double> ** eigenVectors,char* whichp = "LM");

extern void callAprpackSymmetric( int n,double* matrix, unsigned int  numEgs, complex<double>* eigenValues, complex<double> ** eigenVectors,char* whichp = "LM");

class klArpackFunctor
{
public:
	klMatrix<complex<double> >  EigenVectors;//bbcrevisit sure this memory is not leaked!  Use smart pointer.

public: 
	//For now we return complex data even in the symetric case. This should change to conserve memory.
	klVector<complex<double> > run(klMatrix<double>& kldmp,unsigned int numEigenvalues,bool calculateEigenvectors=true)
	{
		if(kldmp.getColumns()!=kldmp.getRows())
			throw "klArpackFunctor : run(klMatrix<double>& kldmp,unsigned int numEigenvalues,bool calculateEigenvectors=true) bad dimensions";

		//double rcondN = kldmp.ConditionNumber(false);
		//cout<<"rcondN = "<<rcondN<<endl;

		complex<double> **Evecs, *Evals;

		klVector<complex<double> > eigenvalues(numEigenvalues);

		if(calculateEigenvectors)
		{
			Evecs = new complex<double> *[numEigenvalues];
			unsigned int j =0;
			for(j=0;j<numEigenvalues;j++)
			{
				Evecs[j]=new complex<double> [kldmp.getRows()];
			}
			klVector<double> RE= klVector<double> (numEigenvalues);
			klVector<double> IM = klVector<double> (numEigenvalues);
			complex<double>* eigVals = new complex<double> [numEigenvalues];

			//double rcondN = kldmp.ConditionNumber(true);
			//cerr<<"------------------------------------------------"<<endl;
			//cerr<<"rcondN = "<<rcondN<<endl;
			//cerr<<"------------------------------------------------"<<endl;
			//cerr<<kldmp.eigenvalues();
			//cerr<<"------------------------------------------------"<<endl;
			//try
			{
				//if(kldmp.transpose() == kldmp)
				{
					//callAprpackSymmetric(kldmp.getColumns(),kldmp.getMemory(), numEigenvalues,eigVals, Evecs,"SM" );
				}
				//else
				{
					callAprpackNonSymmetric(kldmp.getColumns(),kldmp.getMemory(), numEigenvalues,eigVals, Evecs,"SM" );
				}
			}
			/*catch(...)
			{
				cerr<<"Problem in Arpack Call"<<endl;
				double rcondN = kldmp.ConditionNumber(false);
				cout<<"rcondN = "<<rcondN<<endl;
			}*/

			for(int j =0 ; j<numEigenvalues;j++)
			{
				complex<double> val = *(eigVals +j);
				eigenvalues[j].real( val.real() );
				eigenvalues[j].imag( val.imag() );
			}
			delete eigVals;

			klMatrix<complex<double> > T(numEigenvalues,kldmp.getRows());

			for(j=0;j<numEigenvalues;j++)
			{
				//BBC Do memcopy
				for(int i=0;i<kldmp.getRows();i++)
				{
					T[j][i] = *(Evecs[j]+i);
				}
				delete Evecs[j];
			}
			EigenVectors = T;

		}
		return eigenvalues;		
	}
};

#endif //#define __kl_arpack__