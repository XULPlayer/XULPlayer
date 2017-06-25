
/*******************************************************************
* MediaCoder - The universal audio/video transcoder
* Distributed under GPL license
* Copyright (c) 2005 Stanley Huang <stanleyhuangyc@yahoo.com.cn>
* All rights reserved.
*******************************************************************/
#ifndef MPLAYER_WRAPPER_H
#define MPLAYER_WRAPPER_H

#include <string>
#ifdef WIN32
#include <Windows.h>
#ifdef _DEBUG
#define DD printf("%s, %s(%d)\n", __FILE__,__FUNCTION__ , __LINE__)
#else
#define DD
#endif
#else
#include "linux_def.h"
#include <pthread.h>
#endif

#include "PCMacro.h"


_MC_PC_BEGIN

typedef int (*PFN_MP_SEND_CMD)(char* cmd);
typedef void (*PFN_MP_EXIT)(char* how);

#define CONSOLE_BUF_SIZE 2048

class CMPlayer;

typedef int (*PLAYER_EVENT_HANDLER)(int eventid, void* arg, CMPlayer* inst);
#ifdef ENABLE_VIS
typedef int (*CALLBACK_AUDIO_SAMPLE)(int pos, char* data, int bytes);
#endif

typedef enum {
	PC_CONSOLE_OUTPUT,
	PC_STATE_CHANGE,
} PLAYER_CALLBACKS;

typedef enum {
	PLAYER_IDLE = 0,
	PLAYER_ENTRY,
	PLAYER_PLAYING,
	PLAYER_PAUSED,	
	PLAYER_STOPPING,		// palyer going to be stopped
}  PLAYER_STATES;

typedef enum {
	PRIORITY_HIGHEST	= -20,	
	PRIORITY_HIGH,
	PRIORITY_ABOVENORMAL,
	PRIORITY_NORAMAL,
	PRIORITY_BELOWNORAMAL,
	PRIORITY_LOW,	
	PRIORITY_NONE = 0,
}  PLAYER_PRIORITY;

typedef enum {
	PLAYER_POS = 0,
	PLAYER_STATE,
	PLAYER_PERCENTAGE,
	PLAYER_DURATION,
	PLAYER_HAS_AUDIO,
	PLAYER_HAS_VIDEO,
} PLAYER_INFO;

typedef struct {
	int id;
	int format;
	int bitrate;					// audio bit rate
	int samplerate;					// sampling rate
	int channels;					// number of channels
	int samplebits;					// bits of samples
	char codec[32];					// audio codec name string
} PLAYER_ATTR_AUDIO;

typedef struct {
	int id;
	int format;
	int bitrate;					// video bit rate
	int width;						// video width in pixels
	int height;						// video height in pixels
	float fps;						// video frame rate
	float aspect;					// video aspect ratio
	char codec[32];					// video codec name string
} PLAYER_ATTR_VIDEO;

typedef struct {
	char disc_id[65];
	char volume_id[256];
} PLAYER_ATTR_DISC;

class CProcessWrapper;
class MC_PC_EXT CMPlayer
{
public:
	CMPlayer(const char* path = NULL);
	~CMPlayer();
	bool Play(const char* filename, HWND hWndParent = 0, const char* options = 0, PLAYER_PRIORITY priority = PRIORITY_NONE);
	void PrePlay(const char* filename, int milliSecs = 3000);
	void Stop();
	int Pause();
	int Step();
	bool GetFS();
	void SetFS(bool fs = true);
	bool Seek(float seekpos, int type = 2);
	bool Command(const char* cmd, char** result = 0);
	int Read(char** pbuf, int bufsize);
	int GetPos();
	int GetPosData();
	int WaitEvent(int timeout);
	int DoCallback(int eventid, void* arg);
	void SetState(PLAYER_STATES newstate);
	void RefreshVideo();
	void ParseInfo(char* id, char* v);
	void DumpInfo();
	void SetPlayerPath(const char* playerPath);
	const char* GetPlayerPath() const;
	void setId(int id) {m_id = id;}
	int  getId() {return m_id;}

	bool IsPlaying();

	PLAYER_EVENT_HANDLER callback;
#ifdef ENABLE_VIS
	CALLBACK_AUDIO_SAMPLE pfnAudioSample; 
#endif
	CProcessWrapper* pProc;
	PLAYER_STATES state;
	char* console;
	int pos;
	int duration;
	bool hasVideo;
	bool hasAudio;
	bool hasVisual;
	bool isDISC;
	PLAYER_ATTR_VIDEO video;
	PLAYER_ATTR_AUDIO audio;
	PLAYER_ATTR_DISC  discInfo;
	HWND hWnd;

#ifdef ENABLE_VIS
	HANDLE pipeAudioSamples;
#endif
private:
	static void WaitLaunchDone(void* ptrme);
	bool onClose();
#ifdef WIN32
	static BOOL CALLBACK EnumWindowCallBack(HWND hwnd, LPARAM lParam);
	HANDLE hThread;
	HANDLE hThreadAudioSample;
	//HANDLE hMutexQuery;
	HANDLE hEventState;
#else
    pthread_t m_thread;
    pthread_mutex_t m_mutex;    
    pthread_cond_t m_cond;
#endif
	char* mppath;
	int m_id;
};

_MC_PC_END

#endif
