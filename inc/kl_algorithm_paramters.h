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

////This needs work - I'd like to avoid typecast
////This may be a place to use ref counted types.
//
//class klAlgorithmParameterValue
//{
//public:
//
//	klMatrix<double> klmd;
//	klVector<double> klvd;
//	__int64 iv;
//	double dv;
//	string sv;
//};

class klAlgorithmParameter
{
public:

	klAlgorithmParameter()
	{
	}

	klAlgorithmParameter(string name, __int64 intValue) : _name(name)
	{
		klRCInt klrci;
		klrci.intV=intValue;
		klrci.addRef();
		_parameterValue = klrci; 
		_parameterType= klAlgorithmParameterType::klIntType;
	}

	klAlgorithmParameter(string name, double dValue) : _name(name)
	{
		klRCDouble klrcd;
		klrcd.doubleV = dValue;
		_parameterValue = klrcd; 
		_parameterType= klAlgorithmParameterType::klDoubleType;
	}

	klAlgorithmParameter(string name, string sValue) : _name(name)
	{
		klRCString klrcs;
		klrcs.stringV = sValue;
		_parameterValue = klrcs; 
		_parameterType= klAlgorithmParameterType::klDoubleType;
	}

	klAlgorithmParameter(string name, klVector<double> kldv) : _name(name)
	{
		_parameterValue = kldv; 
		_parameterType= klAlgorithmParameterType::klDoubleVectorType;
	}

	klAlgorithmParameter(string name, klMatrix<double> kldm) : _name(name)
	{
		_parameterValue = kldm; 
		_parameterType= klAlgorithmParameterType::klDoubleMatrixType;
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

	string getStringValue()
	{
		if( _parameterType != klAlgorithmParameterType::klStringType)
			throw klError(_name + "is not a klAlgorithmParameterType::klStringType");
		klRCString* klrcs = static_cast<klRCString*>(&_parameterValue);
		string value =klrcs->stringV;
		return value;
	}

	double getDoubleValue()
	{
		if( _parameterType != klAlgorithmParameterType::klDoubleType)
			throw klError(_name + "is not a klAlgorithmParameterType::klDoubleType");
		klRCDouble* klrcd = static_cast<klRCDouble*>(&_parameterValue);
		double value =klrcd->doubleV;
		return value;
	}

	__int64 getIntValue()
	{
		if( _parameterType != klAlgorithmParameterType::klIntType)
			throw klError(_name + "is not a klAlgorithmParameterType::klIntType");
		klRCInt* klrci = static_cast<klRCInt*>(&_parameterValue);
		double value =klrci->intV;
		return value;
	}

	klVector<double> getDoubleVectortValue()
	{
		if( _parameterType != klAlgorithmParameterType::klDoubleVectorType)
			throw klError(_name + "is not a klAlgorithmParameterType::klDoubleVectorType");
		klVector<double>* kldv = static_cast<klVector<double> *>(&_parameterValue);
		klVector<double> value =*kldv;
		return value;
	}

	klMatrix<double> getDoubleMatrixValue()
	{
		if( _parameterType != klAlgorithmParameterType::klDoubleMatrixType)
			throw klError(_name + "is not a klAlgorithmParameterType::klDoubleMatrixType");
		klMatrix<double>* kldm = static_cast<klMatrix<double> *>(&_parameterValue);
		klMatrix<double> value =*kldm;
		return value;
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
	klRefCount<klMutex> _parameterValue;
};
	
typedef std::map<std::string, std::map<std::string, klAlgorithmParameter>>::iterator algorithmParameter_itertator;

//This class is derived from if necessary
class klAlgorithmParameterContainer
{
public:

	klAlgorithmParameterContainer()
	{

	}

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

			klRefCount<klMutex> valueT = value.getValue();
			void* valuePV= &valueT;

			switch(value.getType())
			{
			case klAlgorithmParameterType::klDoubleMatrixType :
				{
					klMatrix<double>* kldm = static_cast<klMatrix<double>* >(valuePV); 
					str<<"   Parameter Vaule = "<<kldm<<endl;
					break;
				}
			case klAlgorithmParameterType::klDoubleType :
				{					
					klRCDouble* klrcdouble = static_cast<klRCDouble*>(valuePV); 
					str<<"   Parameter Vaule = "<<klrcdouble->doubleV<<endl;
					break;
				}
			case klAlgorithmParameterType::klDoubleVectorType :
				{
					klVector<double>* kldv = static_cast<klVector<double>*>(valuePV); 
					str<<"   Parameter Vaule = "<<kldv<<endl;
					break;
				}
			case klAlgorithmParameterType::klIntType :
				{
					klRCInt* klrcintp = static_cast<klRCInt*>(valuePV); 
					str<<"   Parameter Vaule = "<<klrcintp->intV<<endl;
					break;
				}
			case klAlgorithmParameterType::klStringType :
				{
					klRCString* klrcstring = static_cast<klRCString*>(valuePV); 
					str<<"   Parameter Vaule = "<<klrcstring->stringV<<endl;
					break;
				}
			}			
		}
	}
	
	klAlgorithmParameter getParameter(string parameter)
	{
		return parameterMap[parameter];
	}

	void setParameter(klAlgorithmParameter parameterValue)
	{
		parameterMap[parameterValue.getName()]= parameterValue;
	}


protected:
	map<string,klAlgorithmParameter> parameterMap;

};

class klFastGaussAlgorithmParameters : public klAlgorithmParameterContainer
{
public:
	klFastGaussAlgorithmParameters()
	{
		unsigned int numPoints =10000;
		unsigned int numSources=numPoints;
		unsigned int numCenters = 25;
		int dimension =2;

		klAlgorithmParameter numPointsP("NumberOfPoints",(__int64)numPoints);
		parameterMap["NumberOfPoints"] = numPointsP;

		klAlgorithmParameter numSourcesP("NumberOfSources",(__int64)numPoints);
		parameterMap["NumberOfSources"]=numSourcesP;

		klAlgorithmParameter numCentersP("NumberOfCenters",(__int64)25);
		parameterMap["NumberOfCenters"]=numCentersP;
			
    	klAlgorithmParameter dimensionP("Dimension",(__int64)2);		
		parameterMap["Dimension"]=dimensionP;
		
		describeAlgorithmParameters(std::cout);

	}

	klFastGaussAlgorithmParameters(unsigned int numPoints, unsigned int numSources, unsigned int numCenters ,int dimension)
	{
		klAlgorithmParameter numPointsP("NumberOfPoints",(__int64)numPoints);		
		klAlgorithmParameter numSourcesP("NumberOfSources",(__int64)numPoints);
		klAlgorithmParameter numCentersP("numberOfCenters",(__int64)numCenters);
    	klAlgorithmParameter dimensionP("Dimension",(__int64)dimension);

		parameterMap["NumberOfPoints"] = numPointsP;
		parameterMap["NumberOfSources"]=numSourcesP;
		parameterMap["NumberOfCenters"]=numCentersP;
		parameterMap["Dimension"]=dimensionP;
	}

private:  //For now.

	unsigned int numPoints;
	unsigned int numSources;
	unsigned int numCenters ;
	int dimension ;

};

#endif //__kl_alorithm_paramters__


