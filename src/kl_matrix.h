 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#ifndef __kl_matrix__
#define __kl_matrix__

#include "mkl.h"
#include "kl_memory.h"
#include "kl_util.h"
#include "kl_vector.h"
#include <limits.h>
#include <float.h>
#include <string>
#include <vector>
#include <list>
#include <complex>
using namespace std;

template<class TYPE> class klMatrix: public klRefCount<klMutex>
{
public:
	klMatrix(TYPE* mem,unsigned int row,unsigned int col,bool own=0) 
	{
		unsigned int i;
		_vectors=new klVector<TYPE>[row];
		for(i=0;i<row;i++)
			(_vectors+i)->setup(col,0,mem+i*col);
		if(own)
			_own=1;
		else
			_own=0;
		_row=row;
		_col=col;
		_memory=mem;
		_contiguous=true;
		_mgr=0;



	}

	klMatrix(klMemMgr* mgr, unsigned int row, unsigned int col) 
	{                   
		_memory=(TYPE*)mgr->allocate(row*col*sizeof(TYPE) );
		_mgr=mgr;
		unsigned int i;
		_vectors=new klVector<TYPE>[row];
		for(i=0;i<row;i++)
			(_vectors+i)->setup(col,0,_memory+i*col);
		_own=1;
		_row=row;
		_col=col;
		_contiguous=true;

	}

	klMatrix(unsigned int row,unsigned int col) 
	{
		_memory=new TYPE[row*col];
		unsigned int i;
		_vectors=new klVector<TYPE>[row];
		for(i=0;i<row;i++)
			(_vectors+i)->setup(col,0,_memory+i*col);
		_own=1;
		_row=row;
		_col=col;
		_contiguous=true;
		_mgr=0;

	}

	klMatrix() 
	{
		_memory=NULL;
		_mgr=0;
		_col=0;
		_row=0;
		_contiguous=0;
		_own=0;
		_vectors=0;
	}


	klMatrix(const klMatrix<TYPE>& src)
	{
		_memory=NULL;
		_mgr=src._mgr;
		_row=src._row;
		_col=src._col;
		_contiguous=true;
		_own=1;
		setup(_row,_col,_mgr);
		unsigned int i,j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)=src[i][j];
			}
		}
	}

	~klMatrix()
	{

		if(_own)
		{
			if(_mgr)
				_mgr->free(_memory);
			else
				delete _memory;
			delete[] _vectors;
		}
		else
		{
			delete[] _vectors;
		}
	}

	klMatrix<TYPE>& operator=(const klMatrix<TYPE>& src)
	{
		_row=src._row;
		_col=src._col;
		_contiguous=true;
		setup(_row,_col,src._mgr);
		_mgr=src._mgr;//don't step on our mgr until we've had the opportunity to free resources

		unsigned int i,j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)=src[i][j];
			}
		}
		return *this;
	}

	klVector<TYPE>& operator[](unsigned int row) const 
	{
		if(row<_row )
			return _vectors[row]; 
		else
			throw " klVector<TYPE>& operator[](unsigned int row) const ERROR: Memory index out of range in klMatrix";
	}

	klMatrix<TYPE> operator*(klMatrix<TYPE> a) const
	{
		if(_col!=a.getRows())
			throw "klMatrix<TYPE> operator*(klMatrix<TYPE> a) const ERROR: invalid dimension in operator* overlaoad";
		klMatrix<TYPE> product;
		if(_mgr)
			product.setup(_row,a.getColumns(),_mgr);
		else 
			product.setup(_row,a.getColumns(),0);

		unsigned int i,j,k;
//#pragma omp parallel num_threads(4)
		for(i=0;i<_row;i++)
		{
			for(j=0;j<a.getColumns();j++)
			{
				TYPE temp=0;
				for(k=0;k<a.getRows();k++)
					temp+= (_vectors+i)->operator[](k)* a[k][j];
				product[i][j]=temp;

			}
		}
		return product;
	}
	klVector<TYPE> operator*(klVector<TYPE> a) const
	{
		if(_col!=a.getRowSize())
			throw "klVector<TYPE> klMatrix ::operator*(klVector<TYPE> a) const ERROR : invalid dimension in operator* overlaoad";
		klVector<TYPE> product;
		if(_mgr)
			product.setup(_row,_mgr);
		else 
			product.setup(_row);
		unsigned int i,j;
		for(i=0;i<_row;i++)
		{
			TYPE temp=0;
			for(j=0;j<_col;j++)
			{
				temp+=(_vectors+i)->operator[](j) * a[j];
			}
			product[i]=temp;
		}
		return product;
	}

	//Sets the entries of this klMatrix to c.
	klMatrix<TYPE>& operator=(TYPE c) 
	{
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)=c;
			}

		}

		return *this;
	}

	klMatrix<TYPE>& operator+=(TYPE c)
	{     
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)+=c;
			}

		}

		return *this;
	}

	klMatrix<TYPE>& operator+=(const klMatrix<TYPE> &c)
	{
		if(_row!=c.getRows() || _col !=c.getColumns())
			throw "klMatrix<TYPE>& klMatrix<TYPE>::operator+=(const klMatrix<TYPE> &c) ERROR: incompatible dimensions.";
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)+=c[i][j];
			}

		}
		return *this;
	}

	klMatrix<TYPE>& operator-=(const klMatrix<TYPE> &c)
	{
		if(_row!=c.getRows() || _col !=c.getColumns())
			throw "klMatrix<TYPE>& klMatrix<TYPE>::operator+=(const klMatrix<TYPE> &c) ERROR: incompatible dimensions.";
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)-=c[i][j];
			}

		}
		return *this;
	}

	klMatrix<TYPE>& operator-=(TYPE c)
	{
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)-=c;
			}

		}
		return *this;
	}


	klMatrix<TYPE>& operator*=(const klMatrix<TYPE> &a)
	{
		if(_col!=a.getRows())
			throw "klMatrix ERROR: invalid dimension in operator* overlaoad";
		klMatrix<TYPE> product;
		if(_mgr)
			product.setup(_row,a.getColumns(),_mgr);
		else 
			product.setup(_row,a.getColumns(),0);

		unsigned int i,j,k;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<a.getColumns();j++)
			{
				TYPE temp=0;
				for(k=0;k<_row;k++)
					temp+= (_vectors+i)->operator[](k)* a[k][j];
				product[i][j]=temp;

			}
		}
		return product;
	}

	klMatrix<TYPE>& operator*=(TYPE c)
	{

		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)*=c;
			}

		}
		return *this;
	}

	klMatrix<TYPE>& operator/=(TYPE c)
	{
		if(c==0)
			throw "klMatrix<TYPE>& klMatrix<TYPE>::operator/=(TYPE c): ERROR: attempting to divide by zero .";
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				(_vectors+i)->operator[](j)/=c;
			}

		}
		return *this;

	}

	//Elementwise revisit whether this should be invert and multiply.
	klMatrix<TYPE>& operator/=(const klMatrix<TYPE> &c)
	{

		if(_row!=c.getRows() || _col !=c.getColumns())
			throw "klMatrix<TYPE>& klMatrix<TYPE>::operator+=(const klMatrix<TYPE> &c) ERROR: incompatible dimensions.";
		unsigned int i;
		unsigned int j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				if(c[i][j]==0)
					throw "klMatrix<TYPE>& operator/=(const klMatrix<TYPE> &c) ERROR: attempting to divide by zero";
				(_vectors+i)->operator[](j)/=c[i][j];
			}

		}
		return *this;
	}
		
	void setRow(unsigned int j,klVector<TYPE> r)
	{
		if(r.getColumns() != _col)
			throw "klMatrix::setRow(unsigned int j,klVector<TYPE> r) ERROR: invalid dimension in setRow parameter";

		unsigned int i;

		for(i=0;i<_col;i++)
		{
			(_vectors+j)->operator[](i)=r[i];
		}
	}
	
	klMatrix<TYPE> transpose() const
	{
		klMatrix<TYPE> transpose;
		if(_mgr)
			transpose.setup(_col,_row,_mgr);
		else 
			transpose.setup(_col,_row,0);
		unsigned int i,j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{                   
				transpose[j][i]=(_vectors+i)->operator[](j);    
			}
		}
		//bbcrevisit verify there is no copy
		return transpose;
	}

	//Maps x[i][j]<low ----> low and  x[i][j]>high ----> high
	//Defaults of 
	void threshold(double low=DBL_MIN, double lowVal= DBL_MIN, double high=DBL_MAX,double highVal=DBL_MAX)
	{
		if(high<low)
			throw "klMatrix<TYPE> threshold(double low=DBL_MIN, double high=DBL_MAX) called with high<low";
		unsigned int i,j;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{                   
				TYPE c =(_vectors+i)->operator[](j);    
				if(c<low)
					(_vectors+i)->operator[](j)=lowVal + 0.0;//Add 0.0 to clear sign bit when clipping- bbcrevisit odd behavior
				if(c>high)
					(_vectors+i)->operator[](j)=highVal + 0.0;
			}
		}
	}
		
	//Calculates the determinant via LU decomp.  Implemented for float and double types
	//Calculating the determinanat via cofactors is O(n!)
	//An LU decomp, reduces to O(\frac{2 n^3}{3} )
	//If the matrix is SPD we can do a Choleski (LL) decomp. which is O(\frac{n^3}{3}) 
	complex<double> det() //const
	{
		//if(_row!=_col  )
		//	throw "klMatrix<TYPE>::det() ERROR: non square matrix";
		//klMatrix<TYPE> temp;
		//temp=this->transpose(); //deep copy 
		//unsigned int size=0;
		//unsigned int i,j;
		//unsigned int index=0;
		//char uplo='L';
		//int info=0;
		//int n=_row;
		//int m=_col;
		//size_t ipivSz=max(1,min(_row,_col));
		//int* ipiv=new int[ipivSz];

		////Calculating the determinanat via cofactors is O(n!)
		////An LU decomp, reduces to O(\frac{2 n^3}{3} )
		////If the matrix is SPD we can do a Choleski (LL) decomp. which is O(\frac{n^3}{3}) 
		//if(typeid(TYPE) ==typeid(float) ) 
		//	sgetrf(&m, &n, (float*)temp.getMemory(), &m, ipiv, &info);

		//if(typeid(TYPE) == typeid(double) )
		//	dgetrf(&m, &n, (double*)temp.getMemory(), &m, ipiv, &info);

		//if(info<0)
		//	throw "klMatrix::det ERROR: parameter error in MKL call to dgetrf.";
		//if(info>0)
		//	throw "klMatrix::det ERROR: error in MKL call to dgetrf.";
		//temp.transpose();
		//TYPE det=1;

		////Only the lower triangular part of _factor is written to by MKL, 
		////so we have to write 0's in the lower part.
		//for(i=0;i<_row;i++)
		//{
		//	det*=temp[i][i];
		//	for(j=0;j<_row;j++)
		//	{
		//		if(i<j)
		//			continue;
		//		else if(i!=j)
		//			temp[j][i]=0;
		//	}
		//}
		//The LU factorization of A allows the linear system A*x = b to be solved quickly with x = U\(L\b)
		//Determinants and inverses are computed from the LU factorization using det(A) = det(L)*det(U)
		//and inv(A) = inv(U)*inv(L).
		//You can also compute the determinants using det(A) = prod(diag(U)), 
		//though the signs of the determinants may be reversed.
		klVector<complex<double> > eigen=this->eigenvalues();
		complex<double>  detProd=eigen[0];
		for(int i=1;i<_row;i++)
		{
			detProd *= eigen[i];
		}
		//delete ipiv;
		return detProd;
	}

	//Returns the subblock (i,j):(k,l) indicated by the indices.
	klMatrix<TYPE> getSubBlock(unsigned int i,unsigned int j,unsigned int k,unsigned int l)
	{
		//First Verify the indices are in range
		if(k-i<0 || l-j<0 || k>_row || l>_col)
			throw "klMatrix<TYPE> klMatrix<TYPE>::getSubBlock ERROR index out of bounds.";
		klMatrix<TYPE> ret(k-i+1,l-j+1);
		unsigned int n,m;
		for(n=i;n<=k;n++)
			for(m=j;m<l;m++)
				ret[n-i][m-j]=(_vectors+m)->operator[](n);

		return ret;
	}

	//Sets the subblock (i,j):(k,l) indicated by the indices.
	void setSubBlock(klMatrix<TYPE> block,unsigned int i,unsigned int j)
	{
		//First Verify the indices are in range
		unsigned int k=block.getRows()+i-1;
		unsigned int l=block.getColumns()+j-1;
		if(k-i<=0 || l-j<=0 || k>_row || l>_col)
			throw "klMatrix<TYPE> klMatrix<TYPE>::getSubBlock ERROR index out of bounds.";
		unsigned int n,m;
		for(n=i;n<=k;n++)
			for(m=j;m<=l;m++)
				(_vectors+m)->operator[](n)=block[n-i][m-j];
	}
	
	void setColumn(unsigned int i,klVector<TYPE> v)
	{
		if (_row != v.getRowSize())
			throw "Bad dimensions in klMatrix::setRow";
		unsigned int n = 0;
		unsigned int m =i;
			for(n=0;n<=_row;n++)
				(_vectors+n)->operator[](i)=v[n];
	}

	//Calculates the matrix inverse for a square matrix.  Implemented for types double and float
	klMatrix<TYPE> inverse() const
	{
		if(_row!=_col  )
			throw "klMatrix<TYPE>::inverse() ERROR: non square matrix";
		klMatrix<TYPE> temp;
		temp=this->transpose(); //deep copy 
		unsigned int size=0;
		unsigned int i,j;
		unsigned int index=0;
		char uplo='L';
		int info=0;
		int n=_row;
		int m=_col;
		size_t ipivSz=max(1,min(_row,_col));
		int* ipiv=new int[ipivSz];
		//Calculating the determinanat via cofactors is O(n!)
		//An LU decomp, reduces to O(\frac{2 n^3}{3} )
		//If the matrix is SPD we can do a Choleski (LL) decomp. which is O(\frac{n^3}{3}) 
		if(typeid(TYPE) ==typeid(float) ) 
		{
			MKL_INT lwork =  2*n;
			float* work= new float[lwork];
			sgetrf(&m, &n, (float*)temp.getMemory(), &m, ipiv, &info);
			sgetri( &n, (float*)temp.getMemory(), &n, ipiv, work, &lwork, &info );
			delete work;
		}

		if(typeid(TYPE) == typeid(double) )
		{
			MKL_INT lwork =  2*n;
			double* work= new double[lwork];
			dgetrf(&m, &n, (double*)temp.getMemory(), &m, ipiv, &info);
			dgetri( &n, (double*)temp.getMemory(), &n, ipiv, work, &lwork, &info );
			delete work;
		}

		if(info<0)
			throw "klMatrix::inverse ERROR: parameter error in MKL call/";
		if(info>0)
			throw "klMatrix::inverse ERROR: error in MKL call.";
		
		return temp.transpose();

	}
		
	/*
		This method calls the LAPACK driver DGEES whichcomputes for an	N-by-N real nonsymmetric matrix	A, the eigenvalues,
		the real Schur form T, and, optionally, the matrix of	Schur vectors Z.
		This gives the Schur factorization A = Z*T*(Z**T).

		Optionally, it also orders the eigenvalues on	the diagonal of	the real
		Schur	form so	that selected eigenvalues are at the top left.	The leading
		columns of Z then form an orthonormal	basis for the invariant	subspace
		corresponding	to the selected	eigenvalues.

		A matrix is in real Schur form if it is upper	quasi-triangular with 1-by-1
		and 2-by-2 blocks. 2-by-2 blocks will	be standardized	in the form
		[  a	b  ]
		[  c	a  ]

		where	b*c < 0. The eigenvalues of such a block are a +- sqrt(bc).
	*/
	klVector<complex<double> > eigenvalues()
	{
		if(_row!=_col)
			throw "klVector<complex<double> > eigenvalues() ERROR: trying to calculate eigenvalues of non-square matrix.";

		klVector<complex<double> > eigenvalues(_row);

		//uses MKL LAPACK driver routine routine to calculate eigenvalues and Shur vectors
		//bbc revisit - we can define a select function that is used to order the eigenvalues
		//on the Shur form

		char jobvs='V';//Shur forms are computed

		//If sort = 'N', then eigenvalues are not ordered.
		//If sort = 'S', eigenvalues are ordered (see select).
		char sort='S';//don't use select function to order eigenvalues
		int n=_row;

		klMatrix<double> a;
		a.setup(_row,_col,_mgr);// bbc revisit - this is a problem if TYPE is not double 
		unsigned int i,j;
		for(i=0;i<_row;i++)
			for(j=0;j<_col;j++)
				a[i][j]=(_vectors+j)->operator [](i);
		int lwork=3*_col;
		double* work=new double[lwork];
		int lda=_row;
		int ldvs=_col;
		int* bwork=NULL;//not used if sort='N'

		//output arguments
		int sdim;//ouput is 0 if is sort='N', otherwise it equals the number of eigenvalues for which sort returned true
		double* wr=new double[n];//real part of eigenvalues, sorted by sort fn if we use it
		double* wi=new double[n];//imaginary part of eigenvalues, sorted by sort fn if we use it
		double* vs =new double[ldvs*n];//if jobvs this contains the orthogonal/unitary matrix of Shur vectors
		//work[1]=returns the required minimal size of work - so we can reduce work size in subsequent calls
		int info;

		int (*select)(double*,double*);
		//select=NULL;//empty function pointer
		select=mkl_eigs_select;

		dgees(&jobvs,&sort,select,&n,a.getMemory(),&lda,&sdim,wr,wi,vs,&ldvs,work,&lwork,bwork,&info);

		//bbs revisit - store Shur form
		for(i=0;i<_row;i++)
		{
			complex<double> e(*(wr+i),*(wi+i));
			eigenvalues[i]=e;
		}
		delete work;
		delete wr;
		delete wi;
		delete vs;
		return eigenvalues; 
	}

	//If the matrix is not square, we return \sum\limits_{i=0}^{min{_row,_col}}   
	TYPE trace()
	{
		unsigned int min=min(_row,_col);
		unsigned int i;
		TYPE r=0;
		for(i=0;i<min;i++)
		{
			r += (_vectors+i)->operator [](i);
		}
	}

	//See float and double specializations.
	//Defaults to the L1 norm, ellone=false computes the L\infty norm
	TYPE getConditionNumber(bool ellone=0)
	{
		return 0; 
	}
	
	//Returns the \Ell_p norm of the matrix.  This method is specialized for double & float.
	//Requires p \in [0,
	TYPE norm(bool ell_infty=0)
	{
		return 0;
	}

	//Returns the 0-based indexed column
	klVector<TYPE> getColumn(unsigned int col)
	{
		klVector<TYPE> column;
		if(_mgr)
			column.setup(_row,_mgr);
		else
			column.setup(_row);
		unsigned int i;
		for(i=0;i<_row;i++)
			column[i]=(_vectors+i)->operator[](col);
		return column;
	}

	
	unsigned int getRows() const
	{
		return _row;
	}

	
	unsigned int getColumns()const
	{
		return _col;
	}

	
	TYPE* getMemory() const 
	{
		return _memory;
	}

	
	klMemMgr* getMemMgr() const 
	{
		return _mgr;
	}

	
	bool isContiguous() const
	{
		return _contiguous;
	}

	klMatrix<TYPE> columnToRowMajor()
	{
		klMatrix<TYPE> ans(_row,_col);
		TYPE* pM=ans.getMemory();
		TYPE* pI=getMemory();
		unsigned int i=0;
		unsigned int j=0;
		unsigned int k=0;
		for(i=0;i<_row;i++)
		{
			for(j=0;j<_col;j++)
			{
				TYPE A = *(pI+j*_row+i);
				ans[i][j]=A;
			}
		}
		return ans;
	}
	
	//Initializes memory to NaN for types double and float
	void makeNanFriendly() 
	{
		if(sizeof(TYPE)==8)
		{
			unsigned long nan[2]={0xffffffff, 0x7fffffff};
			double x = *( double* )nan;
			bool check = _isnan(x);
			if (check)
			{
				//memset(_memory,0,row+col);
				//if exponent = 2e - 1 and fraction is not 0, the number being represented is not a number (NaN). 
				unsigned int i,j;
				for(i=0;i<_row;i++)
					for(j=0;j<_col;j++)
						(_vectors+j)->operator [](i)=x;
			}
		}
		if(sizeof(TYPE)==4)
		{
			unsigned short nan[2]={0xffff, 0x7fff};
			float x = *( float* )nan;
			bool check = _isnan(x);
			if (check)
			{
				//memset(_memory,0,row+col);
				//if exponent = 2e - 1 and fraction is not 0, the number being represented is not a number (NaN). 
				unsigned int i,j;
				for(i=0;i<_row;i++)
					for(j=0;j<_col;j++)
						(_vectors+j)->operator [](i)=x;
			}
		}
	}

	size_t precision() const
	{
		return sizeof(TYPE);
	}

	void setup(unsigned int row,unsigned int col,klMemMgr* mgr=0)
	{
		if(_own && _memory)
		{
			if(_mgr)
			{
				_mgr->free(_memory);
				_memory=NULL;
			}
			else
			{
				delete _memory;
				_memory=NULL;
			}
			delete[] _vectors;
		}

		_row=row;
		_col=col;
		_contiguous=true;
		_mgr=mgr;

		if(_mgr)
			_memory=(TYPE*)_mgr->allocate(_row*_col*sizeof(TYPE) );
		else
			_memory=new TYPE[_row*_col];
		_vectors=new klVector<TYPE>[_row];
		_own=true;

		unsigned int i;
		for(i=0;i<_row;i++)
			(_vectors+i)->setup(col,0,_memory+i*_col);
	}

	//An STL container for the user to fill with descriptive names
	vector<std::string> _dimensionNames;

	klMatrix<TYPE> upper()
	{
		klMatrix<TYPE> U(_row,_col);
		U=(TYPE)0.0;
		for(int i=0;i<_row;i++)
		{	
			for(int j =0;j<_col;j++)
			{
				if(i>j)
					continue;
				else
					U[i][j] = (_vectors+j)->operator [](i);
			}
		}
		return U;
	}
	
	klMatrix<TYPE> lower()
	{
		klMatrix<TYPE> L(_row,_col);
		L=(TYPE)0.0;
		for(int i=0;i<_row;i++)
		{	
			for(int j =0;j<_col;j++)
			{
				if(i<j)
					continue;
				else
					L[i][j] = (_vectors+j)->operator [](i);
			}
		}
		return L;
	}

	klVector<TYPE> diag()
	{
		klVector<TYPE> result(min(_row,_col));
		result =0;
		for(int i =0;i< min(_row,_col);i++)
		{
			result[i] = (_vectors+i)->operator [](i);
		}
		return result;
	}
