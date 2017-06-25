#ifndef REGLIB_H
#define REGLIB_H

#include <tchar.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------Export functions-----------------

	//Operation on the Rootkey, must call SetRootKey first to change rootkey.
	 BOOL SetRootKey(HKEY hRootKey);
	 BOOL CreateKey(const TCHAR* strKey);
	 BOOL DeleteKey(const TCHAR* strKey);
	 BOOL DeleteValue(const TCHAR* strName);
	 int GetDataSize(const TCHAR* strValueName);
	 DWORD GetDataType(const TCHAR* strValueName);
	 int GetSubKeyCount();
	 int GetValueCount();
	 BOOL KeyExists(const TCHAR* strKey, HKEY hRootKey = NULL);
	 BOOL SetKey(const TCHAR* strKey, BOOL bCanCreate);
	 BOOL ValueExists(const TCHAR* strName);

	// data reading functions
	 double ReadFloat(const TCHAR* strName, double fDefault);
	 BOOL ReadString(const TCHAR* strName, const TCHAR* strDefault, TCHAR* retStr);
	 int ReadInt(const TCHAR* strName, int nDefault);
	 BOOL ReadBool(const TCHAR* strName, BOOL bDefault);
	 COLORREF ReadColor(const TCHAR* strName, COLORREF rgbDefault);
	 DWORD ReadDword(const TCHAR* strName, DWORD dwDefault);

	// data writing functions
	 BOOL WriteBool(const TCHAR* strName, BOOL bValue);
	 BOOL WriteString(const TCHAR* strName, const TCHAR* strValue);
	 BOOL WriteFloat(const TCHAR* strName, double fValue);
	 BOOL WriteInt(const TCHAR* strName, int nValue);
	 BOOL WriteColor(const TCHAR* strName, COLORREF rgbValue);
	 BOOL WriteDword(const TCHAR* strName, DWORD dwValue);	

#ifdef __cplusplus
}
#endif

#endif