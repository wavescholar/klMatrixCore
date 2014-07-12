 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/
#include "kl_latex_helper_fns.h"
#include "kl_matlab_iface.h"

void startLatexDoc(string title,string author,string date,ofstream &_tex,string abs)
{
	//_tex<<"\\documentclass[12pt,landscape]{article}"<<endl;
	_tex<<"\\documentclass[9pt]{article}"<<endl;
	_tex<<"\\usepackage[english]{babel}"<<endl;
	_tex<<"\\usepackage{amsmath,amsthm}"<<endl;
	_tex<<"\\usepackage{amsfonts}"<<endl;
	_tex<<"\\usepackage{graphicx}"<<endl;
	_tex<<"\\usepackage[margin=0.2in]{geometry}"<<endl;
	_tex<<"\\newcommand{\\setlinespacing}[1]{\\setlength{\\baselineskip}{#1 \\defbaselineskip}}"<<endl;
	_tex<<"\\newcommand{\\doublespacing}{\\setlength{\\baselineskip}{2.0 \\defbaselineskip}}"<<endl;
	_tex<<"\\newcommand{\\singlespacing}{\\setlength{\\baselineskip}{\\defbaselineskip}}"<<endl;
	_tex<<"\\newcommand{\\A}{{\\cal A}}"<<endl;
	_tex<<"\\newcommand{\\h}{{\\cal H}}"<<endl;
	_tex<<"\\newcommand{\\s}{{\\cal S}}"<<endl;
	_tex<<"\\newcommand{\\W}{{\\cal W}}"<<endl;
	_tex<<"\\newcommand{\\BH}{\\mathbf B(\\cal H)}"<<endl;
	_tex<<"\\newcommand{\\KH}{\\cal  K(\\cal H)}"<<endl;
	_tex<<"\\newcommand{\\Real}{\\mathbb R}"<<endl;
	_tex<<"\\newcommand{\\Complex}{\\mathbb C}"<<endl;
	_tex<<"\\newcommand{\\Field}{\\mathbb F}"<<endl;
	_tex<<"\\newcommand{\\RPlus}{[0,\\infty)}"<<endl;
	_tex<<"\\newcommand{\\norm}[1]{\\left\\Vert#1\\right\\Vert}"<<endl;
	_tex<<"\\newcommand{\\essnorm}[1]{\\norm{#1}_{\\text{\\rm\\normalshape ess}}}"<<endl;
	_tex<<"\\newcommand{\\abs}[1]{\\left\\vert#1\\right\\vert}"<<endl;
	_tex<<"\\newcommand{\\set}[1]{\\left\\{#1\\right\\}}"<<endl;
	_tex<<"\\newcommand{\\seq}[1]{\\left<#1\\right>}"<<endl;
	_tex<<"\\newcommand{\\eps}{\\varepsilon}"<<endl;
	_tex<<"\\newcommand{\\To}{\\longrightarrow}"<<endl;
	_tex<<"\\newcommand{\\RE}{\\operatorname{Re}}"<<endl;
	_tex<<"\\newcommand{\\IM}{\\operatorname{Im}}"<<endl;
	_tex<<"\\newcommand{\\Poly}{{\\cal{P}}(E)}"<<endl;
	_tex<<"\\newcommand{\\EssD}{{\\cal{D}}}"<<endl;
	_tex<<"\\newcommand{\\field}[1]{\\mathbb{#1}}"<<endl;
	_tex<<"\\newcommand{\\C}{\\field{C}}"<<endl;
	_tex<<"\\newcommand{\\R}{\\field{R}}"<<endl;
	_tex<<"\\newcommand{\\script}[1]{\\mathcal{#1}}"<<endl;
	_tex<<"\\newcommand{\\fall}{\\; \\forall \\;}"<<endl;
	_tex<<"\\newcommand{\\exts}{\\; \\exists \\;}"<<endl;
	_tex<<"\\newcommand{\\mbf}[1]{\\mathbf{#1}}"<<endl;
	_tex<<"\\newcommand{\\binomial}[2]{\\biggl( \\begin{array}{c}  #1 \\\\ #2  \\\\ \\end{array} \\biggr) }"<<endl;
	_tex<<"\\newcommand{\\fderiv}[2]{ \\frac{d}{ d #1} \\: #2}"<<endl;
	_tex<<"\\newcommand{\\sderiv}[2]{ \\frac{d^2}{ d^2 #1} \\: #2}"<<endl;
	_tex<<"\\newcommand{\\pfderiv}[2]{ \\frac{\\partial}{ \\partial #1} \\: #2}"<<endl;
	_tex<<"\\newcommand{\\psderiv}[2]{ \\frac{\\partial^2}{ \\partial^2 #1} \\: #2}"<<endl;
	_tex<<"\\newcommand{\\mat}[1]{\\mathbf{#1}}"<<endl;
	_tex<<"\\DeclareSymbolFont{AMSb}{U}{msb}{m}{n}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dblz}{\\mathalpha}{AMSb}{\"5A}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dblr}{\\mathalpha}{AMSb}{\"52}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dblt}{\\mathalpha}{AMSb}{\"54}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dblq}{\\mathalpha}{AMSb}{\"51}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dbln}{\\mathalpha}{AMSb}{\"4E}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dblf}{\\mathalpha}{AMSb}{\"46}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dblc}{\\mathalpha}{AMSb}{\"43}"<<endl;
	_tex<<"\\DeclareMathSymbol{\\dbld}{\\mathalpha}{AMSb}{\"44}"<<endl;
	_tex<<"\\theoremstyle{plain}"<<endl;
	_tex<<"\\newtheorem{thm}{Theorem}[section]"<<endl;
	_tex<<"\\newtheorem{cor}[thm]{Corollary}"<<endl;
	_tex<<"\\newtheorem{lem}[thm]{Lemma}"<<endl;
	_tex<<"\\newtheorem{prop}[thm]{Proposition}"<<endl;
	_tex<<"\\theoremstyle{definition}"<<endl;
	_tex<<"\\newtheorem{defn}{Definition}[section]"<<endl;
	_tex<<"\\theoremstyle{remark}"<<endl;
	_tex<<"\\newtheorem{rem}{Remark}[section]"<<endl;
	_tex<<"\\numberwithin{equation}{section}"<<endl;
	_tex<<"\\renewcommand{\\theequation}{\\thesection.\\arabic{equation}}"<<endl;
	_tex<<"\\begin{document}"<<endl;
	_tex<<"\\title{"<<title<<"}"<<endl;
	_tex<<"\\author{"<<author<<"}"<<endl;
	_tex<<"\\date{"<<date<<"}"<<endl;
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

void LatexInsert3DPlot(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title,klHoldOnStatus holdon,const char* markerType)
{
	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	char* arg = new char[2048];
	char* evalString = new char[2048];
	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());

	const char* xAxis=NULL;
	const char* yAxis=NULL;
	const char* zAxis=NULL;
	bool useExtents=true;
	
	klScatterPlot3D(mat,filename.c_str(),title.c_str(), xAxis, yAxis,zAxis, useExtents, holdon, markerType);

	if(holdon==klHoldOnStatus::LastPlot  || holdon==klHoldOnStatus::NoHold )
	{
		sprintf(evalString,"print -r1200 -depsc %s;",arg);
		engEvalString(matlabEngine, evalString);

		engEvalString(matlabEngine, "hold off;close(gcf);");

		sprintf(evalString,"epstopdf   %s",arg);

		system(evalString);
		sprintf(arg,"%s.pdf",filename.c_str());
		_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	}
	delete arg;
	delete evalString;
}

