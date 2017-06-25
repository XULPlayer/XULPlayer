//#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include "_iconv.h"
#else
#include <iconv.h>
#endif
#include "charset.h"

using namespace std;

_MC_STRPRO_BEGIN
CCharset::CCharset():cd(0)
{
}

CCharset::CCharset(const char* tocode, const char* fromcode):cd(0)
{
	Set(tocode, fromcode);
}

CCharset::~CCharset()
{
	if (cd) {
		iconv_close(cd);
		cd = 0;
	}
}

void CCharset::Set(const char* tocode, const char* fromcode)
{
	Unset();
	if (tocode || fromcode)
		cd = iconv_open(tocode ? tocode : "", fromcode ? fromcode : "");
}

void CCharset::Unset()
{
	if (cd) {
		iconv_close(cd);
		cd = 0;
	}
}

std::string CCharset::UTF8toANSI(const char* str)
{
	if (!str) return "";

#ifdef WIN32
	void *tmpCd = iconv_open("", "UTF-8");
	size_t insize = strlen(str) + 1;
	size_t outsize = insize;
	char* buffer = (char*)malloc(outsize);
	memset(buffer, 0, outsize);
	char* out = buffer;
#ifdef WIN32
	iconv(tmpCd, (const char**)&str, &insize, &out, &outsize);
#else
	iconv(tmpCd, (char**)&str, &insize, &out, &outsize);
#endif
	iconv_close(tmpCd);
	string result(buffer);
	free(buffer);
	return result;
#else
    //FIXME
    return str;
#endif
}

std::string CCharset::ANSItoUTF8(const char* str)
{
	if (!str) return "";

#ifdef WIN32
	void *tmpCd = iconv_open("UTF-8", "");
	if (!tmpCd || tmpCd == (void*)-1) {
		fprintf(stderr, "Error calling iconv_open\n");
		return "";
	}
	size_t insize = strlen(str) + 1;
	size_t outsize = insize * 2;
	char* buffer = (char*)malloc(outsize);
	memset(buffer, 0, outsize);
	char* out = buffer;
#ifdef WIN32
	iconv(tmpCd, (const char**)&str, &insize, &out, &outsize);
#else
	iconv(tmpCd, (char**)&str, &insize, &out, &outsize);
#endif
	//*out = 0;
	iconv_close(tmpCd);
	string result(buffer);
	free(buffer);
	return result;
#else
    //FIXME
    return str;
#endif
}

std::string CCharset::Convert(const char* str, int len)
{
	char* buffer;
	if (!str) return "";
	size_t insize = (len > 0 ? len : strlen(str)) + 1;
	size_t outsize = insize * 2;
	size_t outsize2 = outsize;
	buffer = (char*)calloc(1, outsize);
	char* out = buffer;
#ifdef WIN32
	iconv(cd, (const char**)&str, &insize, &out, &outsize2);
#else
	iconv(cd, (char**)&str, &insize, &out, &outsize2);
#endif
	string result(buffer);
	free(buffer);
	return result;
}

_MC_STRPRO_END
