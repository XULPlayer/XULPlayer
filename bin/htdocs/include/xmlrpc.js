/*

xmlrpc.js beta version 1
Tool for creating XML-RPC formatted requests in JavaScript

Copyright 2001 Scott Andrew LePera

scott@scottandrew.com
http://www.scottandrew.com/xml-rpc

Copyright 2008 Stanley Huang
stanleyhuangyc@gmail.com

License: 
You are granted the right to use and/or redistribute this 
code only if this license and the copyright notice are included 
and you accept that no warranty of any kind is made or implied 
by the author.

*/

function XMLRPCMessage(methodname){
  this.method = methodname||"system.listMethods";
  this.params = [];
  this.paramNames = [];
  return this;
}

XMLRPCMessage.prototype.setMethod = function(methodName){
  if (!methodName) return;
  this.method = methodName;
}

XMLRPCMessage.prototype.addParameter = function(data, name){
  if (arguments.length==0) return;
  this.params[this.params.length] = data;
  this.paramNames[this.paramNames.length] = name ? name : null;
}

XMLRPCMessage.prototype.xml = function(){

  var method = this.method;
  
  // assemble the XML message header
  var xml = "";
  
  xml += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  xml += "<methodCall>\n";
  xml += "<methodName>" + method+ "</methodName>\n";
  xml += "<params>\n";
  
  // do individual parameters
  for (var i = 0; i < this.params.length; i++){
    var name = this.paramNames[i];
    var data = this.params[i];
    xml += "<param>\n";
	if (name) xml += "<name>" + name + "</name>\n";
    xml += "<value>" + XMLRPCMessage.getParamXML(XMLRPCMessage.dataTypeOf(data),data) + "</value>\n";
    
    xml += "</param>\n";
  }
  
  xml += "</params>\n";
  xml += "</methodCall>";
  
  return xml; // for now
}

XMLRPCMessage.dataTypeOf = function (o){
  // identifies the data type
  var type = typeof(o);
  type = type.toLowerCase();
  switch(type){
    case "number":
      if (Math.round(o) == o) type = "i4";
      else type = "double";
      break;
    case "object":
      var con = o.constructor;
      if (con == Date) type = "date";
      else if (con == Array) type = "array";
      else type = "struct";
      break;
  }
  return type;
}

XMLRPCMessage.doValueXML = function(type,data){
  var xml = "<" + type + ">" + data + "</" + type + ">";
  return xml;
}

XMLRPCMessage.doBooleanXML = function(data){
  var value = (data==true)?1:0;
  var xml = "<boolean>" + value + "</boolean>";
  return xml;
}

XMLRPCMessage.doDateXML = function(data){
  var xml = "<dateTime.iso8601>";
  xml += dateToISO8601(data);
  xml += "</dateTime.iso8601>";
  return xml;
}

XMLRPCMessage.doArrayXML = function(data){
  var xml = "<array><data>\n";
  for (var i = 0; i < data.length; i++){
    xml += "<value>" + XMLRPCMessage.getParamXML(XMLRPCMessage.dataTypeOf(data[i]),data[i]) + "</value>\n";
  }
  xml += "</data></array>\n";
  return xml;
}

XMLRPCMessage.doStructXML = function(data){
  var xml = "<struct>\n";
  for (var i in data){
    xml += "<member>\n";
    xml += "<name>" + i + "</name>\n";
    xml += "<value>" + XMLRPCMessage.getParamXML(XMLRPCMessage.dataTypeOf(data[i]),data[i]) + "</value>\n";
    xml += "</member>\n";
  }
  xml += "</struct>\n";
  return xml;
}

XMLRPCMessage.getParamXML = function(type,data){
  var xml;
  switch (type){
    case "date":
      xml = XMLRPCMessage.doDateXML(data);
      break;
    case "array":
      xml = XMLRPCMessage.doArrayXML(data);
      break;
    case "struct":
      xml = XMLRPCMessage.doStructXML(data);
      break;
	  case "boolean":
      xml = XMLRPCMessage.doBooleanXML(data);
      break;
    default:
      xml = XMLRPCMessage.doValueXML(type,data);
      break;
  }
  return xml;
}

function dateToISO8601(date){
  // wow I hate working with the Date object
  var year = new String(date.getYear());
  var month = leadingZero(new String(date.getMonth()));
  var day = leadingZero(new String(date.getDate()));
  var time = leadingZero(new String(date.getHours())) + ":" + leadingZero(new String(date.getMinutes())) + ":" + leadingZero(new String(date.getSeconds()));

  var converted = year+month+day+"T"+time;
  return converted;
} 
  
