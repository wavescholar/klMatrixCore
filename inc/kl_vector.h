 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/

#ifndef __kl_vector__
#define __kl_vector__

//Very important to do this before including the mkl header
//Otherwise there will be compile errors.
//Intel MKL provides complex types MKL_Complex8 and MKL_Complex16, which are
//structures equivalent to the Fortran complex types COMPLEX(4) and COMPLEX(8),
//respectively. These types are defined in the mkl_types.h header file. You can use these
//types to define complex data. You can also redefine the types with your own types before
//including the mkl_types.h header file. The only requirement is that the types must be
//compatible with the Fortran complex layout, that is, the complex type must be a pair of
//real numbers for the values of real and imaginary parts.
#include <complex>
#define MKL_Complex8 std::complex<float>
#define MKL_Complex16 std::complex<double>
#include "mkl.h"
#include "kl_memory.h"
#include "kl_util.h"
#include <limits.h>
#include <float.h>
#include <string>
#include <vector>
#include<sstream>
using namespace std;

//This allows for debug output of matrices to the console window in
//debug configuration. 
#ifndef _DEBUG
#define klout(A)  ; 
#endif

#ifdef _DEBUG
#define klout(A) cout<<"------------------------"<<endl<<A<<endl ; 
#endif
//klout is for development.  It is intended for display of intermediate results.
//klMatrix<double> A(2,2);
//A=2.0;
//klout(A);



#ifdef _DEBUG
extern __int64 globalKlVectorCopyConstructorCallCount;
extern __int64 globalKlVectorMoveConstructorCallCount;

extern __int64 globalKlVectorCopyConstructorBytesCount;
extern __int64 globalKlVectorMoveConstructorBytesCount;
#endif

inline int mkl_eigs_select(double* x,double* y)
{
	return (*x<=*y) ? true: false;

}

class klGlobalMemoryManager
{
protected:
	static klMemMgr* _globalMemoryManager;
public: 
	static void setklVectorGlobalMemoryManager(klMemMgr* mgr);
};

