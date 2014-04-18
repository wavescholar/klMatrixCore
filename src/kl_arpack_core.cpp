 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/

#include "D:\Packages\ConvexOptimization\SDPA_INTEL_BLAS\arpack++\examples\matrices\nonsym\dnmatrxa.h"
#include "ardnsmat.h"
#include "ardsnsym.h"
#include "D:\Packages\ConvexOptimization\SDPA_INTEL_BLAS\arpack++\examples\matrices\nonsym\lnsymsol.h" 

extern void callAprpack( int n,double* matrix, unsigned int  numEigs,complex<double>* eigenValues , complex<double> ** eigenVectors,char* whichp = "LM")
{
	ARdsNonSymMatrix<double> A(n, matrix);

	//ncv has  default value (2nev+1).
	int ncv = 2* numEigs +1;

	ARluNonSymStdEig<double> Prob(numEigs, A, whichp, ncv);

	Prob.FindEigenvectors();

	int   i, nconv, mode;
	double *Ax;
	double *ResNorm;

	n     = Prob.GetN();
	nconv = Prob.ConvergedEigenvalues();
	mode  = Prob.GetMode();

	cout << "Real nonsymmetric eigenvalue problem: A*x - lambda*x" << endl;
	switch (mode) 
	{
	case 1:
		cout << "Regular mode" << endl << endl;
		break;
	case 3:
		cout << "Shift and invert mode" << endl << endl;
	}

	cout << "Dimension of the system            : " << n              << endl;
	cout << "Number of 'requested' eigenvalues  : " << Prob.GetNev()  << endl;
	cout << "Number of 'converged' eigenvalues  : " << nconv          << endl;
	cout << "Number of Arnoldi vectors generated: " << Prob.GetNcv()  << endl;
	cout << "Number of iterations taken         : " << Prob.GetIter() << endl;
	cout << endl;

	if (Prob.EigenvaluesFound())
	{
		cout << "Eigenvalues:" << endl;
		for (i=0; i<nconv; i++) 
		{
			complex<double> evVal; 
			evVal.real( Prob.EigenvalueReal(i)) ;
			evVal.imag (Prob.EigenvalueImag(i)) ;
			
			(*(eigenValues+i)).real(Prob.EigenvalueReal(i));
			(*(eigenValues+i)).imag( Prob.EigenvalueImag(i));

			cout << "  lambda[" << (i+1) << "]: " << Prob.EigenvalueReal(i);
			if (Prob.EigenvalueImag(i)>=0.0) 
			{
				cout << " + " << Prob.EigenvalueImag(i) << " I" << endl;
			}
			else 
			{
				cout << " - " << fabs(Prob.EigenvalueImag(i)) << " I" << endl;
			}
		}
		cout << endl;
	}
	
	if (Prob.EigenvectorsFound()) 
	{
		// Calculating the residual norm || A*x - lambda*x ||
		// for the nconv accurately computed eigenvectors.
		Ax      = new double[n];
		ResNorm = new double[nconv+1];

		for (i=0; i<nconv; i++) 
		{
			if (Prob.EigenvalueImag(i)==0.0) 
			{ // Eigenvalue is real.

				A.MultMv(Prob.RawEigenvector(i), Ax);
				axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i), 1, Ax, 1);
				ResNorm[i] = nrm2(n, Ax, 1)/fabs(Prob.EigenvalueReal(i));

				double* ev = Prob.RawEigenvector(i);
				
				//Put the eigenvectors in output variable.
				complex<double>* evP = *(eigenVectors+i);

				for(int j=0;j<n;j++)
				{
					(*(evP +  j)).real( *(ev + j)) ;
					(*(evP + j)).imag( 0.0f);
				}
			}
			else 
			{
				//Eigenvalue is complex.
				double* evR = Prob.RawEigenvector(i);
				double* evI = Prob.RawEigenvector(i+1);
				
				complex<double>* evP = *(eigenVectors+i);

				for(int j=0;j<n;j++)
				{
					(*(evP +  j)).real( *(evR + j)) ;
					(*(evP + j)).imag( *(evI + j));
				}
				
				A.MultMv(Prob.RawEigenvector(i), Ax);
				axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i), 1, Ax, 1);
				axpy(n, Prob.EigenvalueImag(i), Prob.RawEigenvector(i+1), 1, Ax, 1);
				ResNorm[i] = nrm2(n, Ax, 1);
				A.MultMv(Prob.RawEigenvector(i+1), Ax);
				axpy(n, -Prob.EigenvalueImag(i), Prob.RawEigenvector(i), 1, Ax, 1);
				axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i+1), 1, Ax, 1);
				ResNorm[i] = lapy2(ResNorm[i], nrm2(n, Ax, 1))/
					lapy2(Prob.EigenvalueReal(i), Prob.EigenvalueImag(i));
				ResNorm[i+1] = ResNorm[i];
				i++;
			}
		}

		for (i=0; i<nconv; i++)
		{
			cout << "||A*x(" << (i+1) << ") - lambda(" << (i+1);
			cout << ")*x(" << (i+1) << ")||: " << ResNorm[i] << "\n";
		}
		cout << "\n";

		delete[] Ax;
		delete[] ResNorm;
	}
} 

