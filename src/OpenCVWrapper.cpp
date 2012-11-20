
#include "OpenCVWrapper.h"
#include "cxtypes.h"
#include "cxerror.h"
#include "cvver.h"
#include "wippi.hpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include "ipps.h"
#include "cv.h"
#include "highgui.h"
#include "cxcore.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include "ppm_helper.h"

//Converts the IplImage to a klRasterBuffer which does not own the memory
klRasterBufferPointer IplImageToklRasterBufferPointer(IplImage* image) 
{ 
	klRasterBufferPointer myklRasterBufferPointer =new klUnmanagedRasterBuffer( (unsigned char*) image->imageData,
		8,  //Sample Bits
		8,  //Band stride bits
		image->nChannels*8, //xstride bits
		image->widthStep*8, //ystride bits
		image->nChannels, //channels
		image->width, //width
		image->height); //height);
	//myklRasterBufferPointer->addRef();
	return myklRasterBufferPointer;

}

void  klRasterBufferPointerToIplImage(klRasterBufferPointer image,IplImage* cvimage ) 
{ 

	//Now we have to do the dirty job of copy - see if there is a way to make a klOpenCVRaster buffer that 
	//allows us to avoid the copy.  bbcrevisit		size_t inbands=src->numBands();
	size_t inbands=image->numBands();
	size_t inwidth =image->width();
	size_t inheight=image->height(); 
	size_t inbandStride=image->bandStride();
	size_t inxStride =image->xStride();
	size_t inyStride= image->yStride();

	size_t outbands=cvimage->nChannels;
	size_t outwidth =cvimage->width;
	size_t outheight=cvimage->height; 
	size_t outbandStride=1;
	size_t outxStride =3;
	size_t outyStride= cvimage->widthStep;

	unsigned int i;
	unsigned int j;
	unsigned int b;

	unsigned char* inbuf=image->buffer();

	unsigned char* outbuf=(unsigned char*)cvimage->imageData;

	unsigned typeSize=sizeof(unsigned char);
	unsigned inyOffset=inyStride/typeSize;
	unsigned inxOffset=inxStride/typeSize;

	unsigned outyOffset=outyStride/typeSize;
	unsigned outxOffset=outxStride/typeSize;

	for (i=0; i<inheight; i++)
	{
		for (j=0; j<inwidth; j++)
		{
			//Need to go from RGB to BGR
			*(outbuf+i*outyOffset + j*outxOffset +2)=*(inbuf+i*inyOffset+j*inxOffset+0);
			*(outbuf+i*outyOffset + j*outxOffset +1)=*(inbuf+i*inyOffset+j*inxOffset+1);
			*(outbuf+i*outyOffset + j*outxOffset +0)=*(inbuf+i*inyOffset+j*inxOffset+2);

		}
	}
}





void detect_and_draw( IplImage image );

