#ifndef __LINUX_CONFIG_H__
#define __LINUX_CONFIG_H__

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LINUX_DEF_VERSION 4.2

#if 0
int fopen_s(FILE **fpp, const char *filename, const char *mode);
int strcpy_s(char *dst, size_t n, const char *src);
int strncpy_s(char *dst, size_t dst_n, const char *src, size_t src_n);
char* itoa( int value, char* result, int base );
char* _itoa_s( int value, char* result, int len, int base );
int GetModuleFileName( char* sModuleName, char* sFileName, int nSize);
bool FileExist(const char* psz_filename);
int ShellExe(char *cmd);
#endif

#ifdef _DEBUG
#define DD printf("%s, %s(%d)\n", __FILE__,__FUNCTION__ , __LINE__)
#else
//#define DD FIXME
#define DD printf("%s, %s(%d)\n", __FILE__,__FUNCTION__ , __LINE__)
#endif

#define _open open
#define _lseek lseek
#define _write write
#define _read read
#define _close close

#define _O_CREAT O_CREAT
#define _O_WRONLY O_WRONLY
#define _S_IWRITE S_IWRITE
#define _S_IREAD S_IREAD
#define _O_RDONLY O_RDONLY 
#define _O_BINARY 0
#define _O_SEQUENTIAL 0

#define strtok_s strtok_r
#define _strdup strdup
#define sprintf_s snprintf
#define _itoa itoa
#define _stricmp strcasecmp
#define _snprintf snprintf
#define _abs64 llabs
//DWORD on x86_64 ?
#define __stdcall
#ifndef MAX_PATH
	#define MAX_PATH 260
#endif

#define HWND long
#define HANDLE long
#define HINSTANCE long

#ifndef TRUE
	#define TRUE (1==1)
#endif
#ifndef FALSE
	#define FALSE (1==0)
#endif

typedef int BOOL;
typedef int64_t __int64;

//min
//#define min(a,b) ((a)<(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
//max
//#define max(a,b) ((a)>(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef unsigned int DWORD;
typedef void * HMODULE;

#define WINAPI  

//////////////////////////////////////////////////////////////////////////////////////
inline void Sleep(int ms)
{
   usleep(1000*ms); 
}

inline void reverse(char *str)
{
    char *p;

    if (str == NULL || *str == '\0') return;

    p = str + strlen(str) - 1;
    while (str < p) {
        char t = *str;
        *str = *p;
        *p = t;
        str++; p--;
    }
}

inline int fopen_s(FILE **fpp, const char *filename, const char *mode)
{
    *fpp = fopen(filename, mode);
    if (*fpp) return 0;
    else return -1;
}

inline int strcpy_s(char *dst, size_t n, const char *src)
{
    char *ret = strncpy(dst, src, n);
    if (ret) return 0;
    else return -1;
}

inline int strncpy_s(char *dst, size_t dst_n, const char *src, size_t src_n)
{
    size_t n = src_n > dst_n ? dst_n : src_n;

    char *ret = strncpy(dst, src, n);
    if (ret) return 0;
    else return -1;
}

inline char* itoa( int value, char* result, int base )
{
	
    // check that the base if valid
	if (base < 2 || base > 16) { *result = 0; return result; }
	
	char* out = result;
	int quotient = value;
	
	do {
	
		*out = "0123456789abcdef"[ abs( quotient % base ) ];
		++out;
		quotient /= base;
	} while ( quotient );
	
	
	// Only apply negative sign for base 10
	if ( value < 0 && base == 10) *out++ = '-';
	
	reverse( result );
	*out = 0;
	return result;
}

inline char* _itoa_s( int value, char* result, int len, int base )
{
	
    // check that the base if valid
	if (base < 2 || base > 16) { *result = 0; return result; }
	
	char* out = result;
	int quotient = value;
	
	do {
        if (out >= result + len) return NULL;
		*out = "0123456789abcdef"[ abs( quotient % base ) ];
		++out;
		quotient /= base;
	} while ( quotient );
	
	
	// Only apply negative sign for base 10
	if ( value < 0 && base == 10) *out++ = '-';
	
	reverse( result );
	*out = 0;
	return result;
}

