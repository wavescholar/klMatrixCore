#ifndef __kl_linear_equation__
#define __kl_linear_equation__

#include "mkl.h"
#include "kl_matrix.h"


template<class TYPE> klLinearEquationSolver
{

	//Finds Ax=b.
	klLinearEquationSolver(klMatrix<TYPE>& A, klVector<TYPE>& b)
	{
		_A=A;
		_b=b;
		if(A.getRows() != b.getRowSize() )
			throw "klLinearEquationSolver ERROR: incompatible dimensions in attempt to solve Ax=b";
	}
	klVector<TYPE> minimumNormSolution()
	{

	}






}


#endif //__kl_linear_equation__
