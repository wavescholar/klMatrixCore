#ifndef KL_OPENCV_FUNCTOR_H
#define KL_OPENCV_FUNCTOR_H
#include "kl_image_buffer.h"

class klOpenCV
{
public: 
	klOpenCV()
	{

	}

	klRasterBufferPointer FaceDetect(klRasterBufferPointer klrbp);


	klRasterBufferPointer  Watershed(klRasterBufferPointer klrbp);

};

void klTestOpenCV()
{
	const char* infilename="faces.ppm";
	unsigned int inwidth=0;
	unsigned int inheight=0;
	unsigned int inbands=0;
	klRasterBufferPointer lsrc;
	if(	query_ppm (infilename, inwidth, inheight,inbands))
	{
		lsrc=new klPackedHeapRasterBuffer<unsigned char>(inwidth,inheight, inbands);
		read_ppm (infilename, inwidth,inheight, inbands,lsrc->buffer());
	}
	else
	{
		throw "Bad imput file in void testKLImageFunctors()";
	}
	klOpenCV klocv;

	klRasterBufferPointer dst_faces	=klocv.FaceDetect(lsrc);
	//write_ppm("facesdetected.ppm", dst_faces);//bbcrevisit there is a problem with the onership of the output buffer
	
	/*const klRasterBufferPointer ldst_color_to_gray=new klPackedHeapRasterBuffer<unsigned char>(lsrc->width(),lsrc->height(), 1);
	klColorToGrayFunctor<unsigned char> klctg_u8(lsrc,ldst_color_to_gray);
	klctg_u8();
	*/
	klRasterBufferPointer dst_watershed	=klocv.Watershed(lsrc);
	write_ppm_single_band("watershed.ppm", dst_watershed);

}

#endif //KL_OPENCV_FUNCTOR_H