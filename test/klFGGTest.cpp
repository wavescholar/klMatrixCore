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

	klVector<int> kCenterClusterLabels(numPoints);

	KCenterClustering KCC(dimension,numPoints,  X_shifted_scaled.getMemory(),X.getClusterMembership().getMemory(),numCenters);
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

	fileName.str("");fileName.clear();
	title.str(""); title.clear();
	fileName<<"FGT"<<numCenters<<"_Centers";
	title<<"Fast Gauss Transform";
	color="'k*'";
	color= "'b.'";
	LatexInsert1DPlot(gaussTransform,_tex,basefilename,fileName.str().c_str(),title.str().c_str(),klHoldOnStatus::NoHold, color);

}