#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "plugin.h"
#include "resource.h"
#include "PluginCommon/processwrapper.h"
#include "PluginCommon/mplayerwrapper.h"
#include "StrPro/charset.h"
#include "ui.h"
#include "Xulplayer.h"
#include "gecko_sdk/npruntime.h"
#include "scriptInvoke.h"
#include "device.h"
#include <vector>

#ifdef ENABLE_MCODER
#include "PluginCommon/mcodewrapper.h"
#endif
#ifdef ENABLE_VIS
#include "PluginCommon/mvisulation.h"
#endif

#define VIDEO_WINDOW_CLASS "PlayerVideoWindow"

#define WM_TRAY_ICON_NOTIFY (WM_USER + 100)

using namespace PluginCommon;
using namespace StrPro;

extern CPlugin * pPlugin;


//bool hideback = false;
//bool adjusting = false;

HWND hwndConsole = 0;
#ifdef WIN32
extern HINSTANCE hDllInst;
//HBITMAP hBack;
HHOOK hHook = NULL;		// Handle of hook
HHOOK hKeyboardHook = NULL;		// Handle of hook
HICON hAppIcon;
#endif


void CreateConsole()
{
	if (!hwndConsole) {
		AllocConsole();
		hwndConsole = GetConsoleWindow();
	}
}

extern DWORD drvmap;

WPARAM discArg1 = 0;
std::string discArg2 = "";
std::string discArg3 = "";
void UpdateDisc(WPARAM wParam, const char* dir, const char* volumeName, const char* serialNum)
{	
	if (wParam == discArg1)
	{
		if (!discArg2.empty() && discArg2.compare(dir) == 0)
		{
			if (!discArg3.empty() && discArg3.compare(volumeName) == 0)
			{
				return;
			}
		}
	}
	else
	{
		discArg1 = wParam;
		discArg2 = dir;
		discArg3 = volumeName;
	}

	static NPVariant args[3];
	INT32_TO_NPVARIANT(wParam, args[0]);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(dir), args[1]);
	std::string sName;
	CCharset set;
	sName = set.ANSItoUTF8(volumeName);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(sName.c_str()), args[2]);
	STRINGZ_TO_NPVARIANT(serialNum, args[3]);
	jsInvoke("onDiscChange", 4, args);
}

void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
	// \\?\USB#Vid_04e8&Pid_503b#0002F9A9828E0F06#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
	// szDevId: USB\Vid_04e8&Pid_503b\0002F9A9828E0F06
	// szClass: USB
	if (strncmp(pDevInf->dbcc_name+4, "USB", 3)) return;

	static NPVariant args[3];
	std::string devid = pDevInf->dbcc_name+4;
	INT32_TO_NPVARIANT(wParam, args[0]);
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(devid), args[1]);

	int idx = devid.find_last_of('#');
	if (idx > 1) devid.erase(idx, devid.length() - idx);
	for (int i = devid.length() - 1; i >= 0; i--) {
		if (devid[i] == '#') 
			devid[i] = '\\';
		else
			devid[i] = toupper(devid[i]);
	}

	DWORD dwFlag = DBT_DEVICEARRIVAL != wParam ? 
		DIGCF_ALLCLASSES : (DIGCF_ALLCLASSES | DIGCF_PRESENT);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL,"USB",NULL,dwFlag);
	if( INVALID_HANDLE_VALUE != hDevInfo ) {
		SP_DEVINFO_DATA spDevInfoData;
		if ( FindDevice(hDevInfo, devid, spDevInfoData) ) {
			// OK, device found
			DWORD DataT ;
			TCHAR buf[MAX_PATH];
			DWORD nSize = 0;

			// get Friendly Name or Device Description
			if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, 
				SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buf, sizeof(buf), &nSize) ) {
			} else if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, 
				SPDRP_DEVICEDESC, &DataT, (PBYTE)buf, sizeof(buf), &nSize) ) {
			} else {
				lstrcpy(buf, "Unknown");
			}
			STRINGZ_TO_NPVARIANT(MemAllocStrdup(buf), args[2]);
		}
	}
	
	jsInvoke("onDeviceChange", 3, args);
}

