#ifndef __kl_vector__
#define __kl_vector__

#include "mkl.h"
#include "kl_memory.h"
#include "kl_util.h"
#include "kl_matrix.h"
#include <limits.h>
#include <float.h>
#include <string>
#include <vector>
#include <list>
#include <complex>
using namespace std;

inline int mkl_eigs_select(double* x,double* y)
{
	return (*x<=*y) ? true: false;

}

	  ///  The distance between \f$(x_1,y_1)\f$ and \f$(x_2,y_2)\f$ is 
      ///  \f$\sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$.
template<class TYPE> class klVector: public klRefCount<klMutex>
{
public:

	klVector(klMemMgr* mgr,unsigned int size,bool own=false) :
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


	  klVector(TYPE* mem,unsigned int size,bool own=false) :
	  x0(0),x1(0), y0(0),y1(0),desc("")
	  {
		  _mMemory=mem;
		  _own=own;
		  _size=size;
		  _mgr=0;
	  }
	  klVector(unsigned int size) :
	  x0(0),x1(0), y0(0),y1(0),desc("")
	  {
		  _mMemory=new TYPE[size];
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
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  _mMemory[i]=src._mMemory[i];
		  x0=src.x0;
		  x1=src.x1;
		  y0=src.y0;
		  y1=src.y1;
		  desc=src.desc;
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
	  klVector<TYPE> histogram(unsigned int histogramBins,double a,double b)
	  {
		  if(b<=a)
			  throw "klVector: ERROR : histogram called with incorrect domain";
		  double binSize = double(b-a)/histogramBins; 
		  TYPE* histogram=new TYPE[histogramBins];
		  for(unsigned int i = 0; i < histogramBins; i++)
		  {
			  histogram[i]=0;
		  }

		  for(unsigned int i = 0; i < this->getRowSize(); i++)
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
		  for(unsigned int i = 0; i < this->getRowSize(); i++)
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
	  void setup(unsigned int size,klMemMgr* mgr=0,TYPE* mem=0)
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

	  unsigned int getRowSize() const
	  {
		  return _size;
	  }

	  unsigned int getColumns() const
	  {
		  return _size;
	  }
	  unsigned int getRows() const
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
	  ///  The distance between \f$(x_1,y_1)\f$ and \f$(x_2,y_2)\f$ is 
      ///  \f$\sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$.
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


	  TYPE& operator[](unsigned int subscript) const 
	  {
		  if(subscript<_size)
			  return *(_mMemory+(subscript));
		  else
			  throw "klVector ERROR: Memory index out of range.";
	  }

	  klVector<TYPE>& operator=(const klVector<TYPE>& src)
	  {
		  setup(src.getRowSize(),src._mgr);
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  _mMemory[i]=src._mMemory[i];
		  return *this;
	  }

	  //This returns the vector of differences spaced by delta elements
	  klVector<TYPE> diff(unsigned int delta)
	  {
		  if(delta>_size-1)
			  throw "klvector<TYPE> diff(unsigned int delta) ERROR: not enough elements to do the diff.";
		  unsigned int i;
		  klVector<TYPE> r(_size-delta);
		  for(i=0;i<_size-delta;i++)
		  {
			  r[i]=_mMemory[i+delta] - _mMemory[i];
		  }
		  return r;
	  }



	  klVector<TYPE> absv()
	  {
		  unsigned int i;
		  klVector<TYPE> r(_size);
		  for(i=0;i<_size;i++)
		  {
			  r[i]=abs(_mMemory[i]);
		  }
		  return r;
	  }

	  TYPE sum()
	  {
		  unsigned int i;
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

		  unsigned int i;
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
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  _mMemory[i]=c;

	  }

	  // Elementwise equal to the scalar
	  klVector<bool> operator==(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]==c);
		  return ans;
	  }
	  // Elementwise not-equal to the scalar
	  klVector<bool> operator!=(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]!=c);
		  return ans;

	  }
	  // Elementwise less than the scalar
	  klVector<bool> operator<(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]<c);
		  return ans;

	  }
	  // Elementwise less than and equal to the scalar
	  klVector<bool> operator<=(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]<=c);
		  return ans;

	  }
	  // Elementwise greater than the scalar
	  klVector<bool> operator>(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  unsigned int i;
		  for(i=0;i<_size;i++)
			  ans[i]=(_mMemory[i]>c);
		  return ans;

	  }

	  // Elementwise greater than and equal to the scalar
	  klVector<bool> operator>=(const TYPE c) const
	  {
		  klVector<bool> ans(_size);
		  unsigned int i;
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
		  unsigned int i;
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
		  unsigned int i;
		  for(i=0;i<_size;i++)
		  {
			  ans=(_mMemory[i]<=0);
			  if(ans==false)
				  return false;
		  }
		  return true;

	  }

public:
	//Used by data display for x range, y range of data.
	//bbcrevisit - make a struct for dimension and add get/set accessors 
	//to vector matrix and cube classes.

	//Domain of data
	double x0,x1;

	//Range of data - typically set by calling setupRange()
	double y0,y1;

	string desc;


private:
	TYPE* _mMemory;
	unsigned int _size;
	mutable unsigned int _own;
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
	unsigned int i;
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
	unsigned int i;
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
	unsigned int i;

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
	unsigned int i;

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
	unsigned int i;
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
	unsigned int i;

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
	unsigned int i;

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
	unsigned int i;

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
	unsigned int i;

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
	unsigned int i;

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
	unsigned int i;
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
	unsigned int i;
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
	unsigned int i;

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
	unsigned int i;

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

	unsigned int j=0;
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

	unsigned int j=0;
	for(j=0;j<vec.getColumns();j++)
	{
		L_im[j]=vec[j].imag();
	}
	return L_im;
}



#endif __kl_vector__