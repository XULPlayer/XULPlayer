var rndtoday = new Date(); 
var rndseed = rndtoday.getTime(); 
//-------------------- Constants define ------------------
var BROWSER_FF = 1;			//Firefox
var BROWSER_IE = 2;			//IE
var BROWSER_SAFARI = 3;		//Safari
var BROWSER_CAMINO = 4;		//Camino
var BROWSER_GECKO = 5;		//GECKO

function rnd() { 
	rndseed = (rndseed*9301+49297) % 233280; 
	return rndseed/(233280.0); 
}; 

function rand(number) { 
	return Math.floor(rnd()*number); 
};

function $(id)
{
	return document.getElementById(id);
}

function parseXMLText(text)
{
	var parser = new DOMParser();
	var doc = parser.parseFromString(text,"text/xml");
	return doc;
}

function transformXML(xmlDoc, xslDoc, element)
{
	var XSLT = new XSLTProcessor;
	XSLT.importStylesheet(xslDoc);
	var e = $(element);
	if (e) {
		while (e.firstChild) e.removeChild(e.firstChild);
		e.appendChild(XSLT.transformToFragment(xmlDoc, document));
	}
}

function setContent(xmlDoc, element)
{
	var e = document.getElementById(element);
	if (e) {
		while (e.firstChild) e.removeChild(e.firstChild);
		e.appendChild(xmlDoc);
	}
}

function timeStr2Int(strTime)
{ /*xx:xx:xx.xxx to msec*/
	if (strTime == null) return 0;
	
	var h = parseInt(strTime.substring(0, 2), 10);
	var m = parseInt(strTime.substring(3, 5), 10);
	var s = parseInt(strTime.substring(6, 8), 10);
	var r = parseInt(strTime.substring(9, 12), 10);
//	alert( strTime + "->"+ h + " " + m + " " + s + " " + r);
	return (h * 3600 + m * 60 + s) * 1000 + r;
}

function isChecked(id)
{
	var e = document.getElementById(id);
	return e && e.getAttribute("checked") == "true";
}

function getValue(id)
{
	return document.getElementById(id).value;
}

function setValue(id, value)
{
	return document.getElementById(id).value = value;
}

function getValueInt(id)
{
	return parseInt(document.getElementById(id).value);
}

function getValueFloat(id)
{
	return parseFloat(document.getElementById(id).value);
}

function getIndex(id)
{
	return document.getElementById(id).selectedIndex;
}

function getFileTitle(filename)
{
	var i = filename.lastIndexOf('\\');
	var str;
	if (i > 0)
		str = filename.substr(i + 1);
	else
		str = filename;
	i = str.lastIndexOf('.');
	if (i > 0)
		str = str.substr(0, i);

	return str;	//replaceXmlChars(str);
}

function getFileExt(filename)
{
	var i = filename.lastIndexOf('.');
	if (i < 0) return "";
	return filename.substr(i + 1).toUpperCase();
}

function getFileType(filename)
{
	var ext = getFileExt(filename);
	switch (ext) {
	case "MPG":
	case "MPEG":
		return "MPEG";
	case "VOB":
		return "MPEG-2";
	case "TS":
	case "TP":
		return "MPEG-2 TS";
	case "RMVB":
	case "RM":
		return "RealMedia";
	case "WMV":
		return "Windows Media";
	case "MOV":
		return "QuickTime";
	case "JPG":
		return "JPEG Image";
	case "PNG":
		return "PNG Image";
	case "GIF":
		return "GIF Image";
	case "BMP":
		return "BMP Image";
	case "HTML":
	case "HTM":
	case "PHP":
	case "ASP":
		return "Web Page";
	default:
		return ext;
	}
}

function getContentType(ext)
{
	switch (ext) {
	case "MPG":
	case "MPEG":
	case "MPV":
	case "VOB":
	case "AVI":
	case "MP4":
	case "MKV":
	case "RMVB":
	case "RM":
	case "MKV":
	case "M4V":
	case "WMV":
	case "ASF":
	case "MOV":
	case "FLV":
	case "TS":
	case "TP":
		return CPlayer.CONTENT_VIDEO;
	case "SWF":
		return CPlayer.CONTENT_SWF;
	case "MP3":
	case "AAC":
	case "OGG":
	case "M4A":
	case "WAV":
	case "MKA":
	case "WV":
	case "FLAC":
	case "RAM":
	case "RA":
	case "APE":
		return CPlayer.CONTENT_AUDIO
	case "JPG":
	case "JPEG":
	case "PNG":
	case "GIF":
	case "BMP":
		return CPlayer.CONTENT_IMAGE;
	}
	return CPlayer.CONTENT_OTHER;
}

function loadValue(id)
{
	var e = document.getElementById(id);
	if (e) {
		var v = e.getAttribute("data");
		if (v) e.value = v;
	}
}

function saveValue(id)
{
	var e = document.getElementById(id);
	if (e) e.setAttribute("data", e.value);
}

function getAttr(id, attr)
{
	var e = document.getElementById(id); 
	if (e) return e.getAttribute(attr);
	return null;
}

function setAttr(id, attr, val)
{
	return document.getElementById(id).setAttribute(attr, val);
}

function enable(id)
{
	document.getElementById(id).setAttribute("disabled", false);
}

function disable(id)
{
	document.getElementById(id).setAttribute("disabled", true);	
}

function dupAttr(id, attrFrom, attrTo)
{
	var e = document.getElementById(id);
	if (e) {
		var v = e.getAttribute(attrFrom);
		if (v) e.setAttribute(attrTo, v);
	}
}

function getAttrInt(id, attr)
{
	var e = document.getElementById(id); 
	if (e) return parseInt(e.getAttribute(attr));
	return null;
}

function getAttrFloat(id, attr)
{
	var e = document.getElementById(id); 
	if (e) return parseFloat(e.getAttribute(attr));
	return null;
}

function padNumber2(num)
{
	return (num < 10 ? "0" : "") + num;
}

//Clear Array object
function clearArray(arr)
{
	var len = arr.length;
	for(var i=0; i<len; ++i)
		arr.pop();
}

//retrieve the dir of filename
function getFileDir(filename)
{
	var i = filename.lastIndexOf('\\');
	var str = null;
	if (i > 0)
		str = filename.substr(0, i);
	return str;
}

// replace XML reserved characters
function replaceXmlChars(str)
{
	for (var i = 0; i < str.length; i++) {
		switch (str.charAt(i)) {
		case "&":
			str = str.substr(0, i) + "&amp;" + str.substr(i + 1)
			i += 4;
			break;
		case "<":
			str = str.substr(0, i) + "&lt;" + str.substr(i + 1)
			i += 3;
			break;
		case ">":
			str = str.substr(0, i) + "&gt;" + str.substr(i + 1)
			i += 3;
			break;
		}
	}
	return str;
}

function getBrowserType()
{
   if(navigator.userAgent.indexOf("MSIE") !=-1) return BROWSER_IE;
   if(navigator.userAgent.indexOf("Firefox") !=-1) return BROWSER_FF;
   if(navigator.userAgent.indexOf("Safari") !=-1) return BROWSER_SAFARI;  
   if(navigator.userAgent.indexOf("Camino") !=-1) return BROWSER_CAMINO;
   if(navigator.userAgent.indexOf("Gecko/") !=-1) return BROWSER_GECKO;
   return 0;
}