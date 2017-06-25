#ifndef _PROCESSWRAPPER
#define _PROCESSWRAPPER

#include "PCMacro.h"
_MC_PC_BEGIN

#define SF_ALLOC 0x1
#define SF_SHOW_WINDOW 0x2
#define SF_WAIT_IDLE 0x4
#define SF_REDIRECT_STDIN 0x1000
#define SF_REDIRECT_STDOUT 0x2000
#define SF_REDIRECT_STDERR 0x4000
#define SF_REDIRECT_OUTPUT (0x8000 | SF_REDIRECT_STDOUT)

#ifdef WIN32
#define SF_WIN32 0x10000
#endif

class MC_PC_EXT CProcessWrapper
{
public:
	CProcessWrapper(int flags = 0);
	~CProcessWrapper()
	{
		Cleanup();
	}
	void Cleanup();
	int Write(const void* data, size_t len = 0);
	int Read(void* buffer, int bufsize);
	int LoopRead(void* buffer, size_t bufsize);
	int ReadStdout();
	int ReadStderr();
	bool Create(const char* commandLine, const char* curDir = 0, const char** env = 0, bool hasGui = false);
	bool SetPriority(int priority);
	int Wait(int timeout = 0);
	static char** Tokenize(char* str, char delimiter = ' ');
	void CloseStdout();
	void CloseStdin();
	void CloseStderr();
	void AllocateBuffer(int bytes);
	void FreeBuffer();
	bool IsProcessRunning(int* pexitcode = 0);
	bool Terminate();
	bool Suspend(bool resume);
#ifdef WIN32
	bool Spawn(const char* commandLine, const char* curDir = 0, const char** env = 0);
	HWND GetWindowHandle();
	int WaitIdle(int milliseconds);
	static int Run(const char* commandLine, const char* curDir = 0, bool hidden = true);
	bool SetAffinity(int mask);
	void AddEnvVariable(char* envName, char* envValue);
	void PurgeVariable();
	DWORD GetExitCode();	 
	static int GetProcessIdByHandle(HANDLE hProcess);
	static bool MakePipe(int& readpipe, int& writepipe, int bufsize = 16384, bool inheritRead = false, bool inheritWrite = false, int flags = -1);
	HANDLE hStdinWrite;
	HANDLE hStdoutRead;
	HANDLE hStderrRead;
	PROCESS_INFORMATION procInfo;
	HWND hWnd;
#else
    int m_pid;
#endif
	int flags;
	char *conbuf;
	int conbufsize;
	int fdStdinWrite;
	int fdStdoutRead;
	int fdStderrRead;
private:
#ifdef WIN32
	int ReadOutput(HANDLE handle);
	static BOOL CALLBACK EnumWindowCallBack(HWND hwnd, LPARAM lParam);
#endif
	int ReadOutput(int fd);
	char *pchCommandLine;
	char *pchPath;
	int envsize;
};

_MC_PC_END
#endif
