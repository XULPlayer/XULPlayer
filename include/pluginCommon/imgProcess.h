#ifndef IMGPROCESS_INCLUDE_H
#define IMGPROCESS_INCLUDE_H

#include "PCMacro.h"
/*******************************************************************************
* Image processing module, using CImg library.
*******************************************************************************/
_MC_PC_BEGIN

//@Function: Process image, retrieve its width and height. And generate thumbnail
//			 in application data directory.
//@Params:   [in]  fileName--full path of image to be processed;
//		           thumbWid--width of thumbnail image; thumbHei--height of thumbnail image;	
//		     [out] imgWidth--image original width; imgHeight--image original height

//@Return value: 0 Failed, 1 success.
int processImage(const char* fileName, const char* saveDir, unsigned int thumbWid, unsigned int thumbHei,
				 /*out*/unsigned int& imgWidth, unsigned int& imgHeight);

_MC_PC_END

#endif