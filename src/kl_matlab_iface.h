 /*******************************
 * WSCMP [2003] - [2014] WSCMP  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
#ifndef __kl_matlab_iface__
#define __kl_matlab_iface__
#include <string>
#include <stdio.h>
#include <malloc.h>
#include <fstream>


/* 
	OMFG ! From Microsoft :	Thanks for reporting this issue.
I looked at the source code for matrix.h <core/matlab/include/matrix.h>
It has a definition for char16_t "typedef CHAR16_T char16_t;". 
Also the C++ header "Microsoft Visual Studio 10.0\VC\include\yvals.h" also defines 
the same identifier(char16_t). Because of these two definition we get the redefinition 
error when matrix.h and iostream.h(it internally includes yvals.h) are included in some cpp file.
Declaration of char16_t in the yvals.h is newly introduced in dev10 and was not there in VS2008. 
Therefore you are getting this redefinition error now on VS 2010(Dev10).
Inorder to solve this I would recommend that Matlab should guard the declaration in their 
file with Microsoft version.
eg:
<<<<<<< HEAD
//#if (MICROSOFT VERSION is less than DEV10)
//typedef CHAR16_T char16_t;
//# endif
////Thanks, -Sunny Gupta   */
////=======
//#if (MICROSOFT VERSION is less than DEV10)
//typedef CHAR16_T char16_t;
//# endif
//Thanks, -Sunny Gupta   */
////This problem has been fixed in Matlab 2012 we keep it here for reference.
//>>>>>>> 90cc802a337e9ff0133377faed404f9a3375351f
//#ifdef _CHAR16T
//#define CHAR16_T
//#endif

#include "mat.h"  //Matlab Iface include file
#include "engine.h"  //Matlab Iface include file
#include "kl_matrix.h"
class klMatlabEngineThreadMap 
{
public:

	void SetGlobalEngine (Engine* theEngine)
	{	klGuard<klMutex> lg(lock);//bbcrevisit -we need to justify/ verify nothing bad [ie race deadlock ]can happen
	klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	cout<<"Setting Global Matlabl Engine From TID: "<<thisThread<<endl;
	theStaticEngine=theEngine;
	}


	void EvalMatlabString(const char* evalString)
	{
		klGuard<klMutex> lg(lock);
		klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
		cout<<"Locking and Using  Global Matlab Engine Frtom TID: "<<thisThread<<endl;
		//Thread blocking until Matlab allows more than one engine per process.
		if(this->theStaticEngine!=0)
		{
			engEvalString(theStaticEngine, evalString);
		}
	}

	void insert (klThreadId id, Engine * matlabEngine)
	{
		klGuard<klMutex> guard(lock);
		engineMap[id] = matlabEngine;
	}
	//This removes the engine from the map and closes the connection to Matlab.
	void remove (klThreadId id)
	{
		klGuard<klMutex> guard(lock);

		map<klThreadId, Engine * >::iterator i = engineMap.find(id);
		if (i != engineMap.end())
		{
			engClose((*i).second);
			engineMap.erase (i);
		}
	}
	static Engine* find (klThreadId id)
	{
		klGuard<klMutex> guard(lock);

		map<klThreadId, Engine*>::iterator i = engineMap.find(id);
		if (i == engineMap.end())
			return 0;

		return (*i).second;

	}
public :
	static klMutex lock;

	//This is for multiple engines in a single process
	static map<klThreadId, Engine*> engineMap;

	//static Engine* engine;//bbcrevisit remove

	static Engine* theStaticEngine;
};

//Pass in Tex for title, xAxix, and yAxis.
//useExtents directs kl to set the scale of the axis according to the 
//domain and range set in klVector<TYPE>
//setting start & finish directs the output to be windowed to that range
//start and finish are 0-based indices to the elements of c - bbc in case we need to index a subset?  Could do better.  Remove or improve this.
//hold on indicates multiple plots will be made to one gcf and the user will save the file.
//use the color attribute to specify a matlab color; r,g,b,c,m,y,k or [r,g,b] - use a ' before and after the spec
template<class TYPE> void klPlot1D(klVector<TYPE>  c,const char* filename,
								   const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,
								   bool useExtents=true, unsigned int start=0,unsigned int finish=0,bool holdOn=false,const char* color=NULL)

