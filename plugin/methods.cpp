#include "config.h"

#ifdef WIN32
#include "ui.h"
#include <shellapi.h>
#else
#include "linux_def.h"
#include <sys/types.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
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
#include "Xulplayer.h"
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
void InitWindow(HWND hWnd);
void UninitWindow(HWND hWnd);
HWND CreateVideoWindow(HWND hwndParent);
int EnumDir(const char* pchDir, const char* funcEnumDir, int level);
int GetVolume();
int SetVolume( const DWORD dwVolume );

bool p2pMessageHandler(int msgID, string &msgXML);

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

	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;

	xulplayer->setMaxPlayerCount(MAX_PLAYER_COUNT);
#ifdef WIN32
	GetModuleFileName(hDllInst, buf, sizeof(buf) - 16);
	p = strrchr(buf, '\\');
	if(p) *(p+1) = 0;
	/*std::string wnaLibPath(buf);
	wnaLibPath += "wnaspi32.dll";
	LoadLibrary(wnaLibPath.c_str());*/
	SetCurrentDirectory(buf);

	// Mplayer path
	mppath = buf;
	mppath += "mplayer.exe";
	if (!FileExist(mppath.c_str())) {
		mppath.insert(mppath.find_last_of('\\'), "\\codecs");
	}
	xulplayer->setMplayerPath(mppath.c_str());

#if defined(ENABLE_MCODER)
	string workDir(buf);
	workDir[workDir.find_last_of('\\')] = '\0';
	xulplayer->setConverterPath(workDir.c_str());
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
	xulplayer->setMlnetPath(mlnetPath.c_str());
#endif
	
	// get current domain name and store it
	//jsEvaluate("document.location.href", &location);
	//converter->onDeviceArrival(0);
	//converter->getDeviceDirs();
	return xulplayer->initialize();
}

