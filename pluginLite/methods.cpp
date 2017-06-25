#include "config.h"

#ifdef WIN32
#include <shellapi.h>
#include <iostream>
#include <sstream>
#else
#include "linux_def.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "httpclient.h"

#ifdef ENABLE_DVDINFO
//DVD/VCD/CD/CDRom info
#include "dvdinfo.h"
#endif

#ifdef ENABLE_AUTH
#include "auth/auth.h"
#endif

#include <stdlib.h>
#include "PluginCommon/PluginCommonInc.h"
#include "strpro/charset.h"
#include "scriptInvoke.h"
#include "PluginCommon/mplayerwrapper.h"
//#include "common.h"
#include <vector>

#ifdef _WIN32
#include "PluginCommon/common.h"
#endif

#define MAX_PLAYER_COUNT 64

extern CPlugin * pPlugin;
#ifdef WIN32
extern HICON hAppIcon;
#endif
using namespace std;
using namespace StrPro;
using namespace PluginCommon;

#ifdef WIN32
HINSTANCE hDllInst;
#endif

//#ifdef WIN32
//void* LoadRes(HMODULE hInstance, int id, int* pbytes, char* restype);
//#endif
HWND CreateVideoWindow(HWND hwndParent);
int EnumDir(const char* pchDir, const char* funcEnumDir, int level);
int GetVolume();
int SetVolume( const DWORD dwVolume );
char* GetStaticBuffer(const char* data, int bytes = 0);

bool Init()
{
	char buf[MAX_PATH] = {0};
	string mppath;
	char *p;

#ifdef ENABLE_AUTH
    if (auth_check() == 0) {
        free((void*)0x1);
        return false;
    }
#endif

#ifdef WIN32
	GetModuleFileName(hDllInst, buf, sizeof(buf) - 16);
	p = strrchr(buf, '\\');
	if(p) *(p+1) = 0;
	/*std::string wnaLibPath(buf);
	wnaLibPath += "wnaspi32.dll";
	LoadLibrary(wnaLibPath.c_str());*/
	SetCurrentDirectory(buf);
	
#if defined(ENABLE_MCODER)
	string workDir(buf);
	workDir[workDir.find_last_of('\\')] = '\0';
#endif

#else
	GetModuleFileName(NULL, buf, sizeof(buf) - 16);
	p = strrchr(buf, '/');
	if (p) *(p + 1) = 0;
	chdir(buf);
	mppath = buf;
	mppath += "mplayer";
	if (!FileExist(mppath.c_str())) {
		mppath.insert(mppath.find_last_of('/'), "/codecs");
		if (!FileExist(mppath.c_str())) {
			if (system("mplayer") != 0) {
				fprintf(stderr, "mplayer can not be found.\n");
				return false;
			}
		}
		mppath = "mplayer";
	}

	xulplayer->setMplayerPath(mppath.c_str());

    printf("%s\n", mppath.c_str());

#endif

#if !defined(_LITE) && defined(ENABLE_MLNET)
	/*init p2p downloader*/
	string mlnetPath(buf);
	mlnetPath += "mlnet\\mlnet.exe";
#endif
	
	// get current domain name and store it
	//jsEvaluate("document.location.href", &location);
	//converter->onDeviceArrival(0);
	//converter->getDeviceDirs();
	return true;
}

void Uninit()
{
	
}
#ifdef WIN32
#if !defined(_LITE) && defined(ENABLE_MLNET)
bool p2pMessageHandler(int msgID, string &msgXML)
{
	static NPVariant args[2];
	INT32_TO_NPVARIANT(msgID, args[0]);
	string strXML;
	CCharset set;
	strXML = set.ANSItoUTF8(msgXML.c_str());
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(strXML.c_str()), args[1]);
	jsInvoke("onDownloaderMessageRecv", 2, args);
#ifdef _DEBUG
	DWORD bytes;
	HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(hStdout, msgXML.c_str(), strlen(msgXML.c_str()), &bytes, 0);
