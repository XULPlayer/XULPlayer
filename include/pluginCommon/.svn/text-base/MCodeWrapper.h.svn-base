/*******************************************************************

* Copyright (c) 2008 Qi Huang <tthq@163.com>
* All rights reserved.
*******************************************************************/
#ifndef MCODE_WRAPPER_H
#define MCODE_WRAPPER_H

//this mainly support for Mencoder.exe and ffmpeg
#include <string>
#ifdef __linux__
#include "strpro/linux_def.h"
#endif
#include "PCMacro.h"
#include "McodeFiles.h"

#define MAX_CONSOLE_BUFFER  0x800
class CTransmitTool;
_MC_PC_BEGIN
class MCodeWrapper;
class CMPlayer;

typedef int (*CONVERTER_EVENT_HANDLER)(int eventid, void* arg, MCodeWrapper* inst);

class MC_PC_EXT MCodeWrapper
{
public:
	MCodeWrapper(void);
	~MCodeWrapper(void);
   
	bool		init(const char* workDir);
	void		uninit();
	bool		begin(const char* res, const char* addtionalAudioCmd = NULL,  const char* addtionalVideoCmd = NULL,  const char* addtionalMuxerCmd = NULL);
	void		end();        
	bool		isExecuting();
	int			pause();
	void		stop();

	void		getXmlResource(const char* resfile, const char* filePath = NULL); 
	bool		setPrefs(const char* prefsXml);    
	bool		setPrefs(const char* key, const char* val);   
	void		revertPrefs(const char* reservedKeys);
	const char* getPrefsValue(const char* key){return m_currentFile.getPrefsNodeValueString(key);}   
	std::string getPrefsAllXml();
	std::string generateFileInfo(const char* fileName, bool rawText = false);	
	std::string	getItem(const char* itemXml);
	int			makeDir(const char* directory);		
	const char* getWorkDir(){return m_WorkDir;}
 
	bool		initWorkEnvironment(const char* workDir);
	int			DoCallback(int eventid, void* arg);    
	CovStatus	getStatus(){return m_Status;}
	float		getPercentEx();
	//CpuCaps*	getCpuCaps() {return &m_currentFile.caps;}
	const T_ITEM* getDestItem(){return m_currentFile.getDestFileInfo();}
	std::string getLocalKeyValue(const char* key){return m_currentFile.getLocalFileNodeValue(key);}
	bool		updateLocalKeyValue(const char* key, const char* val){return m_currentFile.updateLocalFileNodeValue(key, val);}
	// device transmit
	void		endTransmit();
	int			getTransmitPercent();
	int			getTransmitStatus();
	float		getTransmitSpeed();
	DWORD		getTransmitRemainTime();
	std::string	getDeviceDirs(char drive = 0);
	bool		transmit(const char* filename, const char* targetPath, const char* targetName = 0);
	bool        transmit2(const char* filename, const char* targetPath, const char* targetName = 0);
	// disc or other device total copy
	bool		transmit_directory(const char* srcDir, const char* destDir);
	int			getDirTransmitPercent();
	int			getDirTransmitStatus();
	void		endDirTransmit();
	// make shots
	bool		makeMediaShot_FFmpeg(const char* srcFile, int inWidth = 0, int inHeight = 0, int ss = 5, int outWidth = 0, int outHeight = 0, int outWidthMax = 0, int outHeightMax = 0, const char* destFileDir = NULL, const char* destFileName = NULL);
	bool		makeMediaShot_MPlayer(const char* srcFile, int inWidth = 0, int inHeight = 0, int ss = 5, int outWidth = 0, int outHeight = 0, int outWidthMax = 0, int outHeightMax = 0, const char* destFileDir = NULL, const char* destFileName = NULL, int frames = 1);

	HWND                        parentWnd;     
	CONVERTER_EVENT_HANDLER     callback; 
	char                        m_WorkDir[MAX_PATH];
	CovStatus                   m_Status;   
	std::string					m_prefs;
	std::string					m_SavePath;

private:
	PlayFile                    m_currentFile; 
	float						m_singlePercent;
  
	char*                       consoleBuffer;
	//HANDLE                      hMutexQuery;
	HANDLE                      hThread;
	bool                        available;  
	
	CTransmitTool*				pTransDevice;
	CMPlayer*					pAudioConverter;
};

_MC_PC_END
#endif
