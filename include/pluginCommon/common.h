#ifndef COMMON_INCLUDE_H
#define COMMON_INCLUDE_H

#include "PCMacro.h"
#include <string>
/*******************************************************************************
* CPU information
*******************************************************************************/
_MC_PC_BEGIN
bool MC_PC_EXT dirExists(const char* dirPath);
bool MC_PC_EXT FileExist(const char* filename);
std::string MC_PC_EXT OpenTextFile(const char* fileName);
bool MC_PC_EXT GetFolder(char *resultFolder, const char *szTitle="Open File");
bool MC_PC_EXT GetMediaFile(char* resultFile, int maxPathLen = 260);
int MC_PC_EXT MakeDir(const char* dirname);
_MC_PC_END

#endif