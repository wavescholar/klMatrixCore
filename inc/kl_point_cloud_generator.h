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
#endif