void LatexInsert1DPlot(klVector<double>& vec, ofstream &_tex, string dir,string filename,string title,klHoldOnStatus holdon,const char* markerType)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* arg = new char[2048];
	char* evalString = new char[2048];
	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
	
	const char* xAxis=NULL;
	const char* yAxis=NULL;
	bool useExtents=true;
	unsigned int start=0;
	unsigned int finish=0;
		
	klPlot1D<double>(vec,arg,title.c_str(),xAxis,yAxis,useExtents,start,finish,holdon,markerType);

	if(holdon==klHoldOnStatus::LastPlot  || holdon==klHoldOnStatus::NoHold )
	{	
		sprintf(evalString,"print -r1200 -depsc %s;",arg);
		engEvalString(matlabEngine, evalString);
		engEvalString(matlabEngine, "hold off;close(gcf);");
		sprintf(evalString,"epstopdf   %s",arg);
		system(evalString);
		sprintf(arg,"%s.pdf",filename.c_str());
		_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	}

	delete arg;
	delete evalString;
}

void LatexInsert2DScatterPlot( klVector<double>& vecX,klVector<double>& vecY ,ofstream &_tex, string dir,string filename,string title,klHoldOnStatus holdon,const char* markerType)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* arg = new char[2048];
	char* evalString = new char[2048];

	const char* xAxis=vecX.desc.c_str();
	const char* yAxis=vecY.desc.c_str();

	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
	

	bool useExtents=true;
	unsigned int start=0;
	unsigned int finish=0;
		
	klScatterPlot2D<double>(vecX,vecY,arg,title.c_str(),xAxis,yAxis,useExtents,holdon,markerType);
	
	if(holdon==klHoldOnStatus::LastPlot  || holdon==klHoldOnStatus::NoHold )
	{	
		sprintf(evalString,"print -r1200 -depsc %s;",arg);
		engEvalString(matlabEngine, evalString);
		engEvalString(matlabEngine, "hold off;close(gcf);");
		sprintf(evalString,"epstopdf   %s",arg);
		system(evalString);
		sprintf(arg,"%s.pdf",filename.c_str());
		_tex<<"\\includegraphics[width=10.0cm,height=10.0cm]{"<<arg<<"}"<<endl<<endl;
	}
	
	delete arg;
	delete evalString;
}