protected:
	klVector<TYPE>* _vectors;
	mutable unsigned int _own;
	bool _contiguous;//bbcrevisit - contiguous is always true
	unsigned int _row;
	unsigned int _col;
	TYPE* _memory;
	klMemMgr* _mgr;
	string _filename;
};

inline klMatrix<double> mmBLAS(double alpha, klMatrix<double> a, klMatrix<double> b, double beta, klMatrix<double> c)
{
	if(a.getColumns()!=b.getRows())
		throw "double klMatrix<double>::mmBLAS const ERROR: invalid dimension";
	if(a.getRows() !=c.getRows() )
		throw "double klMatrix<double>::mmBLAS const ERROR: invalid dimension";
	if(b.getColumns() !=c.getColumns() )
		throw "double klMatrix<double>::mmBLAS const ERROR: invalid dimension";

	klMatrix<double> C=c;

	const char transa = 'T';
	const char transb = 'T';
	int m =c.getRows();
	MKL_INT *mp=&m;
	int n =b.getColumns();
	MKL_INT *np=&n;
	int k =a.getColumns();
	MKL_INT *kp=&k;
	const double *alphap=&alpha;
	const double *ap=(double*)a.getMemory();
	const MKL_INT *lda=kp;
	const double *bp=(double*)b.getMemory();
	const MKL_INT *ldb=np;
	const double *betap=&beta;
	double *cp=(double*)C.getMemory();
	const MKL_INT *ldc=mp;

	dgemm(&transa,&transb, mp, np, kp, alphap, ap, lda, bp, ldb, betap, cp, ldc);

	//In Fortran, column-major ordering of storage is assumed. This means that elements of the
	//same column occupy successive storage locations.
	//Three quantities are usually associated with a two-dimensional array argument: its leading
	//dimension, which specifies the number of storage locations between elements in the same row,
	//its number of rows, and its number of columns. For a matrix in full storage, the leading dimension
	//of the array must be at least as large as the number of rows in the matrix.
	return C.columnToRowMajor();

}