#endif
	return true;
}
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		hDllInst = (HINSTANCE)hModule; 
		char *p;
		char path[MAX_PATH];
		GetModuleFileName(hDllInst, path, sizeof(path) - 16);
		p = strrchr(path, '\\');
		if (!p++) p = path;
		strcpy(p, "wnaspi32.dll");
		LoadLibrary(path);
		} break;
	case DLL_PROCESS_DETACH:
		//Uninit();
		break;
	}
    return TRUE;
}
#endif

#define NO_PLAYTHREAD

float GetCpuUsage();

static bool method_get_cpu(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	/*
	CCharset charSet;
	string info;
	if (converter) {
	    static int tries = 0;
		info = converter->getCpuCaps()->friendlyName;
		if (!info.empty()) {
			info = charSet.ANSItoUTF8(info.c_str());
		} else {
			if (++tries == 50) info = "Unknown CPU Model";
		}
	}
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(info), *result);
	*/
	DOUBLE_TO_NPVARIANT((double)GetCpuUsage(), *result);
	return true;
}

static bool method_enum_folder(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	string dirPath;
	CCharset charSet;
	dirPath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	
	char* enumFunc = (char*)args[1].value.stringValue.utf8characters;
	int level = argCount > 2 && args[2].value.boolValue ? -1 : 0;
	EnumDir(dirPath.c_str(), enumFunc, level);
	return true;
}

static bool method_get_file_info(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) return false;
	char* fileName = (char*)args[0].value.stringValue.utf8characters;
	//charset.UTF8toANSI((char*)args[0].value.stringValue.utf8characters)
#ifdef WIN32
	WIN32_FIND_DATA finddata;
	HANDLE hFind = FindFirstFile(fileName, &finddata);
	if (hFind != INVALID_HANDLE_VALUE) {
		char buf[16];
		_snprintf(buf, sizeof(buf), "%d", (finddata.nFileSizeHigh << 22) + (finddata.nFileSizeLow >> 10));
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(buf), *result);
	}
#else
   struct stat st;
   if (stat(fileName, &st) == 0) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%ld", st.st_size); 
        STRINGZ_TO_NPVARIANT(MemAllocStrdup(buf), *result);
   }
#endif

	return true;
}

bool property_hwnd(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value);
static bool method_default(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	property_hwnd(plugin, false, result);
	return true;
}


//extern int refcount;

static bool method_init(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
// 	if (refcount > 1) {
// 		INT32_TO_NPVARIANT(1, *result);
// 		return true;
// 	}
	if(pPlugin == NULL) return false;
	if (Init()) {
		INT32_TO_NPVARIANT(0, *result);
	} else {
		INT32_TO_NPVARIANT(-1, *result);
	}
	return true;
}

static bool method_uninit(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(pPlugin == NULL) return false;

	//if (refcount == 1) {
	Uninit();
	//}
	BOOLEAN_TO_NPVARIANT(true, *result);
	return true;
}

static bool method_shell_exec(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CCharset charSet;
	std::string cmdStr = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	std::string docFile;
	const char* pDocFile = NULL;
	int showMode = SW_SHOW;
	if(argCount >1) {
		docFile = charSet.UTF8toANSI(args[1].value.stringValue.utf8characters);
		if(!docFile.empty()) pDocFile = docFile.c_str();
		if(pDocFile) showMode = SW_HIDE;
	}
#ifdef WIN32
	ShellExecute(NULL, "open", cmdStr.c_str(), pDocFile, 0, showMode);
#else
	ShellExe((char*)cmdStr.c_str());
#endif
	return true;
}

bool method_get_env(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
#ifdef WIN32
	char buf[256];
	if (GetEnvironmentVariable((char*)args[0].value.stringValue.utf8characters, buf, sizeof(buf)) > 0) {
		CCharset charSet;
		string env = charSet.ANSItoUTF8(buf);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(env), *result);
	}
#else
    char *p = getenv((char*)args[0].value.stringValue.utf8characters);
    if (p != NULL) {
		CCharset charSet;
		string env = charSet.ANSItoUTF8(p);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(env), *result);
    }
