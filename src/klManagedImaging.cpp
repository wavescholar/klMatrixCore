// This is the main DLL file.

#include "ipp.h"
#include "kl_image_buffer.h"
#include "klManagedImaging.h"

#include "kl_image_processing_functors.h"
#include "kl_img_pca.h"

void RunImageChain(klRasterBufferPointer in);
extern "C" { FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]}; }
__declspec(dllimport) int  errno;


void __clrcall klManagedImaging::klImageOp::Init(String^ filename, int w, int h)
{
	char* file = (char*)(void*)Marshal::StringToHGlobalAnsi(filename);

}

int __clrcall klManagedImaging::klImageOp::Operate(Bitmap^ img,String^ op)
{
	Size bmpSize= img->Size;
	const int width= img->Width;
	const int height=img->Height;
	System::Drawing::Imaging::PixelFormat format= img->PixelFormat;
	if( format ==System::Drawing::Imaging::PixelFormat::Format8bppIndexed)
	{
		System::Drawing::Imaging::ColorPalette^  cpal = img->Palette;
		array<Color>^  cEnt = cpal->Entries;
		for (int i = 0; i < 256; i++)
			cEnt[i] = System::Drawing::Color::FromArgb(i, i, i);
		img->Palette = cpal;
	}

	System::Drawing::Rectangle rect(0, 0,width, height);
	System::Drawing::Imaging::BitmapData^ bmd=img->LockBits(rect,System::Drawing::Imaging::ImageLockMode::ReadWrite, format);
	IntPtr data = bmd->Scan0;
	if( format ==System::Drawing::Imaging::PixelFormat::Format24bppRgb)
	{
		int bytes = img->Width * img->Height * 3;
		array<Byte>^rgbValues = gcnew array<Byte>(bytes);
		System::Runtime::InteropServices::Marshal::Copy( data, rgbValues, 0, bytes );


		// Copy the RGB values back to the bitmap
		System::Runtime::InteropServices::Marshal::Copy( rgbValues, 0, data, bytes );
		img->Save("c:/temp/ippManaged_ProcessedImage.jpg",	System::Drawing::Imaging::ImageFormat::Jpeg);
		img->UnlockBits( bmd);
	}


	if(format ==System::Drawing::Imaging::PixelFormat::Format1bppIndexed)
	{
		//Do something else
	}
	return 42;
}



