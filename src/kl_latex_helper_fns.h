 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
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
void LatexInsert1DPlot( klVector<double>& vec, ofstream &_tex, string dir,string filename,string title);
void LatexInsert3DPlot( klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title);
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
	_tex<<"\\right)$"<<endl<<endl;
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
	_tex<<"\\right)$"<<endl<<endl;
	_tex.flags();

}



#endif __kl_latex_helper_fns__