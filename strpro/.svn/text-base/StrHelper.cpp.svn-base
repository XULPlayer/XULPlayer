#include "StrHelper.h"
#include "bit_osdep.h"

_MC_STRPRO_BEGIN
std::string StrHelper::buffer = "";

StrHelper::StrHelper(void)
{
}

StrHelper::~StrHelper(void)
{
}

std::string StrHelper::strToLower( const std::string& inStr )
{
	std::string outStr = "";
	for ( size_t i = 0; i < inStr.length(); i++ )
	{
		outStr += tolower(inStr[i]);
	}
	return outStr;
}

std::string StrHelper::strToUpper( const std::string& inStr )
{
	std::string outStr = "";
	for ( size_t i = 0; i < inStr.length(); i++ )
	{
		outStr += toupper( inStr[i] );
	}
	return outStr;
}

std::string StrHelper::int32ToString( int inInt32, int radix /* = 10 */ )
{
	std::string outStr = "";
	char num[65] = {0};
	_itoa(inInt32, num, radix);
	outStr = num;
	return outStr;
}

std::string StrHelper::replaceAll(const std::string& inStr, const char* relStr /* = 0 */, const char* newStr /* = 0 */)
{
	std::string outStr = inStr;
	if (!relStr)
	{
		return outStr;
	}
	int pos = outStr.find(relStr);
	while(pos != std::string::npos)
	{
		outStr.replace(pos, strlen(relStr), newStr);
		pos = outStr.find(relStr);		
	}
	return outStr;
}

bool StrHelper::getFileTitle(const char* fileName, std::string& outTitle)
{
	std::string title(fileName);
	size_t slashIdx = title.find_last_of('\\');
	if(slashIdx == title.npos) {
		slashIdx = title.find_last_of('/');
	}
	if(slashIdx != title.npos) {
		title = title.substr(slashIdx+1);
	}

	size_t dotIdx = title.find_last_of('.');
	if(dotIdx != title.npos) {
		outTitle = title.substr(0, dotIdx);
	} else {
		outTitle = title;
	}
	
	return true;
}

bool StrHelper::getFileFolder(const char* fileName, std::string& outDir)
{
	std::string dir(fileName);
	size_t slashIdx = dir.find_last_of('\\');
	if(slashIdx == dir.npos) {
		slashIdx = dir.find_last_of('/');
	}
	if(slashIdx == dir.npos) {
		return false;
	}
	outDir = dir.substr(0, slashIdx+1);
	return true;
}

bool StrHelper::getFileExt(const char* fileName, std::string& ext)
{
	std::string extStr(fileName);
	size_t dotIdx = extStr.find_last_of('.');
	if(dotIdx == extStr.npos) {
		return false;
	}
	ext = extStr.substr(dotIdx);
	return true;
}

bool StrHelper::splitFileName(const char* fileName, std::string& dir, std::string& title, std::string& ext)
{
	std::string originFile(fileName);
	size_t slashIdx = originFile.find_last_of('\\');
	if(slashIdx == originFile.npos) {
		slashIdx = originFile.find_last_of('/');
	}
	if(slashIdx == originFile.npos) {
		return false;
	}
	size_t dotIdx = originFile.find_last_of('.');
	if(dotIdx == originFile.npos) {
		return false;
	}

	dir = originFile.substr(0, slashIdx+1);
	title = originFile.substr(slashIdx+1, dotIdx-slashIdx-1);
	ext = originFile.substr(dotIdx);
	return true;
}

bool StrHelper::isThisStringNumber(const char* inStr)
{
	if (inStr) {
		while (*inStr) {
			if(*inStr < 0) return false;
			if (!isdigit(*inStr)) return false;
			inStr++;
		}
		return true;
	}
	return false;
}

bool StrHelper::splitString(std::vector<std::string>& result, const char* str, const char* delimiter)
{
	if(!str) return false;
	std::string src(str);
	size_t delimiterIdx = src.npos;

	while((delimiterIdx=src.find(delimiter, 0)) != src.npos) {
		result.push_back(src.substr(0, delimiterIdx));
		src = src.substr(delimiterIdx+1);
	}
	result.push_back(src);
	return true;
}

_MC_STRPRO_END
