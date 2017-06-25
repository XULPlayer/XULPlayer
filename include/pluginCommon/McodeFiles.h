#ifndef	MCODE_FILES_H
#define MCODE_FILES_H

#include <string>
#include "PCMacro.h"
#include "common.h"
#include "strpro/StrProInc.h"
using namespace StrPro;
#include "utils/UtilsInc.h"
using namespace UTILS;

#define MAX_COMMAND_LEN     0x400

_MC_PC_BEGIN

//bool isDirExists(const char* dir);

typedef struct cpucaps_s {
	int cpuFamily;
	int cpuModel;
	int cpuStepping;
	int hasMMX;
	int hasMMX2;
	int has3DNow;
	int has3DNowExt;
	int hasSSE;
	int hasSSE2;
	int hasSSE3;
	int hasSSSE3;
	int hasSSE4;
	int cpuCores;
	int cpuFreq;
	int L1Cache;
	int L2Cache;
	char vendorName[20];
	char friendlyName[256];
} CpuCaps;

enum CovStatus
{
    WaitingToRun = 0,
    Converting,
    Finished,
	Aborted,
	Failed,
};

enum CoderType
{
    TypeUnknow = 0,
    MEncoder = 1,
    FFMPEG,
    TypeMax,
};

//! Parsing transcoding commands and config file
class MC_PC_EXT PlayFile
{
public:
	std::string     mWorkPath; 
	std::string		mAppPath;
    std::string     mCoderPath; 
	std::string     mToolPath; 
    std::string     mCoderPriority;
    std::string     command;	
    bool            isConverting;
    CoderType       type;
	int				fileDuration;
	CpuCaps			caps;
public:
    PlayFile();
    ~PlayFile();
public:
	void			initialize(const char*  localPath, const char*  prefsPath, const char*  workDir, const char*  appDir);
public:       
	bool            parseSettingFile(const char*  res, const char*  audioCommands = NULL, const char*  videoCommands = NULL, const char*  muxerCommands = NULL);
    std::string		parseInfoFile(const char*  fileName, const char* mediaFilePath, bool rawText = false);
    bool            parseLocalFile(const char*  fileName);
public: 	
	void			initLocalFile(const char* fileName);
	void			updateLocalFile();
    int             getPriorityValue();			
public:
	bool			makeItem(const char* itemXml);
	bool			initPrefs(const char* prefsPath);
	bool			makePrefs(const char* prefsXml);
	bool            updatePrefs(const char* prefsXml);
	bool			updatePrefsNode(const char* key, const char* val);
	bool			savePrefsFile(const char* prefsPath = NULL);
	void			revertPrefs(const char* keys);
	std::string		getPrefsText();
	std::string     getPrefsTextAll();
	const char*     getPrefsNodeValueString(const char* key);
	float			getPrefsNodeValueFloat(const char* key);
	int				getPrefsNodeValueInt(const char* key);
	const T_ITEM*	getDestFileInfo(){return &queue_Item;}
	bool			updateLocalFileNodeValue(const char* key, const char* value);
	std::string		getLocalFileNodeValue(const char* key);	
private:
	bool			formatCommand(const char* cmds = NULL);
	std::string		getEncoderExt(const char* encoder, bool isAudio = true);
private:
	std::string		localFilePath;
	std::string		prefsFilePath;
private:
	//as string data 
	std::string			destfileName; 
	std::string			filename;
	std::string			tempDir;
	std::string			destfileTitle;
	std::string			destfileExt;
	std::string			subfile;
	std::string			tags;
	//data	
	CXML2			MC_PresetsAllXml;
	CXML2			mc_prefXml;
	CXML2			mc_coreXml;
	CXML2			mc_codecsXml;
	T_ITEM				queue_Item;
	//seeker
	MEvaluater			parser;		
	//local date
	CXML2				m_LocalDoc;
};

_MC_PC_END

#endif