#endif
	return true;
}
static bool method_set_window_top(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	if(!pPlugin) return false;
#if WIN32
    if (!pPlugin->getXulWnd()) return false;

	if(args[0].value.boolValue)
		SetWindowPos(pPlugin->getXulWnd(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	else
		SetWindowPos(pPlugin->getXulWnd(),HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
#else
    #warning "TODO: Linux UI"
#endif
	return true;
}

static int HTTPGET_LOCK = 0;
static bool method_http_get(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(HTTPGET_LOCK) return true;
	HTTPGET_LOCK = 1;

	if(argCount < 1) return false;
	char *url = (char*)args[0].value.stringValue.utf8characters;
	//char *data = (char*)args[1].value.stringValue.utf8characters;

	HTTP_REQUEST req;
	httpInitReq(&req, 0);
	req.method = HM_GET;

	bool ret = false;
	char* response = NULL;
	fprintf(stderr, "HTTP GET: %s\n", url);
	if (!httpRequest(&req, url)) {
		//CCharset charSet;
		//response = charSet.ANSItoUTF8(req.buffer);
		NPN_MemFlush(1024);
		response = req.buffer;
		fprintf(stderr, "HTTP Response: %s\n", response);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(response), *result);
		//STRINGZ_TO_NPVARIANT(GetStaticBuffer(response), *result);
		ret = true;
	} 
	
	httpClean(&req);

	HTTPGET_LOCK = 0;
	return ret;
}

static int HTTPPOST_LOCK = 0;
static bool method_post(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(HTTPPOST_LOCK) return true;
	HTTPPOST_LOCK = 1;

	if(argCount < 2) return false;

	char *url = (char*)args[0].value.stringValue.utf8characters;
	HTTP_REQUEST req;
	httpInitReq(&req, 0);
	req.postPayload = (char*)args[1].value.stringValue.utf8characters;
	req.method = HM_POST;

	if (argCount > 2) {
		req.postPayloadBytes = args[2].value.intValue;
	} else {
		req.postPayloadBytes = args[1].value.stringValue.utf8length;
	}

	string response;
	if (!httpRequest(&req, url)) {
		CCharset charSet;
		response = charSet.ANSItoUTF8(req.buffer);
		NPN_MemFlush(1024);
		//MessageBoxA(0, response.c_str(),0,0);
	}
	httpClean(&req);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(response), *result);
	//STRINGZ_TO_NPVARIANT(GetStaticBuffer(response.c_str()), *result);
	HTTPPOST_LOCK = 0;
	return true;
}

static bool method_post_file(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 2) return false;
	char *url = (char*)args[0].value.stringValue.utf8characters;
	char *filename = (char*)args[1].value.stringValue.utf8characters;
	HTTP_REQUEST req;
	string response;
	if (!httpPostFile(&req, url, "file", filename) && req.buffer) {
		CCharset charSet;
		response = charSet.ANSItoUTF8(req.buffer);
	}
	httpClean(&req);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(response), *result);
	return true;
}

static bool method_get_volume_name(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result) 
{
	if(argCount < 1) return false;
	char *volumePath = (char*)args[0].value.stringValue.utf8characters;
	if(!volumePath || *volumePath == '\0') return false;
	char volumeName[MAX_PATH] = {0};
	SetErrorMode(SEM_FAILCRITICALERRORS);
	if(!GetVolumeInformation(volumePath, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, NULL)) {
		return false;
	}
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(volumeName), *result);
	return true;
}
 
#if defined(ENABLE_MCODER)

static bool method_open_xml_file(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CCharset charSet;
	string filePath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	if(!filePath.empty()) {
		string fileContent = OpenTextFile(filePath.c_str());
		string ret;
		if(!fileContent.empty()) {
			ret = charSet.ANSItoUTF8(fileContent.c_str());
		}
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(ret), *result);
		return true;
	}
	return false;
}

static bool method_get_folder(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char strFolder[MAX_PATH];
	bool retVal = GetFolder(strFolder);
	if(retVal) {
		CCharset charSet;
		string ret = charSet.ANSItoUTF8(strFolder);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(ret), *result);
		return true;
	}
	return false;
}
static bool method_get_media_file(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char strFilePath[MAX_PATH];
	bool retVal = GetMediaFile(strFilePath, MAX_PATH);
	if(retVal) {
		CCharset charSet;
		string ret = charSet.ANSItoUTF8(strFilePath);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(ret), *result);
		return true;
	}
	return false;
}
#endif