template<class TYPE> class klVector: public klRefCount<klMutex>, klGlobalMemoryManager
{
public:
	
	  klVector(klMemMgr* mgr,__int64 size,bool own=false) :
	  x0(0),x1(0), y0(0),y1(0),desc("")
	  {
		  _mgr=mgr;
		  _mMemory=(TYPE*)_mgr->allocate(size*sizeof(TYPE) );
		  if(own)
			  _own=1;
		  else
			  _own=0;
		  _size=size;
	  }
	  
	  klVector(TYPE* mem,__int64 size,bool own=false) :
	  x0(0),x1(0), y0(0),y1(0),desc("")
	  {
		  _mMemory=mem;
		  _own=own;
		  _size=size;
		  _mgr=0;
	  }
	  
	  klVector(__int64 size) :
	  x0(0),x1(0), y0(0),y1(0),desc("")
	  {
		  if(_globalMemoryManager==NULL)
		  {
			  _mMemory=new TYPE[size];
			  _own=1;
			  _size=size;
			  _mgr=0;
		  }
		  else
		  {
			  size_t allocationAmount = sizeof(TYPE)*size;
		
			  _mMemory =(TYPE*) _globalMemoryManager->allocate(allocationAmount);
			  _own=1;
			  _size=size;
			  _mgr=_globalMemoryManager;
		  }
	  }

	  klVector(double xStart, double dx,double xEnd) :
	  x0(xStart),x1(xEnd), y0(0),y1(0),desc("")
	  {
		  if(xEnd<=xStart)
			  throw "Range error constructing klVector :klVector(double xStart, double dx,double xEnd) xEnd<=xStrat";
		  __int64 size = ceil( (xEnd-xStart) / dx);

		  bool okToAllocate = klCheckFreeMemory(size);
		
		  if(!okToAllocate)
		  {
			  std::stringstream ss;
			  ss<<"Max allocation exceeded in constructing klVector("<<xStart<<" "<<dx<<" "<<xEnd<<") consider using smalller dx"<<endl;
			  
			  std::exception ex(ss.str().c_str()); 
			  throw ex;
		  }

		  _mMemory=new TYPE[size];

		  for(unsigned int i=0;i<size;i++)
		  {
			  //We may be loosing precision here
			  *(_mMemory+i)=(TYPE)(xStart + i*dx);
		  }

		  _own=1;
		  _size=size;
		  _mgr=0;
	  }
	  
	  klVector() :
	  x0(0),x1(0), y0(0),y1(0),desc("")
	  {
		  _mMemory=NULL;
		  _own=0;
		  _size=0;
		  _mgr=0;
	  }

	  klVector(const klVector<TYPE>& src)
	  {

		  _mMemory=NULL;
		  _own=0;
		  _size=0;
		  _mgr=0;
		  setup(src.getRowSize(),src._mgr);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  _mMemory[i]=src._mMemory[i];
		  x0=src.x0;
		  x1=src.x1;
		  y0=src.y0;
		  y1=src.y1;
		  desc=src.desc;
#ifdef _DEBUG
		  __int64 byteCount = _size * sizeof(TYPE);
		  globalKlVectorCopyConstructorBytesCount +=byteCount;
		  cerr<<"klVector(klVector<TYPE>& src) call count = "<<globalKlVectorCopyConstructorCallCount++<<" Bytes Count "<<globalKlVectorCopyConstructorBytesCount<<endl; 
#endif
	  }


	  klVector(klVector<TYPE>&& src)
		 : _mMemory(src._mMemory), _own(src._own), _size(src._size),  _mgr(src._mgr),  x0(src.x0),
		  x1(src.x1),  y0(src.y0), y1(src.y1), desc(src.desc)
	  {
#ifdef _DEBUG
		  __int64 byteCount = _size * sizeof(TYPE);
		  globalKlVectorMoveConstructorBytesCount +=byteCount;
		  cerr<<"klVector(klVector<TYPE>&& src) call count = "<<globalKlVectorMoveConstructorCallCount++<<" Bytes Count "<<globalKlVectorCopyConstructorBytesCount<<endl; 
#endif
		  src._mMemory=NULL;
		  src._own=0;
		  src._size=0;
		  src. _mgr=0;
		  src.x0=0;
		  src.x1=0;
		  src.y0=0;
		  src.y1=0;
		  src.desc="";
	  }

	  //Computes this.^b.  Template specializations utilizing MKL VSL are implemented for TYPE double and TYPE float 
	  klVector<TYPE> pow_gen(klVector<TYPE> b)
	  {
		  //this is for integer type
		  if(b.getRowSize()!=_size)
			  throw "klVector::pow_gen ERROR: called with wrong dimension on the exponent vector";
		  TYPE* ans=new TYPE[_size];
		  int i;
		  for(i=0;i<_size;i++)
		  {
			  *(ans+(i))=std::pow(this->operator[](i),b[i]);
		  }
		  return klVector<TYPE>(ans,_size,true);

	  }

	  klVector<TYPE> pow_alpha(double alpha)
	  {
		  TYPE* ans=new TYPE[_size];
		  int i;
		  for(i=0;i<_size;i++)
		  {
			  *(ans+(i))=std::pow(this->operator[](i),alpha);
		  }
		  return klVector<TYPE>(ans,_size,true);

	  }

	  //Returns a histogram over [a,b]
	  //Get the full range of the data by calling klVector<TYPE>::setupRange()
	  //and retrieving it from  klVector<TYPE>::x0 klVector<TYPE>::x1.
	  klVector<TYPE> histogram(__int64 histogramBins,double a,double b)
	  {
		  if(b<=a)
			  throw "klVector: ERROR : histogram called with incorrect domain";
		  double binSize = double(b-a)/histogramBins; 
		  TYPE* histogram=new TYPE[histogramBins];
		  for(__int64 i = 0; i < histogramBins; i++)
		  {
			  histogram[i]=0;
		  }

		  for(__int64 i = 0; i < this->getRowSize(); i++)
		  {
			  //int bin=int(double(this->operator[](i))/binSize);
			  TYPE d0=this->operator[](i);
			  TYPE d1=d0-(double)a;;
			  TYPE d2=d1/binSize;
			  if(d2<0) //safety check - if the range is set up right - this will not be a problem.
				  d2=0;
			  if(d2>=histogramBins)//safety check - if the range is set up right - this will not be a problem.
				  d2=histogramBins-1;
			  histogram[int(d2)]++;

		  }
		  return klVector<TYPE>(histogram,histogramBins,true);

	  }

	  klVector<TYPE> sort(bool ascending=true)
	  {
		  std::list<TYPE> sortl(0);
		  for(__int64 i = 0; i < this->getRowSize(); i++)
		  {
			  TYPE t=this->operator[](i);
			  sortl.push_back(t);
		  }
		  klVector<TYPE> ret(this->getRowSize() );

		  if(ascending)
		  {
			  sortl.sort();
		  }
		  else
		  {
			  sortl.reverse();
		  }


		  std::list<TYPE>::iterator position;
		  int i=0;
		  for(position=sortl.begin();position!=sortl.end();position++,i++)
		  {
			  ret[i]=*(position);
		  }
		  return ret;


	  }
	  //This releases the memory to be freed elsewhere.
	  //Use with caution.
	  void setOwn(bool set)
	  {
		  _own=set;
	  }

	  //When mem!=0, we never own the memory.
	  void setup(__int64 size,klMemMgr* mgr=0,TYPE* mem=0)
	  {
		  _size=size;
		  if(!mem)
		  {

			  if(_own && _mMemory && _mgr)
			  {
				  _mgr->free(_mMemory);
				  _mMemory=NULL;
			  }
			  if(_own && _mMemory && !_mgr)
			  {
				  delete _mMemory;
				  _mMemory=NULL;
			  }


			  if(mgr)
				  _mMemory=(TYPE*)mgr->allocate(size*sizeof(TYPE) );
			  else
				  _mMemory=new TYPE[size];
			  _own=1;
		  }
		  else
		  {
			  if(_own && _mMemory && _mgr)
				  _mgr->free(_mMemory);
			  if(_own && _mMemory && !_mgr)
				  delete _mMemory;

			  _mMemory=mem;
			  _own=0;
		  }

		  _size=size;
		  _mgr=mgr;
		  return;
	  }

	  __int64 getRowSize() const
	  {
		  return _size;
	  }

	  __int64 getColumns() const
	  {
		  return _size;
	  }
	  
	  __int64 getRows() const
	  {
		  return _size;
	  }

	  TYPE* getMemory() const 
	  {
		  return _mMemory;
	  }

	  klMemMgr* getMemMgr() const 
	  {
		  return _mgr;
	  }

	  ~klVector()
	  {
		  if(_own==1)
		  {
			  if(!_mgr)
				  delete _mMemory;
			  else
				  _mgr->free(_mMemory);
		  }
	  }
	  
	  //  The distance between \f$(x_1,y_1)\f$ and \f$(x_2,y_2)\f$ is 
      //  \f$\sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$.
	  TYPE dot(klVector<TYPE>& rhs) const
	  {
		  if(rhs.getRowSize()!=_size)
			  throw "klVector ERROR: Trying to take the dot product of vectors with incompatible dimension";
		  int i;
		  TYPE ans=0;
		  for(i=0;i<_size;i++)
		  {
			  ans+=*(_mMemory+i) * rhs[i];
		  }
		  return ans;
	  }
	  
	  TYPE dotBLAS(klVector<TYPE>& rhs) const
	  {
		  if(rhs.getRowSize()!=_size)
			  throw "klVector ERROR: Trying to take the dot product of vectors with incompatible dimension";

		  int i;
		  TYPE ans=0;

		  cerr<<"Calling dotBLAS with a type that has no template specialization"<<endl;//typename(TYPE)<<"   "<<typeid(TYPE)<<endl;
		  ans = dot(rhs);

		  return ans;
	  }
	  
	  TYPE& operator[](__int64 subscript) const 
	  {
		  if(subscript<_size)
			  return *(_mMemory+(subscript));
		  else
			  throw "klVector ERROR: Memory index out of range.";
	  }

	  klVector<TYPE>& operator=(const klVector<TYPE>& src)
	  {
		  setup(src.getRowSize(),src._mgr);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  _mMemory[i]=src._mMemory[i];
		  return *this;
	  }

	  //This returns the vector of differences spaced by delta elements
	  klVector<TYPE> diff(__int64 delta)
	  {
		  if(delta>_size-1)
			  throw "klvector<TYPE> diff(__int64 delta) ERROR: not enough elements to do the diff.";
		  __int64 i;
		  klVector<TYPE> r(_size-delta);
		  for(i=0;i<_size-delta;i++)
		  {
			  r[i]=_mMemory[i+delta] - _mMemory[i];
		  }
		  return r;
	  }

	  klVector<TYPE> absv()
	  {
		  __int64 i;
		  klVector<TYPE> r(_size);
		  for(i=0;i<_size;i++)
		  {
			  r[i]=abs(_mMemory[i]);
		  }
		  return r;
	  }

	  TYPE sum()
	  {
		  __int64 i;
		  TYPE rsum=0;
		  for(i=0;i<_size;i++)
		  {
			  rsum+=_mMemory[i];
		  }
		  return rsum;
	  }
	  
	  size_t precision() const
	  {
		  return sizeof(TYPE);
	  } 

	  //sets the public variables x0,x1 to be the range of the data
	  void setupDomain(double a,double b)
	  {
		  x0=a;
		  x1=b;
	  }

	  //sets the public variables y0,y1 to be the range of the data
	  void setupRange()
	  {
		  double tmpx0=DBL_MAX;

		  double tmpx1=DBL_MIN;

		  __int64 i;
		  if(getColumns()==0)
			  throw "klvector:setupRange ERROR : no data in vetor.";

		  for(i=0;i<getColumns();i++)
		  {
			  double t=_mMemory[i];
			  if(t<tmpx0)
				  tmpx0=t;
			  if(t>tmpx1)
				  tmpx1=t;

		  }
		  y0=tmpx0;
		  y1=tmpx1;


	  }

	  //Set all values to  the sclar c
	  void operator=(const TYPE c)
	  {
		  __int64 i;
		  for(i=0;i<_size;i++)
			  _mMemory[i]=c;

	  }

	  // Elementwise equal to the scalar
	  klVector<bool> operator==(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]==c);
		  return ans;
	  }
	  
	  // Elementwise not-equal to the scalar
	  klVector<bool> operator!=(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]!=c);
		  return ans;

	  }
	  
	  // Elementwise less than the scalar
	  klVector<bool> operator<(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]<c);
		  return ans;

	  }
	  
	  // Elementwise less than and equal to the scalar
	  klVector<bool> operator<=(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]<=c);
		  return ans;

	  }
	  
	  // Elementwise greater than the scalar
	  klVector<bool> operator>(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]>c);
		  return ans;

	  }

	  // Elementwise greater than and equal to the scalar
	  klVector<bool> operator>=(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  __int64 i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]>=c);
		  return ans;

	  }

	  // Compare two klVectors False if wrong sizes or different values
	  bool operator==(const klVector<TYPE> &v) const
	  {
		  if(_size!=v.getRows())
			  return false;
		  bool ans;
		  __int64 i;
		  for(i=0;i<_size;i++)
		  {
			  ans=(_mMemory[i]==v[i]);
			  if(ans==false)
				  return false;
		  }
		  return ans;

	  }

	  bool isPositive()
	  {
		  bool ans=true;
		  __int64 i;
		  for(i=0;i<_size;i++)
		  {
			  ans=(_mMemory[i]<=0);
			  if(ans==false)
				  return false;
		  }
		  return true;

	  }

