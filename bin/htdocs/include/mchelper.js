var url = document.location.href;
var xmlhttp = window.XMLHttpRequest ? new XMLHttpRequest : new ActiveXObject("Microsoft.XMLHTTP");
if (!xmlhttp) alert("XMLHttpRequest object cannot be created.");

function setPageSize(width, height)
{
	window.innerWidth = width;
	window.innerHeight = height;
}

function moveCenter()
{
	window.moveTo((screen.availWidth - window.outerWidth) / 2, (screen.availHeight - window.outerHeight) / 2);
}

function GetToken(str, token)
{
	var idx = str.indexOf(token + '=');
	if (idx <= 0) return null;
	var argstr = str.substring(idx + token.length + 1);
	idx = argstr.indexOf('&');
	return idx >=0 ? argstr.substring(0, idx) : argstr;
}

function QueryPref(path, opts)
{
	var node;
	var requrl = "/pref/pref.xml?type&";
	if (opts)
		requrl += opts + "&";
	if (path)
		requrl += "path=" + path;
	xmlhttp.open("GET", requrl, false);
	xmlhttp.send(null);
	node = xmlhttp.responseXML.firstChild;
	if (!node) {
		alert("Failed to retrieve preference data from MediaCoder.");
		return null;
	}
	return DomHelper.getChildNode(node);
}

function QueryPlugin(type, option)
{
	var node;
	var requrl = "/pref/plugin.xml?type=" + type;
	if (option) requrl += "&" + option;
	xmlhttp.open("GET", requrl, false);
	xmlhttp.send(null);
	node = xmlhttp.responseXML.firstChild;
	if (!node) {
		alert("Failed to connect to MediaCoder.");
		return null;
	}
	node = DomHelper.getChildNode(node);
	if (type) node = DomHelper.findNode(node, type);
	return node;
}

function QueryString(url, async)
{
	try { 
		xmlhttp.open("GET", url, async);
		xmlhttp.send(null);
		return xmlhttp.responseText;
	}
	catch (ex) {
		return null;
	}
}

function SetPrefValue(param, val)
{
	xmlhttp.open("GET", "/pref/set?" + param + "=" + val, true);
	xmlhttp.send(null);
}

function GetPrefValue(param)
{
	xmlhttp.open("GET", "/pref/get?path=" + param, false);
	xmlhttp.send(null);
	return xmlhttp.responseText;
}

function RevertPrefValue(param)
{
	xmlhttp.open("GET", "/pref/revert?path=" + param, false);
	xmlhttp.send(null);
	return xmlhttp.responseText;
}

function OpenURL(urlstring) {
	xmlhttp.open('GET', '/openurl?url=' + urlstring, false);
	xmlhttp.send(null);
}



function PostData(requrl, data)
{
	xmlhttp.open("POST", requrl, false);
	//xmlhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xmlhttp.send(data);
}