function leadingZero(n){
  // pads a single number with a leading zero. Heh.
  if (n.length==1) n = "0" + n;
  return n;
}

function getReponseValue(xmlval)
{
	var v = getChildNode(xmlval);
	var ret = getNodeValue(v);
	switch (v.nodeName) {
	case "boolean":
		return ret == "1";
	case "int":
	case "i4":
		return parseInt(ret, 10);
	case "double":
		return parseFloat(ret);
	case "string":
	case "date":
		return ret;
	case "struct":
		vals = v.getElementsByTagName("member");
		var result = new Array;
		for (var x = 0; x < vals.length; x++) {
			var name = getChildNodeValue(vals[x], "name");
			if (name != "") {
				var v = findChildNode(vals[x], "value");
				result[name] = getReponseValue(v);
			}
		}
		return result;
	}
	return null;
}

function parseRPC(xml)
{
	// retrieve return value
	if (!xml) return null;
	var vals = xml.getElementsByTagName("value");
	if (vals.length > 0) {
		return getReponseValue(vals[0]);
	} else {
		return null;	
	}
}

function getRPCPayload(method)
{
	var msg = new XMLRPCMessage();
	msg.setMethod(method);
	for(var i=1; i<arguments.length; i++) {
		msg.addParameter(arguments[i]);
	}
	return msg.xml();
}

function addHTML (html) {
  if (document.all) {
	document.body.insertAdjacentHTML('beforeEnd', html);
  } else if (document.createRange) {
	var range = document.createRange();
	range.setStartAfter(document.body.lastChild);
	var docFrag = range.createContextualFragment(html);
	document.body.appendChild(docFrag);
  }
  else if (document.layers) {
	var l = new Layer(window.innerWidth);
	l.document.open();
	l.document.write(html);
	l.document.close();
	l.top = document.height;
	document.height += l.document.height;
  }
}

var rpcURL = "/xmlrpc";
var rpcCallback = null;
var rpcMethod = null;
var retValue;

function getRPCReturnValue()
{
	var response = window.frames['rpcframe'].document;
	if (response.firstChild && response.firstChild.nodeName == "methodResponse") {
		retValue = parseRPC(response);
	} else if (response.XMLDocument) {
		var node = response.XMLDocument.getElementsByTagName("methodResponse");
		if (node.length > 0)
			retValue = parseRPC(node[0]);
	} else {
		return;
	}
	if (rpcCallback) rpcCallback(rpcMethod, retValue);
}

var postElementsHTML = "<div style='display:none'><form id='rpcform' method='post' target='rpcframe' enctype='application/x-www-form-urlencoded'><textarea name='payload'></textarea></form><iframe id='rpcframe' name='rpcframe'></iframe></div>";

function postDataRemote(url, data, onload)
{
	if (!document.getElementById("rpcform")) {
		/*addHTML(postElementsHTML);*/
		alert("Error to perform cross-domain POST");
		return false;
	}
	var frm = document.getElementById("rpcform");
	var ifr = document.getElementById("rpcframe");
	if (!frm || !ifr) {
		alert("Error posting data to " + url);	
		return false;
	}
	frm.action = url;
	frm.payload.value = data;
	/*ifr.onload = onload;*/
	frm.submit();
	return true;
}

function postData(url, data)
{
	var xmlhttp = window.XMLHttpRequest ? new XMLHttpRequest : new ActiveXObject("Microsoft.XMLHTTP");
	if (!xmlhttp) return null;
	try { 
		xmlhttp.open("POST", url, false);
		xmlhttp.send(data);
		return xmlhttp.responseXML;
	}
	catch (ex) {
		alert(ex);
		return null;
	}	
}

function callRPC(method)
{
	var msg = new XMLRPCMessage();
	msg.setMethod(method);
	for(var i=1; i<arguments.length; i++) {
		msg.addParameter(arguments[i]);
	}
	if (rpcURL.indexOf("://") > 0) {
		// different domain
		if (mcplugin) {
			var responseText = mcplugin.post(rpcURL, msg.xml());
			var parser = new DOMParser();
			var response = parser.parseFromString(responseText,"text/xml");
			var ret = parseRPC(response);
			if (rpcCallback) rpcCallback(method, ret);
			return ret;
		} else {
			rpcMethod = method;
			postDataRemote(rpcURL, msg.xml(), rpcCallback ? getRPCReturnValue : null);
			return null;
		}
	} else {
		// same domain
		var response = postData(rpcURL, msg.xml());
		var ret = parseRPC(response);
		if (rpcCallback) rpcCallback(method, ret);
		return ret;
	}
}
