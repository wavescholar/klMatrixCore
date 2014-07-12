 /*******************************
 * Copyright (c) <2007>, <Bruce Campbell> All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *  
 * Bruce B Campbell 07 08 2014  *
 ********************************/

#ifndef __kl_wavelet__
#define __kl_wavelet__

#include "mkl.h"
#include "kl_stat.h"
#include "kl_matrix.h"

using namespace std;

#include "mkl_dfti.h" 
//Discrete Fourier Transform for 1-D signal.
//This uses the mkl flavor of fft
//There is also an fft in the ipps 
template <class TYPE> class klDFT // : public klMatrix<TYPE>
{
public:

	//The weight is used for positions and baskets.
	klDFT(klVector<TYPE> signal)
	{
		_signal=signal;
	}

	//float and double supported 
	klVector<TYPE> operator()()
	{
		//MKL fft functions support mixed-redix transforms for lengths of
		//other than powers of 2.

		//We always produce the complex transform.  
		//bbcrevisit  - put an enum in the algorithm to 
		//select real or complex ouput

		klVector<TYPE> spectrum(_signal.getRows());
		//TYPE* spectrum=new TYPE[_signal.getRows()];

		unsigned int i;
		for(i=0;i<_signal.getRows();i++)
		{
			spectrum[i]=_signal[i];//bbcrevisit 
		}


		//This is mkl fft code - here we just demonstrate 
		//deconstruct and reconstruct the signal
		//bbcrevisit - replace this with the fft functionlity of ippps
		DFTI_DESCRIPTOR *my_desc1_handle, *my_desc2_handle;
		/* .... or alternatively
		DFTI_DESCRIPTOR_HANDLE my_desc1_handle, my_desc2_handle; */
		long status;

		//DFTI_DOUBLE | DFTI_SINGLE 
		//DFTI_COMPLE | DFTI_REAL

		if( typeid(float)==typeid(TYPE) )
			status = DftiCreateDescriptor( &my_desc1_handle, DFTI_SINGLE,DFTI_REAL, 1, _signal.getRows());
		else if (  typeid(double)==typeid(TYPE) )
			status = DftiCreateDescriptor( &my_desc1_handle, DFTI_DOUBLE,DFTI_REAL, 1, _signal.getRows());

		status = DftiCommitDescriptor( my_desc1_handle );
		status = DftiComputeForward( my_desc1_handle, spectrum.getMemory() );

		klVector<TYPE> inverseTransform(_signal.getRows()); 

		//This is where the filter goes.

		inverseTransform=spectrum;

		if( typeid(float)==typeid(TYPE) )
			status = DftiCreateDescriptor( &my_desc2_handle, DFTI_SINGLE,DFTI_REAL, 1, _signal.getRows());
		else if (  typeid(double)==typeid(TYPE) )
			status = DftiCreateDescriptor( &my_desc2_handle, DFTI_DOUBLE,DFTI_REAL, 1, _signal.getRows());
		char * errmsg;
		errmsg=DftiErrorMessage( status );
		status = DftiCommitDescriptor( my_desc2_handle);
		status = DftiComputeForward( my_desc2_handle, inverseTransform.getMemory());

		//status =  DftiSetValue( my_desc2_handle,DFTI_BACKWARD_SCALE,1  );  //Num dims in data - 
		//	status =  DftiGetValue( my_desc2_handle, DFTI_BACKWARD_SCALE, Config_Val )

		status = DftiComputeBackward( my_desc2_handle, inverseTransform.getMemory());
		/* result is given in CCS format */

		errmsg=DftiErrorMessage( status );

		//bbcrevisit setting the scale does not seem to work, we get a bad config err
		//from mkl
		for(i=0;i<inverseTransform.getRows();i++)
		{
			inverseTransform[i]=inverseTransform[i]/inverseTransform.getRows();
		}


		//Mkl deleted the memory passed into the descriptor
		//We need to tell kl not to delete it.
		spectrum.setOwn(false);
		inverseTransform.setOwn(false);

		status = DftiFreeDescriptor(&my_desc1_handle);
		status = DftiFreeDescriptor(&my_desc2_handle);

		return spectrum; 
	}

protected:

	klVector<TYPE> _signal;

};	

