#ifndef __kl_point_cloud_generator__
#define __kl_point_cloud_generator__

#include "kl_multivariate_random_variable.h"

class klGaussianMixture
{
public:

	klGaussianMixture(__int64 numPointsPerCenter, __int64 numCenters,__int64 dimension ,double scale) :
	  _data(numPointsPerCenter*numCenters,dimension),  _numPointsPerCenter(numPointsPerCenter), 
		  _numPoints(numPointsPerCenter*numCenters), _numCenters(numCenters), _dimension(dimension) , _scale(scale),
		  _uniformRG(0,1), _clusterMembership(numPointsPerCenter*numCenters), _clusterCenters(numCenters,dimension)
	{
		_uniformRG.reset( fastSeed());		

		for(__int64 c=0;c<numCenters;c++)
		{
			klVector<double> center(_dimension);
			for(__int64 i =0;i<_dimension;i++)
				center[i]=_uniformRG();
			
			_clusterCenters[c]=center;

			addCluster(center,c);
		}
	}

	klMatrix<double>& getData()
	{
		return _data;
	}

	klMatrix<double>& getClusterCenters()
	{
		return _clusterCenters;
	}
	
	klVector<int>& getClusterMembership()
	{
		return _clusterMembership;
	}

private:
		
	void addCluster(klVector<double> center,__int64 c)
	{
		klMatrix<double> covarianceMatrix =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(_dimension,fastSeed() );
	
		klout(covarianceMatrix);

		covarianceMatrix =covarianceMatrix * _scale;

		klout(covarianceMatrix);

		klNormalMultiVariate<double> X(center,covarianceMatrix,true);
		
		for(__int64 i =0;i<_numPointsPerCenter;i++)
		{
			_clusterMembership[c*_numPointsPerCenter +i] =(int)c;
			_data[c*_numPointsPerCenter +i ]=X();
		}
	}

	klMatrix<double> _data;
	klMatrix<double> _clusterCenters;
	__int64 _numPointsPerCenter;
	__int64 _numPoints;
	__int64 _numCenters;
	__int64 _dimension;
	double _scale;
	klUniformRV<double> _uniformRG;
	klVector<int> _clusterMembership;
};


class klUniformHyperCube
{
public:

	klUniformHyperCube(__int64 numPoints,__int64 dimension ) :
	  _numPoints(numPoints), _dimension(dimension), _data(numPoints,dimension)

	  {
		  size_t sourv=sizeof(klUniformRV<double>);

		  _uniformRGVect = new klUniformRV<double>[dimension];

		  void* pEnd = &_uniformRGVect + dimension*sourv;


		  for(int i=0;i<dimension;i++)
		  {
			  _uniformRGVect[i].reset(fastSeed()+i);
		  }

		  for(int i=0;i<_numPoints;i++)
		  {
			  for(int j=0;j<dimension;j++)
				  _data[i][j] = _uniformRGVect[j]();
		  }
	  }

	  ~klUniformHyperCube()
	  {
		  delete[] _uniformRGVect;
	  }


	  klMatrix<double>& getData()
	  {
		  return _data;
	  }
	  	  
private:

	klMatrix<double> _data;
	__int64 _numPoints;
	__int64 _dimension;
	klUniformRV<double>* _uniformRGVect;

};


