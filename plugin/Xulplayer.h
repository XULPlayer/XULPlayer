#ifndef _XULPLAYER_H_
#define _XULPLAYER_H_

#include "PluginCommon/mplayerwrapper.h"
#if defined(ENABLE_MLNET) &&  !defined(_LITE)
#include "PluginCommon/mlnet.h"
//class PluginCommon::CMLnet;
#endif
#ifdef ENABLE_MCODER
#include "pluginCommon/MCodeWrapper.h"
//class PluginCommon::MCodeWrapper;
#endif

#define BI_EPSILON 0.00001f
#define MSG_CONVERT_EXCUTE WM_USER + 10
#define MSG_CONVERT_END    WM_USER + 11

int PlayerEventHandler(int e, void* arg, PluginCommon::CMPlayer* inst);
#ifdef ENABLE_VIS
int AudioSampleCallback(char* data, int bytes);
#endif
#ifdef ENABLE_MCODER
int converterEventHandler(int eventid, void *arg, PluginCommon::MCodeWrapper *inst);
#endif

class CXulplayer
{
public:
	struct VideoWndPos_t {
		int w, h;
		int left, top;
	};

	CXulplayer(void);
	~CXulplayer(void);
	static CXulplayer* getInstance();
	static void        destroyInstance();
	void        setMaxPlayerCount(int maxCount = 32) {m_maxPlayerNum = maxCount;};
	bool		setMplayerPath(const char* mplayerPath);
	const char* getMpalyerPath() {return m_mplayerPath.c_str();}
	bool        initialize();
	bool        createPlayer(bool bCreateVideoWindow = true);
	void		destroyPlayerWindow(int id);
	bool        setActiveId(int id) ;
	int			getActiveId() {return m_activePlayerId;}
	int         getFirstPlayingIdFromId(int id);	// Get the first playing player Id start from specific Id(if none return -1)
	HWND		getActiveWnd();
	HWND		getWnd(int id);
	int         getPlayerCount();
	void		setCurrentRealVideoSize(int w, int h) {m_videoRealW = w; m_videoRealH = h;}
	void		getCurrentRealVideoSize(int& w, int& h) {w = m_videoRealW; h = m_videoRealH;}
	void		setVideoBgColor(int bgColor) {m_bgColor = bgColor;}
	int         getVideoBgColor() {return m_bgColor;}
	void		setBorderColor(int borderColor) {m_borderColor = borderColor;}
	int         getBorderColor() {return m_borderColor;}
	void		setBorderWidth(int borderWidth) {m_borderWidth = borderWidth;}
	int         getBorderWidth() {return m_borderWidth;}
	void	    setZoomFactor(float zoomFactor) {m_zoomFactor = zoomFactor;}
	float		getZoomFactor() {return m_zoomFactor;}
	void		setDefaultAspect(float aspectRatio) {m_defaultAspect = aspectRatio;}
	float		getDefaultAspect() {return m_defaultAspect;}
	bool        setOnPlayerStart(const char* playStartFuncName);
	std::string &getOnPlayerStart() {return m_onplayerStart;};
	bool        setOnPlayerPause(const char* playPauseFuncName);
	std::string &getOnPlayerPause() {return m_onplayerPause;};
	bool        setOnPlayerStop(const char* playStopFuncName);
	std::string &getOnPlayerStop()  {return m_onplayerStop;};
	bool		setCurrentPlayFile(const char* playFile);
	std::string &getCurrentPlayFile() {return m_currentPlayFile;}

	void        setKeepAspect(bool bkeepAspect) {m_bKeepAspect = bkeepAspect;}
	bool        getKeepAspect() {return m_bKeepAspect;}

	void        setEnableVisual(bool enableVisual) {m_enableVis = enableVisual;}
	bool        getEnableVisual() {return m_enableVis;}

	void        setEnableMultiMode(bool bMulti) {m_bMultiMode = bMulti;}
	bool		getEnableMultiMode() {return m_bMultiMode;}

	int			getPlayingCount();		// Get count of players that are now playing
	PluginCommon::CMPlayer*	getPlayer(int id);
	PluginCommon::CMPlayer*   getActivePlayer();
	void		setVideoWndPos(int id, int leftEdge, int topEdge, int w, int h);
	VideoWndPos_t* getVideoWndPos(int id);
	int			getIdByMousePos(int x, int y);	// Judge mouse position in which player video window
	int			getIdByVideoWnd(HWND hwnd);		// Get player Id by related window handle
	void        stopAllPlayers();
	
#ifdef ENABLE_MCODER
	PluginCommon::MCodeWrapper* getConverter();
	bool		  setConverterPath(const char* converterPath);
	bool          setConvertPrefs(const char* prefs);
	std::string   &getConvertPrefs() {return m_convertPrefs;}
#endif

#if defined(ENABLE_MLNET) &&  !defined(_LITE)
	PluginCommon::CMLnet* getMlnet();
	bool	setMlnetPath(const char* mlnetPath);
#endif

private:
	bool checkId(int id);

#ifdef ENABLE_MCODER
	PluginCommon::MCodeWrapper* m_pConverter;
	std::string   m_convertPrefs;
	std::string   m_convertPath;
#endif

#if defined(ENABLE_MLNET) &&  !defined(_LITE)
	PluginCommon::CMLnet*       m_pMlnet;
	std::string   m_mlnetPath;
#endif

	PluginCommon::CMPlayer** m_pMplayer;
	HWND* m_hWndVideo;
	//VideoWndPos_t* m_pVideosPos;			// Video Window position and size of each player
	std::string m_mplayerPath;
	std::string m_onplayerStart;
	std::string m_onplayerPause;
	std::string m_onplayerStop;
	std::string m_currentPlayFile;
	int			m_bgColor;
	int			m_borderWidth;
	int			m_borderColor;
	bool		m_enableVis;
	bool        m_bKeepAspect;
	bool		m_bMultiMode;
	int			m_activePlayerId;
	float       m_zoomFactor;
	float       m_defaultAspect;
	int			m_videoRealW;			// Current video real width
	int			m_videoRealH;			// Current video real height
	int			m_maxPlayerNum;

    static CXulplayer* m_pXulplayer;
};

#endif