bool IsDriveWritable(char drive)
{
	char path[32];
	sprintf(path, "%c:\\%08x.TMP", drive, GetCurrentProcessId());
	int fd = _open(path, _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC);
	if (fd < 0) return false;
	_close(fd);
	DeleteFile(path);
	return true;
}

bool IsDriveReadable(char drive)
{
	char buf[32];
	sprintf(buf, "%c:\\*.*", drive);
	WIN32_FIND_DATA f;
	HANDLE hFind = FindFirstFile(buf, &f);
	if (hFind != INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		return true;
	} else {
		return false;
	}
}

void NotifyDriveChange()
{
	NPVariant args[1];
	DWORD curdrvmap = GetLogicalDrives();
	std::string driveChanges;
	if (drvmap && curdrvmap != drvmap) {
		char drvletter;
		DWORD b;
		for (b = 1, drvletter = 'A'; b; b <<= 1, drvletter++) {
			if (drvletter <= 'C') continue;
			if ((drvmap & b) && !(curdrvmap & b)) {
				// drive removed
				driveChanges += drvletter;
				driveChanges += '-';
			} else if (!(drvmap & b) && (curdrvmap & b)) {
				// drive added
				if (IsDriveWritable(drvletter) || IsDriveReadable(drvletter)) {
					driveChanges += drvletter;
					driveChanges += '+';
				}
			} else {
				continue;
			}
			driveChanges += ' ';
		}
		drvmap = curdrvmap;
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(driveChanges.c_str()), args[0]);
		jsInvoke("onDriveChange", 1, args);
	}
}

#define BACK_WIDTH 176
#define BACK_HEIGHT 132
//extern WNDPROC lpOldProc;
LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer || !pPlugin) return 0;
	
	switch (msg) {
	case WM_ERASEBKGND: {
		HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);
		//HPEN hpen, hpenOld;
		HBRUSH hbrush, hbrushOld;
		// Create a pen.
		//hpen = CreatePen(PS_SOLID, xulplayer->getBorderWidth(), xulplayer->getBorderColor());
		// Create a brush.
		hbrush = CreateSolidBrush(xulplayer->getVideoBgColor());
		hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);
		Rectangle(hdc, 0,0, rc.right, rc.bottom);
		SelectObject(hdc, hbrushOld);
		DeleteObject(hbrush);
		// Select the new pen and brush, and then draw.
		//hpenOld = (HPEN)SelectObject(hdc, hpen);
		

		// Do not forget to clean up.
		//SelectObject(hdc, hpenOld);
		//DeleteObject(hpen);
		

		/*
		if (!hideback) {
			HDC hdcmem = CreateCompatibleDC(hdc);
			HGDIOBJ oldobj = SelectObject(hdcmem,hBack);
			BitBlt(hdc, (rc.right - BACK_WIDTH) / 2, (rc.bottom - BACK_HEIGHT) / 2 - 4, BACK_WIDTH,BACK_HEIGHT, hdcmem, 0, 0, SRCCOPY);
			SelectObject(hdcmem, oldobj);
		}
		*/
		}
		return TRUE;
#if 0
    case WM_PAINT:
      {
        // draw a frame and display the string
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

		/*
		FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));
        CPlugin * p = (CPlugin *)GetWindowLong(hWnd, GWL_USERDATA);
        if(p) {
          if (p->m_String[0] == 0) {
            strcpy("foo", p->m_String);
          }
          DrawText(hdc, p->m_String, strlen(p->m_String), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }
		*/
        EndPaint(hWnd, &ps);
	  }
      break;