void Uninit()
{
	CXulplayer::destroyInstance();
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

static bool method_stop(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;	// No active player, so just no respond
	mplayer->Stop();
	//xulplayer->destroyPlayerWindow(mplayer->getId());
	return true;
}

static bool method_pause(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;
	int ret = mplayer->Pause();
	INT32_TO_NPVARIANT(ret, *result);
	return true;
}

static bool method_seek(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{	
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;
	float seekval;
	switch (args[0].type) {
	case NPVariantType_Double:
		seekval = (float)(args[0].value.doubleValue);
		break;
	case NPVariantType_String:
		seekval = (float)atof((char*)args[0].value.stringValue.utf8characters);
		break;
	case NPVariantType_Int32:
		seekval = (float)(int)(args[0].value.intValue);
		break;
    default:
	    BOOLEAN_TO_NPVARIANT(false, *result);
        return false;
	}
	bool success = mplayer->Seek(seekval, argCount > 1 ? args[1].value.intValue : 0);
	BOOLEAN_TO_NPVARIANT(success, *result);
	return true;
}

#define NO_PLAYTHREAD
static bool method_get_info(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if (argCount >= 2) mplayer = xulplayer->getPlayer(args[1].value.intValue);
	if(!mplayer) return true; // if specific player hasn't been created, just no respond.

	switch (args[0].value.intValue) {
	case PLAYER_POS:
		INT32_TO_NPVARIANT(mplayer->pos, *result);
		break;
	case PLAYER_STATE:
        //printf("STATE: %d\n", mplayer->state);
		if(mplayer){
#if !defined(WIN32) && defined(NO_PLAYTHREAD)
			int i_state;
			if (mplayer->IsPlaying()) i_state = PLAYER_PLAYING;
			else i_state = PLAYER_IDLE;

			INT32_TO_NPVARIANT(i_state, *result);
#else
			INT32_TO_NPVARIANT(mplayer->state, *result);
#endif
		} else {
			INT32_TO_NPVARIANT(PLAYER_IDLE, *result);
		}
		break;
	case PLAYER_DURATION:
		INT32_TO_NPVARIANT(mplayer->duration, *result);
		break;
	case PLAYER_PERCENTAGE:
		INT32_TO_NPVARIANT(mplayer->duration > 0 ? mplayer->GetPos() * 100 / mplayer->duration : 0, *result);
		break;
	case PLAYER_HAS_AUDIO:
		BOOLEAN_TO_NPVARIANT(mplayer->hasAudio, *result);
		break;
	case PLAYER_HAS_VIDEO:
		BOOLEAN_TO_NPVARIANT(mplayer->hasVideo, *result);
		break;
	}
	return true;
}

static bool method_get_info_xml(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;
	char resultBuf[1024];
	char *p = resultBuf;
	p += sprintf(p, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<info>\n");
	p += sprintf(p, "<generic>\n<duration>%d</duration>\n</generic>\n", mplayer->duration);
	if (mplayer->hasAudio) {
		p += sprintf(p, "<audio>\n<id>%d</id>\n<format>%d</format>\n<codec>%s</codec>\n<bitrate>%d</bitrate>\n<srate>%d</srate>\n<nch>%d</nch>\n</audio>\n",
			mplayer->audio.id, mplayer->audio.format, mplayer->audio.codec, mplayer->audio.bitrate, mplayer->audio.samplerate, mplayer->audio.channels);
	}
	if (mplayer->hasVideo) {
		p += sprintf(p, "<video>\n<id>%d</id>\n<format>%d</format>\n<codec>%s</codec>\n<bitrate>%d</bitrate>\n<width>%d</width>\n<height>%d</height>\n",
			mplayer->video.id, mplayer->video.format, mplayer->video.codec, mplayer->video.bitrate, mplayer->video.width, mplayer->video.height);
		p += sprintf(p, "<fps>%f</fps>\n<aspect>%f</aspect>\n</video>\n",
			mplayer->video.fps, mplayer->video.aspect);
	}
	if (mplayer->isDISC)
	{
		p += sprintf(p,"<disc>\n<volume>%s</volume>\n<id>%s</id></disc>\n", 
			mplayer->discInfo.volume_id, mplayer->discInfo.disc_id);
	}
	p += sprintf(p, "</info>");
	CCharset charSet;
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(charSet.ANSItoUTF8(resultBuf).c_str()), *result);
	return true;
}

static bool method_player_command(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if (argCount > 2) mplayer = xulplayer->getPlayer(args[2].value.intValue);
	if(!mplayer) return false;	// if specific player hasn't been created, just no respond.

	char* resultString;
	bool hasResult = false;
	if (argCount > 1) hasResult = args[1].value.boolValue;
	
	bool success = mplayer->Command((char*)args[0].value.stringValue.utf8characters, hasResult ? &resultString : 0);
	if (success && hasResult) {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(resultString), *result);
	} else {
		INT32_TO_NPVARIANT(0, *result);
	}
	return true;
}

static bool method_play(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	
	char options[1024];
	char *opts = (char*)args[1].value.stringValue.utf8characters;
	char *p = options;
	
	bool bCreateVideoWindow = argCount < 3 || args[2].value.boolValue;
	xulplayer->createPlayer(bCreateVideoWindow);

	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(mplayer == NULL) return false;

	if (argCount >= 2 && opts) {
		p += sprintf(p, " %s", opts);
	} else {
		options[0] = 0;
	}
	if (xulplayer->getActiveWnd()) {
		p += sprintf(p, " -colorkey %d", xulplayer->getVideoBgColor());
	}
	if (!xulplayer->getKeepAspect()) {
		p += sprintf(p, " -nokeepaspect");
	}
	
	CCharset charset;
	mplayer->callback = PlayerEventHandler;
#ifdef ENABLE_VIS
	if (xulplayer->getEnableVisual()) mplayer->pfnAudioSample = AudioSampleCallback;
#endif
	PLAYER_PRIORITY pri = PRIORITY_NONE;
	if(argCount >= 4)
	{
		pri = (PLAYER_PRIORITY)args[3].value.intValue;
	}
	bool success = mplayer->Play(charset.UTF8toANSI((char*)args[0].value.stringValue.utf8characters).c_str(), xulplayer->getActiveWnd(), options, pri); 
	if (xulplayer->getActiveWnd()) {
#ifdef WIN32
		if(pPlugin) {
			RECT rc;
			GetClientRect(pPlugin->getEmbedWnd(), &rc);
			SendMessage(pPlugin->getEmbedWnd(), WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top));
		}
#else
        #warning "TODO: Linux UI"
#endif
	}

	mplayer->hasVisual = false;

#ifdef ENABLE_VIS
	if (xulplayer->getEnableVisual()) {
		//here is visulation part
		if(!mplayer->hasVideo && mplayer->hasAudio)
		{
			mplayer->hasVisual = MVisulation::getInstance()->init(xulplayer->getActiveWnd());
			if (mplayer->hasVisual)
			{
				MVisulation::getInstance()->update(mplayer->audio.samplerate, mplayer->audio.channels);
				MVisulation::getInstance()->render();
				//MVisulation::getInstance()->quit();
			}
		}
	}
#endif

	BOOLEAN_TO_NPVARIANT(success, *result);
	return true;
}

