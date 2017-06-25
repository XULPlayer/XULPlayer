#include "config.h"
#include "strpro/StrProInc.h"
#include "PluginCommon/PluginCommonInc.h"
#include "utils/datatype.h"
#include "Xulplayer.h"
#ifdef _WIN32
#include "PluginCommon/common.h"
#endif

using namespace StrPro;
using namespace PluginCommon;
using namespace std;

extern CPlugin * pPlugin;

#ifndef WIN32
#define UINT unsigned int
#endif

bool property_onPlayerStart(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;

	if (!set) {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(xulplayer->getOnPlayerStart().c_str()), *value);
	} else {
		if (value->type == NPVariantType_String)
			xulplayer->setOnPlayerStart(value->value.stringValue.utf8characters);
	}
	return true;
}

bool property_onPlayerStop(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (!set) {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(xulplayer->getOnPlayerStop().c_str()), *value);
	} else {
		if (value->type == NPVariantType_String)
			xulplayer->setOnPlayerStop(value->value.stringValue.utf8characters);
	}
	return true;
}

bool property_onPlayerPaused(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;

	if (!set) {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(xulplayer->getOnPlayerPause().c_str()), *value);
	} else {
		if (value->type == NPVariantType_String)
			xulplayer->setOnPlayerPause(value->value.stringValue.utf8characters);
	}
	return true;
}

bool property_bgcolor(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (!set) {
		INT32_TO_NPVARIANT(xulplayer->getVideoBgColor(), *value);
	} else {
		int bgcolor = 0;
		if (value->type == NPVariantType_Int32)
			bgcolor = value->value.intValue;
		else
			bgcolor = atoi(value->value.stringValue.utf8characters);
		xulplayer->setVideoBgColor(bgcolor);
#ifdef WIN32
		if(pPlugin) {
			SetWindowLong(pPlugin->getEmbedWnd(), GWL_USERDATA, bgcolor);
			InvalidateRect(pPlugin->getEmbedWnd(), 0, TRUE);
		}
#else
        #warning "TODO: Linux UI"
#endif
	}
	return true;
}

bool property_bordercolor(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (!set) {
		INT32_TO_NPVARIANT(xulplayer->getBorderColor(), *value);
	} else {
		int borderColor = 0;
		if (value->type == NPVariantType_Int32)
			borderColor = value->value.intValue;
		else
			borderColor = atoi(value->value.stringValue.utf8characters);

		xulplayer->setBorderColor(borderColor);
#ifdef WIN32
		if(pPlugin) {
			InvalidateRect(pPlugin->getEmbedWnd(), 0, TRUE);
		}
#else
        #warning "TODO: Linux UI"
#endif
	}
	return true;
}

bool property_borderwidth(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (!set) {
		INT32_TO_NPVARIANT(xulplayer->getBorderWidth(), *value);
	} else {
		int borderWidth = 0;
		if (value->type == NPVariantType_Int32)
			borderWidth = value->value.intValue;
		else
		borderWidth = atoi(value->value.stringValue.utf8characters);

		xulplayer->setBorderWidth(borderWidth);
#ifdef WIN32
		if(pPlugin) {
			InvalidateRect(pPlugin->getEmbedWnd(), 0, TRUE);
		}
#else
#warning "TODO: Linux UI"
#endif
	}
	return true;
}

bool property_width(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;
	if (!set) {
		INT32_TO_NPVARIANT(mplayer->video.width, *value);
	} else {
		return false;
	}
	return true;
}

bool property_height(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;
	if (!set) {
		INT32_TO_NPVARIANT(mplayer->video.height, *value);
	} else {
		return false;
	}
	return true;
}

bool property_real_width(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if(!mplayer) return false;
	if (!set) {
		if(xulplayer->getActiveWnd()) {
			if(xulplayer->getZoomFactor() > BI_EPSILON)
				INT32_TO_NPVARIANT((UINT)(mplayer->video.width*xulplayer->getZoomFactor()), *value);
			else {
				int realH = 0, realW = 0;
				xulplayer->getCurrentRealVideoSize(realW, realH);
				INT32_TO_NPVARIANT(realW, *value);
			}
		}else {
			INT32_TO_NPVARIANT(0, *value);
		}
	} else {
		return false;
	}
	return true;
}
bool property_real_height(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	CMPlayer* mplayer = xulplayer->getActivePlayer();
	if (!set) {
		if(xulplayer->getActiveWnd()) {
			if(xulplayer->getZoomFactor() > BI_EPSILON)
				INT32_TO_NPVARIANT((UINT)(mplayer->video.height*xulplayer->getZoomFactor()), *value);
			else {
				int realH = 0, realW = 0;
				xulplayer->getCurrentRealVideoSize(realW, realH);
				INT32_TO_NPVARIANT(realH, *value);
			}
		} else {
			INT32_TO_NPVARIANT(0, *value);
		}
	} else {
		return false;
	}
	return true;
}
bool property_hwnd(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (!set && pPlugin) {
#ifdef WIN32
		INT32_TO_NPVARIANT((int)pPlugin->getEmbedWnd(), *value);
#else
#warning "TODO: Linux UI"
#endif
	} else {
		return false;
	}
	return true;
}

