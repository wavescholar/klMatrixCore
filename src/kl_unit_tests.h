 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/
#ifndef __kl_unit_tests__
#define __kl_unit_tests__

#include "kl_matrix.h"

template<class TYPE> void MatrixOpsQuickCheck(ofstream &_tex,unsigned int& n)
{
	/*ios_base::openmode wMode = ios_base::app;
	ofstream _tex(fileName, wMode);	*/

	float* memory=new float[9];
	int il=0;
	for(il=0;il<9;il++)
		*(memory+il)=0;
	klMatrix<float> b(memory,3,3);

	b.makeNanFriendly();//should fail for float

	//Testing if we have a nan
	float x=b[2][2];
	bool check = _isnan(x);


	b[0][0]=11;b[0][1]=12;b[0][2]=13;
	b[1][0]=21;b[1][1]=22;b[1][2]=23;
	b[2][0]=31;b[2][1]=32;b[2][2]=33;


	klVector<float> col2=b.getColumn(2);

	klVector<float> diffr=col2.diff(1);
	diffr=col2.diff(2);
	try{    diffr=col2.diff(3);}
	catch(...)
	{ cout<<"caught an expected error";} 



	cout<<klApplyFn<float,double>(std::sin,b)<<endl;
	cout<<klApplyFn<float,double>(klErrorFunction,b/1000.0f);

	cout<<klApplyFn<float,double>(std::sin,col2)<<endl;
	cout<<klApplyFn<float,double>(klErrorFunction,col2/100.0f);


	///////////Test apply function
	/*        int     __cdecl abs(int);
	double  __cdecl acos(double);
	double  __cdecl asin(double);
	double  __cdecl atan(double);
	double  __cdecl atan2(double, double);
	double  __cdecl cos(double);
	double  __cdecl cosh(double);
	double  __cdecl exp(double);
	double  __cdecl fabs(double);
	double  __cdecl fmod(double, double);
	long    __cdecl labs(long);
	double  __cdecl log(double);
	double  __cdecl log10(double);
	double  __cdecl pow(double, double);
	double  __cdecl sin(double);
	double  __cdecl sinh(double);
	double  __cdecl tan(double);
	double  __cdecl tanh(double);
	double  __cdecl sqrt(double);
	*/


	klMatrix<double>moo(2,2);

	moo.makeNanFriendly();

	moo[0][0]=1; moo[0][1]=0;
	moo[1][0]=0;moo[1][1]=1;


	klMatrix<float> goo(2,2);
	goo[0][0]=1; goo[0][1]=0;
	goo[1][0]=0;goo[1][1]=1;

	klMatrix<float> gsb(2,2);
	gsb=b.getSubBlock(1,1,2,2);
	cout<<gsb<<endl;
	b.setSubBlock(goo,1,1);
	cout<<b<<endl;
	b.setSubBlock(gsb,1,1);
	cout<<b<<endl;



	klVector<double> mooo(2);mooo[0]=1;mooo[1]=1;
	cout<<moo*mooo;
	cout<<moo.getConditionNumber();
	cout<<moo.getConditionNumber(false);//L\infty cond number
	klVector<complex<double> > eigen=moo.eigenvalues();
	cout<<eigen;
	eigen=b.eigenvalues();cout<<eigen;
	klMatrix<float> bbb(b);
	bbb[2][2]=3.14f;
	cout<<bbb;
	klMatrix<float> ccc=b * bbb;
	cout<<ccc;
	//  b[2][2]=3.1415;
	klVector<float> mfa=b[2];
	cout<<b*mfa;
	cout<<mfa;
	string _filename="moo.txt";
	ofstream _fileostream(_filename.c_str() );
	_fileostream<<b<<endl;
	_fileostream.close();
	cout<<b;
	cout<<b.getColumn(1);
	klVector<float> sd(3);
	klMatrix<float> sb(3,3);
	flushall();
	fstream _fileistream;
	_fileistream.open("moo.txt");
	_fileistream>>sb;
	cout<<sb;
	klMatrix<float>  as(memory,3,3);

	as[2][2]=22;
	float answer=as[2][2];
	klVector<float> boo(3);
	boo[2]=2.141f;
	klVector<float> copy;
	copy=boo;//copy ctor not invoked
	float and=copy[2];
	float dot=copy.dot(boo);

	klVector<double> av(4096);
	av[2]=1;
	int ai=av[2];
	int* pi=new int[256];
	for(ai=0;ai<256;ai++)*(pi+ai)=ai;
	klVector<int> am(pi,256);
	int wam=am[2];
	int* pi2=new int[256];
	for(ai=0;ai<256;ai++)*(pi2+ai)=ai;
	klVector<int> am2(pi2,256);

	//Test some operators on klVecor
	bool sans=( am2==  am);
	cout<<sans;
	sans=( am2==  am2);
	klVector<bool> ans= ( am2>128 );
	cout<<ans;
	ans=( am2>=128 );
	cout<<ans;
	ans = (am2>128 );
	cout<<ans;
	ans = ( am2<=128 ) ;
	cout<<ans;
	ans = ( am2<128 ) ;
	cout<<ans;
	am2 = 3.14;//set all the vals to scalar
	cout<<am2;
	sans=( am2==  am);
	cout<<sans;

	{
		//These are not members
		klVector<int> ans=am2  +    3;
		cout<<ans;
		ans= am2  +   3;
		cout<<ans;
		ans= ans+ am2;
		cout<<ans;
		ans= am2-3;
		cout<<ans;
		ans= 3-am2;
		cout<<ans;
		ans= ans - am2;
		cout<<ans;
		ans = -ans;
		cout<<ans;
		ans= ans*ans;
		cout<<ans;
		ans=ans*3;
		cout<<ans;
		ans=3*ans;
		cout<<ans;
		ans=ans/3;
		cout<<ans;
		ans=3/ans;
		cout<<ans;
		ans= ans/ans;
		cout<<ans;

	}

	klMatrix<float> klpb=b;
	cout<<b+3.1f<<endl;
	cout<<b<<endl;
	cout<<3.1f+b<<endl;
	cout<<"b= "<<b<<endl;
	cout<<b-b<<endl;
	cout<<"b= "<<b<<endl;
	cout<<b+b<<endl;
	cout<<"b= "<<b<<endl;
	cout<<b-1.0f<<endl;
	cout<<"b= "<<b<<endl;
	cout<<b*b<<endl;//this calls member operator - we did not define an elementwise operator* 
	cout<<"b= "<<b<<endl;
	b /= b;//calls member operator
	cout<<"b= "<<b<<endl;
	cout<<b/2.0f<<endl;
	cout<<"b= "<<b<<endl;
	cout<<b/(b/4.0f)<<endl;
	cout<<"b= "<<b<<endl;

	klMatrix<float>c = (b-b)+ 1.0f;
	c*=0;
	c[0][0]=1;c[1][1]=1;c[2][2]=1;
	cout<<c.det()<<endl;

	delete memory;
	delete pi;
	delete pi2;




}

