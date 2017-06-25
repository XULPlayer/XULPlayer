#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VALUE_NAME 512
 
int ReadRegKey(HKEY hRootKey, char* pszKey, int index, char* pBuf, DWORD cbBuf) 
{ 
	int retCode;
    char achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
	HKEY hKey;

	if( RegOpenKeyEx(hRootKey, pszKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return -1;

	retCode = RegEnumValue(hKey, index, achValue, &cchValue, NULL, NULL,(LPBYTE)pBuf, &cbBuf);
	return retCode == ERROR_SUCCESS ? 0 : -1;
}

BOOL FileExist(char* filename)
{
	WIN32_FIND_DATA f;
	HANDLE hFind = FindFirstFile(filename,&f);
	if (hFind != INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		return TRUE;
	} else {
		return FALSE;
	}
}

int CheckFF()
{
	char ffver[32];
	char buf[128];
	if (!ReadRegKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Mozilla\\Mozilla Firefox", 1, ffver, sizeof(ffver))) {
		char *p = strchr(ffver, ' ');
		if (p) *p = 0;
		_snprintf(buf, MAX_PATH, "SOFTWARE\\Mozilla\\Mozilla Firefox %s\\bin", ffver);
		if (!ReadRegKey(HKEY_LOCAL_MACHINE, buf, 0, buf, MAX_PATH) && FileExist(buf)) {
			char *p = strchr(ffver, ' ');
			if (p) *p = 0;
			return atoi(ffver);
		}
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	int ffver = CheckFF();
	return ffver;
}
