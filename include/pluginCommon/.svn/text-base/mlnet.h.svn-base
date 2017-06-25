/*******************************************************************
*mlnet.h
* felixwang (felixwang@sjtu.edu.cn)
* libmlnet export header file
*******************************************************************/

#ifndef __MLNET_H__
#define __MLNET_H__

#ifdef WIN32
#include <Windows.h>
#endif
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "PCMacro.h"

#ifdef WIN32
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
#endif

enum downloadStates {
	DOWNLOADING = 0,
	PAUSED,
	COMPLETE,
	SHARED,
	CANCELLED,
	NEW,
	ABORTED,
	QUEUED
};

enum messageType {
	MSG_DOWNLOADEDS = 0,
	MSG_DOWNLOADINGS,
	//TODO:add more message type
};

typedef struct {
	int32_t id;
	string name;
	int64_t size;
	int64_t downloaded;
	int32_t numSrc;
	int32_t numClient;
	downloadStates state;
	int32_t priority;
	float speed;
	string link; //FIXME: links
	string format;
	char md4[16];
	//TODO subfiles for BT
} fileInfo_t;

bool startUniqueProcessByName(const string &strPath);
bool killProcessByName(const string &strPath);

_MC_PC_BEGIN

typedef bool (*PFNMessageCallback)(int msgType, void* args);
typedef bool (*PFNMessageCallbackXML)(int msgType, string * pInfosXML);

class CProcessWrapper;
class CMLnetMsgW;
class CMLnetMsgR;
class CNetwork;

class MC_PC_EXT CMLnet
{
public:
    //! Search types.
    enum SearchType {
        LocalSearch = 0,
        RemoteSearch,
        SubscribeSearch
    };

	typedef enum {
		MLNET_IDLE = 0,
		MLNET_STARTED,
		MLNET_CONNECTED,
		MLNET_STOPPING,
	}  DAEMON_STATES;

	CMLnet(const char* path = NULL);
	CMLnet(const string &strPath);
	~CMLnet();

	bool Start();
	bool Stop();
	void setPath(const char* Path);
	void setPath(const string &strPath) {m_strPath = strPath;}
	const string getPath() const { return m_strPath; }
	const int getState() const { return m_State; }

	bool Connect();
	bool addDownload(const string &strUrl);
	bool removeDownload(int fileId);
	bool pauseDownload(int fileId);
	bool resumeDownload(int fileId);
	void setMessageCallbackXML(PFNMessageCallbackXML callbackXML) {m_pfnMessageCallbackXML = callbackXML;}
	bool getDownloadings();
	string *pollDownloadings();
	bool getDownloadeds();
	string *pollDownloadeds();
	bool addServers(const string &strServerDotMetURL);
	bool removeOldServers();
	bool setOption(const string &strOption, const string &strValue);
	void optimizeForChina();
	void optimizeCommon();
	bool exConsoleCommand(const string &strCmd);
	bool killServer();

	bool MsgRecvCallback(char *pBuf, int i_size);
	// decode downloading and downloaded file info and callback
	bool processDownloads(CMLnetMsgR &msg, int msgType);

private:
	//helper
	int16_t getOpcodeQuick(char *pBuf, int i_size);
	bool sendGenericCmdToCore(int opCode);

	//send
	int sendMsg(CMLnetMsgW &msg);
	int sendProtocolVersion(int v);
	int sendGuiExtensions(bool pollMode);
	int sendPassword(const string &username, const string &password);
	int switchDownload(int fileId, bool resume);

	static bool WINAPI MsgCallback(void* pParam, char *pBuf, int i_size) {
		CMLnet *self = (CMLnet*)pParam;
		return self->MsgRecvCallback(pBuf, i_size);
	}

	bool doMessageCallbackXML(int msgType, string* pInfosXML); 
	void flushMessagePool();
	void insertMessageToPool(int msgType, string* pInfosXML);
	/*must delete the message by the caller*/
	string *getMessageByType(int msgType);

	CNetwork *m_pSock;
	HANDLE m_hMutex;
	HANDLE m_hMsgMutex;
	string m_strPath;
	DAEMON_STATES m_State;

	//callbacks
	PFNMessageCallbackXML m_pfnMessageCallbackXML;

	int m_maxVersion;
	int m_maxOpcodeSendToGui;
	int m_maxOpcodeAcceptedByCore;
	// <type, InfoXML>
	map<int, string *> m_msgPool;
};

_MC_PC_END

#endif