public:
	
	//Domain of data
	double x0,x1;

	//Range of data - typically set by calling setupRange()
	double y0,y1;

	string desc;

private:
	TYPE* _mMemory;
	__int64 _size;
	mutable __int64 _own;
	klMemMgr* _mgr;
};

template<  > float klVector<float>::dotBLAS(klVector<float>& rhs) const
{
	if(rhs.getRowSize()!=_size)
		throw "klVector ERROR: Trying to take the dot product of vectors with incompatible dimension";
	int i;
	float ans=0;

	MKL_INT N=_size;
	const float  *X=this->getMemory();
	const MKL_INT incX=1;
	const float  *Y=rhs.getMemory();
	const MKL_INT incY=1;
	ans= cblas_sdot ( N, X, incX, Y, incY);

	return ans;
}

template<  > double klVector<double>::dotBLAS(klVector<double>& rhs) const
{
	if(rhs.getRowSize()!=_size)
		throw "klVector ERROR: Trying to take the dot product of vectors with incompatible dimension";
	int i;
	double ans=0;

	MKL_INT N=_size;
	const double  *X=this->getMemory();
	const MKL_INT incX=1;
	const double  *Y=rhs.getMemory();
	const MKL_INT incY=1;
	ans= cblas_ddot ( N, X, incX, Y, incY);

	return ans;

}