inline klMatrix<float> mmBLAS(float alpha, klMatrix<float> a, klMatrix<float> b, float beta, klMatrix<float> c)
{
	if(a.getColumns()!=b.getRows())
		throw "double klMatrix<float>::mmBLAS const ERROR: invalid dimension";
	if(a.getRows() !=c.getRows() )
		throw "double klMatrix<float>::mmBLAS const ERROR: invalid dimension";
	if(b.getColumns() !=c.getColumns() )
		throw "double klMatrix<float>::mmBLAS const ERROR: invalid dimension";

	klMatrix<float> C=c;

	const char transa = 'T';
	const char transb = 'T';
	int m =c.getRows();
	MKL_INT *mp=&m;
	int n =b.getColumns();
	MKL_INT *np=&n;
	int k =a.getColumns();
	MKL_INT *kp=&k;
	const float *alphap=&alpha;
	const float *ap=(float*)a.getMemory();
	const MKL_INT *lda=kp;
	const float *bp=(float*)b.getMemory();
	const MKL_INT *ldb=np;
	const float *betap=&beta;
	float *cp=(float*)C.getMemory();
	const MKL_INT *ldc=mp;

	sgemm(&transa,&transb, mp, np, kp, alphap, ap, lda, bp, ldb, betap, cp, ldc);

	//In Fortran, column-major ordering of storage is assumed. This means that elements of the
	//same column occupy successive storage locations.
	//Three quantities are usually associated with a two-dimensional array argument: its leading
	//dimension, which specifies the number of storage locations between elements in the same row,
	//its number of rows, and its number of columns. For a matrix in full storage, the leading dimension
	//of the array must be at least as large as the number of rows in the matrix.
	return C.columnToRowMajor();
}


