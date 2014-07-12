 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#ifndef __kl_random_number_generator__
#define __kl_random_number_generator__

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include<cmath>
#pragma warning (once : 4244)
#pragma warning (once : 4305)


#include "kl_matrix.h"

#define SQRT_TWO_PI 2.5066282746310

extern double klErrorFunction(double x);

//Base class for Random Variables
//If the cdf or pdf is not defined, as for Levy distributions, 
template<class TYPE> class klRV
{
public:
    virtual TYPE operator()()=0;//{return 0;}
    virtual TYPE CDF(TYPE x){return 0;}
    virtual TYPE PDF(TYPE x){return 0;}
};

//MersseneTwister uniformly distibuted integer numbers in the range [0:2^32-1].  The algorithm for 
// this random number generator is called Mersenne Twister by
// Takuji Nishimura.  It passes the DIEHARD tests written by 
// George Marsaglia of Florida State University.  It has a
// very long period (it will not repeat it's sequence for 2^19937-1 iterations).
// This is intended to be a base class for other random number generators.
class klMersenneTwister
{
public: 

    // Construtor that specifies the seed for the random number generator
    klMersenneTwister(int s = 4357) : 
      next(NULL), remaining(-1), seed(s){}

      // Destructor
      virtual ~klMersenneTwister(){}

      // Reset the random number generator.  Pass in a new seed.
      // If the value is 0 (default), then the same seed that
      // was set in the constructor will be used.
      void reset( unsigned int s = 0 );

      // Calculate the next value in the random sequence
      unsigned int nextValue();


      // Calculate the next value and normalize it so it is in the 
      // range [0,1].
      double nextNormalizedValue();
private:

    // This routine refills the state array when it becomes empty.
    unsigned int reloadMT();
    unsigned int state[624];
    unsigned int *next;
    int remaining;
    // This is the seed
    unsigned int seed;
};

//globalSeedGenerator, used in conjunction with ANSI clock() to generate seeds
extern klMersenneTwister globalSeedGenerator;

inline int fastSeed()
{	
    unsigned seed;
    time_t time_of_day;
    struct tm *tm_buf;
    time_of_day = time( NULL );
    tm_buf=localtime(&time_of_day);
    seed=time_of_day%clock()+(long)tm_buf->tm_sec;
    seed+=globalSeedGenerator.nextValue();
    return seed;

}

template<class TYPE>
class klUniformRV: public klMersenneTwister, public klRV<TYPE>
{
public:
    // Default constructor  - sets the range to all possible values
    // for the given type.  
    klUniformRV() : 
      minimum(0), maximum(1)
      {
          range = maximum-minimum;
      }

      // Constructor that lets you set the minimum and maximum.
      klUniformRV(TYPE min, TYPE max ) : 
      minimum(min), maximum(max)
      {
          range = maximum-minimum;
      }

      // Get the next random number in the sequence.
      virtual TYPE operator()()
      {
          if( range <= 0 )
              return minimum;
          TYPE dummy=range*nextNormalizedValue()+minimum;;//implicit conversion from double

          return dummy;
      }


      // Accessor for setting the min and max values.
      void setRange( TYPE min, TYPE max )
      {
          minimum = min;
          maximum = max;
          range = max-min;
      }

      // Accessor for setting the minimum value
      void setMinimum( TYPE min )
      {
          minimum = min;
          range = maximum-minimum;
      }

      // Accessor for setting the maximum value
      void setMaximum( TYPE max )
      {
          maximum = max;
          range = maximum-minimum;
      }


protected:
    // The minimum value produced by this class
    TYPE minimum;
    // The maximum value produced by this class
    TYPE maximum;

    // The range of values produced by this class
    double range;
};