//Computes this.^b elementwise
template<  > klVector<float> klVector<float>::pow_gen(klVector<float> b)
{
	if(b.getRowSize()!=_size)
		throw "klVector::pow ERROR :called with wrong dimension on the exponent vector";
	//Calls single precision mkl vector math library methods.
	float* ans=new float[_size];
	vsPow(_size,_mMemory,b.getMemory(),ans);
	return klVector<float>(ans,_size,true);

}

//Computes this.^b elementwise
template<  > klVector<double> klVector<double>::pow_gen(klVector<double> b)
{
	if(b.getRowSize()!=_size)
		throw "klVector::pow  ERROR called with wrong dimension on the exponent vector";
	//Calls double precision mkl vector math library methods.
	double* ans=new double[_size];
	vdPow(_size,_mMemory,b.getMemory(),ans);
	return klVector<double>(ans,_size,true);

}

//Computes this^\alpha - template spoecialization using MKL VSL
template<  > klVector<double> klVector<double>::pow_alpha(double alpha)
{
	double* ans=new double[_size];
	vdPowx (_size,_mMemory,alpha,ans);
	return klVector<double>(ans,_size,true);

}
//Computes this^\alpha - template spoecialization using MKL VSL
template<  > klVector<float> klVector<float>::pow_alpha(double alpha)
{
	float* ans=new float[_size];
	vsPowx (_size,_mMemory,alpha,ans);
	return klVector<float>(ans,_size,true);

}