//#include "kl_matrix.h"
//#include "kl_stat.h"
//#include "kl_random_number_generator.h"
//#include "kl_time_series.h"
//#include "kl_multivariate_random_variable.h"
//#include "kl_sample_population.h"
//#include "kl_principal_components.h"
//#include "kl_large_dataset.h"
//#include "kl_regression.h"
//#include "kl_multiclass_svm.h"
//#include "kl_wavelet.h"
//#include "kl_util.h"
//#include "kl_unit_tests.h"
//#include "kl_matlab_iface.h"
//#include "kl_matlab_dependent_unit_tests.h"
//#include "kl_image_processing_functors.h"
//const char* kl_base_dir="D://KL";
//const char* basefilename="//output//UnitTestOutput//";
extern "C" void  klRunMatlabWorkFlow(void);
int __clrcall klManagedImaging::klImageOp::OperateTile(klImageTile^ tile,String^ op)
{
	/*klThreadId thisThread=klThread<klMutex>::getCurrentThreadId();
	klMatlabEngineThreadMap klmtm;


	Engine* matlabEngine=NULL;
    if (!(matlabEngine = engOpen(NULL))) 
        throw "klWorkflow: no matlab engine available";

	klmtm.insert(thisThread,matlabEngine);*/

	//testklPrincipalComponentsMatlab<double> ("C://KL//output//UnitTestOutput//kl_pca");
	//klRunMatlabWorkFlow();

	array<Byte>^ rgbValues= tile->rgbValues;
	

	System::Console::WriteLine(tile->TileHeight);
	System::Console::WriteLine(tile->TileWidth);
	System::Console::WriteLine(tile->TileTop);
	System::Console::WriteLine(tile->TileLeft);

	int imageBufferOffset = 0;
	for (int roiY = 0; roiY < tile->TileHeight; roiY++)
	{
		for (int roiX = 0; roiX < tile->TileWidth; roiX++)
		{
			int r = rgbValues[imageBufferOffset + 0];
			int g = rgbValues[imageBufferOffset + 1];
			int b = rgbValues[imageBufferOffset + 2];
			imageBufferOffset += 3;
		}
	}
	
	char* identifier = (char*)(void*)Marshal::StringToHGlobalAnsi(tile->Identifier);
	char* arg=new char[1024];
	

	klRasterBufferPointer lsrc= new klIPPRasterBuffer<unsigned char >(tile->TileWidth, tile->TileHeight,3);
	imageBufferOffset = 0;
	size_t inbands=lsrc->numBands();
	size_t inwidth =lsrc->width();
	size_t inheight=lsrc->height(); 
	size_t inbandStride=lsrc->bandStride();
	size_t inxStride =lsrc->xStride();
	size_t inyStride= lsrc->yStride();

	unsigned int i;
	unsigned int j;
	unsigned int b;

	unsigned char* buf=lsrc->buffer();

	for (i=0; i<inheight; i++)
	{
		for (j=0; j<inwidth; j++)
		{
			for(b=0;b<inbands;b++)
			{
				*(buf+ i*inyStride + j*inxStride + b)=rgbValues[imageBufferOffset];
				imageBufferOffset+=1;
			}
		}
	}
	const klRasterBufferPointer ldst=new klPackedHeapRasterBuffer<unsigned char>(inwidth,inheight, inbands);
	int lband=-1;
	klCopyFunctor<unsigned char> klcf_u8(lsrc, ldst);
	klcf_u8();
	sprintf(arg,"%s_Input.ppm",identifier);
	write_ppm (arg, ldst->width(), ldst->height(),ldst->buffer());


	unsigned lkernelWidthDF=3;
	unsigned lkernelHeightDF=3;
	//Mask values. Only pixels that correspond to nonzero 
	//mask values are taken into account during operation.
	unsigned char* lkernelDF = new unsigned char[ lkernelWidthDF * lkernelHeightDF ];
	lkernelDF[0]=1;lkernelDF[1]=1;lkernelDF[2]=1;
	lkernelDF[3]=1;lkernelDF[4]=0;lkernelDF[5]=1;
	lkernelDF[6]=1;lkernelDF[7]=1;lkernelDF[8]=1;
	const klRasterBufferPointer lsrc_dilated=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	klCopyFunctor<unsigned char> klcfdf_u8(lsrc, lsrc_dilated);
	klcfdf_u8();//Do the copy
	klDilateFunctor< unsigned char> kldf_u8( lsrc,  lsrc_dilated,  lkernelDF,  lkernelWidthDF, lkernelHeightDF);
	kldf_u8();
	delete lkernelDF;
	//sprintf(arg,"%s_Dilated.ppm",identifier);
	//write_ppm(arg, lsrc_dilated->width(), lsrc_dilated->height(),lsrc_dilated->buffer());

	klApplyStainSpaceBasis ssb(lsrc_dilated);
	klRasterBufferPointer dstssb =  ssb();
	
	
	klStainPurity ssp(dstssb);
	klRasterBufferPointer dstssp =  ssp();
	sprintf(arg,"%s_StainPurity.ppm",identifier);
	write_ppm_single_band(arg, dstssp->width(), dstssp->height(),dstssp->buffer());



	double lthresholdHi=110;
	double lthresholdLow=110;
	bool luseLowThreshold=true;
	bool luseHighThreshold=true;
	double lthresholdHiVal=255;
	double lthresholdLowVal=0;
	bool luseThresholdVals=true;
	const klRasterBufferPointer lsrc_thresh=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );
	klCopyFunctor<unsigned char> klcfth_u8(dstssp, lsrc_thresh);
	klcfth_u8();
	klThresholdFunctor<unsigned char> kltf_u8( lsrc_thresh, lthresholdHi, lthresholdLow,luseLowThreshold,luseHighThreshold,lthresholdHiVal,lthresholdLowVal, luseThresholdVals);
	kltf_u8();
	sprintf(arg,"%s_Threshold.ppm",identifier);
	write_ppm_single_band (arg, lsrc_thresh->width(), lsrc_thresh->height(),lsrc_thresh->buffer());



	/*const klRasterBufferPointer ldst_cptbissb=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),3 );
	klCopySplitBandFunctor<unsigned char> kl_cfpitbissb_u8( dstssb,  ldst_cptbissb);
	kl_cfpitbissb_u8();
	write_ppm_single_band ("SSB_PIXTOBAND.pgm", ldst_cptbissb->width(), ldst_cptbissb->height()*3,ldst_cptbissb->buffer());*/

	//unsigned lkernelWidthEF=3;
	//unsigned lkernelHeightEF=3;
	//unsigned char* lkernelEF = new unsigned char[ lkernelWidthEF * lkernelHeightEF ];
	//lkernelEF[0]=1;lkernelEF[1]=1;lkernelEF[2]=1;
	//lkernelEF[3]=1;lkernelEF[4]=0;lkernelEF[5]=1;
	//lkernelEF[6]=1;lkernelEF[7]=1;lkernelEF[8]=1;
	//const klRasterBufferPointer lsrc_erroded=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	//klCopyFunctor<unsigned char> klcfef_u8(dstssb, lsrc_erroded);
	//klcfef_u8();//Do the copy
	

	//klErodeFunctor< unsigned char> klef_u8( dstssb,  lsrc_erroded,  lkernelEF,  lkernelWidthEF, lkernelHeightEF);
	//klef_u8();//Do the errode
	//write_ppm("ERRODED_u8_C3R.ppm", lsrc_erroded->width(), lsrc_erroded->height(),lsrc_erroded->buffer());
	//delete lkernelEF;

	const klRasterBufferPointer lsrc_logical_B1=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );

	klCopyFunctor<unsigned char> klcflf_B1_u8(lsrc_thresh, lsrc_logical_B1);
	klcflf_B1_u8();

	unsigned lkernelWidthO=3;
	unsigned lkernelHeightO=3;
	unsigned char* lkernelO = new unsigned char[ lkernelWidthO * lkernelHeightO ];
	lkernelO[0]=0;lkernelO[1]=1;lkernelO[2]=0;
	lkernelO[3]=1;lkernelO[4]=1;lkernelO[5]=1;
	lkernelO[6]=0;lkernelO[7]=1;lkernelO[8]=0;
	const klRasterBufferPointer lsrc_opened=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );
	klCopyFunctor<unsigned char> klcfof_u8(lsrc_logical_B1, lsrc_opened);
	klcfof_u8();//Do the copy
	klOpenFunctor< unsigned char> klof_u8( lsrc_logical_B1,  lsrc_opened,  lkernelO,  lkernelWidthO, lkernelHeightO);
	klof_u8();//Do the errode
	sprintf(arg,"%s_Opened.ppm",identifier);
	write_ppm_single_band(arg, lsrc_opened->width(), lsrc_opened->height(),lsrc_opened->buffer());
	delete lkernelO;

	
	klCompositeMask klcm( lsrc, lsrc_thresh ,0.7,0,120,0);
	klRasterBufferPointer blended = klcm();
	sprintf(arg,"%s_Nulcei.ppm",identifier);
	write_ppm(arg, blended->width(), blended->height(),blended->buffer());


	//klPCAFunctor pca(lsrc);
	//klRasterBufferPointer dstpca =  pca();
	//const klRasterBufferPointer ldst_cptbipca=new klPackedHeapRasterBuffer<unsigned char> (dstpca->width(),dstpca->height(),3 );
	//klCopySplitBandFunctor<unsigned char> kl_cfpitbipca_u8( dstpca,  ldst_cptbipca);
	//kl_cfpitbipca_u8();
	//sprintf(arg,"%s_PCA_PIXTOBAND.pgm",identifier);
	//write_ppm_single_band (arg, ldst_cptbipca->width(), ldst_cptbipca->height()*3,ldst_cptbipca->buffer());

	//klApplyStainSpaceBasis ssb(lsrc);
	//klRasterBufferPointer dstssb =  ssb();
	//const klRasterBufferPointer ldst_cptbissb=new klPackedHeapRasterBuffer<unsigned char> (dstssb->width(),dstssb->height(),3 );
	//klCopySplitBandFunctor<unsigned char> kl_cfpitbissb_u8( dstssb,  ldst_cptbissb);
	//kl_cfpitbissb_u8();
	//sprintf(arg,"%s_SSB_PIXTOBAND.pgm",identifier);
	//write_ppm_single_band (arg, ldst_cptbissb->width(), ldst_cptbissb->height()*3,ldst_cptbissb->buffer());




	////Test filter functor
	//unsigned int lkernelWidth=3;
	//unsigned int lkernelHeight=3;
	//unsigned char*  lkernel=new unsigned char[ lkernelWidth * lkernelHeight ];
	//lkernel[0]= 1.0/9.0*256;lkernel[1]= 1.0/9.0*256;lkernel[2]= 1.0/9.0*256;
	//lkernel[3]= 1.0/9.0*256;lkernel[4]= 1.0/9.0*256;lkernel[5]= 1.0/9.0*256;
	//lkernel[6]= 1.0/9.0*256;lkernel[7]= 1.0/9.0*256;lkernel[8]= 1.0/9.0*256;

	//klFilterFunctor<unsigned char> klff_u8(ldst, lkernel,  lkernelWidth, lkernelHeight);
	//klff_u8();
	//delete lkernel;
	//sprintf(arg,"%s_FILTERED_u8_C3R.ppm",identifier);
	//write_ppm (arg, ldst->width(), ldst->height(),ldst->buffer());

	//const klRasterBufferPointer ldst_color_to_gray=new klPackedHeapRasterBuffer<unsigned char>(inwidth,inheight, 1);
	//klColorToGrayFunctor<unsigned char> klctg_u8(lsrc,ldst_color_to_gray);
	//klctg_u8();
	//sprintf(arg,"%s_COLOR_TO_GRAY.pgm",identifier);
	//write_ppm_single_band (arg, ldst_color_to_gray->width(), ldst_color_to_gray->height(),ldst_color_to_gray->buffer());

	////Our morphological structure elelment
	//unsigned lkernelWidthEF=3;
	//unsigned lkernelHeightEF=3;
	//unsigned char* lkernelEF = new unsigned char[ lkernelWidthEF * lkernelHeightEF ];
	//lkernelEF[0]=1;lkernelEF[1]=1;lkernelEF[2]=1;
	//lkernelEF[3]=1;lkernelEF[4]=0;lkernelEF[5]=1;
	//lkernelEF[6]=1;lkernelEF[7]=1;lkernelEF[8]=1;
	//const klRasterBufferPointer lsrc_erroded=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	//klCopyFunctor<unsigned char> klcfef_u8(lsrc, lsrc_erroded);
	//klcfef_u8();//Do the copy
	//delete lkernelEF;

	//klErodeFunctor< unsigned char> klef_u8( lsrc,  lsrc_erroded,  lkernelEF,  lkernelWidthEF, lkernelHeightEF);
	//klef_u8();//Do the errode
	//sprintf(arg,"%s_ERRODED_u8_C3R.ppm",identifier);
	//write_ppm(arg, lsrc_erroded->width(), lsrc_erroded->height(),lsrc_erroded->buffer());

	//unsigned lkernelWidthDF=3;
	//unsigned lkernelHeightDF=3;
	////Mask values. Only pixels that correspond to nonzero 
	////mask values are taken into account during operation.
	//unsigned char* lkernelDF = new unsigned char[ lkernelWidthDF * lkernelHeightDF ];
	//lkernelDF[0]=1;lkernelDF[1]=1;lkernelDF[2]=1;
	//lkernelDF[3]=1;lkernelDF[4]=0;lkernelDF[5]=1;
	//lkernelDF[6]=1;lkernelDF[7]=1;lkernelDF[8]=1;
	//const klRasterBufferPointer lsrc_dilated=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	//klCopyFunctor<unsigned char> klcfdf_u8(lsrc, lsrc_dilated);
	//klcfdf_u8();//Do the copy
	//klDilateFunctor< unsigned char> kldf_u8( lsrc,  lsrc_dilated,  lkernelDF,  lkernelWidthDF, lkernelHeightDF);
	//kldf_u8();
	//delete lkernelDF;
	//sprintf(arg,"%s_DILATED_u8_C3R.ppm",identifier);
	//write_ppm(arg, lsrc_dilated->width(), lsrc_dilated->height(),lsrc_dilated->buffer());

	return 42;
}


