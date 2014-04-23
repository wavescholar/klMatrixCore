This repository contains a high performance analytics framework based on Intel MKL. Use the solutions in the wavescholar ProjectFiles repository to build the libraries. Project files are provided for Visual Studio 2010 with Intel Compiler for the unmanaged C++ and Fortran libraries. 

Doxygen code documentation on gh-pages branch hosted here;
http://wavescholar.github.io/klMatrixCore/

The best way to use build everything is to clone the following repositories to a folder or drive that is mapped to d:  
klMatrixCore
klImageCore
klServiceCore
Packages

Put the solution and x64 directories in root of the d:
The x64 folders will have dlls and libs in them.  

Libraries are included in the Visual Studio projects as files rather than lib includes,  therefore the paths need to match up.  Relative paths won't work for this setup.  

  

  