//This generator uses the approximation to the inverse CDF of the normal distribution by 
//Peter J. Acklam.  See http://home.online.no/~pjacklam/notes/invnorm/index.html
//This method of generating normal variates avoids the undesirable side effects 
//found by Neave in 
//		H.R. Neave "On using the Box-Muller transformation with multiplicative congurential 
//		psuedo-random number generators"  Applied Statistics. 22 pp92, 1973
//The uniform vairates produced by this generator are suitable for use with low discrepancy numbers
//and quasi-Monte Carlo simulations.
//
template<class TYPE> class klNormalInverseApproxRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klNormalInverseApproxRV(TYPE mean=0, TYPE variance=1, bool bSeedFromClock = true, unsigned int rvSeed0 = 0)
    {
        _stddev=sqrt(variance);
        NormalRVReset(mean,variance,bSeedFromClock, rvSeed0);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        double a=normal();
        TYPE dummy=a;//implicit conversion from double
        return dummy;
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void NormalRVReset(TYPE mean=0, TYPE variance=1,bool bSeedFromClock = true, unsigned int rvSeed0 = 0 )
    {
        _mean=mean;
        _variance=variance;
        if( bSeedFromClock )
        {
            _seedValue0 = fastSeed();

        }
        else
        {
            _seedValue0 = rvSeed0;
            _Normal.reset(_seedValue0);
            _Normal.setRange(0,1);
        }

    }


    double PDF(double x)
    {
        //bbc revisit - add mu and sigma and investigate cdf 
        return 0.3989422804014326779399460599343818684758586311649 * exp( -0.5 * (x*x) );
    }

    double NormalCDF(double z)
    {
        double result;

        int floatingPointStatus=_status87();

        _clear87(); //clears foating point status word

        floatingPointStatus=_status87();

#define ONE_OVER_SQRT_TWO 0.7071067811865475244008443621048490392848359376887

        result = 0.5 * ( 1 + klErrorFunction( z*ONE_OVER_SQRT_TWO ) ); 

        floatingPointStatus=_status87(); //MSVC RTL floating point status


        //#define _SW_DENORMAL    0x00080000   denormal status bit  
        if (result<=DBL_EPSILON ||floatingPointStatus==_SW_DENORMAL){//bbcrevisit which one get's hit 
            //
            // Asymptotic expansion for very negative z following (26.2.12) on page 408 in
            // M. Abramowitz and A. Stegun, Pocketbook of Mathematical Functions, ISBN 3-87144818-4.
            //
            double sum=1, zsqr=z*z, i=1, g=1, x, y, a=DBL_MAX, lasta;
            do {
                lasta=a;
                x = (4*i-3)/zsqr;
                y = x*((4*i-1)/zsqr);
                a = g*(x-y);
                sum -= a;
                g *= y;
                ++i;
                a = fabs(a);
            } while (lasta>a && a>=fabs(sum*DBL_EPSILON));
            result = -PDF(z)/(z*sum);
        }
        return result;
    }




private:
    unsigned int _seedValue0;
    TYPE _mean;
    TYPE _variance;
    TYPE _stddev;
    klUniformRV<double> _Normal;

    double normal()
    {	
        double u=_Normal();
        assert( 0 <= u && u <= 1 );
		if (u==0)
		{
			u=DBL_MIN; //So we don't take log of 0 down below.
		}

        // Coefficients for the rational approximation.
        const double
            a1 = -3.969683028665376e+01,
            a2 =  2.209460984245205e+02,
            a3 = -2.759285104469687e+02,
            a4 =  1.383577518672690e+02,
            a5 = -3.066479806614716e+01,
            a6 =  2.506628277459239e+00;

        const double
            b1 = -5.447609879822406e+01,
            b2 =  1.615858368580409e+02,
            b3 = -1.556989798598866e+02,
            b4 =  6.680131188771972e+01,
            b5 = -1.328068155288572e+01;

        const double
            c1 = -7.784894002430293e-03,
            c2 = -3.223964580411365e-01,
            c3 = -2.400758277161838e+00,
            c4 = -2.549732539343734e+00,
            c5 =  4.374664141464968e+00,
            c6 =  2.938163982698783e+00;

        const double
            d1 =  7.784695709041462e-03,
            d2 =  3.224671290700398e-01,
            d3 =  2.445134137142996e+00,
            d4 =  3.754408661907416e+00;

        // Limits of the approximation region.
        const double
            u_low   = 0.02425,
            u_high  = 1.0 - u_low;

        register double z, r;

        // Rational approximation for the lower region. ( 0 < u < u_low )
        if( u < u_low ){
            z = sqrt(-2.0*log(u));
            z = (((((c1*z+c2)*z+c3)*z+c4)*z+c5)*z+c6) / ((((d1*z+d2)*z+d3)*z+d4)*z+1.0);
        }
        // Rational approximation for the central region. ( u_low <= u <= u_high )
        else if( u <= u_high ){
            z = u - 0.5;
            r = z*z;
            z = (((((a1*r+a2)*r+a3)*r+a4)*r+a5)*r+a6)*z / (((((b1*r+b2)*r+b3)*r+b4)*r+b5)*r+1.0);
        }
        // Rational approximation for the upper region. ( u_high < u < 1 )
        else {
            z = sqrt(-2.0*log(1.0-u));
            z = -(((((c1*z+c2)*z+c3)*z+c4)*z+c5)*z+c6) /  ((((d1*z+d2)*z+d3)*z+d4)*z+1.0);
        }

#define REFINE_INVERSECUMULATIVENORMAL_TO_FULL_MACHINE_PRECISION_USING_HALLEYS_METHOD
#ifdef REFINE_INVERSECUMULATIVENORMAL_TO_FULL_MACHINE_PRECISION_USING_HALLEYS_METHOD
        //  // The relative error of the approximation has absolute value less
        //  // than 1.15e-9.  One iteration of Halley's rational method (third
        //  // order) gives full machine precision.
        // 
        r = (this->NormalCDF(z) - u) * SQRT_TWO_PI * exp( 0.5 * z * z );	//	f(z)/df(z)
        z -= r/(1+0.5*z*r);							//	Halley's method
#endif


        //Now to convert the N(0,1) variate to N(sigma,omega^2)


        return _mean+_stddev*z;


    }

};