#ifdef ENABLE_MEDIALIB
static bool method_start_media_search(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CCharset set;
	char* path = 0;
	if (argCount >= 1)
		path = (char*)args[0].value.stringValue.utf8characters;
	else 
		return false;
	char* skipPath = "";
	if (argCount >= 2)
	{
		skipPath = (char*)args[1].value.stringValue.utf8characters;
	}	
	string spath = set.UTF8toANSI(path);
	string sskip = set.UTF8toANSI(skipPath);
	BOOLEAN_TO_NPVARIANT(CFileSeeker::getInstance()->begin(spath.c_str(), sskip.c_str()), *result);
	return true; 
}

static bool method_get_media_search_info(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	int idx = 0;
	if (argCount >= 1) idx = args[0].value.intValue;
    CFileSeeker *pSeeker = CFileSeeker::getInstance();
	char* filter = 0;
	switch (idx) {
	case 0:
		//get status
		INT32_TO_NPVARIANT(pSeeker->status, *result);
		break;
	case 1:
		//get files' number already founded
		INT32_TO_NPVARIANT(pSeeker->fileCount, *result);
		break;
	case 2:
		{
			//dump a string of all the files through the filter
			//filter like MP3,3GP,.....AVI,
			//return value like d:\cats.mp4;c:\demo.wmv;....;		
			if (argCount >= 2){
				filter = (char*)args[1].value.stringValue.utf8characters;
			}
			string sfileNames = "";
			CCharset set;
			string sfliter = "";
			if(filter) {
				sfliter = set.UTF8toANSI(filter);
			}
			const char* dumpfiles = pSeeker->dumpFiles(sfliter.c_str());
			if(dumpfiles) {
				sfileNames = set.ANSItoUTF8(dumpfiles);
			}
			STRINGZ_TO_NPVARIANT(MemAllocStrdup(sfileNames.c_str()), *result);
		}
		break;
	case 3:
		//get the files count of those had been dumped
		INT32_TO_NPVARIANT(pSeeker->dumpedFileCount, *result);
		break;
	case 4:
		{
			//dump all in a string
			int arg0 = pSeeker->status;
			int arg1 = pSeeker->fileCount;
			int arg3 = pSeeker->dumpedFileCount;
			char snum[256] = {0};
			sprintf(snum, "%d,%d,none,%d", arg0, arg1,arg3);
			STRINGZ_TO_NPVARIANT(MemAllocStrdup(snum), *result);
		}
		break;
	}
	return true;
}

static bool method_stop_media_search(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CFileSeeker::getInstance()->end();
	return true; 
}
#endif

#ifdef ENABLE_DVDINFO
// DVD/VCD/CD/CDRom info method

static bool method_get_dvdinfo(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CCharset charSet;
	string dvdPath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	if(!dvdPath.empty()) {
		char *psz_xml = get_dvdifo_xml(dvdPath.c_str());
		if (psz_xml == NULL) return false;
		string ret(psz_xml);
		free(psz_xml);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(ret.c_str())), *result);
		return true;
	}
	return false;
}

bool method_get_vcdinfo(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CCharset charSet;
	string vcdPath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	if(!vcdPath.empty()) {
		char *psz_xml = get_vcdifo_xml(vcdPath.c_str());
		if (psz_xml == NULL) return false;
		string ret(psz_xml);
		free(psz_xml);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(ret.c_str())), *result);
		return true;
	}
	return false;
}

bool method_get_cdinfo(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CCharset charSet;
	string cdPath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	if(!cdPath.empty()) {
		char *psz_xml = get_cdifo_xml(cdPath.c_str());
		if (psz_xml == NULL) return false;
		string ret(psz_xml);
		free(psz_xml);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(ret.c_str())), *result);
		return true;
	}
	return false;
}

bool method_get_cdrominfo(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char *psz_xml = get_cdrom_xml();
	if (psz_xml == NULL) return false;
	string ret(psz_xml);
	free(psz_xml);
	CCharset charSet;
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(ret.c_str())), *result);
	return true;
}