//Use floating point types
template<class TYPE  > klVector<TYPE> logReturns(klVector<TYPE> b)
{
	klVector<TYPE> t(b.getRowSize() -1);
	__int64 i;
	for(i=0;i<t.getRowSize();i++)
	{
		t[i]=log(b[i+1])-log(b[i]);
	}
	return t;
}

//klVector stream io.  Operator << >> overrides for klVector class
template <class TYPE> static ostream& operator<<(ostream& str, const klVector<TYPE>& v) {
	int i = v.getRowSize();

	for (int j=0; j<i; j++) 
	{
		if(j!=i-1)
			str << v[j]<<", ";
		else
			str<<v[j];
	}

	return str<<"\n";
}

template <class TYPE> static inline istream& operator>>(istream& c, klVector<TYPE> & v) {
	char ch;
	int i=0;
	do {

		TYPE d;
		c >> d;
		v[i]=d;
		i++;
		c >> ch;
	} while (i<v.getRowSize());
	return c;
}

// Addition of two klVectors
template<class TYPE>  klVector<TYPE> operator+(const klVector<TYPE> &v1, const klVector<TYPE> &v2)
{
	__int64 i;
	if(v1.getColumns() != v2.getColumns() )
		throw "template<class TYPE> const klVector<TYPE> operator+(const klVector<TYPE> &v1, const klVector<TYPE> &v2) ERROR : bad dimensions";

	klVector<TYPE> c(v1.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v1[i]+v2[i];
	}
	return c;
}

// Addition of a klVector and a scalar
template<class TYPE>  klVector<TYPE> operator+(const klVector<TYPE> &v, const TYPE t)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]+t;
	}
	return c;
}
// Addition of a scalar and a klVector
template<class TYPE>  klVector<TYPE> operator+(const TYPE t, const klVector<TYPE> &v)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]+t;
	}
	return c;
}

