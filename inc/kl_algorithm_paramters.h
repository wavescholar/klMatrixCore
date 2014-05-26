 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
#ifndef __kl_alorithm_paramters__
#define __kl_alorithm_paramters__

#include <string>
#include <map>
#include <ostream>
#include "kl_matrix.h"
using namespace std;

//This is under construction. 

typedef enum klAlgorithmParameterType { klIntType=1, klDoubleType=2, klStringType=3, klDoubleVectorType=4, klDoubleMatrixType=5};

//This needs work - I'd like to avoid typecast
//This may be a place to use ref counted types.

class klAlgorithmParameterValue
{
public:

	klMatrix<double> klmd;
	klVector<double> klvd;
	__int64 iv;
	double dv;
	string sv;
};

class klAlgorithmParameter
{
public:

	klAlgorithmParameter()
	{
	}

	klAlgorithmParameter(string name) : _name(name)
	{
	}

	klAlgorithmParameter(string name, __int64 intValue) : _name(name)
	{
		klRCInt klrci;
		klrci.intV=intValue;
		_parameterValue = klrci; //bbcrevisit - make sure this is corret use!
		_parameterType= klAlgorithmParameterType::klIntType;
	}

	void setDescription(string descrption)
	{
		_description= descrption;
	}

	//bbcrevisit - make sure this is corret use!
	klRefCount<klMutex> getValue()
	{ 
		return _parameterValue;
	}

	klAlgorithmParameterType getType()
	{
		return _parameterType;
	}

	string getName()
	{
		return _name;
	}
	
	string getDescription()
	{
		return _description;
	}

private:
	
	string _name;
	string _description;
	klAlgorithmParameterType _parameterType;
	//klAlgorithmParameterValue parameterVaule;
	klRefCount<klMutex> _parameterValue;
};
	
typedef std::map<std::string, std::map<std::string, klAlgorithmParameter>>::iterator algorithmParameter_itertator;

//This class is derived from if necessary
class klAlgorithmParameterContainer
{
	void add(klAlgorithmParameter algorithmParameter)
	{

		string name = algorithmParameter.getName();


		std::pair<string ,klAlgorithmParameter> kvp;

		parameterMap[name]=algorithmParameter;

	}

	void describeAlgorithmParameters(ostream& str)
	{
		//parameterMap.begin()
		//for(algorithmParameter_itertator iterator = parameterMap.begin(); iterator != parameterMap.end(); iterator++) 
		for(auto iterator = parameterMap.begin(); iterator != parameterMap.end(); iterator++)
		{
			string key;
			klAlgorithmParameter value;

			key = iterator->first;
			value = iterator->second;
			str<<key<<"  : TYPE = "<<value.getType()<<" Description = "<<value.getDescription()<<endl;

			klRefCount<klMutex> vauleT = value.getValue();

			switch(value.getType())
			{
			case klAlgorithmParameterType::klDoubleMatrixType :
				{
					break;
				}
			case klAlgorithmParameterType::klDoubleType :
				{
					break;
				}
			case klAlgorithmParameterType::klDoubleVectorType :
				{
					break;
				}
			case klAlgorithmParameterType::klIntType :
				{
					klRCInt* klrcintp = dynamic_cast(&valueT); 
					str<<"\t\t Parameter Vaule = "<<klrcintp->intV<<endl;
					break;
				}
			case klAlgorithmParameterType::klStringType :
				{
					break;
				}
			}



			
		}
	}

protected:
	map<string,klAlgorithmParameter> parameterMap;

};

#endif //__kl_alorithm_paramters__