#endif
	case WM_MOUSEMOVE:
		SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW)));
		break;
		
	case WM_SIZE: {
		int w = LOWORD(lParam);
		int h = HIWORD(lParam);
		ResizeVideo(w, h);
#ifdef ENABLE_VIS
		MVisulation::getInstance()->resizeWnd();
#endif
		//InvalidateRect(hWnd, 0, TRUE);
		} break;
	case WM_USER:
		return xulplayer->getVideoBgColor();
	case WM_USER + 0x1000:
		jsInvoke((char*)lParam, 0, 0);
		break;
	case WM_LBUTTONUP: break;
	case WM_TRAY_ICON_NOTIFY:
		switch (lParam) {
		case WM_MOUSEMOVE:
			//mouse over tray icon, update tooltip information here
			SetTrayIcon(hWnd, hAppIcon, NIM_MODIFY, "XULPlayer", 0, 0);
			break;
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
			//right click on tray icon
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// Restore xulplayer window and delete tray icon
			SetTrayIcon(hWnd, hAppIcon, NIM_DELETE, 0, 0, 0);
			if(pPlugin->getXulWnd()) ShowWindow(pPlugin->getXulWnd(), SW_SHOW);
			break;
		}
		break;
	case WM_TIMER:
		NotifyDriveChange();
		break;
	case WM_DEVICECHANGE:
		if ( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam ) {
			PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
			PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
			PDEV_BROADCAST_HANDLE pDevHnd;
			PDEV_BROADCAST_OEM pDevOem;
			PDEV_BROADCAST_PORT pDevPort;
			PDEV_BROADCAST_VOLUME pDevVolume;
			switch( pHdr->dbch_devicetype ) {
				case DBT_DEVTYP_DEVICEINTERFACE:
					pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
					UpdateDevice(pDevInf, wParam);
					break;

				case DBT_DEVTYP_HANDLE:
					pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
					break;

				case DBT_DEVTYP_OEM:
					pDevOem = (PDEV_BROADCAST_OEM)pHdr;
					break;

				case DBT_DEVTYP_PORT:
					pDevPort = (PDEV_BROADCAST_PORT)pHdr;
					break;

				case DBT_DEVTYP_VOLUME:
					pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
					break;
			}
		}
		break;
	default:			
		//FIXME: this will cause overhead
		jsSetVar("focusFromPlugin", true);			
		return DefWindowProc(hWnd, msg, wParam, lParam);
  }
//  if(lpOldProc) return lpOldProc(hWnd, msg, wParam, lParam);
  return 0;
}


LRESULT CALLBACK VideoWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static RECT rc;
	static int mx = 0;
	static int my = 0;
	int x, y;
	RECT rc2;

	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer || !pPlugin) return 0;

	switch (msg) {
	case WM_LBUTTONDBLCLK:
		jsInvoke("toggleFullScreen", 0, 0);
		break;
	case WM_MOUSEMOVE:
		do 
		{
			NPVariant ret = jsGetVar("fs");  
			bool bfs = NPVARIANT_TO_BOOLEAN(ret);
			if(bfs) break;

			x = LOWORD(lParam);
			y = HIWORD(lParam);
			if (wParam == MK_LBUTTON) {
				SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_SIZEALL)));
				if (!mx) {
					mx = x;
					my = y;
					return false;
				}
			} else {
				mx = 0;
				my = 0;
				SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));
				return false;
			}
			if(pPlugin) {
				GetWindowRect(hWnd, &rc);
				GetWindowRect(pPlugin->getEmbedWnd(), &rc2);
				rc.left += x - mx;
				rc.top += y - my;
				rc.right += x - mx;
				rc.bottom += y - my;
				x = rc.left - rc2.left;
				y = rc.top - rc2.top;
				if (x > - 5 && x < 5) x = 0;
				if (y > -5 && y < 5) y = 0;
				MoveWindow(hWnd, x, y, rc.right -rc.left, rc.bottom - rc.top, TRUE);
			}
		} while(false);
		break;
	case WM_LBUTTONUP:
 	case WM_RBUTTONUP: {
		int playerId = xulplayer->getIdByVideoWnd(hWnd);
		if(playerId >= 0) {
			xulplayer->setActiveId(playerId);
		}
		jsSetVar("focusFromPlugin", true);
		if(pPlugin->getXulWnd()) SetFocus(pPlugin->getXulWnd());
	}
	break;