//y := alpha*A*x + beta*y
inline klVector<double> mvpBLAS(double alpha, klMatrix<double> a, klVector<double> x, double beta,klVector<double> yi)
{
	klVector<double> y(yi);
	const char trans = 'N';
	MKL_INT m=a.getRows();
	MKL_INT n=a.getColumns();
	MKL_INT lda=n;
	MKL_INT incx=1;
	MKL_INT incy=1;

	dgemv(&trans, &m, &n, &alpha, a.getMemory(), &lda, x.getMemory(), &incx, &beta, yi.getMemory(), &incy);

	return yi;
}

inline klVector<float> mvpBLAS(float alpha,  klMatrix<float> a,klVector<float> x, float beta,klVector<float> yi)
{
	klVector<float> y(yi);
	const char trans = 'N';
	MKL_INT m=a.getRows();
	MKL_INT n=a.getColumns();
	MKL_INT lda=n;
	MKL_INT incx=1;
	MKL_INT incy=1;

	sgemv(&trans, &m, &n, &alpha, a.getMemory(), &lda, x.getMemory(), &incx, &beta, yi.getMemory(), &incy);

	return yi;
}

//Specialization for float matrix multiply that uses BLAS Level 3 sgemm
template< > klMatrix<float> klMatrix<float>::operator*(klMatrix<float> a) const
{
	klMatrix<float> I(_row,a.getColumns());
	I=0;
	return mmBLAS(1.0f, (klMatrix<float>)*this, a, 0.0f, I);
}


