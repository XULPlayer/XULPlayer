/**
* filename: cdinfo.js
* a class to get cdda tracks info
*
*/

function Ccdinfo(plugin) 
{
	this.plugin = plugin;
	this.ifoXmlDoc = null; /*document object*/
	this.ListTrack = document.getElementById('track-list');
	this.broadcaster = document.getElementById('isDvdMode');
}

Ccdinfo.prototype.getTrackList = function()
{
	if(this.ifoXmlDoc ==  null) {
		return false;
	}
	
	var tracks = this.ifoXmlDoc.getElementsByTagName("track");
	
	if(tracks.length > 0) {
		for (var i = 0;i < tracks.length; i++) {
			var tmp = "Track" + tracks[i].getAttribute("id")+
							' [' + tracks[i].getAttribute("length") + ']';
			this.ListTrack.appendItem(tmp, tracks[i].getAttribute("id"));
		}
	}
	return true;
};

Ccdinfo.prototype.getinfo = function(path)
{
	this.clearlist();
	var ifoXML = this.plugin.getCDInfo(path);

	this.broadcaster.setAttribute('hidden', 'true');	

//	alert(ifoXML);
	if (ifoXML == "error") {
		alert("Please insert a CD into drive " + path.substr(0, 2));
		this.ifoXmlDoc = null;
		return false;
	}

	this.ifoXmlDoc = new DOMParser().parseFromString(ifoXML, 'text/xml');
	
	if (this.ifoXmlDoc == null){
		alert("Please insert a CD into drive " + path.substr(0, 2));
		return false;
	}

	return this.getTrackList();
}

Ccdinfo.prototype.clearlist = function()
{
	while(this.ListTrack.hasChildNodes()) {
		if (this.ListTrack.lastChild.tagName == "listcols") break;
		this.ListTrack.removeChild(this.ListTrack.lastChild);
	}
};
