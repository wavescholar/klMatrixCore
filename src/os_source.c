/*
OSCILLATING SEARCH feature selection method source code - implemented in C
--------------------------------------------------------------------------------
(taken from the Feature Selection Toolbox package, being developed at
      Institute of Information Theory and Automation, Prague, Czech Republic.
 Implemented by Petr Somol to contain several algorithm variations in one procedure)

- this procedure implements a subset search strategy presented in 
  "Oscillating Search Algorithms For Feature Selection"
  paper for the ICPR 2000 Conference in Barcelona

- the code is not further developed in this form and should be considered experimental
  
- the procedure as implemented here seems somewhat complex. This is not due to the 
  core algorithm principle but beacause of the many included generalized step variations:
  a) "sequential" steps, b) "exhaustive" steps, and/or even c) "floating" steps.
  This technically means, that the procedure as a whole implements not only OS
  itself but also SFFS/SBFS as its sub-procedure

- the code is written basically in ANSI C, the only C++ specific points relate
  to user-interface integration and are commented out or marked as unnecessary
  
- to use the code you will probably have to rewrite/modify the following parts:
  * criterion procedure "Criterion()" calls should be replaced by your own
    criterion function, possibly even with modified parameter passing. Three 
    "Criterion()" calls should be replaced in 3 different places in the code.
  * functions and types "time()", "ctime()", "time_t", "difftime()" serve for 
    measuring computational time only and are not needed for the algorithm to work 
    properly unless you want to restrict computational time using 'maxseconds'
  * information about algorithm state (percentages, current dimension etc.) 
    is passed using "ProcessText" variable. Relating workarounds come from Feature
    Selection Toolbox and are unnecessary (and commented out) here. 
    This includes "GetStopFlag()", "SetProcessFlag()" procedures and "ProcessTextCS"
    semaphore. In F.S.T. the procedure runs as a thread, while another low-priority 
    thread keeps the user informed about current procedure state.
    

PARAMETERS and CONSTANTS:  (local variables are described inside the procedure):
--------------------------------------------------------------------------------

  * int n - full feature set size
  * int d - desired feature subset size
  
  Parameters deltasq, deltafl and deltaex denote the oscillation cycle behavior.
  - deltasq limits the sequential cycle depth,
  - deltafl limits the floating cycle depth, 
  - deltaex limits the exhaustive cycle depth. 
  The sequential OS(x) is available via setting (deltasq=x,deltafl=0,deltaex=0),
  the floating OS(x) via (deltasq=0,deltafl=x,deltaex=0), 
  the full exhaustive OS(x) via (deltasq=0,deltafl=0,deltaex=x). 
  If more deltas are set (i.e. deltasq=x,deltafl=y, deltaex=z), than all appropriate
  cycle steps would be called subsequently one after another. That means: 
  the algorithm starts sequentially as OS(x), then continues as floating OS(y)
  and finishes as exhaustive OS(z). Whenever the algorithm finds a subset better 
  than the current one, it resets to start again in its simplest form as specified
  using delta?? parameters (sequential, then floating, then exhaustive).
  At least one delta value must be set >0, or the procedure call makes no sense.

  * int deltasq - oscillation depth in case of sequential oscillation steps
       0 - disable sequential steps
       deltasq>0 - sequential oscillation is limited to <d-deltasq,d+deltasq>.
  * int deltafl - oscillation depth in case of floating oscillation steps
       0 - disable floating steps
       deltafl>0 - floating oscillation is limited to <d-deltafl,d+deltafl>.
  * int deltaex - oscillation depth in case of exhaustive oscillation steps
       0 - disable exhaustive steps
       deltaex>0 - exhaustive oscillation is limited to <d-deltaex,d+deltaex>.
  * int init - initialization method to construct the initial subset
       0 - random (note: srand() is currently commented out. uncomment it if needed.)
       1 - by means of SFS procedure (deterministic, relatively fast)
       2 - by means of SFFS (deterministic, slow, guarantees equal or better 
                             result than that of SFFS, but may cause the OS to
                             get stuck in SFFS-like local extremes)
       3 - by setting an initial subset (must be stored in TSubset *initset)
  * TSubset *initset - structure to pass an initial subset, may be omitted if 'init!=3'
  * int strict - oscillation cycle result testing behavior (affects speed)
       0 - every cycle will be completed - this results in slower, but possibly 
           more thorough search
       1 - cycle may be stopped, if its first half has led to no improvement - this 
           results in a bit faster search
  * TSubset *bset - structure for passing final results (see TSubset definition later)
  * int detail - textual output detail level (use NOTHING=0 or STANDARD=2)

  Following parameters should be considered experimental:
  * long maxseconds - recommended default 0; otherwise it limits computational time (in seconds)
  * int floatdelta - recommended default -3 (gives preference to speed over thoroughness); 
                         in case of floating oscillation cycles floatdelta limits the 
                         extent of search in the floating subprocedure:
					0 - full search (the subprocedure will not stop after reaching 
					    dimension d; SFFS will reach n, SFBS will reach 0)
					delta>0 - SFFS will float until dimension d+delta is reached, 
					          SFBS will float until d-delta is reached
					-1 - actual delta is computed during the course of the 
					     algorithm by averaging the depth of backtracking
					-2 - actual delta is set during the course of the algorithm 
					     as the maximum depth of backtracking
					-3 - actual delta is set to d+-2x(current generalization level)
  * int autodelta - recommended default 0; if 1, then deltasq, deltafl and deltaex values 
                    are not considered to be absolute, but relative (0-100% of set size) 

RECOMMENDATION
--------------------------------------------------------------------------------
- for recommendations on how to set procedure parameters please 
  consult the accompanying documents on our web and the respective papers.
  Our experience shows, that simpler procedure calls (sequential form only)
  seem preferable due to better speed/quality-of-result ratio.
  If you need to get results in one run, choose one of deterministic initializations.
  If you can afford multiple runs, then try to call the OS repeatedly with
  random initialization. This very often leads to better results, outperforming
  other feature selection procedures.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SGSEQUENTIAL 1
#define SGFLOATING 2
#define SGFULL 3
/* how often should the informational percentage be estimated */
#define PERCENTDETAIL  5
#define SECONDS  2

/* output types */
#define NOTHING 0
#define PERCENT 1
#define STANDARD 2

#define DELTAMUL 1.0            /* a multiplier of estimated delta (in floating search sub-step)*/
#define DELTAADD 1              /* a constant to be added to estimated delta (in floating search sub-step)*/
#define SAFEUP   5e300          /* double overflow limit */
#define SAFEDOWN 5e-300         /* double underflow limit */
#define SAFEXP   1000           /* double exponent limit */

#define FREE(x)       { free( x ); x = NULL; }
#define printint(x)   { printf("%d",x); }
#define printtext(x)  { printf("%s",x); }
#define true   1
#define false  0