#ifdef ENABLE_VIS
	case WM_WA_IPC:
		return MVisulation::parentWindowProc(wParam, lParam);
		break;
#endif
	
	default:
		//jsSetVar("focusFromPlugin", true);
		//if(xul_hWnd) SetFocus(xul_hWnd);
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

WNDPROC pfnXULWindowProc;

LRESULT CALLBACK XULWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN:
		break;
	case WM_DEVICECHANGE:
		{
			PDEV_BROADCAST_HDR  lpdb   =   (PDEV_BROADCAST_HDR)lParam;
			switch(wParam)
			{
			case DBT_DEVICEARRIVAL: 
				{
					if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME){ 
						PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;      
						if(lpdbv->dbcv_flags & DBTF_MEDIA) 
						{
							// is disc in rom
							char dirName = FindFirstDriveFromMask(lpdbv->dbcv_unitmask);
							std::string spath;
							spath += dirName;
							spath += ":\\";
							char volumeName[256] = {0};
							unsigned long volID = 0;
							GetVolumeInformation(spath.c_str(),volumeName,sizeof(volumeName),&volID,0,0,0,0);
							spath = "";	
							spath += dirName;
							spath += ":";
							char hex[20] = {0};
							sprintf(hex,"%x",volID);
							UpdateDisc(wParam, spath.c_str(), volumeName, hex);
						} 
					} 
				}
				break;
			case DBT_DEVICEQUERYREMOVE: 
				break; 
			case DBT_DEVICEREMOVECOMPLETE: 
				if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME){ 
					PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;      
					if(lpdbv->dbcv_flags & DBTF_MEDIA) 
					{
						// is disc in rom
						char dirName = FindFirstDriveFromMask(lpdbv->dbcv_unitmask);
						std::string spath;
						spath += dirName;
						spath += ":\\";
						char volumeName[256] = {0};
						GetVolumeInformation(spath.c_str(),volumeName,sizeof(volumeName),0,0,0,0,0);
						spath = "";	
						spath += dirName;
						spath += ":";
						UpdateDisc(wParam, spath.c_str(), volumeName, "");
					} 
				} 
				break; 
			}
		}
		break;
	}
	static UINT uMsgQueryCancelAutoPlay = RegisterWindowMessage("QueryCancelAutoPlay");
	if(msg == uMsgQueryCancelAutoPlay) 
	{  
		SetWindowLongA(hWnd, DWL_MSGRESULT, TRUE);
	}
	return CallWindowProc(pfnXULWindowProc, hWnd, msg, wParam, lParam);
}

HWND CreateVideoWindow(HWND hwndParent)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return NULL;

	WNDCLASS wc;
	ZeroMemory(&wc,sizeof(WNDCLASS));
	wc.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc = (WNDPROC)VideoWindowProc;
	wc.hInstance = hDllInst;
	wc.hbrBackground =  CreateSolidBrush(xulplayer->getVideoBgColor());
	wc.lpszClassName = VIDEO_WINDOW_CLASS;
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	UnregisterClass(wc.lpszClassName, hDllInst);
	RegisterClass(&wc);
	return CreateWindow(VIDEO_WINDOW_CLASS,NULL,WS_CHILD | WS_VISIBLE,0,0,0,0,hwndParent,NULL,hDllInst,0);
}

HWND GetRootWindow(HWND hwndChild)
{
	if(! hwndChild) return false;
	HWND tempHwnd = GetParent(hwndChild);
	HWND topHwnd = NULL;
	while (tempHwnd){
		topHwnd = tempHwnd;
		tempHwnd = GetParent(tempHwnd);
	}
	return topHwnd ? topHwnd : NULL;
}