#include "kl_random_number_generator.h"
template<class type> void testKLDescriptiveStatistics(ofstream &FS1,unsigned int &n)
{
	char* arg=new char[1024];


#define __D1 65536

	type* v1=new type[__D1];
	int c;
	klNormalInverseApproxRV<type>		_normal(0,1);
	for(c=0;c< __D1;c++)
	{
		v1[c]=_normal();
	}

	type* m1=KL_stat::mean<type>(v1,1,__D1);
	FS1<<"Mean N(0,1): "<<*m1<<endl;
	delete m1;

	type* m2=KL_stat::variance<type>(v1, (unsigned int )1,(unsigned int)__D1);
	FS1<<"Variance N(0,1): "<<*m2<<endl;
	delete m2;

	type m1_recur=KL_stat::mean<type>(v1,__D1);
	FS1<<"Mean N(0,1) [recurrence relation method] :"<<m1_recur<<endl;

	type m2_recur=KL_stat::variance<type>(v1,__D1,m1_recur);
	FS1<<"Variance [recurrence relation method] :"<<m2_recur<<endl;

	type m3 = KL_stat::skewness<type>(v1,__D1,m1_recur,m2_recur ); 
	FS1<<"Skewness : "<<m3<<endl;

	type m4 = KL_stat::kurtosis<type>(v1,__D1,m1_recur,m2_recur ); 
	FS1<<"Kurtosis : "<<m4<<endl;

	delete v1;

	delete arg;

	flushall();



}

#include "kl_time_series.h"
#include "kl_random_number_generator.h"
template<class TYPE> void testKLTimeSeries2(ofstream &_tex,unsigned int &n)
{
	size_t popsize=1024*2;
	klVector<TYPE> a(popsize);
	klNormalInverseApproxRV<TYPE> normalinv(0,0.1);
	unsigned i;
	for(i=0;i<popsize;i++)
	{
		TYPE pi= 3.141592653589793238462643383279502;
		a[i]=normalinv()+ .5* sin(4*pi*(double(i)/popsize)) + 1* sin(7*pi*(double(i)/popsize) );
	}
	klTimeSeries<TYPE> c(a);

	_tex<<c.mean()<<endl;
	_tex<<c.variance()<<endl;
	_tex<<c.skewness()<<endl;
	_tex<<c.kurtosis()<<endl;

	klTimeSeries<TYPE>::klTimeSeriesInterpolation interp=klTimeSeries<TYPE>::klTimeSeriesInterpolation::PREVIOUS;

	TYPE tau=12.0;

	klTimeSeries<TYPE> ema=c.EMA(popsize,tau,interp);

	klTimeSeries<TYPE> iema=c.IEMA(popsize,6,tau,interp);

	klTimeSeries<TYPE> ma=c.MA(popsize,6,tau,interp);

	TYPE gamma=1.22208;
	TYPE beta=0.65;
	TYPE alpha=1/(gamma*(8* beta - 3));

	klTimeSeries<TYPE> diff=c.DIFF(popsize,gamma,beta,alpha,64,interp);

	ofstream _fileostream("klIEMA.txt");
	for(i=0;i<popsize;i++)
	{
		_fileostream<<c[i]<<", "<<ma[i]<<", "<<iema[i]<<" ,"<<ema[i]<<", "<<diff[i]<<endl;
	}
	_fileostream.close();

	//pop new data in c
	c[popsize-1024]=1237;//big shock

	//bbcsvc broken 011805 revisit - used to work  - nfg
	//TYPE advanceEma=c.advanceEMA(popsize-1024,128,ema[popsize-1025],interp);
	//since PREVIOUS interp is used we don't see shock
	//advance again and we should start to see the effect
	//advanceEma=c.advanceEMA(popsize-1023,128,advanceEma,interp);

	//  pair<TYPE,TYPE> advanceIEma=c.advanceIEMA(popsize-1024,4,128,iema[popsize-1025],interp);
	//  TYPE advanceMA=c.advanceMA(popsize-1024,4,128,interp);//bbcrevisit this call has to be consistent in second parameter as previous MA call
	//  advanceMA=c.advanceMA(popsize-1023,4,128,interp);

	_tex.flush();
	//_tex.close();
}

#include "kl_regression.h"
#include "kl_random_number_generator.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
template<class TYPE> void testSamplePopulation2(ofstream &_tex)//const char* fileName)
{
	//ios_base::openmode wMode = ios_base::app;
	//ofstream _tex(fileName, wMode);	

	klSamplePopulation<double> b(3,3);

	b[0][0]=11;b[0][1]=12;b[0][2]=13;
	b[1][0]=21;b[1][1]=22;b[1][2]=23;
	b[2][0]=31;b[2][1]=32;b[2][2]=33;
	b.calcDescriptiveStats();

	unsigned int i;
	for(i=0;i<3;i++)
	{
		_tex<<b.mean(i)<<", "<<b.variance(i)<<", "<<b.skewness(i)<<", "<<b.kurtosis(i)<<endl;
	}
	_tex<<b.covarianceMatrix();
	_tex<<b.correlationMatrix(b.covarianceMatrix() );

	/*  Recipe for SPD
	The diagonal entries of the matrix have the form (0.5 + rand)*dim; 
	the lower-diagonal entries take the form rand — 0.5; 
	the upper-diagonal entries equal their lower-diagonal counterparts.
	Here, dim is the number of equations and rand a random-number generator with values in the interval [0, 1].
	This construction yields a symmetric and positive definite matrix.
	*/
	
	_tex.flush();
	//_tex.close();

}