{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	//How to get the error message from matlab engine
	char errmsg[1024];
	errmsg[1023] = '\0';
	engOutputBuffer(matlabEngine, errmsg, 512);

	mxArray *T = NULL, *a = NULL, *d = NULL;

	unsigned int range=c.getRowSize();

	TYPE x0 =c.x0;
	TYPE x1 =c.x1;

	if(start==0 && finish ==0)
		T = mxCreateDoubleMatrix(1, c.getRowSize(), mxREAL);
	else
	{
		if(start>finish || finish>=c.getRowSize())
			throw "kllot1D: ERROR bad domain for window.";
		range=finish-start;

		T = mxCreateDoubleMatrix(1, range+1, mxREAL);
	}
	unsigned int i;
	double* pMx=mxGetPr(T);
	if(start==0 && finish ==0)
	{
		for(i=0;i<c.getRowSize();i++)
		{
			*(pMx+i)=(double)c[i];
		}

	}
	else
	{
		for(i=start;i<=finish;i++)
		{
			*( pMx+ (i-start)  )=(double)c[i];
		}

	}
	engPutVariable(matlabEngine, "T", T);

	char* evalString=new char[256];

	char* colorSpec = new char[256];
	if( color==NULL)
	{
		sprintf(colorSpec,"'b'");
	}
	else
	{
		sprintf(colorSpec,"%s",color);
	}

	if(c.x1>c.x0)
	{
		mxArray *Tx = NULL;
		Tx = mxCreateDoubleMatrix(1, c.getRowSize(), mxREAL);
		double* pTx=mxGetPr(Tx);
		double dx = (c.x1-c.x0)/c.getRowSize();
		for(i=0;i<c.getRowSize();i++)
		{
			*(pTx+i)=(double)c.x0 + i*dx;
		}
		engPutVariable(matlabEngine, "Tx", Tx);

		if(holdOn==true)
		{
			//sprintf(evalString,"figure('Visible','off');plot(%f:%f:%f,T)",c.x0+((c.x1-c.x0)/c.getRowSize()),(c.x1-c.x0)/c.getRowSize(),c.x1);
			sprintf(evalString,"plot(Tx,T)");
		}
		else
		{
			sprintf(evalString,"figure('Visible','off');kp=plot(Tx,T)");
		}
	}
	else
	{
		sprintf(evalString,"figure('Visible','off');plot(T)");
		
	}
	engEvalString(matlabEngine, evalString);

	if(title!=NULL)
	{
		sprintf(evalString,"title('%s');",title);//title({'This title','has 2 lines'}) % 
		engEvalString(matlabEngine, evalString);
	}	

	if(xAxis!=NULL)
	{
		sprintf(evalString,"xlabel('%s');",xAxis);
		engEvalString(matlabEngine, evalString);
	}
	if(yAxis!=NULL)
	{
		sprintf(evalString,"ylabel('%s');",yAxis);
		engEvalString(matlabEngine, evalString);
	}
	if(c.y0<c.y1)
	{
		sprintf(evalString,"set(gca,'YTick',%f:%f:%f);",c.y0,(c.y1-c.y0)/20,c.y1);
		//axis([xmin,xmax,ymin,ymax])
		//add 10% to the top and bottom
		sprintf(evalString,"axis([%f %f %f %f])",c.x0-(c.x1-c.x0)*.1 ,c.x1+(c.x1-c.x0)*.1,c.y0-(c.y1-c.y0)*.1,c.y1+(c.y1-c.y0)*.1);
		engEvalString(matlabEngine, evalString);
	}
	if(!holdOn)
	{
		sprintf(evalString,"saveas(gcf,'%s');",filename);
		engEvalString(matlabEngine, evalString);
	}
	mxDestroyArray(T);
	delete evalString;
}

