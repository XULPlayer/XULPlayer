#include <string>
#include "Xulplayer.h"
#include "scriptInvoke.h"
#include "PluginCommon/PluginCommonInc.h"
#include "strpro/charset.h"
#include "PluginCommon/mplayerwrapper.h"
#include "plugin.h"
#include "ui.h"

using namespace std;
using namespace PluginCommon;
using namespace StrPro;

//////////////////////////////////////////////////////////////////////////
// Global functions (event handlers)
//////////////////////////////////////////////////////////////////////////
extern CPlugin * pPlugin;

#ifdef WIN32
int PlayerEventHandler(int e, void* arg, CMPlayer* inst)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer || !pPlugin) return 0;
	HWND hPluginWnd = pPlugin->getEmbedWnd();
	switch (e) {
	case PC_STATE_CHANGE:
		switch ((int)arg) {
		case PLAYER_PLAYING:
			//hideback = true;
			if (!xulplayer->getOnPlayerStart().empty())
				SendMessage(hPluginWnd, WM_USER + 0x1000, 0, (LPARAM)xulplayer->getOnPlayerStart().c_str());
			InvalidateRect(hPluginWnd, 0, TRUE);
			break;
		case PLAYER_STOPPING: {
			if (!xulplayer->getOnPlayerStop().empty())
				SendMessage(hPluginWnd, WM_USER + 0x1000, 0, (LPARAM)xulplayer->getOnPlayerStop().c_str());
			InvalidateRect(hPluginWnd, 0, TRUE);
			printf("stop and call stop callback\n");
			//Destroy player window that stopped
			xulplayer->destroyPlayerWindow(inst->getId());
			} break;
		case PLAYER_PAUSED:
			if (!xulplayer->getOnPlayerPause().empty())
				SendMessage(hPluginWnd, WM_USER + 0x1000, 0, (LPARAM)xulplayer->getOnPlayerPause().c_str());
			break;
		}
		break;
	case PC_CONSOLE_OUTPUT: {
#ifdef _DEBUG
		DWORD bytes;
		HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
		WriteFile(hStdout, (char*)arg, strlen((char*)arg), &bytes, 0);
#endif
	  } break;
	default:
		printf("Play event:%d\n", e);
		break;
	}
	return 0;
}

#else //LINUX

#ifdef X86_64
typedef int64_t INT;
#else
typedef int INT; 
#endif

int PlayerEventHandler(int e, void* arg, CMPlayer* inst)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer || !pPlugin) return 0;

	switch (e) {
	case PC_STATE_CHANGE:
		switch ((INT)arg) {
	    case PLAYER_PLAYING:
		if (!xulplayer->getOnPlayerStart().empty())
			jsInvoke(xulplayer->getOnPlayerStart().c_str(), 0, 0);
		break;
	    case PLAYER_STOPPING: 
		//bool found = false;
        printf("Handler STOP PLAY event!\n");
		if (!xulplayer->getOnPlayerStop().empty()) {
            DD;
			jsInvoke(xulplayer->getOnPlayerStop().c_str(), 0, 0);
		} 
		//Destroy player window that stopped
		xulplayer->destroyPlayerWindow(inst->getId());
		break;
	    case PLAYER_PAUSED:
		if (!xulplayer->getOnPlayerPause().empty()) {
			jsInvoke(xulplayer->getOnPlayerPause().c_str(), 0, 0);
		} 
		break;
        }
	    printf("Play event:%d %d\n", e, (INT)arg);
	case PC_CONSOLE_OUTPUT: {
#if 0
		if (arg) printf("CONSOLE\n%s\n", arg);
#endif
	} break;
	default:
		printf("Unknown Play event:%d %d\n", e, (INT)arg);
		break;
	}
	return 0;
}

#endif

#ifdef ENABLE_VIS
int AudioSampleCallback(char* data, int bytes)
{
	MVisulation::getInstance()->render(data, bytes);
	return 0;
}
#endif