//Specialization for double matrix multiply that uses BLAS Level 3 sgemm
template< > klMatrix<double> klMatrix<double>::operator*(klMatrix<double> a) const
{
	klMatrix<double> I(_row,a.getColumns());
	I=0;
	return mmBLAS(1.0f, (klMatrix<double>)*this, a, 0.0f, I);
}

//Apply a univariate function to a klVector
//These are some commone ones available from math.h
//double  __cdecl acos(double);
//double  __cdecl asin(double);
//double  __cdecl atan(double);
//double  __cdecl cos(double);
//double  __cdecl cosh(double);
//double  __cdecl exp(double);
//double  __cdecl fabs(double);
//long    __cdecl labs(long);
//double  __cdecl log(double);
//double  __cdecl log10(double);
//double  __cdecl sin(double);
//double  __cdecl sinh(double);
//double  __cdecl tan(double);
//double  __cdecl tanh(double);
//double  __cdecl sqrt(double);
template<class TYPE, class TYPE1> klVector<TYPE> klApplyFn(TYPE1 (*f)(TYPE1), const klVector<TYPE> &c)
{
	klVector<TYPE> r(c.getColumns() );
	unsigned int i;
	for (i=0;i<c.getColumns();i++)
		r[i]=TYPE(f(TYPE1(c[i])));
	return r;
}

