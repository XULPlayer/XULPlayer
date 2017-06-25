#ifndef _UI_H_
#define _UI_H_

#ifdef WIN32
void InitWindow(HWND hWnd);
HWND GetRootWindow(HWND hwndChild);
void ResizeVideo(int w, int h);
HWND CreateVideoWindow(HWND hwndParent);
bool SetTrayIcon(HWND hwnd, HICON hIcon, DWORD dwTrayMsg, TCHAR* tips, TCHAR* info, TCHAR* title);
#else
HWND CreateVideoWindow(HWND hwndParent);
bool DestroyWindow(HWND hWnd);
bool ShowWindow(HWND hWnd, int nCmdShow);
#define SW_SHOW 0
#endif

#endif