bool method_detect_media_type(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CCharset charSet;
	string cdPath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	if(!cdPath.empty()) {
		INT32_TO_NPVARIANT(detect_media_type(cdPath.c_str()), *result);
		return true;
	}
	return false;
}
#endif

#if defined(WIN32) && !defined(_LITE)
extern std::vector<int> ctrlKeyCodes;
bool method_register_control_key(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	for (int i = 0; i < argCount; i++) {
		ctrlKeyCodes.push_back(args[i].value.intValue);
	}
	return true;
}
#endif

#if WIN32
int CALLBACK CollectFontProc(ENUMLOGFONTEX *lpelfe,NEWTEXTMETRICEX *lpntme,unsigned long FontType,long lParam)
{
	((string*)lParam)->append((char*)lpelfe->elfFullName);
	((string*)lParam)->append(";");
	return 1;
}
#endif

bool method_get_font_types(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CCharset charSet;
	string fonts = "";
#if WIN32
	HWND  hW=::GetFocus();  
	HDC  hdc=::GetDC(hW);  
	LOGFONT logfont;
	logfont.lfCharSet  =  DEFAULT_CHARSET; 
	logfont.lfFaceName[0]  ='\0';  
	logfont.lfPitchAndFamily  =  0;  
	::EnumFontFamiliesExA(hdc, &logfont, (FONTENUMPROC)CollectFontProc, (LPARAM)&fonts, 0);
	if(!fonts.empty())STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(fonts.c_str())), *result);
#endif	
	return true;
}

bool method_get_file_meidainfo_xml(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
#ifndef WIN32
	return false;
#else
	if(argCount < 1) return false;
	CCharset charSet;
	string filePath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	ostringstream cmd;
	if (!filePath.empty())
	{
		cmd << "./plugins/mediaInfo.exe" << " --output=XML --language=raw \"" << filePath << "\"";
		char* xmlText = NULL;
		CProcessWrapper proc;
		proc.flags = SF_REDIRECT_STDOUT | SF_ALLOC;

		bool success = (proc.Create(cmd.str().c_str()) && proc.ReadStdout() > 0);
		if (!success) return "";
		xmlText = proc.conbuf;

		char *p = NULL;
		for (p = xmlText; *p; p++) {
			if (*(LPWORD)p == 0xaaaa) {
				*p = ' ';
				*(++p) = ' ';
			} else if (*p > 13 && *p < 32) {
				*p = ' ';
			} else if (*p > 0 && *p < 9) {
				*p = ' ';
			} else if (*p==11 || *p==12 || *p==127) {
				*p = ' ';
			}
		}
		for (p = xmlText; *p && memcmp(p, "<?xml", 5); p++);

		STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(p)), *result);
		return true;
	}
#endif
	return false;
}

PLUGIN_METHOD methods[] = {
	{"getHandle", method_default},
	{"getCPUInfo", method_get_cpu},
	{"getFileInfo", method_get_file_info},
	{"enumFolder", method_enum_folder},
	{"shellExecute", method_shell_exec},
	{"getEnv", method_get_env},
	{"setWindowTop", method_set_window_top},
	{"httpGet", method_http_get},	
	{"post", method_post},
	{"postFile", method_post_file},
	{"init", method_init},
	{"uninit", method_uninit},
	{"getVolumeName", method_get_volume_name},
	{"getFontTypes", method_get_font_types},
	{"getFileMediaInfoXml", method_get_file_meidainfo_xml},
	 
	//{"registerKeyGroup", method_register_key_group},
#if  defined(WIN32) && !defined(_LITE)
	{"registerControlKey", method_register_control_key},
#endif
	
#ifdef ENABLE_MEDIALIB
	//Media Searcher Methods
	{"startMediaSearch", method_start_media_search},
	{"getMediaSearchInfo", method_get_media_search_info},
	{"stopMediaSearch", method_stop_media_search},
#endif

#ifdef ENABLE_DVDINFO
	//DVD/VCD/CD/CDRom info
	{"getDVDInfo", method_get_dvdinfo},
	{"getVCDInfo", method_get_vcdinfo},
	{"getCDInfo", method_get_cdinfo},
	{"getCDROMInfo", method_get_cdrominfo},
	{"detectMediaType", method_detect_media_type},
#endif

	{0},
};
