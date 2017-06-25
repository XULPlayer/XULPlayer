#ifdef WIN32
#include <windows.h>
#else
#include "linux_def.h"
#endif

#include <string>
#include "plugin.h"
#include "strpro/charset.h"
#include <list>

using namespace std;

NPVariant jsInvoke(const char* funcname, int argc, NPVariant argv[]);

#ifdef WIN32
int EnumDir(const char* pchDir, const char* funcEnumDir, int level)
{
	WIN32_FIND_DATA findData;
	string fullpath(pchDir);
	HANDLE hFind = INVALID_HANDLE_VALUE;
	int ret = 0;
	if(*(fullpath.rbegin()) != '\\') {
		fullpath += "\\";
	} 
	
	string anyFile = fullpath + "*";
	hFind = FindFirstFile(anyFile.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return -1;
	
	do {
		if (findData.cFileName[0]=='.' || *(WORD*)(findData.cFileName+1)=='.') continue;
		string filePath = fullpath + findData.cFileName;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			ret=0;
			if (level == -1 || level > 0) 
				ret = EnumDir(filePath.c_str(), funcEnumDir, level == -1 ? -1 : level - 1);
			if (ret)
				break;
			else
				continue;
		}
		char* p = strrchr(findData.cFileName,'.');
		if (p++) {
			
			NPVariant arg[2];
			StrPro::CCharset chSet;
			STRINGZ_TO_NPVARIANT(MemAllocStrdup(chSet.ANSItoUTF8(filePath.c_str()).c_str()), arg[0]);
			INT32_TO_NPVARIANT((findData.nFileSizeLow >> 10) + (findData.nFileSizeHigh << 22), arg[1]);
			jsInvoke(funcEnumDir, sizeof(arg) / sizeof(arg[0]), arg);
		}
	} while (FindNextFile(hFind, &findData));
	FindClose(hFind);
	return ret;
}
#endif


int GetVolume()
{
#ifdef WIN32
    DWORD                           dwVolume = -1;
    MMRESULT                        result;
    HMIXER                          hMixer;
    MIXERLINE                       ml   = {0};
    MIXERLINECONTROLS               mlc  = {0};
    MIXERCONTROL                    mc   = {0};
    MIXERCONTROLDETAILS             mcd  = {0};
    MIXERCONTROLDETAILS_UNSIGNED    mcdu = {0};


    // get a handle to the mixer device
    result = mixerOpen(&hMixer, 0, 0, 0, MIXER_OBJECTF_HMIXER);
    if (MMSYSERR_NOERROR == result)
    {
        ml.cbStruct        = sizeof(MIXERLINE);
        ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

        // get the speaker line of the mixer device
        result = mixerGetLineInfo((HMIXEROBJ) hMixer, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE);
        if (MMSYSERR_NOERROR == result)
        {
            mlc.cbStruct      = sizeof(MIXERLINECONTROLS);
            mlc.dwLineID      = ml.dwLineID;
            mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
            mlc.cControls     = 1;
            mlc.pamxctrl      = &mc;
            mlc.cbmxctrl      = sizeof(MIXERCONTROL);

            // get the volume controls associated with the speaker line
            result = mixerGetLineControls((HMIXEROBJ) hMixer, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
            if (MMSYSERR_NOERROR == result)
            {
                mcd.cbStruct    = sizeof(MIXERCONTROLDETAILS);
                mcd.hwndOwner   = 0;
                mcd.dwControlID = mc.dwControlID;
                mcd.paDetails   = &mcdu;
                mcd.cbDetails   = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                mcd.cChannels   = 1;

                // get the volume
                result = mixerGetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);
                if (MMSYSERR_NOERROR == result)
                    dwVolume = mcdu.dwValue;
            }
        }
        mixerClose(hMixer);
    }
    return (dwVolume);
#else
    #warning "TODO: Linux GetVolume()"
    return 0;
#endif

}

int SetVolume( const DWORD dwVolume )
{
#ifdef WIN32
    MMRESULT                        result = MMSYSERR_ERROR;
    HMIXER                          hMixer;
    MIXERLINE                       ml   = {0};
    MIXERLINECONTROLS               mlc  = {0};
    MIXERCONTROL                    mc   = {0};
    MIXERCONTROLDETAILS             mcd  = {0};
    MIXERCONTROLDETAILS_UNSIGNED    mcdu = {0};


    // get a handle to the mixer device
    result = mixerOpen(&hMixer, MIXER_OBJECTF_MIXER, 0, 0, 0);
    if (MMSYSERR_NOERROR == result)
    {
        ml.cbStruct        = sizeof(MIXERLINE);
        ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

        // get the speaker line of the mixer device
        result = mixerGetLineInfo((HMIXEROBJ) hMixer, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE);
        if (MMSYSERR_NOERROR == result)
        {
            mlc.cbStruct      = sizeof(MIXERLINECONTROLS);
            mlc.dwLineID      = ml.dwLineID;
            mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
            mlc.cControls     = 1;
            mlc.pamxctrl      = &mc;
            mlc.cbmxctrl      = sizeof(MIXERCONTROL);

            // get the volume controls associated with the speaker line
            result = mixerGetLineControls((HMIXEROBJ) hMixer, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
            if (MMSYSERR_NOERROR == result)
            {
                mcdu.dwValue    = dwVolume;

                mcd.cbStruct    = sizeof(MIXERCONTROLDETAILS);
                mcd.hwndOwner   = 0;
                mcd.dwControlID = mc.dwControlID;
                mcd.paDetails   = &mcdu;
                mcd.cbDetails   = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                mcd.cChannels   = 1;

                // set the volume
                result = mixerSetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);
                if (MMSYSERR_NOERROR == result)
                    return 0;
            }
        }
 
        mixerClose(hMixer);
    }
	return result == MMSYSERR_NOERROR ? 0 : -1;
#else
    #warning "TODO: Linux SetVolume()"
    return 0;
#endif
}
