/******************************************************************
* MediaCoder extension base scripts
* Distributed under GPL license
* Copyright (c) 2007,2008,2009 Stanley Huang
* All rights reserved.
******************************************************************/

var mcurl;
var mcport;
var rpcurl;
var mcplugin = null;
var isUsingMc = false;
var postsDone = 0;

function AlertConnError()
{
	if (debug) alert("Error connecting to MediaCoder.");
}

function PostPrefsXML(data)
{
	if (postsDone) {
		document.getElementById("norevert").value = "1";
	}
	postDataRemote(mcurl+"prefs/import", data);
	postToWebEncoder(data, postsDone == 0); //to use by tthq
	postsDone++;
}

/*
function QueryPref(path, opts)
{
	var node;
	var requrl = "/prefs/prefs.xml?type&";
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
	return getChildNode(node);
}
*/
function AddFile(filename, index)
{
	if (filename) {
		callRPC("mc.queue.add", filename, index);
		callRPC('ui.list.refresh');
	}
}

function SetWindowSize(width, height)
{
	window.innerWidth = width;
	window.innerHeight = height;
}

function ShowItem(id)
{
	var e = document.getElementById(id);
	if (e) e.style.display = "block";
}

function HideItem(id)
{
	var e = document.getElementById(id);
	if (e) e.style.display = "none";
}

function ValueOf(id)
{
	return document.getElementById(id).value;
}

function GetToken(str, token)
{
	var idx = str.indexOf(token + '=');
	if (idx < 0) return null;
	var argstr = str.substring(idx + token.length + 1);
	idx = argstr.indexOf('&amp;');
	return idx > 0 ? argstr.substring(0, idx) : argstr;
}

function setPref(key, val)
{
	postPrefs(makePref(key, val));
	//callRPC("mc.prefs.set", param, val);
}

function getPref(param)
{
	return callRPC("mc.prefs.get", param);
}

function makePref(key, value)
{
	return '<node key="' + key + '" value="' + value + '"/>';
}

function makePrefHead()
{
	return '<?xml version="1.0" encoding="UTF-8"?><MediaCoderPrefs>';	
}

function makePrefTail()
{
	return '</MediaCoderPrefs>';
}

function revertPrefs()
{
	callRPC("ui.prefs.revert");
}

function postPrefs(xmlbody)
{
	var xml = makePrefHead() + xmlbody + makePrefTail();
	PostPrefsXML(xml);
}

function getItemFileName(index)
{
	var selitem
	if (index)
		selitem = callRPC("mc.queue.get");
	else
		selitem = callRPC("mc.queue.get", index);
	return selitem["filename"];
}

function getOutputFileName(index)
{
	var filename
	if (index)
		filename = callRPC("mc.queue.output");
	else
		filename = callRPC("mc.queue.output", index);
	return filename;
}

function playWithSettings(filename)
{
	var playopts = callRPC("mc.mplayer.opts");
	mcplugin.play(filename, playopts["options"], true);
}

function getProgress()
{
	return callRPC("mc.stats.progress");
}

function extInit(plugin)
{
	host = GetToken(document.location.href, "h");
	if (!host) host = "localhost";
	if (!mcport) mcport = GetToken(document.location.href, "p");
	if (!mcport) mcport = "19819";
	mcurl = "http://" + host + ":" + mcport + "/";
	rpcURL = mcurl + "xmlrpc";
	if (plugin) {
		mcplugin = plugin;
		mcplugin.init();
		mcplugin.keepAspect = false;
	}
	// enable parameter controls
	var w = document.getElementById("extWrapper");
	if (w) {
		e = w.getElementsByTagName("select");
		for (var x = 0; x < e.length; x++) e[x].disabled = false;
		e = w.getElementsByTagName("input");
		for (var x = 0; x < e.length; x++) e[x].disabled = false;
	}
}

function extUninit()
{
	if (mcplugin) {
		mcplugin.uninit();
	}
}

function showDefaultControls()
{
}

// player related
var PLAYER_POS = 0;
var PLAYER_STATE = 1;
var PLAYER_PERCENTAGE = 2;
var PLAYER_DURATION = 3;
var PLAYER_HAS_AUDIO = 4;
var PLAYER_HAS_VIDEO = 5;

function getPlayerPos()
{
	return mcplugin.getInfo(PLAYER_POS);
}

function getPlayerPercentage()
{
	return mcplugin.getInfo(PLAYER_PERCENTAGE);
}

function playerSeek(pos, type)
{
	mcplugin.seek(pos, type);	
}

//extInit();