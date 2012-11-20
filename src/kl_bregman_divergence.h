#ifndef __kl_bregman_divergence__
#define __kl_bregman_divergence__

#include "mkl.h"
#include "kl_matrix.h"


template<class TYPE> TYPE klMahalanobisDivergence(klMatrix<TYPE> A, klVector<TYPE> x,klVector<TYPE> y)
{
		if(A.getColumns()!=A.getRows())
			throw "TYPE klManhalobisDivergence(klMatrix<TYPE> A, klVector<TYPE> x,klVector<TYPE> y) invalid dimension";
		if(x.getColumns()!=A.getRows())
			throw "TYPE klManhalobisDivergence(klMatrix<TYPE> A, klVector<TYPE> x,klVector<TYPE> y) invalid dimension";
		if(y.getColumns()!=A.getRows())
			throw "TYPE klManhalobisDivergence(klMatrix<TYPE> A, klVector<TYPE> x,klVector<TYPE> y) invalid dimension";
	
		TYPE dist = (x-y).dot( A * (x-y));
}


//B_F(x,y) = \sum x(i) \log \frac{x(i)}{y(i)} - \sum x(i) + \sum y(i)
template<class TYPE> TYPE klKullbackLiebelerDivergence(klVector<TYPE> x,klVector<TYPE> y)
{
		if(x.getColumns()!=y.getColumns())
			throw "TYPE TYPE klKullbackLiebelerDivergence(klVector<TYPE> x,klVector<TYPE> y) invalid dimension";
		if(!x.isPositive() || !y.isPositive())
			throw "TYPE TYPE klKullbackLiebelerDivergence(klVector<TYPE> x,klVector<TYPE> y) x and y should be positive";
		TYPE sx=x.sum();
		TYPE sy=y.sum();
		klVector<double> lx= klApplyFn<double,double>(std::log,x);
		klVector<double> ly= klApplyFn<double,double>(std::log,y);

		TYPE s1= (lx-ly).dot(x)-sx+sy;
}

//B_F(x,y) = \sum_i \left(\frac {x(i)}{y(i)} - \log \frac{x(i)}{y(i)} - 1
 template<class TYPE> TYPE klItakuraSaitoDivergence(klVector<TYPE> x,klVector<TYPE> y)
{
		if(x.getColumns()!=x.getRows())
			throw "TYPE TYPE klItakuraSaitoDivergence(klVector<TYPE> x,klVector<TYPE> y) invalid dimension";
		if(!x.isPositive() || !y.isPositive())
			throw "TYPE TYPE klItakuraSaitoDivergence(klVector<TYPE> x,klVector<TYPE> y) x and y should be positive";
		TYPE s1= (x/y).sum();
		klVector<double> lx= klApplyFn<double,double>(std::log,x);
		klVector<double> ly= klApplyFn<double,double>(std::log,y);
		TYPE s2=(lx-ly).sum();
		TYPE result = s1-s2-1;


}
#endif //__kl_bregman_divergence__