void RunImageChain(klRasterBufferPointer in)
{
	
	{//Test the smart pointer x64 port went well
		size_t width=128;
		size_t height=128;
		size_t bands = 3;
		klRasterBufferPointer lsrc_No_boo_boo_pointer=new klPackedHeapRasterBuffer<unsigned char>(width,height, bands);
	}//YAY!
	const char* infilename="thumbnail.ppm";
	unsigned int inwidth=0;
	unsigned int inheight=0;
	unsigned int inbands=0;
	klRasterBufferPointer lsrc=in;
	//Test read ppm
	//if(	query_ppm (infilename, inwidth, inheight,inbands))
	//{
	//	lsrc=new klPackedHeapRasterBuffer<unsigned char>(inwidth,inheight, inbands);
	//	read_ppm (infilename, inwidth,inheight, inbands,lsrc->buffer());
	//}
	//else
	//{
	//	throw "Bad imput file in void testKLImageFunctors()";
	//}
	

	const klRasterBufferPointer ldst=new klPackedHeapRasterBuffer<unsigned char>(inwidth,inheight, inbands);
	int lband=-1;
	klCopyFunctor<unsigned char> klcf_u8(lsrc, ldst);
	klcf_u8();
	write_ppm ("SRC_u8_C3R.ppm", ldst->width(), ldst->height(),ldst->buffer());

	//Test filter functor
	unsigned int lkernelWidth=3;
	unsigned int lkernelHeight=3;
	unsigned char*  lkernel=new unsigned char[ lkernelWidth * lkernelHeight ];
	lkernel[0]= 0.0813*256;lkernel[1]= 0.0838*256;lkernel[2]= 0.0813*256;
	lkernel[3]= 0.0838*256;lkernel[4]= 0.0813*256;lkernel[5]= 0.08383*256;
	lkernel[6]= 0.0813*256;lkernel[7]= 0.0838*256;lkernel[8]= 0.0813*256;
	//unsigned char *  lkernel=new unsigned char[ lkernelWidth * lkernelHeight ];
	//lkernel[0]= 0;lkernel[1]= 0;lkernel[2]=0;
	//lkernel[3]= 0;lkernel[4]= 1;lkernel[5]= 0;
	//lkernel[6]= 0;lkernel[7]= 0;lkernel[8]= 0;

	klFilterFunctor<unsigned char> klff_u8(ldst, lkernel,  lkernelWidth, lkernelHeight);
	klff_u8();
	delete lkernel;
	write_ppm ("FILTERED_u8_C3R.ppm", ldst->width(), ldst->height(),ldst->buffer());

	const klRasterBufferPointer ldst_color_to_gray=new klPackedHeapRasterBuffer<unsigned char>(inwidth,inheight, 1);
	klColorToGrayFunctor<unsigned char> klctg_u8(lsrc,ldst_color_to_gray);
	klctg_u8();
	write_ppm_single_band ("COLOR_TO_GRAY.ppm", ldst_color_to_gray->width(), ldst_color_to_gray->height(),ldst_color_to_gray->buffer());




	////////////////////////////////Test threshold functor
	//Maps [0,thresholdLow] U [thresholdHigh,MAXVAL(TYPE) ] -----> LowVal , HighVal
	double lthresholdHi=240;
	double lthresholdLow=32;
	bool luseLowThreshold=true;
	bool luseHighThreshold=true;
	double lthresholdHiVal=200;
	double lthresholdLowVal=40;
	bool luseThresholdVals=true;
	const klRasterBufferPointer lsrc_thresh=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	klCopyFunctor<unsigned char> klcfth_u8(lsrc, lsrc_thresh);
	klcfth_u8();
	klThresholdFunctor<unsigned char> kltf_u8( lsrc_thresh, lthresholdHi, lthresholdLow,luseLowThreshold,luseHighThreshold,lthresholdHiVal,lthresholdLowVal, luseThresholdVals);
	kltf_u8();
	write_ppm ("THRESHOLDED_u8_C3R.ppm", lsrc_thresh->width(), lsrc_thresh->height(),lsrc_thresh->buffer());


	/////////////////////////////Test morphological functors 
	//Our morphological structure elelment
	unsigned lkernelWidthEF=3;
	unsigned lkernelHeightEF=3;
	unsigned char* lkernelEF = new unsigned char[ lkernelWidthEF * lkernelHeightEF ];
	lkernelEF[0]=1;lkernelEF[1]=1;lkernelEF[2]=1;
	lkernelEF[3]=1;lkernelEF[4]=0;lkernelEF[5]=1;
	lkernelEF[6]=1;lkernelEF[7]=1;lkernelEF[8]=1;
	const klRasterBufferPointer lsrc_erroded=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	klCopyFunctor<unsigned char> klcfef_u8(lsrc, lsrc_erroded);
	klcfef_u8();//Do the copy
	delete lkernelEF;

	klErodeFunctor< unsigned char> klef_u8( lsrc,  lsrc_erroded,  lkernelEF,  lkernelWidthEF, lkernelHeightEF);
	klef_u8();//Do the errode
	write_ppm("ERRODED_u8_C3R.ppm", lsrc_erroded->width(), lsrc_erroded->height(),lsrc_erroded->buffer());

	unsigned lkernelWidthDF=3;
	unsigned lkernelHeightDF=3;
	//Mask values. Only pixels that correspond to nonzero 
	//mask values are taken into account during operation.
	unsigned char* lkernelDF = new unsigned char[ lkernelWidthDF * lkernelHeightDF ];
	lkernelDF[0]=1;lkernelDF[1]=1;lkernelDF[2]=1;
	lkernelDF[3]=1;lkernelDF[4]=0;lkernelDF[5]=1;
	lkernelDF[6]=1;lkernelDF[7]=1;lkernelDF[8]=1;
	const klRasterBufferPointer lsrc_dilated=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),lsrc->numBands() );
	klCopyFunctor<unsigned char> klcfdf_u8(lsrc, lsrc_dilated);
	klcfdf_u8();//Do the copy
	klDilateFunctor< unsigned char> kldf_u8( lsrc,  lsrc_dilated,  lkernelDF,  lkernelWidthDF, lkernelHeightDF);
	kldf_u8();
	delete lkernelDF;
	write_ppm("DILATED_u8_C3R.ppm", lsrc_dilated->width(), lsrc_dilated->height(),lsrc_dilated->buffer());

	
	/////////////////////////////////////Test the rotate functor.
	//Let's write the file out with the ppm writer
	double angle=-45;
	//int interpolate=IPPI_INTER_CUBIC2P_BSPLINE;
	int interpolate=IPPI_INTER_NN;
	klRotateFunctor<unsigned char> klro_u8(lsrc, angle, interpolate);
	klRasterBufferPointer src_rotated =klro_u8();
	write_ppm ("ROTATED_u8_C3R.ppm", src_rotated->width(), src_rotated->height(),src_rotated->buffer());

	///////////////////////Test resample functor.
	double xFactor=2;
	double yFactor=2;
	interpolate=IPPI_INTER_CUBIC2P_BSPLINE;
	klResizeFunctor<unsigned char> klrs_u8(lsrc, xFactor, yFactor, interpolate);
	klRasterBufferPointer src_resampled =klrs_u8();
	write_ppm ("RESAMPLED_u8_C3R.ppm", src_resampled->width(), src_resampled->height(),src_resampled->buffer());


	//////////////////////Test roi  copy functor.
	klRect iRoi(128,128,256,256);
	klRect oRoi(128,128,256,256);
	klCopyROIFunctor<unsigned char> kl_cf_u8( src_rotated,  src_resampled,  iRoi, oRoi);
	kl_cf_u8();
	write_ppm ("COPYFUNCTION_DIFF_SIZE_u8_C3R.ppm", src_rotated->width(), src_rotated->height(),src_rotated->buffer());

	klCopyROIFunctor<unsigned char> kl_cf_u8t( lsrc_thresh,  lsrc_dilated,  iRoi, oRoi);
	kl_cf_u8t();
	write_ppm ("COPYFUNCTION_SAME_SIZE_u8_C3R.ppm", lsrc_thresh->width(), lsrc_thresh->height(),lsrc_thresh->buffer());

	
	
	/////////////////////Test Copy Pixel To Band Interleaved
	const klRasterBufferPointer ldst_cptbi=new klPackedHeapRasterBuffer<unsigned char> (src_rotated->width(),src_rotated->height(),src_rotated->numBands() );
	klCopySplitBandFunctor<unsigned char> kl_cfpitbi_u8( src_rotated,  ldst_cptbi);
	kl_cfpitbi_u8();
	write_ppm_single_band ("COPYFUNCTION_PIXTOBAND_u8_P3R.pgm", ldst_cptbi->width(), ldst_cptbi->height()*3,ldst_cptbi->buffer());

	
	
	//Test logical functor - and the first and second bands
	const klRasterBufferPointer lsrc_logical_B1=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );
	const klRasterBufferPointer lsrc_logical_B2=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );
	const klRasterBufferPointer lsrc_logical_B3=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );

	const klRasterBufferPointer logical_result=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );
		
	klCopyFunctor<unsigned char> klcflf_B1_u8(lsrc, lsrc_logical_B1,0);
	klcflf_B1_u8();
	write_ppm_single_band("LOGICAL_IN_B1.pgm", lsrc_logical_B1->width(), lsrc_logical_B1->height(),lsrc_logical_B1->buffer());
	
	klCopyFunctor<unsigned char> klcflf_B2_u8(lsrc, lsrc_logical_B2,1);
	klcflf_B2_u8();
	write_ppm_single_band("LOGICAL_IN_B2.pgm", lsrc_logical_B2->width(), lsrc_logical_B2->height(),lsrc_logical_B2->buffer());

	klCopyFunctor<unsigned char> klcflf_B3_u8(lsrc, lsrc_logical_B3,2);
	klcflf_B3_u8();
	write_ppm_single_band("LOGICAL_IN_B3.pgm", lsrc_logical_B3->width(), lsrc_logical_B3->height(),lsrc_logical_B3->buffer());

	klLogicalFunctor klflf_u8(lsrc_logical_B1, lsrc_logical_B2, logical_result,klLogicalFunctor::LogicalFunctorOperationType::AND);
	klflf_u8();
	write_ppm_single_band("LOGICAL_u8_C1R.pgm", logical_result->width(), logical_result->height(),logical_result->buffer());


	klFillHolesFunctor<unsigned char> klfhf_NOT_GREEN_u8(lsrc_logical_B2,255,0);
	klfhf_NOT_GREEN_u8();
	
	write_ppm_single_band("HOLES_FILLED_B2_u8_C1R.pgm", lsrc_logical_B2->width(), lsrc_logical_B2->height(),lsrc_logical_B2->buffer());

	
	unsigned char lbackgroundcolor=0;
	unsigned char lforegroundcolor=255;
	const klRasterBufferPointer lsrc_holefill=new klPackedHeapRasterBuffer<unsigned char> (lsrc->width(),lsrc->height(),1 );
	klCopyFunctor<unsigned char> klcfhf_u8(lsrc, lsrc_holefill,1);//Do the first band only
	klcfhf_u8();
	klFillHolesFunctor<unsigned char> klfhf_u8(lsrc_holefill,lbackgroundcolor,lforegroundcolor);
	klfhf_u8();
	write_ppm_single_band("HOLES_FILLED_u8_C1R.pgm", lsrc_holefill->width(), lsrc_holefill->height(),lsrc_holefill->buffer());

	//Test type convert functor.
	klRasterBufferPointer lfloatSrc=new klPackedHeapRasterBuffer<float> (lsrc->width(),lsrc->height(),lsrc->numBands());
	unsigned char linmin=0;
	unsigned char linmax=255;
	float loutmin=0.0f;
	float loutmax=1.0f;
	klTypeConvertFunctor<unsigned char,float> kltcfn_f_u8( lsrc, lfloatSrc,  true,linmin, linmax,loutmin, loutmax);
	kltcfn_f_u8();

	{
		///////////////////////////////////////////////////////TEST FLOATING POINT FN
		unsigned int lkernelWidthf32=3;
		unsigned int lkernelHeightf32=3;
		float *  lkernelf32=new float [ lkernelWidth * lkernelHeight ];
		lkernel[0]= 0.0113;lkernel[1]= 0.0838;lkernel[2]= 0.0113;
		lkernel[3]= 0.0838;lkernel[4]= 0.0113;lkernel[5]= 0.08383;
		lkernel[6]= 0.0113;lkernel[7]= 0.0838;lkernel[8]= 0.0113;

		klFilterFunctor<float > klff_f32(lfloatSrc, lkernelf32,  lkernelWidthf32, lkernelHeightf32);
		klff_f32();
		delete lkernelf32;

		//BBCREVISIT PROBLEM WITH FLOAT MORPH Functors

		//const klRasterBufferPointer lsrc_erroded_f32=new klPackedHeapRasterBuffer<float> (lfloatSrc->width(),lfloatSrc->height(),lfloatSrc->numBands() );
		//klCopyFunctor<float> klcfef_f32(lfloatSrc, lsrc_erroded_f32);
		//klcfef_f32();//Do the copy
		//klErodeFunctor< float> klef_f32( lfloatSrc,  lsrc_erroded_f32,  lkernelEF,  lkernelWidthEF, lkernelHeightEF);
		//klef_f32();//Do the errode

		//const klRasterBufferPointer lsrc_dilatedf32=new klPackedHeapRasterBuffer<float> (lfloatSrc->width(),lfloatSrc->height(),lfloatSrc->numBands() );
		//klCopyFunctor<float > klcfdf_f32(lfloatSrc, lsrc_dilatedf32);
		//klcfdf_f32();//Do the copy
		//klDilateFunctor< float > kldf_f32( lfloatSrc,  lsrc_dilatedf32,  lkernelDF,  lkernelWidthDF, lkernelHeightDF);
		//kldf_f32();


		double angle=-45;
		//int interpolate=IPPI_INTER_CUBIC2P_BSPLINE;
		int interpolate=IPPI_INTER_NN;
		klRotateFunctor<float> klro_f32(lfloatSrc, angle, interpolate);
		klRasterBufferPointer src_rotatedf32 =klro_f32();

		double xFactor=2;
		double yFactor=2;
		interpolate=IPPI_INTER_CUBIC2P_BSPLINE;

		klResizeFunctor<float  > klrs_f32(lfloatSrc, xFactor, yFactor, interpolate);
		klRasterBufferPointer src_resampledf32 =klrs_f32();

		klRect iRoi(128,128,256,256);
		klRect oRoi(128,128,256,256);

		klCopyROIFunctor<float > kl_cf_f32( src_rotatedf32,  src_resampledf32,  iRoi, oRoi);
		kl_cf_f32();
	}
	///////////////////////////////////END FLOAT TEST //////////////////////////////////////////

}

















