/*************************************************************************
* MediaCoder - The universal media transcoder
* Distributed under MPL 1.1
* Copyright (c) 2005-2008 Stanley Huang <stanleyhuangyc@gmail.com>
* All rights reserved.
**************************************************************************/

#ifndef _CHARSET_H
#define _CHARSET_H

#include "StrProMacro.h"
#include <string>

_MC_STRPRO_BEGIN

class MC_STRPRO_EXT CCharset
{
public:
	CCharset();
	CCharset(const char* tocode, const char* fromcode = 0);
	~CCharset();
	void Set(const char* tocode, const char* fromcode = 0);
	void Unset();
	std::string UTF8toANSI(const char* str);
	std::string ANSItoUTF8(const char* str);
	std::string Convert(const char* str, int len = 0);
private:
	void* cd;
};

_MC_STRPRO_END

#endif