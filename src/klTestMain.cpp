#include <stdio.h>
#include <fstream>
#include <iostream>
using namespace std;

#include <iostream>
using namespace std;
void unitTestMain();

int klMain()
{	
	FILE *stream ;
	if((stream = freopen("klDemo.tex", "a", stdout)) == NULL)
			throw "kl: error redirecting std::cout to a file.";

	unitTestMain();
	return 0;
}

