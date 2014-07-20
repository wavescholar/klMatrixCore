 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *      
 * Bruce B Campbell 07 08 2014  *
 ********************************/
#include "kl_util.h"
void klIntegrationTest(bool useIntelMemMgr,klTestType klItegrationTestSize);
void klPerformanceTesting();

#include <iostream>
int main(int argc, char* argv[])
{
	//klPerformanceTesting();	
	klTimer klt;

	klt.tic();
	klIntegrationTest(false,klTestType::MEDIUM);
	cerr<<"dt klIntegrationTest(false,klTestType::MEDIUM) = "<<klt.toc()<<endl; 
	
	klt.tic();
	klIntegrationTest(true,klTestType::MEDIUM);
	cerr<<"dt klIntegrationTest(true,klTestType::MEDIUM) = "<<klt.toc()<<endl; 
	

	/*klt.tic();	
	klIntegrationTest(false,klTestType::LARGE);
	cerr<<"dt klIntegrationTest(false,klTestType::LARGE) = "<<klt.toc()<<endl; 

	klt.tic();
	klIntegrationTest(true,klTestType::SMALL);
	cerr<<"dt klIntegrationTest(true,klTestType::SMALL) = "<<klt.toc()<<endl; 
	
	klt.tic();
	klIntegrationTest(false,klTestType::SMALL);
	cerr<<"dt klIntegrationTest(false,klTestType::SMALL) = "<<klt.toc()<<endl; 
	

	klt.tic();
	klIntegrationTest(true,klTestType::LARGE);
	cerr<<"dt klIntegrationTest(true,klTestType::LARGE) = "<<klt.toc()<<endl; 
	 
	klt.tic();
	klIntegrationTest(false,klTestType::VERYLARGE);
	cerr<<"dt klIntegrationTest(false,klTestType::VERYLARGE) = "<<klt.toc()<<endl; 

	klt.tic();
	klIntegrationTest(true,klTestType::VERYLARGE);
	cerr<<"dt klIntegrationTest(true,klTestType::VERYLARGE) = "<<klt.toc()<<endl; 
			*/
	return 0;
}