static bool method_pre_play(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;

	/*CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(mplayer == NULL) return false;*/

	CCharset charset;
	std::string filename = charset.UTF8toANSI((char*)args[0].value.stringValue.utf8characters);
	if(filename.empty()) return false;

	int playtime = 3500;
	if (argCount == 2) {
		playtime = args[1].value.intValue;
	}

	CProcessWrapper procWrap;
	char cmdStr[2048] = {0};
	const char* mppath = xulplayer->getMpalyerPath();

    if (mppath == NULL) return false;

	sprintf_s(cmdStr, 2048, "\"%s\" \"%s\" -quiet -vo null -nosound", mppath, filename.c_str());

	procWrap.Create(cmdStr);
	Sleep(playtime);
	procWrap.Cleanup();

    printf("pre player return\n");

	return true;
}

static bool method_get_time_string(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	int sec = args[0].type == NPVariantType_Double ? (int)args[0].value.doubleValue : args[0].value.intValue;
	int hour = sec / 3600;
	sec -= hour * 3600;
	int min = sec / 60;
	sec -= min * 60;
	char buf[64];
	char *p = buf;
	if (hour > 0) {
		p += sprintf(p, "%d:", hour);
	}
	sprintf(p, "%02d:%02d", min, sec);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(buf), *result);
	return true;
}

static bool method_get_cpu(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CCharset charSet;
	string info;
	/*
	if (converter) {
	    static int tries = 0;
		info = converter->getCpuCaps()->friendlyName;
		if (!info.empty()) {
			info = charSet.ANSItoUTF8(info.c_str());
		} else {
			if (++tries == 50) info = "Unknown CPU Model";
		}
	}
	*/
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(info), *result);
	return true;
}

static bool method_set_vol(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount > 0)
		SetVolume(args[0].value.intValue);
	return true;
}

static bool method_get_vol(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	INT32_TO_NPVARIANT(GetVolume(), *result);
	return true;
}

static bool method_zoom(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(xulplayer == NULL) return false;

	if (xulplayer->getActiveWnd()) {
#ifdef WIN32
	    int zoom = (int)(args[0].type == NPVariantType_Int32 ? args[0].value.intValue : args[0].value.doubleValue);
		RECT rc;
		GetClientRect(pPlugin->getEmbedWnd(), &rc);
		int vw = 0, vh = 0;
		if (zoom > 0) {
			xulplayer->getCurrentRealVideoSize(vw, vh);
			MoveWindow(xulplayer->getActiveWnd(), (rc.right-rc.left-vw)/2, (rc.bottom - rc.top-vh)/2, (int)(zoom * vw), (int)(zoom * vh), TRUE);
		} else {
			MoveWindow(xulplayer->getActiveWnd(), 0,0,rc.right - rc.left , rc.bottom - rc.top, TRUE);
		}
		DWORD lparem ;
		if(zoom < 0) 			//Resize the outer window to fit video window
			lparem= MAKELONG(vw, vh);
		else
			lparem= MAKELONG(rc.right - rc.left, rc.bottom - rc.top);

		if(pPlugin) {
			SendMessage(pPlugin->getEmbedWnd(), WM_SIZE, NULL, lparem);
		}
#else
        #warning "TODO: Linux UI"
#endif
	}
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

static bool method_set_player(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(xulplayer == NULL) return false;

	int activeId = args[0].value.intValue;
	xulplayer->setActiveId(activeId);
// #ifdef WIN32
// 	RECT rc;
// 	GetClientRect(s_hWnd, &rc);
// 	SendMessage(s_hWnd, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top));
// #else
//     #warning "TODO: Linux UI"
// #endif
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
#ifdef WIN32
		//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetCPUInfo, 0, 0, &id);
		InitWindow(pPlugin->getEmbedWnd());
#else
		#warning "TODO: Linux UI"
#endif
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
#ifdef WIN32
	UninitWindow(pPlugin->getEmbedWnd());
#endif
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

static bool method_http_get(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	char *url = (char*)args[0].value.stringValue.utf8characters;
	//char *data = (char*)args[1].value.stringValue.utf8characters;

	HTTP_REQUEST req;
	httpInitReq(&req, url, 0);
	req.method = HM_GET;

	bool ret = false;
	char* response = NULL;
	fprintf(stderr, "HTTP GET: %s\n", url);
	if (!httpRequest(&req)) {
		//CCharset charSet;
		//response = charSet.ANSItoUTF8(req.buffer);
		response = req.buffer;
		fprintf(stderr, "HTTP Response: %s\n", response);
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(response), *result);
		ret = true;
	} 
	
	httpClean(&req);
	return ret;
}

static bool method_post(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 2) return false;
	char *url = (char*)args[0].value.stringValue.utf8characters;

	HTTP_REQUEST req;
	httpInitReq(&req, url, 0);
	req.postPayload = (char*)args[1].value.stringValue.utf8characters;
	req.method = HM_POST;

	if (argCount > 2) {
		req.postPayloadBytes = args[2].value.intValue;
	} else {
		req.postPayloadBytes = args[1].value.stringValue.utf8length;
	}

	string response;
	if (!httpRequest(&req)) {
		CCharset charSet;
		response = charSet.ANSItoUTF8(req.buffer);
		//MessageBoxA(0, response.c_str(),0,0);
	}
	httpClean(&req);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(response), *result);
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
static bool method_dump_media_info(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;

	CCharset charSet;
	string fileName = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	bool rawText = false;
	if (argCount > 1) rawText = args[1].value.boolValue;
	std::string infoXml = xulplayer->getConverter()->generateFileInfo(fileName.c_str(), rawText);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(infoXml.c_str()), *result);
	return true;
}

