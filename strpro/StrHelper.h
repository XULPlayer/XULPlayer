#ifndef _STRHELPER_H
#define _STRHELPER_H

#include <stdlib.h>
#include <string>
#include <vector>
#include "StrProMacro.h"

_MC_STRPRO_BEGIN
class MC_STRPRO_EXT StrHelper
{
public:	
	~StrHelper(void);
	static std::string strToLower( const std::string& inStr );
	static std::string strToUpper( const std::string& inStr );
	static std::string int32ToString( int inInt32, int radix = 10 );
	static std::string replaceAll(const std::string& inStr, const char* relStr = 0, const char* newStr = 0);
	static bool getFileTitle(const char* fileName, std::string& outTitle);
	static bool getFileFolder(const char* fileName, std::string& outDir);	// Dir with last '\\'
	static bool getFileExt(const char* fileName, std::string& ext);			// Ext with '.'
	static bool splitFileName(const char* fileName, std::string& dir, std::string& title, std::string& ext); // Dir with last '\\', ext with '.'
	static bool isThisStringNumber(const char* inStr);
	static bool splitString(std::vector<std::string>& result, const char* str, const char* delimiter=",");

	template<typename T>
	static inline bool parseStringToNumArray(std::vector<T>& resultVct, const char* str, const char* delimiter=",")
	{
		if(!str) return false;
		std::string src(str);
		size_t delimiterIdx = 0;

		while((delimiterIdx=src.find(delimiter, 0)) != src.npos) {
			double curIdx = atof(src.substr(0, delimiterIdx).c_str());
			resultVct.push_back((T)curIdx);
			src = src.substr(delimiterIdx+1);
		}

		double lastIdx = atof(src.c_str());
		resultVct.push_back((T)lastIdx);
		return true;
	}

private:	
	StrHelper(void);
	static std::string buffer;
};

_MC_STRPRO_END

#endif