// Implemented using the polar Box-Muller method of Marsaglia and Bray.<br>
//This method avoids the use of transcendental functions.
//There is no upper limit to the number of uniform samples used to generate a single 
//instance, so this generator is not compatible with quasi-Monte Carlo simulation.
// Probability Density Function: p(x) = e^( (-x^2)/2 )/( sqrt(2PI) )
// Dagpunar, Principles of Random Variate Generation, Clarendon Press 1988
template<class TYPE> class klNormalBoxMullerRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klNormalBoxMullerRV(bool bSeedFromClock = true, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        NormalRVReset(bSeedFromClock, rvSeed0, rvSeed1);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        normal();
        TYPE dummy=Normal_X;//implicit conversion from double
        return dummy;
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void NormalRVReset(bool bSeedFromClock = true, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        if( bSeedFromClock )
        {
            seedValue0 = fastSeed();
            seedValue1 = seedValue0/7;
        }
        else
        {
            seedValue0 = rvSeed0;
            seedValue1 = rvSeed1;
        }

        Normal_R1.reset(seedValue0);
        Normal_R1.setRange(0,1);
        Normal_R2.reset(seedValue1);
        Normal_R2.setRange(0,1);			

        Normal_Iset=0;
    }

private:
    int Normal_I, Normal_Iset;
    double Normal_B, Normal_U2,Normal_X;
    unsigned int seedValue0, seedValue1;
    klUniformRV<double> Normal_R1;
    klUniformRV<double> Normal_R2;

    double normal()
    {	
        if(Normal_Iset==0)
        { 
            Normal_I=0;
            Normal_Iset=1;
        }

        normalCompHlp();
        return Normal_X;
    }

    void normalCompHlp()
    {
        double Normal_U1,Normal_S,Normal_Y1,Normal_Y2;
        if(Normal_I==1)
        {
            Normal_X=Normal_B*Normal_U2;
        }
        else
        {
            do{
                //	generate Normal_R1;
                Normal_Y1 = Normal_R1();
                //	generate Normal_R2;
                Normal_Y2 = Normal_R2();
                Normal_U1=(2.0*Normal_Y1)-1.0;
                Normal_U2=(2.0*Normal_Y2)-1.0;
                Normal_S=pow(Normal_U1,2.0)+pow(Normal_U2,2.0);

            }while(Normal_S>1);
            Normal_B=pow( (-2.0*log(Normal_S))/Normal_S,0.5 );
            Normal_X=Normal_B*Normal_U1;
        }

        Normal_I=(Normal_I+1)%2;
    }
};


