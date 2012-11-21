#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_time_series.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_ML_helper_fns.h"
#include "kl_bregman_divergence.h"
#include "kl_util.h"
#include "kl_unit_tests.h"
#include "kl_matrix_facorizations.h"
#include "expokit.h"

#include <complex> 
using namespace std;

void matvecfunc_double_Impl(void *data, double *in, double *out)
{

}
void matvecfunc_cplx_Impl(void *data, cplx *in, cplx *out)
{
}

class klExpoKitDGEXPVDriver
{
	/* ---  DGEXPV computes w = exp(t*A)*v - for a General matrix A. */
	/*     It does not compute the matrix exponential in isolation but */
	/*     instead, it computes directly the action of the exponential */
	/*     operator on the operand vector. This way of doing so allows */
	/*     for addressing large sparse problems. */
	/*     The method used is based on Krylov subspace projection */
	/*     techniques and the matrix under consideration interacts only */
	/*     via the external routine `matvec' performing the matrix-vector */
	/*     product (matrix-free method). */

	/*     n      : (input) order of the principal matrix A. */
	int n;
	/*     m      : (input) maximum size for the Krylov basis. */
	int m;
	/*     t      : (input) time at wich the solution is needed (can be < 0). */
	double t;
	/*     v(n)   : (input) given operand vector. */

	/*     w(n)   : (output) computed approximation of exp(t*A)*v. */

	/*     tol    : (input/output) the requested accuracy tolerance on w. */
	/*              If on input tol=0.0d0 or tol is too small (tol.le.eps) */
	/*              the internal value sqrt(eps) is used, and tol is set to */
	/*              sqrt(eps) on output (`eps' denotes the machine epsilon). */
	/*              (`Happy breakdown' is assumed if h(j+1,j) .le. anorm*tol) */

	/*     anorm  : (input) an approximation of some norm of A. */

	/*   wsp(lwsp): (workspace) lwsp .ge. n*(m+1)+n+(m+2)^2+4*(m+2)^2+ideg+1 */
	/*                                   +---------+-------+---------------+ */
	/*              (actually, ideg=6)        V        H      wsp for PADE */

	/* iwsp(liwsp): (workspace) liwsp .ge. m+2 */

	/*     matvec : external subroutine for matrix-vector multiplication. */
	/*              synopsis: matvec( x, y ) */
	/*                        double precision x(*), y(*) */
	/*              computes: y(1:n) <- A*x(1:n) */
	/*                        where A is the principal matrix. */

	/*     itrace : (input) running mode. 0=silent, 1=print step-by-step info */

	/*     iflag  : (output) exit flag. */
	/*              <0 - bad input arguments */
	/*               0 - no problem */
	/*               1 - maximum number of steps reached without convergence */
	/*               2 - requested tolerance was too high */
public: void Run(klMatrix<double> A,klVector<double> V)
		{
			int n; int m; double t; 
			double *v; double *w; 
			double tol; double anorm; 
			double *wsp; 
			int lwsp; int iwsp; int liwsp; 
			expokit::matvecfunc_double matvecfunc_double_Impl;
			void* matvecdata;
			int itrace;
			int iflag;
			/*int dgexpv(int *n, int *m, double *t, 
			double *v, double *w, double *tol, double *anorm, 
			double *wsp, int *lwsp, int *iwsp, int *liwsp, matvecfunc_double 
			matvec, void* matvecdata, int *itrace, int *iflag);*/
			int ans = expokit::dgexpv(&n, &m, &t, 
				v, w, &tol, &anorm, 
				wsp, &lwsp, &iwsp, &liwsp, matvecfunc_double_Impl , matvecdata, &itrace, &iflag);
		}
};