//Apply a univariate function to a klMatrix
template<class TYPE, class TYPE1> klMatrix<TYPE> klApplyFn(TYPE1 (*f)(TYPE1), const klMatrix<TYPE> &c)
{
	klMatrix<TYPE> r(c.getRows(),c.getColumns());
	unsigned int i;
	unsigned int j;
	for ( i=0;i<c.getRows();i++)
		for ( j=0;j<c.getColumns();j++)
			//out(i,j)=static_cast<T>(f(static_cast<fT>(data(i,j))));
			r[i][j]=TYPE(f(TYPE1(c[i][j])));

	return r;
}


template<class TYPE> inline const klMatrix<TYPE> operator+(const klMatrix<TYPE> &c1, const klMatrix<TYPE> &c2)
{
	if(c1.getRows() !=c2.getRows() || c1.getColumns() !=c2.getColumns())
		throw "const klMatrix<TYPE> operator+(const klMatrix<TYPE> &c1, const klMatrix<TYPE> &c2) ERROR: incompatible dimensions.";
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j]+c2[i][j];
		}

	}
	return r;
}



template<class TYPE> inline const klMatrix<TYPE> operator+(const klMatrix<TYPE> &c1, TYPE c2)
{
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j]+c2;
		}

	}
	return r;
}

template<class TYPE> inline const klMatrix<TYPE> operator+(TYPE c1, const klMatrix<TYPE> &c2)
{
	klMatrix<TYPE> r(c2.getRows(),c2.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c2.getRows();i++)
	{
		for(j=0;j<c2.getColumns();j++)
		{
			r[i][j]=c2[i][j]+c1;
		}

	}
	return r;
}

template<class TYPE> inline	const klMatrix<TYPE> operator-(const klMatrix<TYPE> &c1, const klMatrix<TYPE> &c2)
{
	if(c1.getRows() !=c2.getRows() || c1.getColumns() !=c2.getColumns())
		throw "const klMatrix<TYPE> operator+(const klMatrix<TYPE> &c1, const klMatrix<TYPE> &c2) ERROR: incompatible dimensions.";
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j] - c2[i][j];
		}

	}
	return r;

}


