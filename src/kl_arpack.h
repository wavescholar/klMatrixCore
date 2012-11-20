#ifndef __kl_arpack__
#define __kl_arpack__
#include <math.h>
#include <fstream>
#include <iostream>
using namespace std;
#include "kl_matrix.h"

extern void callAprpack( int n,double* matrix, unsigned int  numEgs, complex<double>* eigenValues, complex<double> ** eigenVectors);



class klArpackFunctor
{
public:
	klMatrix<complex<double> >  EigenVectors;

public: 
	klVector<complex<double> > run(klMatrix<double>& kldmp,unsigned int numEigenvalues,bool calculateEigenvectors=true)
	{
		//Arpack does not do SVD, we can only feed it nxn matrices
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

			callAprpack(kldmp.getColumns(),kldmp.getMemory(), numEigenvalues,eigVals, Evecs );

			for(int j =0 ; j<numEigenvalues;j++)
			{
				complex<double> val = *(eigVals +j);
				eigenvalues[j].real( val.real() );
				eigenvalues[j].imag( val.imag() );
			}
			delete eigVals;
			for(j=0;j<numEigenvalues;j++)
			{
				delete Evecs[j];
			}
		}
		return eigenvalues;		
	}

	

};

#endif //#define __kl_arpack__