// System msg hook
LRESULT CALLBACK SysProcHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode > 0){
		if(wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER){
			//MessageBox(NULL, "ScreenSave!", NULL, NULL);
			return TRUE;	
		}
	}
	return CallNextHookEx(hHook,nCode,wParam,lParam);
}


/*  DWORD* keyGroupMap = 0;
	int keyGroupCount = 0;
LRESULT CALLBACK KeyboardProcHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	static DWORD matchCode = 0;
	static DWORD keyLastTick = 0;
	static DWORD keyGroupCode = 0;
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
		if (GetRootWindow(GetFocus()) == xul_hWnd) {
			NPVariant arg;
			//INT32_TO_NPVARIANT(wParam, arg);	// keycode
			DWORD keyCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
			do {
				if (!matchCode) {
					// start matching
					for (int i = 0; i < keyGroupCount; i++) {
						if ((keyGroupMap[i] & 0xff) == keyCode) {
							// first code matched
							keyLastTick = GetTickCount();
							keyGroupCode = keyGroupMap[i];
							matchCode = keyGroupCode >> 8; // shift to next code
						}
					}
				} else {
					if (GetTickCount() - keyLastTick < 1000 && (matchCode & 0xff) == keyCode) {
						keyLastTick = GetTickCount();
						matchCode >>= 8;	// shift to next code
						if (!matchCode) {
							// final match
							keyCode = keyGroupCode;
						} else {
							// still has code to match, do not notify this code
							break;
						}
					} else {
						// unmatched;
						matchCode = 0;
					}
				}
				INT32_TO_NPVARIANT(keyCode, arg);
				jsInvoke("onAppKeyEvent", 1, &arg);
			} while(0);
		}
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}
*/

#ifndef _LITE
std::vector<int> ctrlKeyCodes;

LRESULT CALLBACK KeyboardProcHook(int nCode,WPARAM wParam,LPARAM lParam)
{
	static bool bCtrlKey = false;
	static std::vector<DWORD> vctKeycode;
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN && pPlugin) {
		if (GetRootWindow(GetFocus()) == pPlugin->getXulWnd()) {
			NPVariant arg;
			//INT32_TO_NPVARIANT(wParam, arg);	// keycode
			DWORD keyCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
			vctKeycode.push_back(keyCode);
			size_t i = 0;
			for (; i<ctrlKeyCodes.size(); ++i) {
				if(keyCode == ctrlKeyCodes[i]) {
					bCtrlKey = true;
					break;
				}
			}
			if(i == ctrlKeyCodes.size()) {
				DWORD finalKeyCoder = 0;
				if(bCtrlKey == true) {
					bCtrlKey = false;
					for (size_t j=0; j<vctKeycode.size(); ++j) {
						finalKeyCoder |= vctKeycode[j]<<(8*j);
					}
				} else {
					finalKeyCoder = keyCode;
				}
				INT32_TO_NPVARIANT(finalKeyCoder, arg);
				jsInvoke("onAppKeyEvent", 1, &arg);
				vctKeycode.clear();
			}
			return TRUE;
		}
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}
#endif

void InitWindow(HWND hWnd)
{
#ifdef _DEBUG
	CreateConsole();
#endif
	//hBack = LoadBitmap(hDllInst,MAKEINTRESOURCE(IDB_BACK));
	//hHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)SysProcHook, hDllInst, NULL);  // Register hook
#ifndef _LITE
	//hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProcHook, hDllInst, NULL);  // Register hook
#endif
	//MessageBox(NULL,"Hook Install NEW!", NULL, NULL);
	hAppIcon = LoadIcon(hDllInst,MAKEINTRESOURCE(IDI_MAIN));
	RegisterDeviceEvents(hWnd);
	//hDiscHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CDROMHookProc, hDllInst,0);
	if(pPlugin) {
		//pfnXULWindowProc = (WNDPROC)SetWindowLongPtr(pPlugin->getXulWnd(), GWLP_WNDPROC, (LONG)XULWindowProc);
	}
}