#ifdef ENABLE_MCODER
// Event Handler of Converter
int converterEventHandler(int eventid, void *arg, MCodeWrapper *inst)
{
	if (eventid == Finished) { 
		PostMessage(inst->parentWnd, MSG_CONVERT_END, (WPARAM)inst, 0);
	}
	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Implementation of CXulplayer
//////////////////////////////////////////////////////////////////////////
CXulplayer* CXulplayer::m_pXulplayer = 0;

CXulplayer::CXulplayer(): m_pMplayer(NULL), m_hWndVideo(NULL), m_defaultAspect(1.3333f), // 4/3
                            m_maxPlayerNum(32), m_activePlayerId(-1), m_enableVis(false), 
                            m_bgColor(0x1), m_videoRealW(0), m_videoRealH(0), m_bKeepAspect(true),m_bMultiMode(false),
                            m_borderWidth(0), m_borderColor(0), m_zoomFactor(0.f)/*, m_pVideosPos(NULL)*/
{
#ifdef ENABLE_MCODER
	m_pConverter = NULL;
#endif

#if defined(ENABLE_MLNET) &&  !defined(_LITE)
	m_pMlnet = NULL;
#endif
}

CXulplayer::~CXulplayer(void)
{
	stopAllPlayers();

#ifdef ENABLE_MCODER
	delete m_pConverter;
#endif

#if defined(ENABLE_MLNET) &&  !defined(_LITE)
	delete m_pMlnet;
#endif

	//FIXME: This causes XULRunner crashing on exit
#if 0
	for (int i=0; i<m_maxPlayerNum; ++i) {
		if(m_pMplayer[i]) {
			delete m_pMplayer[i];
			m_pMplayer[i] = NULL;
		}
		if(m_hWndVideo[i]) {
			DestroyWindow(m_hWndVideo[i]);
			m_hWndVideo[i] = NULL;
		}
	}
#endif
	
	delete[] m_pMplayer;
	m_pMplayer = NULL;
	delete[] m_hWndVideo;
	m_hWndVideo = NULL;
}

bool CXulplayer::setMplayerPath(const char* mplayerPath)
{
	if(!mplayerPath) return false;
	m_mplayerPath = mplayerPath;
	return true;
}

CXulplayer* CXulplayer::getInstance()
{
	if(m_pXulplayer == NULL) {
		m_pXulplayer = new CXulplayer();
	}
	return m_pXulplayer;
}

void CXulplayer::destroyInstance()
{
    if (m_pXulplayer != NULL) {
    	delete m_pXulplayer;
    	m_pXulplayer = NULL;
    }
}

bool CXulplayer::initialize()
{
	if(m_pMplayer == NULL && m_maxPlayerNum > 0) {
		m_pMplayer = new CMPlayer*[m_maxPlayerNum];
	}
	if(m_pMplayer) {
		for (int i=0; i<m_maxPlayerNum; ++i) {
			m_pMplayer[i] = NULL;
		}
	}

	if(m_hWndVideo == NULL && m_maxPlayerNum > 0) {
		m_hWndVideo = new HWND[m_maxPlayerNum];
	}

	if(m_hWndVideo) {
		for (int i=0; i<m_maxPlayerNum; ++i) {
			m_hWndVideo[i] = NULL;
		}
	}

// 	if(m_pVideosPos == NULL && m_maxPlayerNum > 0) {
// 		m_pVideosPos = new VideoWndPos_t[m_maxPlayerNum];
// 	}
// 
// 	if(m_pVideosPos) {
// 		for (int i=0; i<m_maxPlayerNum; ++i) {
// 			m_pVideosPos[i].w = m_pVideosPos[i].h = 0;
// 			m_pVideosPos[i].left = m_pVideosPos[i].top = 0;
// 		}
// 	}

#ifdef ENABLE_MCODER
	m_pConverter = new MCodeWrapper();
	m_pConverter->init(m_convertPath.c_str());
#endif

#if defined(ENABLE_MLNET) &&  !defined(_LITE)
	m_pMlnet = new CMLnet(m_mlnetPath);
	m_pMlnet->setMessageCallbackXML(NULL);
#endif

	return (m_pMplayer != NULL) && (m_hWndVideo != NULL);
}

bool CXulplayer::createPlayer(bool bCreateVideoWindow)
{
	if(m_hWndVideo == NULL) return false;
	if(!pPlugin) return false;

	int i=0;
	if(m_bMultiMode) {
		for (; i<m_maxPlayerNum; ++i) {
			if(m_hWndVideo[i] == NULL) break;
		}

		if(i == m_maxPlayerNum) return false;
	}
	
	if(m_pMplayer[i] == NULL)
		m_pMplayer[i] = new CMPlayer(m_mplayerPath.c_str());
	if(m_pMplayer[i] == NULL) return false;

	//if (m_hWndVideo[i]) DestroyWindow(m_hWndVideo[i]);
	if(bCreateVideoWindow) {
		m_hWndVideo[i] = CreateVideoWindow(pPlugin->getEmbedWnd());
		ShowWindow(m_hWndVideo[i], SW_SHOW);
	} else if (m_hWndVideo[i]){
		 DestroyWindow(m_hWndVideo[i]);
		 m_hWndVideo[i] = NULL;
	}

	m_pMplayer[i]->setId(i);
	m_activePlayerId = i;
	return true;
}

void CXulplayer::destroyPlayerWindow(int id)
{
	if(checkId(id) && m_hWndVideo) {
		if(m_hWndVideo[id]) {
			DestroyWindow(m_hWndVideo[id]);
			m_hWndVideo[id] = NULL;
			// If destroy current player, then change current player id
			if(m_activePlayerId == id) {
				while (--id >= 0) {
					if(m_hWndVideo[id]) {
						break;
					}
				}
				if(id >= 0) {
					m_activePlayerId = id;
				}
			}
		}
	}
}

bool CXulplayer::setActiveId(int id)
{
	if(checkId(id)) {
		m_activePlayerId = id;
		return true;
	}
	return false;
}

CMPlayer* CXulplayer::getActivePlayer() 
{
	if(m_pMplayer && checkId(m_activePlayerId)) {
		return m_pMplayer[m_activePlayerId];
	} else {
		return NULL;
	}
}

int CXulplayer::getFirstPlayingIdFromId(int id)
{
	if(checkId(id) && m_hWndVideo) {
		for (int i=id; i<m_maxPlayerNum; ++i) {
			if(m_hWndVideo[i]) return i;
		}
	} 
	return -1;
}

HWND CXulplayer::getActiveWnd()
{
#ifdef WIN32
	if(m_hWndVideo) {
		return m_hWndVideo[m_activePlayerId];
	} else {
		return NULL;
	}
#else
    return pPlugin->getEmbedWnd();
#endif
}

HWND CXulplayer::getWnd(int id)
{
	if(checkId(id) && m_hWndVideo) {
		return m_hWndVideo[id];
	} else {
		return NULL;
	}
}

CMPlayer* CXulplayer::getPlayer(int id)
{
	if(checkId(id) && m_pMplayer) {
		return m_pMplayer[id];
	} else {
		return NULL;
	}
}

int	CXulplayer::getPlayingCount()
{
	int count = 0;
	if(m_hWndVideo) {
		for (int i=0; i<m_maxPlayerNum; ++i) {
			if(m_hWndVideo[i]) count++;
		}
	}
	return count;
}

bool CXulplayer::setOnPlayerStart(const char* playStartFuncName)
{
    if(!playStartFuncName) return false;
	m_onplayerStart = playStartFuncName;
	return true;
}

bool CXulplayer::setOnPlayerPause(const char* playPauseFuncName)
{
	if(!playPauseFuncName) return false;
	m_onplayerPause = playPauseFuncName;
	return true;
}

bool CXulplayer::setOnPlayerStop(const char* playStopFuncName)
{
	if(!playStopFuncName) return false;
	m_onplayerStop = playStopFuncName;
	return true;
}

bool CXulplayer::setCurrentPlayFile(const char* playFile)
{
	if(!playFile) return false;
	m_currentPlayFile = playFile;
	return true;
}

void CXulplayer::setVideoWndPos(int id, int leftEdge, int topEdge, int w, int h)
{
// 	if(checkId(id) && m_pVideosPos) {
// 		m_pVideosPos[id].left = leftEdge;
// 		m_pVideosPos[id].top = topEdge;
// 		m_pVideosPos[id].w = w;
// 		m_pVideosPos[id].h = h;
// 	}
}

CXulplayer::VideoWndPos_t* CXulplayer::getVideoWndPos(int id)
{
// 	if(checkId(id) && m_pVideosPos) {
// 		return &m_pVideosPos[id];
// 	}
	return NULL;
}

int	CXulplayer::getIdByMousePos(int x, int y)
{
// 	if(m_pVideosPos) {
// 		for (int i=0; i<m_maxPlayerNum; ++i) {
// 			if(getWnd(i) == NULL) return -1;
// 			CXulplayer::VideoWndPos_t* pos = getVideoWndPos(i);
// 			if(!pos) return -1;
// 			if(x>=pos->left && x<=pos->left+pos->w &&
// 				y>=pos->top && y<=pos->top+pos->h) {		// mouse in current video window
// 				return i;
// 			}
// 		}
// 	}
	return -1;
}

int	CXulplayer::getIdByVideoWnd(HWND hwnd)
{
	if(hwnd && m_hWndVideo) {
		for (int i=0; i<m_maxPlayerNum; ++i) {
			if(m_hWndVideo[i] == hwnd) {
				return i;
			}
		}
	}
	return -1;
}

bool CXulplayer::checkId(int id)
{
	if(id>=0 && id<m_maxPlayerNum) {
		return true;
	}
	return false;
}

void CXulplayer::stopAllPlayers()
{
	if(m_pMplayer && m_hWndVideo) {
		for (int i=0; i<m_maxPlayerNum; ++i) {
			if(m_hWndVideo[i]) {
				m_pMplayer[i]->Stop();
			}
		}
	}
}

#ifdef ENABLE_MCODER
MCodeWrapper* CXulplayer::getConverter()
{
	return m_pConverter;
}
bool CXulplayer::setConverterPath(const char* converterPath)
{
	if(!converterPath) return false;
	m_convertPath = converterPath;
	return true;
}
bool CXulplayer::setConvertPrefs(const char* prefs)
{
	if(!prefs) return false;
	m_convertPrefs = prefs;
	return true;
}
#endif

#if defined(ENABLE_MLNET) &&  !defined(_LITE)
CMLnet* CXulplayer::getMlnet()
{
	return m_pMlnet;
}

bool CXulplayer::setMlnetPath(const char* mlnetPath)
{
	if(!mlnetPath) return false;
	m_mlnetPath = mlnetPath;
	return true;
}
#endif