//	require alpha > 1
// Probability Density Function: y = x ^ (alpha -1) * exp(- x) / gamma (alpha)
// Dagpunar, Principles of Random Variate Generation, Clarendon Press 1988
template<class TYPE> class klGammaRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klGammaRV(double G_a = 1.9, bool bSeedFromClock = true,unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        Gamma_X=0;

        klGammaRVReset(bSeedFromClock, G_a, rvSeed0, rvSeed1);

    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        if( Gamma_X != -1 )
        {
            gamma();
            TYPE dummy=Gamma_X;//implicit conversion from double
            return dummy;
        }
        else
        {
            cout << "Problem with klGammaRV! Gamma_X != -1 " << endl;
            TYPE dummy=Gamma_X;//implicit conversion from double
            return dummy;
        }
    }

    // Operator that allows alpha to be changed during the distribution calculation
    TYPE operator()(double G_a)
    {
        if( Gamma_X != -1 )
        {
            Gamma_alpha = G_a;

            gamma();
            TYPE dummy=Gamma_X;
            return dummy;
        }
        else
        {
            cout << "Gamma RNG is not setup correctly.  A reset is required." << endl;
            cout << "The input value \"Alpha\" must be >1!" << endl;
            return Gamma_X;
        }
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klGammaRVReset(  bool bSeedFromClock = true,double G_a = 1.9, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        Gamma_alpha = G_a;

        if( Gamma_alpha <= 1)	// alpha must be > 1
        {
            Gamma_X = -1; // error value
            return;
        }

        if( bSeedFromClock )
        {
            seedValue0 = fastSeed();
            seedValue1 = seedValue0/7;
        }
        else
        {
            seedValue0 = rvSeed0;
            seedValue1 = rvSeed1;
        }

        Gamma_U1.reset(seedValue0);
        Gamma_U1.setRange(0,1);
        Gamma_U2.reset(seedValue1);
        Gamma_U2.setRange(0,1);

        Gamma_b = Gamma_alpha - 1;
        Gamma_c = (3*Gamma_alpha) - 0.75;
    }

private:
    double Gamma_a, Gamma_alpha, Gamma_b, Gamma_c, Gamma_g, Gamma_X;
    unsigned int seedValue0, seedValue1;
    klUniformRV<double> Gamma_U1;
    klUniformRV<double> Gamma_U2;

    double gamma()
    {	
        gammaCompHlp();

        return Gamma_X;
    }

    void gammaCompHlp()
    {
        double Gamma_R1, Gamma_g, Gamma_f, Gamma_d, Gamma_temp;
        do{
            do{
                Gamma_R1 = Gamma_U1();
                Gamma_g = Gamma_R1 - pow(Gamma_R1, 2.0);
                Gamma_f = (Gamma_R1 - 0.5) * pow( (Gamma_c/Gamma_g), 0.5);
                Gamma_X = Gamma_b + Gamma_f;
            }while (Gamma_X<=0);

            Gamma_d = 64.0 * pow(Gamma_U2(), 2.0) * pow(Gamma_g, 3.0);

            Gamma_temp = 1.0 - 2.0*( pow(Gamma_f,2.0) / Gamma_X );
            if( Gamma_d < Gamma_temp)
            {
                return;
            }

            Gamma_temp = 2 * ( (Gamma_b * log(Gamma_X/Gamma_b)) - Gamma_f);
        }while(log(Gamma_d) >= Gamma_temp);
    }
};


