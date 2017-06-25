/*
 mlnet.js
*/

var Mlnet = {
//--------------------- Interface functions -------------------------
    start: function() {
	this.downloader = window.arguments[0];
	this.downloader.start_Mlnet();
    },
    stop: function() {
	this.downloader.P2P_stop_Mlnet();	
    },
    getDownload: function () {
//	dumpErr(this._on);
	if (!this._on) return;
	try {
            var xmlString = this.downloader.pollDownloadings_Mlnet();
	    MDialog.updateDownloads(this._processDownloads(xmlString), this);
	}
	catch(ex) {
		dumpErr("getDownload():" + ex);
	}	
    },
    addDownload: function(url) {
	if (!this._on) this.start();
	if (!this._on) return false;

        return this.downloader.addDownload_Mlnet(url);
    },
    resumeItem: function(id) {
	return this.downloader.resumeItem_Mlnet(parseInt(id));
    },
    pauseItem: function(id) {
	return this.downloader.pauseItem_Mlnet(parseInt(id));
    },
    stopItem: function(id) {
	//FIXME:
	return this.downloader.pauseItem_Mlnet(parseInt(id));
    },
    removeItem: function(id) {
	this.downloader.removeItem_Mlnet(parseInt(id));
	Tree._downloads = [];
	this.downloader.getDownloadings_Mlnet();
	this.getDownload();
    },
    cancelItem: function(id) {
	this.removeItem(id)
    },
    setItemPriority: function(id, prio) {
        return; //TODO:
    },
//----------------------helper functions----------------------
    get downloader() {
	return window.arguments[0];
    },
    get _on() {
	return this.downloader.isOn_Mlnet();
    },
    get state() {
        return this.downloader.state_Mlnet();
    },
    _processDownloads: function(xmlString) {
	//alert("Enter _processDownloads()" + xmlString);
	var xmlDOM = new DOMParser().parseFromString(xmlString, 'text/xml');

	var nodes = xmlDOM.getElementsByTagName("file");
	var ret = new Array();
	//Components.utils.reportError(nodes.length);
	for (i = 0; i < nodes.length; i++) {
	    var info = new Object;
	    for (j = 0; j < nodes[i].childNodes.length; j++)
	    for each (let childNode in nodes[i].childNodes) {
		if (typeof(childNode.nodeName) == "undefined" ||
		    typeof(childNode.childNodes) == "undefined" || childNode.childNodes.length == 0)  continue;
		info[childNode.nodeName] =  childNode.childNodes[0].nodeValue;
	    }
	    ret.push(info);
	}
	return ret;
    }
};