class klExpoKitDGPADMDriver
{
	//     ideg       the degre of the diagonal Pade to be used,  a value of 6 is generally satisfactory.
public : unsigned int _deg;
		 //    m          order of H. */
		 unsigned int _m;
		 //H(ldh,m), argument matrix.
		 klMatrix<double> _H;
		 // time-scale (can be < 0). 
		 double t;
		 /*     wsp(lwsp) : (workspace/output) lwsp .ge. 4*m*m+ideg+1. */
public : klMatrix<double> expH;


public: 
	void  Run(klMatrix<double> H)
	{
		_H = H;
		/*     ipiv(m)   : (workspace) */

		/*  iexph     : (output) number such that wsp(iexph) points to exp(tH) */
		/*                 i.e., exp(tH) is located at wsp(iexph ... iexph+m*m-1) */
		/*                 NOTE: if the routine was called with wsp(iptr), */
		/*                       then exp(tH) will start at wsp(iptr+iexph-1). */

		/*     ns        : (output) number of scaling-squaring used. */

		/*     iflag     : (output) exit flag. */
		/*                      0 - no problem */
		/*                     <0 - problem */

		//we generated this, but if f2c did it would look the same.
		//int dgpadm(int *ideg, int *m, double *t, 
		//	double *h__, int *ldh, int *wsp, int *lwsp, 
		//	int *ipiv, int *iexph, int *ns, int *iflag);

		int ideg=6; 
		int m=_H.getRows(); 
		double t = 1; 
		double *h = _H.getMemory(); 
		int ldh = m; 
		double* wsp = new double [4*m*m+ideg+1]; 
		int lwsp= 4*m*m+ideg+1;//4*m*m+ideg+1 

		//Curious - intel let's us call mkl methods with stack object variables and does not complain
		//However the f2c we're using results in a stack corruption unless we allocate on the free store.
		int* ipiv= new int[m];  
		int* iexph = new int[1]; 
		int* ns = new int[1]; 
		int* iflag=new int[1];

		int ans = expokit::dgpadm_(&ideg,&m, &t,h, &ldh, wsp, &lwsp, ipiv, iexph, ns, iflag);

		klMatrix<double> Exp(m,m);
		double* start = (wsp+*(iexph)-1);
		for(int i=0;i<m;i++)
		{
			for(int j=0;j<m;j++)
				Exp[i][j] =*(start + m*i + j);
		}

		expH = Exp;
		delete wsp;
	}
};

klVector<complex<double> > klApplyLog( const klVector<complex<double> > &c)
{
	klVector<complex<double> > r(c.getColumns() );
	unsigned int i;
	for (i=0;i<c.getColumns();i++)
		r[i]=complex<double> (std::log(complex<double> (c[i])));
	return r;
}
void testExpoKit(ofstream &_tex,unsigned int  &n)
{
	//long long MB = 1048576 ;
	//long long GB =  1073741824 ;
	//long long GBo = 1073741824*2;
	//long sz = sizeof(long);
	//char* g3b = new char[GB*3];
	//char* gb = new char[GB];
	//char* g2b = new char [GBo];
	//delete gb;
	//delete g2b;

	
	try{

		do
		{
		klMatrix<double> SPD =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(n);
		//cout<<"SPD"<<endl<<SPD<<endl;
		//klVector<complex<double> > eigen =SPD.eigenvalues();
		//cout<<"eigs  = "<<endl<<eigen<<endl;

		klExpoKitDGPADMDriver drv;
		drv._deg=6;
		drv.Run(SPD);

		//cout<<"exp(SPD) = "<<endl<<drv.expH<<endl;
		//klVector<std::complex<double> > logeigs=klApplyLog( drv.expH.eigenvalues() );//klApplyFn<complex<double> ,complex<double> >(std::log, drv.expH.eigenvalues() );
		//cout<<"eigs  = "<<endl<<drv.expH.eigenvalues()<<endl;
		//cout<<"log(eigs)  = "<<endl<<logeigs<<endl;

		//klMatrix<double> Id(n,n);
		//Id= IdentityMatrix<double>(n);
		//drv.Run(Id);
		//cout<<"exp(Id) = "<<endl<<drv.expH<<endl;

		//logeigs= klApplyLog( drv.expH.eigenvalues() );//klApplyFn<complex<double> ,complex<double> >(std::log, drv.expH.eigenvalues() );
		//cout<<"eigs  = "<<endl<<drv.expH.eigenvalues()<<endl;
		//cout<<"log(eigs)  = "<<endl<<logeigs<<endl;	

		n+=128;
		cout<<n<<endl;
		}while(n<4097);
	}
	catch(...)
	{
		int a = n;
	}

	
}