//========================== Resize Video Functions ===========================
void HorizontalAlignVideos(int w, int h, int playersNum)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return;
	
	int playingId = 0;
	for (int i = 0; i < playersNum; i++) {
		playingId = xulplayer->getFirstPlayingIdFromId(playingId);
		if(playingId < 0) return;
		CMPlayer* mplayer = xulplayer->getPlayer(playingId);
		if(mplayer == NULL) return;
		if (xulplayer->getWnd(playingId)) {
			int curLeft = w / playersNum * i;
			MoveWindow(xulplayer->getWnd(playingId), curLeft, 0, w / playersNum, h, TRUE);
		}
		playingId++;
	}
}

void VerticalAlignVideos(int w, int h, int playersNum)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return;

	int playingId = 0;
	for (int i = 0; i < playersNum; i++) {
		playingId = xulplayer->getFirstPlayingIdFromId(playingId);
		if(playingId < 0) return;
		CMPlayer* mplayer = xulplayer->getPlayer(playingId);
		if(mplayer == NULL) return;
		if (xulplayer->getWnd(playingId)) {
			int curTop = h / playersNum * i;
			MoveWindow(xulplayer->getWnd(playingId), 0, curTop, w, h / playersNum, TRUE);
		}
		playingId++;
	}
}

void TileAlignVideos(int w, int h, int playersNum)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return;

	int rowNum, colNum;
	if(playersNum < 3) {
		rowNum = 1;
	} else if (playersNum < 7) {
		rowNum = 2;
	} else if (playersNum < 13) {
		rowNum = 3;
	} else if (playersNum < 21) {
		rowNum = 4;
	} else if (playersNum < 31) {
		rowNum = 5;
	} else if (playersNum < 43) {
		rowNum = 6;
	} else if (playersNum < 57) {
		rowNum = 7;
	} else {
		rowNum = 8;
	}

	bool hasRemain = false;
	colNum = playersNum/rowNum;
	if(playersNum%rowNum != 0) {
		colNum += 1;
		hasRemain = true;
	}
	
	int realColNum = colNum;	// last colNum may not be the same as other row

	int playingId = 0;
	int curTop = 0;
	
	CMPlayer* mplayer = NULL;
	for (int i = 0; i < rowNum; i++) {
		if(hasRemain && i==rowNum-1) {
			realColNum = playersNum-(rowNum-1)*colNum;
		} 
		
		int maxH = 0;
		for(int j=0; j<realColNum; j++) {
			playingId = xulplayer->getFirstPlayingIdFromId(playingId);
			if(playingId < 0) return;
			mplayer = xulplayer->getPlayer(playingId);
			if(mplayer == NULL) return;
			int curW = w / colNum;
			if(mplayer->video.width <= 0) mplayer->video.width = curW;
			if(curW > mplayer->video.width) {
				curW = mplayer->video.width;
			}
			int curLeft = curW * j;
			float aspectRatio = mplayer->video.aspect;
			if(aspectRatio > -BI_EPSILON && aspectRatio < BI_EPSILON) {
				aspectRatio = xulplayer->getDefaultAspect();
			}
			int curH = (int)(curW/aspectRatio);
			if(maxH < curH) maxH = curH;
			if (xulplayer->getWnd(playingId)) {
				MoveWindow(xulplayer->getWnd(playingId), curLeft, curTop, curW, curH, TRUE);
				//xulplayer->setVideoWndPos(playingId, curLeft, curTop, curW, curH);
			}
			playingId++;
		}
		curTop += maxH;
	}
}

