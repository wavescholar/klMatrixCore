
#ifndef __kl_multiclass_svm__
#define __kl_multiclass_svm__

#include "kl_matrix.h"

#include "kl_sample_population.h"

#include <math.h>

//Originally this was going to be for float and double template parameters , as of 101905 this class should only be
//instantiated for type double
template <class TYPE> class klMulticlassSVMTrain : public klSamplePopulation<TYPE>
{
public:
    //These are the outputs of the training operation
    klVector<TYPE> _lagMults;
    klVector<TYPE> _intercepts;
    klMatrix<TYPE> _supportPoints;
    klVector<int>  _supportClasses;


    //Inputs of the training operation
    klVector<int> _trainingClasses;

    //	  nu                - fraction of the distribution to exclude - called
    //                     "outlierFraction" in much of the rest of the SVM code
    //						p x 1 array of desired fraction of outliers, one for each of p classes.
    klVector<TYPE> _outlierFraction;

    //  mixingCoeff =0.5 is a good place to start
    TYPE _mixingCoeff;

    unsigned int _numClasses;

    //	double smoThresh=1/10,000; is the  tolerance for SMO algorithm
    TYPE _smoThresh;


    //The width of RBF kernel.
    double _sigma;

    TYPE RadialBasisFunctionKernel(TYPE* x, TYPE* y) 
    {
        unsigned int     n;
        TYPE  sum, d;

        sum = 0;
        for(n = 0; n <getColumns(); n++)
        {
            d = x[n] - y[n];
            sum += d * d;
        }

        return(std::exp(-   sum /(2.0 * _sigma * _sigma)) );  
    }

    klMulticlassSVMTrain(klMatrix<TYPE> trainingPoints, klVector<int> trainingClasses ,unsigned int numClasses, klVector<TYPE> outlierFraction, TYPE mixingCoeff, TYPE smoThresh, TYPE sigma) 
        : klSamplePopulation<TYPE>(trainingPoints) , _trainingClasses(trainingClasses),_numClasses(numClasses),
        _outlierFraction(outlierFraction), _mixingCoeff(mixingCoeff), _smoThresh(smoThresh),_sigma(sigma)
    {

        if(getRows() !=_trainingClasses.getRows() )
            throw "klMulticlassSVMTrain  ERROR: incopmatible training points and training classes.";

        //Mixing coefficient \in [0,1] is recomended, but the algorithm may work regardless.
        //bbcrevisit making this a requirement.

        //Outlier fraction \in [0,1] is recomended, but the algorithm may work regardless.
        //bbcrevisit making this a requirement.

        //bbcrevisit requirements on SMO threshold.

        //bbcrevisit requirementson the ordering of the training points and trainingclasses

        //bbcrevisit randomizing the presentation of the training points to the SMO in case the user 
        //has ordered the data along one of the features.

        //Verify the training clsases are 0,1,2,...

        //Normalize the data - remind the user to extract the mean vector and stddev to normalize
        //test points when classifying.

        calcDescriptiveStats();

        unsigned int i;
        unsigned int j;

        for(i=0;i<getRows();i++)
        {
            for(j=0;j<getColumns();j++)
            {
                (_vectors+i)->operator[](j) -=  *(_mean+j);

                if(*(_variance+j)==0)
                    throw "klMulticlassSVMTrain ERROR throwing exception to avoid divide by zero.  klSamplePopulation had a zero variance for some dimension";

                (_vectors+i)->operator[](j) /=  sqrt( *(_variance+j) );

            }

        }




    }


    void operator()()
    {
        unsigned int i;
        unsigned int j;

        //First get the number of points in each class  

        klVector<TYPE> pointsPerClass(_numClasses);
        for(i=0;i<_numClasses;i++)
        {
            pointsPerClass[i]=0;
        }
        for(i=0;i<_trainingClasses.getRows();i++)
        {
            pointsPerClass[_trainingClasses[i] ]++;
        }
        _mixingCoeff = _mixingCoeff/(_numClasses - 1);

        klMatrix<TYPE> kernelMatrix(getRows(),getRows() );
        //bbcrevisit memset to 0, NaN, ...etc  on klMatrix, klVector

        for(i=0;i<getRows();i++)
        {
            for(j=0;j<getRows();j++)
            {
                kernelMatrix[i][j]=RadialBasisFunctionKernel((*this)[i].getMemory(),(*this)[j].getMemory());
            }
        }

        //Make the class weighting matrix

        //First initialize to 1
        klMatrix<TYPE> classWeightingMatrix(getRows(),getRows());
        for(i=0;i<getRows();i++)
        {
            for(j=0;j<getRows();j++)
            {
                classWeightingMatrix[i][j]=(TYPE)1.0;
            }
        }
        for(i=0;i<getRows();i++)
        {
            for(j=0;j<getRows();j++)
            {
                if(_trainingClasses[i]!=_trainingClasses[j])
                {
                    classWeightingMatrix[i][j]= -_mixingCoeff;
                    classWeightingMatrix[j][i]= -_mixingCoeff;
                }
            }
        }


#ifdef MCSVM_DEBUG
        {
            char* arg=new char[256];
            

            sprintf(arg,"%s//classWeightingMatrixTrain.txt",basefilename);
            ofstream fileostreamobj(arg );
            fileostreamobj<<classWeightingMatrix<<endl;
            fileostreamobj.close();

            sprintf(arg,"%%s//trainingKernelMatrixPreClassWeightMultiply.txt",basefilename);
            fileostreamobj.open(arg);
            fileostreamobj<<kernelMatrix<<endl;
            fileostreamobj.close();

            sprintf(arg,"%s//trainingKernelMatrix_DOTStarGraph.dot",basefilename);

            delete arg;
        }

#endif


        //kernelMatrix .* classWeightingMatrix
        for(i=0;i<getRows();i++)
        {
            for(j=0;j<getRows();j++)
            {
                if(_trainingClasses[i]!=_trainingClasses[j])
                {
                    kernelMatrix[i][j]=kernelMatrix[i][j]*classWeightingMatrix[i][j];
                }
            }
        }


        //Find lag mults with SMO
        klVector<TYPE> localLagMults(getRows() );

        //bbcrevisit - the SMO algorithm should be generalized to handle any floating point type.
        //Also - implement in terms of kl functionlaity one day
        TYPE** kernMx = new TYPE*[kernelMatrix.getRows()];
        for(i=0;i<kernelMatrix.getRows();i++)
        {
            *(kernMx + i)=kernelMatrix[i].getMemory();
        }
        SMONuMultiCache(kernMx, (int*) _trainingClasses.getMemory(), (double) _outlierFraction[0], (double) _mixingCoeff,
            (double) _smoThresh, (int) getRows(), (int) _numClasses, (TYPE*)localLagMults.getMemory());


        double lagMultEpsilon=0.01;//f1.17549e-8;//2.2x10^-7;
        unsigned int firstAboveEpsilon=0;
        unsigned int totalAboveEpsilon=0;
        //Now calculate the intercepts

        _intercepts.setup(_numClasses);

        for(i=0;i<localLagMults.getRows();i++)
        {
            if(localLagMults[i]>lagMultEpsilon)
                totalAboveEpsilon++;
        }

        for(j=0;j<_numClasses;j++)
        {
            //Find the first non-zero lag mult for this class
            for(i=0;i<localLagMults.getRows();i++)
            {
                if(_trainingClasses[i]==j && localLagMults[i]>lagMultEpsilon)//bbcrevisit one based issue 
                {
                    firstAboveEpsilon=i;
                    break;
                }


            }

            _intercepts[j]=0;
            //  int=lm dot kl(:,firstAboveEpsilon)
            for(i=0;i<kernelMatrix.getRows();i++)
            {
                _intercepts[j]+=localLagMults[i]* kernelMatrix[i][firstAboveEpsilon];
            }




        }

        _lagMults.setup(totalAboveEpsilon);

        _supportPoints.setup(totalAboveEpsilon,getColumns() ) ;

        _supportClasses.setup(totalAboveEpsilon);

        unsigned int index=0;
        unsigned int loc=0;
        for(loc=0;loc<getRows();loc++)
        {
            if(localLagMults[loc]>lagMultEpsilon)
            {
                _lagMults[index]=localLagMults[loc];

                _supportClasses[index]=_trainingClasses[loc];

                _supportPoints[index]=this->operator[](loc);

                index++;
            }
        }

#ifdef MCSVM_DEBUG
        {
            char* arg=new char[256];
            sprintf(arg,"%s//trainingKernelMatrix.txt",basefilename);
            ofstream fileostreamobj(arg );
            fileostreamobj<<kernelMatrix<<endl;
            fileostreamobj.close();

            sprintf(arg,"%s//trainingClasses.txt",basefilename);
            fileostreamobj.open(arg );
            fileostreamobj<<_trainingClasses<<endl;
            fileostreamobj.close();

            sprintf(arg,"%s//lagMults.txt",basefilename);
            fileostreamobj.open(arg );
            fileostreamobj<<_lagMults<<endl;
            fileostreamobj.close();

            sprintf(arg,"%s//supportPoints.txt",basefilename);
            fileostreamobj.open(arg);
            fileostreamobj<<_supportPoints<<endl;
            fileostreamobj.close();


            delete arg;
        }
#endif



    }//end operator()





    /* TakeStepNuMultiCache
    *
    * Subroutine for the nu-type SMO algorithm for multi-categorical data.  This
    * formulation of multi-categorical nu-type SVM is presented in "Support
    * Vector Representation of Multi-Categorical Data", Silvio Borer and
    * Wulfram Gerstner, Laboratory of Computational Neuroscience, EPFL, CH-1015
    * Lausanne, Switzerland.  This uses the SMO modifications
    * introduced by Keerthi, et. al. (q.v. "Improvements to Platt's SMO
    * Algorithm for SVM Classifier Design", Technical Report CD-99-14, Control
    * Division, Dept. of Mechanical and Production Engineering, National
    * University of Singapore).  Calculations for the derivation appear in TII
    * notebook 007.
    *   This version caches the kernel matrix, and is
    * thus faster for small (<500?) numbers of training points, but
    * inapplicable for larger sets.  
    *
    * [success, lagMults, fCache, idxSet, bLowAll, bUpAll, iLowAll, iUpAll] = ...
    *     TakeStepNuMultiCache(i1, i2, classes, lagMults, kernMx, fCache, idxSet,...
    *     bLowAll, bUpAll, iLowAll, iUpAll, nu)
    * i1                - index of the second Lagrange multiplier to vary for
    *                     maximal minimization of the cost function
    * i2                - index of the Lagrange multiplier to vary for
    *                     minimization of the cost function
    * classes           - class of each data point
    * lagMults          - n x 1 array of Lagrange multipliers found by minimizing the
    *                     quadratic cost function
    * kernMx            - n x n symmetric kernel array.  Elements (i,j) and (j,i)
    *                     are the value of the kernel evaluated for data points i
    *                     and j.  We assume kernMx has been multiplied, entry
    *                     by entry, by Yij, which is 1 if xi and xj are of the
    *                     same class, and -mixingCoeff otherwise.
    * fCache            - cache of error terms for the non-bound Lagrange
    *                     mulitpliers
    * idxSet            - n x 1 array giving the set membership for each
    *                     Lagrange multiplier (bound low, non-bound, bound
    *                     high)
    * bLowAll           - lower bound for the intercept for each class
    * bUpAll            - upper bound for the intercept for each class
    * iLowAll           - index of the Lagrange multiplier corresponding to
    *                     bLow
    * iUpAll            - index of the Lagrange multiplier corresponding to bUp
    * nu                - fraction of the distribution to exclude - called
    *                     "outlierFraction" in much of the rest of the SVM code
    * success           - set to 1 if the algorithm successfully updated the
    *                     multiplier
    */

    int TakeStepNuMultiCache(int i1, int i2, int *classes, double *lagMults,
        double **kernMx, double *fCache, int *idxSet,
        double *bLow, double *bUp, int *iLow,
        int *iUp, double nu, int numPoints)
    {
        const double    eps = 2.2204e-016;
        double          alpha1, alpha2, f1, f2, L, H, k11, k12, k22, eta, a1, a2;
        double          mxProd, mxProdL, mxProdH, Lobj, Hobj, thisBLow, thisBUp;
        int             thisClass, pointsInClass, k, thisILow, thisIUp;

        if(i1 == i2)
        {
            return(0);
        }


        /* We assume i1 and i2 are indices of points in the same class,
        * as they must be for the algorithm to work.
        */    
        alpha1 = lagMults[i1];
        alpha2 = lagMults[i2];
        f1 = fCache[i1];
        f2 = fCache[i2];
        thisClass = classes[i2];

        /* Compute L and H. */
        pointsInClass = 0;
        for(k = 0; k < numPoints; k++)
        {
            if(classes[k] == thisClass)
            {
                pointsInClass += 1;
            }
        }

        L = alpha1 + alpha2 - (1/(nu * ((double)pointsInClass)));
        if(L < 0)
        {
            L = 0;
        }

        H = 1/(nu * ((double)pointsInClass));
        if(H > (alpha1 + alpha2))
        {
            H = (alpha1 + alpha2);
        }

        k11 = kernMx[i1][i1];
        k12 = kernMx[i1][i2];
        k22 = kernMx[i2][i2];

        eta = 2 * k12 - k11 - k22;
        if(eta < -eps)  /* Should we test this against eps or 0? */
        {    
            a2 = alpha2 - ((f1-f2)/eta);
            if(a2 < L)
            {
                a2 = L;
            }
            else if(a2 > H)
            {
                a2 = H;
            }

        }    
        else
        {    
            /* Danger, Will Robinson!  This may not be the correct way to do this,
            * but here's the thinking: only alpha(i2) is being replaced, and thus
            * the difference between Lobj and Hobj depelse  on only a few terms of
            * the quadratic form.  In addition, the kernel matrix is symmetric,
            * meaning that the difference depends on even fewer terms of the
            * quadratic form, in fact, only one column of the kernel matrix is 
            * needed.  Calculate the sums using only these terms.
            * This has been optimized further by only performing the
            * matrix multiplication with column i2 of kernMx once, and
            * then adding and subtracting the necessary values to compute
            * Lobj and Hobj.
            */
            mxProd = 0;
            for(k = 0; k < numPoints; k++)
            {
                mxProd += lagMults[k] * kernMx[k][i2];
            }
            mxProdL = mxProd - (lagMults[i2] * k22) + (L * k22);
            Lobj = -(1/2) * (2 * L * mxProdL  - (L * L * k22));

            mxProdH = mxProd - (lagMults[i2] * k22) + (H * k22);
            Hobj = -(1/2) * (2 * H * mxProdH  - (H * H * k22));

            if(Lobj > Hobj + eps)
            {
                a2 = L;
            }
            else if(Lobj < Hobj - eps)
            {
                a2 = H;
            }
            else
            {
                a2 = alpha2;
            }

        }

        if(std::abs(a2 - alpha2) < eps * (a2 + alpha2 + eps))
        {
            return(0);
        }

        a1 = alpha1 + (alpha2 - a2);

        /* Update fCache(i) for i where lagMults(i) > 0 and lagMults(i) < 1/(nu *
        * pointsInClass).  Is this correct, or should every component of lagMults be
        * updated?  Well, technically, every element of fCache should be updated,
        * however, this algorithm is designed for speed, and fCache is updated only
        * for non-bound values.  The calling function, ExamineExampleNuCache.m,
        * updates the value of fCache for any bound values it uses at the time it
        * uses them.
        */
        lagMults[i1] = a1;
        lagMults[i2] = a2;

        for(k = 0; k < numPoints; k++)
        {
            if(idxSet[k] == 0)
            {
                fCache[k] = fCache[k] + (a1 - alpha1) * kernMx[k][i1] + (a2 - alpha2) * kernMx[k][i2];
            }
        }

        fCache[i1] = f1 + (a1 - alpha1) * k11 + (a2 - alpha2) * k12;
        fCache[i2] = f2 + (a1 - alpha1) * k12 + (a2 - alpha2) * k22;

        /* Update the index sets for indices i1 and i2. */
        if(a1 < eps)
        {
            idxSet[i1] = 1;    
        }
        else if(a1 > (1/(nu * ((double)pointsInClass))) - eps)
        {
            idxSet[i1] = 3;    
        }
        else
        {
            idxSet[i1] = 0;
        }

        if(a2 < eps)
        {    
            idxSet[i2] = 1;    
        }
        else if(a2 > (1/(nu * ((double)pointsInClass))) - eps)
        {
            idxSet[i2] = 3;    
        }
        else
        {
            idxSet[i2] = 0;
        }

        /* NB. There is a different intercept for each class!  Is this the proper
        * way to find the intercept for this class, or should all values of fcache
        * with idxSet = 0 be considered, regardless of their class?
        */
        if(fCache[i1] > fCache[i2])
        {
            thisBLow = fCache[i1];
            thisILow = i1;
            thisBUp = fCache[i2];
            thisIUp = i2;
        }
        else
        {
            thisBLow = fCache[i2];
            thisILow = i2;
            thisBUp = fCache[i1];
            thisIUp = i1;
        }

        for(k = 0; k < numPoints; k++)
        {
            if((idxSet[k] == 0) && (classes[k] == thisClass))
            {
                if(fCache[k] > thisBLow)
                {
                    thisBLow = fCache[k];
                    thisILow = k;
                }

                if(fCache[k] < thisBUp)
                {
                    thisBUp = fCache[k];
                    thisIUp = k;
                }
            }
        }

        bLow[thisClass] = thisBLow;
        iLow[thisClass] = thisILow;
        bUp[thisClass] = thisBUp;
        iUp[thisClass] = thisIUp;

        return(1);

    }

    /* ExamineExampleNuMultiCache
    *
    *   Subroutine for the nu-type SMO algorithm.  This
    * formulation of multi-categorical nu-type SVM is presented in "Support
    * Vector Representation of Multi-Categorical Data", Silvio Borer and
    * Wulfram Gerstner, Laboratory of Computational Neuroscience, EPFL, CH-1015
    * Lausanne, Switzerland.  This uses the SMO modifications
    * introduced by Keerthi, et. al. (q.v. "Improvements to Platt's SMO
    * Algorithm for SVM Classifier Design", Technical Report CD-99-14, Control
    * Division, Dept. of Mechanical and Production Engineering, National
    * University of Singapore).  Calculations for the derivation appear in TII
    * notebook 007.
    *   This version caches the kernel matrix, and is
    * thus faster for small (<500?) numbers of training points, but
    * inapplicable for larger sets.
    *
    * [success, lagMults, fCache, idxSet, bLow, bUp, iLow, iUp] = ...
    *     ExamineExampleNuMultiCache(i2, classes, lagMults, fCache, idxSet, kernMx,...
    *     bLow, bUp, iLow, iUp, nu, tol)
    * i2                - index of the Lagrange multiplier to vary for
    *                     minimization of the cost function
    * classes           - the class membership for each data point (and
    *                     corresponding Lagrange multiplier
    * lagMults          - n x 1 array of Lagrange multipliers found by minimizing the
    *                     quadratic cost function
    * fCache            - cache of error terms for the non-bound Lagrange
    *                     mulitpliers
    * kernMx            - n x n symmetric kernel array.  Elements (i,j) and (j,i)
    *                     are the value of the kernel evaluated for data points i
    *                     and j.
    * idxSet            - n x 1 array giving the set membership for each
    *                     Lagrange multiplier (bound low, non-bound, bound
    *                     high)
    * bLowAll           - lower bound for the intercept for each class
    * bUpAll            - upper bound for the intercept for each class
    * iLowAll           - index of the Lagrange multiplier corresponding to
    *                     bLow
    * iUpAll            - index of the Lagrange multiplier corresponding to bUp
    * nu                - fraction of the distribution to exclude - called
    *                     "outlierFraction" in much of the rest of the SVM code
    * tol               - tolerance for the Sequential Minimal Optimization
    *                     (SMO) routine
    * success           - set to 1 if the algorithm successfully updated the
    *                     multiplier
    */


    int ExamineExampleNuMultiCache(int i2, int *classes, double *lagMults, double **kernMx,
        double *fCache, int *idxSet, double *bLow, double *bUp, int *iLow, int *iUp, 
        double nu, int numPoints, double tol)

    {    

        double  alpha2, f2;
        int     thisClass, k, optimality, i1;

        alpha2 = lagMults[i2];
        thisClass = classes[i2];

        if(idxSet[i2] == 0)
        {
            f2 = fCache[i2];
        }
        else
        {

            /* Find one column of the kernel matrix. */
            f2 = 0;
            for(k = 0; k < numPoints; k++)
            {
                f2 += lagMults[k] * kernMx[k][i2];
            }
            fCache[i2] = f2;

            /* Update (bLow, iLow) or (bUp, iUp) using (f2, i2). */
            if( (idxSet[i2] == 1) && (f2 < bUp[thisClass]) )
            {
                bUp[thisClass] = f2;
                iUp[thisClass] = i2;
            }
            else if( (idxSet[i2] == 3) && (f2 > bLow[thisClass]) )
            {
                bLow[thisClass] = f2;
                iLow[thisClass] = i2;
            }

        }

        /* Check optimality using current bLow and bUp, and if violated, find an
        * index i1 to do joint optimization with i2...
        */
        optimality = 1;
        if( idxSet[i2] == 0 || idxSet[i2] == 1 )
        {
            if( bLow[thisClass] - f2 > 2.0 * tol )
            {
                optimality = 0;
                i1 = iLow[thisClass];
            }
        }

        if( idxSet[i2] == 0 || idxSet[i2] == 3 )
        {
            if( f2 - bUp[thisClass] > 2.0 * tol )
            {
                optimality = 0;
                i1 = iUp[thisClass];
            }
        }

        if(optimality == 1)
        {
            return(0);
        }

        /* For i2 in I0, choose the better i1... */
        if(idxSet[i2] == 0)
        {    
            if( bLow[thisClass] - f2 > f2 - bUp[thisClass] )
            {
                i1 = iLow[thisClass];
            }
            else
            {
                i1 = iUp[thisClass];
            }
        }

        return(TakeStepNuMultiCache(i1, i2, classes, lagMults, kernMx, fCache,
            idxSet, bLow, bUp, iLow, iUp, nu, numPoints));

    }




    /* SMONuMultiCache
    *
    *   Solve the quadratic optimization problem for a multi-category nu-type
    * SVM using the Sequential Minimal Optimization routine.  This
    * formulation of multi-categorical nu-type SVM is presented in "Support
    * Vector Representation of Multi-Categorical Data", Silvio Borer and
    * Wulfram Gerstner, Laboratory of Computational Neuroscience, EPFL, CH-1015
    * Lausanne, Switzerland.  This uses the SMO modifications
    * introduced by Keerthi, et. al. (q.v. "Improvements to Platt's SMO
    * Algorithm for SVM Classifier Design", Technical Report CD-99-14, Control
    * Division, Dept. of Mechanical and Production Engineering, National
    * University of Singapore).  Calculations for the derivation appear in TII
    * notebook 007.
    *   This version caches the kernel matrix, and is
    * thus faster for small (<500?) numbers of training points, but
    * inapplicable for larger sets. 
    *
    * lagMults = SMONuMultiCache(kernMx, classes, nu, mixingCoeff)
    * kernMx            - n x n symmetric kernel array.  Elements (i,j) and (j,i)
    *                     are the value of the kernel evaluated for data points i
    *                     and j.
    * classes           - n x 1 array of class membership values for each data
    *                     point
    * nu                - fraction of the distribution to exclude - called
    *                     "outlierFraction" in much of the rest of the SVM code
    * mixingCoeff       - scalar between 0 and 1, inclusive, that determines
    *                     the degree of inter-class influence
    * lagMults          - n x 1 array of Lagrange multipliers found by minimizing the
    *                     quadratic cost function
    */


    void SMONuMultiCache(double **kernMx, int *classes, double nu, double mixingCoeff,
        double tol, int numPoints, int numClasses, double *lagMults)
    {
        int     j, k, *idxSet, thisClass, *notYetAssigned, numChanged;
        int    *iUp, *iLow, examineAll, examineSuccess, *pointsPerClass;
        double *fCache, thisFCache, *bUp, *bLow; 

        /* Ok, one of the points of the SMO algorithm is to eliminate the use of a
        * pre-computed kernel matrix, but for small training sets, it will run much
        * faster if the matrix is pre-computed and passed to the subroutines.
        */

        /* In order to bring the notation in line with most of the common texts, we
        * solve the quadratic optimization problem for which the Lagrange
        * multipliers sum to 1, not nu, and for which the variables lie between 0
        * and 1/(nu * pointsInClass), not 1/pointsInClass.  The solution then differs by a
        * factor of 1/nu, which we correct at the end of the routine.
        */

        pointsPerClass = (int *)malloc(numClasses * sizeof(int));

        for(k = 0; k < numClasses; k++)
        {
            pointsPerClass[k] = 0;
        }

        for(k = 0; k < numPoints; k++)
        {
            pointsPerClass[classes[k]] += 1;
        }


        /* Initialize.  This is probably not the best way to initialize fCache,
        * since the kernel matrix is completely calculated.  NB. The choice of
        * initial point seems to be very important.  The original intialization,
        * shown commented below, would not find the correct answer, but rather an
        * answer similar to the correct one, in which the Lagrange multipliers for
        * each class were multiplied by the fraction of the total points that
        * were members of the class.  Note that the initialization violated the
        * constraints; this may be the source of the problem.
        */
        fCache = (double *)malloc(numPoints * sizeof(double));
        idxSet = (int *)malloc(numPoints * sizeof(int));
        for(k = 0; k < numPoints; k++)
        {
            /*lagMults[k] = (1.0/((double)numPoints));*/
            lagMults[k] = 1.0/((double)pointsPerClass[classes[k]]);
            idxSet[k] = 0;
            fCache[k] = 0.0;
        }

        for(k = 0; k < numPoints; k++)
        {
            for(j = 0; j < numPoints; j++)
            {
                fCache[k] += kernMx[k][j] * lagMults[j];
            }
        }

        /* Find the minimum of fCache for each class. */
        bUp = (double *)malloc(numClasses * sizeof(double));
        bLow = (double *)malloc(numClasses * sizeof(double));
        iUp = (int *)malloc(numClasses * sizeof(int));
        iLow = (int *)malloc(numClasses * sizeof(int));
        notYetAssigned = (int *)malloc(numClasses * sizeof(int));

        for(k = 0; k < numClasses; k++)
        {
            notYetAssigned[k] = 1;
        }

        for(k = 0; k < numPoints; k++)
        {
            thisFCache = fCache[k];
            thisClass = classes[k];

            if(notYetAssigned[thisClass])
            {
                bUp[thisClass] = thisFCache;
                iUp[thisClass] = k;
                bLow[thisClass] = thisFCache;
                iLow[thisClass] = k;
                notYetAssigned[thisClass] = 0;
            }
            else
            {
                if(thisFCache < bUp[thisClass] && (idxSet[k] == 0 || idxSet[k] == 1))
                {
                    bUp[thisClass] = thisFCache;
                    iUp[thisClass] = k;
                }

                if(thisFCache > bUp[thisClass] && (idxSet[k] == 0 || idxSet[k] == 3))
                {
                    bLow[thisClass] = thisFCache;
                    iLow[thisClass] = k;
                }

            }

        }
        std::free(notYetAssigned);

        /*  Now begin the Sequential Minimal Optimization. */
        numChanged = 0;
        examineAll = 1;

        while( numChanged > 0 || examineAll == 1 )
        {    
            numChanged = 0;
            if(examineAll == 1)
            {    
                for(k = 0; k < numPoints; k++)
                {
                    examineSuccess = ExamineExampleNuMultiCache(k, classes, lagMults, kernMx, 
                        fCache, idxSet, bLow, bUp, iLow, iUp, nu, numPoints, tol);

                    numChanged = numChanged + examineSuccess;
                }
            }    
            else
            {    
                for(k = 0; k < numPoints; k++)
                {    
                    if(idxSet[k] == 0)
                    {
                        examineSuccess = ExamineExampleNuMultiCache(k, classes, lagMults, kernMx, 
                            fCache, idxSet, bLow, bUp, iLow, iUp, nu, numPoints, tol);

                        numChanged = numChanged + examineSuccess;
                    }

                    /* At this point, do we need to check for optimality? */
                    if( bUp[classes[k]] > bLow[classes[k]] - 2.0 * tol )
                    {
                        break;
                    }

                }
                numChanged = 0;

            }

            if(examineAll == 1)
            {
                examineAll = 0;
            }
            else if(numChanged == 0)
            {
                examineAll = 1;
            }

        }


        std::free(fCache);
        std::free(idxSet);
        std::free(bUp);
        std::free(bLow);
        std::free(iUp);
        std::free(iLow);

        return;
    }





};