static bool method_run_cov(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	char* command = (char*)args[0].value.stringValue.utf8characters;
	char* saudio = NULL;
	if (argCount >=2 ) {
		saudio = (char*)args[1].value.stringValue.utf8characters;
	}
	char* svideo = NULL;
	if (argCount >=3 ) {
		svideo = (char*)args[2].value.stringValue.utf8characters;
	}
	char* smuxser = NULL;
	if (argCount >=4 )
	{
		smuxser = (char*)args[3].value.stringValue.utf8characters;
	}
	if (!mc->begin(command, saudio, svideo, smuxser)) {	//command Ó¦ÎªUTF-8×Ö·û´®
		mc->end();
		return false;
	}    
	mc->callback = converterEventHandler;
	mc->parentWnd = pPlugin->getEmbedWnd();
	return true;
}

static bool method_pause_convert(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(mc) {
		mc->pause();
		return true;
	}
	return false;
}

static bool method_stop_convert(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(mc) {
		mc->stop();
		return true;
	}
	return false;
}

static bool method_create_xml_file(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 2) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	CCharset charSet;
	string fPath = charSet.UTF8toANSI(args[1].value.stringValue.utf8characters);
	string xmlText = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);

	if(!xmlText.empty() && !fPath.empty()) {	//xmlTextÎªAnsi String
		mc->getXmlResource(xmlText.c_str(), fPath.c_str());
		return true;
	}
	return false;
}

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

static bool method_make_media_shot(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	char* srcfile = 0;
	char* destFileDir = 0;
	char* destFileName = 0;
	int ss = 5;
	int width = 0;
	int height = 0;
	int outWidth = 0;
	int outHeight = 0;
	int outWidthMax = 0;
	int outHeightMax = 0;
	if (argCount < 1)return false;
	if (argCount >= 1) srcfile = (char*)args[0].value.stringValue.utf8characters;
	if (argCount >= 2) width = args[1].value.intValue;
	if (argCount >= 3) height = args[2].value.intValue;
	if (argCount >= 4) ss = args[3].value.intValue;
	if (argCount >= 5) outWidth = args[4].value.intValue;
	if (argCount >= 6) outHeight = args[5].value.intValue;
	if (argCount >= 7) outWidthMax = args[6].value.intValue;
	if (argCount >= 8) outHeightMax = args[7].value.intValue;
	if (argCount >= 9) destFileDir = (char*)args[8].value.stringValue.utf8characters;
	if (argCount >= 10) destFileName = (char*)args[9].value.stringValue.utf8characters;
	string ssrcfile = "";
	string sdestFileDir = "";
	string sdestFileName = "";
	CCharset set;
	if(srcfile)ssrcfile = set.UTF8toANSI(srcfile);
	if(destFileDir)sdestFileDir = set.UTF8toANSI(destFileDir);
	if(destFileName)sdestFileName = set.UTF8toANSI(destFileName);
	bool success = false;
	if(ss < 0){
		success = mc->makeMediaShot_FFmpeg(ssrcfile.c_str(), width, height, ss, 
						outWidth, outHeight, outWidthMax, outHeightMax,
						sdestFileDir.empty() ? 0 : sdestFileDir.c_str(),
						sdestFileName.empty() ? 0 : sdestFileName.c_str());
	}
	else{
		success =
		mc->makeMediaShot_MPlayer(ssrcfile.c_str(), width, height, ss, 
			outWidth, outHeight, outWidthMax, outHeightMax,
			sdestFileDir.empty() ? 0 : sdestFileDir.c_str(),
			sdestFileName.empty() ? 0 : sdestFileName.c_str());
	/*if (!success){
		converter->makeMediaShot_FFmpeg(ssrcfile.c_str(), width, height, ss, 
			outWidth, outHeight, outWidthMax, outHeightMax,
			sdestFileDir.empty() ? 0 : sdestFileDir.c_str(),
			sdestFileName.empty() ? 0 : sdestFileName.c_str());	
		}*/
	}
	return success;
}
#endif