static int RESIZE_LOCK = 0;
void ResizeVideo(int w, int h)
{
	if(RESIZE_LOCK == 1) return;
	RESIZE_LOCK = 1;

	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer || !pPlugin) {
		RESIZE_LOCK = 0;
		return;
	}
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) {
		RESIZE_LOCK = 0;
		return;
	}

	int playerCount = xulplayer->getPlayingCount();
	float zoom = xulplayer->getZoomFactor();
	if (playerCount == 1) {
		int vw = mplayer->video.width;
		int vh = mplayer->video.height;

		bool bfs = NPVARIANT_TO_BOOLEAN(jsGetVar("fs"));
		if (bfs) zoom = 0.f;
		if (zoom > BI_EPSILON && vw > BI_EPSILON) {
			MoveWindow(xulplayer->getActiveWnd(), (int)((w-zoom*vw)/2), 
				(int)((h-zoom*vh)/2), (int)(zoom * vw), (int)(zoom * vh), TRUE);
		} else {
			MoveWindow(xulplayer->getActiveWnd(), 0, 0, w, h, TRUE);
		}
		
		if(vh <= 0 || vw <= 0) {
			RESIZE_LOCK = 0;
			return;
		}

		// Calculate the current real size of video
		float ratio = mplayer->video.aspect;
		if(ratio > -BI_EPSILON && ratio < BI_EPSILON) ratio = (float)vw/vh;

		RECT rc;
		int videoRealW = 0, videoRealH = 0;
		GetClientRect(xulplayer->getActiveWnd(), &rc);
		int originW = rc.right - rc.left, originH = rc.bottom - rc.top;
		int temp = (int)(originW/ratio); videoRealH = originH < temp ? originH : temp;
		temp = (int)(originH*ratio); videoRealW = originW < temp ? originW : temp;
		xulplayer->setCurrentRealVideoSize(videoRealW, videoRealH);
		
		MoveWindow(xulplayer->getActiveWnd(), (w - videoRealW) / 2, (h - videoRealH) / 2, videoRealW, videoRealH, TRUE);
	} else {
		//HorizontalAlignVideos(w, h, playerCount);
		//VerticalAlignVideos(w, h, playerCount);
		TileAlignVideos(w, h, playerCount);
	}
	RESIZE_LOCK = 0;
}
//============================================================================================

void UninitWindow(HWND hWnd)
{
	UnregisterDeviceEvents();
#ifndef _LITE
	UnhookWindowsHookEx(hKeyboardHook);		//Destroy hook
#endif
	UnhookWindowsHookEx(hHook);				//Destroy hook
	//if (keyGroupMap) free(keyGroupMap);
}

// dwTrayMsg: NIM_ADD, NIM_MODIFY, NIM_DELETE
bool SetTrayIcon(HWND hwnd, HICON hIcon, DWORD dwTrayMsg, TCHAR* tips, TCHAR* info, TCHAR* title)
{
	static NOTIFYICONDATA	m_nidIconData;
	memset(&m_nidIconData, 0, sizeof(m_nidIconData));
	m_nidIconData.cbSize			= sizeof(NOTIFYICONDATA);
	m_nidIconData.hWnd				= hwnd;
	m_nidIconData.uID				= 1;
	m_nidIconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY;
	m_nidIconData.szTip[0]			= 0;	
	m_nidIconData.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nidIconData.hIcon				= hIcon;
	if (tips) {
		strncpy(m_nidIconData.szTip, tips, sizeof(m_nidIconData.szTip) / sizeof(m_nidIconData.szTip[0] - 1));
	} else {
		GetWindowText(hwnd, m_nidIconData.szTip, sizeof(m_nidIconData.szTip) / sizeof(m_nidIconData.szTip[0]));
	}
	if (info) {
		m_nidIconData.uFlags |= NIF_INFO;
		m_nidIconData.dwInfoFlags = NIIF_NOSOUND;
		if (title) strncpy(m_nidIconData.szInfoTitle, title, sizeof(m_nidIconData.szInfoTitle) / sizeof(m_nidIconData.szInfoTitle[0] - 1));
		strncpy(m_nidIconData.szInfo, info, sizeof(m_nidIconData.szInfo) / sizeof(m_nidIconData.szInfo[0] - 1));
		m_nidIconData.uTimeout = 3000;
	}
	return Shell_NotifyIcon(dwTrayMsg, &m_nidIconData) == TRUE;
}
