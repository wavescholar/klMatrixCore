#ifndef __kl_time_series__
#define __kl_time_series__

#include "kl_matrix.h"
#include "kl_stat.h"  //Provides basic descriptive statisics for time series.

using namespace KL_stat;


template<class TYPE> class klTimeSeries : public klVector< TYPE>
{
public:

    klTimeSeries() : _mean(0),_variance(0), _skewness(0), _kurtosis(0) ,_statsCalculated(false), _viema(NULL) ,_viemaSize(0)
    { }


    klTimeSeries(klVector<TYPE> a) : klVector<TYPE>(a), _mean(0), 
        _variance(0), _skewness(0), _kurtosis(0) ,_statsCalculated(false), _viema(NULL) ,_viemaSize(0)
    { }

    klTimeSeries(unsigned int size) : klVector<TYPE>(size),  _mean(0), 
        _variance(0), _skewness(0), _kurtosis(0) ,_statsCalculated(false), _viema(NULL), _viemaSize(0)
    { }

    klTimeSeries(klTimeSeries<TYPE> &src) : klVector<TYPE>(src), _ema(src._ema), _ma(src._ma), _diff(src._diff) 
    {
        _mean= src._mean;

        _variance=src._variance;

        _skewness=src._skewness;

        _kurtosis=src._kurtosis;

        _statsCalculated= src._statsCalculated;

        _viemaSize=src._viemaSize;

        _viema=new klVector<TYPE>[_viemaSize];

        unsigned int i;

        for(i=0;i<_viemaSize;i++)
        {
            *(_viema+i)= *(src._viema +i);
        }

    }



    ~klTimeSeries()
    {
        if(_viema)
            delete[] _viema; 
    }
    //Calculate the descriptive statistics.
    virtual void calcDescriptiveStats()
    {
        unsigned int size=getRowSize();
        TYPE* data=getMemory();
        _mean= KL_stat::mean(data,size);
        _variance= KL_stat::variance(data,size,_mean);
        _skewness= KL_stat::skewness(data,size,_mean, _variance); 
        _kurtosis= KL_stat::kurtosis(data,size,_mean, _variance);
        _statsCalculated=true;
    }


    TYPE mean() 
    {
        if(_statsCalculated==true) return _mean;
        else
        {
            calcDescriptiveStats();
            _statsCalculated=true;
            return _mean;
        }
    }

    TYPE variance() 
    {
        if(_statsCalculated==true) return _variance;
        else
        {
            calcDescriptiveStats();
            _statsCalculated=true;
            return _variance;
        }
    }

    TYPE skewness() 
    {
        if(_statsCalculated==true) return _skewness;
        else
        {
            calcDescriptiveStats();
            _statsCalculated=true;
            return _skewness;
        }
    }


    TYPE kurtosis() 
    {
        if(_statsCalculated==true) return _kurtosis;
        else
        {
            calcDescriptiveStats();
            _statsCalculated=true;
            return _kurtosis;
        }
    }

    //virtual TYPE autoCorrelation(unsigned int k)
    //{
    //
    //}
    //virtual TYPE autoCovariance(unsigned int k)
    //{

    //}

    //bbc revisit - preparing for inhomogeneous time series
    enum klTimeSeriesInterpolation{PREVIOUS,LINEAR,NEXT};

    //Computes the Exponential Moving average up to time k 
    virtual klVector<TYPE> EMA(unsigned int k,  TYPE tau, klTimeSeriesInterpolation interp=PREVIOUS,bool doiema=false, klVector<TYPE> iema=0)
    {

        _ema.setup(getRowSize());

        unsigned int i;
        TYPE alpha=1/tau; 
        TYPE mu=exp(-alpha);
        TYPE upsilon;
        switch (interp)
        {
        case klTimeSeriesInterpolation::LINEAR :
            {
                upsilon=(1-mu)/alpha;               
                break;
            }
        case klTimeSeriesInterpolation::NEXT :
            {
                upsilon=mu;
                break;
            }
        case klTimeSeriesInterpolation::PREVIOUS :
            {
                upsilon=1;
                break;
            }
        default :
            {
                throw "klTimeSeries::EMA() ERROR called with invalid klTimeSeriesInterpolation";
            }
        }//end switch       
        for(i=0;i<k;i++)
        {

            if(!doiema)
            {
                if(i==0)
                {
                    _ema[0]=(1-upsilon)*this->operator [](0);
                }
                else if(i==1)
                {
                    _ema[1]=(upsilon-mu)*this->operator [](0)+(1-upsilon)*this->operator[](1);
                }
                else
                    _ema[i]=advanceEMA(alpha,mu,upsilon,tau,_ema[i-1],this->operator[](i-1),this->operator[](i));

            }
            else
            {
                if(i==0)
                {
                    _ema[0]=(1-upsilon)*iema[0];
                }
                else if(i==1)
                {
                    _ema[1]=(upsilon-mu)*iema[0]+(1-upsilon)*iema[1];
                }
                else
                    _ema[i]=advanceEMA(alpha,mu,upsilon,tau,_ema[i-1],iema[i-1],iema[i]);


            }




        }//end ema loop
        return _ema;
    }
    //Computes the Iterated Exponential Moving Average up to time k-1
    klVector<TYPE> IEMA(unsigned int k, unsigned int n, TYPE tau, klTimeSeriesInterpolation interp=PREVIOUS)
    {
        if(n==1)
        {
            return EMA(k,tau,interp);
        }
        else
        {
            return EMA(k,tau,interp,true,IEMA(k,--n,tau,interp));
        }


    }

    //require storage of three iema sets, 1,2,4 deep respectively
    //Computes differential via
    //gamma ( EMA[alpha,tau] + IEMA[alpga tau,2]- 2 IEMA[alpha beta tau, 4] )
    klVector<TYPE> DIFF(unsigned int k,TYPE gamma, TYPE beta, TYPE alpha,TYPE tau,klTimeSeriesInterpolation interp=PREVIOUS)
    {
        _diff.setup(getRowSize() );

        //bbc revisit - tau must be consistent within an instance
        //bool localCopy;
        klVector<TYPE> _diff1=EMA(k, alpha * tau,interp);  //bbc revisit - this steps on local copy _ema - impl no step variable on EMA and IEMA
        klVector<TYPE> _diff2=IEMA(k,2,alpha * tau,interp);//another problem - we need to store the 2 imeas for advanceDifferential method
        klVector<TYPE> _diff4=IEMA(k,4,alpha * beta * tau,interp);                                   // 4 imeas for this component

        //reset the first four _viema elements 
        IEMA(k,4,tau,interp);

        unsigned int i;
        for(i=0;i<k;i++)
        {
            _diff[i]=gamma * (_diff1[i] + _diff2[i]- 2 * _diff4[i] );
            //cout<<_diff1[i]<<" " << _diff2[i]<< _diff4[i] );
        }
        return _diff;



    }


    //Moving average using average of iterated exponential moving averages

	//L^p moving norm
	klVector<TYPE> LPNORM(unsigned int k,unsigned int n,TYPE tau,klTimeSeriesInterpolation interp=PREVIOUS)
	{
		klVector<TYPE> ma;
        ma.setup(getRowSize() );
        unsigned int i,j;
        _viema=new klVector<TYPE>[n];
        _viemaSize=n;
        for(i=1;i<=n;i++)
        {
            *(_viema+i-1)=IEMA(k,i,tau,interp);
        }
	}


    klVector<TYPE> MA(unsigned int k,unsigned int n,TYPE tau,klTimeSeriesInterpolation interp=PREVIOUS)
    {
        TYPE tau_scaled=2*tau/(n+1);
        klVector<TYPE> ma;
        ma.setup(getRowSize() );
        unsigned int i,j;
        _viema=new klVector<TYPE>[n];
        _viemaSize=n;
        for(i=1;i<=n;i++)
        {
            *(_viema+i-1)=IEMA(k,i,tau,interp);
        }
        for(j=0;j<k;j++)
        {
            TYPE temp=0;
            for(i=1;i<=n;i++)
            {
                temp+=(_viema+i-1)->operator [](j);
            }
            ma[j]=temp/n;   
        }
        _ma=ma;//store
        return ma;

    }   

    //Advances Exponential Moving Average
    TYPE advanceEMA(TYPE alpha, TYPE mu, TYPE upsilon, TYPE tau,TYPE ema_i_minus_1, TYPE z_i_minus_one ,TYPE z_i)
    {
        TYPE temp=mu*ema_i_minus_1+(upsilon-mu)*z_i_minus_one+(1-upsilon)*z_i;
        return temp;
    }

    //Advances EMA from step k
    TYPE advanceEMA(unsigned int k,TYPE tau, TYPE ema_k_minus_1,klTimeSeriesInterpolation interp=PREVIOUS)
    {
        TYPE alpha=1/tau; 
        TYPE mu=exp(-alpha);
        TYPE upsilon;
        switch (interp)
        {
        case klTimeSeriesInterpolation::LINEAR :
            {
                upsilon=(1-mu)/alpha;               
                break;
            }
        case klTimeSeriesInterpolation::NEXT :
            {
                upsilon=mu;
                break;
            }
        case klTimeSeriesInterpolation::PREVIOUS :
            {
                upsilon=1;
                break;
            }
        default :
            {
                throw "klTimeSeries::EMA() ERROR: called with invalid klTimeSeriesInterpolation";
            }
        }//end switch
        TYPE temp =mu*ema_k_minus_1+(upsilon-mu)*this->operator[](k-1)+(1-upsilon)*this->operator[](k);
        _ema[k]=temp;//store results
        return temp;
    }



    //This method uses the recursion
    //IEMA(tau,n)[i]=mu IEMA(tau,n)[i-1]+(upsilon-mu) IEMA(tau,n-1)[i-1] + (1-upsilon) IEMA(tau,n-1)[i]
    //where IEMA(tau,1)=EMA(tau)
    //Call returns a pair (IEMA(tau,n)[i],IEMA(tau,n-1)[i])
    pair<TYPE,TYPE> advanceIEMA(unsigned int k, unsigned int n,TYPE tau,TYPE iema_k_minus_1,klTimeSeriesInterpolation interp=PREVIOUS)
    {           

        if(n==1)
            return pair<TYPE,TYPE>(advanceEMA(k,tau,interp),iema_k_minus_1);
        else
        {

            pair<TYPE,TYPE> ret= advanceIEMA(k,--n,tau,iema_k_minus_1);
            TYPE alpha=1/tau; 
            TYPE mu=exp(-alpha);
            TYPE upsilon;
            switch (interp)
            {
            case klTimeSeriesInterpolation::LINEAR :
                {
                    upsilon=(1-mu)/alpha;               
                    break;
                }
            case klTimeSeriesInterpolation::NEXT :
                {
                    upsilon=mu;
                    break;
                }
            case klTimeSeriesInterpolation::PREVIOUS :
                {
                    upsilon=1;
                    break;
                }
            default :
                {
                    throw "klTimeSeries::EMA() ERROR: called with invalid klTimeSeriesInterpolation";
                }
            }//end switch       
            TYPE temp=mu *iema_k_minus_1 + (upsilon-mu)*ret.second + (1-upsilon)*ret.first;
            //bbc revisit - store results in (_viema+n) - this should be coordinated with MA methods since they update
            return pair<TYPE,TYPE>(temp,ret.first);
        }
    }


    TYPE advanceMA(unsigned int k, unsigned int n,TYPE tau,klTimeSeriesInterpolation interp=PREVIOUS)
    {           

        TYPE tau_scaled=2*tau/(n+1);
        TYPE ma;
        unsigned int i,j;
        TYPE* ad_viema=new TYPE[n];
        for(i=1;i<=n;i++)
        {

            pair<TYPE,TYPE> ret=advanceIEMA(k,i,tau,(_viema+i-1)->operator[](k-1),interp);//bbcrevisit - inefficiency - store the results of the recurrsion in advanceIEMA for largest recurse
            *(ad_viema+i-1)=ret.first;

            //store advance in _viema
            (_viema+i-1)->operator [](k)=*(ad_viema+i-1);
        }
        for(j=0;j<k;j++)
        {
            TYPE temp=0;
            for(i=1;i<=n;i++)
            {
                temp+=*(ad_viema+i-1);
            }
            ma=temp/n;  
        }
        _ma[k]=ma;//sore results
        return ma;

    }

protected:
    //Sample Descriptive statistics
    TYPE _mean;
    TYPE _variance;
    TYPE _skewness;
    TYPE _kurtosis;

    //Exponential Moving Average
    klVector<TYPE> _ema;

    //Iterated exponentials for MA and other operators
    klVector<TYPE>* _viema;

    size_t _viemaSize;

    //Moving Average
    klVector<TYPE> _ma;

    //Differential
    klVector<TYPE> _diff;

    bool _statsCalculated;

};




#endif //__kl_time_series__