// Implemented using the log-logistic method
// It is templated so that you can get a random number of any data type.
// The fixed point versions will round the generated number to the
// nearest integral value.
// Probability Density Function: fw(x) = [x^(alpha + beta)]*[(1+x)^-(alpha + beta)]/B(alpha, beta), where (x>=0)
// Dagpunar, Principles of Random Variate Generation, Clarendon Press 1988
template<class TYPE> class klBetaRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klBetaRV(double Beta_alp = 0.25, double Beta_bet = 0.5, bool bSeedFromClock = true, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        klBetaRVReset(bSeedFromClock, Beta_alp, Beta_bet, rvSeed0, rvSeed1);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        beta();
        TYPE dummy=Beta_X;
        return dummy;
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klBetaRVReset( bool bSeedFromClock = true, double Beta_alp = 0.25, double Beta_bet = 0.5, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        Beta_alpha	= Beta_alp;
        Beta_beta	= Beta_bet;

        if( bSeedFromClock )
        {
            seedValue0 = fastSeed();
            seedValue1 = seedValue0/7;
        }
        else
        {
            seedValue0 = rvSeed0;
            seedValue1 = rvSeed1;
        }

        Beta_U1.reset(seedValue0);
        Beta_U1.setRange(0,1);
        Beta_U2.reset(seedValue1);
        Beta_U2.setRange(0,1);

        Beta_U=DBL_EPSILON;  //from float.h
        Beta_V=DBL_MAX;

        if( Beta_beta > Beta_alpha)
        {
            Beta_a	= Beta_beta;
            Beta_b	= Beta_alpha;
            Beta_J	= 1;
        }
        else
        {
            Beta_a	= Beta_alpha;
            Beta_b	= Beta_beta;
            Beta_J	= 0;
        }

        Beta_d = Beta_a/Beta_b;
        Beta_f = Beta_a + Beta_b;

        if( Beta_b > 1 )
        {
            Beta_lambda = sqrt( (2 * Beta_a * Beta_b-Beta_a-Beta_b)/(Beta_a + Beta_b -2) );
            Beta_T = 1;
        }
        else
        {
            Beta_lambda = Beta_b;
            Beta_T = 1/( 1 + pow((Beta_a/(Beta_b * Beta_V)), Beta_b) );
        }

        Beta_c = Beta_a + Beta_lambda;
    }
private:
    double Beta_alpha, Beta_beta, Beta_a, Beta_b, Beta_c, Beta_d, Beta_f, Beta_J, Beta_lambda, Beta_R2, Beta_T, Beta_U, Beta_V, Beta_X;
    unsigned int seedValue0, seedValue1;
    klUniformRV<double> Beta_U1;
    klUniformRV<double> Beta_U2;

    double beta()
    {
        betaCompHlp();

        return Beta_X;
    }

    void betaCompHlp()
    {
        double Beta_R1, Beta_R2, Beta_G, Beta_S, Beta_Y, Beta_Z;

        do{
            do{
                do{
                    //	generate Beta_R1;
                    Beta_R1 = Beta_U1();
                    //	generate Beta_R2;
                    Beta_R2 = Beta_U2();

                    Beta_S = Beta_R2 * pow(Beta_R1, 2.0);
                }while( (Beta_R1 < Beta_U) || (Beta_S <= 0) );

                Beta_G = (log(Beta_R1/(1 - Beta_R1)))/ Beta_lambda;
                Beta_Y = Beta_d * exp(Beta_G);
                Beta_Z = (Beta_c * Beta_G) + (Beta_f * log( (1 + Beta_d)/(1 + Beta_Y) )) - log(4.0);

            }while( Beta_R1 < Beta_T && (Beta_S - 1) > Beta_Z &&  (Beta_S - 1) > Beta_Z*Beta_S );
        }while( Beta_R1 < Beta_T && (Beta_S - 1) > Beta_Z &&  log(Beta_S) > Beta_Z  );

        Beta_X = Beta_Y/(1 + Beta_Y);

        if( (4 * Beta_S) > pow((1 + (1/Beta_d) ), Beta_f) )
        {
            Beta_X = 1;
        }

        if( Beta_J == 1)
        {
            Beta_X = 1 - Beta_X;
        }
    }
};


