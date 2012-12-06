 /*******************************
 * WSCMP [2003] - [2012] WSCMP  *  
 * Bruce B Campbell 11 30 2012  *
 ********************************/

#include "kl_image_buffer.h"

bool query_ppm (const char* filename,unsigned int &width,unsigned int &height,unsigned int &bands);

bool read_ppm (const char* filename,unsigned int width,unsigned int height,unsigned int bands, unsigned char * inputbuf);

void write_ppm_single_band (const char* filename, unsigned int width,unsigned int height, unsigned char * buf);

void write_ppm(const char* filename, int width, int height, unsigned char * buf);

void write_ppm(const char* filename, klRasterBufferPointer klrbp);

void write_ppm_single_band (const char* filename,klRasterBufferPointer klrbp);