#include "ipp.h"
template <class TYPE> class klDWT 
{
public:

	klDWT(klVector<TYPE> signal)
	{
		_signal=signal;
	}

	klMatrix<TYPE> operator()()
	{
		{//Version a
			//IppStatus ippiWTFwdInitAlloc_32f_C1R (IppiWTFwdSpec_32f_C1R** pSpec,
			//	const Ipp32f* pTapsLow, int lenLow, int anchorLow,
			//	const Ipp32f* pTapsHigh, int lenHigh, int anchorHigh);

			IppiWTFwdSpec_32f_C1R *spec=NULL;
			int lenLow=31;
			int anchorLow=16;
			int lenHigh =31;
			int anchorHigh=16;	
			IppStatus  ippStatusResult;
			//Mexican Hat wavelet.
			//dmey subsampled from Matlab - why does  Clev have 104tap filt padded by 35 zeros?
			const Ipp32f pTapsHigh[31] =
			{
				-0.0008595,	-0.00057819,	0.0027022,	0.0021948,	-0.0060455,
				-0.0063867,	0.011045,	0.015251,	-0.017404,	-0.032094,	
				0.024322,	0.063667,	-0.030621,	-0.1327,	0.035048,
				0.4441,	-0.74375,	0.4441,	0.035048,	-0.1327,	-0.030621,
				0.063667,	0.024322,	-0.032094,	-0.017404,	0.015251,	0.011045,
				-0.0063867,	-0.0060455,	0.0021948,	0.0027022
			};
			const Ipp32f pTapsLow[31] =
			{
				0.00016119,	0.0008595,	-0.00057819,	-0.0027022,	0.0021948,
				0.0060455,	-0.0063867,	-0.011045,	0.015251,	0.017404,
				-0.032094,	-0.024322,	0.063667,	0.030621,	-0.1327,
				-0.035048,	0.4441,	0.74375,	0.4441,	-0.035048,	-0.1327,
				0.030621,	0.063667,	-0.024322, 0.032094,	0.017404,
				0.015251,	-0.011045,	-0.0063867,	0.0060455,	0.0021948
			};

			//These are in ippi - for 2-d image processing - 
			//They will later be used to do VWAP signal processing 

			//Bbcrevisit note - to do the VWAP - put 2d price/volume into matrix
			//on an asset and do the analysis in a separate module that contains all of the image processing
			//capabilities.

			//  ippStatusResult=ippiWTFwdInitAlloc_32f_C1R(&spec,    pTapsLow,  lenLow, anchorLow,  pTapsHigh, lenHigh, anchorHigh);
			//   if(!spec) throw "klDWT :ipp error";
			//int bufSize;
			//ippiWTFwdGetBufSize_C1R(spec, &bufSize);
			//unsigned char* ippworkbuffer = ippsMalloc_8u(bufSize);
			//if(!ippworkbuffer)
			//{
			//	ippiWTFwdFree_32f_C1R(spec);     
			//	throw "klDWT :ipp memory error";
			//}
		}//

		{//Version B - iips 1-d mra


			//Filter bank for Daubechies, order 2
			//Ref Intro Wavelets and Other Filtering Methods In Finance and Economics. R Gencay,  F Selcuk B Whitcher
			//P114
			static const int fwdFltLenL = 4;
			static const int fwdFltLenH = 4;
			static const float fwdFltL[4] =
			{
				-1.294095225509215e-001f,
				2.241438680418574e-001f,
				8.365163037374690e-001f,
				4.829629131446903e-001f
			};
			static const float fwdFltH[4] =
			{
				-4.829629131446903e-001f,
				8.365163037374690e-001f,
				-2.241438680418574e-001f,
				-1.294095225509215e-001f
			};

			static const int invFltLenL = 4;
			static const int invFltLenH = 4;
			static const float invFltL[4] =
			{
				4.829629131446903e-001f,
				8.365163037374690e-001f,
				2.241438680418574e-001f,
				-1.294095225509215e-001f
			};
			static const float invFltH[4] =
			{
				-1.294095225509215e-001f,
				-2.241438680418574e-001f,
				8.365163037374690e-001f,
				-4.829629131446903e-001f
			};
			// minimal values
			static const int fwdFltOffsL = -1;
			static const int fwdFltOffsH = -1;
			// minimal values, that corresponds to perfect reconstruction
			static const int invFltOffsL = 0;
			static const int invFltOffsH = 0;
			//We convert the signal to float - ususally if there's a 32bit float impl with mkl/ipp then there
			//is also a 64bit float version too...apparantly not as of ipp5/mkl8.  Soo we do all our work with 32f

			klVector<float> fsignal(_signal.getRows());

			klVector<float> low(_signal.getRows()/2);

			klVector<float> high(_signal.getRows()/2);

			klMatrix<float> all(_signal.getRows(),_signal.getRows());

			unsigned int i;
			for(i=0;i<_signal.getRows();i++)
			{
				fsignal[i]=-_signal[i];
			}

			Ipp32f* src=fsignal.getMemory();


			{
				IppsWTFwdState_32f* state;
				ippsWTFwdInitAlloc_32f (&state, fwdFltL, fwdFltLenL, fwdFltOffsL,fwdFltH, fwdFltLenH, fwdFltOffsH);
				// We substitute wrapping extension in "the beginning of stream"
				// Here should be the same pointers for this offsets,
				// but in the general case it may be different

				//This gives us a mirror padding scheme for the boundary elements?
				float* pI=src;
				float* pE=src+fsignal.getRows();
				ippsWTFwdSetDlyLine_32f( state, pI,pE);

				// Forward transform
				ippsWTFwd_32f (src, low.getMemory(), high.getMemory(), 6, state);

				ippsWTFwdFree_32f (state);
			}



			{//Reconstruct

				klVector<float> reconstruction(_signal.getRows());

				Ipp32f* dst=reconstruction.getMemory();

				IppsWTInvState_32f* state;
				ippsWTInvInitAlloc_32f (&state,
					invFltL, invFltLenL, invFltOffsL,
					invFltH, invFltLenH, invFltOffsH);
				// For this particular case (non-shifted reconstruction)
				// here is first data itself,
				// that we need to place to delay line
				// [(invFltLenL + invFltOffsL - 1) / 2] elements for l. filtering
				// [(invFltLenH + invFltOffsH - 1) / 2] elements for h. filtering

				//ippsWTInvSetDlyLine_32f( state, low, high);
				ippsWTInvSetDlyLine_32f( state, low.getMemory(), high.getMemory());

				// Inverse transform
				//ippsWTInv_32f (&low[1], &high[1], 5, dst, state);
				ippsWTInv_32f (low.getMemory(), high.getMemory(), 5, dst, state);


				// Here are the substitution of the wrapping extension
				// at the "end of stream" and calculation of last samples of reconstruction
				// We do not use additional buffer and do not copy any data externally,
				// just substitute beginning of input data itself to simulate wrapping


				//ippsWTInv_32f (low, high, 1, &dst[10], state);

				float* offset= reconstruction.getMemory()+reconstruction.getRows()/2;
				ippsWTInv_32f (low.getMemory(), high.getMemory(), 1, offset, state);
				ippsWTInvFree_32f (state);

			}

		}//end version B

		//bbcrevisit
		return klMatrix<TYPE> (0,0);

	}

protected:

	klVector<TYPE> _signal;

};	