klRasterBufferPointer  klOpenCV::FaceDetect (klRasterBufferPointer klrbp)
{
	// Haar classifier
	CvHaarClassifierCascade* cascade = 0;

	// Harr Classifier Cascade Name
	const char* cascade_name ="C:/KL/Packages/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml";
	//"haarcascade_profileface.xml";

	const int width= klrbp->width();
	const int height=klrbp->height();
	unsigned int bands=klrbp->numBands();

	klRasterBufferPointer dst=new klPackedHeapRasterBuffer<unsigned char>(width,height, bands);

	IppiSize maskSize = {3, 3};

	IppiSize roiSize = {width-6, height-6};
	IppiSize dstRoiSize = {width-6, height-6};
	IppiPoint anchor = {3, 3};

	int pBufferSize;
	int channels = 3;
	int bits =8;
	const int whc=width*height*channels;

	int srcStep = width*sizeof(Ipp8u)*channels;
	int dstStep = width*sizeof(Ipp8u)*channels;

	Ipp8u* pSrc =NULL;   

	CvSize size;
		size.width=klrbp->width();
		size.height=klrbp->height();
		CvImage cvImageObj;
		cvImageObj.create(cvSize(klrbp->width(),klrbp->height() ),8,3);
		IplImage* image = cvImageObj;

		klRasterBufferPointerToIplImage(klrbp,image);
		cvSaveImage( "testcon3.png", image );
	detect_and_draw( *image );
	klRasterBufferPointer detected = IplImageToklRasterBufferPointer( image) ;
	write_ppm("facesdetected.ppm",detected);
	return detected;
}
void detect_and_draw( IplImage img )
{
	// Memory for calculations
	CvMemStorage* storage = 0;

	// Haar classifier
	CvHaarClassifierCascade* cascade = 0;

	// Harr Classifier Cascade Name
	const char* cascade_name ="C:/KL/Packages/OpenCV/data/haarcascades/haarcascade_frontalface_alt2.xml";
	//"haarcascade_profileface.xml";


	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );	

	int scale = 1;

	// Create a new image based on the input image
	IplImage* temp = cvCreateImage( cvSize(img.width/scale,img.height/scale), 8, 3 );

	// Create two points to represent the face locations
	CvPoint pt1, pt2;
	int i;

	// Find whether the cascade is loaded, to find the faces. If yes, then:
	if( cascade )
	{
		storage = cvCreateMemStorage(0);

		// There can be more than one face in an image. So create a growable sequence of faces.
		// Detect the objects and store them in the sequence
		CvSeq* faces = cvHaarDetectObjects( &img, cascade, storage,
			1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
			cvSize(40, 40) );

		// Loop the number of faces found.
		for( i = 0; i < (faces ? faces->total : 0); i++ )
		{
			// Create a new rectangle for drawing the face
			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

			// Find the dimensions of the face,and scale it if necessary
			pt1.x = r->x*scale;
			pt2.x = (r->x+r->width)*scale;
			pt1.y = r->y*scale;
			pt2.y = (r->y+r->height)*scale;

			// Draw the rectangle in the input image
			cvRectangle( &img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
		}
	}
	cvReleaseImage( &temp );
}


