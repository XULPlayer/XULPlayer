// RegLib.cpp : Defines the entry point for the DLL application.
//

#include "RegDefine.h"

#define PATH_LEN 255 

#ifndef ASSERT
#define ASSERT(x) {if(!(x)) MessageBox(NULL, _T("Error!"), NULL, NULL);}
#endif

static HKEY m_hRootKey = HKEY_CURRENT_USER;
static BOOL m_bLazyWrite = TRUE;
static DWORD m_nLastError = ERROR_SUCCESS;
static TCHAR m_strCurrentPath[PATH_LEN] = {0};

/*BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch(   ul_reason_for_call   )   {  
          case   DLL_PROCESS_ATTACH: 
			  break;
                  
          case   DLL_THREAD_ATTACH:  
			  break;
          case   DLL_THREAD_DETACH: 
			  break;
                 
          case   DLL_PROCESS_DETACH: 
			  break;  
          }   
	return TRUE;
}*/

void SetCurrentPath(const TCHAR* strKey)
{
	if(lstrlen(m_strCurrentPath) > 0)
	{
		ZeroMemory(m_strCurrentPath, PATH_LEN);
	}
#ifdef _UNICODE
	wstrncpy(m_strCurrentPath, LPCTSTR(strKey), lstrlen(strKey));
#else
	strncpy(m_strCurrentPath, LPCTSTR(strKey), lstrlen(strKey));
#endif
}

 BOOL SetRootKey(HKEY hRootKey)
{
	if (hRootKey != HKEY_CLASSES_ROOT &&
		hRootKey != HKEY_CURRENT_USER &&
		hRootKey != HKEY_LOCAL_MACHINE &&
		hRootKey != HKEY_USERS) return FALSE;

	m_hRootKey = hRootKey;
	return TRUE;
}


 BOOL CreateKey(const TCHAR* strKey)
{
	/* Use CreateKey to add a new key to the registry. 
		Key is the name of the key to create. Key must be 
		an absolute name. An absolute key 
		begins with a backslash (\) and is a subkey of 
		the root key. */

	ASSERT(strKey[0] != _T('\\'));
	if(strKey[0] == _T('\\')) return FALSE;

	HKEY hKey;

	DWORD dwDisposition = 0;

	if (::RegCreateKeyEx(m_hRootKey, LPCTSTR(strKey), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
			&dwDisposition)	!= ERROR_SUCCESS) return FALSE;
	
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);

	SetCurrentPath(strKey);

	return TRUE;
}


 BOOL DeleteKey(const TCHAR* strKey)
{
	/* Call DeleteKey to remove a specified key and its associated data, 
	if any, from the registry. Returns FALSE is there are subkeys
	Subkeys must be explicitly deleted by separate calls to DeleteKey.
	DeleteKey returns True if key deletion is successful. On error, 
	DeleteKey returns False. */
	ASSERT(strKey[0] != _T('\\'));
	if(strKey[0] == _T('\\')) return FALSE;

	if (!KeyExists(strKey)) return TRUE;
	if (::RegDeleteKey(m_hRootKey, strKey) != ERROR_SUCCESS) return FALSE;
	return TRUE;
}



 BOOL DeleteValue(const TCHAR* strName)
{
	/* Call DeleteValue to remove a specific data value 
		associated with the current key. Name is string 
		containing the name of the value to delete. Keys can contain 
		multiple data values, and every value associated with a key 
		has a unique name. */

	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	HKEY hKey;
	LONG lResult;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) return FALSE;

	lResult = ::RegDeleteValue(hKey, LPCTSTR(strName));
	::RegCloseKey(hKey);

	if (lResult == ERROR_SUCCESS) return TRUE;
	return FALSE;
}


 int GetDataSize(const TCHAR* strValueName)
{
	/* Call GetDataSize to determine the size, in bytes, of 
	a data value associated with the current key. ValueName 
	is a string containing the name of the data value to query.
	On success, GetDataSize returns the size of the data value. 
	On failure, GetDataSize returns -1. */

	HKEY hKey;
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	LONG lResult;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return -1;

	DWORD dwSize = 1;
	lResult = ::RegQueryValueEx(hKey, LPCTSTR(strValueName),
		NULL, NULL, NULL, &dwSize);
	::RegCloseKey(hKey);

	if (lResult != ERROR_SUCCESS) return -1;
	return (int)dwSize;
}

 DWORD GetDataType(const TCHAR* strValueName)
{
	HKEY hKey;
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return 0;

	m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_QUERY_VALUE, &hKey);

	if (m_nLastError != ERROR_SUCCESS) return 0;

	DWORD dwType = 1;
	m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strValueName),
		NULL, &dwType, NULL, NULL);
	::RegCloseKey(hKey);		

	if (m_nLastError == ERROR_SUCCESS) return dwType;

	return 0;
}



 int GetSubKeyCount()
{
	/* Call this function to determine the number of subkeys.
		the function returns -1 on error */
	HKEY hKey;
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return -1;

	LONG lResult;
	DWORD dwSubKeyCount, dwValueCount, dwClassNameLength,
		dwMaxSubKeyName, dwMaxValueName, dwMaxValueLength;
	FILETIME ftLastWritten;

	TCHAR szClassBuffer[PATH_LEN];
		
	dwClassNameLength = PATH_LEN;
	lResult = ::RegQueryInfoKey(hKey, szClassBuffer, &dwClassNameLength,
		NULL, &dwSubKeyCount, &dwMaxSubKeyName, NULL, &dwValueCount,
		&dwMaxValueName, &dwMaxValueLength, NULL, &ftLastWritten);
				
	::RegCloseKey(hKey);
	if (lResult != ERROR_SUCCESS) return -1;

	return (int)dwSubKeyCount;
}


 int GetValueCount()
{
	/* Call this function to determine the number of subkeys.
		the function returns -1 on error */
	HKEY hKey;
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return -1;

	LONG lResult;
	DWORD dwSubKeyCount, dwValueCount, dwClassNameLength,
		dwMaxSubKeyName, dwMaxValueName, dwMaxValueLength;
	FILETIME ftLastWritten;

	TCHAR szClassBuffer[PATH_LEN];
		
	dwClassNameLength = PATH_LEN;
	lResult = ::RegQueryInfoKey(hKey, szClassBuffer, &dwClassNameLength,
		NULL, &dwSubKeyCount, &dwMaxSubKeyName, NULL, &dwValueCount,
		&dwMaxValueName, &dwMaxValueLength, NULL, &ftLastWritten);
				
	::RegCloseKey(hKey);
	if (lResult != ERROR_SUCCESS) return -1;

	return (int)dwValueCount;
}


 BOOL KeyExists(const TCHAR* strKey, HKEY hRootKey)
{
	/* Call KeyExists to determine if a key of a specified name exists.
		 Key is the name of the key for which to search. */

	ASSERT(strKey[0] != _T('\\'));
	if(strKey[0] == _T('\\')) return FALSE;

	HKEY hKey;

	if (hRootKey == NULL) hRootKey = m_hRootKey;
	
	LONG lResult = ::RegOpenKeyEx(hRootKey, LPCTSTR(strKey), 0,
		KEY_ALL_ACCESS, &hKey);
	::RegCloseKey(hKey);
	if (lResult == ERROR_SUCCESS) return TRUE;
	return FALSE;
}

 BOOL SetKey(const TCHAR* strKey, BOOL bCanCreate)
{
	/* Call SetKey to make a specified key the current key. Key is the 
		name of the key to open. If Key is null, the CurrentKey property 
		is set to the key specified by the RootKey property.

		CanCreate specifies whether to create the specified key if it does 
		not exist. If CanCreate is True, the key is created if necessary.

		Key is opened or created with the security access value KEY_ALL_ACCESS. 
		OpenKey only creates non-volatile keys, A non-volatile key is stored in 
		the registry and is preserved when the system is restarted. 

		OpenKey returns True if the key is successfully opened or created */

	ASSERT(strKey[0] != _T('\\'));
	if(strKey[0] == _T('\\')) return FALSE;

	HKEY hKey;

	DWORD dwDisposition;
	if (bCanCreate) // open the key with RegCreateKeyEx
	{
		if (::RegCreateKeyEx(m_hRootKey, LPCTSTR(strKey), 0, NULL, 
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
				&dwDisposition) != ERROR_SUCCESS) return FALSE;
		SetCurrentPath(strKey);
		if (!m_bLazyWrite) ::RegFlushKey(hKey);
		::RegCloseKey(hKey);	
		return TRUE;
	}

	// otherwise, open the key without creating
	// open key requires no initial slash
	m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(strKey), 0,
		KEY_ALL_ACCESS, &hKey);
	if (m_nLastError != ERROR_SUCCESS) return FALSE;
	SetCurrentPath(strKey);
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return TRUE;
}


 BOOL ValueExists(const TCHAR* strName)
{
	/* Call ValueExists to determine if a particular key exists in 
		the registry. Calling Value Exists is especially useful before 
		calling other TRegistry methods that operate only on existing keys.

		Name is the name of the data value for which to check.
	ValueExists returns True if a match if found, False otherwise. */

	HKEY hKey;
	LONG lResult;
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return FALSE;

	lResult = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		NULL, NULL, NULL);
	::RegCloseKey(hKey);

	if (lResult == ERROR_SUCCESS) return TRUE;
	return FALSE;
}


 double ReadFloat(const TCHAR* strName, double fDefault)
{
	/* Call ReadFloat to read a float value from a specified 
		data value associated with the current key. Name is the name 
		of the data value to read.
		
		If successful, ReadFloat returns a double value. 
		On error, an exception is raised, and the value returned by 
		this function should be discarded. */

	DWORD dwType = REG_BINARY;
	double d;
	DWORD dwSize = sizeof(d);
	HKEY hKey;

	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return 0;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS) return fDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS) d = fDefault;
	::RegCloseKey(hKey);	
	return d;
}

 BOOL ReadString(const TCHAR* strName, const TCHAR* strDefault, TCHAR* retStr)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = 255;
	BOOL bSuccess = TRUE;
	TCHAR sz[PATH_LEN];
	HKEY hKey;
	
								 
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	// make sure it is the proper type
	dwType = GetDataType(strName);
	
	if (dwType != REG_SZ && dwType != REG_EXPAND_SZ)
	{
		return FALSE;
	}

	m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey);
	if (m_nLastError != ERROR_SUCCESS) return FALSE;

	m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)sz, &dwSize);
	if (m_nLastError != ERROR_SUCCESS) bSuccess = FALSE;
	::RegCloseKey(hKey);	
	if(bSuccess) _tcsncpy(retStr, sz, lstrlen(sz));
	return bSuccess;
}

 DWORD ReadDword(const TCHAR* strName, DWORD dwDefault)
{
	DWORD dwType = REG_DWORD;
	DWORD dw;
	DWORD dwSize = sizeof(dw);
	HKEY hKey;

	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS) return dwDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)&dw, &dwSize) != ERROR_SUCCESS) dw = dwDefault;
	::RegCloseKey(hKey);	
	return dw;
}



 int ReadInt(const TCHAR* strName, int nDefault)
{
	DWORD dwType = REG_BINARY;
	int n;
	DWORD dwSize = sizeof(n);
	HKEY hKey;

	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS) return nDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)&n, &dwSize) != ERROR_SUCCESS) n = nDefault;
	::RegCloseKey(hKey);	
	return n;
}

 BOOL ReadBool(const TCHAR* strName, BOOL bDefault)
{
	DWORD dwType = REG_BINARY;
	BOOL b;
	DWORD dwSize = sizeof(b);
	HKEY hKey;

	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS) return bDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)&b, &dwSize) != ERROR_SUCCESS) b = bDefault;
	::RegCloseKey(hKey);	
	return b;
}


 COLORREF ReadColor(const TCHAR* strName, COLORREF rgbDefault)
{
	DWORD dwType = REG_BINARY;
	COLORREF rgb;
	DWORD dwSize = sizeof(rgb);
	HKEY hKey;

	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS) return rgbDefault;

	if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
		&dwType, (LPBYTE)&rgb, &dwSize) != ERROR_SUCCESS) rgb = rgbDefault;
	::RegCloseKey(hKey);	
	return rgb;
}


 BOOL WriteBool(const TCHAR* strName, BOOL bValue)
{
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	BOOL bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
	
	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&bValue, sizeof(bValue))
		 != ERROR_SUCCESS) bSuccess = FALSE;
		
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

 BOOL WriteString(const TCHAR* strName, const TCHAR* strValue)
{
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	BOOL bSuccess = TRUE;
	HKEY hKey;
	TCHAR sz[PATH_LEN];
	
	if (lstrlen(strValue) > 254) return FALSE;

#ifdef _UNICODE
	wstrcpy(sz, LPCTSTR(strValue));
#else
	strcpy(sz, LPCTSTR(strValue));
#endif

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
	
        if (::RegSetValueEx(hKey, LPCTSTR(strName), NULL,
		REG_SZ, (LPBYTE)sz, (lstrlen(sz)+1)*sizeof(TCHAR))
		 != ERROR_SUCCESS) bSuccess = FALSE;
		
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}


 BOOL WriteFloat(const TCHAR* strName, double fValue)
{
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	BOOL bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
	
	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&fValue, sizeof(fValue))
		 != ERROR_SUCCESS) bSuccess = FALSE;
		
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

 BOOL WriteInt(const TCHAR* strName, int nValue)
{
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	BOOL bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
	
	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&nValue, sizeof(nValue))
		 != ERROR_SUCCESS) bSuccess = FALSE;
		
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

 BOOL WriteDword(const TCHAR* strName, DWORD dwValue)
{
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	BOOL bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
	
	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&dwValue, sizeof(dwValue))
		 != ERROR_SUCCESS) bSuccess = FALSE;
		
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}

 BOOL WriteColor(const TCHAR* strName, COLORREF rgbValue)
{
	ASSERT(lstrlen(m_strCurrentPath) > 0);
	if(lstrlen(m_strCurrentPath) == 0) return FALSE;

	BOOL bSuccess = TRUE;
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
		KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
	
	if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
		REG_BINARY, (LPBYTE)&rgbValue, sizeof(rgbValue))
		 != ERROR_SUCCESS) bSuccess = FALSE;
		
	if (!m_bLazyWrite) ::RegFlushKey(hKey);
	::RegCloseKey(hKey);
	return bSuccess;
}





