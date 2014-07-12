 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#include "D:\Packages\arpack++\examples\matrices\nonsym\dnmatrxa.h"
#include "ardnsmat.h"
#include "ardsnsym.h"
#include "ardsmat.h"
#include "D:\Packages\arpack++\examples\matrices\nonsym\lnsymsol.h" 


//------------------------These really belong in a arpack lib - or te be inlined in the Arpack++ headers.  This is temporary.
#include "arrseig.h"
void MemoryOverflow() { throw ArpackError(ArpackError::MEMORY_OVERFLOW); }
//From arerror.h
void ArpackError::Set(ErrorCode error, char* where)
{

  code = error;
  switch (code) {
  case NO_ERRORS          :
    return;
  case NOT_IMPLEMENTED    :
    Print(where, "This function was not implemented yet");
    return;
  case MEMORY_OVERFLOW    :
    Print(where, "Memory overflow");
    return;
  case GENERIC_SEVERE     :
    Print(where, "Severe error");
    return;
  case PARAMETER_ERROR    :
    Print(where, "Some parameters were not correctly defined");
    return;
  case N_SMALLER_THAN_2   :
    Print(where, "'n' must be greater than one");
    return;
  case NEV_OUT_OF_BOUNDS  :
    Print(where, "'nev' is out of bounds");
    return;
  case WHICH_UNDEFINED    :
    Print(where, "'which' was not correctly defined");
    return;
  case PART_UNDEFINED     :
    Print(where, "'part' must be one of 'R' or 'I'");
    return;
  case INVMODE_UNDEFINED  :
    Print(where, "'InvertMode' must be one of 'S' or 'B'");
    return;
  case RANGE_ERROR  :
    Print(where, "Range error");
    return;
  case LAPACK_ERROR       :
    Print(where, "Could not perform LAPACK eigenvalue calculation");
    return;
  case START_RESID_ZERO   :
    Print(where, "Starting vector is zero");
    return;
  case NOT_ACCURATE_EIG   :
    Print(where, "Could not find any eigenvalue to sufficient accuracy");
    return;
  case REORDERING_ERROR   :
    Print(where, "Reordering of Schur form was not possible");
    return;
  case ARNOLDI_NOT_BUILD  :
    Print(where, "Could not build an Arnoldi factorization");
    return;
  case AUPP_ERROR         :
    Print(where, "Error in ARPACK Aupd fortran code");
    return;
  case EUPP_ERROR         :
    Print(where, "Error in ARPACK Eupd fortran code");
    return;
  case CANNOT_PREPARE     :
    Print(where, "Could not correctly define internal variables");
    return;
  case CANNOT_FIND_BASIS  :
    Print(where, "Could not find the Arnoldi basis vectors");
    return;
  case CANNOT_FIND_VALUES :
    Print(where, "Could not find any eigenvalue");
    return;
  case CANNOT_FIND_VECTORS:
    Print(where, "Could not find any eigenvector");
    return;
  case CANNOT_FIND_SCHUR  :
    Print(where, "Could not find any Schur vector");
    return;
  case SCHUR_UNDEFINED    :
    Print(where, "FindEigenvectors must be used instead of FindSchurVectors");
    return;
  case CANNOT_GET_VECTOR  :
    Print(where, "Vector is not already available");
    return;
  case CANNOT_GET_PROD    :
    Print(where, "Matrix vector product is not already available");
    return;
  case CANNOT_PUT_VECTOR  :
    Print(where, "Could not store vector");
    return;
  case PREPARE_NOT_OK     :
    Print(where, "DefineParameters must be called prior to this function");
    return;
  case BASIS_NOT_OK       :
    Print(where, "Arnoldi basis is not available");
    return;
  case VALUES_NOT_OK      :
    Print(where, "Eigenvalues are not available");
    return;
  case VECTORS_NOT_OK     :
    Print(where, "Eigenvectors are not available");
    return;
  case SCHUR_NOT_OK       :
    Print(where, "Schur vectors are not available");
    return;
  case RESID_NOT_OK       :
    Print(where, "Residual vector is not available");
    return;
  case MATRIX_IS_SINGULAR :
    Print(where, "Matrix is singular and could not be factored");
    return;
  case DATA_UNDEFINED     :
    Print(where, "Matrix data was not defined");
    return;
  case INSUFICIENT_MEMORY :
    Print(where, "fill-in factor must be increased");
    return;
  case NOT_SQUARE_MATRIX  :
    Print(where, "Matrix must be square to be factored");
    return;
  case NOT_FACTORED_MATRIX:
    Print(where, "Matrix must be factored before solving a system");
    return;
  case INCOMPATIBLE_SIZES :
    Print(where, "Matrix dimensions must agree");
    return;
  case DIFFERENT_TRIANGLES:
    Print(where, "A.uplo and B.uplo must be equal");
    return;
  case INCONSISTENT_DATA  :
    Print(where, "Matrix data contain inconsistencies");
    return;
  case CANNOT_READ_FILE   :
    Print(where, "Data file could not be read");
    return;
  case CANNOT_OPEN_FILE   :
    Print(where, "Invalid path or filename");
    return;
  case WRONG_MATRIX_TYPE  :
    Print(where, "Wrong matrix type");
    return;
  case WRONG_DATA_TYPE    :
    Print(where, "Wrong data type");
    return;
  case RHS_IGNORED        :
    Print(where, "RHS vector will be ignored");
    return;
  case UNEXPECTED_EOF     :
    Print(where, "Unexpected end of file");
    return;
  case NCV_OUT_OF_BOUNDS  :
    Print(where, "'ncv' is out of bounds");
    return;
  case MAXIT_NON_POSITIVE :
    Print(where, "'maxit' must be greater than zero");
    return;
  case MAX_ITERATIONS     :
    Print(where, "Maximum number of iterations taken");
    return;
  case NO_SHIFTS_APPLIED  :
    Print(where, "No shifts could be applied during a cycle of IRAM iteration");
    return;
  case CHANGING_AUTOSHIFT :
    Print(where, "Turning to automatic selection of implicit shifts");
    return;
  case DISCARDING_FACTORS :
    Print(where, "Factors L and U were not copied. Matrix must be factored");
    return;
  case GENERIC_WARNING    :
  default: ;
    Print(where, "There is something wrong");
    return;
  }

} // Set.
ArpackError::ErrorCode ArpackError::code = NO_ERRORS;
//-------------------------------------------------------------------------------

