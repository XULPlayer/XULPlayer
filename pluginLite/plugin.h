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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifdef WIN32
#include "gecko_sdk/npapi.h"
#include "gecko_sdk/npruntime.h"
#else
#include "npapi.h"
#include "npruntime.h"
#include "linux_def.h"
#endif
#include <string>

// Helper class that can be used to map calls to the NPObject hooks
// into virtual methods on instances of classes that derive from this
// class.
class ScriptablePluginObjectBase : public NPObject
{
public:
  ScriptablePluginObjectBase(NPP npp)
    : mNpp(npp)
  {
  }

  // Virtual NPObject hooks called through this base class. Override
  // as you see fit.
  virtual void Invalidate();
  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);
  virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                             NPVariant *result);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
  virtual bool RemoveProperty(NPIdentifier name);

public:
  static void _Deallocate(NPObject *npobj);
  static void _Invalidate(NPObject *npobj);
  static bool _HasMethod(NPObject *npobj, NPIdentifier name);
  static bool _Invoke(NPObject *npobj, NPIdentifier name,
                      const NPVariant *args, uint32_t argCount,
                      NPVariant *result);
  static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
                             uint32_t argCount, NPVariant *result);
  static bool _HasProperty(NPObject * npobj, NPIdentifier name);
  static bool _GetProperty(NPObject *npobj, NPIdentifier name,
                           NPVariant *result);
  static bool _SetProperty(NPObject *npobj, NPIdentifier name,
                           const NPVariant *value);
  static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
  NPP mNpp;
};

typedef bool (*pfnProperty)(ScriptablePluginObjectBase* plugin, bool set, NPVariant* value);
typedef bool (*pfnInvoke)(ScriptablePluginObjectBase* plugin, const NPVariant *args, int argCount, NPVariant *result);

typedef struct {
	const char* name;
	pfnProperty access;
	NPIdentifier id;
} PLUGIN_PROPERTY;

typedef struct {
	const char* name;
	pfnInvoke invoke;
	NPIdentifier id;
} PLUGIN_METHOD;

class CPlugin
{
private:
  NPP m_pNPInstance;

  HWND m_hWnd; 

  NPWindow * m_Window;
  
  NPStream * m_pNPStream;
  NPBool m_bInitialized;

  NPObject *m_pScriptableObject;

public:
  char m_String[128];

public:
  CPlugin(NPP pNPInstance);
  ~CPlugin();

  NPBool init(NPWindow* pNPWindow);
  void shut();
  NPBool isInitialized();
  int16 handleEvent(void* event);
  void showVersion();
  void clear();
  void getVersion(char* *aVersion);
  NPObject *GetScriptableObject();
  NPObject* getBrowserObj() {return m_browserObj;}
  NPP getNpInstance() {return m_pNPInstance;}

  HWND getEmbedWnd() {return m_hWnd;}; 
#ifdef XP_WIN
  HWND getXulWnd() {return m_xulWin;}
#endif
// xulplayer specific functions
public:


// xulplayer specific data
private:
	NPIdentifier m_docId;
	NPObject* m_browserObj;
	HWND m_xulWin;
	//HINSTANCE hXulInst = 0;
};

char *MemAllocStrdup(const char *str);
char *MemAllocStrdup(std::string& str);

#endif // __PLUGIN_H__
