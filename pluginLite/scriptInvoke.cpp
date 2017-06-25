#include "scriptInvoke.h"
#ifdef WIN32
#include <windows.h>
#include "gecko_sdk/npapi.h"
#include "gecko_sdk/npupp.h"
#else
#include "npapi.h"
#include "npupp.h"
#include "linux_def.h"
#endif

#include <stdio.h>
#include "plugin.h"

extern CPlugin* pPlugin;

NPVariant jsInvoke(const char* funcname, int argc, NPVariant argv[])
{
	NPVariant barval;
	NPVariant rval;
	NPIdentifier id = NPN_GetStringIdentifier(funcname);
	if(id && pPlugin){
		NPN_GetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), id, &barval);
		NPN_InvokeDefault(pPlugin->getNpInstance(), NPVARIANT_TO_OBJECT(barval), argv, argc, &rval);
		NPN_ReleaseVariantValue(&barval);
	}
#ifdef _DEBUG
	else {
#ifdef WIN32
		DWORD bytes;
		HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
		WriteFile(hStdout, funcname, strlen(funcname), &bytes, 0);
#else
        printf("jsInvoke error: funcname = %s\n", funcname);
#endif
	}
#endif
	return rval;
}

bool jsSetVar(char* varname, int value)
{
	if(!pPlugin) return false;
	NPVariant v;
	INT32_TO_NPVARIANT(value, v);
	NPIdentifier n = NPN_GetStringIdentifier(varname);
	bool success = NPN_SetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), n, &v);
	NPN_ReleaseVariantValue(&v);
	return success;
}

bool jsSetVar(char* varname, char* value)
{
	if(!pPlugin) return false;
	NPVariant v;
	STRINGZ_TO_NPVARIANT(value, v);
	NPIdentifier n = NPN_GetStringIdentifier(varname);
	bool success = NPN_SetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), n, &v);
	NPN_ReleaseVariantValue(&v);
	return success;
}

bool jsSetVar(const char* varname, bool value)
{
	if(!pPlugin) return false;
	NPVariant v;
	BOOLEAN_TO_NPVARIANT(value, v);
	NPIdentifier n = NPN_GetStringIdentifier(varname);
	bool success = NPN_SetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), n, &v);
	NPN_ReleaseVariantValue(&v);
	return success;
}

bool jsSetVar(const char* varname, double value)
{
	if(!pPlugin) return false;
	NPVariant v;
	DOUBLE_TO_NPVARIANT(value, v);
	NPIdentifier n = NPN_GetStringIdentifier(varname);
	bool success = NPN_SetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), n, &v);
	NPN_ReleaseVariantValue(&v);
	return success;
}

// remeber to call NPN_ReleaseVariantValue(&rval);
NPVariant jsGetVar(const char* varname)
{
	NPVariant rval;
	if(!pPlugin) return rval;
	NPIdentifier n = NPN_GetStringIdentifier(varname);
	NPN_GetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), n, &rval);
	return rval;
}

bool jsEvaluate(const char* expr, NPVariant* result)
{
	if(!pPlugin) return false;
	NPVariant rval;
	NPString str;
	NPIdentifier sDocument_id = NPN_GetStringIdentifier("document");
	NPN_GetProperty(pPlugin->getNpInstance(), pPlugin->getBrowserObj(), sDocument_id, &rval);
	NPObject *doc = NPVARIANT_TO_OBJECT(rval);
	str.utf8characters = expr;
	str.utf8length = strlen(str.utf8characters);
	bool success = NPN_Evaluate(pPlugin->getNpInstance(), doc, &str, result);
	NPN_ReleaseVariantValue(&rval);
	return success;
}
