#ifdef WIN32
#include <windows.h>
#include "atlstr.h"
#endif

#include "processwrapper.h"
#include "mplayerwrapper.h"

CMPlayer player;
static HWND hVideo = 0;
//static PFN_EVENT_HANDLER pfnPlayerEventHandler = 0;

int PlayerCallback(int eventid, void* arg)
{
	switch (eventid) {
	case PC_CONSOLE_OUTPUT:
		break;		
	case PC_STATE_CHANGE:
		switch ((int)arg) {
		case PLAYER_IDLE: {
			if (hVideo) {
				//PostMessage(hVideo, WM_CLOSE, 0, 0);
				hVideo = 0;
			}
			} break;
		case PLAYER_STOPPING:
			if (hVideo) {
				ShowWindow(hVideo, SW_HIDE);
			}
			break;
		}
		break;
	}
	return 0;
}

bool StartPlayer(char* mppath, char* filename, char* opts, int overlayColor, HWND hwnd)
{
	bool ret;
	//mcGetMediaInfo(item);
	CString options;
	options.AppendFormat(" -colorkey %d -wid %d -nokeepaspect", overlayColor, hwnd);
	if (opts) options.AppendFormat(" %s", opts);
	player.mppath = mppath;
	player.options = options;
	player.callback = PlayerCallback;
	ret = player.Play(filename);
	//SetWindowPos(hwnd, HWND_TOP, 0, 0, 160, 120, SWP_SHOWWINDOW);
	UpdateWindow(hwnd);
	return ret;
}

int mcPlayerStop()
{
	if (player.state == PLAYER_PLAYING || player.state == PLAYER_PAUSED) {
		player.Stop();
		return 0;
	} else {
		return -1;
	}
}

int mcPlayerPause()
{
	return player.Pause();
}

int mcPlayerSeek(int seekpos, int type)
{
	return player.Seek(seekpos, type);
}

int mcPlayerCommand(char* cmd)
{
	return player.Command(cmd);
}

int mcPlayerGetPos()
{
	return player.GetPos();
}

int mcPlayerGetState()
{
	return player.state;
}
