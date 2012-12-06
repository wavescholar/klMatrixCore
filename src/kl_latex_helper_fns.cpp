 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/
#include "kl_latex_helper_fns.h"
#include "kl_matlab_iface.h"


void startLatexDoc(string title,string author,string date,ofstream &_tex,string abs)
{
	_tex<<"\\documentclass[12pt]{article}"<<endl;
	_tex<<"\\usepackage[english]{babel}"<<endl;
	_tex<<"\\usepackage{amsmath,amsthm}"<<endl;
	_tex<<"\\usepackage{amsfonts}"<<endl;
	_tex<<"\\usepackage{graphicx}"<<endl;
	_tex<<"\\newtheorem{thm}{Theorem}[section]"<<endl;
	_tex<<"\\newtheorem{cor}[thm]{Corollary}"<<endl;
	_tex<<"\\newtheorem{lem}[thm]{Lemma}"<<endl;
	_tex<<"\\newtheorem{prop}[thm]{Proposition}"<<endl;
	_tex<<"\\theoremstyle{definition}"<<endl;
	_tex<<"\\newtheorem{defn}[thm]{Definition}"<<endl;
	_tex<<"\\theoremstyle{remark}"<<endl;
	_tex<<"\\newtheorem{rem}[thm]{Remark}"<<endl;
	_tex<<"\\numberwithin{equation}{section}"<<endl;
	_tex<<"\\begin{document}"<<endl;
	_tex<<"\\title{"<<title<<"}"<<endl;
	_tex<<"\\author{"<<author<<"}"<<endl;
	_tex<<"\\date{"<<date<<"}"<<endl;
	//_tex<<"\\begin{abstract}"<<endl;
	//_tex<<abs<<endl;
	//_tex<<"\\end{abstract}"<<endl;
	_tex<<"\\maketitle"<<endl;

}
void makeLatexSection(string sectionTitle,ofstream &_tex)
{
	_tex<<"\\subsubsection{"<<sectionTitle<<"}"<<endl;
}
void endLatexDoc(ofstream &_tex)
{
	_tex<<"\\end{document}"<<endl;
}
void LatexInsert3DPlot(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title)
{
	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	char* arg = new char[512];
	char* evalString = new char[512];
	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
	const char* xAxis=NULL;const char* yAxis=NULL;const char* zAxis=NULL;
	bool useExtents=true;bool holdOn=false;const char* marker=NULL;
	klScatterPlot3D(mat,filename.c_str(),title.c_str(), xAxis, yAxis,zAxis, useExtents, holdOn, marker);
	sprintf(evalString,"print -r1200 -depsc %s;",arg);
	engEvalString(matlabEngine, evalString);
	engEvalString(matlabEngine, "hold off;close(gcf);");
	sprintf(evalString,"epstopdf   %s",arg);
	system(evalString);
	sprintf(arg,"%s.pdf",filename.c_str());
	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	delete arg;
	delete evalString;
}
void LatexInsert1DPlot(klVector<double>& vec, ofstream &_tex, string dir,string filename,string title)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* arg = new char[512];
	char* evalString = new char[512];
	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
	klPlot1D<double>(vec,arg,title.c_str());
	sprintf(evalString,"print -r1200 -depsc %s;",arg);
	engEvalString(matlabEngine, evalString);
	engEvalString(matlabEngine, "hold off;close(gcf);");
	sprintf(evalString,"epstopdf   %s",arg);
	system(evalString);
	sprintf(arg,"%s.pdf",filename.c_str());
	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	delete arg;
	delete evalString;
}
void LatexInsertHeatMap(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* arg = new char[512];
	char* evalString = new char[512];
	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
	//klMatrix<TYPE>  c,const char* filename,  const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,const char* zAxis=NULL,bool useExtents=true,bool holdOn=false,const char* marker=NULL
	kl2DPlot<double>(mat,arg,title.c_str());
	sprintf(evalString,"print -r1200 -depsc %s;",arg);
	engEvalString(matlabEngine, evalString);
	engEvalString(matlabEngine, "hold off;close(gcf);");
	sprintf(evalString,"epstopdf   %s",arg);
	system(evalString);
	sprintf(arg,"%s.pdf",filename.c_str());
	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	delete arg;
	delete evalString;
}
void LatexInsertHistogram(klVector<double>& vec, unsigned int numBins,ofstream &_tex, string dir,string filename,string title)
{
	vec.setupRange();
	klVector<double> hist=vec.histogram(numBins,vec.y0,vec.y1);
	hist.setupRange();
	hist.setupDomain(vec.y0,vec.y1);
	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	char* arg = new char[512];
	char* evalString = new char[512];
	sprintf(arg,"%s\\%s.eps",dir.c_str(),filename.c_str());
	klPlot1D<double>(hist,arg,title.c_str());
	sprintf(evalString,"print -r1200 -depsc %s;",arg);
	engEvalString(matlabEngine, evalString);
	engEvalString(matlabEngine, "hold off;close(gcf);");
	sprintf(evalString,"epstopdf   %s",arg);
	system(evalString);
	sprintf(arg,"%s.pdf",filename.c_str());
	_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	delete arg;
	delete evalString;
}
