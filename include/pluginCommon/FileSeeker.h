#ifndef _CFILESEEKER
#define _CFILESEEKER

#include "PCMacro.h"
#include <string>
#include <list>

#ifdef __linux__
#include <pthread.h>
#endif

_MC_PC_BEGIN

enum FILEFIND_STATUS
{
	FFS_Idel = 0,
	FFS_Searching,
	FFS_Pause,
	FFS_Finished,
	FFS_Failed,
	FFS_Aborted,
};

class MC_PC_EXT CFileSeeker
{
public:	
	~CFileSeeker(void);	
	bool				begin(const char* startPath, const char* skipPaths = 0);
	int					pause();
	void				end();
	void				cleanUp();
	bool				enumFile(const char* startDir);
	bool				isThreadRunning();
	const char*			dumpFiles(const char* filter);	
	std::string			popFile();
	std::string			popDir();
	static CFileSeeker* getInstance(){static CFileSeeker fileSeeker; return &fileSeeker;}
	
	std::string				startPath;
	std::string				skipPath;
	FILEFIND_STATUS			status;
	int						fileCount;
	int						dirCount;
	int						dumpedFileCount;
private:
	CFileSeeker(void);	

	std::list<std::string>	fileList;
	std::list<std::string>	dirList;
#ifndef __linux__
	HANDLE					hThread;
#else
	pthread_t				hThread;
#endif
	std::string				buffer;
};

_MC_PC_END
#endif