klRasterBufferPointer klOpenCV::Watershed(klRasterBufferPointer klrbp)
{
	IplImage* marker_mask = 0;
	IplImage* markers = 0;
	IplImage* img0 = 0, *img = 0, *img_gray = 0, *wshed = 0;

	const int width= klrbp->width();
	const int height=klrbp->height();
	unsigned int bands=klrbp->numBands();

	klRasterBufferPointer dst=new klPackedHeapRasterBuffer<unsigned char>(width,height, bands);

	IppiSize maskSize = {3, 3};

	IppiSize roiSize = {width-0, height-0};
	IppiSize dstRoiSize = {width-0, height-0};
	IppiPoint anchor = {0, 0};

	int pBufferSize;
	int bits =8;

	const int whc=width*height*bands;

	int srcStep = width*sizeof(Ipp8u)*bands;
	int dstStep = width*sizeof(Ipp8u)*bands;

	Ipp8u* pSrc =NULL;   

	CvImage cvImageObj;
	CvSize mySize;
	mySize.height=height;
	mySize.width=width;
	cvImageObj.create(cvSize(width,height),bits,bands);

	Ipp8u* inPtr=klrbp->buffer();
	IplImage image = *cvImageObj;

	pSrc=cvImageObj.data();

	//do some sanity check
	{
		CvSize size;
		size.width=klrbp->width();
		size.height=klrbp->height();
		CvImage cvImageObj;
		cvImageObj.create(cvSize(klrbp->width(),klrbp->height() ),8,3);
		IplImage* cvimage = cvImageObj;

		klRasterBufferPointerToIplImage(klrbp,cvimage);
		cvSaveImage( "testcon2.png", cvimage );

		klRasterBufferPointer tesstcon1=IplImageToklRasterBufferPointer(cvimage) ;
		write_ppm("testcon2.ppm",tesstcon1);
	}
	//////////////end sanity check

	CvPoint prev_pt = {-1,-1};

	CvRNG rng = cvRNG(-1);

	CvImage cvImageObjGray;

	cvImageObjGray.create(cvSize(width,height),8,1);//Don't forget to release

	IplImage* _pImgOrig=cvImageObj;
	IplImage* _pImgGray=cvImageObjGray;

	cvCvtColor( _pImgOrig,_pImgGray,CV_RGB2GRAY);
	img = cvCloneImage( cvImageObjGray );
	img_gray = cvCloneImage( cvImageObjGray );
	wshed = cvCloneImage( cvImageObjGray );
	marker_mask = cvCreateImage( cvGetSize(img), 8, 1 );
	markers = cvCreateImage( cvGetSize(img), IPL_DEPTH_32S, 1 );

	//cvCvtColor( img, marker_mask, CV_BGR2GRAY );
	//cvCvtColor( marker_mask, img_gray, CV_GRAY2BGR );

	cvZero( marker_mask );
	cvZero( wshed );

	int c ='w';

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = 0;
	CvMat* color_tab;

	int i, j, comp_count = 0;

	cvFindContours( marker_mask, storage, &contours, sizeof(CvContour),
		CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	cvZero( markers );
	cvSaveImage( "wshed_markers_mask.png", marker_mask );
	cvSaveImage( "wshed_in.png", _pImgGray );
	cvSaveImage( "wshed_wshed.png", wshed );
	cvSaveImage( "wshed_markers.png", markers );


	marker_mask = cvLoadImage( "wshed_mask.png", 0 );

	for( ; contours != 0; contours = contours->h_next, comp_count++ )
	{
		cvDrawContours( markers, contours, cvScalarAll(comp_count+1),
			cvScalarAll(comp_count+1), -1, -1, 8, cvPoint(0,0) );
	}

	color_tab = cvCreateMat( 1, comp_count, CV_8UC3 );
	for( i = 0; i < comp_count; i++ )
	{
		uchar* ptr = color_tab->data.ptr + i*3;
		ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
		ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
		ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);
	}

	{
		double t = (double)cvGetTickCount();
		cvWatershed( img0, markers );

	}
	for( i = 0; i < markers->height; i++ )
		for( j = 0; j < markers->width; j++ )
		{
			int idx = CV_IMAGE_ELEM( markers, int, i, j );
			uchar* dst = &CV_IMAGE_ELEM( wshed, uchar, i, j*3 );
			if( idx == -1 )
				dst[0] = dst[1] = dst[2] = (uchar)255;
			else if( idx <= 0 || idx > comp_count )
				dst[0] = dst[1] = dst[2] = (uchar)0; // should not get here
			else
			{
				uchar* ptr = color_tab->data.ptr + (idx-1)*3;
				dst[0] = ptr[0]; dst[1] = ptr[1]; dst[2] = ptr[2];
			}
		}

		cvAddWeighted( wshed, 0.5, img_gray, 0.5, 0, wshed );

		cvReleaseMemStorage( &storage );

		cvReleaseMat( &color_tab );

		//	cvReleaseImage(*cvImageObjGray);


		Ipp8u* outPtr=dst->buffer();
		char* writePtr1=img_gray->imageData ;
		char* writePtr2=wshed->imageData ;
		char* writePtr3=marker_mask->imageData ;

		for ( int counter = 0; counter < width*height ; counter += 1 )
		{
			outPtr[counter]=writePtr1[counter]; 
		}
		for ( int counter =  width*height; counter < 2* width*height ; counter += 1 )
		{
			outPtr[counter]=writePtr2[counter]; 
		}
		for ( int counter =2* width*height; counter < 3*width*height ; counter += 1 )
		{
			outPtr[counter]=writePtr3[counter]; 
		}

		return dst;

}








