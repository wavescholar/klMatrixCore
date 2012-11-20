#include "kl_matrix.h"
#include "kl_stat.h"
#include "kl_random_number_generator.h"
#include "kl_time_series.h"
#include "kl_multivariate_random_variable.h"
#include "kl_sample_population.h"
#include "kl_principal_components.h"
#include "kl_large_dataset.h"
#include "kl_regression.h"
#include "kl_multiclass_svm.h"
#include "kl_wavelet.h"
#include "kl_ML_helper_fns.h"
#include "kl_bregman_divergence.h"
#include "kl_util.h"
#include "kl_unit_tests.h"
#include "kl_matrix_facorizations.h"
#include"kl_unit_test_wrapper.h"
#include "kl_image_processing_functors.h"
#include "kl_img_pca.h"
#include "kl_matlab_dependent_unit_tests.h"
#include "kl_matlab_iface.h"
#include "kl_arpack.h"
#include "kl_fast_gauss_transform.h"


void dir2Work(void)
{
	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=NULL;
	if (!(matlabEngine = engOpen(NULL))) 
		throw "klWorkflow: no matlab engine available";

	klmtm.insert(thisThread,matlabEngine);

	matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	engEvalString(matlabEngine, "DriveThumbnail");

}

void writeDOT(ofstream& _dot, klMatrix<double>& adjacency);

void graphWork(void )
{	
	const char* edgeWeightAdjacencyMatrix="";

	klMatrix<double> adjacency;
	
	std::string featureVectors[] = {"boo","boo","bob","boo"};
	char* featureVectors2[] = {"boo","boo","bob","boo"};
		

	char* arg=new char[256];
	const char* ofilename = "graph.dot";

	ios_base::openmode wMode = ios_base::app;

	ofstream _dot(ofilename, wMode);	

    writeDOT(_dot,adjacency);

	const char* psfilename="graph.ps";

	sprintf(arg,"C://bin//dot//dot -Tps %s -o %s",ofilename,psfilename);

	sprintf(arg,"C://bin//epstopdf   %s",psfilename);

	system(arg);
}


void writeDOT(ofstream& _dot, klMatrix<double>& adjacency)
 {    
    _dot<<"digraph G {";
    _dot<<"size=\"8,10\"; ratio=auto; node[fontsize=11]; orientation=landscape; labelloc=b; "<<endl;
	_dot<<"};"<<endl;
}
