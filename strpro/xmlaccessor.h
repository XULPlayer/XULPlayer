/*************************************************************************
* Distributed under GPL license
* Copyright (c) 2005-06 Stanley Huang <stanleyhuangyc@yahoo.com.cn>
* All rights reserved.
*************************************************************************/

#ifndef XMLACCESSOR_H
#define XMLACCESSOR_H

#include "StrProMacro.h"
_MC_STRPRO_BEGIN

class MC_STRPRO_EXT CXML
{
public:
	CXML(const char* filename = 0);
	virtual ~CXML();
	virtual void New(const char* xmlVersion, const char* rootNodeName, const char* xsltPath = 0) = 0;
	virtual int Open(const char* filename, const char* encoding = 0) = 0;
	virtual int Open2(const char* filename) = 0;
	virtual int Save(const char* filename, const char* encoding = 0) = 0;
	virtual int Read(const char* xmlbuf, int length = 0, const char* url = 0, const char* encoding = 0) = 0;
	virtual int Dump(char** buffer, char* encoding = 0) = 0;
	virtual void FreeDump() = 0;
	virtual int DumpCurrentNode(char** buffer, char* encoding = 0) = 0;

	//navigation methods
	virtual void* goToKey(const char *path, int index = 0) = 0;
	virtual void* goRoot() = 0;
	virtual void* goNext() = 0;
	virtual void* goPrev() = 0;
	virtual void* goFirst() = 0;
	virtual void* goChild() = 0;
	virtual void* goParent() = 0;

	//get/set methods
	virtual void* addChild(void* node) = 0;
	virtual void* addChild(const char* nodename) = 0;
	virtual void* addChild(const char* nodename, const char* value) = 0;
	virtual void* addChild(const char* nodename, int value) = 0;
	virtual void* addChild(const char* nodename, float value) = 0;
	virtual void* addText(const char* text) = 0;
	virtual void* addText(int value) = 0;
	virtual void* addAttribute(const char* name, const char* value) = 0;
	virtual void* setAttribute(const char* name, const char* value) = 0;
	virtual void* setAttribute(const char* name, int value) = 0;
	virtual void* setAttribute(const char* name, float value) = 0;
	virtual void  removeAttribute(const char* name) = 0;
	virtual void* findNode(const char* name, const char* attrname = 0, const char* attrvalue = 0) = 0;
	virtual void* findNextNode(const char* name, const char* attrname = 0, const char* attrvalue = 0) = 0;
	virtual void* findPrevNode(const char* name, const char* attrname = 0, const char* attrvalue = 0) = 0;
	virtual void* findChildNode(const char* name, const char* attrname = 0, const char* attrvalue = 0) = 0;
	virtual int getChildCount() = 0;
	virtual void setNodeName(const char* nodename) = 0;
	virtual void setNodeValue(const char* value, bool isCData = false) = 0;
	virtual void setNodeValue(int value) = 0;
	virtual void setNodeValue(float value) = 0;
	virtual char* getNodeName() = 0;
	virtual const char* getNodeValue(int index = 0) = 0;
	virtual int getNodeValueInt(int index = 0) = 0;
	virtual const char* getNodeValueByName(const char* name, const char* defaultValue = 0) = 0;
	virtual const char* getChildNodeValue(const char* name, const char* attrname = 0,  const char* attrvalue = 0, int index = 0) = 0;
	virtual int getChildNodeValueInt(const char* name, const char* attrname = 0, const char* attrvalue = 0, int index = 0) = 0;
	virtual const char* getAttribute(const char* propname, const char* defaultValue = 0, void* node = 0) = 0;
	virtual int getAttributeInt(const char* propname, int def = 0, void* node = 0) = 0;
	virtual float getAttributeFloat(const char* propname, float def = 0, void* node = 0) = 0;
	virtual void* removeNode() = 0;
	virtual void* replaceNode(const void* node) = 0;
	virtual bool isAttrMatched(const char* propname, const char* match) = 0;
	virtual bool isMatched(const char* match, bool matchCase = false) = 0;
	virtual void SetCurrentNode(void* node) = 0;
	virtual void Close() = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	static void Init();
	static void Cleanup();

};

_MC_STRPRO_END
#endif