bool property_buildDate(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (!set) {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(BUILD_DATE), *value);
	} else {
		return false;
	}
	return true;
}

bool property_edition(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (!set) {
#if defined(_LITE)
		INT32_TO_NPVARIANT(0, *value);
#else
		INT32_TO_NPVARIANT(1, *value);
#endif
	} else {
		return false;
	}
	return true;
}

bool property_codepage(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (set) return false;
#ifdef WIN32
	char buf[16];
	if (GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, buf, sizeof(buf)) > 0) {
		INT32_TO_NPVARIANT(atoi(buf), *value);
	}
#endif
	return true;
}

bool property_keep_aspect(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;

	if (set) {
		xulplayer->setKeepAspect(value->value.boolValue);
	} else {
		BOOLEAN_TO_NPVARIANT(xulplayer->getKeepAspect(), *value);
	}
	return true;
}

bool property_playingFile(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (set) {
		xulplayer->setCurrentPlayFile(value->value.stringValue.utf8characters);
	}
    else {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(xulplayer->getCurrentPlayFile()), *value);
    }
	return true;
}

bool property_active_id(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (set) {
		xulplayer->setActiveId(value->value.intValue);
	}
	else {
		INT32_TO_NPVARIANT(xulplayer->getActiveId(), *value);
	}
	return true;
}


#ifdef ENABLE_MCODER
bool property_cov_percent(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (set)  return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;

	DOUBLE_TO_NPVARIANT((float)(int)(mc->getPercentEx() * 10) / 10, *value);
	return true;
}

bool property_cov_status(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (set)  return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	int convertStatus = (int)mc->getStatus();
	INT32_TO_NPVARIANT(convertStatus, *value);
	return true;
}

bool property_cov_destfile(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (set) return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	const UTILS::T_ITEM *item = mc->getDestItem();
	STRINGZ_TO_NPVARIANT(MemAllocStrdup(item->destfile ? item->destfile : ""), *value);
	return true;
}

bool property_work_directory(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	if (set)  return false;
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	MCodeWrapper* mc = xulplayer->getConverter();
	if(!mc) return false;
	char* workDir = mc->m_WorkDir;
	CCharset charset;
	string sworkDir = charset.ANSItoUTF8(workDir);	
	if(workDir) {
		STRINGZ_TO_NPVARIANT(MemAllocStrdup(sworkDir.c_str()), *value);
		return true;
	} 
	return false;
}

bool property_convert_Prefs(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (set) {
		xulplayer->setConvertPrefs(value->value.stringValue.utf8characters);
	} else {
		STRINGZ_TO_NPVARIANT((xulplayer->getConvertPrefs().c_str()), *value);
	}
	return true;
}
#endif

#ifdef ENABLE_VIS
bool property_visualization(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value)
{
	CXulplayer* xulplayer = CXulplayer::getInstance();
	if(!xulplayer) return false;
	if (set) {
		xulplayer->setEnableVisual(value->value.boolValue);
	} else {
		BOOLEAN_TO_NPVARIANT(xulplayer->getEnableVisual(), *value);
	}
	return true;
}
#endif

PLUGIN_PROPERTY properties[] = {
	{"hwnd", property_hwnd},
	{"buildDate", property_buildDate},
	{"edition", property_edition},
	{"bgColor", property_bgcolor},
	{"borderColor", property_bordercolor},
	{"borderWidth", property_borderwidth},
	{"onPlayerStop", property_onPlayerStop},
	{"onPlayerStart", property_onPlayerStart},
	{"onPlayerPaused", property_onPlayerPaused},
	//{"width", property_width},
	//{"height", property_height},
	{"realWidth", property_real_width},
	{"realHeight", property_real_height},
	{"syscp", property_codepage},
	{"keepAspect", property_keep_aspect},
	{"playingFile", property_playingFile}, 
	{"activeId", property_active_id}, 
#ifdef ENABLE_VIS
	{"visualization", property_visualization},
#endif
#ifdef ENABLE_MCODER
	// Properties of Converter
	{"covPercent", property_cov_percent}, 
	{"covStatus", property_cov_status}, 
	{"covDestFile", property_cov_destfile}, 
	{"workDirectory", property_work_directory},
	{"covPrefs", property_convert_Prefs},
#endif
	{0},
};
