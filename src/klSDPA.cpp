 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#include "sdpa-lib.hpp"
#include "sdpa-lib2.hpp"
void  klTestSDPA(ofstream &_tex,unsigned int  &n)
{	
	SDPA	Problem1;

	Problem1.Method = KSH;

	strcpy(Problem1.ParameterFileName, "d:/Packages/ConvexOptimization/SDPA_INTEL_BLAS/sdpa/param.sdpa");

	Problem1.ParameterFile = fopen("d:/Packages/ConvexOptimization/SDPA_INTEL_BLAS/sdpa/example1.dat-s", "r");

	strcpy(Problem1.InputFileName, "d:/Packages/ConvexOptimization/SDPA_INTEL_BLAS/sdpa/example1.dat-s");

	Problem1.InputFile = fopen("d:/Packages/ConvexOptimization/SDPA_INTEL_BLAS/sdpa/example1.dat-s","r");

	strcpy(Problem1.OutputFileName, "D:/klMAtrixCore/output/sdpa_example1.result_loc");

	Problem1.OutputFile = fopen("D:/klMAtrixCore/output/sdpa_example1.result_loc","w");

	Problem1.DisplayInformation = stdout;

	SDPA_initialize(Problem1);
	SDPA_Solve(Problem1);

	fclose(Problem1.InputFile);
	fclose(Problem1.OutputFile);

	Problem1.Delete();

};





