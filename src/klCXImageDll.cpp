#include "stdafx.h"
#include "klCXImageDll.h"

extern char* klfindImageFilenameExtension(const char * name);
int	klCXImageConvertImageTypesBatch( char *input_image , int typein , char *output_image , int typeout );
int	klCXImageConvertImage(  char *input_image , int typein , char *output_image , int typeout );


TCHAR* FindExtension(const TCHAR * name);
int	convert_image_types( TCHAR *input_image , int typein , TCHAR *output_image , int typeout );
int	convert_image_type(  TCHAR *input_image , int typein , TCHAR *output_image , int typeout );

////////////////////////////////////////////////////////////////////////////////
TCHAR* FindExtension(const TCHAR * name)
{
	int len = _tcslen(name);
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return (TCHAR*)(name+i+1);
		}
	}
	return (TCHAR*)(name+len);
}

int	convert_image_types( TCHAR *input_image , int typein , TCHAR *output_image , int typeout )
{
	TCHAR  file_name[ _MAX_PATH ];
	long find_handle;
	int status = 0;

#ifdef UNICODE
	struct _wfinddata_t c_file;
#else
	struct _finddata_t c_file;
#endif

	// search for all input file types
	find_handle = _tfindfirst( input_image , &c_file );
	if (find_handle > 0)
	{
		do {
			_tcscpy( file_name , c_file.name ); // Get the found file name
			file_name[ _tcschr( c_file.name , '.' ) - c_file.name + 1 ] = 0; // cut to the file extention
			_tcscat( file_name , (TCHAR*)( _tcschr( output_image , '.' ) + 1 )  ); 
			_tprintf( _T("Converting [ %s ] to [ %s ]\n") , c_file.name , file_name );
			status = convert_image_type( c_file.name , typein , file_name , typeout );
			if( status != 0 )
				break;
		} while( _tfindnext( find_handle , &c_file ) == 0 );
		_findclose( find_handle );
	}

	return status;
}


int	convert_image_type( TCHAR *input_image , int typein , TCHAR *output_image , int typeout )
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