inline int GetModuleFileName( char* sModuleName, char* sFileName, int nSize)
{
    int ret = -1;
    char sLine[1024] = { 0 };
    void* pSymbol = (void*)"";
    FILE *fp;
    char *pPath;
    
    fp = fopen ("/proc/self/maps", "r");
    if ( fp != NULL ) {
        while (!feof (fp)) {
            unsigned long start, end;
            char *tmp;
            size_t len, m_len;
            
            if ( !fgets (sLine, sizeof (sLine), fp)) continue;
            if ( !strstr (sLine, " r-xp ") || !strchr (sLine, '/')) continue;
            
            /* Get rid of the newline */
            tmp = strrchr (sLine, '\n');
            if (tmp) *tmp = 0;

            if (sModuleName && *sModuleName) {
                len = strlen(sLine);
                m_len = strlen(sModuleName);
#if 0
                if (m_len > len || strcmp(sLine + len - m_len, sModuleName)) continue;
#else
                if (strstr(sLine, sModuleName) == NULL) continue;
#endif
            }
            else {
                sscanf (sLine, "%lx-%lx ", &start, &end);
                if (pSymbol < (void *) start || pSymbol >= (void *) end) continue;
            }
                
            /* Extract the filename; it is always an absolute path */
            pPath = strchr (sLine, '/');

            /* Get rid of "(deleted)" */
                
            len = strlen (pPath);
            if (len > 10 && strcmp (pPath + len - 10, " (deleted)") == 0) {
                tmp = pPath + len - 10;
                *tmp = 0;
            }
            ret = 0;
            strncpy( sFileName, pPath, nSize );
            fclose (fp);
            return ret;
        }

        fclose (fp);
    }
    
    return ret;
}

inline bool FileExist(const char *psz_filename)
{
    struct stat sb;

    return stat(psz_filename, &sb) == 0;
}

inline bool isFolderExist(const char *sptr)
{
	struct stat sb;
	
	if (stat(sptr, &sb) != 0) return false;

	return (sb.st_mode & S_IFDIR) != 0;
}

inline bool isFileExist(const char *sptr)
{
	struct stat sb;
	
	if (stat(sptr, &sb) != 0) return false;

	return (sb.st_mode & S_IFREG) != 0;
}

inline int ShellExe(char *cmd)
{
    pid_t pid;
    pid=fork();
    if (pid == 0)
    {
        if (system(cmd)<0) return -1;
        else return 1;
    }
    else if (pid > 0) return 2;
    else return 0;
}

#include <dlfcn.h>
inline HMODULE LoadLibrary(const char *filename)
{
	return dlopen(filename, RTLD_LAZY);
}

inline void* GetProcAddress(HMODULE handle, const char *symbol)
{
	return dlsym(handle, symbol);
}

inline int FreeLibrary(HMODULE handle)
{
	return dlclose(handle);
}

#include<sys/time.h>
#include<unistd.h>
//return ms
inline int64_t GetTickCount()
{
	struct timeval tv;
	int64_t ret;

	if (gettimeofday(&tv, NULL) != 0) return 0;

	ret = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return ret;
}

inline BOOL MoveFile(const char * psz_src, const char *psz_dst)
{
	char psz_cmd[MAX_PATH*2];

	if (psz_src == NULL || *psz_src == '\0' || 
		psz_dst == NULL || *psz_dst == '\0') return FALSE;
	snprintf(psz_cmd, sizeof(psz_cmd), "mv %s %s", psz_src, psz_dst);

	return (system(psz_cmd) == 0)? TRUE:FALSE;
}

inline BOOL DeleteFile(const char * psz_pathname)
{
#if 1
	if (psz_pathname == NULL || *psz_pathname == '\0') return FALSE;
//or unlink()
	return (remove(psz_pathname) == 0)? TRUE:FALSE;
#else
	return TRUE;
#endif
}

inline int _mkdir(const char *psz_pathname)
{
	return mkdir(psz_pathname, 755);
}
#endif

