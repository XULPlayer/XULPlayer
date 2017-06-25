/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

//////////////////////////////////////////////////
//
// CPlugin class implementation
//
#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#else
#include <linux_def.h>
#endif

#ifdef XP_MAC
#include <TextEdit.h>
#endif

#ifdef XP_UNIX
#include <string.h>
#endif

#include "plugin.h"
#ifdef WIN32
#include "gecko_sdk/npupp.h"
#else
#include "npupp.h"
#endif

#include "ui.h"

extern PLUGIN_METHOD methods[];
extern PLUGIN_PROPERTY properties[];

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                        \
static NPClass s##_class##_NPClass = {                                        \
  NP_CLASS_STRUCT_VERSION,                                                    \
  ctor,                                                                       \
  ScriptablePluginObjectBase::_Deallocate,                                    \
  ScriptablePluginObjectBase::_Invalidate,                                    \
  ScriptablePluginObjectBase::_HasMethod,                                     \
  ScriptablePluginObjectBase::_Invoke,                                        \
  ScriptablePluginObjectBase::_InvokeDefault,                                 \
  ScriptablePluginObjectBase::_HasProperty,                                   \
  ScriptablePluginObjectBase::_GetProperty,                                   \
  ScriptablePluginObjectBase::_SetProperty,                                   \
  ScriptablePluginObjectBase::_RemoveProperty                                 \
}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass

void ScriptablePluginObjectBase::Invalidate() 
{
}

bool ScriptablePluginObjectBase::HasMethod(NPIdentifier name)
{
  return false;
}

bool ScriptablePluginObjectBase::Invoke(NPIdentifier name, const NPVariant *args,
                                   uint32_t argCount, NPVariant *result)
{
  return false;
}

bool ScriptablePluginObjectBase::InvokeDefault(const NPVariant *args,
                                          uint32_t argCount, NPVariant *result)
{
  return false;
}

bool ScriptablePluginObjectBase::HasProperty(NPIdentifier name)
{
	for (int i = 0; properties[i].name; i++) {
		if (name == properties[i].id) return true;
	}
	return false;
}

bool ScriptablePluginObjectBase::GetProperty(NPIdentifier name, NPVariant *result)
{
	for (int i = 0; properties[i].name; i++) {
		if (name == properties[i].id) {
			if (properties[i].access(this, false, result)) return true;
			break;
		}
	}
	//VOID_TO_NPVARIANT(*result);
	return false;
}

bool ScriptablePluginObjectBase::SetProperty(NPIdentifier name,
                                        const NPVariant *value)
{
	for (int i = 0; properties[i].name; i++) {
		if (name == properties[i].id) {
			return properties[i].access(this, true, (NPVariant*)value);
		}
	}
	return false;
}

bool ScriptablePluginObjectBase::RemoveProperty(NPIdentifier name)
{
  return false;
}

// static
void ScriptablePluginObjectBase::_Deallocate(NPObject *npobj)
{
  // Call the virtual destructor.
  delete (ScriptablePluginObjectBase *)npobj;
}

// static
void ScriptablePluginObjectBase::_Invalidate(NPObject *npobj)
{
  ((ScriptablePluginObjectBase *)npobj)->Invalidate();
}

// static
bool
ScriptablePluginObjectBase::_HasMethod(NPObject *npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->HasMethod(name);
}

// static
bool ScriptablePluginObjectBase::_Invoke(NPObject *npobj, NPIdentifier name,
                                    const NPVariant *args, uint32_t argCount,
                                    NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->Invoke(name, args, argCount,
                                                       result);
}

// static
bool ScriptablePluginObjectBase::_InvokeDefault(NPObject *npobj,
                                           const NPVariant *args,
                                           uint32_t argCount,
                                           NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->InvokeDefault(args, argCount,
                                                              result);
}

// static
bool ScriptablePluginObjectBase::_HasProperty(NPObject * npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->HasProperty(name);
}

// static
bool ScriptablePluginObjectBase::_GetProperty(NPObject *npobj, NPIdentifier name,
                                         NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->GetProperty(name, result);
}

// static
bool
ScriptablePluginObjectBase::_SetProperty(NPObject *npobj, NPIdentifier name,
                                         const NPVariant *value)
{
  return ((ScriptablePluginObjectBase *)npobj)->SetProperty(name, value);
}

// static
bool
ScriptablePluginObjectBase::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->RemoveProperty(name);
}



class ScriptablePluginObject : public ScriptablePluginObjectBase
{
public:
  ScriptablePluginObject(NPP npp)
    : ScriptablePluginObjectBase(npp)
  {
  }

  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);
  virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                             NPVariant *result);
};

