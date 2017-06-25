var curkey = null;
var curnode = null;
var url = document.location.href;
var dvdrip = null;

function SetPrefValue(param, val)
{
	dvdrip.setPref(param, val);
}

function GetPrefValue(param)
{
	return dvdrip.getPref(param);
}

function RevertPrefValue(param)
{
	xmlhttp.open("GET", "/prefs/revert?path=" + param, false);
	xmlhttp.send(null);
	return xmlhttp.responseText;
}

function getNextNode(node)
{
	while ((node = node.nextSibling) && node.nodeType!=1);
	return node;
}

function getChildNode(node)
{
	node = node.firstChild;
	while (node && node.nodeType != 1) {
		node = node.nextSibling;
	} 
	return node;
}

function loadXML(xmlFile)
{
	var $xml = new XMLHttpRequest;
	$xml.open('GET', xmlFile, false);
	$xml.overrideMimeType('text/xml');
	$xml.send(null);
	var xml = $xml.responseXML;
	if (!xml) {
		alert("Unable to load "+xmlFile);
		return null;
	}
	return xml;
}

function transformXML(xmlDoc, xslDoc, element)
{
	var XSLT = new XSLTProcessor;
	XSLT.importStylesheet(xslDoc);
	var e = document.getElementById(element);
	if (e) {
		while (e.firstChild) e.removeChild(e.firstChild);
		e.appendChild(XSLT.transformToFragment(xmlDoc, document));
	}
}

function getTransformedXML(xmlDoc, xslDoc)
{
	var XSLT = new XSLTProcessor;
	XSLT.importStylesheet(xslDoc);
	return XSLT.transformToFragment(xmlDoc, document);
}

function UpdateTree(path)
{
	var xmlFile = new DOMParser().parseFromString(dvdrip.getPluginPrefXML(path), 'text/xml');
	var xslTree = loadXML("prefs.xsl");
	transformXML(xmlFile, xslTree, "treeview");
}

function ShowValue(path)
{
	var xmlFile = new DOMParser().parseFromString(mc.getPref(path), 'text/xml');
	var xslFile = loadXML("value.xsl");
	transformXML(xmlFile, xslFile, "valuebox");

	xslFile = loadXML("desc.xsl");
	transformXML(xmlFile, xslFile, "descview");
	curkey = path;

	// update value in the tree
	var val = xmlFile.getElementsByTagName("value");
	if (val.length > 0) {
		//UpdateListValue(val[0].firstChild.nodeValue);
	}
}

function UpdateListValue(value)
{
	if (curnode) {
		var rownode = curnode.firstChild.firstChild;
		if ((rownode = rownode.nextSibling) && (rownode = rownode.nextSibling))
			rownode.setAttribute("label", value);
	}
}

function RevertValue()
{

	if (dvdrip == null) {
		alert("I can not get plugin");
		return false;
	}
	
	var ifoXML = dvdrip.getCDROMInfo();
	
	alert(ifoXML);
	
	return true;
//	if (curkey) {
//		var val = RevertPrefValue(curkey);
//		ShowValue(curkey);
//	}
}

function SaveValue(value)
{
	if (curkey && value) {
		SetPrefValue(curkey, value);
		UpdateListValue(value);
	}
}

function ChooseFile()
{
	var nsIFilePicker = Components.interfaces.nsIFilePicker;
	var fp = Components.classes["@mozilla.org/filepicker;1"].createInstance(nsIFilePicker);
	fp.init(window, "Select a File", nsIFilePicker.modeOpen);	
	var res = fp.show();
	if (res == nsIFilePicker.returnOK){
	  var thefile = fp.file;
		alert(thefile);
		return thefile;
	}
	return null;
}

function Init()
{
	var parent = window.opener;
	if (parent == null) return;
	dvdrip = parent.document.getElementById('player');
	UpdateTree(null);
	
}

function onTreeSelected(tree)
{
	var curidx = tree.currentIndex;
	var items = tree.getElementsByTagName("treeitem");
	var i;
	for (i = 0, idx = 0; i < items.length && idx < curidx; i++, idx++) {
		if (items[i].getAttribute("container") == "true") {
			if (items[i].getAttribute("open") != "true") {
				// skip child nodes
				var next = items[i].nextSibling;
				if (!next) next = items[i].parentNode.parentNode.nextSibling;
				if (next) {
					// look for the next right node in items array
					for (i++; i < items.length && items[i] != next; i++);
					i--;
				}
			}
		}
	}
	if (idx == curidx) {
		var key = items[i].getAttribute("uri");
		var node = items[i];
		while ((node = node.parentNode) && (node = node.parentNode) && node.nodeName == "treeitem") {
			key = node.getAttribute("uri") + "." + key;
		}
		curnode = items[i];
		ShowValue(key);
	}
}