#if !defined(_LITE) && defined(WIN32)
static bool method_process_image(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 3) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	string imgFileName = (char*)args[0].value.stringValue.utf8characters;
	unsigned int thumbWid = args[1].value.intValue;
	unsigned int thumbHei = args[2].value.intValue;
	CCharset set;
	imgFileName = set.UTF8toANSI(imgFileName.c_str());
	unsigned int imgW = 0, imgH = 0;
	int retVal = processImage(imgFileName.c_str(), mc->m_SavePath.c_str(), thumbWid, thumbHei, imgW, imgH);
	if(retVal == 1) {
		// Hight bit is image width, low bit is image height
		unsigned int imgWH = (imgW<<16) | imgH;
		INT32_TO_NPVARIANT(imgWH, *result);
		return true;
	}
	return false;
}
#endif

static bool method_set_tray_icon(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
#ifdef WIN32
	if(pPlugin == NULL) return false;

	bool addicon = true;
	if (argCount >= 1) addicon = args[0].value.boolValue;
	if (addicon) {
		SetTrayIcon(pPlugin->getEmbedWnd(), hAppIcon, NIM_ADD, "XULPlayer", 0, 0);
		if(pPlugin->getXulWnd()) ShowWindow(pPlugin->getXulWnd(), SW_HIDE);
	} else {
		SetTrayIcon(pPlugin->getEmbedWnd(), hAppIcon, NIM_DELETE, 0, 0, 0);
		if(pPlugin->getXulWnd()) ShowWindow(pPlugin->getXulWnd(), SW_SHOW);
	}
#else
    #warning "TODO: Linux UI"
#endif
	return true;
}

// When exit dual player mode, we should set playercount to 1;
static bool method_set_enable_multi_mode(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* pxulplayer = CXulplayer::getInstance();
	if(argCount < 1) return false;
	pxulplayer->setEnableMultiMode(args[0].value.boolValue);
	return true;
}

#if defined(ENABLE_MCODER)
static bool method_get_prefs_value(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(mc->getPrefsValue(args[0].value.stringValue.utf8characters)), *result);
	return true;
}

static bool method_get_prefs_xml(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	string sxml = mc->getPrefsAllXml();
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(sxml), *result);
	return true;
}

static bool method_set_prefs(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	switch (argCount) {
	case 1:
		BOOLEAN_TO_NPVARIANT(mc->setPrefs(args[0].value.stringValue.utf8characters), *result);
		break;
	case 2:
		BOOLEAN_TO_NPVARIANT(mc->setPrefs(args[0].value.stringValue.utf8characters, args[1].value.stringValue.utf8characters), *result);
		break;
	default:
		return false;
	}
	if(pPlugin) {
		SendMessage(pPlugin->getEmbedWnd(), WM_USER + 0x1000, 0, (LPARAM)"onPrefsChange");
	}
	return true;
}

static bool method_revert_prefs(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	mc->revertPrefs(argCount < 1 ? 0 : args[0].value.stringValue.utf8characters);
	return true;
}

static bool method_device_get_dirs(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	char drive = argCount > 0 ? args[0].value.stringValue.utf8characters[0] : 0;
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(mc->getDeviceDirs(drive).c_str()), *result);
	return true;
}

static bool method_device_transmit(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	const char* type = mc->getPrefsValue("extensions.device.type");
	string targetName = argCount > 2 ? args[2].value.stringValue.utf8characters : "";
	CCharset charSet;
	string filename = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	bool success = false;
	if (!FileExist(filename.c_str())) {
		BOOLEAN_TO_NPVARIANT(false, *result);
		return true;
	}

	if (!strcmp(type, "MTP")) {

	} else if (!strcmp(type, "iPod")) {

	} else {
		const char* drv = args[1].value.stringValue.utf8characters;
		success = mc->transmit(filename.c_str(), drv);		
	}

	// = CopyFile(filename.c_str(), path.c_str(), FALSE);
	BOOLEAN_TO_NPVARIANT(success, *result);
	return true;
}