// Subtraction of a klVector from a klVector
template<class TYPE>  klVector<TYPE> operator-(const klVector<TYPE> &v1, const klVector<TYPE> &v2)
{
	__int64 i;
	if(v1.getColumns() != v2.getColumns() )
		throw "template<class TYPE> const klVector<TYPE> operator+(const klVector<TYPE> &v1, const klVector<TYPE> &v2 ERROR: bad dimensions";

	klVector<TYPE> c(v1.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v1[i]-v2[i];
	}
	return c;
}

// Subtraction of a scalar from a klVector
template<class TYPE>  klVector<TYPE> operator-(const klVector<TYPE> &v, const TYPE t)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]-t;
	}
	return c;
}
// Subtraction of klVector from scalar. Results in a klVector
template<class TYPE>  klVector<TYPE> operator-(const TYPE t, const klVector<TYPE> &v)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]-t;
	}
	return c;
}

// Negation of klVector
template<class TYPE>  klVector<TYPE> operator-(const klVector<TYPE> &v)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=-v[i];
	}
	return c;
}

// Inner (dot) product of two klVectors v1 and v2
// template<class TYPE> const TYPE dot(const klVector<TYPE> &v1, const klVector<TYPE> &v2)

// Inner (dot) product of two klVectors v1 and v2
//template<class TYPE> const TYPE operator*(const klVector<TYPE> &v1, const klVector<TYPE> &v2)

// Outer product of two klVectors v1 and v2
//template<class TYPE> const Mat<TYPE> outer_product(const klVector<TYPE> &v1, const klVector<TYPE> &v2);


// Multiplication of a klVector and a scalar
template<class TYPE>  klVector<TYPE> operator*(const klVector<TYPE> &v, const TYPE t)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]*t;
	}
	return c;
}

// Multiplication of a scalar and a klVector. Results in a klVector
template<class TYPE>  klVector<TYPE> operator*(const TYPE t, const klVector<TYPE> &v)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]*t;
	}
	return c;
}

// Subtraction of a klVector from a klVector
template<class TYPE>  klVector<TYPE> operator*(const klVector<TYPE> &v1, const klVector<TYPE> &v2)
{
	__int64 i;
	if(v1.getColumns() != v2.getColumns() )
		throw "template<class TYPE> const klVector<TYPE> operator*(const klVector<TYPE> &v1, const klVector<TYPE> &v2) ERROR: bad size";

	klVector<TYPE> c(v1.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v1[i]*v2[i];
	}
	return c;
}

template<class TYPE>  klVector<TYPE> operator/(const klVector<TYPE> &v1, const klVector<TYPE> &v2)
{
	__int64 i;
	if(v1.getColumns() != v2.getColumns() )
		throw " template<class TYPE> const klVector<TYPE> operator/(const klVector<TYPE> &v1, const klVector<TYPE> &v2) ERROR: bad size";

	klVector<TYPE> c(v1.getColumns() );

	for(i=0;i<c.getColumns();i++)
	{
		if(v2[i]==0)
			throw " template<class TYPE> const klVector<TYPE> operator/(const klVector<TYPE> &v1, const klVector<TYPE> &v2) ERROR: attempting to divide by zero";

		c[i]=v1[i]/ v2[i];
	}
	return c;
}

// Elementwise multiplication of the two klVectors
//  template<class TYPE> const klVector<TYPE> elem_mult(const klVector<TYPE> &v1, const klVector<TYPE> &v2);
// Elementwise multiplication of the three klVectors
//  template<class TYPE> const klVector<TYPE> elem_mult(const klVector<TYPE> &v1, const klVector<TYPE> &v2, const klVector<TYPE> &v3);
// Elementwise multiplication of the four klVectors
//  template<class TYPE> const klVector<TYPE> elem_mult(const klVector<TYPE> &v1, const klVector<TYPE> &v2, const klVector<TYPE> &v3, const klVector<TYPE> &v4);
template<class TYPE>  klVector<TYPE> operator/(const klVector<TYPE> &v, const TYPE t)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	if(t==0)
		throw " template<class TYPE> const klVector<TYPE> operator/(const klVector<TYPE> &v, const TYPE t) ERROR: attempting to divide by zero";

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]/t;
	}
	return c;

}