//GIG Random Variate
//The GIG distributions are characterized by
//\[p(x; \lambda , \alpha, \beta) = \frac{ C }{x^{\lambda+1}}
//e^{ \frac{-1}{2} ( \alpha x + \frac{\beta}{x^{-1}}) } \]
//This is a special case of the more general GIG where \alpha=\beta.
//lambda < 0; beta > 0; lambda/beta > (0.25 * sqrt(V)), where V = machine's largest real number<br><br>
// See
// Dagpunar, Principles of Random Variate Generation, Clarendon Press 1988
template<class TYPE> class klGeneralizedInverseGaussianRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klGeneralizedInverseGaussianRV( double Gaussian_lam = 0.75, double Gaussian_bet = 0.25, bool bSeedFromClock = true, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        Gaussian_X=0;
        klGeneralizedInverseGaussianRVReset(bSeedFromClock, Gaussian_lam, Gaussian_bet, rvSeed0, rvSeed1);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        if( Gaussian_X != -1 )
        {
            gaussian();
            TYPE dummy=Gaussian_X;
            return dummy;
        }
        else
        {
            cout << "Problem with klGeneralizedInverseGaussianRV" << endl;
            cout << "The input value \"lambda\" must be <0!" << endl;
            cout << "The input value \"beta\" must be >0!" << endl;
            TYPE dummy=Gaussian_X;
            return dummy;
        }
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klGeneralizedInverseGaussianRVReset(bool bSeedFromClock = true, double Gaussian_lam = 0.75, double Gaussian_bet = 0.25, unsigned int rvSeed0 = 0, unsigned int rvSeed1 = 1)
    {
        if( (Gaussian_lam < 0) || !(Gaussian_bet > 0) )
        {
            Gaussian_X = -1; // error value
            return;
        }

        Gaussian_lambda	= Gaussian_lam;
        Gaussian_beta	= Gaussian_bet;

        if( bSeedFromClock )
        {
            seedValue0 = fastSeed();
            seedValue1 = seedValue0/7;
        }
        else
        {
            seedValue0 = rvSeed0;
            seedValue1 = rvSeed1;
        }

        Gaussian_U1.reset(seedValue0);
        Gaussian_U1.setRange(0,1);
        Gaussian_U2.reset(seedValue1);
        Gaussian_U2.setRange(0,1);

        Gaussian_V = DBL_MAX; // in float.h

        if( (Gaussian_lambda/Gaussian_beta) > (0.25 * sqrt(Gaussian_V)) )
        {
            Gaussian_X = -1;
            return;
        }

        Gaussian_mx = ((Gaussian_lambda - 1.0) + sqrt( pow( (Gaussian_lambda - 1.0), 2.0 ) + pow(Gaussian_beta, 2.0) ) / Gaussian_beta);
        Gaussian_my = (( (-1.0 * Gaussian_lambda) - 1.0) + sqrt( pow( (Gaussian_lambda + 1.0), 2.0 ) + pow(Gaussian_beta, 2.0) ) / Gaussian_beta);//mfftpt+io-

        double tempA1 = pow( (Gaussian_mx * Gaussian_my), ((-1.0 * Gaussian_lambda)/2.0) );
        double tempA2 = pow( (Gaussian_mx/Gaussian_my), 0.5 );
        double tempA3 = exp( (0.25 * Gaussian_beta) * (Gaussian_mx + pow(Gaussian_mx, -1.0) - Gaussian_my - pow(Gaussian_my, -1.0)) );

        Gaussian_A	= tempA1 * tempA2 * tempA3;

        Gaussian_C = -0.5*(Gaussian_lambda - 1.0) * log(Gaussian_mx) + (0.25 * Gaussian_beta * (Gaussian_mx + pow(Gaussian_mx, -1.0)));
    }

private:
    double Gaussian_A, Gaussian_C, Gaussian_beta, Gaussian_lambda, Gaussian_mx, Gaussian_my, Gaussian_V, Gaussian_X;
    unsigned int seedValue0, seedValue1;
    klUniformRV<double> Gaussian_U1;
    klUniformRV<double> Gaussian_U2;

    double gaussian()
    {
        double Gaussian_R1, Gaussian_R2;

        do
        {
            //	generate Gaussian_R1;
            Gaussian_R1 = Gaussian_U1();
            //	generate Gaussian_R2;
            Gaussian_R2 = Gaussian_U2();

            Gaussian_X = (Gaussian_A * Gaussian_R2)/ Gaussian_R1;
        } while( log(Gaussian_R1) > ((0.5 * (Gaussian_lambda -1) * log(Gaussian_X) - (0.25 * Gaussian_beta) * ( Gaussian_X + pow(Gaussian_X, -1.0) ) + Gaussian_C)) );

        return Gaussian_X;
    }
};




// mu > -lnU, where U = machine's smallest positive real number
// Probability Mass Function = fx(x) = (mu^(x) * e^(-mu))/x!, where (X>=0)
// Dagpunar, Principles of Random Variate Generation, Clarendon Press 1988
template<class TYPE> class klPoissonRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klPoissonRV( double Poi_m = 0.50, bool bSeedFromClock = true, unsigned int rvSeed = 1)
    {
        Poisson_X=0;
        klPoissonRVReset(bSeedFromClock, Poi_m, rvSeed);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        if( Poisson_X != -1 )
        {
            poisson();
            TYPE dummy=(TYPE)Poisson_X;
            return dummy;
        }
        else
        {
            cout << "Problem with klPoissonRV." << endl;
            cout << "The input value \"mu\" must be <= -log(DBL_EPSILON)!" << endl;
            cout << "The input value \"mu\" must be >0!" << endl;
            cout << "The input value \"mu\" must be <=38!" << endl;
            TYPE dummy=Poisson_X;
            return dummy;
        }
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klPoissonRVReset(bool bSeedFromClock = true, double Poi_m = 0.50, unsigned int rvSeed = 1)
    {
        Poisson_mu = Poi_m;

        if( bSeedFromClock )
        {
            seedValue = fastSeed();
        }
        else
        {
            seedValue = rvSeed;
        }

        Poisson_U1.reset(seedValue);
        Poisson_U1.setRange(0,1);	

        Poisson_U=DBL_EPSILON;  //from float.h	

        if(Poisson_mu > -log(Poisson_U) || Poisson_mu <=0 || Poisson_mu > 38) //error
        {
            Poisson_X = -1;
            return; //indicates error since posisson distribution is non-negative
        }

        Poisson_W=exp ( -Poisson_mu);
    }

private:
    double Poisson_W, Poisson_mu, Poisson_X, Poisson_U;
    unsigned int seedValue;
    klUniformRV<double> Poisson_U1;

    double poisson()
    {
        //	generate Poisson_R1;
        double Poisson_R1 = Poisson_U1();

        Poisson_X=0;

        double Poisson_r=Poisson_W;

        do{
            Poisson_R1=Poisson_R1-Poisson_r;

            Poisson_X=Poisson_X+1;

            Poisson_r=(Poisson_r*Poisson_mu)/Poisson_X;

        }while(Poisson_r < Poisson_R1);
        return Poisson_X;
    }
};