static bool method_local_device_transmit(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	string targetName = argCount > 2 ? args[2].value.stringValue.utf8characters : "";
	CCharset charSet;
	string filename = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	bool success = false;
	if (!FileExist(filename.c_str())) {
		BOOLEAN_TO_NPVARIANT(false, *result);
		return true;
	}
	const char* drv = args[1].value.stringValue.utf8characters;
	success = mc->transmit2(filename.c_str(), drv);		
	BOOLEAN_TO_NPVARIANT(success, *result);
	return true;
}

static bool method_device_cancel(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	mc->endTransmit();
	return true;
}

static bool method_get_transmit_percent(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	INT32_TO_NPVARIANT(mc->getTransmitPercent(), *result);
	return true;
}

static bool method_get_transmit_status(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	INT32_TO_NPVARIANT(mc->getTransmitStatus(), *result);
	return true;
}

static bool method_get_transmit_info(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	int idx = 0;
	if (argCount >= 1) idx = args[0].value.intValue;
	switch (args[0].value.intValue) {
	case 0:
		INT32_TO_NPVARIANT(mc->getTransmitPercent(), *result);
		break;
	case 1:
		INT32_TO_NPVARIANT(mc->getTransmitStatus(), *result);
		break;
	case 2:
		//get Time needed unit ms
		INT32_TO_NPVARIANT(mc->getTransmitRemainTime(), *result);
		break;
	case 3:
		//get speed rate unit K/s
		DOUBLE_TO_NPVARIANT(mc->getTransmitSpeed(), *result);
		break;
	case 4:
		{
			//dump all in a string
			int arg0 = mc->getTransmitPercent();
			int arg1 = mc->getTransmitStatus();
			int arg2 = mc->getTransmitRemainTime();
			float arg3 = mc->getTransmitSpeed();
			char snum[256] = {0};
			sprintf(snum, "%d,%d,%d,%6f", arg0, arg1, arg2, arg3);
			STRINGZ_TO_NPVARIANT(MemAllocStrdup(snum), *result);
		}
		break;
	}
	return true;
}

static bool method_directory_transmit(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 2) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	CCharset charSet;
	string srcdir = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	string destdir = charSet.UTF8toANSI(args[1].value.stringValue.utf8characters);
	bool success = mc->transmit_directory(srcdir.c_str(), destdir.c_str());		
	BOOLEAN_TO_NPVARIANT(success, *result);

	return true;
}

static bool method_directory_cancel(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	mc->endDirTransmit();
	return true;
}

static bool method_get_dirtransmit_info(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	int idx = 0;
	if (argCount >= 1) idx = args[0].value.intValue;
	switch (idx) {
	case 0:
		//get status
		INT32_TO_NPVARIANT(mc->getDirTransmitStatus(), *result);
		break;
	case 1:
		//get percent
		INT32_TO_NPVARIANT(mc->getDirTransmitPercent(), *result);
		break;
	}
	return true;
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

#if defined(ENABLE_MCODER)
static bool method_get_local_keyvalue(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	char* key = (char*)args[0].value.stringValue.utf8characters;
	CCharset set;
	string skey = set.UTF8toANSI(key); 
	string sval = mc->getLocalKeyValue(skey.c_str());
	sval = set.ANSItoUTF8(sval.c_str());
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(sval.c_str()), *result);
	return true; 
}

static bool method_update_local_keyvalue(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 2) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	char* key = (char*)args[0].value.stringValue.utf8characters;
	char* val = (char*)args[1].value.stringValue.utf8characters;
	CCharset set;
	string skey = set.UTF8toANSI(key); 
	string sval = set.UTF8toANSI(val);
	mc->updateLocalKeyValue(skey.c_str(), sval.c_str());
	return true; 
}
#endif

#if !defined(_LITE) && defined(ENABLE_MLNET)
static bool method_p2p_start(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	if (argCount >= 1) {
		char* path = 0;
		path = (char*)args[0].value.stringValue.utf8characters;
		CCharset set;
		string strPath = set.UTF8toANSI(path);
		mlnet->setPath(strPath);
	}

	BOOLEAN_TO_NPVARIANT(mlnet->Start(), *result);
	return true; 
}