template<class TYPE> klVector<TYPE> testKLDFT(klVector<TYPE> signal )
{

	klDFT<TYPE> fft(signal);

	klVector<TYPE> spectrum=fft();

	return spectrum;
}

template<class TYPE> klMatrix<TYPE> testKLDWT(klVector<TYPE> signal)
{
	klDWT<TYPE> dwt(signal);

	klMatrix<TYPE> spectrum=dwt();

	return spectrum;
}

template<class TYPE> void testKLWavelet(ofstream &_tex)
{
	//ios_base::openmode wMode = ios_base::app;
	//ofstream _tex(fileName, wMode);	

	char* filename=new char[512];

	size_t popsize=1024*2;
	klVector<TYPE> signal(popsize);
	klNormalInverseApproxRV<TYPE> normalinv(0,0.025);

	unsigned i;
	for(i=0;i<popsize;i++)
	{
		TYPE pi= 3.141592653589793238462643383279502;
		signal[i]=sin(2*pi*(double(i)/popsize))+sin(3*pi*(double(i)/popsize))+sin(4*pi*(double(i)/popsize))+sin(5*pi*(double(i)/popsize));//normalinv()+ .5* sin(4*pi*(double(i)/popsize)) + 1* sin(7*pi*(double(i)/popsize) );
	}

	klVector<TYPE> dirtysignal(popsize);
	for(i=0;i<popsize;i++)
	{
		TYPE pi= 3.141592653589793238462643383279502;
		dirtysignal[i]=sin(2*pi*(double(i)/popsize))+sin(3*pi*(double(i)/popsize))+sin(4*pi*(double(i)/popsize))+sin(5*pi*(double(i)/popsize))+normalinv();//+ .5* sin(4*pi*(double(i)/popsize)) + 1* sin(7*pi*(double(i)/popsize) );
	}

	char* arg=new char[1024];

	{
		ofstream fileostreamobj("Wavelet_Signal.txt" );
		fileostreamobj<<signal<<endl;
		fileostreamobj.close();
	}
	
		
	{
		ofstream fileostreamobj("Wavelet_Signal_With_Noise.txt" );
		fileostreamobj<<dirtysignal<<endl;
		fileostreamobj.close();
	}

	klVector<TYPE> dftspec=testKLDFT<TYPE>(signal);
	klMatrix<TYPE> dwtspec=testKLDWT<TYPE> (signal);
	
	{
		ofstream fileostreamobj("Wavelet_Signal_Spectrum_DFT.txt" );
		fileostreamobj<<dftspec<<endl;
		fileostreamobj.close();
	}

	{		
		ofstream fileostreamobj("Wavelet_Signal_Spectrum_DWT.txt" );
		fileostreamobj<<dwtspec<<endl;
		fileostreamobj.close();
	}

	klVector<TYPE> dirtydftspec=testKLDFT<TYPE>(dirtysignal);
	klMatrix<TYPE> dirtydwtspec=testKLDWT<TYPE> (dirtysignal);

	{
		ofstream fileostreamobj("Wavelet_Noisy_Signal_Spectrum_DFT.txt" );
		fileostreamobj<<dirtydftspec<<endl;
		fileostreamobj.close();
	}


	{
		ofstream fileostreamobj("Wavelet_Noisy_Signal_Spectrum_DWT.txt" );
		fileostreamobj<<dirtydwtspec<<endl;
		fileostreamobj.close();
	}

	delete filename;
	
	_tex.flush();
	//_tex.close();

}


#endif //__kl_wavelet__