//	Density			f(x)						F(x)			X=F^-1(U)
//	Pareto-k,j      k*j^k/x^(k+1) x>=j>0		1-(j/x)^k		j/(1-U)^(1/k)
// Devorye, Non-Uniform Random Variate Generation, Springer-Verlag 1986
template<class TYPE> class klParetoRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klParetoRV(double dk = 0.50, double dj = 0.75, bool bSeedFromClock = true, unsigned int rvSeed = 1 )
    {
        klParetoRVReset(bSeedFromClock, dk, dj, rvSeed);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        pareto();
        TYPE dummy =Pareto_X;
        return dummy;
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klParetoRVReset(bool bSeedFromClock = true, double dk = 0.50, double dj = 0.75, unsigned int rvSeed = 1)
    {
        Pareto_k = dk;
        Pareto_j = dj;

        if( bSeedFromClock )
        {
            seedValue = fastSeed();
        }
        else
        {
            seedValue = rvSeed;
        }

        Pareto_R1.reset(rvSeed);
        Pareto_R1.setRange(0,1);
    }

private:
    klUniformRV<double> Pareto_R1;
    double Pareto_X, Pareto_k, Pareto_j;
    unsigned int seedValue;

    void pareto()
    {
        //check j>=0
        Pareto_X=Pareto_j/pow(Pareto_R1(),(1/Pareto_k));
    }
};





//	Density				f(x)					F(x)			X=F^-1(U)
//	exponential-k		k*exp(-k*x) x>0			1-exp(1k*x)		-1/k * ln(1-U)
// Devorye, Non-Uniform Random Variate Generation, Springer-Verlag 1986
template<class TYPE> class klExponentialRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klExponentialRV( double dk = 0.50, bool bSeedFromClock = true, unsigned int rvSeed = 1)
    {
        klExponentialRVReset(bSeedFromClock, dk, rvSeed);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        exponential();
        TYPE dummy=Exponential_X;
        return dummy;
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klExponentialRVReset(bool bSeedFromClock = true, double dk = 0.50, unsigned int rvSeed = 1)
    {
        Exponential_k = dk;

        if( bSeedFromClock )
        {
            seedValue = fastSeed();
        }
        else
        {
            seedValue = rvSeed;
        }

        Exponential_R1.reset(rvSeed);
        Exponential_R1.setRange(0,1);
    }

private:
    double Exponential_X, Exponential_k;
    unsigned int seedValue;

    klUniformRV<double> Exponential_R1;
    void exponential()
    {
        Exponential_X= -1/Exponential_k * log(Exponential_R1());
    }
};




//	Density			f(x)				F(x)				X=F^-1(U)
//	cauchy-k		k/[pi(x^2+k^2)]		1-exp(-x^2/2*k^2)	k*sqrt(-ln((1-U))
// Devorye, Non-Uniform Random Variate Generation, Springer-Verlag 1986
template<class TYPE>
class klCauchyRV : public klMersenneTwister, public klRV<TYPE>
{
public:
    klCauchyRV(double dk = 0.50, bool bSeedFromClock = true, unsigned int rvSeed = 1)
    {
        klCauchyRVReset(bSeedFromClock, dk, rvSeed);
    }

    // Operator that returns the next value in the distribution
    virtual TYPE operator()()
    {
        cauchy();
        TYPE dummy=Cauchy_X;
        return dummy;
    }

    // Method used to reset the RNG if a setup error occurs or if one desires to
    // reuse the existing RNG object with a different seed
    void klCauchyRVReset(bool bSeedFromClock = true, double dk = 0.50, unsigned int rvSeed = 1)
    {
        Cauchy_k = dk;

        if( bSeedFromClock )
        {
            seedValue = fastSeed();
        }
        else
        {
            seedValue = rvSeed;
        }

        Cauchy_R1.reset(rvSeed);
        Cauchy_R1.setRange(0,1);
    }

private:
    double Cauchy_X, Cauchy_k;
    unsigned int seedValue;
    klUniformRV<double> Cauchy_R1;

    void cauchy()
    {
        Cauchy_X=Cauchy_k*sqrt(-log(Cauchy_R1() ));
    }
};