static bool method_p2p_stop(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	mlnet->Stop();
	return true; 
}

static bool method_p2p_addDownload(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) {
		BOOLEAN_TO_NPVARIANT(false, *result);
		return false;
	}
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;

	char* url = 0;
	url = (char*)args[0].value.stringValue.utf8characters;
	CCharset set;
	string strUrl = set.UTF8toANSI(url);

	BOOLEAN_TO_NPVARIANT(mlnet->addDownload(strUrl), *result);
	return true; 
}

static bool method_p2p_removeDownload(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) {
		BOOLEAN_TO_NPVARIANT(false, *result);
		return false;
	}

	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;

	BOOLEAN_TO_NPVARIANT(mlnet->removeDownload(args[0].value.intValue), *result);
	return true; 
}

static bool method_p2p_pauseDownload(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) {
		BOOLEAN_TO_NPVARIANT(false, *result);
		return false;
	}

	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;

	BOOLEAN_TO_NPVARIANT(mlnet->pauseDownload(args[0].value.intValue), *result);
	return true; 
}

static bool method_p2p_resumeDownload(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if (argCount < 1) {
		BOOLEAN_TO_NPVARIANT(false, *result);
		return false;
	}

	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;

	BOOLEAN_TO_NPVARIANT(mlnet->resumeDownload(args[0].value.intValue), *result);
	return true; 
}

static bool method_p2p_getState(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	INT32_TO_NPVARIANT(mlnet->getState(), *result);
	return true;
}

static bool method_p2p_getDownloadings(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	BOOLEAN_TO_NPVARIANT(mlnet->getDownloadings(), *result);
	return true; 
}

bool method_p2p_pollDownloadings(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	string *pInfos = mlnet->pollDownloadings();
	if (pInfos == NULL) return false;

	// no need do ANSItoUTF8
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(pInfos->c_str()), *result);
	delete pInfos;
	return true;
}

static bool method_p2p_getDownloadeds(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	BOOLEAN_TO_NPVARIANT(mlnet->getDownloadeds(), *result);
	return true; 
}

bool method_p2p_pollDownloadeds(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMLnet* mlnet = xulplayer->getMlnet();
	if(mlnet == NULL) return false;
	string *pInfos = mlnet->pollDownloadeds();
	if (pInfos == NULL) return false;

	// no need do ANSItoUTF8
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(pInfos->c_str()), *result);
	delete pInfos;
	return true;
}

static bool method_p2p_startuTorrent(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char path[MAX_PATH];
	char *p;

	GetModuleFileName(hDllInst, path, MAX_PATH - 25);
	p = strrchr(path, '\\');
	if (p == NULL) return false; 

	strcpy(p + 1, "uTorrent\\uTorrent.exe");
	BOOLEAN_TO_NPVARIANT(startUniqueProcessByName(path), *result);
	return true; 
}

static bool method_p2p_stopuTorrent(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char path[MAX_PATH];
	char *p;

	GetModuleFileName(hDllInst, path, MAX_PATH - 25);
	p = strrchr(path, '\\');
	if (p == NULL) return false; 

	strcpy(p + 1, "uTorrent\\uTorrent.exe");
	BOOLEAN_TO_NPVARIANT(killProcessByName(path), *result);
	return true; 
}

static bool method_p2p_starteMule(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char path[MAX_PATH];
	char *p;

	GetModuleFileName(hDllInst, path, MAX_PATH - 15);
	p = strrchr(path, '\\');
	if (p == NULL) return false; 

	strcpy(p + 1, "emule\\emule.exe");
	BOOLEAN_TO_NPVARIANT(startUniqueProcessByName(path), *result);
	return true; 
}

