#include "kl_vector.h"
#include "kl_vsl.h"
#include "kl_unit_test_wrapper.h"
#include "kl_matlab_iface.h"
#include "kl_latex_helper_fns.h"
#include "kl_fast_gauss_transform.h"
#include "kl_point_cloud_generator.h"
#include <math.h>
//Visual Studio does not support C99 - this should be in math.h
//inline double round(double x) { return (x-floor(x))>0.5 ? ceil(x) : floor(x); }
#include <ostream>
#include <sstream>
extern const char* basefilename;

void klFGTTest(ofstream &_tex, __int64& n)
{
	unsigned int numPoints = 10000;
	unsigned int numSources=numPoints;
	unsigned int numCenters = 25;
	int dimension =n;
 
	//__int64 numPointsPerCenter, __int64 numCenters,__int64 dimension ,double scale
	klGaussianMixture X(numPoints/numCenters,numCenters,dimension,1.0f /1250.0f);

	stringstream fileName;stringstream title;
	
	fileName.str("");fileName.clear();
	title.str(""); title.clear();
	fileName<<"GaussianMixture_"<<numCenters<<"_Centers";
	title<<"Gaussian Mixture";
	char* color="'c.'";
	LatexInsert2DScatterPlot(X.getData().getColumn(0),X.getData().getColumn(1),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::FirstPlot, color);
	
	fileName.str("");fileName.clear();
	title.str(""); title.clear();
	fileName<<"GaussianMixture_ClusterCenters"<<numCenters<<"_Centers";
	title<<"Gaussian Mixture Cluster Centers";
	 color="'k*'";
	LatexInsert2DScatterPlot(X.getClusterCenters().getColumn(0),X.getClusterCenters().getColumn(1),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::LastPlot, color);

	//Source Weights
	klVector<double> q(numPoints);
	klUniformRV<double> uniform(0,1);
	
	for(int i=0;i<numPoints;i++)
		q[i]=uniform();

	double h=std::sqrt((double)dimension)*0.2*std::sqrt((double)dimension);

	double epsilon=1e-3f;

	double Klimit=round(0.2*sqrt((double)dimension)*100/h);

	ImprovedFastGaussTransformChooseParameters IFGTP(dimension,h,epsilon,Klimit);

	klMatrix<double>  X_shifted_scaled= X.getData();
		
	klVector<double> minValsColumnWise(dimension);
	minV(X_shifted_scaled, minValsColumnWise ,0);
	for(__int64 i=0;i<numPoints;i++)
	{
		for(__int64 j=0;j<dimension;j++)
		{
			X_shifted_scaled[i][j] = X_shifted_scaled[i][j] -minValsColumnWise[j];
		}
	}
	klVector<double> maxValsColumnWise(dimension);
	maxV(X_shifted_scaled, maxValsColumnWise ,0);

	//Check For Zeros Before We divide
	bool isOK = (maxValsColumnWise > 0.0).sum();  
	for(__int64 i=0;i<numPoints;i++)
	{
		for(__int64 j=0;j<dimension;j++)
		{
			X_shifted_scaled[i][j] = X_shifted_scaled[i][j]* (1.0f / maxValsColumnWise[j]);
		}
	}


	//Target Points
	klUniformHyperCube Y(numPoints, dimension );

	//KCenterClustering(int Dim,int NSources,  double *pSources,int *pClusterIndex,int NumClusters);

	klVector<int> kCenterClusterLabels(numPoints);

	KCenterClustering KCC(dimension,numPoints,  X_shifted_scaled.getMemory(),X.getClusterMembership().getMemory(),numCenters);
	//KCenterClustering KCC(dimension,numPoints,  X_shifted_scaled.getMemory(),kCenterClusterLabels.getMemory(),numCenters);
	KCC.Cluster();
		
	klVector<double> clusterRadii(numCenters);

	klMatrix<double> clusterCenters(numCenters,dimension);

	KCC.ComputeClusterCenters(numCenters,clusterCenters.getMemory(),kCenterClusterLabels.getMemory(),clusterRadii.getMemory() ) ;
	
	{		
		fileName.str("");fileName.clear();
		title.str(""); title.clear();
		fileName<<"KCenterClusterMemberships_"<<numCenters<<"_Centers";
		title<<"KCenter Cluster Memberships";
		char* color="'k*'";
		LatexInsert2DScatterPlot(X.getClusterCenters().getColumn(0),X.getClusterCenters().getColumn(1),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::FirstPlot, color);
		color="'b+'";
		LatexInsert2DScatterPlot(clusterCenters.getColumn(0),clusterCenters.getColumn(1),_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::LastPlot, color);
	}
	
	double MaxClusterRadius =KCC.MaxClusterRadius;
	
	ImprovedFastGaussTransformChooseTruncationNumber IFGTCTN(dimension,h,epsilon, MaxClusterRadius);	
	
	int TruncationNumber= IFGTCTN.p_max;

	klVector<double> gaussTransform(numPoints);

	ImprovedFastGaussTransform IFGT(dimension,
		numPoints,
		numSources,
		X_shifted_scaled.getMemory(),
		h,
		q.getMemory(),
		X_shifted_scaled.getMemory(),
		TruncationNumber,
		numCenters,
		X.getClusterMembership().getMemory(), 
		clusterCenters.getMemory(),
		clusterRadii.getMemory(),
		MaxClusterRadius,
		epsilon,
		gaussTransform.getMemory()	);	
	IFGT.Evaluate();




}