//Meshplot of 3-D Data
template<class TYPE> void klScatterPlot3D(klMatrix<TYPE>  c,const char* filename,
										  const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,const char* zAxis=NULL,
										  bool useExtents=true,bool holdOn=false,const char* marker=NULL)
{

	//	klGuard<klMutex> lg(matlablock);
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );


	mxArray *T = NULL, *a = NULL, *d = NULL;

	if(c.getColumns() !=3)
		throw " template<class TYPE> void klMatlabPlot3D(klMatrix<TYPE>  - ERROR :  needs 3 dimensional data";

	T = mxCreateDoubleMatrix(c.getRows(),c.getColumns(), mxREAL);
	klMatrix<TYPE>  ctr=c.transpose();
	double* pd=mxGetPr(T);
	memcpy((char *) mxGetPr(T), (char *) ctr.getMemory(), ctr.getColumns()*ctr.getRows()*sizeof(TYPE));
	engPutVariable(matlabEngine, "T", T);

	char* evalString=new char[256];
	d = engGetVariable(matlabEngine, "T");
	//How to get the error message from matlab engine
	char errmsg[1024];
	errmsg[1023] = '\0';
	engOutputBuffer(matlabEngine, errmsg, 512);

	//Set the marker if one is passed in.
	if(marker==NULL)
	{
		//sprintf(evalString,"figure('Visible','off');kp=scatter3(T(:,1),T(:,2),T(:,3))");
		sprintf(evalString,"figure('Visible','off');kp=scatter3(T(:,1),T(:,2),T(:,3))");
		engEvalString(matlabEngine, evalString);
	}
	else
	{
//		sprintf(evalString,"figure('Visible','off');kp=scatter3(T(:,1),T(:,2),T(:,3),'%s')",marker);
		sprintf(evalString,"figure('Visible','off');kp=scatter3(T(:,1),T(:,2),T(:,3),'%s')",marker);
		engEvalString(matlabEngine, evalString);
	}

	if(title!=NULL)
	{
		sprintf(evalString,"title('%s');",title);//title({'This title','has 2 lines'}) % 
		engEvalString(matlabEngine, evalString);
	}	

	if(xAxis!=NULL)
	{
		sprintf(evalString,"xlabel('%s');",xAxis);
		engEvalString(matlabEngine, evalString);
	}


	if(yAxis!=NULL)
	{
		sprintf(evalString,"ylabel('%s');",yAxis);
		engEvalString(matlabEngine, evalString);
	}

	if(zAxis!=NULL)
	{
		sprintf(evalString,"zlabel('%s');",zAxis);
		engEvalString(matlabEngine, evalString);
	}

	if(holdOn==false)
	{
		sprintf(evalString,"h=kp; saveas(h,'%s');",filename);
		engEvalString(matlabEngine, evalString);
		
	}
	mxDestroyArray(T);
	delete evalString;
}


template<class TYPE> void kl2DPlot(klMatrix<TYPE>  c,const char* filename,
								   const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,const char* zAxis=NULL,
								   bool useExtents=true,bool holdOn=false,const char* marker=NULL)
{

	//	klGuard<klMutex> lg(matlablock);
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	mxArray *T = NULL, *a = NULL, *d = NULL;

	T = mxCreateDoubleMatrix(c.getRows(),c.getColumns(), mxREAL);
	klMatrix<TYPE>  ctr=c.transpose();
	double* pd=mxGetPr(T);
	memcpy((char *) mxGetPr(T), (char *) c.getMemory(), ctr.getColumns()*ctr.getRows()*sizeof(TYPE));
	engPutVariable(matlabEngine, "T", T);

	char* evalString=new char[256];
	d = engGetVariable(matlabEngine, "T");
	//How to get the error message from matlab engine
	char errmsg[1024];
	errmsg[1023] = '\0';
	engOutputBuffer(matlabEngine, errmsg, 512);

	if(holdOn==false)
		sprintf(evalString,"figure('Visible','off');kp=imagesc(T);");
	else
		sprintf(evalString,"figure('Visible','off');kp=imagesc(T);");

	engEvalString(matlabEngine, evalString);

	if(title!=NULL)
	{
		sprintf(evalString,"title('%s');",title);
		engEvalString(matlabEngine, evalString);
	}	

	if(xAxis!=NULL)
	{
		sprintf(evalString,"xlabel('%s');",xAxis);
		engEvalString(matlabEngine, evalString);
	}


	if(yAxis!=NULL)
	{
		sprintf(evalString,"ylabel('%s');",yAxis);
		engEvalString(matlabEngine, evalString);
	}

	if(zAxis!=NULL)
	{
		sprintf(evalString,"zlabel('%s');",zAxis);
		engEvalString(matlabEngine, evalString);
	}

	if(!holdOn)
	{
		sprintf(evalString,"h=kp; saveas(h,'%s');",filename);
		engEvalString(matlabEngine, evalString);
	}
	mxDestroyArray(T);

	delete evalString;
}





