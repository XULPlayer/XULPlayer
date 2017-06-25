#ifndef _MVISULATION
#define _MVISULATION

#include "PCMacro.h"
#include "vis/wa_ipc.h"

struct winampVisModule;

_MC_PC_BEGIN

class MC_PC_EXT MVisulation
{
public:	
	~MVisulation(void);
	bool	init(HWND hWnd, int vis_type = 0);
	void    update(int sampleRate, int channelNum);
	void    render(char*, int);
	void    resizeWnd();
	int		render();
	void	quit();
	static	LRESULT parentWindowProc(WPARAM wParam, LPARAM lParam);
	static	MVisulation* getInstance(){static MVisulation visulation_maker; return &visulation_maker;}

	embedWindowState	*pWndState;
	HWND				hParentWnd;
	HWND				hPluginWnd;
private:
	MVisulation(void);
	HMODULE				hModule;
	winampVisModule		*pWnpModule;	
};

_MC_PC_END

#endif