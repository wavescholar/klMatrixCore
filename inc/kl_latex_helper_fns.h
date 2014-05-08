 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 03 26 2014  *
 ********************************/
#ifndef __kl_latex_helper_fns__
#define __kl_latex_helper_fns__

#include "kl_matrix.h"
#include "kl_util.h"
#include <string>
using namespace std;

void startLatexDoc(string title,string author,string date,ofstream &_tex,string abs);
void makeLatexSection(string sectionTitle,ofstream &_tex);
void endLatexDoc(ofstream &_tex);
void LatexInsert1DPlot( klVector<double>& vec, ofstream &_tex, string dir,string filename,string title,bool holdon= false);
void LatexInsert2DScatterPlot( klVector<double>& vecX,klVector<double>& vecY ,ofstream &_tex, string dir,string filename,string title,bool holdon= false,const char* color=NULL);
void LatexInsert3DPlot( klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title,bool holdon= false);
void LatexInsertHistogram( klVector<double>& vec, unsigned int numBins,ofstream &_tex, string dir,string filename,string title);
void LatexInsertHeatMap(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title);
template<class TYPE> void LatexPrintMatrix( klMatrix<TYPE>& matrix, string name,ofstream &_tex);
template<class TYPE> void LatexPrintVector( klVector<TYPE>& klvector, string name,ofstream &_tex);
#include <iomanip>
template<class TYPE> void LatexPrintMatrix( klMatrix<TYPE>& matrix, string name,ofstream &_tex)
{
	unsigned int row = matrix.getRows();
	unsigned int col = matrix.getColumns();
	_tex<<"$"<<name<<" = "<<"\\left("<<endl;
	_tex<<"\\begin{array}{"<<endl;
	for(int i=0;i<col;i++)
	{
		_tex<<"c";
	}
	_tex<<"}"<<endl;
	_tex.precision(3);
	_tex.flags( ios::showpos | ios::internal | ios::fixed );
	_tex << setw(4);
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col-1;j++)
		{
			_tex<<matrix[i][j]<<" & ";
		}
		_tex<<matrix[i][col-1]<<" \\\\"<<endl;
	}
	_tex<<"\\end{array}"<<endl;
	_tex<<"\\right)$ \\newline "<<endl<<endl;
	_tex.flags();
}
template<class TYPE> void LatexPrintVector( klVector<TYPE>& klvector, string name,ofstream &_tex)
{
	unsigned int col = klvector.getColumns();
	_tex<<"$"<<name<<" = "<<"\\left("<<endl;
	_tex<<"\\begin{array}{"<<endl;
	for(int i=0;i<col;i++)
	{
		_tex<<"c";
	}
	_tex<<"}"<<endl;
	_tex.precision(5);
	_tex.flags( ios::showpos | ios::internal | ios::fixed );
	_tex << setw(6);

	for(int j=0;j<col-1;j++)
	{
		_tex<<klvector[j]<<" & ";
	}
	_tex<<klvector[col-1]<<" \\\\"<<endl;

	_tex<<"\\end{array}"<<endl;
	_tex<<"\\right)$ \\newline "<<endl<<endl;
	_tex.flags();

}

#endif __kl_latex_helper_fns__