template<class TYPE> void klPlotHistogram(klVector<TYPE>  c,const char* filename,
								   const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,
								   bool holdOn=false,const char* color=NULL)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	//How to get the error message from matlab engine
	char errmsg[1024];
	errmsg[1023] = '\0';
	engOutputBuffer(matlabEngine, errmsg, 512);

	mxArray *T = NULL, *a = NULL, *d = NULL;
	
	TYPE x0 =c.x0;
	TYPE x1 =c.x1;
	
	char* evalString=new char[256];

	char* colorSpec = new char[256];
	if( color==NULL)
	{
		sprintf(colorSpec,"'b'");
	}
	else
	{
		sprintf(colorSpec,"%s",color);
	}

	//Put the variable in Matlab
	T = mxCreateDoubleMatrix(1, c.getRowSize(), mxREAL);
	unsigned int i;
	double* pMx=mxGetPr(T);
	for(i=0;i<c.getRowSize();i++)
	{
		*(pMx+i)=(double)c[i];
	}
	engPutVariable(matlabEngine, "T", T);

	unsigned int numBins=std::floor(c.getRows()/10.0);
	if (numBins > 200 )
		numBins = 200;
	if (numBins <10)
		numBins =10;
	
	sprintf(evalString,"figure('Visible','off');hist(T,%d)",numBins);
	
	engEvalString(matlabEngine, evalString);

	if(title!=NULL)
	{
		//title({'This title','has 2 lines'}) %
		sprintf(evalString,"title('%s');",title); 
		engEvalString(matlabEngine, evalString);
	}	

	if(xAxis!=NULL)
	{
		sprintf(evalString,"xlabel('%s');",xAxis);
		engEvalString(matlabEngine, evalString);
	}
	if(yAxis!=NULL)
	{
		sprintf(evalString,"ylabel('%s');",yAxis);
		engEvalString(matlabEngine, evalString);
	}
	if(c.y0<c.y1)
	{
		sprintf(evalString,"set(gca,'YTick',%f:%f:%f);",c.y0,(c.y1-c.y0)/20,c.y1);
		sprintf(evalString,"axis([%f %f %f %f])",c.x0-(c.x1-c.x0)*.1 ,c.x1+(c.x1-c.x0)*.1,c.y0-(c.y1-c.y0)*.1,c.y1+(c.y1-c.y0)*.1);
		engEvalString(matlabEngine, evalString);
	}
	if(!holdOn)
	{
		sprintf(evalString,"saveas(gcf,'%s');",filename);
		engEvalString(matlabEngine, evalString);
	}
	mxDestroyArray(T);
	delete evalString;
}




template<class TYPE> klMatrix<TYPE> klMatlabImportMatrix(char* filen,TYPE dummy);
template<class TYPE> klMatrix<TYPE> klMatlabImportMatrix(char* file,TYPE dummy)
{
	MATFile *pmat;
	mxArray *pa;
	const char **dir;
	const char *name;
	int	  ndir;
	int i;
	unsigned int j;

	pmat = matOpen(file, "r");
	if (pmat == NULL)
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix :Error opening file";

	}
	dir = (const char **)matGetDir(pmat, &ndir);
	if (dir == NULL)
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix :Error reading directory of file";

	} 
	mxFree(dir);

	// In order to use matGetNextXXX correctly, reopen file to read in headers. 
	if (matClose(pmat) != 0) 
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix: Error closing file";
	}
	pmat = matOpen(file, "r");
	if (pmat == NULL) 
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix :Error reopening file";

	}
	for (i=0; i < ndir; i++) 
	{
		pa = matGetNextVariableInfo(pmat, &name);
		if (pa == NULL) 
		{
			throw "klMatrix<TYPE> klMatlabImportMatrix :Error reading in file";
		}
		//Verify all dimensions are 2
		int sz=mxGetNumberOfDimensions(pa);
		if (sz!=2)
		{
			throw "klMatrix<TYPE> klMatlabImportMatrix :only two dimensional arrays supported";
		}
		mxDestroyArray(pa);
	}

	// Reopen file to read in actual arrays. 
	if (matClose(pmat) != 0)
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix: Error closing file";
	}
	pmat = matOpen(file, "r");
	if (pmat == NULL)
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix: Error opening file";

	}

	// Read in each array.
	for (i=0; i<ndir; i++)
	{
		pa = matGetNextVariable(pmat, &name);
		if (pa == NULL)
		{
			throw "klMatrix<TYPE> klMatlabImportMatrix :Error reading in file";
		} 
	}

	if (matClose(pmat) != 0)
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix: Error closing file";
	}

	unsigned long rows, columns;
	rows =mxGetM(pa);
	columns = mxGetN(pa);
	int bytes =mxGetElementSize(pa);
	if(bytes==0)
	{
		throw "klMatrix<TYPE> klMatlabImportMatrix: unrecognized data type - possible cell array";

	}

	klMatrix<TYPE> data(rows,columns);

	// void* pData=mxGetData(pa);
	double* pData =mxGetPr(pa);

	for(i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			double* l=(pData + j*rows +i);
			//data[i][j]=static_cast<TYPE>( *l );//(TYPE)*(pData + j*bytes*rows +i);
			data[i][j]=(TYPE)(*l);

		}
	}
	return data;
}