typedef struct Subset{ int subsetsize; int *featureset; double critvalue; unsigned long enumcount; double comp_time;} TSubset;
/* subsetsize - subset size
   featureset - indexes of selected features (beginning at 0)
   critvalue - criterion value corresponding to the selected subset
   enumcount - number of performed Criterion() calls
   comp_time - length of computation in seconds
Note: the TSubset structure is used for:
a) result storage - it is filled once at the end of the oscillation search algorithm.
b) initialization (optional) - subsetsize and featureset to be pre-specified by user
*/




/************************* HERE STARTS THE CODE *******************************/
int OscillatingSearch(int n, int d, int deltasq, int deltafl, int deltaex, int floatdelta, int autodelta, int strict, int init, TSubset *initset, int maxseconds, TSubset *bset, int detail)
{    
  /* a non-redundand coding of subset configurations is used.
     For easier understanding imagine our goal is to find a subset by exhaustive search,
     when d=5 and n=12
    - initial configuration is 111110000000   (actually stored in "bin" field)
    - in every step: a) find the leftmost block of 1's
                     b) shift its rightmost 1 one position to the right
                     c) shift the rest of this block to the left boundary (if it is not there)
    - this algorithm generates sequentially binary representations of all subsets
    - identifiers different than 0 or 1 are used to identify temporarily fixed features etc. (2,-1)
    - by means of swapping the meanings of 0 and 1 the code implements
      both forward and backward search sub-steps at once
  */

  int *bin=NULL;         /* of size n, stores 0/1 information on currently selected features */
  int *index=NULL;       /* of size d, it is computed from bin, stores indexes of selected features */
  int *bestset=NULL; double bestvalue; /* best subset */
  int *globalbestset=NULL; double *globalbestvalue=NULL; /* pro nejlepsi subsety vsech rozmeru */

  /* all identifiers with SG prefix are used for computation of three kinds of 
     generalized steps and should be considered experimental*/

  int *SG_fixbin=NULL;   /* of size n, stores 0/1 information on features selected before calling SG */
  int *SG_globalbestset=NULL; double *SG_globalbestvalue=NULL; /* so-far best subsets in all dimensions */
  int SG_wasthebest;     /* indicates, if a better subset has been found during last step*/
  int SG_sumr=0;         /* current subset size */
  int SG_zmenasumr;      /* how to change sumr: -2,-1,+1,+2 steps */
  int SG_rr;
  int SG_stopfloat;      /* identifies end of generalized step */
  int SG_stopfloatmez;   /* identifies the dimension, for which the algorithm should stop */
  int SG_sbs=0;          /* 0=sfs, 1=sbs current search direction*/
  int SG_vykyv=0;        /* stores current backtracking depth (+forward,-backward)*/
  float SG_vykyvmez=0.0; /* predicted delta estimate */
  int SG_vykyvcount=0;   /* number of direction changes (needed for delta averaging)*/
  int SG_sfbs=0;         /* 0=sffs, 1=sfbs main search direction while in generalized step */
  
  int SG_r = 1;          /* default size of internal floating step*/
  int SG_n;
  int SG_fixsize;        /* size of fixovane part*/
  int SG_fixsumr;        /* like SG_sumr but takes fixed features into account*/
  int SG_floatdelta = -3; /* delta for internal floating (0-nodelta, >0 absolute, -1 avg, -2 max, -3 2x*/
  int SG_d;
  int SG_type = SGSEQUENTIAL;  /* identifier of current generalization type, 1-sequential, 2-floated (semi-generalized), 3-full generalization */
  int SG_TYPE = SGSEQUENTIAL;  /* logical procedure type (i.e. for the floating type set in SG_TYPE the SG_type value may be sequential if rr=1 ...)*/
  char SG_fdir;            /* variable for informational output only*/
  int SG_accelerate = 0;   /* acceleration possible ? (by using criterion values stored previously in memory)*/
  long SG_accelerated = 0; /* informational counter*/

  int delta;

  int firstloop=1;

  int maxdelta;          /* temporary storage for max{deltasq,deltafl,deltaex}*/
  int kmitpart;
  int wasthebest;        /* indicates, if a better subset has been found during last step*/
  double value;
  int sumrint;
  int i;
  int sumr;              /* current subset size */
  int rr;
  int max_rr_advantage=0; /* records the maximum cycle depth in which a better solution has been found*/
  int pom,initpokus;
  
  int beg,piv;           /* beg - beginning of block of identifiers, piv - "pivot" - last identifier in a block */
  int stopex;            /* identifies end of internal exhaustive search */
  int stopfloat;         /* identifies end of it all */
  int id0=0,id2=2;       /* identifiers (0 and 2) are exchanged in case of backward search */
  int vykyv=0;           /* stores current backtracking depth (+forward,-backward)*/
  int error=0;
  int rrchange=0;        /* after a non-improving cycle the rr was increased */
  int repeat=0;          /* after breaking an ocsillation cycle the next one should be started. this is done within the swing-property "switch" */  
  int globcit,ecit=0;
  long kombcit;
  int timcet=false;
  time_t tbegin,telp,tact,tlast;   /* for measuring computational time only */
  tbegin=time(NULL);
  tlast=tbegin;

  error=0;
  if(autodelta)
  {
      pom=n-d; if(pom<d) pom=d;
      if(deltasq>100) deltasq=100; else if(deltasq<0) deltasq=0;
      i=((long)deltasq*(long)pom)/100L;
      if((i==0)&&(deltasq>0)) deltasq=1; else deltasq=i;
      if(deltafl>100) deltafl=100; else if(deltafl<0) deltafl=0;
      i=((long)deltafl*(long)pom)/100L;
      if((i==0)&&(deltafl>0)) deltafl=1; else deltafl=i;
      if(deltaex>100) deltaex=100; else if(deltaex<0) deltaex=0;
      i=((long)deltaex*(long)pom)/100L;
      if((i==0)&&(deltaex>0)) deltaex=1; else deltaex=i;
  }

  if((d<1)||(d>n)){return(24);} /* nothing to search for */
  pom=n-d-1; if(d-1>pom)pom=d-1; /* (n-d-1) instead of (n-d). reaching n has no sense */
  if(((deltasq>pom)||(deltasq<1))&&((deltafl>pom)||(deltafl<1))&&((deltaex>pom)||(deltaex<1))) {return(29);} /* any delta may be higher than one limit only (upper or lower) */
  if(n<=2){return(30);}

  if((bin=(int *)malloc((long)(n+1)*(long)sizeof(int)))==NULL)
            {return(3);}
  if((index=(int *)malloc((long)n*(long)sizeof(int)))==NULL)
            {FREE(bin) return(3);}
  if((bestset=(int *)malloc((long)n*(long)sizeof(int)))==NULL)
            {FREE(index) 
             FREE(bin) return(3);}
  if((globalbestset=(int *)malloc((((long)n*((long)n+1L))/2L)*(long)sizeof(int)))==NULL)
            {FREE(bestset) 
             FREE(index) 
             FREE(bin) return(3);}
  if((globalbestvalue=(double *)malloc((long)n*(long)sizeof(double)))==NULL)
            {FREE(globalbestset) 
             FREE(bestset) 
             FREE(index) 
             FREE(bin) return(3);}
  if((SG_fixbin=(int *)malloc((long)(n+1)*(long)sizeof(int)))==NULL)
            {FREE(globalbestvalue) 
             FREE(globalbestset) 
             FREE(bestset) 
             FREE(index) 
             FREE(bin) return(3);}
  if((SG_globalbestset=(int *)malloc((((long)n*((long)n+1L))/2L)*(long)sizeof(int)))==NULL)
            {FREE(SG_fixbin) 
             FREE(globalbestvalue) 
             FREE(globalbestset) 
             FREE(bestset) 
             FREE(index) 
             FREE(bin) return(3);}
  if((SG_globalbestvalue=(double *)malloc((long)n*(long)sizeof(double)))==NULL)
            {FREE(SG_globalbestset) 
             FREE(SG_fixbin) 
             FREE(globalbestvalue) 
             FREE(globalbestset) 
             FREE(bestset) 
             FREE(index) 
             FREE(bin) return(3);}
             
  /* k-th set of size k is stored at [(k*(k-1))/2] */
  for(i=0;i<(n*(n+1))/2;i++) globalbestset[i]=0;
  for(i=0;i<n;i++) globalbestvalue[i]=-SAFEUP;

  for(i=0;i<=n;i++) bin[i]=id0; /* bin[n]=id0 for testing the end */

  rr=1; /* initial cycle depth */
  max_rr_advantage=0;
  kmitpart=0;
  sumr=1; 
  if(detail&STANDARD){printtext("Oscillating Search++ (delta "); printint(deltasq); printtext(",");
                                  printint(deltafl); printtext(","); printint(deltaex); printtext(" init: ");
                                  switch(init){case 0: printtext("random"); break; case 1: printtext("sfs"); break; case 2: printtext("sffs"); break;
                                  case 3: { printtext("set "); for(i=0;i<initset->subsetsize;i++) {printint(initset->featureset[i]); printtext(" ");}} break;}
                                  if(maxseconds>0) printtext(", time limit");
                                  printtext("):\n");}
  if(detail&STANDARD) {printtext(", started on "); printtext(ctime(&tbegin));}

  maxdelta=d+deltasq;
  if(d+deltafl>maxdelta) maxdelta=d+deltafl;
  if(d+deltaex>maxdelta) maxdelta=d+deltaex;
  
  stopfloat=0;
  
  if(maxdelta>n) maxdelta=n;
  if((d==1)&&(init==0)) init=1; /* (for d==1 would not be possible to call vykyv=1 in random)*/
  if (init==1) /* SFS initialization*/
  {
      vykyv=0; 
      rr=maxdelta; sumr=maxdelta;   /* original interval*/
      /*rr=d; sumr=d;*/
      SG_type=SGSEQUENTIAL;
  }
  else if(init==2) /* SFFS initialization*/
  {
      vykyv=0; 
      rr=maxdelta;
      sumr=maxdelta;
      SG_floatdelta=0;
      SG_type=SGFLOATING;
  }
  else if(init==3) /* initialization via initial subset*/
  {
    memcpy(&globalbestset[(d*(d-1))/2],initset->featureset,d*sizeof(int));

    /* result of criterion function should be stored to "globalbestvalue[d-1]". When calling the criterion function, "&globalbestset[(d*(d-1))/2]" field
       contains indexes (beginning by 0) of features in the subset being currently tested, having dimension "d"*/
    if((error=Criterion(globalbestvalue[d-1],&globalbestset[(d*(d-1))/2],d))!=0) goto AfterError;
    ecit++;

    vykyv=0;
    goto AfterInit;
  }
  else /* init==0, random initialization*/
  {
      /*if(opt_randomize) srand(tbegin%32200);*/
      initpokus=0;
      do
      {
        for(i=0;i<=n;i++) bin[i]=-1;
        for(i=0;i<d;i++)
        { 
            piv=((long)rand()*(long)(n-1))/RAND_MAX;
            while(bin[piv]!=-1) {piv++; if(piv>n-1) piv=0;}
            bin[piv]=2;
        }
    
        pom=(d*(d-1))/2;
        piv=0;
        for(i=0;i<d;i++)
        {
          while(bin[piv]<=0) piv++;
          globalbestset[pom+i]=piv;
          piv++;
        }
    
        /* result of criterion function should be stored to "globalbestvalue[d-1]". When calling the criterion function, "&globalbestset[pom]" field
           contains indexes (beginning by 0) of features in the subset being currently tested, having dimension "d"*/
        if((error=Criterion(globalbestvalue[d-1],&globalbestset[pom],d))!=0) goto AfterError;
        ecit++;
    
        if(globalbestvalue[d-1]>-SAFEUP) {vykyv=0; goto AfterInit;}
        
      } while(++initpokus<15); /* more attempts for case of problematic data, where certain subsets may yield unspecified criterion values*/
  }
  
  /* main cycle */
  do
  {
      
    /* rr-tuple is being searched for since this moment
       :bin  for forward direction   -1 for unselected, id0=2 for those selected, who may become unselected
       :bin  for backward direction   1 for selected,   id0=0 for those not selected, who may become selected
       sumr  desired dimension
       rr     difference from the last dimension
       vykyv keeps information on direction (down 1,4  up 0,2,3)*/
    
      if(/*(vykyv==0)||*/(SG_type==SGFULL)) /* full generalization*/
      { /* necessary steps see later*/
      }
      else if(SG_type==SGSEQUENTIAL) /* sequential generalization*/
      { /* necessary steps see later*/
          SG_rr=1;
          if((vykyv==1)||(vykyv==4))
          { /* SBS */
              SG_sbs=1; id0=2; id2=0;
              if(SG_accelerate==SGSEQUENTIAL) 
                      {SG_rr=rr; SG_fixsumr=sumr;}
              else
                      SG_fixsumr = sumr+rr-1;
          }
          else
          { /* SFS */
              SG_sbs=0; id0=0; id2=2;
              if(SG_accelerate==SGSEQUENTIAL) 
                      {SG_rr=rr; SG_fixsumr=sumr;}
              else
                      SG_fixsumr = sumr-rr+1;
          }
      }
      else if(SG_type==SGFLOATING) /* SG_type==2 floating generalization*/
      {
          for(i=0;i<=n;i++) SG_fixbin[i]=bin[i];
          SG_rr=SG_r; 
          if((vykyv==1)||(vykyv==4))
          { /* SFBS, first run SBS */
            SG_n = sumr+rr;
            SG_fixsize = n - SG_n;
            SG_d = SG_n - rr;
            SG_sfbs=1; SG_sbs=1; id0=2; id2=0; 
            SG_sumr=SG_n-SG_rr;
            if(SG_floatdelta==0) SG_stopfloatmez=1;
            else if((SG_floatdelta>0)||(SG_floatdelta==-3))
            {
                if(SG_floatdelta>0) SG_stopfloatmez=SG_d-SG_floatdelta; 
                else /*SG_floatdelta==-3*/ SG_stopfloatmez=SG_d-rr-DELTAADD;
                if(SG_stopfloatmez<1) SG_stopfloatmez=1;
            }
            /* delta -1 and -2 means setting stopfloatmez later*/
          }
          else
          { /* SFFS, first run SFS */
            SG_fixsize = sumr-rr;
            SG_n = n - SG_fixsize;
            SG_d = rr;
            SG_sfbs=0; SG_sbs=0; id0=0; id2=2;
            SG_sumr=SG_rr;
            if(SG_floatdelta==0) SG_stopfloatmez=SG_n;
            else if((SG_floatdelta>0)||(SG_floatdelta==-3))
            {
                if(SG_floatdelta>0) SG_stopfloatmez=SG_d+SG_floatdelta; 
                else /*SG_floatdelta==-3*/ SG_stopfloatmez=SG_d+rr+DELTAADD;
                if(SG_stopfloatmez>SG_n) SG_stopfloatmez=SG_n;
            }
            /* delta -1 and -2 means setting stopfloatmez later*/
          }
          SG_vykyv=0;
          SG_vykyvmez=0.0;
          SG_vykyvcount=0;
    
          /* tests primarily for debugging purposes*/
          if((SG_d<1)||(SG_d>SG_n)){return(24);} /* otherwise there is nothing to search for */
          if((SG_r<1)||(SG_r>SG_n)||((!SG_sfbs)&&(SG_r>SG_d))||((SG_sfbs)&&(SG_r>SG_n-SG_d))){return(25);} /* otherwise it is impossible */
          
          /* k-th set of k features is stored at [(k*(k-1))/2] */
          for(i=0;i<(n*(n+1))/2;i++) SG_globalbestset[i]=0;
          for(i=0;i<n;i++) SG_globalbestvalue[i]=-SAFEUP;
        
          for(i=0;i<=n;i++) bin[i]=id0; /* bin[n]=id0 to test for end */
      }
      
      SG_stopfloat=0;
      do
      {
        /* ------------------- preparation steps before every internal exhaustive -----------*/
        if(/*(vykyv==0)||*/(SG_type==SGFULL))
        { /* runs once*/
            for(i=0;i<n;i++) bestset[i]=i; bestvalue=-SAFEUP;
            pom=rr;
            for(i=0;((pom>0)&&(i<n));i++) if(bin[i]==id0) {bin[i]=id2; pom--;} /* initialize bin for exhaustive step */
            SG_fixsumr = sumr;
            sumrint=sumr*sizeof(int);
            
            globcit=0;
            /* estimate the number of following steps*/
            kombcit=1;
            if((vykyv==1)||(vykyv==4)) pom=sumr+rr; else pom=n-(sumr-rr); 
            for(i=0;i<rr;i++) kombcit*=pom-i; for(i=2;i<=rr;i++) kombcit/=i;
        }
        else if(SG_type==SGSEQUENTIAL)
        { /* it will run rr-times (except accelerated jump)*/
            for(i=0;i<n;i++) bestset[i]=i; bestvalue=-SAFEUP;
            pom=1;
            for(i=0;((pom>0)&&(i<n));i++) if(bin[i]==id0) {bin[i]=id2; pom--;} /* initialize bin for exhaustive step */
            sumrint=SG_fixsumr*sizeof(int);
    
            globcit=0;
            /* estimate the number of following steps*/
            kombcit=1;
            if((vykyv==1)||(vykyv==4)) pom=SG_fixsumr+1; else pom=n-(SG_fixsumr-1); 
            for(i=0;i<1;i++) kombcit*=pom-i; for(i=2;i<=1;i++) kombcit/=i;
        }
        else if(SG_type==SGFLOATING) /* SG_type==2*/
        {
            for(i=0;i<n;i++) bestset[i]=i; bestvalue=-SAFEUP;
            pom=SG_rr;
            /* forced fixing*/
            if(SG_sfbs) {for(i=0;i<n;i++) if(SG_fixbin[i]<=0) bin[i]=-3;}
                   else {for(i=0;i<n;i++) if(SG_fixbin[i]>0) bin[i]=3;}
            for(i=0;((pom>0)&&(i<n));i++) if(bin[i]==id0) {bin[i]=id2; pom--;} /* initialize bin for exhaustive step */
            if(SG_sfbs) {SG_fixsumr=SG_sumr;}
            else        {SG_fixsumr=SG_sumr+SG_fixsize;}
            sumrint=SG_fixsumr*sizeof(int);

            globcit=0;
            /* estimate the number of following steps*/
            kombcit=1;
            if(SG_sbs) pom=SG_sumr+SG_rr; else pom=SG_n-(SG_sumr-SG_rr); 
            for(i=0;i<SG_rr;i++) kombcit*=pom-i; for(i=2;i<=SG_rr;i++) kombcit/=i;
        }
        /*-----------------------------------------------------------------------------*/
        stopex=0;     /* >>>>>>>>>>>>>>>>>>>>  internal exhaustive step >>>>>>>>>>>>>>>>>>>>*/
        firstloop=1;
        do
        {
          pom=0; /* convert "bin" to "index" */
          for(i=0;i<SG_fixsumr;i++)
          {
            while(bin[pom]<=0) pom++;
            index[i]=pom;
            pom++;
          }
          
          /* (optional) computational time test.. has it exceeded pre-specified limit maxseconds ?*/
          if(!(globcit%PERCENTDETAIL)){
              tact=time(NULL);
              if(difftime(tact,tlast)>SECONDS) {
                  tlast=tact; timcet=true;
                  /* test cancellation
                  if((error=GetStopFlag())!=0)  goto AfterError;*/
                  if((maxseconds>0)&&(difftime(tact,tbegin)>maxseconds)) {stopex=1; SG_stopfloat=1; stopfloat=1;}
              }
          }

          /* ----------- the following block served only for outputting the information about current algorithm state ----------*/
          /*if((!globcit)||timcet)
          {
              pom = (int)floor((100.0*(double)sumr)/(double)n);            
              
              ProcessTextCS->Acquire();
              switch(vykyv){
                  case 0: {ProcessText[0]='i'; ProcessText[1]=' '; break;}
                  case 1: {ProcessText[0]='v'; ProcessText[1]=' '; break;}
                  case 2: {ProcessText[0]='^'; ProcessText[1]=' '; break;}
                  case 3: {ProcessText[0]='^'; ProcessText[1]='^'; break;}
                  case 4: {ProcessText[0]='v'; ProcessText[1]='v'; break;}
              }              
              switch (SG_type) {
                case SGSEQUENTIAL: sprintf(ProcessText+2," k=%d, sq.(k=%d, %d/%ld acc=%d), rr=%d, Cr=%g",sumr,SG_fixsumr,globcit,kombcit,SG_accelerated,rr,globalbestvalue[d-1]); break;
                case SGFLOATING  : if(SG_sbs) SG_fdir='v'; else SG_fdir='^';
                                   sprintf(ProcessText+2," k=%d, fl.(%c k=%d, %d/%ld, acc=%d, delta=%d), rr=%d, Cr=%g",sumr,SG_fdir,SG_fixsumr,globcit,kombcit,SG_accelerated,SG_floatdelta,rr,globalbestvalue[d-1]); break;
                case SGFULL      : sprintf(ProcessText+2," k=%d, ex.(%d/%ld), rr=%d, Cr=%g",sumr,globcit,kombcit,rr,globalbestvalue[d-1]); break;
              }
              ProcessTextCS->Release();
                        
              SetProcessFlag(pom);
              if(detail&PERCENT) {
                  printtext("\r");
                  ProcessTextCS->Acquire();
                  printtext(ProcessText); 
                  ProcessTextCS->Release();
                  printtext(" ");
              }
          }*/
          /* ----------- the previous block served only for outputting the information about current algorithm state ----------*/
          timcet=false;
          globcit++;

     
          /* result of criterion function should be stored to "value". When calling the criterion function, "index" field
             contains indexes (beginning by 0) of features in the subset being currently tested, having dimension "SG_fixsumr"*/
          if((error=Criterion(value,index,SG_fixsumr))!=0) goto AfterError;
          ecit++;
          
          if((value>bestvalue)||firstloop)
          {
            memcpy(bestset,index,sumrint);
            bestvalue=value;
            firstloop=0;
          }
    
          /* finding the new configuration during internal exhaustive step */
          for(beg=0;bin[beg]!=id2;beg++);
          for(piv=beg;(piv<n)&&(bin[piv]!=id0);piv++);
          if(piv==n) stopex=1;
          else
          {
            pom=piv; /* remember the position of first 0 on the right */
            do piv--; while(bin[piv]!=id2); /* find a real pivot */
            bin[piv]=id0; bin[pom]=id2; /* shift pivot to the right */
            pom=0;
            /* run "pom" from left, "piv" from right. the 0,2 pairs found are changed to 2,0 */
            do piv--; while((piv>0)&&(bin[piv]!=id2));
            while((pom<piv)&&(bin[pom]!=id0))pom++;
            while(piv-pom>0)
            {
              bin[piv]=id0; bin[pom]=id2;
              do piv--; while((piv>0)&&(bin[piv]!=id2));
              while((pom<piv)&&(bin[pom]!=id0))pom++;
            }
          }
                         
        }while(!stopex);
        /* >>>>>>>>>>>>>>>>>>>>>>>>> end of internal exhaustive step >>>>>>>>>>>>>>>>>>>>*/
        /*-------------------------------------------------------------------------------*/
    
        if(/*(vykyv==0)||*/(SG_type==SGFULL))
        {
            SG_stopfloat = 1;
        }
        else if(SG_type==SGSEQUENTIAL)
        {
            SG_rr++;
            if(SG_rr>rr)
            {
                if((bestvalue>SG_globalbestvalue[SG_fixsumr-1])||(SG_globalbestvalue[SG_fixsumr-1]<-SAFEUP/2))
                {
                  memcpy(&SG_globalbestset[(SG_fixsumr*(SG_fixsumr-1))/2],bestset,sumrint);
                  SG_globalbestvalue[SG_fixsumr-1]=bestvalue;
                  /*SG_wasthebest=1;*/
                }
                /*else SG_wasthebest=0;*/
                SG_stopfloat = 1;
            }
            else
            {
                if(SG_sbs)
                {
                    for(i=0;i<=n;i++) bin[i]=-1;
                    for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=2;
                    SG_fixsumr--;                    
                }
                else
                {
                    if(vykyv==0) /* during initialization, for SG_rr>rr it will run after the end of the whole SG_stopfloat cycle*/
                    {
                        memcpy(&globalbestset[(SG_fixsumr*(SG_fixsumr-1))/2],bestset,SG_fixsumr*sizeof(int));
                        globalbestvalue[SG_fixsumr-1]=bestvalue;
                    }
                    for(i=0;i<=n;i++) bin[i]=0;
                    for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=1;
                    SG_fixsumr++;
                }
            }
        }
        else if(SG_type==SGFLOATING)
        {
            if(bestvalue>SG_globalbestvalue[SG_fixsumr-1]) /* sumr is from interval <1,n>*/
            {
              memcpy(&SG_globalbestset[(SG_fixsumr*(SG_fixsumr-1))/2],bestset,sumrint);
              SG_globalbestvalue[SG_fixsumr-1]=bestvalue;
              SG_wasthebest=1;
            }
            else SG_wasthebest=0;
        
            if(SG_sfbs)
            {
              if(SG_sbs) /* last step was sbs */
              {
                if(SG_sumr<SG_n-SG_r) /* if adding is possible, prepare sfs */
                {
                  for(i=0;i<=n;i++) bin[i]=0; /* conversion to sfs format */
                  for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=1;
                  SG_sbs=0; id0=0; id2=2;
                  SG_zmenasumr=1;
                }
                else
                {
                  SG_zmenasumr=-1; /* otherwise stay by sbs */
                  for(i=0;i<=n;i++) bin[i]=-1;
                  for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=2; /* freeze the change */
                }
              }
              else /* last step was sfs */
              {
                if(SG_wasthebest) /* better solution was found */
                {
                  if(SG_sumr<SG_n-SG_r)
                  {
                    SG_zmenasumr=1; /* repeat sfs */
                    for(i=0;i<=n;i++) bin[i]=0;
                    for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=1; /* freeze the change */
                  }
                  else
                  { /* nothing may be added, switch to sbs */
                    for(i=0;i<=n;i++) bin[i]=-1; /* conversion to sbs format */
                    for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=2;
                    SG_sbs=1; id0=2; id2=0;
                    SG_zmenasumr=-1;
                  }
                }
                else /* no improvement during last step (sfs) */
                {
                  /* change "bin" for sbs but after the change of "sumr" */
                  SG_sbs=1; id0=2; id2=0;
                  SG_zmenasumr=-2; /* forget last step and perform one new sbs step */
                }
              }
              /* actualize sumr and rr */
              if(SG_zmenasumr==1)
                {if((SG_sumr==SG_d)&&((SG_n-SG_d)%SG_r!=0)) {SG_sumr=SG_d+(SG_n-SG_d)%SG_r; SG_rr=(SG_n-SG_d)%SG_r;} else {SG_sumr+=SG_r; SG_rr=SG_r;}
                     if(SG_vykyv>0) SG_vykyv+=SG_rr; /* continue*/
                     else SG_vykyv=SG_rr; /* begin */
                    }
              else /* zmenasumr== -1 nebo -2 */
                {if((SG_sumr>SG_d)&&(SG_sumr-SG_r<SG_d)) {SG_sumr=SG_d; SG_rr=(SG_n-SG_d)%SG_r;} else {SG_sumr-=SG_r; SG_rr=SG_r;}
                     if(SG_vykyv>0) { /* end of going up*/
                         if(SG_floatdelta==-1){ //averaging
                             SG_vykyvmez=(SG_vykyvcount*SG_vykyvmez+SG_vykyv)/(SG_vykyvcount+1);
                             SG_vykyvcount++;
                         }else if(SG_floatdelta==-2){ /* maximization*/
                             if(SG_vykyv>SG_vykyvmez) SG_vykyvmez=SG_vykyv;
                         }
                     }
                     SG_vykyv=0;
                    }
              if(SG_zmenasumr==-2) /* once more */
              {
                SG_fixsumr=SG_sumr; /* change to sbs with changed "sumr" */
                for(i=0;i<=n;i++) bin[i]=-1; 
                pom=(SG_fixsumr*(SG_fixsumr-1))/2;
                for(i=0;i<SG_fixsumr;i++) bin[SG_globalbestset[pom+i]]=2;
                if((SG_sumr>SG_d)&&(SG_sumr-SG_r<SG_d)) {SG_sumr=SG_d; SG_rr=(SG_n-SG_d)%SG_r;} else {SG_sumr-=SG_r; SG_rr=SG_r;}
                    /* no change in direction*/
              }
        
              if((SG_floatdelta==-1)||(SG_floatdelta==-2)) {SG_stopfloatmez=(int)(SG_d-DELTAMUL*SG_vykyvmez-DELTAADD); if(SG_stopfloatmez<1) SG_stopfloatmez=1;}
              if(SG_sumr<SG_stopfloatmez) SG_stopfloat=1; /* end if delta reached */
            }
            else /* sffs */
            {
              if(SG_sbs) /* last step was sbs */
              {
                if(SG_wasthebest) /* a better subset was found */
                {
                  if(SG_sumr>SG_r)
                  {
                    SG_zmenasumr=-1; /* so repeat sbs */
                    for(i=0;i<=n;i++) bin[i]=-1;
                    for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=2; /* freeze changes */
                  }
                  else
                  { /* nothing to remove, change to sfs */
                    for(i=0;i<=n;i++) bin[i]=0; /* switch to sfs format */
                    for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=1;
                    SG_sbs=0; id0=0; id2=2;
                    SG_zmenasumr=1;
                  }
                }
                else /* no improvement during last step (sbs) */
                {
                  /* change to "bin" for sfs later after "sumr" gets its original value */
                  SG_sbs=0; id0=0; id2=2;
                  SG_zmenasumr=2; /* forget last step and perform one new sfs step */
                }
              }
              else /* last step was sfs */
              {
                if(SG_sumr>SG_r) /* if removing is possible, prepare sbs */
                {
                  for(i=0;i<=n;i++) bin[i]=-1;
                  for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=2;
                  SG_sbs=1; id0=2; id2=0;
                  SG_zmenasumr=-1;
                }
                else
                {
                  SG_zmenasumr=1; /* othervise stay by sfs */
                  for(i=0;i<=n;i++) bin[i]=0;
                  for(i=0;i<SG_fixsumr;i++) bin[bestset[i]]=1; /* freeze changes */
                }
              }
              /* renew sumr and rr */
              if(SG_zmenasumr==-1)
                {if((SG_sumr==SG_d)&&(SG_d%SG_r!=0)) {SG_sumr=SG_d-SG_d%SG_r; SG_rr=SG_d%SG_r;} else {SG_sumr-=SG_r; SG_rr=SG_r;}
                     if(SG_vykyv<0) SG_vykyv-=SG_rr; /* continue*/
                     else SG_vykyv=-SG_rr; /* begin*/
                    }
              else /* zmenasumr== 1 or 2 */
                {if((SG_sumr<SG_d)&&(SG_sumr+SG_r>SG_d)) {SG_sumr=SG_d; SG_rr=SG_d%SG_r;} else {SG_sumr+=SG_r; SG_rr=SG_r;}
                     if(SG_vykyv<0) { /* end of going down*/
                         if(SG_floatdelta==-1){ /* averaging*/
                             SG_vykyvmez=(SG_vykyvcount*SG_vykyvmez+(-SG_vykyv))/(SG_vykyvcount+1);
                             SG_vykyvcount++;
                         }else if(SG_floatdelta==-2){ /* maximizing*/
                             if(-SG_vykyv>SG_vykyvmez) SG_vykyvmez=-SG_vykyv;
                         }
                     }
                     SG_vykyv=0;
                    }
              if(SG_zmenasumr==2) /* once more and renew "bin"*/
              {
                SG_fixsumr=SG_sumr+SG_fixsize; 
                for(i=0;i<=n;i++) bin[i]=0; /* change to sfs format now with actualized "sumr" */
                pom=(SG_fixsumr*(SG_fixsumr-1))/2;
                for(i=0;i<SG_fixsumr;i++) bin[SG_globalbestset[pom+i]]=1;
                if((SG_sumr<SG_d)&&(SG_sumr+SG_r>SG_d)) {SG_sumr=SG_d; SG_rr=SG_d%SG_r;} else {SG_sumr+=SG_r; SG_rr=SG_r;}
                    /* no direction change*/
              }
        
              if((SG_floatdelta==-1)||(SG_floatdelta==-2)){SG_stopfloatmez=(int)(SG_d+DELTAMUL*SG_vykyvmez+DELTAADD); if(SG_stopfloatmez>SG_n) SG_stopfloatmez=SG_n;}
              if(SG_sumr>SG_stopfloatmez) SG_stopfloat=1; /* prekrocils-li delta, konci */
            }
        }
    
      }while(!SG_stopfloat);
      
    /*----------------------------------------------------------------------------------*/
    
    do
    {
        if(/*(vykyv==0)||*/(SG_type==SGFULL)||(SG_type==SGSEQUENTIAL))
        {
            if((vykyv!=0)&&(bestvalue>globalbestvalue[sumr-1])) /* sumr is from interval <1,n>*/
            {
              memcpy(&globalbestset[(sumr*(sumr-1))/2],bestset,sumr*sizeof(int));
              globalbestvalue[sumr-1]=bestvalue;
              if(bestvalue>SAFEUP) goto AfterError; 
              wasthebest=1;
            }
            else wasthebest=0;
        }
        else if(SG_type==SGFLOATING)
        {
            if(vykyv==0) /* during initialization copy SG_globalbest... to globalbest...*/
            {
              memcpy(globalbestset,SG_globalbestset,((n*(n+1))/2)*sizeof(int));
              memcpy(globalbestvalue,SG_globalbestvalue,n*sizeof(double));
            }
            if(SG_globalbestvalue[sumr-1]>globalbestvalue[sumr-1]) /* sumr is from interval <1,n>*/
            {
              memcpy(&globalbestset[(sumr*(sumr-1))/2],&SG_globalbestset[(sumr*(sumr-1))/2],sumr*sizeof(int));
              globalbestvalue[sumr-1]=SG_globalbestvalue[sumr-1];
              wasthebest=1;
            }
            else wasthebest=0;
        }
    
        /*=============== OSCILLATING BODY CONTINUES ======================================*/
        AfterInit: ;

        do
        {
            SG_accelerate=0; /* to be set if computations may be overjumped*/
            repeat=0; /* to be set in forthcoming code to break the os.cycle swing - causes a return to the beginning of this switch */
            kmitpart++; /* next swing phase*/
            
            switch(vykyv){
                case 0: /* after initialization*/
                
                        /* initial setting of the internal run type */
                        if(deltasq>0) {SG_TYPE=SGSEQUENTIAL; delta=deltasq;}
                        else if(deltafl>0) {SG_TYPE=SGFLOATING; delta=deltafl;}
                        else if(deltaex>0) {SG_TYPE=SGFULL; delta=deltaex;}
                        else {FREE(SG_globalbestvalue) FREE(SG_globalbestset) FREE(SG_fixbin) FREE(globalbestvalue) FREE(globalbestset) FREE(bestset) FREE(index) FREE(bin) return(34);}
                        for(i=0;i<(n*(n+1))/2;i++) SG_globalbestset[i]=0;
                        for(i=0;i<n;i++) SG_globalbestvalue[i]=-SAFEUP;
                        rr=1;
                        if(SG_TYPE==SGFLOATING) {SG_type=SGSEQUENTIAL; SG_floatdelta=floatdelta;}
                        else SG_type=SG_TYPE;
                        /* end of setting of the internal run type */
                        
                        kmitpart=0;
                        if(d-rr<1) {kmitpart++; wasthebest=0; vykyv=2; repeat=1;}
                        else
                        {
                            for(i=0;i<=n;i++) bin[i]=-1; /* change to sbs format (1) */
                            pom=(d*(d-1))/2;
                            for(i=0;i<d;i++) bin[globalbestset[pom+i]]=2; /* starting in d-th dimension*/
                            if(d==1) stopfloat=1; /* no sense in this case*/
                            sumr=d-1;
                            vykyv=1; id0=2; id2=0;
                        }
                        break;
                case 1: vykyv = 2;
                        if(strict&&(!wasthebest)) repeat=1; /* first down-swing phase not successfull - switch to up-swing*/
                        else /* first down-swing phase successfull - continue by the second phase */
                        { 
                            for(i=0;i<=n;i++) bin[i]=0; /* switch to sfs format (2) */
                            if((!strict)&&(!wasthebest))
                            {
                                if((SG_type==SGFULL)||(SG_type==SGSEQUENTIAL))
                                    for(i=0;i<sumr;i++) bin[bestset[i]]=1; /* starting in (d-rr)-th dimension*/
                                else /* SG_type==SGFLOATING*/
                                {
                                    pom=(sumr*(sumr-1))/2;
                                    for(i=0;i<sumr;i++) bin[SG_globalbestset[pom+i]]=1; /* starting in (d-rr)-th dimension*/
                                }
                            }
                            else
                            {
                                pom=(sumr*(sumr-1))/2;
                                for(i=0;i<sumr;i++) bin[globalbestset[pom+i]]=1; /* starting in (d-rr)-th dimension*/
                            }
                            sumr=d; /* no check necessary, d is safe, rr is not here*/
                            /*vykyv=2;*/ id0=0; id2=2;                    
                        }
                        break;
                case 2: if(wasthebest)  /* second down-swing phase successfull*/
                        {
                            kmitpart=0;
                            /* initial setting of the internal run type */
                            if(deltasq>0) {SG_TYPE=SGSEQUENTIAL; delta=deltasq;}
                            else if(deltafl>0) {SG_TYPE=SGFLOATING; delta=deltafl;}
                            else if(deltaex>0) {SG_TYPE=SGFULL; delta=deltaex;}
                            else {FREE(SG_globalbestvalue) FREE(SG_globalbestset) FREE(SG_fixbin) FREE(globalbestvalue) FREE(globalbestset) FREE(bestset) FREE(index) FREE(bin) return(34);}
                            if((SG_type==SGSEQUENTIAL)/*&&(rr>1)*/) { 
                                for(i=0;i<(n*(n+1))/2;i++) SG_globalbestset[i]=0;
                                for(i=0;i<n;i++) SG_globalbestvalue[i]=-SAFEUP;
                            }
                            if(rr>max_rr_advantage) max_rr_advantage=rr; /* informational only*/
                            rr=1;
                            if(SG_TYPE==SGFLOATING) {SG_type=SGSEQUENTIAL; SG_floatdelta=floatdelta;}
                            else SG_type=SG_TYPE;
                            /* end of setting of the internal run type */
                        }
                        else
                        {
                            if(kmitpart==4) 
                            {
                                kmitpart=0; 
                                /* rr increase, and/or selecition of the next internal run type */
                                rr++;
                                if(rr<=delta) {SG_type=SG_TYPE; if(SG_TYPE==SGFLOATING) SG_floatdelta=floatdelta;}
                                else
                                {
                                    if(SG_TYPE==SGSEQUENTIAL) {
                                        if(deltafl>0) {SG_TYPE=SGFLOATING; SG_type=SGSEQUENTIAL; delta=deltafl; rr=1;}
                                        else if(deltaex>0) {SG_TYPE=SGFULL; SG_type=SGFULL; delta=deltaex; rr=1;}
                                        else {stopfloat=1; break;}
                                        if(!stopfloat) {
                                            for(i=0;i<(n*(n+1))/2;i++) SG_globalbestset[i]=0;
                                            for(i=0;i<n;i++) SG_globalbestvalue[i]=-SAFEUP;
                                        }
                                    }
                                    else if(SG_TYPE==SGFLOATING) {
                                        if(deltaex>0) {SG_TYPE=SGFULL; SG_type=SGFULL; delta=deltaex; rr=1;}
                                        else {stopfloat=1; break;}
                                    }
                                    else {stopfloat=1; break;}
                                }
                                /* end of rr increase, and/or selecition of the next internal run type */
                            }
                            /* deleting next 2 lines + the equivalent ones in case 4 would switch off the acceleration*/
                            if((rr>1)&&(SG_type==SGSEQUENTIAL)) {SG_accelerate=SGSEQUENTIAL; SG_accelerated++;}
                            if((rr>2)&&(SG_type==SGFLOATING)&&(SG_floatdelta==0)) {SG_accelerate=SGFLOATING; SG_accelerated++;}
                        }
                        if(d+rr>n-1) {kmitpart++; vykyv=4; wasthebest=0; repeat=1;}
                        else {
                            for(i=0;i<=n;i++) bin[i]=0; /* switch to sfs format (2) */
                            if(SG_accelerate==SGSEQUENTIAL)
                            {
                                pom=((d+rr-1)*((d+rr-1)-1))/2;
                                for(i=0;i<d+rr-1;i++) bin[SG_globalbestset[pom+i]]=1; /* startng in (d+rr-1)-th dimension */
                            }
                            else
                            {
                                pom=(d*(d-1))/2;
                                for(i=0;i<d;i++) bin[globalbestset[pom+i]]=1; /* starting in d-th dimension*/
                            }
                            sumr=d+rr; 
                            vykyv=3; id0=0; id2=2;                    
                        }
                        break;
                case 3: vykyv = 4;
                        if(strict&&(!wasthebest)) repeat=1; /* first up-swing phase not successfull - switch to down-swing*/
                        else /* first up-swing phase successfull - continue by the second phase    */
                        {
                            for(i=0;i<=n;i++) bin[i]=-1; /* switch to sbs format (4) */
                            if((!strict)&&(!wasthebest))
                            {
                                if((SG_type==SGFULL)||(SG_type==SGSEQUENTIAL))
                                    for(i=0;i<sumr;i++) bin[bestset[i]]=2; /* starting in (d+rr)-th dimension*/
                                else /* SG_type==SGFLOATING*/
                                {
                                    pom=(sumr*(sumr-1))/2;
                                    for(i=0;i<sumr;i++) bin[SG_globalbestset[pom+i]]=2; /* starting in (d+rr)-th dimension*/
                                }
                            }
                            else
                            {
                                pom=(sumr*(sumr-1))/2;
                                for(i=0;i<sumr;i++) bin[globalbestset[pom+i]]=2; /* starting in (d+rr)-th dimension*/
                            }
                            sumr=d; 
                            /*vykyv=4;*/ id0=2; id2=0;
                        }
                        break;
                case 4: if(wasthebest) /* second up-swing phase successfull*/
                        {
                            kmitpart=0;
                            /* initial setting of the internal run type */
                            if(deltasq>0) {SG_TYPE=SGSEQUENTIAL; delta=deltasq;}
                            else if(deltafl>0) {SG_TYPE=SGFLOATING; delta=deltafl;}
                            else if(deltaex>0) {SG_TYPE=SGFULL; delta=deltaex;}
                            else {FREE(SG_globalbestvalue) FREE(SG_globalbestset) FREE(SG_fixbin) FREE(globalbestvalue) FREE(globalbestset) FREE(bestset) FREE(index) FREE(bin) return(34);}
                            if((SG_type==SGSEQUENTIAL)/*&&(rr>1)*/) {
                                for(i=0;i<(n*(n+1))/2;i++) SG_globalbestset[i]=0;
                                for(i=0;i<n;i++) SG_globalbestvalue[i]=-SAFEUP;
                            }
                            if(rr>max_rr_advantage) max_rr_advantage=rr; /* informational only*/
                            rr=1;
                            if(SG_TYPE==SGFLOATING) {SG_type=SGSEQUENTIAL; SG_floatdelta=floatdelta;}
                            else SG_type=SG_TYPE;
                            /* end of setting of the internal run type */
                        }
                        else
                        {
                            if(kmitpart==4) 
                            {
                                kmitpart=0; 
                                /* rr increase, and/or selecition of the next internal run type */
                                rr++;
                                if(rr<=delta) {SG_type=SG_TYPE; if(SG_TYPE==SGFLOATING) SG_floatdelta=floatdelta;}
                                else
                                {
                                    if(SG_TYPE==SGSEQUENTIAL) {
                                        if(deltafl>0) {SG_TYPE=SGFLOATING; SG_type=SGSEQUENTIAL; delta=deltafl; rr=1;}
                                        else if(deltaex>0) {SG_TYPE=SGFULL; SG_type=SGFULL; delta=deltaex; rr=1;}
                                        else {stopfloat=1; break;}
                                        if(!stopfloat) { 
                                            for(i=0;i<(n*(n+1))/2;i++) SG_globalbestset[i]=0;
                                            for(i=0;i<n;i++) SG_globalbestvalue[i]=-SAFEUP;
                                        }
                                    }
                                    else if(SG_TYPE==SGFLOATING) {
                                        if(deltaex>0) {SG_TYPE=SGFULL; SG_type=SGFULL; delta=deltaex; rr=1;}
                                        else {stopfloat=1; break;}
                                    }
                                    else {stopfloat=1; break;}
                                }
                                /* end of rr increase, and/or selecition of the next internal run type */
                            }
                            /* deleting next 2 lines + the equivalent ones in case 2 would switch off the acceleration*/
                            if((rr>1)&&(SG_type==SGSEQUENTIAL)) {SG_accelerate=SGSEQUENTIAL; SG_accelerated++;}
                            if((rr>2)&&(SG_type==SGFLOATING)&&(SG_floatdelta==0)) {SG_accelerate=SGFLOATING; SG_accelerated++;}
                        }
                        if(d-rr<1) {kmitpart++; vykyv=2; wasthebest=0; repeat=1;}
                        else {
                            for(i=0;i<=n;i++) bin[i]=-1; /* switch to sbs format (1) */
                            if(SG_accelerate==SGSEQUENTIAL)
                            {
                                pom=((d-rr+1)*((d-rr+1)-1))/2;
                                for(i=0;i<d-rr+1;i++) bin[SG_globalbestset[pom+i]]=2; /* starting in (d-rr+1)-th dimension */
                            }
                            else
                            {
                                pom=(d*(d-1))/2;
                                for(i=0;i<d;i++) bin[globalbestset[pom+i]]=2; /* starting in d-th dimension*/
                            }
                            sumr=d-rr; /* tested before*/
                            vykyv=1; id0=2; id2=0;
                        }
                        break;
            }        
            if(stopfloat) {repeat=0; SG_accelerate=0;}
        }while(repeat);  
        
    }while(SG_accelerate==SGFLOATING);
    
  }while(!stopfloat);

  AfterError: ;
  if(detail&STANDARD){printtext("(max_rr_advantage = "); printint(max_rr_advantage); printtext(")\n");}
  telp=time(NULL); 
  bset->comp_time=difftime(telp,tbegin);
  pom=(d*(d-1))/2;
  bset->subsetsize=d;
  FREE(bset->featureset)
  if((bset->featureset=(int *)malloc((long)d*(long)sizeof(int)))==NULL) error=3;
  else for(i=0;i<d;i++)bset->featureset[i]=globalbestset[pom+i];
  bset->critvalue=globalbestvalue[d-1];
  bset->enumcount=ecit;
  
  FREE(SG_globalbestvalue) 
  FREE(SG_globalbestset) 
  FREE(SG_fixbin) 
  FREE(globalbestset)
  FREE(globalbestvalue)
  FREE(bestset)
  FREE(index)
  FREE(bin)
  return(error);
}