template<class TYPE>  klVector<TYPE> operator/(const TYPE t, const klVector<TYPE> &v)
{
	__int64 i;

	klVector<TYPE> c(v.getColumns() );

	if(t==0)
		throw "template<class TYPE> const klVector<TYPE> operator/(const TYPE t, const klVector<TYPE> &v) ERROR attempting to divide by zero";

	for(i=0;i<c.getColumns();i++)
	{
		c[i]=v[i]/t;
	}
	return c;

}

// Elementwise division
//template<class TYPE> const klVector<TYPE> elem_div(const klVector<TYPE> &v1, const klVector<TYPE> &v2);
// Elementwise division of scalar \c t and klVector \c v
//template<class TYPE> const klVector<TYPE> elem_div(const TYPE t, const klVector<TYPE> &v);

// Append element \c a to the end of the klVector \c v
//template<class TYPE> const klVector<TYPE> concat(const klVector<TYPE> &v, const TYPE a);
// Concat element \c a to the beginning of the klVector \c v
//template<class TYPE> const klVector<TYPE> concat(const TYPE a, const klVector<TYPE> &v);
// Concat klVectors \c v1 and \c v2
//template<class TYPE> const klVector<TYPE> concat(const klVector<TYPE> &v1,const klVector<TYPE> &v2);
// Concat klVectors \c v1, \c v2 and \c v3
//template<class TYPE> const klVector<TYPE> concat(const klVector<TYPE> &v1, const klVector<TYPE> &v2, const klVector<TYPE> &v3);
// Concat klVectors \c v1, \c v2, \c v3 and \c v4
//template<class TYPE> const klVector<TYPE> concat(const klVector<TYPE> &v1, const klVector<TYPE> &v2, const klVector<TYPE> &v3, const klVector<TYPE> &v4);
// Concat klVectors \c v1, \c v2 \c v3, \c v4 and \c v5
//template<class TYPE> const klVector<TYPE> concat(const klVector<TYPE> &v1, const klVector<TYPE> &v2, const klVector<TYPE> &v3, const klVector<TYPE> &v4, const klVector<TYPE> &v5);

template <class TYPE> inline klVector<TYPE> RE(klVector<complex<TYPE > >  vec)
{
	//Real part of first eig
	klVector<TYPE> L_re(vec.getColumns() );

	__int64 j=0;
	for(j=0;j<vec.getColumns();j++)
	{
		L_re[j]=vec[j].real();
	}
	return L_re;
}

template <class TYPE> inline klVector<TYPE> IM(klVector<complex<TYPE > >  vec)
{
	//Real part of first eig
	klVector<TYPE> L_im(vec.getColumns() );

	__int64 j=0;
	for(j=0;j<vec.getColumns();j++)
	{
		L_im[j]=vec[j].imag();
	}
	return L_im;
}

inline klVector<complex<double> >  klApplyLog( const klVector<complex<double> > &c)
{
	klVector<complex<double> > r(c.getColumns() );
	unsigned int i;
	for (i=0;i<c.getColumns();i++)
		r[i]=complex<double> (std::log(complex<double> (c[i])));
	return r;
}

//Some simple ref counted classes used in algorithm parameter design pattern

class klRCInt :public klRefCount<klMutex>
{
public:

	__int64 intV;	
};
typedef klSmartPtr<klRCInt >  klIntPtr;

class klRCDouble:public klRefCount<klMutex>
{
public:
	double doubleV;
};
typedef klSmartPtr<klRCDouble >  klDoublePtr;


class klRCString:public klRefCount<klMutex>
{
public:
	std::string stringV;
};
typedef klSmartPtr<klRCString >  klStringPtr;


//For convenience we have short name versions of the smart pointers

typedef klSmartPtr<klVector<double> >  klDoubleVectorPtr;

typedef klSmartPtr<klVector<complex<double> > >  klComplexDoubleVectorPtr;

typedef klSmartPtr<klVector<complex<float> > >  klComplexFloatVectorPtr;

typedef klSmartPtr<klVector<float> >  klFloatVectorPtr;

#endif __kl_vector__
