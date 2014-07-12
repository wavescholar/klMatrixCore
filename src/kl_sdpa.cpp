 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#include "sdpa-lib.hpp"
#include "sdpa-lib2.hpp"
void  SemidefiniteProgrammingCheck(ofstream &_tex,__int64  &n)
{	
	SDPA	Problem1;

	Problem1.Method = KSH;

	strcpy(Problem1.ParameterFileName, "d:/Packages/sdpa/param.sdpa");

	Problem1.ParameterFile = fopen("d:/Packages/sdpa/example1.dat-s", "r");

	strcpy(Problem1.InputFileName, "d:/Packages/sdpa/example1.dat-s");

	Problem1.InputFile = fopen("d:/Packages/sdpa/example1.dat-s","r");

	strcpy(Problem1.OutputFileName, "D:/klMAtrixCore/output/sdpa_example1.result_loc");

	Problem1.OutputFile = fopen("D:/klMAtrixCore/output/sdpa_example1.result_loc","w");

	Problem1.DisplayInformation = stdout;

	SDPA_initialize(Problem1);
	SDPA_Solve(Problem1);

	fclose(Problem1.InputFile);
	fclose(Problem1.OutputFile);

	Problem1.Delete();

};





