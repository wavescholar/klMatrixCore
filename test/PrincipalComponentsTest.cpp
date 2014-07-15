#include "kl_principal_components.h"
#include "kl_multivariate_random_variable.h"
extern char* basefilename;

void PrincipalComponentsTest(ofstream &_tex,__int64  &n)
{ 
	char* arg=new char[256];

	//Do a pca on 3d rv
	klMatrix<double> covarianceMatrix(3,3);

	covarianceMatrix[0][0]=1; covarianceMatrix[0][1]=.2;covarianceMatrix[0][2]=.1;
	covarianceMatrix[1][0]=.2; covarianceMatrix[1][1]=2;covarianceMatrix[1][2]=.1;
	covarianceMatrix[2][0]=.1; covarianceMatrix[2][1]=.1;covarianceMatrix[2][2]=3;
	klVector<double> meanVector(3);
	meanVector[0]=0;meanVector[1]=0;meanVector[2]=0;

	klNormalMultiVariate<double> X(meanVector, covarianceMatrix);

	unsigned int numTestPoints=1024;

	klSamplePopulation<double> sample(numTestPoints,3);
	unsigned int j;
	for(j=0;j<numTestPoints;j++)
	{
		klVector<double> rv=X();
		sample[j][0]=rv[0]; sample[j][1]=rv[1];sample[j][2]=rv[2];
	}

	{
		sprintf(arg,"%ssample.txt",basefilename);
		ofstream fileostreamobj(arg );
		fileostreamobj<<sample<<endl;
		fileostreamobj.close();
	}

	klPrincipalComponents<double> pca=sample;

	klMatrix<double> V=pca();

	{
		sprintf(arg,"%spca_first_2.txt",basefilename);
		ofstream fileostreamobj(arg );
		fileostreamobj<<V<<endl;
		fileostreamobj.close();
	}

	_tex<<"The eigenvectors:"<<endl;
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
