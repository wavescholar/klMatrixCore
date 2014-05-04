
#include "kl_multivariate_random_variable.h"

class klGaussianMixture
{
public:

	klGaussianMixture(__int64 numPointsPerCenter, __int64 numCenters,__int64 dimension ,double scale) :
	  _data(numPointsPerCenter*numCenters,dimension),  _numPointsPerCenter(numPointsPerCenter), 
		  _numPoints(numPointsPerCenter*numCenters), _numCenters(numCenters), _dimension(dimension) , _scale(scale),
		  uniformRG(0,1)
	{
		uniformRG.reset( fastSeed());

		for(__int64 c=0;c<numCenters;c++)
		{
			klVector<double> center(_dimension);
			for(__int64 i =0;i<_dimension;i++)
				center[i]=uniformRG();
			addCluster(center,c);
		}
	}

	klMatrix<double>& getData()
	{
		return _data;
	}

private:

	
	void addCluster(klVector<double> center,__int64 c)
	{
		klMatrix<double> covarianceMatrix =klGenerateRandomSymmetricPositiveDefiniteMatrix<double>(_dimension,fastSeed() );
	
		klNormalMultiVariate<double> X(center,covarianceMatrix,true);
		
		for(__int64 i =0;i<_numPointsPerCenter;i++)
		{
			_data[c+i]=X();
		}
	}

	klMatrix<double> _data;
	__int64 _numPointsPerCenter;
	__int64 _numPoints;
	__int64 _numCenters;
	__int64 _dimension;
	double _scale;


	klUniformRV<double> uniformRG;
};
