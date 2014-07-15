// #include "kl_matrix.h"
// #include "kl_stat.h"
// #include "kl_random_number_generator.h"
// #include "kl_time_series.h"
// #include "kl_multivariate_random_variable.h"
// #include "kl_sample_population.h"
// #include "kl_principal_components.h"
// #include "kl_regression.h"
// #include "kl_multiclass_svm.h"
// #include "kl_wavelet.h"
// #include "kl_matrix_helper_fns.h"
// #include "kl_divergence_functions.h"
// #include "kl_util.h"
// #include "kl_unit_tests.h"
// #include "kl_matrix_facorizations.h"
// #include "kl_unit_test_wrapper.h"
// #include "kl_matlab_dependent_unit_tests.h"
// #include "kl_matlab_iface.h"
// #include "kl_arpack.h"
#include "kl_latex_helper_fns.h"
#include "kl_algorithm_paramters.h"
#include "kl_fast_gauss_transform.h"
#include "kl_point_cloud_generator.h"

extern char* basefilename;

int mainOneShotDriver(int argc, char**argv);

void RandomBallCoverTest(ofstream &_tex, klAlgorithmParameterContainer& klapc)
{
	int argcLoc=0; char**argvLoc;
	argvLoc= new char*[13*2];
	for(int i=0;i<13*2;i++)
	{
		argvLoc[i]  = new char[128];
	}

	sprintf(argvLoc[argcLoc++] ,"26");

	sprintf(argvLoc[argcLoc++] ,"-n");
	sprintf(argvLoc[argcLoc++] ,"1280");

	sprintf(argvLoc[argcLoc++] ,"-m");//Number of Queries
	sprintf(argvLoc[argcLoc++] ,"128");

	sprintf(argvLoc[argcLoc++] ,"-d");
	sprintf(argvLoc[argcLoc++] ,"2");

	sprintf(argvLoc[argcLoc++] ,"-r");
	sprintf(argvLoc[argcLoc++] ,"1000");
	

	//Use -x -q for binary read
	sprintf(argvLoc[argcLoc++] ,"-X");
    sprintf(argvLoc[argcLoc++] ,"D:\\Packages\\RandomBallCover\\x64\\Debug\\2DData.txt");//dataFileX 

	sprintf(argvLoc[argcLoc++] ,"-Q");
    sprintf(argvLoc[argcLoc++] ,"D:\\Packages\\RandomBallCover\\x64\\Debug\\2DQueryPoints.txt");//dataFileQ 

	sprintf(argvLoc[argcLoc++] ,"-O");
    sprintf(argvLoc[argcLoc++] ,"D:\\Packages\\RandomBallCover\\x64\\Debug\\RBC_Results.txt");//dataFileQ 


	//use -b to run brute force search (in addition to the RBC)\n");
    //use -e to run the evaluation procedure.  \n\tNOTE: this procedure takes as long as brute force.\n");
	
	sprintf(argvLoc[argcLoc++] ,"-b");
	sprintf(argvLoc[argcLoc++] ,"-e");

	/*if(!strcmp(argv[i], "-x"))
      dataFileX = argv[++i];
    else if(!strcmp(argv[i], "-q"))
      dataFileQ = argv[++i];
    else if(!strcmp(argv[i], "-X"))
      dataFileXtxt = argv[++i];
    else if(!strcmp(argv[i], "-Q"))
      dataFileQtxt = argv[++i];
    else if(!strcmp(argv[i], "-n"))
      n = atoi(argv[++i]);
    else if(!strcmp(argv[i], "-m"))
      m = atoi(argv[++i]);
    else if(!strcmp(argv[i], "-d"))
      d = atoi(argv[++i]);
    else if(!strcmp(argv[i], "-r"))
      numReps = atoi(argv[++i]);
    else if(!strcmp(argv[i], "-b"))
      runBrute = 1;
    else if(!strcmp(argv[i], "-e"))
      runEval = 1;
    else if(!strcmp(argv[i], "-o"))
      outFile = argv[++i];
    else if(!strcmp(argv[i], "-O"))
      outFiletxt = argv[++i];
    else if(!strcmp(argv[i], "-k"))
      K = atoi(argv[++i]);*/

	mainOneShotDriver(argcLoc, argvLoc);
	//mainExactDriver(argc, argv);
	for(int i=0;i<13*2;i++)
	{
		char* w = argvLoc[i];
		delete w;		
	}
	delete[] argvLoc;
}