#include "kl_multiclass_svm.h"
#include "kl_principal_components.h"
template<class TYPE> void klMulticlassSVMHarness2(ofstream &_tex)//const char* fileName)
{
	char* arg=new char[256];
	char* ofilename=new char[256];

	//ios_base::openmode wMode = ios_base::app;
	//ofstream _tex(fileName, wMode);	

	unsigned int i;
	unsigned int j;

	unsigned int numTrainingPoints=256;
	unsigned int numFeatureDimensions=3;
	unsigned int numClasses=3;

	klVector<TYPE> meanVector1(numFeatureDimensions);
	meanVector1[0]=9;
	meanVector1[1]=0.1;
	meanVector1[2]=0.1;

	klVector<TYPE> meanVector2(numFeatureDimensions);
	meanVector2[0]=0.1;
	meanVector2[1]=9;
	meanVector2[2]=0.1;

	klVector<TYPE> meanVector3(numFeatureDimensions);
	meanVector3[0]=0;
	meanVector3[1]=0;
	meanVector3[2]=9;

	_tex<<"\\textbf{ KLMulticlassSVM unit test ouput.  This LaTex file and the associated diagrams are produced by the KL software libraries.}"<<endl;

	_tex<<"\\begin{itemize}"<<endl; 
	_tex<<"\\item numTrainingPoints = "<<numTrainingPoints<<endl;
	_tex<<"\\item numFeatureDimensions = "<<numFeatureDimensions<<endl;
	_tex<<"\\item numClasses = "<<numClasses<<endl;
	_tex<<"\\end{itemize}"<<endl;

	_tex<<"{The mean vectors of the 3 classes}"<<endl;
	_tex<<"\\begin{tabular}{c | c c c}"<<endl;//{|"<<numClasses<<"|"<<numFeatureDimensions<<"|}"<<endl
	_tex<<"class 1 &"<<meanVector1[0]<<" & "<<meanVector1[1]<<"& "<<meanVector1[2]<<"\\\\"<<endl;
	_tex<<"\\hline"<<endl;
	_tex<<"class 2 &"<<meanVector2[0]<<" & "<<meanVector2[1]<<"& "<<meanVector2[2]<<"\\\\"<<endl;
	_tex<<"\\hline"<<endl;
	_tex<<"class 3 &"<<meanVector3[0]<<" & "<<meanVector3[1]<<"& "<<meanVector3[2]<<"\\\\"<<endl;
	_tex<<"\\end{tabular}\\newline"<<endl;


	//The Kullback-Leibler divergence: D($N(\mu_{1}, \Sigma_{1}),$N(\mu_{2}, \Sigma_{2}) = \int {N_{1} ln \frac{N_{1}}{N_{2}} dx


	klMatrix<TYPE> covarianceMatrix1 =klGenerateRandomSymmetricPositiveDefiniteMatrix<TYPE>(numFeatureDimensions,fastSeed() );
	klMatrix<TYPE> covarianceMatrix2 =klGenerateRandomSymmetricPositiveDefiniteMatrix<TYPE>(numFeatureDimensions,fastSeed() );
	klMatrix<TYPE> covarianceMatrix3 =klGenerateRandomSymmetricPositiveDefiniteMatrix<TYPE>(numFeatureDimensions,fastSeed() );

	_tex<<"A random SPD covairance matrix is generated for each of the classes.\\newline"<<endl;
	_tex<<"$Cov_1 = \\biggl[\\begin{tabular}{c c c}"<<endl;
	_tex<<covarianceMatrix1[0][0]<<" & "<<covarianceMatrix1[0][1]<<"& "<<covarianceMatrix1[0][2]<<"\\\\"<<endl;
	_tex<<covarianceMatrix1[1][0]<<" & "<<covarianceMatrix1[1][1]<<"& "<<covarianceMatrix1[1][2]<<"\\\\"<<endl;
	_tex<<covarianceMatrix1[2][0]<<" & "<<covarianceMatrix1[2][1]<<"& "<<covarianceMatrix1[2][2]<<"\\\\"<<endl;
	_tex<<"\\end{tabular}\\biggr]$"<<endl;

	_tex<<"$Cov_2 =\\biggl[\\begin{tabular}{c c c}"<<endl;
	_tex<<covarianceMatrix2[0][0]<<" & "<<covarianceMatrix2[0][1]<<"& "<<covarianceMatrix2[0][2]<<"\\\\"<<endl;
	_tex<<covarianceMatrix2[1][0]<<" & "<<covarianceMatrix2[1][1]<<"& "<<covarianceMatrix2[1][2]<<"\\\\"<<endl;
	_tex<<covarianceMatrix2[2][0]<<" & "<<covarianceMatrix2[2][1]<<"& "<<covarianceMatrix2[2][2]<<"\\\\"<<endl;
	_tex<<"\\end{tabular}\\biggr]$"<<endl;


	_tex<<"$Cov_3 =\\biggl[\\begin{tabular}{c c c}"<<endl;
	_tex<<covarianceMatrix3[0][0]<<" & "<<covarianceMatrix3[0][1]<<"& "<<covarianceMatrix3[0][2]<<"\\\\"<<endl;
	_tex<<covarianceMatrix3[1][0]<<" & "<<covarianceMatrix3[1][1]<<"& "<<covarianceMatrix3[1][2]<<"\\\\"<<endl;
	_tex<<covarianceMatrix3[2][0]<<" & "<<covarianceMatrix3[2][1]<<"& "<<covarianceMatrix3[2][2]<<"\\\\"<<endl;
	_tex<<"\\end{tabular}\\biggr]$\\newline"<<endl;

	_tex<<"Verify L1 condition number of covariance. The diagonal entries of the matrix have the form $(0.5 + U(0,1) )*dim(Dom(Cov))$"<<endl; 
	_tex<<"The lower-diagonal entries take the form $U(0,1) — 0.5$. "<<endl;
	_tex<<"The $L_1$ condition numbers are :"<<endl;
	_tex<<"\\begin{itemize}"<<endl; 
	_tex<<"\\item "<<covarianceMatrix1.getConditionNumber(true)<<endl;
	_tex<<"\\item "<<covarianceMatrix2.getConditionNumber(true)<<endl;
	_tex<<"\\item "<<covarianceMatrix3.getConditionNumber(true)<<endl;
	_tex<<"\\end{itemize}"<<endl;

	//Here we scale all of the elements of the covariance 
	covarianceMatrix1/=covarianceMatrix1.getColumns()*2;
	covarianceMatrix2/=covarianceMatrix2.getColumns()*2;
	covarianceMatrix3/=covarianceMatrix3.getColumns()*2;

	_tex<<"We scale the covariance matrices by 2*dimension and  re-verify $L_1$ condition number."<<endl;
	_tex<<"\\begin{itemize}"<<endl; 
	_tex<<"\\item "<<covarianceMatrix1.getConditionNumber(true)<<endl;
	_tex<<"\\item "<<covarianceMatrix2.getConditionNumber(true)<<endl;
	_tex<<"\\item "<<covarianceMatrix3.getConditionNumber(true)<<endl;
	_tex<<"\\end{itemize}"<<endl;

	//Give the distributions a nice big variance so our classifier has 
	//some work to do, and we'll make the variances unequal just for fun.  
	//If the variances are equal, we can do F -statistic on regression coefficients.  bbcrevisit 

	klVector<unsigned int > seed1(numFeatureDimensions);
	seed1[0]=12;seed1[1]=2344;seed1[2]=21341;

	klNormalMultiVariate<TYPE> X1(meanVector1,covarianceMatrix1,false,seed1 );

	klVector<unsigned int > seed2(numFeatureDimensions); 
	seed2[0]=345;seed2[1]=4753;seed2[2]=857536;

	klNormalMultiVariate<TYPE> X2(meanVector2, covarianceMatrix2,false,seed2);

	klVector<unsigned int > seed3(numFeatureDimensions);
	seed3[0]=45423;seed3[1]=2358;seed3[2]=65321;

	klNormalMultiVariate<TYPE> X3(meanVector3, covarianceMatrix3,false,seed3);

	klSamplePopulation<TYPE> trainingPoints(numTrainingPoints,numFeatureDimensions);
	klVector<int> trainingClasses(numTrainingPoints);

	/////////////////////////////TEST CODE
	//klSamplePopulation<TYPE> test(numTrainingPoints/3,numFeatureDimensions);
	//for(j=0;j<numTrainingPoints/3;j++)
	//{
	//    klVector<TYPE> rv=X1();
	//    test[j][0]=rv[0]; test[j][1]=rv[1];test[j][2]=rv[2];
	//}
	//sprintf(ofilename,"//sanitycheckMVRV.eps");
	//sprintf(arg,"%s%s%s",kl_base_dir,basefilename,ofilename);
	//engEvalString(matlabEngine, "hold on);");
	//klScatterPlot3D(TS,filename,NULL,NULL,NULL,NULL,true,false,"or");
	//klScatterPlot3D(test,arg,NULL,NULL,NULL,NULL,true,true,"r");

	//for(j=0;j<numTrainingPoints/3;j++)
	//{
	//    klVector<TYPE> rv=X2();
	//    test[j][0]=rv[0]; test[j][1]=rv[1];test[j][2]=rv[2];
	//}
	//klScatterPlot3D(test,arg,NULL,NULL,NULL,NULL,true,true,"g");

	//for(j=0;j<numTrainingPoints/3;j++)
	//{
	//    klVector<TYPE> rv=X3();
	//    test[j][0]=rv[0]; test[j][1]=rv[1];test[j][2]=rv[2];
	//}


	//klScatterPlot3D(test,arg,NULL,NULL,NULL,NULL,true,true,"b");
	//sprintf(evalString,"print -r1200 -depsc %s;",arg);//Eps needs "special care" ( b nice to Cleve ) for color output
	//engEvalString(matlabEngine, evalString);

	//engEvalString(matlabEngine, "hold off;close(gcf);");

	////Convert to pdf for better imaging
	//sprintf(evalString,"epstopdf   %s",arg);
	//system(evalString);
	//sprintf(ofilename,"//sanitycheckMVRV.pdf");
	//sprintf(arg,"%s%s%s",kl_base_dir,basefilename,ofilename);
	// _tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl;


	//////////////////////////////////////////////END TEST CODE

	//Create a uniform generator to decide which RV to pull a trainingPoints from.
	//This randomized the data set.  
	klUniformRV<double> U(0,1);

	for(j=0;j<numTrainingPoints;j++){

		//double sampleFrom=U();
		//if( sampleFrom< 1.0/3.0)
		//{
		//    klVector<TYPE> rv=X1();
		//    trainingPoints[j][0]=rv[0]; trainingPoints[j][1]=rv[1];trainingPoints[j][2]=rv[2];
		//    trainingClasses[j]=0;
		//}
		//else if(1.0/3.0< sampleFrom && sampleFrom < 2.0/3.0)
		//{
		//    klVector<TYPE> rv=X2();
		//    trainingPoints[j][0]=rv[0]; trainingPoints[j][1]=rv[1];trainingPoints[j][2]=rv[2];
		//    trainingClasses[j]=1;
		//}
		//else
		//{
		//    klVector<TYPE> rv=X3();
		//    trainingPoints[j][0]=rv[0]; trainingPoints[j][1]=rv[1];trainingPoints[j][2]=rv[2];
		//    trainingClasses[j]=2;
		//}

		if( j<numTrainingPoints/3.0)
		{
			klVector<TYPE> rv=X1();
			trainingPoints[j][0]=rv[0]; trainingPoints[j][1]=rv[1];trainingPoints[j][2]=rv[2];
			trainingClasses[j]=0;
		}
		else if(numTrainingPoints/3.0< j && j < 2.0*numTrainingPoints/3.0)
		{
			klVector<TYPE> rv=X2();
			trainingPoints[j][0]=rv[0]; trainingPoints[j][1]=rv[1];trainingPoints[j][2]=rv[2];
			trainingClasses[j]=1;
		}
		else
		{
			klVector<TYPE> rv=X3();
			trainingPoints[j][0]=rv[0]; trainingPoints[j][1]=rv[1];trainingPoints[j][2]=rv[2];
			trainingClasses[j]=2;
		}


	}

#ifdef MCSVM_DEBUG
	{
		sprintf(ofilename,"//trainingPointsPreNormalize.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<trainingPoints<<endl;
		_fileostream.close();
	}
#endif

	cout<<trainingPoints.correlationMatrix(trainingPoints.covarianceMatrix() );

	double allOutlierFraction=0.05;
	double mixingCoeff=0.3;
	double smoThresh=1.0/10000.0;

	double sigma=1;  //choose the right one given the covariance matrices from the distributiuons above

	_tex<<"These are the SVM parameters - the RBF kernel is used\\begin{itemize}"<<endl; 
	_tex<<"\\item allOutlierFraction=0.05"<<endl;
	_tex<<"\\item mixingCoeff=0.3"<<endl;
	_tex<<"\\item smoThresh=1.0/10000.0"<<endl;
	_tex<<"\\item sigma=1"<<endl;
	_tex<<"\\end{itemize}"<<endl;

	klVector<TYPE> outlierFraction(numClasses);
	for(i=0;i<numClasses;i++)
	{
		outlierFraction[i]=allOutlierFraction;
	}
	klMulticlassSVMTrain<TYPE> multiclassTrain(trainingPoints,trainingClasses , numClasses, 
		outlierFraction,  mixingCoeff,  smoThresh, sigma);

	multiclassTrain();


#ifdef MCSVM_DEBUG
	{
		sprintf(ofilename,"//trainingPointsPostNormalize.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<trainingPoints<<endl;
		_fileostream.close();
	}
#endif


	klVector<TYPE> lagMults=multiclassTrain._lagMults;
	klVector<TYPE> intercepts=multiclassTrain._intercepts;
	klMatrix<TYPE> supportPoints=multiclassTrain._supportPoints;
	klVector<int> supportClasses=multiclassTrain._supportClasses;

	//Now classify and evaluate performance
	unsigned int numTestPoints=1024;
	klMulticlassSVMClassify<TYPE> testPoints(numTestPoints,numFeatureDimensions,supportPoints,
		supportClasses,lagMults, intercepts, numClasses,mixingCoeff,sigma );
	klVector<int> testClasses(numTestPoints);

	for(j=0;j<numTestPoints;j++){

		//double sampleFrom=U();
		//if( sampleFrom< 1.0/3.0)
		//{
		//    klVector<TYPE> rv=X1();
		//    testPoints[j][0]=rv[0]; testPoints[j][1]=rv[1];testPoints[j][2]=rv[2];
		//    testClasses[j]=0;
		//}
		//else if(1.0/3.0< sampleFrom && sampleFrom < 2.0/3.0)
		//{
		//    klVector<TYPE> rv=X2();
		//    testPoints[j][0]=rv[0]; testPoints[j][1]=rv[1];testPoints[j][2]=rv[2];
		//    testClasses[j]=1;
		//}
		//else
		//{
		//    klVector<TYPE> rv=X3();
		//    testPoints[j][0]=rv[0]; testPoints[j][1]=rv[1];testPoints[j][2]=rv[2];
		//    testClasses[j]=2;*/
		//}
		if( j<numTestPoints/3.0)
		{
			klVector<TYPE> rv=X1();
			testPoints[j][0]=rv[0]; testPoints[j][1]=rv[1];testPoints[j][2]=rv[2];
			testClasses[j]=0;
		}
		else if(numTestPoints/3.0< j && j < 2.0*numTestPoints/3.0)
		{
			klVector<TYPE> rv=X2();
			testPoints[j][0]=rv[0]; testPoints[j][1]=rv[1];testPoints[j][2]=rv[2];
			testClasses[j]=1;
		}
		else
		{
			klVector<TYPE> rv=X3();
			testPoints[j][0]=rv[0]; testPoints[j][1]=rv[1];testPoints[j][2]=rv[2];
			testClasses[j]=2;
		}
	}

#ifdef MCSVM_DEBUG
	{
		sprintf(ofilename,"//testPointsPreNormalized.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<testPoints<<endl;
		_fileostream.close();
	}

	{
		sprintf(ofilename,"//testClasses.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<testClasses<<endl;
		_fileostream.close();
	}

#endif


	_tex<<"The marginal sample moments (mean var skew kurtosis) for training points.\\newline"<<endl;
	_tex<<"\\begin{tabular}{ c |  c  c  c  c}"<<endl;
	_tex<<"Feature & $\\mu_1$ & $\\mu_2$ & $\\mu_3$ & $\\mu_4$ \\\\"<<endl;
	for(i=0;i<numFeatureDimensions;i++)
	{
		_tex<<i<<" & "<<trainingPoints.mean(i)<<" & "<<trainingPoints.variance(i)<<" & "<<trainingPoints.skewness(i)<<"& "<<trainingPoints.kurtosis(i)<<" \\\\"<<endl;
		_tex<<"\\hline"<<endl;
	}
	_tex<<"\\end{tabular}"<<endl;


	_tex<<"The marginal sample moments (mean var skew kurtosis) for test points.\\newline"<<endl;
	_tex<<"\\begin{tabular}{ c | c  c  c  c}"<<endl;
	_tex<<"Feature & $\\mu_1$ & $\\mu_2$ & $\\mu_3$ & $\\mu_4$ \\\\"<<endl;
	for(i=0;i<numFeatureDimensions;i++)
	{
		_tex<<i<<" & "<<testPoints.mean(i)<<" & "<<testPoints.variance(i)<<" & "<<testPoints.skewness(i)<<"& "<<testPoints.kurtosis(i)<<"\\\\"<<endl;
		_tex<<"\\hline"<<endl;
	}
	_tex<<"\\end{tabular}\\newline"<<endl;



	//We have to subtract the mean and multiply by the standard deviation 
	//of the training set.
	for(i=0;i<testPoints.getRows();i++)
	{
		for(j=0;j<testPoints.getColumns();j++)
		{
			testPoints[i][j] -= trainingPoints.mean(j);

			testPoints[i][j] /= std::sqrt(trainingPoints.variance(j) );

		}
	}

#ifdef MCSVM_DEBUG
	{
		sprintf(ofilename,"//testPointsPostNormalized.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<testPoints<<endl;
		_fileostream.close();
	}
#endif

	//Run the classification
	klMatrix<double> SVMResult;
	SVMResult=testPoints();

#ifdef MCSVM_DEBUG
	{
		sprintf(ofilename,"//SVMResult_raw_pval.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<SVMResult<<endl;
		_fileostream.close();
	}
#endif

	int classResult=-1;
	klVector<int > classDiffs(testClasses.getRows());
	double percentDiff=0;
	for(j=0;j<numTestPoints;j++)
	{
		double ans=-DBL_MAX;

		int floatingPointStatus=_status87();//Just in case we blew it above
		for(i=0;i<SVMResult.getColumns();i++)
		{
			if(SVMResult[j][i]>ans)
			{
				ans=SVMResult[j][i];
				classResult=i;
			}

		}
		classDiffs[j]=testClasses[j]-classResult;
		if(classDiffs[j]!=0)
			percentDiff+=1;

	}
	percentDiff/=classDiffs.getRows();
	_tex<<"The error rate for this run is "<<percentDiff<<""<<endl;

#ifdef MCSVM_DEBUG
	{
		sprintf(ofilename,"//classDiffs.txt");
		sprintf(arg,"%s%s",basefilename,ofilename);
		ofstream _fileostream(arg );
		_fileostream<<classDiffs<<endl;
		_fileostream.close();
	}
#endif

	_tex.flush();
	//_tex.close();

	delete arg;
	delete ofilename;
}


#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
template<class TYPE> void testKLMultiVariateRandomNumberGenerator2(ofstream &_tex)//const char* fileName)
{

	//ios_base::openmode wMode = ios_base::app;
	//ofstream _tex(fileName, wMode);	
	{
		unsigned i,j;
		klVector<TYPE> meanVector(3);
		meanVector[0] =0 ,meanVector[1]= 0,meanVector[2]= 0;
		klVector<unsigned int > seed1(3);
		seed1[0]=12;seed1[1]=2344;seed1[2]=21341;
		klMatrix<TYPE> cv(3,3);
		//cv[0][0]= .3, cv[0][1]=.3, cv[0][2]=.3,
		//cv[1][0]= .3, cv[1][1]=.3, cv[1][2]=.3,
		//cv[2][0]= .3, cv[2][1]=.3, cv[2][2]=.3;
		cv[0][0]= 1, cv[0][1]=.1, cv[0][2]=.2,
			cv[1][0]= .1, cv[1][1]=1, cv[1][2]=.2,
			cv[2][0]= .2, cv[2][1]=.2, cv[2][2]=1;

		klNormalMultiVariate<TYPE> T(meanVector,cv,false,seed1 );
		klSamplePopulation<TYPE> TS(65536,3);
		for(j=0;j<65536;j++)
		{
			klVector<TYPE> tv=T();
			TS.setRow(j,tv);
		}
		for(i=0;i<3;i++)
		{
			_tex<<"mean= "<<TS.mean(i)<<", variance="<<TS.variance(i)<<", skewness="<<TS.skewness(i)<<", kurtosis="<<TS.kurtosis(i)<<endl;
		}
		cout<<TS.covarianceMatrix()<<endl;

	}

	{//Generate a sample from a unifom mixture of three Gaussians in R^3
		unsigned i,j;
		klVector<TYPE> meanVector1(3);

		klVector<TYPE> meanVector2(3);

		klVector<TYPE> meanVector3(3);

		meanVector1[0] = 0 ,meanVector1[1]= 0, meanVector1[2]= 0;
		meanVector2[0] = 0 ,meanVector2[1]= 1 ,meanVector2[2]= 0;
		meanVector3[0] = 1 ,meanVector3[1]= 0 ,meanVector3[2]= 0;

		klVector<unsigned int > seed1(3);

		seed1[0]=12;seed1[1]=2344;seed1[2]=21341;
		klMatrix<TYPE> cv(3,3);

		cv[0][0]= 1, cv[0][1]=0, cv[0][2]=0,
			cv[1][0]= 0, cv[1][1]=1, cv[1][2]=0,
			cv[2][0]= 0, cv[2][1]=0, cv[2][2]=1;

		klNormalMultiVariate<TYPE> T(meanVector1,cv,false,seed1 );

		klSamplePopulation<TYPE> TS(65536,3);
		for(j=0;j<65536;j++)
		{
			klVector<TYPE> tv=T();
			TS.setRow(j,tv);
		}

		{ 
			ofstream _fileostream("R_3_Normal_N_0_1.txt");
			_fileostream<<TS<<endl;
			_fileostream.close();
		}

		for(i=0;i<3;i++)
		{
			_tex<<"mean= "<<TS.mean(i)<<", variance="<<TS.variance(i)<<", skewness="<<TS.skewness(i)<<", kurtosis="<<TS.kurtosis(i)<<endl;
		}

		_tex<<TS.covarianceMatrix()<<endl;

		double spd[3][3]=
		{   1,0,0,
		0,1,0,
		0,0,1
		};

		TYPE m1[3][3];
		TYPE m2[3][3];
		TYPE m3[3][3];

		klUniformRV<TYPE> Ud(.2,.4);
		TYPE r1=Ud();
		TYPE r2=Ud();
		TYPE r3=Ud();
		for(i=0;i<3;i++)
		{
			for(j=0;j<3;j++)
			{
				if(i!=j){
					m1[i][j]=r1*spd[i][j];
					m2[i][j]=r2*spd[i][j];
					m3[i][j]=r3*spd[i][j];
				}
				else 
				{
					m1[i][i]=m2[i][i]=m3[i][i]=.3;
				}
			}
		}
		void* vm=m1;
		klMatrix<TYPE> covarianceMatrix1((TYPE*)vm,3,3,false);

		vm=m2;
		klMatrix<TYPE> covarianceMatrix2((TYPE*)vm,3,3,false);

		vm=m3;
		klMatrix<TYPE> covarianceMatrix3((TYPE*)vm,3,3,false);

		seed1[0]=12;seed1[1]=2344;seed1[2]=21341;
		klNormalMultiVariate<TYPE> X1(meanVector1,covarianceMatrix1,false,seed1 );

		klVector<unsigned int > seed2(3); 
		seed2[0]=345;seed2[1]=4753;seed2[2]=857536;
		klNormalMultiVariate<TYPE> X2(meanVector2, covarianceMatrix2,false,seed2);

		klVector<unsigned int > seed3(3);
		seed3[0]=45423;seed3[1]=2358;seed3[2]=65321;
		klNormalMultiVariate<TYPE> X3(meanVector3, covarianceMatrix3,false,seed3);

		klSamplePopulation<TYPE> sample(256*3,3);

		//Create a uniform generator to decide which RV to pull a sample from.
		//This randomized the data set.  
		klUniformRV<double> U(0,1);

		for(j=0;j<256*3;j++){

			double sampleFrom=U();
			if( sampleFrom< 1.0/3.0)
			{
				klVector<TYPE> rv=X1();
				sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
			}
			else if(1.0/3.0< sampleFrom && sampleFrom < 2.0/3.0)
			{
				klVector<TYPE> rv=X2();
				sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
			}
			else
			{
				klVector<TYPE> rv=X3();
				sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
			}
		}

		{
			ofstream _fileostream("R_3_mixture_Gaussians.txt");
			_fileostream<<sample<<endl;
			_fileostream.close();
		}

		{
			//Do a pca on 3d rv

			klNormalMultiVariate<TYPE> X3(meanVector3, covarianceMatrix3,false,seed3);

			cout<<covarianceMatrix3<<endl;

			klSamplePopulation<TYPE> sample(256*3,3);
			for(j=0;j<256*3;j++)
			{
				klVector<TYPE> rv=X3();
				sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
			}

			cout<<sample.covarianceMatrix()<<endl;

			klPrincipalComponents<TYPE> pca=sample;

			klMatrix<TYPE> V=pca(2);
			cout<<V<<endl;

			klVector<TYPE> e=pca.eigenvalues();

			klMatrix<double> dist=sample.calcDistributionTests();
		}


		{ 
			klSamplePopulation<TYPE> sample(256*3,5);
			for(j=0;j<256*3;j++)
			{
				klVector<TYPE> rv=X3();
				sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];sample[j][3]=rv[2]+1;sample[j][4]=rv[2]+2;
			}

			// klIndependentComponents<TYPE> ica=sample;
			//klMatrix<TYPE> V=ica(3);bbcrevisit - do this!
			//cout<<V<<endl;
			//klVector<TYPE> e=ica.eigenvalues();
		}
	}

	klVector<TYPE> meanVector(3);
	klMatrix<TYPE> covarianceMatrix(3,3);
	//gernerate random positive definite matrix

	/*  Recipe for SPD
	The diagonal entries of the matrix have the form (0.5 + rand)*dim; 
	the lower-diagonal entries take the form rand — 0.5; 
	the upper-diagonal entries equal their lower-diagonal counterparts.
	Here, dim is the number of equations and rand a random-number generator with values in the interval [0, 1].
	This construction yields a symmetric and positive definite matrix.
	*/
	klUniformRV<double> U(0,1);

	unsigned int i,j;
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			if(i==j)
			{
				covarianceMatrix[i][j]=(0.5+U())*3;
				continue;
			}
			if(i<j)
			{
				covarianceMatrix[i][j]=(0.5-U());
				continue;
			}


		}
		meanVector[i]=i*i;
	}
	//now fill the upper half with the values in the lower half.
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			if(i>j)
			{
				covarianceMatrix[i][j]=covarianceMatrix[j][i];
				continue;
			}


		}
		meanVector[i]=i*i;
	}
	klVector<unsigned int> seed(3);
	seed[0]=32456;seed[1]=35656;seed[2]=7643;  

	cout<<covarianceMatrix;
	klNormalMultiVariate<TYPE> a(meanVector, covarianceMatrix,false,seed);


	klSamplePopulation<TYPE> sample(1024,3);
	for(j=0;j<1024;j++)
	{
		klVector<TYPE> rv=a();
		sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
	}
	for(i=0;i<3;i++)
	{
		_tex<<"mean= "<<sample.mean(i)<<", variance="<<sample.variance(i)<<", skewness="<<sample.skewness(i)<<", kurtosis="<<sample.kurtosis(i)<<endl;
	}

	_tex<<sample.covarianceMatrix();
	ofstream _fileostream("sample_1_gaussian.txt");
	_fileostream<<sample<<endl;
	_fileostream.close();

	_tex<<sample.correlationMatrix(sample.covarianceMatrix() );

	_tex.flush();
	//_tex.close();
}

#include "kl_principal_components.h"
#include "kl_multivariate_random_variable.h"
template<class TYPE> void PrincipalComponentsDemo(ofstream &_tex,unsigned int  &n)
{ 
	char* arg=new char[256];

	//Do a pca on 3d rv
	klMatrix<TYPE> covarianceMatrix(3,3);

	covarianceMatrix[0][0]=1; covarianceMatrix[0][1]=.2;covarianceMatrix[0][2]=.1;
	covarianceMatrix[1][0]=.2; covarianceMatrix[1][1]=2;covarianceMatrix[1][2]=.1;
	covarianceMatrix[2][0]=.1; covarianceMatrix[2][1]=.1;covarianceMatrix[2][2]=3;
	klVector<TYPE> meanVector(3);
	meanVector[0]=0;meanVector[1]=0;meanVector[2]=0;

	klNormalMultiVariate<TYPE> X(meanVector, covarianceMatrix);

	unsigned int numTestPoints=1024;

	klSamplePopulation<TYPE> sample(numTestPoints,3);
	unsigned int j;
	for(j=0;j<numTestPoints;j++)
	{
		klVector<TYPE> rv=X();
		sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
	}

	{
		sprintf(arg,"%ssample.txt",basefilename);
		ofstream fileostreamobj(arg );
		fileostreamobj<<sample<<endl;
		fileostreamobj.close();
	}

	klPrincipalComponents<TYPE> pca=sample;

	unsigned int numComponents=2;
	klMatrix<TYPE> V=pca(numComponents);

	{
		sprintf(arg,"%spca_first_2.txt",basefilename);
		ofstream fileostreamobj(arg );
		fileostreamobj<<V<<endl;
		fileostreamobj.close();
	}

	_tex<<"The "<<numComponents<<" eigenvectors:"<<endl;
	_tex<<V<<endl;

	klVector<complex<double> > e=covarianceMatrix.eigenvalues();

	{
		sprintf(arg,"%ssample_covariance_matrix.txt",basefilename);
		ofstream fileostreamobj(arg );
		fileostreamobj<<covarianceMatrix<<endl;
		fileostreamobj.close();
	}

	_tex<<"All of the eigenvalues of the covariance matrix:"<<endl;
	_tex<<e<<endl;

	{
		sprintf(arg,"%sseigenvalues_of_covariance_matrix.txt",basefilename);
		ofstream fileostreamobj(arg );
		fileostreamobj<<e<<endl;
		fileostreamobj.close();
	}

	klMatrix<double> dist=sample.calcDistributionTests();
	{
		sprintf(arg,"%sssample_univariate_dist_tests.txt",basefilename);
		std::vector<string> distTests = sample.getDistTestNames();

		ofstream fileostreamobj(arg );
		fileostreamobj<<distTests[0]<<"  "<<distTests[1]<<"    "<<distTests[2]<<"           "<<distTests[3]<<"     "<<distTests[4]<<endl;
		fileostreamobj<<dist<<endl;
		fileostreamobj.close();
	}

	_tex.flush();
	//_tex.close();
}

#include <vector>
template<class TYPE>  void __cdecl testKLRandomNumberGenerator2(ofstream &_tex)
{

	klBetaRV<TYPE>			_beta;

	klCauchyRV<TYPE>		_cauchy;

	klExponentialRV<TYPE>	_exponential;

	klGammaRV<TYPE>			_gamma;

	klGeneralizedInverseGaussianRV<TYPE>		_gaussian;

	klNormalBoxMullerRV<TYPE>		_normal;

	klNormalInverseApproxRV<TYPE>		_normalIA;

	klParetoRV<TYPE>		_pareto;

	klPoissonRV<TYPE>		_poisson;

	klUniformRV<TYPE>		_uniform;

	//klLevyAlphaStableRV<TYPE>		_lveyalphastabel;

	//klLevySkewAlphaStableRV<TYPE>		_lveyskewalphastabel;


	klRV<TYPE> ** pRV;
	pRV=new klRV<TYPE>* [12];
	*(pRV+0)=&_uniform;
	*(pRV+1)=&_cauchy;
	*(pRV+2)=&_exponential;
	*(pRV+3)=&_gamma;
	*(pRV+4)=&_gaussian;
	*(pRV+5)=&_normal;
	*(pRV+6)=&_normalIA;
	*(pRV+7)=&_pareto;
	*(pRV+8)=&_poisson;
	*(pRV+9)=&_beta;
	//*(pRV+10)=&_lveyalphastabel;
	//*(pRV+11)=&_lveyskewalphastabel;

	//These are the titles
	vector<string > names;
	names.push_back("uniform");
	names.push_back("cauchy");
	names.push_back("exponential");
	names.push_back("gamma");
	names.push_back("GIG");
	names.push_back("normal-box-muller");
	names.push_back("normal-inverse-approximation");
	names.push_back("pareto");
	names.push_back("poisson");
	names.push_back("beta");
	//names.push_back("lveyalphastabel");
	//names.push_back("levyskewalphastable");

	klRVUnitTestHarnessLaTex2<TYPE>(pRV,10,names,_tex);

	delete pRV;

}
#include "kl_sample_population.h"
template<class TYPE> void klRVUnitTestHarnessLaTex2(klRV<TYPE> ** RV,unsigned int size,vector<string > names, ofstream &_tex)
{
	//ios_base::openmode wMode = ios_base::app;
	//ofstream _tex(fileName, wMode);

	_tex<<"KL Univariate Random Number Gerneator Unit Test Output.  This Latex section, data, corresponding figures and graphics were produced by the KL Framework.  "<<endl<<endl;

	_tex<<"Clear The Floating point status word: $\\_clear87() ="<<_clear87()<<"$"<<endl; //clears foating point status word

	unsigned int sampleSize= 10000; //1000000;

	_tex<<"\\newline The sample size generated for this run is "<<sampleSize<<"."<<endl<<endl;

	klSamplePopulation<TYPE> sample(sampleSize,size);
	unsigned int i;
	unsigned int j; 
	for(i=0;i<size;i++)
	{
		for(j=0;j<sampleSize;j++)
		{
			klRV<TYPE>* t=*(RV+i);
			sample[j][i]=t->operator ()();
		}
	}

	klMatrix<TYPE> tr=sample.transpose();

	klMatrix<double> dt=sample.calcDistributionTests();

	_tex<<dt;

	for(i=0;i<size;i++)
	{
		klVector<TYPE> s=tr[i];

		s.desc=names[i];

		s.setupRange();

		unsigned int numBins=300;

		klVector<TYPE> hist=s.histogram(numBins,s.y0,s.y1);

		hist.desc=names[i];

		hist.x0=s.y0;
		hist.x1=s.y1;

		//Normalize the histogram
		//bbcrevisit we may want to implement normalizeHist(unsigned int sampleSize, unsigned int numBins)
		unsigned int k=0;
		for(k=0;k<hist.getColumns();k++)
		{
			hist[k]=hist[k]/(sampleSize/numBins);
		}

		//This will set the range 
		//hist.setupRange(); 

		const char* xAxis="x";
		const char* yAxis="y";
		const char* title=names[i].c_str();

		char* histFile=new char[1024];
		sprintf(histFile,"RV_%s_hist_%d_bins.txt",title,numBins);
		ofstream _hist(histFile);
		_hist<<hist;

	}

	_tex.flush();
	//_tex.close();

}


#include "kl_memory.h"
inline void  MemoryManagementDemo(ofstream &_tex,unsigned int  &n)
{
	int reserve = 1024*1024*64;

	MEMORYSTATUS _memoryInfo;
	GlobalMemoryStatus (&_memoryInfo);
	MEMORYSTATUSEX _memoryInfoEx;
	_memoryInfoEx.dwLength=sizeof(_MEMORYSTATUSEX);

	GlobalMemoryStatusEx (&_memoryInfoEx);
	_memoryInfoEx.ullAvailPhys;

	void* some_mem;
	int a;
	int total=0;

	_tex<<"Virtual Memory Manager.  Initalizing with all but 64MB of available physical memory."<<std::endl;
	{
		klVMemMgr vmemmgr(_memoryInfoEx.ullAvailPhys - reserve);
		for(a=1024;a<1024*20;a+=1024)
		{
			unsigned long b=a*sizeof(double) * a*sizeof(double);
			some_mem=vmemmgr.allocate(b);
			_tex<<"Virtual Memory Manager.  Allocated :  "<<b<<"  bytes"<<endl;
			_tex<<"\t  committed = "<<vmemmgr.committed()<<" available = "<<vmemmgr.available()<<endl;
			klNormalBoxMullerRV<double> normal;
			klMatrix<double> rand((double*)some_mem,a,a);
			vmemmgr.free(some_mem);
		}
	}

	GlobalMemoryStatusEx (&_memoryInfoEx);
	_memoryInfoEx.ullAvailPhys;
	{klHeapMgr heapmgr;
	heapmgr.klHeapInit(_memoryInfoEx.ullAvailPhys - reserve);
	_tex<<"Heap Memory Manager.  Initalizing with all but "<<reserve<<" of available physical memory."<<std::endl;
	for(a=1024;a<1024*20;a+=1024)
	{
		unsigned long b=a*sizeof(double) * a*sizeof(double);
		some_mem=heapmgr.allocate(b);
		_tex<<"Heap Memory Manager.  Allocated :  "<<b<<"  bytes"<<endl;
		klNormalBoxMullerRV<double> normal;
		klMatrix<double> rand((double*)some_mem,a,a);
		heapmgr.free(some_mem);
	}
	heapmgr.klHeapCompact();
	heapmgr.klHeapCheck();
	heapmgr.klHeapDump("HeapDump.txt");

	}
#ifdef _DEBUG
	try
	{
		klRedZoneHeapMgr redheapmgr;
		redheapmgr.klHeapInit(_memoryInfoEx.ullAvailPhys);
		_tex<<"Red Zone Heap Memory Manager.  Initalizing with all but "<<reserve<<" of available physical memory."<<std::endl;
		for(a=1024;a<2560;a+=512)  //bbc revisit we have a problem houston!
		{
			unsigned long b=a*sizeof(double) * a*sizeof(double);
			some_mem=redheapmgr.allocate(b);
			_tex<<"Red Zone Heap Memory Manager.  Allocated :  "<<b<<"  bytes"<<endl;
			klNormalBoxMullerRV<double> normal;
			klMatrix<double> rand((double*)some_mem,a,a);
			redheapmgr.free(some_mem);
		}
		redheapmgr.klHeapCompact();
		redheapmgr.klHeapCheck();
		redheapmgr.klHeapDump("RedZoneHeapDump.txt");
	}
	catch(...)
	{

	}
#endif
	//klAWEMemMgr awemgr(1024*1024*128);
	/*for(a=1;a<128;a++)
	{
	unsigned long b=a*sizeof(double) * a*sizeof(double);
	total+=b;
	some_mem=awemgr.allocate(b);
	klNormalBoxMullerRV<double> normal;
	klMatrix<double> rand((double*)some_mem,a,a);
	int j,k;
	for(j=0;j<a;j++)
	for(k=0;k<a;k++)
	rand[j][k]=normal();
	awemgr.free(some_mem);
	}*/

	_tex.flush();
	//_tex.close();
}


#include "kl_random_number_generator.h"
template<class TYPE>  void __cdecl testKLRandomNumberGeneratorL(const char* fileName)
{
	//The Univariate RV's

	klBetaRV<TYPE>			_beta;

	klCauchyRV<TYPE>		_cauchy;

	klExponentialRV<TYPE>	_exponential;

	klGammaRV<TYPE>			_gamma;

	klGeneralizedInverseGaussianRV<TYPE>		_gaussian;

	klNormalBoxMullerRV<TYPE>		_normal;

	klNormalInverseApproxRV<TYPE>		_normalIA;

	klParetoRV<TYPE>		_pareto;

	klPoissonRV<TYPE>		_poisson;

	klUniformRV<TYPE>		_uniform;


	klRV<TYPE> ** pRV;
	pRV=new klRV<TYPE>* [12];
	*(pRV+0)=&_uniform;
	*(pRV+1)=&_cauchy;
	*(pRV+2)=&_exponential;
	*(pRV+3)=&_gamma;
	*(pRV+4)=&_gaussian;
	*(pRV+5)=&_normal;
	*(pRV+6)=&_normalIA;
	*(pRV+7)=&_pareto;
	*(pRV+8)=&_poisson;
	*(pRV+9)=&_beta;

	//These are the titles
	vector<string > names;
	names.push_back("uniform");
	names.push_back("cauchy");
	names.push_back("exponential");
	names.push_back("gamma");
	names.push_back("GIG");
	names.push_back("normal-box-muller");
	names.push_back("normal-inverse-approximation");
	names.push_back("pareto");
	names.push_back("poisson");
	names.push_back("beta");
	//names.push_back("lveyalphastabel");
	//names.push_back("levyskewalphastable");

	//Generates a sample for each, writes histogram image, and data file in the ouput directory.
	//klRVUnitTestHarnessLaTex<TYPE>(pRV,12,names);
	//klRVUnitTestHarness<TYPE>(pRV,12,names);


	delete pRV;

}

#endif   //__kl_unit_tests__