static bool method_p2p_stopeMule(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	char path[MAX_PATH];
	char *p;

	GetModuleFileName(hDllInst, path, MAX_PATH - 15);
	p = strrchr(path, '\\');
	if (p == NULL) return false; 

	strcpy(p + 1, "emule\\emule.exe");
	BOOLEAN_TO_NPVARIANT(killProcessByName(path), *result);
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

static bool method_get_file_size(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result)
{
	if(argCount < 1) return false;
	CCharset charSet;
	string filePath = charSet.UTF8toANSI(args[0].value.stringValue.utf8characters);
	struct stat st;
	int filesize = 0;
	stat(filePath.c_str(), &st);		
	filesize = (int)st.st_size/1024;
	INT32_TO_NPVARIANT(filesize, *result);
	return true;
}

PLUGIN_METHOD methods[] = {
	{"getHandle", method_default},
	{"getTimeString", method_get_time_string},
	{"getCPUInfo", method_get_cpu},
	{"play", method_play},
	{"prePlay", method_pre_play},
	{"stop", method_stop},
	{"pause", method_pause},
	{"seek", method_seek},
	{"getInfo", method_get_info},
	{"getFileInfo", method_get_file_info},
	{"getClipInfoXML", method_get_info_xml},
	{"sendCommand", method_player_command},
	{"getVolume", method_get_vol},
	{"setVolume", method_set_vol},
	{"enumFolder", method_enum_folder},
	{"shellExecute", method_shell_exec},
	{"zoom", method_zoom},
	{"setPlayer", method_set_player},
	{"getEnv", method_get_env},
	{"setWindowTop", method_set_window_top},
	{"httpGet", method_http_get},	
	{"post", method_post},
	{"postFile", method_post_file},
	{"init", method_init},
	{"uninit", method_uninit},
	{"setTrayIcon", method_set_tray_icon},
	{"setEnableMultiMode", method_set_enable_multi_mode},
	{"getVolumeName", method_get_volume_name},
	{"getFontTypes", method_get_font_types},
	{"getFileSize", method_get_file_size},
	 
	//{"registerKeyGroup", method_register_key_group},
#if  defined(WIN32) && !defined(_LITE)
	{"registerControlKey", method_register_control_key},
#endif
	
#if defined(ENABLE_MCODER)
	// Converter Methods
	{"dumpMediaInfo", method_dump_media_info},
	{"runCov", method_run_cov},
	{"getPrefsValue", method_get_prefs_value},
	{"getPrefsXml", method_get_prefs_xml},
	{"setPrefs", method_set_prefs},
	{"revertPrefs", method_revert_prefs},
	{"pauseConvert", method_pause_convert},
	{"stopConvert", method_stop_convert},
	{"createXmlFile", method_create_xml_file},
	{"openXmlFile", method_open_xml_file}, 
	{"getFolder", method_get_folder},
	{"getMediaFile", method_get_media_file},
	{"makeMediaShot", method_make_media_shot},
#endif

#if !defined( _LITE) && defined(WIN32)
	{"processImage", method_process_image},
#endif //_LITE
	
#if defined(ENABLE_MCODER)
	// Device Methods
	{"deviceTransmit", method_device_transmit},
	{"deviceLocalTransmit", method_local_device_transmit},
	{"deviceTransmitCancel", method_device_cancel},
	{"getDeviceDirs", method_device_get_dirs},
	{"getTransmitPercent", method_get_transmit_percent},
	{"getTransmitStatus", method_get_transmit_status},
	{"getTransmitInfo", method_get_transmit_info},

	// directory copy Methods
	{"directoryTransmit", method_directory_transmit},
	{"directoryTransmitCancel", method_directory_cancel},
	{"getDirTransmitInfo", method_get_dirtransmit_info},
#endif

#ifdef ENABLE_MEDIALIB
	//Media Searcher Methods
	{"startMediaSearch", method_start_media_search},
	{"getMediaSearchInfo", method_get_media_search_info},
	{"stopMediaSearch", method_stop_media_search},
#endif

#if defined(ENABLE_MCODER)
	//Local file information Methods
	{"getLocalKeyValue", method_get_local_keyvalue},
	{"updateLocalKeyValue", method_update_local_keyvalue},
#endif

#if !defined(_LITE) && defined(ENABLE_MLNET)
	//P2P downloader
	{"P2P_start_Mlnet", method_p2p_start},
	{"P2P_addDownload", method_p2p_addDownload},
	{"P2P_removeDownload", method_p2p_removeDownload},
	{"P2P_pauseDownload", method_p2p_pauseDownload},
	{"P2P_resumeDownload", method_p2p_resumeDownload},
	{"P2P_stop_Mlnet", method_p2p_stop},
	{"P2P_getState", method_p2p_getState},
	{"P2P_getDownloadings", method_p2p_getDownloadings},
	{"P2P_pollDownloadings", method_p2p_pollDownloadings},
	{"P2P_getDownloadeds", method_p2p_getDownloadeds},
	{"P2P_pollDownloadeds", method_p2p_pollDownloadeds},
	{"P2P_start_uTorrent", method_p2p_startuTorrent},
	{"P2P_stop_uTorrent", method_p2p_stopuTorrent},
	{"P2P_start_eMule", method_p2p_starteMule},
	{"P2P_stop_eMule", method_p2p_stopeMule},
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
