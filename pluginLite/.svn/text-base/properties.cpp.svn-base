#include "config.h"
#include "strpro/StrProInc.h"
#include "PluginCommon/PluginCommonInc.h"
#include "utils/datatype.h"
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

PLUGIN_PROPERTY properties[] = {
	{"hwnd", property_hwnd},
	{"buildDate", property_buildDate},
	{"edition", property_edition},
	{"syscp", property_codepage},
	{0},
};