template<class TYPE> inline	const klMatrix<TYPE> operator-(const klMatrix<TYPE> &c2, TYPE c1)
{
	klMatrix<TYPE> r(c2.getRows(),c2.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c2.getRows();i++)
	{
		for(j=0;j<c2.getColumns();j++)
		{
			r[i][j]=c2[i][j]-c1;
		}

	}
	return r;

}

template<class TYPE> inline	const klMatrix<TYPE> operator-(TYPE c1, const klMatrix<TYPE> &c2)
{
	klMatrix<TYPE> r(c2.getRows(),c2.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c2.getRows();i++)
	{
		for(j=0;j<c2.getColumns();j++)
		{
			r[i][j]=c2[i][j]-c1;
		}

	}
	return r;

}
//bbcrevisit this is not implemented correctly.
template<class TYPE> inline	const klMatrix<TYPE> operator-(const klMatrix<TYPE> &c)
{
	if(this._row != c.getRows() ||this._col!=c.getColumns())
		throw "Bad dimensions in klMatrix<TYPE> operator-(const klMatrix<TYPE> &c)";
	klMatrix<TYPE> r(c.getRows(),c.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c.getRows();i++)
	{
		for(j=0;j<c.getColumns();j++)
		{
			r[i][j]=c[i][j];
		}

	}
	return r;
}
template<class TYPE> inline const klMatrix<TYPE> operator/(const klMatrix<TYPE> &c1, TYPE c2)
{
	if(c2==0)
		throw "klMatrix<TYPE> operator/(const klMatrix<TYPE> &c1, TYPE c2) : ERROR: attempting to divide by zero .";
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j] / c2;
		}

	}
	return r;

}
template<class TYPE> inline const klMatrix<TYPE> operator/(TYPE c2,const klMatrix<TYPE> &c1 )
{ 
	if(c2==0)
		throw "klMatrix<TYPE> operator/(const klMatrix<TYPE> &c1, TYPE c2) : ERROR: attempting to divide by zero .";
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j] / c2;
		}

	}
	return r;
}

template<class TYPE> inline const klMatrix<TYPE> operator/(klMatrix<TYPE> &c1,const klMatrix<TYPE> &c2 )
{ 

	if(c1.getRows()!=c2.getRows() || c1.getColumns() !=c2.getColumns())
		throw "klMatrix<TYPE> operator/(klMatrix<TYPE> &c1,const klMatrix<TYPE> &c2 ) ERROR: incompatible dimensions.";

	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());

	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			if(c2[i][j]==0)
				throw "klMatrix<TYPE>& operator/=(const klMatrix<TYPE> &c) ERROR: attempting to divide by zero";
			r[i][j]=c1[i][j] / c2[i][j];
		}

	}
	return r;
}


template<class TYPE> inline	const klMatrix<TYPE> operator*(const klMatrix<TYPE> &c1, TYPE c2 )
{
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j] * c2;
		}

	}
	return r;

}

template<class TYPE> inline const klMatrix<TYPE> operator*(TYPE c2, const klMatrix<TYPE> &c1)
{
	klMatrix<TYPE> r(c1.getRows(),c1.getColumns());
	unsigned int i;
	unsigned int j;
	for(i=0;i<c1.getRows();i++)
	{
		for(j=0;j<c1.getColumns();j++)
		{
			r[i][j]=c1[i][j] * c2;
		}

	}
	return r;
}


/*
template<class TYPE> inline	const klMatrix<TYPE> elem_mult(const klMatrix<TYPE> &m1, const klMatrix<TYPE> &m2)
{

}
template<class TYPE>	const klMatrix<TYPE> operator*(const klMatrix<TYPE> &c1, const klMatrix<TYPE> &c2)
{ 
}

template<class TYPE>	const klVector<TYPE> operator*(const klMatrix<TYPE> &m, const klVector<TYPE> &v)
{
}

template<class TYPE> inline const klMatrix<TYPE> operator*(const klVector<TYPE> &v, const klMatrix<TYPE> &m)
{
}


template<class TYPE> inline const klMatrix<TYPE> elem_div(const klMatrix<TYPE> &m1, const klMatrix<TYPE> &m2)
{
}

template<class TYPE> 	bool klMatrix<TYPE>::operator==(const klMatrix<TYPE> &m) const
{ 
}

template<class TYPE> 	bool klMatrix<TYPE>::operator!=(const klMatrix<TYPE> &m) const
{
}

*/

//Returns a diagonal matrix D_ij = c[i]
template<class TYPE> klMatrix<TYPE> diag(klVector<TYPE> c)
{
	klMatrix<TYPE> result(c.getColumns(),c.getColumns());
	result=0.0;
	unsigned int i=0;
	unsigned int j=0;
	for(i=0;i<c.getColumns();i++)
	{
		for(j=0;j<c.getColumns();j++)
		{
			if(i==j)
				result[i][j] = c[i];
		}
	}
	return result;
}
template<> double klMatrix<double>::norm(bool ell_infty)
{
	char norm = 'G' ;
	if(ell_infty==true)
		norm = 'I';
	else 
		norm = '1';
	int m=_row;
	int n=_col;
	int lda=m;
	double* work = new double[m*3];
	double val = dlange(&norm, &m, &n, this->transpose().getMemory(), &lda,work);
	delete work;
	return val;
}