void LatexInsertHeatMap(klMatrix<double>& mat, ofstream &_tex, string dir,string filename,string title)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* arg = new char[2048];
	char* evalString = new char[2048];
	sprintf(arg,"%s//%s.eps",dir.c_str(),filename.c_str());
	//klMatrix<TYPE>  c,const char* filename,  const char* title=NULL,const char* xAxis=NULL,const char* yAxis=NULL,const char* zAxis=NULL,bool useExtents=true,bool holdOn=false,const char* marker=NULL
	klHeatMapPlot<double>(mat,arg,title.c_str());
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
	/*vec.setupRange();
	klVector<double> hist=vec.histogram(numBins,vec.y0,vec.y1);
	hist.setupRange();
	hist.setupDomain(vec.y0,vec.y1);*/

	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	char* arg = new char[2048];
	char* evalString = new char[2048];
	sprintf(arg,"%s\\%s.eps",dir.c_str(),filename.c_str());
	
	//klPlot1D<double>(hist,arg,title.c_str());

	klPlotHistogram<double>(vec,arg,title.c_str());
		
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

void LatexInsertLegend(string legend)
{
	klMatlabEngineThreadMap klmtm;
	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );
	stringstream evalString;
	evalString<<"legend("<<legend<<");";
	engEvalString(matlabEngine, evalString.str().c_str());
	
}
void WritePlot(ofstream &_tex, string dir,string filename,char* title,char* xAxis,char* yAxis)
{
	klMatlabEngineThreadMap klmtm;

	Engine* matlabEngine=klmtm.find(klThread<klMutex>::getCurrentThreadId() );

	char* arg = new char[2048];
	char* evalString = new char[2048];

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

	sprintf(arg,"%s%s.eps",dir.c_str(),filename.c_str());
	sprintf(evalString,"saveas(gcf,'%s');",arg);
	engEvalString(matlabEngine, evalString);
	
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