//This is a 3-d test data set used in regression tests
inline void generateSicknessManifoldDataSet( klMatrix<double>& dataSet, klVector<int>& classLabels )
{
	
	if(dataSet.getColumns() !=3)
	{
		std::stringstream ANSI_INFO_ss (std::stringstream::in | std::stringstream::out );
		ANSI_INFO_ss<<"ANSI COMPILE INFO: " <<__DATE__<<"     "<<__TIME__<<"   "<<__FILE__<<"   "<<__LINE__<<"       "<<std::endl;
		std::string err ="generateSicknessManifoldDataSet( klMatrix<double>& sample ) is a 3-d data set." + ANSI_INFO_ss.str();		
		throw klError(err);		
	}

	if(dataSet.getRows() !=classLabels.getRows())
	{
		std::stringstream ANSI_INFO_ss (std::stringstream::in | std::stringstream::out );
		ANSI_INFO_ss<<"ANSI COMPILE INFO: " <<__DATE__<<"     "<<__TIME__<<"   "<<__FILE__<<"   "<<__LINE__<<"       "<<std::endl;
		std::string err ="generateSicknessManifoldDataSet bad size for classLabels : must match dataSet size" + ANSI_INFO_ss.str();		
		throw klError(err);		
	}

	//This is the original Maatlab code
	//mu1 = [-20 0];
	//sigma1 = [3 .4; .4 3];
	//
	//mu2 = [-15 0];
	//sigma2 = [3 0; 0 3];
	//
	//mu3 = [-4 0];
	//sigma3 = [3 0; 0 1.5];
	//
	//mu4 = [4 0];
	//sigma4 = [2 0; 0 1];
	//
	//X1=[mvnrnd(mu1,sigma1,1000)]
	//X2=[mvnrnd(mu2,sigma2,1000)];
	//X3=[mvnrnd(mu3,sigma3,1000)];
	//X4=[ mvnrnd(mu4,sigma4,1000)];
	//X=[X1;X2;X3;X4];
	//X3 = [X3,atan(X3(:,1))];
	//X2 = [X2,atan(X2(:,1))];
	//X1 = [X1,atan(X1(:,1))];
	//X4 = [X4,atan(X4(:,1))];
	//X=[X1;X2;X3;X4];

	klVector<double> mu1(2); 
	mu1[0]= -20; mu1[1]=0;
	klMatrix<double> sigma1(2,2) ;
	sigma1[0][0]= 3 ; sigma1[1][0]= .4; sigma1[0][1]=  .4; sigma1[1][1]=  3 ;
	klNormalMultiVariate<double> RV1(mu1,sigma1,true);
		
	klVector<double> mu2(2);
	mu2[0]= -15; mu2[1]=0;
	klMatrix<double>sigma2(2,2);
	sigma2[0][0]= 3 ; sigma2[1][0]= 0; sigma2[0][1]=  0; sigma2[1][1]=  3 ;
	klNormalMultiVariate<double> RV2(mu2,sigma2,true);

	klVector<double> mu3(2);
	mu3[0]= -4; mu2[1]=0;
	klMatrix<double>sigma3(2,2);
	sigma3[0][0]= 3 ; sigma3[1][0]= 0; sigma3[0][1]=  0; sigma3[1][1]=  1.5f ;
	klNormalMultiVariate<double> RV3(mu3,sigma3,true);
	
	klVector<double> mu4(2);
	mu4[0]= 4; mu4[1]=0;	
	klMatrix<double> sigma4(2,2);
	sigma4[0][0]= 2 ; sigma4[1][0]= 0; sigma4[0][1]=  0; sigma4[1][1]=  1 ;
	klNormalMultiVariate<double> RV4(mu4,sigma4,true);
	
	for(__int64 i=0;i<dataSet.getRows();i++)
	{
		klVector<double> sample(2);
		sample =0;
		switch(i%4)
		{
		case(0) :
			{
				sample = RV1();
				break;
			}
		case(1) :
			{
				sample = RV2();
				break;
			}
		case(2) :
			{
				sample = RV3();
				break;
			}
		case(3) :
			{
				sample = RV4();
				break;
			}
		}
		klVector<double> dataPoint(3);
		dataPoint[0]=sample[0];
		dataPoint[1]=sample[1];
		dataPoint[2]=atan(sample[0]);

		dataSet[i] = dataPoint;
		classLabels[i] = (i%4);
	}

	//Crude 
	klMatrix<double> dataSetT(dataSet.getRows(),3);
	klVector<int> classLabelsT(dataSet.getRows());
	__int64 currentRow=0;
	for(int j=0;j<4;j++)
	{
		for(__int64 i=0;i<dataSet.getRows();i++)
		{
			if(classLabels[i]==j)
			{
				dataSet[currentRow]=dataSet[i];
				classLabelsT[currentRow]=j;
				currentRow++;
			}
		}
	}
}

#endif