template<class TYPE>void klMatlabExportVector(klVector<TYPE> vector, char* filename_prefix,TYPE dummy);
template<class TYPE> void klMatlabExportVector(klVector<TYPE> vector, char* filename_prefix,TYPE dummy)
{
	char* filename=new char[256];
	sprintf(filename,"%s.mat",filename_prefix);

	MATFile *pmat;
	mxArray *pa1;
	pmat = matOpen(filename, "w");
	unsigned long dataSize=vector.getRowSize();
	if (pmat == NULL) {printf("Error creating file %s\n", filename);    printf("(Do you have write permission in this directory?)\n");  }
	pa1 = mxCreateDoubleMatrix(1,dataSize,mxREAL);
	if (pa1 == NULL ) {printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);printf("Unable to create mxArray.\n");}
	char* name1=new char[256];
	//Copy Vector to double array...
	double* data1;
	if(sizeof(TYPE)!=sizeof(double))
	{
		int j;
		data1= new double[dataSize];
		for(j=0;j<dataSize;j++)
		{
			*(data1+j)=vector[j];
		}
	}
	else
		data1=vector.getMemory();
	memcpy((void *)(mxGetPr(pa1)), (void *)data1, sizeof(double)*dataSize);  
	sprintf(name1,"%s",filename_prefix);
	int status=0;
	status |= matPutVariable(pmat,name1, pa1);
	if (status != 0) {printf("%s :  Error using matPutArray on line %d\n", __FILE__, __LINE__); }  
	mxDestroyArray(pa1);
	if (matClose(pmat) != 0) {printf("Error closing file %s\n",filename);}
}
template<class TYPE>void klMatlabExportMatrix(klMatrix<TYPE> matrix, char* filename_prefix,TYPE dummy);
template<class TYPE> void klMatlabExportMatrix(klMatrix<TYPE> matrix, char* filename_prefix,TYPE dummy)
{
	char* filename=new char[256];
	sprintf(filename,"%s.mat",filename_prefix);
	MATFile *pmat;
	mxArray *pa1;
	unsigned long rows, columns;
	rows=matrix.getRows();
	columns=matrix.getColumns();
	pmat = matOpen(filename, "w");
	if (pmat == NULL) {printf("Error creating file %s\n", filename);    printf("(Do you have write permission in this directory?)\n");  }
	pa1 = mxCreateDoubleMatrix(rows,columns,mxREAL);
	if (pa1 == NULL ) {printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);printf("Unable to create mxArray.\n");}
	char* name1=new char[256];
	sprintf(name1,"%s",filename_prefix);
	double* data1;
	int i,j;
	unsigned long size=rows*columns;

	if(sizeof(TYPE)==sizeof(double))
	{
		data1=(double*)matrix.transpose().getMemory();
		memcpy((void *)(mxGetPr(pa1)), (void *)data1, sizeof(double)*rows*columns);  
	}
	else
	{
		data1=new double[size];
		for(i=0;i<columns;i++)
		{
			for(j=0;j<rows;j++)
			{
				klMatrix<TYPE> t=matrix.transpose();

				*(data1+i*rows+j)=(double)t[i][j];
			}
		}
		memcpy((void *)(mxGetPr(pa1)), (void *)data1, sizeof(double)*rows*columns);  
		delete data1;
	}
	int status=0;
	status |= matPutVariable(pmat, name1, pa1);
	if (status != 0) {printf("%s :  Error using matPutArray on line %d\n", __FILE__, __LINE__); }  
	mxDestroyArray(pa1);
	if (matClose(pmat) != 0) {printf("Error closing file %s\n",filename);}
}

#endif