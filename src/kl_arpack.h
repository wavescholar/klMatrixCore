 /*******************************
 * WSCMP [2003] - [2014] WSCMP  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
#ifndef __kl_arpack__
#define __kl_arpack__
#include <math.h>
#include <fstream>
#include <iostream>
using namespace std;
#include "kl_matrix.h"

extern void callAprpack( int n,double* matrix, unsigned int  numEgs, complex<double>* eigenValues, complex<double> ** eigenVectors,char* whichp = "LM");

class klArpackFunctor
{
public:
	klMatrix<complex<double> >  EigenVectors;//BBC Make sure thi memory is not leaked!  Use smart pointer.

public: 
	klVector<complex<double> > run(klMatrix<double>& kldmp,unsigned int numEigenvalues,bool calculateEigenvectors=true)
	{
		if(kldmp.getColumns()!=kldmp.getRows())
			throw "klArpack does not do SVD, we can only feed it nxn matrices";

		complex<double> **Evecs, *Evals;

		klVector<complex<double> > eigenvalues(numEigenvalues);

		if(calculateEigenvectors)
		{
			//klMatrix<complex<double > > eigenvectors (kldmp.getRows(),numEigenvalues);
			Evecs = new complex<double> *[numEigenvalues];
			unsigned int j =0;
			for(j=0;j<numEigenvalues;j++)
			{
				Evecs[j]=new complex<double> [kldmp.getRows()];//eigenvectors.getColumn(j).getMemory();
			}
			klVector<double> RE= klVector<double> (numEigenvalues);
			klVector<double> IM = klVector<double> (numEigenvalues);


			complex<double>* eigVals = new complex<double> [numEigenvalues];

			callAprpack(kldmp.getColumns(),kldmp.getMemory(), numEigenvalues,eigVals, Evecs,"SM" );

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