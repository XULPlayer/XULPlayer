/**
* filename: info_dvd.js
* a class to get dvd ifo
*
*/
function Cdvdifo(plugin) 
{
	this.plugin = plugin;
	this.dvdpath = "";
	this.ifoXmlDoc = null; /*document object*/
	this.titlenum = 0;
	this.ListTrack = document.getElementById('track-list');
	this.ListChapter = document.getElementById('chapter-list');
	this.ListAudio = document.getElementById('audio-list');
	this.ListSubtitle = document.getElementById('subtitle-list');
	this.broadcaster = document.getElementById('isDvdMode');
	this.trackArray = new Array(); 	
	this.chapterArray = new Array();
	//this.clearList(-1); //clear ALL
}

Cdvdifo.prototype.getTrackList = function() 
{
	if(!this.ifoXmlDoc) {return false;}
	
	var titles = this.ifoXmlDoc.getElementsByTagName("title");
	var e = this.ListTrack;
	
	while (this.trackArray.length > 0) this.trackArray.pop();
	
	if(titles.length > 0) {
		var mainmovie = this.ifoXmlDoc.getElementsByTagName("mainmovie")[0];
		e.appendItem('Main Movie', mainmovie.getAttribute("id"));
		this.trackArray.push({dur:'0'});
		for (var i = 1;i <= titles.length; i++) {
			var tmp = titles[i-1].getAttribute("id") +
				' [' + titles[i-1].getAttribute("playbackTimeStr") + ']';
			this.trackArray.push({dur:titles[i-1].getAttribute("playbackTime"),
					     framerate:titles[i-1].getAttribute("framerate"),
					     video_format:titles[i-1].getAttribute("video_format"),
					     width:titles[i-1].getAttribute("width"),
					     height:titles[i-1].getAttribute("height"),
					     display_aspect_ratio:titles[i-1].getAttribute("display_aspect_ratio")
					     });
			e.appendItem(tmp, i);
		}
	}
	selectDefault("track-list", 0);	
	return true;
};

Cdvdifo.prototype.getChapterList = function(TrackID) 
{
	if(!this.ifoXmlDoc || TrackID > this.titlenum) {
		return false;
	}
	//clear first
	this.clearList(1);

	var path = "//titles/title[" + TrackID + "]/chapters/chapter";
	var iterator = this.ifoXmlDoc.evaluate(path,this.ifoXmlDoc,null,XPathResult.ANY_TYPE,null);
	var thisNode = iterator.iterateNext(); 
	if(thisNode == null) {
//		alert("can not get chapter list");
		return false;
	}
	
	var a = this.chapterArray;
	while (a.length > 0)  a.pop();
	this.ListChapter.appendItem('All Chapters', '0');
	a.push({start: '0', end: '0'});
	var i = 0;
	var tmp = "";
	while(thisNode != null) {
		tmp = thisNode.getAttribute("id") +
			' [' + thisNode.getAttribute("playbackTimeStartStr") +
			' - ' + thisNode.getAttribute("playbackTimeEndStr") + ']';
		a.push({start: thisNode.getAttribute("PlaybackTimeStart"), end: thisNode.getAttribute("PlaybackTimeEnd")});
		this.ListChapter.appendItem(tmp, ++i);
		thisNode = iterator.iterateNext();
	}
	
	selectDefault("chapter-list", 0);
	return true;
};

Cdvdifo.prototype.getAudioList = function(TrackID) 
{
	if(!this.ifoXmlDoc || TrackID > this.titlenum) {
		return false;
	}
	//clear first
	this.clearList(2);
	
	var e = this.ListAudio; 
	var path = "//titles/title[" + TrackID + "]/audios/audio";
	var iterator = this.ifoXmlDoc.evaluate(path,this.ifoXmlDoc,null,XPathResult.ANY_TYPE,null);
	var thisNode = iterator.iterateNext(); 

	e.appendItem('No Audio', '-1');
	
	while(thisNode != null) {
		var tmp = '0x' + parseInt(thisNode.getAttribute("aid")).toString(16) +
							' - ' + thisNode.getAttribute("language") +
							'/' + thisNode.getAttribute("type") +
							'/' +thisNode.getAttribute("channels");
		e.appendItem(tmp, thisNode.getAttribute("aid"));
		thisNode = iterator.iterateNext();
	}
	selectDefault("audio-list", 1);	
	return true;
};

Cdvdifo.prototype.getSubtitleList = function(TrackID) 
{
	if(!this.ifoXmlDoc || TrackID > this.titlenum) {
		return false;
	}
	//clear first
	this.clearList(3);
	
	var e = this.ListSubtitle;
	var path = "//titles/title[" + TrackID + "]/subtitles/subtitle";
	var iterator = this.ifoXmlDoc.evaluate(path,this.ifoXmlDoc,null,XPathResult.ANY_TYPE,null);
	var thisNode = iterator.iterateNext(); 

	e.appendItem('No Subtitle', '-1');
	
	while(thisNode != null) {
		var tmp = '0x' + parseInt(thisNode.getAttribute("sid")).toString(16)+
							' - ' + thisNode.getAttribute("language");
		e.appendItem(tmp, thisNode.getAttribute("sid"));
		thisNode = iterator.iterateNext();
	}
	selectDefault("subtitle-list", 1);	
	return true;
};