#include <complex>
using namespace std;
#undef debug

extern void callAprpackNonSymmetric( int n,double* matrix, unsigned int  numEigs,complex<double>* eigenValues , complex<double> ** eigenVectors,char* whichp = "LM")
{
	ARdsNonSymMatrix<double> A(n, matrix);

	//ncv has  default value (2nev+1).
	int ncv = 2* numEigs +1;

	//ARluNonSymStdEig<double> Prob(numEigs, A, whichp, ncv);
	
	ARluNonSymStdEig<double> Prob(numEigs, A, whichp, ncv,0.0,2048);

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

	if(Prob.GetNev() !=nconv) //Try the shift and invert mode
	{
		double  sigmap= 0.005;
		Prob.SetShiftInvertMode(sigmap);
		Prob.FindEigenvectors();
		n     = Prob.GetN();
		nconv = Prob.ConvergedEigenvalues();
	}

	cout << "Dimension of the system            : " << n              << endl;
	cout << "Number of 'requested' eigenvalues  : " << Prob.GetNev()  << endl;
	cout << "Number of 'converged' eigenvalues  : " << nconv          << endl;
	cout << "Number of Arnoldi vectors generated: " << Prob.GetNcv()  << endl;
	cout << "Number of iterations taken         : " << Prob.GetIter() << endl;
	cout << endl;

	//OK
	//Solution(A, Prob);

	//--------------------------------Solution Code
	{

		int   i, n, nconv, mode;
		double *Ax;
		double *ResNorm;

		n     = Prob.GetN();
		nconv = Prob.ConvergedEigenvalues();
		mode  = Prob.GetMode();

		cout << endl << endl << "Testing ARPACK++ class ARluNonSymStdEig \n";
		cout << "Real nonsymmetric eigenvalue problem: A*x - lambda*x" << endl;
		switch (mode) {
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
			// Printing eigenvalues.

			cout << "Eigenvalues:" << endl;
			for (i=0; i<nconv; i++) 
			{
				cout << "  lambda[" << (i+1) << "]: " << Prob.EigenvalueReal(i);
				if (Prob.EigenvalueImag(i)>=0.0)
				{
					cout << " + " << Prob.EigenvalueImag(i) << " I" << endl;
				}
				else 
				{
					cout << " - " << fabs(Prob.EigenvalueImag(i)) << " I" << endl;
				}

				//Put in the parameter
				complex<double> evVal; 
				evVal.real( Prob.EigenvalueReal(i)) ;
				evVal.imag (Prob.EigenvalueImag(i)) ;

				(*(eigenValues+i)).real(Prob.EigenvalueReal(i));
				(*(eigenValues+i)).imag( Prob.EigenvalueImag(i));
				
			}
			cout << endl;
		}

		if (Prob.EigenvectorsFound()) 
		{
			// Printing the residual norm || A*x - lambda*x ||
			// for the nconv accurately computed eigenvectors.

			Ax      = new double[n];
			ResNorm = new double[nconv+1];

			for (i=0; i<nconv; i++) 
			{

				if (Prob.EigenvalueImag(i)==0.0) 
				{ 
					// Eigenvalue is real
					A.MultMv(Prob.RawEigenvector(i), Ax);
					axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i), 1, Ax, 1);
					ResNorm[i] = nrm2(n, Ax, 1)/fabs(Prob.EigenvalueReal(i));

					//---------------Put in parameter
					double* ev = Prob.RawEigenvector(i);	
					complex<double>* evP = *(eigenVectors+i);
					for(int j=0;j<n;j++)
					{
						(*(evP +  j)).real( *(ev + j)) ;
						(*(evP + j)).imag( 0.0f);
					}
				}
				else 
				{
					//-----------------
					//Eigenvalue is complex.
					double* evR = Prob.RawEigenvector(i);
					double* evI = Prob.RawEigenvector(i+1);
				
					complex<double>* evP = *(eigenVectors+i);

					for(int j=0;j<n;j++)
					{
						(*(evP +  j)).real( *(evR + j)) ;
						(*(evP + j)).imag( *(evI + j));
					}

					// Eigenvalue is complex
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
	} // Solution




	//BBCREVISIT - BUSTED
	//if (Prob.EigenvaluesFound())
	//{
	//	cout << "Eigenvalues:" << endl;
	//	for (i=0; i<nconv; i++) 
	//	{
	//		complex<double> evVal; 
	//		evVal.real( Prob.EigenvalueReal(i)) ;
	//		evVal.imag (Prob.EigenvalueImag(i)) ;
	//		
	//		(*(eigenValues+i)).real(Prob.EigenvalueReal(i));
	//		(*(eigenValues+i)).imag( Prob.EigenvalueImag(i));

	//		cout << "  lambda[" << (i+1) << "]: " << Prob.EigenvalueReal(i);
	//		if (Prob.EigenvalueImag(i)>=0.0) 
	//		{
	//			cout << " + " << Prob.EigenvalueImag(i) << " I" << endl;
	//		}
	//		else 
	//		{
	//			cout << " - " << fabs(Prob.EigenvalueImag(i)) << " I" << endl;
	//		}
	//	}
	//	cout << endl;
	//}
	//
	//if (Prob.EigenvectorsFound()) 
	//{
	//	// Calculating the residual norm || A*x - lambda*x ||
	//	// for the nconv accurately computed eigenvectors.
	//	Ax      = new double[n];
	//	ResNorm = new double[nconv+1];

	//	for (i=0; i<nconv; i++) 
	//	{
	//		cout<<"nconv = "<<i<<endl;
	//		if (Prob.EigenvalueImag(i)==0.0) 
	//		{ // Eigenvalue is real.

	//			A.MultMv(Prob.RawEigenvector(i), Ax);
	//			axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i), 1, Ax, 1);
	//			ResNorm[i] = nrm2(n, Ax, 1)/fabs(Prob.EigenvalueReal(i));

	//			double* ev = Prob.RawEigenvector(i);
	//			
	//			//Put the eigenvectors in output variable.
	//			complex<double>* evP = *(eigenVectors+i);

	//			for(int j=0;j<n;j++)
	//			{
	//				(*(evP +  j)).real( *(ev + j)) ;
	//				(*(evP + j)).imag( 0.0f);
	//			}
	//			//
	//			A.MultMv(Prob.RawEigenvector(i), Ax);
	//			axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i), 1, Ax, 1);
	//			axpy(n, Prob.EigenvalueImag(i), Prob.RawEigenvector(i+1), 1, Ax, 1);
	//			ResNorm[i] = nrm2(n, Ax, 1);
	//			A.MultMv(Prob.RawEigenvector(i+1), Ax);
	//			axpy(n, -Prob.EigenvalueImag(i), Prob.RawEigenvector(i), 1, Ax, 1);
	//			axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i+1), 1, Ax, 1);
	//			ResNorm[i] = lapy2(ResNorm[i], nrm2(n, Ax, 1))/
	//				lapy2(Prob.EigenvalueReal(i), Prob.EigenvalueImag(i));
	//			ResNorm[i+1] = ResNorm[i];
	//			i++;
	//			//
	//		}
	//		else 
	//		{
	//			//Eigenvalue is complex.
	//			double* evR = Prob.RawEigenvector(i);
	//			double* evI = Prob.RawEigenvector(i+1);
	//			
	//			complex<double>* evP = *(eigenVectors+i);

	//			for(int j=0;j<n;j++)
	//			{
	//				(*(evP +  j)).real( *(evR + j)) ;
	//				(*(evP + j)).imag( *(evI + j));
	//			}
	//			
	//			A.MultMv(Prob.RawEigenvector(i), Ax);
	//			axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i), 1, Ax, 1);
	//			axpy(n, Prob.EigenvalueImag(i), Prob.RawEigenvector(i+1), 1, Ax, 1);
	//			ResNorm[i] = nrm2(n, Ax, 1);
	//			A.MultMv(Prob.RawEigenvector(i+1), Ax);
	//			axpy(n, -Prob.EigenvalueImag(i), Prob.RawEigenvector(i), 1, Ax, 1);
	//			axpy(n, -Prob.EigenvalueReal(i), Prob.RawEigenvector(i+1), 1, Ax, 1);
	//			ResNorm[i] = lapy2(ResNorm[i], nrm2(n, Ax, 1))/
	//				lapy2(Prob.EigenvalueReal(i), Prob.EigenvalueImag(i));
	//			ResNorm[i+1] = ResNorm[i];
	//			i++;
	//		}
	//	}

	//	for (i=0; i<nconv; i++)
	//	{
	//		cout << "||A*x(" << (i+1) << ") - lambda(" << (i+1);
	//		cout << ")*x(" << (i+1) << ")||: " << ResNorm[i] << "\n";
	//	}
	//	cout << "\n";

	//	delete[] Ax;
	//	delete[] ResNorm;
	//}
} 

//#include "D:\Packages\arpack++\examples\matrices\sym\dsmatrxb.h"
//#include "D:\Packages\arpack++\examples\matrices\sym\dsmatrxc.h"
//#include "ardsmat.h"
//#include "ardgsym.h"
//#include "D:\Packages\arpack++\examples\matrices\sym\lsymsol.h"
//extern void callAprpackSymmetric( int n,double* matrix, unsigned int  numEigs,complex<double>* eigenValues , complex<double> ** eigenVectors,char* whichp = "LM")
//{
//	ARdsSymMatrix<double> A(n, matrix);
//	int ncvp = 0;
//    double tolp = 0.0; int maxitp = 2000;
//	ARluSymStdEig<double> Prob(numEigs,A, "SM",ncvp,tolp,maxitp);//Prob(numEigs, A, 0.00001f);
//
//	Prob.FindEigenvectors();
//	Solution(A, Prob);
//	int   i, nconv, mode;
//
//	n     = Prob.GetN();
//	nconv = Prob.ConvergedEigenvalues();
//	mode  = Prob.GetMode();
//
//	cout << "Real nonsymmetric eigenvalue problem: A*x - lambda*x" << endl;
//	switch (mode) 
//	{
//	case 1:
//		cout << "Regular mode" << endl << endl;
//		break;
//	case 3:
//		cout << "Shift and invert mode" << endl << endl;
//	}
//
//	if(Prob.GetNev() !=nconv) //Try the shift and invert mode
//	{
//		double  sigmap= 0.005;
//		Prob.SetShiftInvertMode(sigmap);
//		Prob.FindEigenvectors();
//		n     = Prob.GetN();
//		nconv = Prob.ConvergedEigenvalues();
//	}
//
//	cout << "Dimension of the system            : " << n              << endl;
//	cout << "Number of 'requested' eigenvalues  : " << Prob.GetNev()  << endl;
//	cout << "Number of 'converged' eigenvalues  : " << nconv          << endl;
//	cout << "Number of Arnoldi vectors generated: " << Prob.GetNcv()  << endl;
//	cout << "Number of iterations taken         : " << Prob.GetIter() << endl;
//	cout << endl;
//
//	if (Prob.EigenvaluesFound())
//	{
//		cout << "Eigenvalues:" << endl;
//		for (i=0; i<nconv; i++) 
//		{
//
//			cout << "  lambda[" << (i+1) << "]: " << Prob.Eigenvalue(i);
//			
//		}
//		cout << endl;
//	}
//	
//	if (Prob.EigenvectorsFound()) 
//	{
//		for (i=0; i<nconv; i++) 
//		{
//			cout<<"nconv = "<<i<<endl;
//
//			double* ev = Prob.RawEigenvector(i);
//
//			//Put the eigenvectors in output variable.
//			complex<double>* evP = *(eigenVectors+i);
//
//			for(int j=0;j<n;j++)
//			{
//				(*(evP +  j)).real( *(ev + j)) ;
//				(*(evP + j)).imag( 0.0f);
//			}
//		}
//	}
//} 