//bbc revisit the _supportClasses are 1 indexed since they originated from Matlab
//Once the training algorithm is implemented in kl change the indexing the the classification
//algorithm to accomodate the change to 0 based indexing.
template <class TYPE> class klMulticlassSVMClassify : public klSamplePopulation<TYPE>
{
    //Class members - set in the constructor
    unsigned int _numClasses;
    double	_mixingCoeff; 
    klMatrix<TYPE> _supportPoints;
    klVector<int> _supportClasses;
    klVector<TYPE> _lagMults;
    klVector<TYPE> _intercepts;
    unsigned int _numDims;

    //The width of RBF kernel.
    unsigned int _numPoints;//Number of points to classify.
    double _sigma;

public:	
    TYPE RadialBasisFunctionKernel(TYPE* x, TYPE* y) 
    {
        unsigned int     n;
        TYPE  sum, d;

        sum = 0;
        for(n = 0; n < _numDims; n++)
        {
            d = x[n] - y[n];
            sum += d * d;
        }

        return(std::exp(-   sum /(2.0 * _sigma * _sigma)) );  
    }


public:

    klMulticlassSVMClassify(unsigned int numPoints,unsigned int numFeatures,klMatrix<TYPE> supportPoints, klVector<int> supportClasses, klVector<TYPE> lagMults, klVector<TYPE> intercepts, unsigned int numClasses,double mixingCoeff,double sigma )
        : klSamplePopulation<TYPE>(numPoints,numFeatures),
        _supportPoints(supportPoints),_supportClasses(supportClasses),_lagMults(lagMults),_intercepts(intercepts),_sigma(sigma)
    {
        //bbcrevisit - the training points, classes, lag mults and intercepts probably don't need to be copied.
        _numClasses=numClasses;
        _mixingCoeff=mixingCoeff;
        _numDims=getColumns();
        _numPoints=numPoints;
    }

    klMatrix<TYPE> operator()()
    {
        if(getColumns() != _supportPoints.getColumns() )
            throw "klMulticlassSVMClassify ERROR: incompatible dimensions in data and supportPoints.";

        if(_supportClasses.getRows() != _supportPoints.getRows())
            throw "klMulticlassSVMClassify ERROR: incompatible dimensions in supportClasses and supportPoints.";

        if(_lagMults.getRows() != _supportPoints.getRows())
            throw "klMulticlassSVMClassify ERROR: incompatible dimensions in lagMults and supportPoints.";

        if(_intercepts.getColumns() != _numClasses)
            throw "klMulticlassSVMClassify ERROR: incompatible dimensions in intercepts.";

        //This is the matrix representing the classification results
        klMatrix<TYPE> testClasses(getRows(), _numClasses);  

        unsigned int numsupportPoints= _supportPoints.getRows();

        unsigned int numTestPoints= getRows();

        unsigned int numDims= _supportPoints.getColumns();

        _mixingCoeff /= (_numClasses - 1);

        for(unsigned i=0;i<numTestPoints;i++)
        {
            for(unsigned j=0;j<_numClasses;j++)
            {
                testClasses[i][j]=0.0;
            }
        }

        unsigned int     k, m, n;

        double  kernelElement, classMx;


#ifdef MCSVM_DEBUG
        //Test variables -
        klVector<TYPE> kernelEvals(numsupportPoints);
        klMatrix<TYPE> classWeightingMatrix(numsupportPoints,_numClasses);
        for(m=0;m<numsupportPoints;m++)
        {
            for(n=0;n<_numClasses;n++)
            {
                if(  (_supportClasses[m]) != n)//(_supportClasses[m]-1) because Matlab used 1 based indexing 
                {
                    classMx = - _mixingCoeff;
                }
                else
                {
                    classMx = 1.0;
                }
                classWeightingMatrix[m][n]=_lagMults[m]*classMx;
            }
        }
#endif




        for(k = 0; k < numTestPoints; k++)
        {
            for(m = 0; m < numsupportPoints; m++)
            {
                //cout<<_supportPoints[m];
                //row k from data and training point m
                kernelElement = RadialBasisFunctionKernel( operator[](k).getMemory(), _supportPoints[m].getMemory() );
#ifdef MCSVM_DEBUG
                kernelEvals[m]=kernelElement;
#endif

                for(n = 0; n < _numClasses; n++){

                    if(  (_supportClasses[m]) != n)//(_supportClasses[m]-1) because Matlab used 1 based indexing 
                    {
                        classMx = - _mixingCoeff;
                    }
                    else
                    {
                        classMx = 1.0;
                    }
                    testClasses[k][n] +=_lagMults[m] * classMx * kernelElement;
                    //cout<<_lagMults[m]<<" "<<classMx<< " "<<kernelElement<<endl;
                }        
            }

            //classVals = kM' * y - interceptMx;

            for(n = 0; n < _numClasses; n++)
            {
                testClasses[k][n]-= _intercepts[n];
                //cout<<_intercepts[n];
            }

#ifdef MCSVM_DEBUG
            {
                char* arg=new char[256];
                sprintf(arg,"%s//testKernelElements.txt",basefilename);
                ofstream fileostreamobj(arg );
                fileostreamobj<<kernelEvals<<endl;
                fileostreamobj.close();

                sprintf(arg,"%s//testClassWeightingMatrixClassify.txt",basefilename);
                fileostreamobj.open(arg );
                fileostreamobj<<classWeightingMatrix<<endl;
                fileostreamobj.close();

                sprintf(arg,"%s//testClassesPost_Mx.txt",basefilename);
                fileostreamobj.open(arg );
                fileostreamobj<<testClasses<<endl;
                fileostreamobj.close();
            }
#endif
        }

        return testClasses;

    }


private:
};



#endif //__kl_multiclass_svm__