//template<class TYPE>
//class klLevyAlphaStableRV : public klMersenneTwister, public klRV<TYPE>
//{
//public:
//    klLevyAlphaStableRV(double c=1, double alpha=2,bool bSeedFromClock = true, unsigned int seedValue = 1)
//    {
//        klLevyAlphaStableRVReset(bSeedFromClock,c,alpha,seedValue);
//    }
//    ~klLevyAlphaStableRV()
//    {
//        gsl_rng_free(gsl_RandObj);
//    }
//
//
//    // Operator that returns the next value in the distribution
//    virtual TYPE operator()()
//    {
//        LevyAlphaStableSample();
//        TYPE dummy=LevyAlphaStable_X;
//        return dummy;
//    }
//
//    // Method used to reset the RNG if a setup error occurs or if one desires to
//    // reuse the existing RNG object with a different seed
//    void klLevyAlphaStableRVReset(bool bSeedFromClock = true, double c=0,double alpha=0,unsigned int seedValue = 1)
//    {
//        if(alpha<0 || alpha>2)
//            throw "klLevySkewAlphaStableRV::klLevyAlphaStableRVReset (bool bSeedFromClock = true, double c=0,double alpha=0,unsigned int seedValue = 1) ERROR: bad parameters for pdf.";
//
//        if( bSeedFromClock )
//        {
//            _seedValue = fastSeed();
//
//        }
//        else
//        {
//            _seedValue= seedValue;
//        }
//
//        _c = c;
//        _alpha = alpha;
//
//        const gsl_rng_type * T;
//
//        gsl_rng_env_setup();
//
//        T = gsl_rng_default;
//
//        gsl_RandObj = gsl_rng_alloc (T);
//
//        gsl_rng_set(gsl_RandObj,_seedValue);
//
//
//
//
//
//    }
//
//private:
//    gsl_rng * gsl_RandObj;
//    double _c;
//    double _alpha;
//    double LevyAlphaStable_X;
//    unsigned int _seedValue;
//    void LevyAlphaStableSample()
//    {
//        LevyAlphaStable_X= gsl_ran_levy (gsl_RandObj,  _c,_alpha);
//    }
//};
//
//
//
//
//
//
//template<class TYPE>
//class klLevySkewAlphaStableRV : public klMersenneTwister,public  klRV<TYPE>
//{
//public:
//    klLevySkewAlphaStableRV(double c=1,double alpha=2,double beta=0,bool bSeedFromClock = true, unsigned int seedValue = 1)
//    {
//        klLevySkewAlphaStableRVReset(c,alpha,beta,bSeedFromClock,seedValue);
//    }
//    ~klLevySkewAlphaStableRV()
//    {
//        gsl_rng_free(gsl_RandObj);
//    }
//
//    // Operator that returns the next value in the distribution
//    virtual TYPE operator()()
//    {
//        LevySkewAlphaStableSample();
//        TYPE dummy=LevySkewAlphaStable_X;
//        return dummy;
//    }
//
//    // Method used to reset the RNG if a setup error occurs or if one desires to
//    // reuse the existing RNG object with a different seed
//    void klLevySkewAlphaStableRVReset(double c=0,double alpha=0,double beta=0,bool bSeedFromClock = true, unsigned int seedValue = 1)
//    {
//        if(beta<-1 ||beta>1 ||alpha<0||alpha>2)
//            throw "klLevySkewAlphaStableRVReset(double c=0,double alpha=0,double beta=0,bool bSeedFromClock = true, unsigned int seedValue = 1) ERROR:  bad parameters for pdf.";
//
//
//        if( bSeedFromClock )
//        {
//            _seedValue = fastSeed();
//
//        }
//        else
//        {
//            _seedValue= seedValue;
//        }
//
//        _c=c;
//        _alpha=alpha;
//        _beta=beta;
//        const gsl_rng_type * T;
//        gsl_rng_env_setup();
//        T = gsl_rng_default;
//        gsl_RandObj = gsl_rng_alloc (T);
//
//        gsl_rng_set(gsl_RandObj,_seedValue);
//
//
//
//
//    }
//
//private: 
//    gsl_rng * gsl_RandObj;
//    double _c;
//    double _alpha;
//    double _beta;
//    double LevySkewAlphaStable_X;
//    unsigned int _seedValue;
//    void LevySkewAlphaStableSample()
//    {
//        LevySkewAlphaStable_X=gsl_ran_levy_skew (gsl_RandObj,_c, _alpha,_beta);
//    }
//};
//

#endif //__kl_random_number_generator__        