static NPObject *
AllocateScriptablePluginObject(NPP npp, NPClass *aClass)
{
  return new ScriptablePluginObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ScriptablePluginObject,
                                 AllocateScriptablePluginObject);

bool ScriptablePluginObject::HasMethod(NPIdentifier name)
{
	for (int i = 0; methods[i].name; i++) {
		if (name == methods[i].id) return true;
	}
	return false;
}

bool ScriptablePluginObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	for (int i = 0; methods[i].name; i++) {
		if (name == methods[i].id) {
			return (*methods[i].invoke)(this, args, argCount, result) ? PR_TRUE : PR_FALSE;
		}
	}
	return PR_FALSE;
}

bool ScriptablePluginObject::InvokeDefault(const NPVariant *args, uint32_t argCount,
                                      NPVariant *result)
{
  printf ("ScriptablePluginObject default method called!\n");

  if (methods[0].invoke) (*methods[0].invoke)(this, args, argCount, result);
  //STRINGZ_TO_NPVARIANT(MemAllocStrdup("default method return val"), *result);

  return PR_TRUE;
}

CPlugin::CPlugin(NPP pNPInstance) :
  m_pNPInstance(pNPInstance),
  m_pNPStream(NULL),
  m_bInitialized(FALSE),
  m_pScriptableObject(NULL),
  m_browserObj(NULL)
{
#ifdef XP_WIN
	m_hWnd = NULL;
#endif

	NPN_GetValue(m_pNPInstance, NPNVWindowNPObject, &m_browserObj);
	
	for (int i = 0; properties[i].name; i++)
		properties[i].id = NPN_GetStringIdentifier(properties[i].name);

	for (int i = 0; methods[i].name; i++)
		methods[i].id = NPN_GetStringIdentifier(methods[i].name);

	
	m_docId = NPN_GetStringIdentifier("document");

#if 0
	NPObject *doc;

	NPN_GetProperty(m_pNPInstance, sWindowObj, sDocument_id, &rval);

	if (NPVARIANT_IS_OBJECT(rval) && (doc = NPVARIANT_TO_OBJECT(rval))) {
		n = NPN_GetStringIdentifier("title");

		NPN_GetProperty(m_pNPInstance, doc, n, &rval);

		if (NPVARIANT_IS_STRING(rval)) {
			printf ("title = %s\n", NPVARIANT_TO_STRING(rval).utf8characters);
			NPN_ReleaseVariantValue(&rval);
		}

		n = NPN_GetStringIdentifier("plugindoc");

		OBJECT_TO_NPVARIANT(doc, v);
		NPN_SetProperty(m_pNPInstance, sWindowObj, n, &v);

		NPString str;
		str.utf8characters = "alert('NPN_Evaluate() test, document = ' + this);";
		str.utf8length = strlen(str.utf8characters);

		NPN_Evaluate(m_pNPInstance, doc, &str, NULL);

		NPN_ReleaseObject(doc);
	}

	n = NPN_GetStringIdentifier("prompt");

	NPVariant vars[3];
	STRINGZ_TO_NPVARIANT("foo", vars[0]);
	STRINGZ_TO_NPVARIANT("bar", vars[1]);
	STRINGZ_TO_NPVARIANT("foof", vars[2]);

	NPN_Invoke(sWindowObj, n, vars, 3, &rval);

	if (NPVARIANT_IS_STRING(rval)) {
		printf ("prompt returned '%s'\n", NPVARIANT_TO_STRING(rval).utf8characters);
	}

	NPN_ReleaseVariantValue(&rval);
#endif

  NPObject *myobj = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(ScriptablePluginObject));

  //! plugin-name exposed to Javascript
  NPIdentifier n = NPN_GetStringIdentifier("player");

  NPVariant v;
  NPVariant rval;
  OBJECT_TO_NPVARIANT(myobj, v);
  NPN_SetProperty(m_pNPInstance, m_browserObj, n, &v);

  NPN_GetProperty(m_pNPInstance, m_browserObj, n, &rval);

  //printf ("Object set/get test ");

  if (NPVARIANT_IS_OBJECT(rval) && NPVARIANT_TO_OBJECT(rval) == myobj) {
    printf ("Object set/get test succeeded!\n");
  } else {
    printf ("Object set/get test FAILED!\n");
  }

  NPN_ReleaseVariantValue(&rval);
  NPN_ReleaseObject(myobj);

  const char *ua = NPN_UserAgent(m_pNPInstance);
  strcpy(m_String, ua);
}

