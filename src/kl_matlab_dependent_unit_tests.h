/*******************************
* WSCMP [2003] - [2014] WSCMP  *  
* Bruce B Campbell 03 26 2014  *
********************************/
#ifndef __kl_matlab_dependent_unit_tests__
#define __kl_matlab_dependent_unit_tests__
#include "kl_multiclass_svm.h"
#include "kl_principal_components.h"

template<class TYPE> void klMulticlassSVMHarnessMatlab(ofstream &_tex,unsigned int &n)
{
	char* evalString=new char[256];
	char* arg=new char[256];
	char errmsg[1024];
	errmsg[1023] = '\0';

	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	engOutputBuffer(matlabEngine, errmsg, 512);

	unsigned int i;
	unsigned int j;

	unsigned int numTrainingPoints;
	unsigned int numFeatureDimensions;
	unsigned int numClasses;

	if (klTestSize==klTestType::VERYLARGE)
	{
		numTrainingPoints=16384;
		numFeatureDimensions=20;
		numClasses=10;
	}

	if (klTestSize==klTestType::LARGE)
	{
		numTrainingPoints=8192;
		numFeatureDimensions=10;
		numClasses=5;	}

	if (klTestSize==klTestType::MEDIUM)
	{
		numTrainingPoints=2048;
		numFeatureDimensions=5;
		numClasses=5;	
	}

	if (klTestSize==klTestType::SMALL)
	{ 
		numTrainingPoints=1024;
		numFeatureDimensions=3;
		numClasses=3;
	}

	//bbcrevisit - git Task - this is the only config that works right now.
	//This code needs to be generalized.
	numTrainingPoints=1024;
	numFeatureDimensions=3;
	numClasses=3;

	klVector<TYPE> meanVector1(numFeatureDimensions); 
	meanVector1[0]=1.9;
	meanVector1[1]=0.1;
	meanVector1[2]=0.1;

	klVector<TYPE> meanVector2(numFeatureDimensions);
	meanVector2[0]=0.1;
	meanVector2[1]=1.9;
	meanVector2[2]=0.1;

	klVector<TYPE> meanVector3(numFeatureDimensions);
	meanVector3[0]=0;
	meanVector3[1]=0;
	meanVector3[2]=1.9;

	_tex<<"\\begin{itemize}"<<endl; 
	_tex<<"\\item Number or training points = "<<numTrainingPoints<<endl;
	_tex<<"\\item Feature dimension = "<<numFeatureDimensions<<endl;
	_tex<<"\\item Number or classes = "<<numClasses<<endl;
	_tex<<"\\end{itemize}"<<endl;

	_tex<<"{The mean vectors of the 3 classes}"<<endl<<endl;

	LatexPrintVector( meanVector1, "\\mu_1",_tex);
	LatexPrintVector( meanVector2, "\\mu_2",_tex);
	LatexPrintVector( meanVector3, "\\mu_3",_tex);

	//The Kullback-Leibler divergence: D($N(\mu_{1}, \Sigma_{1}),$N(\mu_{2}, \Sigma_{2}) = \int {N_{1} ln \frac{N_{1}}{N_{2}} dx

	klMatrix<TYPE> covarianceMatrix1 =klGenerateRandomSymmetricPositiveDefiniteMatrix<TYPE>(numFeatureDimensions,fastSeed() );
	klMatrix<TYPE> covarianceMatrix2 =klGenerateRandomSymmetricPositiveDefiniteMatrix<TYPE>(numFeatureDimensions,fastSeed() );
	klMatrix<TYPE> covarianceMatrix3 =klGenerateRandomSymmetricPositiveDefiniteMatrix<TYPE>(numFeatureDimensions,fastSeed() );

	_tex<<"A random SPD covairance matrix is generated for each of the classes.\\newline"<<endl<<endl;

	LatexPrintMatrix( covarianceMatrix1,"\\rho_1",_tex);
	LatexPrintMatrix( covarianceMatrix2,"\\rho_2",_tex);
	LatexPrintMatrix( covarianceMatrix3,"\\rho_3",_tex);

	_tex<<"Verify $L_1$ condition number of covariance. The diagonal entries of the matrix have the form $(0.5 + U(0,1) )*dim(Dom(Cov))$"<<endl; 
	_tex<<"The lower-diagonal entries take the form $U(0,1) - 0.5$. "<<endl;
	_tex<<"The $L_1$ condition numbers are :"<<endl;
	_tex<<"\\begin{itemize}"<<endl; 
	_tex<<"\\item "<<covarianceMatrix1.getConditionNumber(true)<<endl;
	_tex<<"\\item "<<covarianceMatrix2.getConditionNumber(true)<<endl;
	_tex<<"\\item "<<covarianceMatrix3.getConditionNumber(true)<<endl;
	_tex<<"\\end{itemize}"<<endl;

	//scale all of the elements of the covariance 
	covarianceMatrix1/=covarianceMatrix1.getColumns()*2;
	covarianceMatrix2/=covarianceMatrix2.getColumns()*2;
	covarianceMatrix3/=covarianceMatrix3.getColumns()*2;

	LatexInsertHeatMap(covarianceMatrix1,_tex, basefilename,"rv1_corr","Class 1 covariance");

	LatexInsertHeatMap(covarianceMatrix2,_tex, basefilename,"rv2_corr","Class 2 covariance");

	LatexInsertHeatMap(covarianceMatrix3,_tex, basefilename,"rv3_corr","Class 3 covariance");


	klVector<unsigned int > seed1(numFeatureDimensions);
	seed1[0]=12;seed1[1]=2344;seed1[2]=21341;//bbcrevisit - generalize

	klNormalMultiVariate<TYPE> X1(meanVector1,covarianceMatrix1,false,seed1 );

	klVector<unsigned int > seed2(numFeatureDimensions); 
	seed2[0]=345;seed2[1]=4753;seed2[2]=857536;

	klNormalMultiVariate<TYPE> X2(meanVector2, covarianceMatrix2,false,seed2);

	klVector<unsigned int > seed3(numFeatureDimensions);
	seed3[0]=45423;seed3[1]=2358;seed3[2]=65321;

	klNormalMultiVariate<TYPE> X3(meanVector3, covarianceMatrix3,false,seed3);

	klSamplePopulation<TYPE> trainingPoints(numTrainingPoints,numFeatureDimensions);
	klVector<int> trainingClasses(numTrainingPoints);

	for(j=0;j<numTrainingPoints;j++)
	{
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
	LatexInsert3DPlot(trainingPoints, _tex, basefilename,"trainingPoints","Multi-Class SVM Training Points");

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

	klVector<TYPE> lagMults=multiclassTrain._lagMults;
	klVector<TYPE> intercepts=multiclassTrain._intercepts;
	klMatrix<TYPE> supportPoints=multiclassTrain._supportPoints;
	klVector<int> supportClasses=multiclassTrain._supportClasses;

	//Now classify and evaluate performance
	unsigned int numTestPoints=1024;
	klMulticlassSVMClassify<TYPE> testPoints(numTestPoints,numFeatureDimensions,supportPoints,
		supportClasses,lagMults, intercepts, numClasses,mixingCoeff,sigma );
	klVector<int> testClasses(numTestPoints);

	for(j=0;j<numTestPoints;j++)
	{

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

	LatexInsert3DPlot(testPoints, _tex, basefilename,"testPoints","Multi-Class SVM Test Points");

	_tex<<"The marginal sample moments (mean var skew kurtosis) for training points.\\newline"<<endl;
	_tex<<"\\begin{tabular}{ c |  c  c  c  c}"<<endl;
	_tex<<"Feature & $\\mu_1$ & $\\mu_2$ & $\\mu_3$ & $\\mu_4$ \\\\"<<endl;
	for(i=0;i<numFeatureDimensions;i++)
	{
		_tex<<i<<" & "<<trainingPoints.mean(i)<<" & "<<trainingPoints.variance(i)<<" & "<<trainingPoints.skewness(i)<<"& "<<trainingPoints.kurtosis(i)<<" \\\\"<<endl;
		_tex<<"\\hline"<<endl;
	}
	_tex<<"\\end{tabular}"<<endl;
	_tex<<"\\newline"<<endl;
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

	//Run the classification
	klMatrix<double> SVMResult;
	SVMResult=testPoints();

	{
		char* fileNameCD= new char[1024];
		sprintf(fileNameCD,"%sSVMResult_raw_pval.txt",basefilename);
		ofstream _fileostream(fileNameCD );
		_fileostream<<SVMResult<<endl;
		_fileostream.close();
		delete fileNameCD;
	}

	int classResult=-1;
	klVector<double > classDiffs(testClasses.getRows());
	double percentDiff=0;
	for(j=0;j<numTestPoints;j++)
	{
		double ans=-DBL_MAX;

		int floatingPointStatus=_status87();
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

	LatexInsert1DPlot( classDiffs, _tex, basefilename,"classDiffs","Class Differences for Test Points");

	percentDiff/=classDiffs.getRows();
	_tex<<"The error rate for this run is "<<percentDiff<<"\\newline"<<endl;

	{
		char* fileNameCD= new char[1024];
		sprintf(fileNameCD,"%slSVMclassDiffs.txt",basefilename);
		ofstream _fileostream(fileNameCD);
		_fileostream<<classDiffs<<endl;
		_fileostream.close();
		delete fileNameCD;
	}

	flushall();
	_tex.flush();

	delete arg;
	delete evalString;
}

#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
template<class TYPE> void testKLMultiVariateRandomNumberGeneratorMatlab(ofstream &_tex,unsigned int &n)
{
	{
		_tex<<"Sample from $N(\\mu,\\Sigma)$"<<endl;
		unsigned i,j;
		klVector<TYPE> meanVector(3);
		meanVector[0] =0 ,meanVector[1]= 0,meanVector[2]= 0;
		klVector<unsigned int > seed1(3);
		seed1[0]=12;seed1[1]=2344;seed1[2]=21341;
		klMatrix<TYPE> cv(3,3);

		cv[0][0]= 1, cv[0][1]=0, cv[0][2]=0,
			cv[1][0]= 0, cv[1][1]=1, cv[1][2]=0,
			cv[2][0]= 0, cv[2][1]=0, cv[2][2]=1;

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
		_tex<<"Covariance Matrix "<<endl<<TS.covarianceMatrix()<<endl;

		LatexInsert3DPlot(TS, _tex, basefilename, "R_3_Normal","Sample");
	}

	{
		_tex<<"Generate a sample from a unifom mixture of three Gaussians in $R^3$"<<endl;
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

		double spd[3][3]=
		{ 
			1,0,0,
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
		LatexInsert3DPlot(sample, _tex, basefilename, "R_3_Normal_Mixture","Mixture Sample");

		_tex.flush();
	}
}


#include "kl_principal_components.h"
#include "kl_multivariate_random_variable.h"
template<class TYPE> void testklPrincipalComponentsMatlab(ofstream &_tex,unsigned int &n)
{

	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* evalString=new char[256];
	char* arg=new char[256];

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

	LatexInsert3DPlot(sample, _tex, basefilename,"PCAPoints","PCA Points");

	klPrincipalComponents<TYPE> pca=sample;

	klMatrix<TYPE> V=pca();

	_tex<<"The eigenvectors:"<<endl;
	_tex<<V<<endl;

	klVector<complex<double> > e=covarianceMatrix.eigenvalues();

	_tex<<"All of the eigenvalues of the covariance matrix:"<<endl;
	_tex<<e<<endl;

	klMatrix<double> dist=sample.calcDistributionTests();

}


template<class TYPE>  void __cdecl testKLRandomNumberGeneratorMatlab(ofstream &_tex,unsigned int &n)
{
	time_t time_of_day;
	struct tm *tm_buf;
	time_of_day = time( NULL );
	tm_buf=localtime(&time_of_day);

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

	//Generates a sample for each, writes histogram image, and data file in the ouput directory.
	klRVUnitTestHarnessLaTexMatlab<TYPE>(pRV,10,names,_tex);

	delete pRV;

}
#include "kl_sample_population.h"
#include "kl_time_series.h"
//The unit test harness is reponsible for taking a collection of components to be tested
//testing them, generating output, and summarizing the results in an html file.
template<class TYPE> void klRVUnitTestHarnessLaTexMatlab(klRV<TYPE> ** RV,unsigned int size,vector<string > names,ofstream &_tex)
{
	char* fileName=new char[512];
	char* linkName=new char[512];
	char* evalString=new char[1024];

	unsigned int sampleSize=  100000;

	_tex<<"The sample size generated for this run is "<<sampleSize<<"."<<endl<<endl;

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
	sample.calcDescriptiveStats();

	klMatrix<TYPE> tr=sample.transpose();

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

		hist=hist / (double)(sampleSize/numBins); 
		hist.setupRange(); 

		const char* xAxis="x"; const char* yAxis="y";
		const char* title=names[i].c_str();

		klMatlabEngineThreadMap klmtm;
		Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

		if(i%3==0)
			_tex<<"\\newpage"<<endl;
		if(s.desc.size()!=0)
		{
			sprintf(fileName,"%s%s.eps",basefilename,s.desc.c_str());
		}
		else
		{
			sprintf(fileName,"%sRV-%d.eps",basefilename,i);
		}
		klPlot1D(hist,fileName,title,xAxis,yAxis,true,0,0,false);
		sprintf(evalString,"print -r1200 -depsc %s;",fileName);
		engEvalString(matlabEngine, evalString);
		engEvalString(matlabEngine, "close gcf;delete gcf");
		sprintf(evalString,"epstopdf   %s",fileName);
		system(evalString);
		if(s.desc.size()!=0)
		{
			sprintf(fileName,"%s.pdf",s.desc.c_str());
		}
		else
		{
			sprintf(fileName,"RV-%d.pdf",i);
		}

		_tex<<s.desc<<" \\begin{tabular}{|c|c|c|c|}  mean & variance & skewness & kurtosis \\\\  \\hline"<<endl;
		_tex<<"$\\mu_1 = "<<sample.mean(i)<<"$ & $\\mu_2 = "<<sample.variance(i)<<"$ & $\\mu_3 = "<<sample.skewness(i)<<"$ & $\\mu_4 ="<<sample.kurtosis(i)<<"$ \\\\"<<endl;
		_tex<<"\\end{tabular}"<<endl<<endl<<"\\includegraphics[width=5cm,height=5cm]{"<<fileName<<"}"<<endl<<endl; 
	}

	delete fileName;
	delete linkName;
	delete evalString;
}

#endif //__kl_matlab_dependent_unit_tests__