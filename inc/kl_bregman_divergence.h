 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
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
