char* FindExtension(const char * name);
int	convert_image_types( char *input_image , int typein , char *output_image , int typeout );
int	convert_image_type(  char *input_image , int typein , char *output_image , int typeout );
#include <iostream>
#include <fstream>
#include <string>
#define CXIMAGE_SUPPORT_BMP 1
#define CXIMAGE_SUPPORT_GIF 1
#define CXIMAGE_SUPPORT_JPG 1
#define CXIMAGE_SUPPORT_PNG 1
#define CXIMAGE_SUPPORT_ICO 1
#define CXIMAGE_SUPPORT_TIF 1
#define CXIMAGE_SUPPORT_TGA 1
#define CXIMAGE_SUPPORT_PCX 1
#define CXIMAGE_SUPPORT_WBMP 1
#define CXIMAGE_SUPPORT_WMF 1

#define CXIMAGE_SUPPORT_BMP  1
#define CXIMAGE_SUPPORT_GIF 1
#define CXIMAGE_SUPPORT_ICO 1
#define CXIMAGE_SUPPORT_TGA 1
#define CXIMAGE_SUPPORT_PCX 1
#define CXIMAGE_SUPPORT_WBMP 1
#define CXIMAGE_SUPPORT_WMF 1
#define CXIMAGE_SUPPORT_SKA 1
#define CXIMAGE_SUPPORT_JPG 1
#define CXIMAGE_SUPPORT_TIF 1
#define CXIMAGE_SUPPORT_JBG 1
#define CXIMAGE_SUPPORT_PNM  1
#define CXIMAGE_SUPPORT_RAS 1
#define CXIMAGE_SUPPORT_PNG 1
#define CXIMAGE_SUPPORT_JP2 1
#define CXIMAGE_SUPPORT_JPC 1
#define CXIMAGE_SUPPORT_PGX 1
#define CXIMAGE_SUPPORT_RAW 1
#define CXIMAGE_SUPPORT_TRANSFORMATION 1
#define CXIMAGE_SUPPORT_DSP 1
#define CXIMAGE_SUPPORT_ALPHA 1
#define CXIMAGE_SUPPORT_LAYERS 1
#define CXIMAGE_SUPPORT_DECODE 1
#define CXIMAGE_SUPPORT_ENCODE 1
#define CXIMAGE_SUPPORT_INTERPOLATION 1
#define CXIMAGE_SUPPORT_EXCEPTION_HANDLING 1
#define CXIMAGE_SUPPORT_DECODE	1
#define CXIMAGE_SUPPORT_ENCODE	1
#include "ximacfg.h"
#include "ximage.h"
#include "ximabmp.h"
#include <io.h>



int	klCXImageConvertImageTypesBatch( char *input_image , int typein , char *output_image , int typeout )
{
	char  file_name[ _MAX_PATH ];
	long find_handle;
	int status = 0;

#ifdef UNICODE
	struct _wfinddata_t c_file;
#else
	struct _finddata_t c_file;
#endif

	// search for all input file types
	find_handle = findfirst( input_image , &c_file );
	if (find_handle > 0)
	{
		do {
			_tcscpy( file_name , c_file.name ); // Get the found file name
			file_name[ _tcschr( c_file.name, '.' ) - c_file.name + 1 ] = 0; // cut to the file extention
			_tcscat( file_name , (char*)( _tcschr( output_image , '.' ) + 1 )  ); 
			_tprintf( _T("Converting [ %s ] to [ %s ]\n") , c_file.name , file_name );
			status = convert_image_type( c_file.name , typein , file_name , typeout );
			if( status != 0 )
				break;
		} while( _tfindnext( find_handle , &c_file ) == 0 );
		_findclose( find_handle );
	}

	return status;
}


int	klCXImageConvertImage( char *input_image , int typein , char *output_image , int typeout )
{
	CxImage image;

	if(!image.Load(input_image,typein))
	{
		_ftprintf(stderr, _T("%s\n"), image.GetLastError());
		_ftprintf(stderr, _T("error loading %s\n"), input_image);
		return 1;
	}

	if(!image.Save(output_image,typeout))
	{
		_ftprintf(stderr, _T("%s\n"), image.GetLastError());
		_ftprintf(stderr, _T("error saving %s\n"), output_image);
		return 1;
	}

	return 0;
}