template<> float klMatrix<float>::norm(bool ell_infty )
{
	char norm = 'G' ;
	if(ell_infty==true)
		norm = 'I';
	else 
		norm = '1';
	int m=_row;
	int n=_col;
	int lda = m;
	float* work = new float[m*3];
	float val = slange(&norm, &m, &n, this->transpose().getMemory(), &lda, work);
	delete work;
	return val;	
}

template<  > double klMatrix<double>::getConditionNumber(bool ellone)
{
	//Compute the Norm
	unsigned int i,j;
	
	double anorm =0.0f;
	if(ellone)
		anorm = norm();
	else
		anorm = norm(true);
	//Compute the LU factorization
	if(!_contiguous)
		throw "template<  > double klMatrix<double>::getConditionNumber(bool ellone) ERROR: this routine is only supported for matrices with contiguous memory layout ";

	//transpose to get into FORTRAN column major storage format
	klMatrix<double> tr=transpose();
	//allocate memory for returned pivot indices
	size_t size=_row>_col? _row : _col;
	int* ipiv=new int[2*size];
	int info=0;
	int n=_row;
	int m=_col;
	dgetrf(&n,&m,tr.getMemory(),&n,ipiv,&info);
	//tr is now LU factored

	char Norm;
	if(ellone==true)
		Norm='O';//doing L1 norm
	else
		Norm='I';//doing L\infty norm
	int* iwork=new int[2*_col+1];
	double* work=new double[4*_col+1]; 
	double rcond=0;

	dgecon(&Norm,&m,tr.getMemory(),&n,&anorm,&rcond,work,iwork,&info);

	if(info<0)
		throw "template<  > double klMatrix<double>::getConditionNumber(bool ellone) ERROR: parameter error in MKL call to dgecon.";

	delete ipiv;
	delete iwork;
	delete work;

	//dgecon returns 1/cond in case condition number is very big
	//check for rcond near EPS and then return the reciprocal
	if(rcond<2*DBL_EPSILON)
		return 0;
	else 
		return 1/rcond; 
}

template<  > float klMatrix<float>::getConditionNumber(bool ellone)
{
	//Compute the Norm
	unsigned int i,j;
	
	float anorm =0;
	if(ellone)
		anorm = norm();
	else
		anorm = norm(true);

	//Compute the LU factorization
	if(!_contiguous)
		throw "template<  > float klMatrix<float>::getConditionNumber(bool ellone) ERROR: this routine is only supported for matrices with contiguous memory layout ";

	//transpose to get into FORTRAN column major storage format
	klMatrix<float> tr=transpose();
	//allocate memory for returned pivot indices
	size_t size=_row>_col? _row : _col;
	int* ipiv=new int[2*size];
	int info=0;
	int n=_row;
	int m=_col;
	sgetrf(&n,&m,tr.getMemory(),&n,ipiv,&info);
	//tr is now LU factored
	char Norm;
	if(ellone==true)
		Norm='O';//doing L1 norm
	else
		Norm='I';
	int* iwork=new int[2*_col+1];
	float* work=new float[4*_col+1]; 
	float rcond=0;

	sgecon(&Norm,&m,tr.getMemory(),&n,&anorm,&rcond,work,iwork,&info);

	if(info<0)
		throw "template<  > float klMatrix<float>::getConditionNumber(bool ellone) ERROR: parameter error in MKL call to dgecon.";
	delete ipiv;
	delete iwork;
	delete work;

	//dgecon returns 1/cond in case condition number is very big
	//check for rcond near EPS and then return the reciprocal
	if(rcond<2*FLT_EPSILON)
		return FLT_MAX;
	else 
		return 1.0/rcond; 

}


//klMatrix stream io.  Operator <<  override for klMatrix class
template <class TYPE> static ostream& operator<<(ostream& str, const klMatrix<TYPE>& v) {
	int i = v.getRows();
	int k = v.getColumns();
	for (int l=0;l<i;l++)
	{
		for (int j=0; j<k; j++) 
		{
			if(j!=k-1)
				str << v[l][j]<<", ";
			else
				str<<v[l][j];
		}
		if(l!=i-1)
			str<<"\n";
	}
	return str<<"\n";
}

//klMatrix stream io.  Operator >> override for klMatrix class
template <class TYPE> static inline istream& operator>>(istream& c, klMatrix<TYPE> & v) {
	char ch;
	int j=0;
	do{
		int i=0;
		do {
			TYPE d;
			c >> d;
			v[j][i]=d;
			i++;
			if(i!=v[j].getRowSize())
				c >> ch;
		} while (i<(int)v[j].getRowSize());
		j++;
	}while(j<(int)v.getRows());
	return c;
}

//Smart Pointer Defs
typedef klSmartPtr<klMatrix<double> >  klDoubleMatrixPtr;
typedef klSmartPtr<klMatrix<complex<double> > >  klComplexDoubleMatrixPtr;
typedef klSmartPtr<klMatrix<complex<float> > >  klComplexFloatMatrixPtr;
typedef klSmartPtr<klMatrix<float> >  klFloatMatrixPtr;

typedef klSmartPtr<klVector<double> >  klDoubleVectorPtr;
typedef klSmartPtr<klVector<complex<double> > >  klComplexDoubleVectorPtr;
typedef klSmartPtr<klVector<complex<float> > >  klComplexFloatVectorPtr;
typedef klSmartPtr<klVector<float> >  klFloatVectorPtr;

#endif __kl_matrix__
