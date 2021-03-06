 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#ifndef __kl_basic_stat__
#define __kl_basic_stat__
#include "mkl.h"
#include <math.h>


namespace KL_stat{

//Calculate the mean of a sample for contiguous data sets.
//The return is a pointer to an array with the components of the mean vector.
//The user is responsible for deleting the returned memory.
template<class TYPE> TYPE* mean(TYPE* data, unsigned int data_dim, unsigned int data_size)
{
	unsigned int i,j;
	TYPE* mean=new TYPE[data_dim];
	for(i=0;i<data_dim;i++)
	{
		*(mean+i)=0;
		for(j=0;j<data_size;j++)
			*(mean+i)+=*(data + i*data_size + j);

		*(mean+i)/=data_size;
	}
	return mean;
	
}

//Computes variance of data set contiguous in menory. 
//The user is responsible for deleteting the memory returned from this method.
template<class TYPE> TYPE* variance(TYPE* data, unsigned int data_dim, unsigned int data_size)
{
	unsigned int i,j;
	TYPE* lmean=KL_stat::mean(data,data_dim,data_size);
	TYPE* lvariance=new TYPE[data_dim];
	for(i=0;i<data_dim;i++)
	{
		*(lvariance+i)=0;
		for(j=0;j<data_size;j++)
			*(lvariance+i)+= pow((double) *(data + i*data_size + j), 2.0) -  pow((double) *(lmean+i), 2.0) ;

		*(lvariance+i)/=data_size;
	}

	delete lmean;
	
	return lvariance;
	
}

//Compute the arithmetic mean of a 1D dataset using a recurrence relation 
//     mean(i) = mean(i-1) + (data[i] - mean(i-1))/(i+1) 
//that avoids potential overflow.
template<class TYPE> TYPE mean(TYPE* data,unsigned int n)
{
	TYPE mean=0;
	size_t i;
	for(i=0;i<n;i++)
	{
		mean+=(*(data+i)-mean)/(i+1);
	}
	return mean;
}

//Compute the variance of a 1D datatset using the recurrence relation
//    varaince(i)=(variance(i-1) + (data[i]-mean)^2 ) /(i+1)
template<class TYPE> TYPE variance(TYPE* data,unsigned int n,TYPE mean)
{
	TYPE variance=0;
	size_t i;
	for(i=0;i<n;i++)
	{
		TYPE s = *(data+i) - mean;
		variance += (s * s  - variance) / (i + 1);
	}
	
	return variance;
}
//Compute the third moment.  Skewness is a measure of the tail size of a sample or population
template<class TYPE> TYPE skewness(TYPE* data,unsigned int n,TYPE mean, TYPE variance)
{
 TYPE skewness=0;
 size_t i;
 for(i=0;i<n;i++)
 {
	 TYPE s = *(data+i) - mean;
	 skewness += (s * s * s  - skewness) / (i + 1);
 }
	skewness/=pow(double(variance),double(1.5) );
	return skewness;
}

//Compute the fourth moment.  Kurtosis is a measure of the peakedness of a sample or population
template<class TYPE> TYPE kurtosis(TYPE* data,unsigned int n,TYPE mean, TYPE variance)
{
TYPE kurtosis=0;
 size_t i;
 for(i=0;i<n;i++)
 {
	 TYPE s = *(data+i) - mean;
	 kurtosis += (s * s * s * s  - kurtosis) / (i + 1);
 }
	kurtosis/=pow(double(variance) ,double(2.0));
	return kurtosis;

}

}//end namespace KL


#endif //__kl_basic_stat__        