CPlugin::~CPlugin()
{
  
	if (m_browserObj) {
		NPN_ReleaseObject(m_browserObj);
		m_browserObj = NULL;
	}
  
	if (m_pScriptableObject) {
		NPN_ReleaseObject(m_pScriptableObject);
		m_pScriptableObject = NULL;
	}

}

#ifdef XP_WIN
LRESULT CALLBACK XULWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC lpOldProc = NULL;
LRESULT CALLBACK XulWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL val1 = XULWindowProc(hWnd, msg, wParam, lParam);
	BOOL val2 = lpOldProc(hWnd, msg, wParam, lParam);
	return val1 > val2 ? val1 : val2; 
}
#endif

NPBool CPlugin::init(NPWindow* pNPWindow)
{
  if(pNPWindow == NULL)
    return FALSE;

#ifdef XP_WIN
  m_hWnd = (HWND)pNPWindow->window;
  if(m_hWnd == NULL)
    return FALSE;

  m_xulWin = GetRootWindow(m_hWnd);
  //hXulInst = (HINSTANCE)GetWindowLong(xul_hWnd,GWL_HINSTANCE);
  //GetWindowLong(xul_hWnd,GWL_HINSTANCE);

  // subclass window so we can intercept window messages and
  // do our drawing to it
  lpOldProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWinProc);
  //SetTimer(m_hWnd, 0, 2000, 0);

  // associate window with our CPlugin object so we can access 
  // it in the window procedure
  SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
#else
  m_hWnd = (HWND)pNPWindow->window;
#endif

  m_Window = pNPWindow;
  m_bInitialized = TRUE;
  return TRUE;
}

void CPlugin::shut()
{
#ifdef XP_WIN
  // subclass it back
  SubclassWindow(m_hWnd, lpOldProc);
  //KillTimer(m_hWnd, 0);
  m_hWnd = NULL;
#endif

  m_bInitialized = FALSE;
}

NPBool CPlugin::isInitialized()
{
  return m_bInitialized;
}

int16 CPlugin::handleEvent(void* event)
{
#ifdef XP_MAC
  NPEvent* ev = (NPEvent*)event;
  if (m_Window) {
    Rect box = { m_Window->y, m_Window->x,
                 m_Window->y + m_Window->height, m_Window->x + m_Window->width };
    if (ev->what == updateEvt) {
      ::TETextBox(m_String, strlen(m_String), &box, teJustCenter);
    }
  }
#endif
  return 0;
}

// this will force to draw a version string in the plugin window
void CPlugin::showVersion()
{
  const char *ua = NPN_UserAgent(m_pNPInstance);
  strcpy(m_String, ua);

#ifdef XP_WIN
  InvalidateRect(m_hWnd, NULL, TRUE);
  UpdateWindow(m_hWnd);
#endif

  if (m_Window) {
    NPRect r =
      {
        (uint16)m_Window->y, (uint16)m_Window->x,
        (uint16)(m_Window->y + m_Window->height),
        (uint16)(m_Window->x + m_Window->width)
      };

    NPN_InvalidateRect(m_pNPInstance, &r);
  }
}

// this will clean the plugin window
void CPlugin::clear()
{
  strcpy(m_String, "");

#ifdef XP_WIN
  InvalidateRect(m_hWnd, NULL, TRUE);
  UpdateWindow(m_hWnd);
#endif
}

void CPlugin::getVersion(char* *aVersion)
{
  const char *ua = NPN_UserAgent(m_pNPInstance);
  char*& version = *aVersion;
  version = (char*)NPN_MemAlloc(1 + strlen(ua));
  if (version)
    strcpy(version, ua);
}

NPObject *
CPlugin::GetScriptableObject()
{
  if (!m_pScriptableObject) {
    m_pScriptableObject =
      NPN_CreateObject(m_pNPInstance,
                       GET_NPOBJECT_CLASS(ScriptablePluginObject));
  }

  if (m_pScriptableObject) {
    NPN_RetainObject(m_pScriptableObject);
  }

  return m_pScriptableObject;
}


char *MemAllocStrdup(const char *str) {
  int lenz = strlen(str) + 1;
  char *dup = static_cast<char *>(NPN_MemAlloc(lenz));
  if (NULL != dup) {
    strncpy(dup, str, lenz);
  }
  // else abort();
  return dup;
}

char *MemAllocStrdup(std::string& str) {
  int lenz = str.length() + 1;
  char *dup = static_cast<char *>(NPN_MemAlloc(lenz));
  if (NULL != dup) {
    strcpy(dup, str.c_str());
  }
  // else abort();
  return dup;
}
