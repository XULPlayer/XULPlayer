#ifndef _SCRIPT_INVOKE_H_
#define _SCRIPT_INVOKE_H_

#ifdef WIN32
#include "gecko_sdk/npapi.h"
#include "gecko_sdk/npupp.h"
#else
#include "npapi.h"
#include "npupp.h"
#endif

NPVariant jsInvoke(const char* funcname, int argc, NPVariant argv[]);
bool      jsSetVar(char* varname, int value);
bool	  jsSetVar(const char* varname, bool value);
bool      jsSetVar(char* varname, char* value);
bool      jsSetVar(const char* varname, double value);
NPVariant jsGetVar(const char* varname);
bool      jsEvaluate(const char* expr, NPVariant* result);


#endif