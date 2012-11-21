// /*
//  *  Copyright (C) 2009 Omnyx, LLC  All Rights Reserved.
//  * 
//  *  This file contains proprietary software licensed from Omnyx, LLC 
//  *  and is subject to a non-disclosure agreement.      
//  *  This header must remain in any source code despite modifications       
//  *  or enhancements by any party.  
//  * 
//  *  Original Author:  Bruce Campbell
//  *  Description: Function declarations for ppm_helper static library
// */

#include "kl_image_buffer.h"

bool query_ppm (const char* filename,unsigned int &width,unsigned int &height,unsigned int &bands);

bool read_ppm (const char* filename,unsigned int width,unsigned int height,unsigned int bands, unsigned char * inputbuf);

void write_ppm_single_band (const char* filename, unsigned int width,unsigned int height, unsigned char * buf);

void write_ppm(const char* filename, int width, int height, unsigned char * buf);

void write_ppm(const char* filename, klRasterBufferPointer klrbp);

void write_ppm_single_band (const char* filename,klRasterBufferPointer klrbp);