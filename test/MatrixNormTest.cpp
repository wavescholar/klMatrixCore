 #include "kl_matrix.h"
#include "kl_latex_helper_fns.h"
#include "kl_matrix_facorizations.h"
#include "kl_matrix_helper_fns.h"
#include "testmatgenunit.h"
bool testmatgen(bool silent);
klMatrix<double> real_2d_array_to_klMatrix(ap::real_2d_array a);
void MatrixNormTest(ofstream &_tex,__int64  &n)
{	
	n= 12;

	makeLatexSection("Haar Distributed Random Orthogonal Matrix $A \\in O(n)$",_tex);
	_tex<<" Testing Operator Norm"<<endl<<"Number of Dimensions: "<<n<<endl<<endl; 

	testmatgen(false);
	ap::real_2d_array a;
	rmatrixrndorthogonal(n, a);
	klMatrix<double> Op = real_2d_array_to_klMatrix(a);
	klLU<double> LU(Op);
	klMatrix<double> u =LU();
	klMatrix<double> l =LU.L();

	klMatrix<double> lu = l*u;

	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$Det(A) :   A \\in O(n)$ = "<<Op.det()<<endl<<endl;
	LatexPrintMatrix(l, "L",_tex);
	LatexPrintMatrix(u, "U",_tex);
	LatexPrintMatrix(lu, "L * U ",_tex);
	_tex<<"$Det(L) :    = "<<l.det()<<"    "<<" Det(U) :    = "<<u.det()<<"    "<<" Det(LU) :    = "<<lu.det()<<"$"<<endl<<endl;
	_tex<<"$||A||_{L_1}$  = "<<Op.norm()<<endl<<endl;

	_tex<<"$||A||_{L_{\\infty}}$ = "<<Op.norm(true)<<endl<<endl;

	_tex<<"$||A^{-1}||_{L_1}$  = "<<Op.inverse().norm()<<endl<<endl;

	_tex<<"$||A^{-1}||_{L_{\\infty}}$ = "<<Op.inverse().norm(true)<<endl<<endl;

	_tex<<"$||A||_{L_{\\infty}} * ||A^{-1}||_{L_{\\infty}} = "<<Op.norm(true) * Op.inverse().norm(true)<<"$"<<endl<<endl;

	_tex<<"$||A||_{L_1} * ||A^{-1}||_{L_1} = "<< Op.norm() * Op.inverse().norm()<<"$"<<endl<<endl;

	double A_f =0;
	for(int i=0;i<n;i++)
	{
		A_f+=Op[i].pow_alpha(2).sum();		
	}
	A_f = sqrt(A_f);
	_tex<<"Frobenious Norm  $||A||_{\\textit{F}}$ via $\\sum\\limits_{i,j =0}^{n} \\|A_{i,j}|$   of  $A \\in O(n)$  "<<A_f<<endl<<endl;

	_tex<<"$L_1$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.ConditionNumber(1)<<endl<<endl;
	LatexPrintMatrix(Op, "A",_tex);
	_tex<<"$L_{\\infty}$ condition number of Haar Distributed Random Orthogonal Matrix $A \\in O(n)$ "<<Op.ConditionNumber()<<endl<<endl;

	klVector<complex<double> > eigen =Op.eigenvalues();
	_tex<<"Eigenvalues of $A \\in O(n)$"<<endl<<endl;
	_tex<<eigen<<endl;

	klVector<double> argEigenvalues(n);
	for(int i=0;i<n;i++)
	{
		argEigenvalues[i] = std::abs(eigen[i]);
	}
	_tex<<" $|\\lambda | : \\lambda \\in \\sigma(A) , A \\in O(n)$"<<endl<<endl;
	_tex<<argEigenvalues<<endl<<endl;


	_tex<<"Calculating $A^{\\dag} A,$  we expect $A^{\\dag} A \\approx I$"<<endl<<endl;
	klMatrix<double> ortho(n,n);
	int i=0;
	int j=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			ortho[i][j] = Op.getColumn(i).dotBLAS(Op.getColumn(j));
		}
	}
	LatexPrintMatrix(ortho,"A^{\\dag} A",_tex);

	//test \alpha A x + \beta y
	//klMatrix<float> mvpBLAS(float alpha,  klMatrix<float> a,klVector<float> x, float beta,klVector<float> yi);
	klVector<double> Axbpy = mvpBLAS(1.0,  ortho,ortho[0], 2.0,ortho[1]);
	//_tex<<ortho[0]<<endl;
	//_tex<<ortho[1]<<endl;
	//_tex<<Axbpy<<endl;

	ortho = Op;

	_tex<<"Calculating $A^{-1} ,  A \\in O(n)$."<<endl<<endl;
	klMatrix<double> invOrtho= Op.inverse();
	LatexPrintMatrix(invOrtho,"A^{-1}",_tex);

	_tex<<"Calculating $A^{-1} *A  ,  A \\in O(n)$.   We expect $A^{-1} *A  \\approx I$. "<<endl<<endl;
	LatexPrintMatrix(invOrtho*Op,"A^{-1} *A",_tex);

	_tex<<"Calculating SVD of  $A \\in O(n)$"<<endl<<endl;

	{
		klSVD<double> SVD(Op);
		klDoubleVectorPtr Sigma = SVD();
		klMatrix<double> S = diag(*Sigma);
		klDoubleMatrixPtr U = SVD.U();
		klDoubleMatrixPtr V = SVD.V();

		LatexPrintMatrix(*U,"U",_tex);
		LatexPrintMatrix(S,"S",_tex);
		LatexPrintMatrix(*V,"V",_tex);

		LatexPrintMatrix( *(U) * (S) * *(V),"U S V",_tex);
	}

	makeLatexSection("Wishart Matrix $A \\in W(n)$",_tex);

	klMatrix<double> AW=     SampleWishart(n);
	_tex<<"$L_1$ condition number of Wishart Matrix "<<AW.ConditionNumber(true)<<endl;
	_tex<<"$L_\infty$ condition number of Wishart Matrix "<<AW.ConditionNumber()<<endl;

	makeLatexSection("Gaussian Orthogonal Ensemble $A \\in GOE(n)$",_tex);

	klMatrix<double> A_GOE = SampleGOE(n);
	klMatrix<double> Ainv=A_GOE.inverse();
	klMatrix<double> Id_goe=Ainv * A_GOE;
	Id_goe.threshold(0.001f,+0.0f);
	Id_goe = Id_goe + 0.0;
	klVector<double> x=Id_goe[0]; 
	klVector<double> y=Id_goe[2];

	_tex<<"$L_1$ condition number of GOE Matrix "<<A_GOE.ConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of GOE Matrix "<<A_GOE.ConditionNumber(true)<<endl;

	makeLatexSection("The Identity Matrix $I \\in M(n)$",_tex);

	klMatrix<double> Id(n,n);
	Id= IdentityMatrix<double>(n);
	_tex<<"$L_1$ condition number of $I$ = "<<Id.ConditionNumber(true)<<endl;
	_tex<<"$L_\\infty$ condition number of $I$ = "<<Id.ConditionNumber()<<endl;

	_tex.flush();
	
}