Cdvdifo.prototype.getinfo = function(dvdpath) 
{
	this.clearList(-1); //clear ALL

	var ifoXML = "";
	var txt = "";

	this.broadcaster.setAttribute('hidden', 'false');	
	
	try {
		ifoXML = this.plugin.getDVDInfo(dvdpath);
		//alert(ifoXML);
	} catch(err) {
		var txt;
		txt = "There was an error when open dvd " + dvdpath + "\n\n"; 
		txt += "Error description: " + err.description + "\n\n"
		txt += "Click OK to continue.\n\n"
		alert(txt);
	}

	if (typeof(ifoXML) == 'undefined' || ifoXML == '') {
		alert("Please insert a DVD into drive " + dvdpath.substr(0, 2));
		this.ifoXmlDoc = null;
		return false;
	}

	this.dvdpath = dvdpath;
	this.ifoXmlDoc = new DOMParser().parseFromString(ifoXML, 'text/xml');
	
	if (this.ifoXmlDoc == null){
		alert("Please insert a DVD into drive " + dvdpath.substr(0, 2));
		return false;
	}
	var te = this.ifoXmlDoc.getElementsByTagName("titlenum");
	this.titlenum = te[0].getAttribute("num");

	this.getTrackList();
//	this.getChapterList(1);
	this.getAudioList(1);
	this.getSubtitleList(1);
	
	return true;
};

Cdvdifo.prototype.update = function() 
{
	if (this.ListTrack.selectedIndex == 0) {
		this.clearList(1);
//		this.clearList(2);
//		this.clearList(3);
		this.getAudioList(this.ListTrack.selectedItem.value);
		this.getSubtitleList(this.ListTrack.selectedItem.value);
	}
	else {
		this.getChapterList(this.ListTrack.selectedIndex);
		this.getAudioList(this.ListTrack.selectedIndex);
		this.getSubtitleList(this.ListTrack.selectedIndex);
	}
};

/**
 * clear given list
 * @param {int} idx
 */
Cdvdifo.prototype.clearList = function(idx) 
{
	var lists = new Array(this.ListTrack, this.ListChapter, this.ListAudio, this.ListSubtitle)
	if (idx > 3) return;
	
	if (idx < 0) { //clear all
		for (var i in lists) {
			while(lists[i].hasChildNodes()) {
				if (lists[i].lastChild.tagName == "listcols") break;
				lists[i].removeChild(lists[i].lastChild);
			}	
		}
	} else {
		while(lists[idx].hasChildNodes()) {
			if (lists[idx].lastChild.tagName == "listcols") break;
			lists[idx].removeChild(lists[idx].lastChild);
		}	
	}
};

Cdvdifo.prototype.selectStartTime = function()
{
	var cp = this.ListChapter;
	var count = cp.selectedCount;
	var a = this.chapterArray;
	if (count <= 0) return 0;

	if (cp.selectedIndex == 0 || cp.selectedIndex >= a.length) {
		return 0;
	}

	return parseInt(a[cp.selectedIndex].start);
};

Cdvdifo.prototype.selectEndTime = function()
{
	var cp = this.ListChapter;
	var count = cp.selectedCount;
	var a = this.chapterArray;
	
	if (count <= 0 || cp.selectedIndex == 0) {
		return this.trackTime();
	}
	var idx = cp.getSelectedItem(count-1).value;

	return parseInt(a[idx].end);
};

Cdvdifo.prototype.trackTime = function()
{
	var tk = this.ListTrack;
	var idx = tk.selectedIndex;
	
	if (idx == 0) {
		idx = tk.selectedItem.value;//main movies
	}
	else if (idx < 0 || idx >= this.trackArray.length) return 0;
	
	return parseInt(this.trackArray[idx].dur);
};

Cdvdifo.prototype.videoinfo = function()
{
	var tk = this.ListTrack;
	var idx = tk.selectedIndex;
	var ret =new Array();
	if (idx <= 0 || idx >= this.trackArray.length) return null;

	/*
	 framerate:		2500->25.00fps	2997->29.97fps
	 video_format:		0->NTSC		1->PAL
	 width:
	 height:
	 display_aspect_ratio:	0->4:3		3->16:9
	*/

	ret['framerate'] = this.trackArray[idx].framerate;
	ret['video_format'] = this.trackArray[idx].video_format;
	ret['width'] = this.trackArray[idx].width;
	ret['height'] = this.trackArray[idx].height;
	ret['display_aspect_ratio'] = this.trackArray[idx].display_aspect_ratio;
	
//	alert(ret['framerate'] + '\n' +
//	      ret['video_format'] + '\n' +
//	      ret['width'] + '\n' +
//	      ret['height'] + '\n' +
//	      ret['display_aspect_ratio']);
	return ret;
};


