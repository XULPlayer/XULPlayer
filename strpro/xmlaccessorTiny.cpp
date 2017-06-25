/*************************************************************************
* Distributed under GPL license
* Copyright (c) 2005-06 Stanley Huang <stanleyhuangyc@yahoo.com.cn>
* All rights reserved.
*************************************************************************/
#include <fcntl.h>

#include "bit_osdep.h"
#include "xmlaccessorTiny.h"
#include "tinyXml/tinyxml.h"

_MC_STRPRO_BEGIN

#ifdef WIN32
CXMLTiny_OUT::CXMLTiny_OUT(const char* filename):doc(0),result(0),xmlfile(0),hLock(0)
{
#else
CXMLTiny_OUT::CXMLTiny_OUT(const char* filename):doc(0),result(0),xmlfile(0)
{
    pthread_mutex_init(&mMutex, NULL);
#endif
	if (filename) Open(filename);
}

CXMLTiny_OUT::~CXMLTiny_OUT()
{
	Close();
}

void CXMLTiny_OUT::Init()
{
	//xmlInitParser();
}

void CXMLTiny_OUT::Cleanup()
{
	//xmlCleanupParser();
}

void CXMLTiny_OUT::New(const char* xmlVersion, const char* rootNodeName, const char* xsltPath)
{
	Close();
	//xmlInitParser();
	doc = new TiXmlDocument();
	TiXmlDeclaration declaration(xmlVersion, "UTF-8", "yes");
	((TiXmlDocument*)doc)->InsertEndChild( declaration );
	cur = new TiXmlElement(rootNodeName);
	((TiXmlDocument*)doc)->InsertEndChild(*((TiXmlElement*)cur));
	if (xsltPath) {
		/*char *style = (char*)malloc(strlen(xsltPath) + 32);
		sprintf(style, "type=\"text/xsl\" href=\"%s\"", xsltPath);
		free(style);*/
	}
	goRoot();
}

int CXMLTiny_OUT::Open(const char* filename, const char* encoding)
{
	if (!xmlfile || strcmp(filename, xmlfile)) {
		Close();

		bool success;
		doc = new TiXmlDocument(filename);
		TiXmlEncoding type = TIXML_ENCODING_UTF8;
		if (encoding != NULL)
		{	
			if (strcmp(encoding, "utf8") != 0)
			{
				type = TIXML_ENCODING_LEGACY;
			}
		}
		((TiXmlDocument*)doc)->SetCondenseWhiteSpace(false);
		success = ((TiXmlDocument*)doc)->LoadFile(type);
		if (!success)
		{
			return -1;
		}

		xmlfile = _strdup(filename); 
	}
	cur = ((TiXmlDocument*)doc)->RootElement();
	return 0;
}

int CXMLTiny_OUT::Save(const char* filename, const char* encoding)
{
	if (!((TiXmlDocument*)doc))
	{
		return -1;
	}
	bool ret = false;
	if (encoding)
		ret = ((TiXmlDocument*)doc)->SaveFile(filename); //can decide only by open
	else
		ret = ((TiXmlDocument*)doc)->SaveFile(filename);
	if (xmlfile) {
		free(xmlfile);
		xmlfile = 0;
	}
	if (!ret) {
		xmlfile = _strdup(filename);
	}
	return ret ? 1 : -1;
}

int CXMLTiny_OUT::Read(const char* xmlbuf, int length, const char* url, const char* encoding)
{
	Close();
	doc = 0;
	if (!xmlbuf)return -1;
	if (!length) length = (int)strlen(xmlbuf);
	if (xmlbuf == NULL || length == 0)
	{	
		return -1;
	}
	
	TiXmlEncoding type = TIXML_ENCODING_UTF8;
	if (encoding != NULL)
	{	
		if (strcmp(encoding, "utf8") != 0)
		{
			type = TIXML_ENCODING_LEGACY;
		}
	}
	std::string safebuf = std::string(xmlbuf);
	if (safebuf[0] == '<' && safebuf[1] != '?')
	{
		//add head
		safebuf = "<?xml version=\"1.0\"?>" + safebuf;
	}
	if(safebuf[0] == '<')
	{	
		if(url == NULL)
			doc = new TiXmlDocument();
		else 
			doc = new TiXmlDocument(url);	
		//((TiXmlDocument*)doc)->SetCondenseWhiteSpace(false);
		const char* str = ((TiXmlDocument*)doc)->Parse(safebuf.c_str(), 0, type);
		if (!this->goRoot())
		{
			if (doc)
			{
				delete doc;
				doc = 0;
			}			
		}
	}
	//encoding;
	if (!doc) return -1;
	cur = ((TiXmlDocument*)doc)->RootElement();
	return cur ? 0 : -1;
}

int CXMLTiny_OUT::Dump(char** buffer, char* encoding)
{
	if (result) free(result);	
	TiXmlPrinter printer;
	if(!doc || ((TiXmlDocument*)doc)->Accept(&printer) == false)
	{
		*buffer = 0;
		return 0;
	}

	const std::string temp = printer.Str();
	result = (char*)malloc(temp.size() + 1);
	memset(result, 0, temp.size() + 1);
	memcpy(result, temp.c_str(), temp.size());
	*buffer = result;
	return temp.size();
}

int CXMLTiny_OUT::DumpCurrentNode(char** buffer, char* encoding)
{
	if (result) free(result);	
	TiXmlPrinter printer;
	if (doc && cur &&((TiXmlElement*)cur)->Accept(&printer))
	{
		const std::string temp = printer.Str();
		result = (char*)malloc(temp.size() + 1);
		memset(result, 0, temp.size() + 1);
		memcpy(result, temp.c_str(), temp.size());
		*buffer = result;
		return temp.size();
	}
	else
	{
		*buffer = 0;
		return 0;
	}	
}

void CXMLTiny_OUT::Close()
{
	if (result) {
		free(result);
		result = 0;
	}
	if ((TiXmlDocument*)doc) {
		((TiXmlDocument*)doc)->Clear();
		delete (TiXmlDocument*)doc;
		doc = 0;
	}
	if (xmlfile) {
		free(xmlfile);
		xmlfile = 0;
	}
}

void* CXMLTiny_OUT::goToKey(const char* path, int index)
{
	//wait
	return 0;
}

const char* CXMLTiny_OUT::getChildNodeValue(const char* name, const char* attrname /* = 0 */, const char* attrvalue /* = 0 */, int index /* = 0 */)
{
	if (findChildNode(name, attrname, attrvalue))
	{
		const char* val = getNodeValue(index);
		goParent();
		return val;
	}
	return NULL;
}

int CXMLTiny_OUT::getChildNodeValueInt(const char* name, const char* attrname /* = 0 */, const char* attrvalue /* = 0 */, int index /* = 0 */)
{
	const char* val = getChildNodeValue(name, attrname, attrvalue, index);
	return val ? atoi(val) : 0;
}

int CXMLTiny_OUT::getChildCount()
{
	int count = 0;
	for (LPXCNODE node = ((LPXCNODE)cur)->FirstChild(); node; node = node->NextSibling()) {
		if (node->Type() == TiXmlNode::ELEMENT) count++;
	}
	return count;
}

void* CXMLTiny_OUT::goRoot()
{
	if (!doc)
	{
		return NULL;
	}
	return (cur = ((TiXmlDocument*)doc)->RootElement());
}

void* CXMLTiny_OUT::goNext()
{
	if (!cur)
	{
		return NULL;
	}
	LPXNODE node = ((LPXNODE)cur)->NextSibling();
	while (node && node->Type() != TiXmlNode::ELEMENT) node = node->NextSibling();
	if (node) cur = node;
	return node;
}

void* CXMLTiny_OUT::goPrev()
{
	if (!cur)
	{
		return NULL;
	}
	LPXNODE node = ((LPXNODE)cur)->Parent();
	while (node && node->Type() != TiXmlNode::ELEMENT) node = node->Parent();
	if (node) cur = node;
	return node;
}

void* CXMLTiny_OUT::goFirst()
{
	if (!cur) return 0;
	LPXNODE node = (LPXNODE)cur;
	node = node->Parent();
	if (!node)
	{
		return cur;
	}
	node = node->FirstChild();
	if (!node)
	{
		return cur;
	}
	while (node->Type() != TiXmlNode::ELEMENT) node = node->NextSibling();
	if (node) cur = node;
	return node;
}

void* CXMLTiny_OUT::goChild()
{
	LPXNODE node = ((LPXNODE)cur)->FirstChild();
	while (node && node->Type() != TiXmlNode::ELEMENT) node = node->NextSibling();
	if (node) cur = node;
	return node;
}

void* CXMLTiny_OUT::goParent()
{
	LPXNODE node = cur ? ((LPXNODE)cur)->Parent() : 0;
	while (node && node->Type() != TiXmlNode::ELEMENT && node->Type() != TiXmlNode::DOCUMENT ) 
		node = node->Parent();
	if (node) cur = node;
	return node;
}

void CXMLTiny_OUT::SetCurrentNode(void* node)
{
	cur = (LPXNODE)node;
}

void* CXMLTiny_OUT::findNextNode(const char* name, const char* attrname, const char* attrvalue)
{
	if (!cur) return 0;
	LPXNODE node = ((LPXNODE)cur)->NextSibling();
	while (node) {
		if ((node->Type() == TiXmlNode::ELEMENT && !strcmp(node->Value(), name)) &&
			(!attrname || !attrvalue || !strcmp(attrvalue, getAttribute(attrname, "", node)))) {
			cur = node;
			return node;
		}
		node = node->NextSibling();
	}
	return 0;
}

void* CXMLTiny_OUT::findNode(const char* name, const char* attrname, const char* attrvalue)
{
	if (!cur) return 0;
	LPXNODE node = (LPXNODE)cur;

	// look forward
	while (node) {
		if ((node->Type() == TiXmlNode::ELEMENT && !strcmp(node->Value(), name)) &&
			((!attrname || !attrvalue) || !strcmp(attrvalue, getAttribute(attrname, "", node)))) {
			cur = node;
			return node;
		}
		node = node->NextSibling();
	}

	// look backward
	node = (LPXNODE)cur;
	do {
		if ((node->Type() == TiXmlNode::ELEMENT && !strcmp(node->Value(), name)) &&
			(!attrname || !attrvalue || !strcmp(attrvalue, getAttribute(attrname, "", node)))) {
			cur = node;
			return node;
		}
		node = node->Parent();
	} while (node);
	return 0;
}

void* CXMLTiny_OUT::findPrevNode(const char* name, const char* attrname, const char* attrvalue)
{
    //TODO
    return NULL;
}

void* CXMLTiny_OUT::findChildNode(const char* name, const char* attrname, const char* attrvalue)
{
	if (!cur) return 0;
	LPXNODE node = ((LPXNODE)cur)->FirstChild();
	while (node && node->Type() != TiXmlNode::ELEMENT) node = node->NextSibling();
	while (node) {
		if ((node->Type() == TiXmlNode::ELEMENT && !strcmp(node->Value(), name)) &&
			((!attrname || !attrvalue) || !strcmp(attrvalue, getAttribute(attrname, "", node)))) {
			cur = node;
			return node;
		}
		node = node->NextSibling();
	}
	return 0;
}

bool CXMLTiny_OUT::isAttrMatched(const char* propname, const char* match)
{
	const char *attr = getAttribute(propname);
	return (attr && !strcmp(attr, match));
}

bool CXMLTiny_OUT::isMatched(const char* match, bool matchCase)
{
	if (matchCase)
		return !strcmp(((LPXNODE)cur)->Value(), match);
	else
		return !_stricmp(((LPXNODE)cur)->Value(), match);
}

const char* CXMLTiny_OUT::getNodeValue(int index)
{
	LPXNODE node = ((LPXNODE)cur);
	int i = 0;
	while (node) {
		if ((node->Type() == TiXmlNode::TEXT || node->Type() == TiXmlNode::ELEMENT) && i++ == index) break;
		node = node->NextSibling();
	}
	if (!node) return 0;
	const char* text = ((LPXELEMENT)node)->GetText();
	if (text == NULL)
	{
		return 0;
	}
	while (*text == '\n' || *text == '\r' || *text == '\t' || *text == ' ') text++;
	return *text ? text : 0;
}

int CXMLTiny_OUT::getNodeValueInt(int index)
{
	const char* val = getNodeValue(index);
	return val ? atoi(val) : 0;
}

const char* CXMLTiny_OUT::getNodeValueByName(const char* name, const char* defaultValue)
{
	if (!cur) return defaultValue;
	LPXNODE node = (LPXNODE)goFirst();
	do {
		if (node->Type() == TiXmlNode::ELEMENT && !strcmp(node->Value(), name)) {
			cur = node;
			return getNodeValue();
		}
		node = node->Parent();
	} while (node);
	goNext();
	return findNode(name) ? getNodeValue() : defaultValue;
}

const char* CXMLTiny_OUT::getAttribute(const char* propname, const char* defaultValue, void* node)
{
	LPXCHAR attr = ((LPXELEMENT)(node ? node : cur))->Attribute(propname);
	if (attr != NULL) {
		return attr;
	}
	return defaultValue;
}

int CXMLTiny_OUT::getAttributeInt(const char* propname, int def, void* node)
{
	LPXCHAR res = getAttribute(propname, "0", node);
	return atoi(res);
	
}

float CXMLTiny_OUT::getAttributeFloat(const char* propname, float def, void* node)
{
	LPXCHAR res = getAttribute(propname, "0", node);
	return (float)atof(res);
}

char* CXMLTiny_OUT::getNodeName()
{
	return (char*)((LPXNODE)cur)->Value();
}

void CXMLTiny_OUT::setNodeName(const char* nodename)
{
	//xmlNodeSetContent(cur, (const xmlChar*)value);
	if ((LPXNODE)cur && nodename)((LPXNODE)cur)->SetValue(nodename);
}

void CXMLTiny_OUT::setNodeValue(const char* value, bool isCData)
{
	if (!value || !cur)
	{
		return;
	}
	TiXmlText* pText = new TiXmlText(value);
	pText->SetCDATA(isCData);
	if(((LPXNODE)cur)->NoChildren())
	{
		((LPXNODE)cur)->LinkEndChild(pText);
	}
	else
	{
		((LPXELEMENT)cur)->Clear();
		((LPXNODE)cur)->LinkEndChild(pText);
	}
	//pText->
}

void CXMLTiny_OUT::setNodeValue(int value)
{
	char buf[64];
	sprintf_s(buf, sizeof(buf), "%d", value);
	setNodeValue(buf, false);
}

void CXMLTiny_OUT::setNodeValue(float value)
{
	char buf[64];
	sprintf_s(buf, sizeof(buf), "%1.4f", value);
	setNodeValue(buf, false);
}

void* CXMLTiny_OUT::addChild(void* node)
{
	if (cur) cur = ((LPXNODE)cur)->LinkEndChild((LPXNODE)node);
	return cur;
}

void* CXMLTiny_OUT::addChild(const char* nodename)
{
	if (!nodename)
	{
		return NULL;
	}
	LPXNODE node = NULL;
	if (cur) node = ((LPXNODE)cur)->LinkEndChild(new TiXmlElement(nodename));
	//xmlAddNextSibling((LPXNODE)cur, xmlNewText((const xmlChar*)"\n"));
	return node;
}

void* CXMLTiny_OUT::addChild(const char* nodename, const char* value)
{
	if (!value)
	{
		return NULL;
	}
	LPXNODE node = NULL;
	if (cur) { node = ((LPXNODE)cur)->LinkEndChild(new TiXmlElement(nodename));
	node->LinkEndChild(new TiXmlText(value));}	
	return node;
}

void* CXMLTiny_OUT::addChild(const char* nodename, int value)
{
	if (!nodename)
	{
		return NULL;
	}
	LPXNODE node = NULL;
	
	char buf[64];
	sprintf_s(buf, sizeof(buf), "%d", value);
	if (cur) {node = ((LPXNODE)cur)->LinkEndChild(new TiXmlElement(nodename));
	node->LinkEndChild(new TiXmlText(buf));}
	return node;
}

void* CXMLTiny_OUT::addChild(const char* nodename, float value)
{
	if (!nodename)
	{
		return NULL;
	}
	LPXNODE node = NULL;
	char buf[64];
	sprintf_s(buf, sizeof(buf), "%f", value);
	if (cur) {node = ((LPXNODE)cur)->LinkEndChild(new TiXmlElement(nodename));
	node->LinkEndChild(new TiXmlText(buf));}
	return node;
}

void* CXMLTiny_OUT::addText(const char* text)
{
	if(!text)
		return NULL;
	return ((LPXNODE)cur)->LinkEndChild(new TiXmlText(text));
}

void* CXMLTiny_OUT::addText(int value)
{
	char buf[64];
	_snprintf(buf, sizeof(buf), "%d", value);
	return ((LPXNODE)cur)->LinkEndChild(new TiXmlText(buf));
}

void* CXMLTiny_OUT::addAttribute(const char* name, const char* value)
{
	if(cur)((LPXELEMENT)cur)->SetAttribute(name, value);
	return cur;
}

void* CXMLTiny_OUT::setAttribute(const char* name, const char* value)
{
	if (!value || !cur)
	{
		return NULL;
	}
	((LPXELEMENT)cur)->SetAttribute(name, value);
	return cur;
}

void* CXMLTiny_OUT::setAttribute(const char* name, int value)
{
	if(cur)((LPXELEMENT)cur)->SetAttribute(name, value);
	return cur;
}

void* CXMLTiny_OUT::setAttribute(const char* name, float value)
{

	if(cur)((LPXELEMENT)cur)->SetDoubleAttribute(name, value);

	return cur;
}

void CXMLTiny_OUT::removeAttribute(const char* name)
{
	if(cur && name)((LPXELEMENT)cur)->RemoveAttribute(name);

	return;
}

void* CXMLTiny_OUT::removeNode()
{
	LPXNODE node = cur;
	if(node && goParent())
	{	
		cur->RemoveChild(node);
		if (cur == node) 
			cur = 0;
		return cur;
	}
	
	return 0;
}

void* CXMLTiny_OUT::replaceNode(const void* rNode)
{
	LPXNODE node = (LPXNODE)cur;
	if (node && goParent())
	{
		return ((LPXNODE)cur)->ReplaceChild(node, *((LPXNODE)rNode));
	}
	return 0;
}

void CXMLTiny_OUT::Lock()
{
#ifdef WIN32
	if (!hLock)
		hLock = CreateMutex(0, TRUE, 0);
	else
		WaitForSingleObject(hLock, INFINITE);
#else
    pthread_mutex_lock(&mMutex);
#endif
}

void CXMLTiny_OUT::Unlock()
{
#ifdef WIN32
	ReleaseMutex(hLock);
#else
    pthread_mutex_lock(&mMutex);
#endif
}
_MC_STRPRO_END