//void klFastGaussTransformTestParams();
//void testFastGaussTransform(const char* fileName);
//klVector<double> klFastGaussTransformTest();

//void klFGGTest()
//{
//	klFGGSmallTest();
//	testFastGaussTransform("klFGG.txt");
//	klVector<double> ans2=klFastGaussTransformTest();
//	
//klFastGaussTransformTestParams();
//
//	char* syscmd = new char[2048];
//	sprintf(syscmd,"mkdir %s",basefilename);
//	system(syscmd); 
//
//	int heapstatus = _heapchk();
//	time_t time_of_day;
//	struct tm *tm_buf;
//	time_of_day = time( NULL );
//	tm_buf=localtime(&time_of_day);
//	char* testRunDateTime = new char[1024];
//	char* regressionFile = new char[1024];
//	char* coutFile = new char[1024];
//	char* sysInfoFile = new char[1024];
//	heapstatus = _heapchk();
//
//	sprintf(testRunDateTime,"%d_%d_%d_%d_%d",tm_buf->tm_mon+1,tm_buf->tm_mday+1,tm_buf->tm_hour+1,tm_buf->tm_min+1,tm_buf->tm_sec+1);
//	sprintf(regressionFile,"%KLFGGTest%s.tex",basefilename,testRunDateTime);
//	sprintf(coutFile,"%skl_cout%s.txt",basefilename,testRunDateTime);
//	sprintf(sysInfoFile,"%sklFGGTest_SysInfo%s.txt",basefilename,testRunDateTime);
//
//
//	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
//	klMatlabEngineThreadMap klmtm;
//
//	Engine* matlabEngine=NULL;
//	if (!(matlabEngine = engOpen(NULL))) 
//		throw "klWorkflow: no matlab engine available";
//
//	engSetVisible(matlabEngine,false);
//#ifdef _DEBUG
//	engSetVisible(matlabEngine,true);
//#endif
//
//	klmtm.insert(thisThread, matlabEngine);
//
//	ofstream _tex(regressionFile);
//
//	ofstream _systemText(sysInfoFile);	
//
//	klUnitTestWrapper klutw(_tex,_systemText);
//
//	klutw.setDimension(0);
//}
//
//
//#include "kl_matrix.h"
//#include "kl_stat.h"
//#include "kl_random_number_generator.h"
//#include "kl_time_series.h"
//#include "kl_multivariate_random_variable.h"
//#include "kl_sample_population.h"
//#include "kl_principal_components.h"
//#include "kl_regression.h"
//#include "kl_multiclass_svm.h"
//#include "kl_wavelet.h"
//#include "kl_ML_helper_fns.h"
//#include "kl_bregman_divergence.h"
//#include "kl_util.h"
//#include "kl_unit_tests.h"
//#include "kl_matrix_facorizations.h"
//#include "kl_matlab_dependent_unit_tests.h"
//#include "kl_matlab_iface.h"
//#include "kl_arpack.h"
//#include "kl_fast_gauss_transform.h"
//#include <crtdbg.h>
//#include <iostream>
//
////bbcrevisit Big Mess
//void testFastGaussTransform(const char* fileName)
//{
//	ios_base::openmode wMode = ios_base::app;
//	ofstream _tex(fileName);
//	time_t time_of_day;
//	struct tm *tm_buf;
//	time_of_day = time( NULL );
//	tm_buf=localtime(&time_of_day);
//	startLatexDoc("Linear Regression","KL Software Libraries",asctime(tm_buf),_tex,""); 
//
//	char * arg = new char[256];
//
//	unsigned int i;
//	unsigned int j;
//
//	unsigned int kernelDim=64;
//	unsigned int featureDim=4;
//
//	unsigned int testPoints = 10;
//
//	klVector<double> meanVector1(featureDim);  meanVector1 = 0;
//
//	klMatrix<double> covariance = IdentityMatrix<double>(featureDim);// klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(featureDim,fastSeed() );
//
//	double sigma = mean(covariance.diag().getMemory(),featureDim);
//
//	klNormalMultiVariate<double> X1(meanVector1,covariance );
//
//	klSamplePopulation<double> X(kernelDim,featureDim);
//	klSamplePopulation<double> Y(testPoints,featureDim);
//
//	klVector<double> one(featureDim); one = 1.0;
//	for(j=0;j<kernelDim;j++)
//	{
//		X.setRow(j,one);//X.setRow(j,X1());
//
//	}
//	klVector<double> y(featureDim); y=1.0;
//	for(j=0;j<testPoints;j++)
//	{	y = y+  0.1;
//	Y.setRow(j,y);
//	}
//	LatexPrintMatrix(X,"X",_tex);
//	LatexPrintMatrix(Y,"Y",_tex);
//
//	klFastGaussTransform klfg(X,Y);
//	double kernelBandwidth =sigma;
//	klVector<double> dist =klfg(kernelBandwidth);
//
//	klMatrix<double> dm(dist.getMemory(),dist.getRows(),1);
//	LatexPrintMatrix(dm,"D",_tex);
//
//	endLatexDoc(_tex);
//	flushall();
//	_tex.close();
//	delete arg;
//}
//klVector<double> klFastGaussTransformTest()
//{
//	unsigned int i;
//	unsigned int j;
//	unsigned int numTrainingPoints=64;
//	unsigned int numFeatureDimensions=5;
//	unsigned int numTestPoints = 64;
//	double q = numTrainingPoints;//Total Weight of Gaussians
//
//	klVector<double> meanVector1(numFeatureDimensions);  meanVector1 = 0.0;
//	klVector<double> meanVector2(numFeatureDimensions);  meanVector2 = 0.5;
//	klMatrix<double> covarianceMatrix1 =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(numFeatureDimensions,fastSeed() );
//	klMatrix<double> covarianceMatrix2 =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(numFeatureDimensions,fastSeed() );
//
//	klNormalMultiVariate<double> X1(meanVector1,covarianceMatrix1 );
//	klNormalMultiVariate<double> X2(meanVector2, covarianceMatrix2);
//
//	klSamplePopulation<double> x(numTrainingPoints,numFeatureDimensions);
//	klSamplePopulation<double> y(numTrainingPoints,numFeatureDimensions);
//	double* X  =x.getMemory();
//	double* Y = y.getMemory();
//
//	klVector<double> one (3);one = 1.0;
//	klVector<double> oneplus(3); oneplus = 1.1;
//
//	for(j=0;j<numTrainingPoints;j++)
//	{
//		x.setRow(j,X1());
//
//	}
//	for(j=0;j<numTrainingPoints;j++)
//	{
//		y.setRow(j,X2());
//	}
//
//	double h = 1.0; //Kernel Bandwidth
//	double epsilon = 1.0e-3f; //Error bound
//
//	double KLimit = ceil(0.2f * sqrt((double)numFeatureDimensions)* 100/h);
//
//	ImprovedFastGaussTransformChooseParameters IFGTP(numFeatureDimensions,h,epsilon,KLimit);
//
//	//_CrtCheckMemory( );
//
//	int K = IFGTP.K; //number of clusters
//	int p_max = IFGTP.p_max; // maximum truncation number
//	double r = IFGTP.r; //cutoff radius
//
//	int *pClusterIndex = new int[numTrainingPoints];
//	for(int i=0;i<numTrainingPoints;i++)
//		*(pClusterIndex + i) =0;
//
//	KCenterClustering KCC(numFeatureDimensions,numTrainingPoints,X,pClusterIndex,K);
//
//	double *pClusterCenters = new double[K*numFeatureDimensions];
//	double *pClusterRadii = new double[K];
//
//	KCC.Cluster();
//	KCC.ComputeClusterCenters(K, pClusterCenters,pClusterIndex,pClusterRadii);	
//	double rx = KCC.MaxClusterRadius; // maximum radius of the clusters
//
//	// Initially the truncation number was chosen based on an estimate
//	//of the maximum cluster radius. But now since we have already run
//	// the clustering algorithm we know the actual maximum cluster radius.
//	ImprovedFastGaussTransformChooseTruncationNumber IFGTCTN(numFeatureDimensions,h,epsilon,rx);
//	p_max = IFGTCTN.p_max;
//
//	double *pWeights = new double[numTrainingPoints];
//	for(int i=0;i<numTrainingPoints;i++)
//		*(pWeights+i) = 1.0f/q;
//
//	klVector<double> results(numTestPoints);
//	double *pGaussTransform =  results.getMemory();
//
//	ImprovedFastGaussTransform IFGT(numFeatureDimensions,numTrainingPoints,numTestPoints,X,h,pWeights,Y,double(p_max),double(K),pClusterIndex,pClusterCenters,pClusterRadii,r,epsilon,pGaussTransform);
//	IFGT.Evaluate();
//
//	for(int i=0;i<numTestPoints;i++)
//		std::cout<<*(pGaussTransform +i)<<std::endl;
//
//	return results;
//}
//void klFastGaussTransformTestParams()
//{
//	unsigned int i;
//	unsigned int j;
//	unsigned int numTrainingPoints=64;
//	unsigned int numFeatureDimensions=5;
//	unsigned int numTestPoints = 64;
//	double q = numTrainingPoints;//Total Weight of Gaussians
//
//	klVector<double> meanVector1(numFeatureDimensions);  meanVector1 = 0.0;
//	klVector<double> meanVector2(numFeatureDimensions);  meanVector2 = 0.5;
//	klMatrix<double> covarianceMatrix1 =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(numFeatureDimensions,fastSeed() );
//	klMatrix<double> covarianceMatrix2 =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(numFeatureDimensions,fastSeed() );
//
//	klNormalMultiVariate<double> X1(meanVector1,covarianceMatrix1 );
//	klNormalMultiVariate<double> X2(meanVector2, covarianceMatrix2);
//
//	klSamplePopulation<double> x(numTrainingPoints,numFeatureDimensions);
//	klSamplePopulation<double> y(numTrainingPoints,numFeatureDimensions);
//	double* X  =x.getMemory();
//	double* Y = y.getMemory();
//
//	klVector<double> one (3);one = 1.0;
//	klVector<double> oneplus(3); oneplus = 1.1;
//
//	for(j=0;j<numTrainingPoints;j++)
//	{
//		x.setRow(j,X1());
//
//	}
//	for(j=0;j<numTrainingPoints;j++)
//	{
//		y.setRow(j,X2());
//	}
//
//	double h = 1.0; //Kernel Bandwidth
//	double epsilon = 1.0e-3f; //Error bound
//
//	for(int i =1; i<128;i++)
//	{
//		numFeatureDimensions =i;
//		double KLimit = ceil(0.2f * sqrt((double)numFeatureDimensions)* 100/h);
//		ImprovedFastGaussTransformChooseParameters IFGTP(numFeatureDimensions,h,epsilon,KLimit);
//		int K = IFGTP.K; //number of clusters
//		int p_max = IFGTP.p_max; // maximum truncation number
//		double r = IFGTP.r; //cutoff radius
//
//		int *pClusterIndex = new int[numTrainingPoints];
//		for(int i=0;i<numTrainingPoints;i++)
//			*(pClusterIndex + i) =0;
//
//		KCenterClustering KCC(numFeatureDimensions,numTrainingPoints,X,pClusterIndex,K);
//
//		double *pClusterCenters = new double[K*numFeatureDimensions];
//		double *pClusterRadii = new double[K];
//
//		KCC.Cluster();
//		KCC.ComputeClusterCenters(K, pClusterCenters,pClusterIndex,pClusterRadii);	
//		double rx = KCC.MaxClusterRadius; 
//		ImprovedFastGaussTransformChooseTruncationNumber IFGTCTN(numFeatureDimensions,h,epsilon,rx);
//		p_max = IFGTCTN.p_max;
//
//		ImprovedFastGaussTransformChooseTruncationNumber IFGTCTN2(i,h,epsilon,rx);
//		p_max = IFGTCTN2.p_max;
//		cerr<<"n = "<<i<<" K = "<<K<<" p_max = "<<IFGTP.p_max<<" r = "<<r<<" p_max after cluster= "<<IFGTCTN.p_max<<endl;
//	}
//
//
//}
