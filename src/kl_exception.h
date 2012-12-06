 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/
#ifndef __kl_exception__
#define __kl_exception__

#include "kl_SEH.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;
class klError : public  exception
{
public:
	klError(std::string msg) : exception( (msg+ klStackWalkFn()).c_str() ) 
	{
	
	}

};


#endif //__kl_exception__