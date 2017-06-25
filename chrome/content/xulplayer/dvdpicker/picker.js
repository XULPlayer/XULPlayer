//info.js
var TYPE_NOMEDIA = 0;
var TYPE_DVD = 1;
var TYPE_VCD = 2;
var TYPE_SVCD = 3;
var TYPE_CD = 4;
var TYPE_UNKNOWN = 5;
var dvdInfo;
var cdInfo;
var vcdInfo;

function Init(args)
{
    plugin = args[0];
    addFileCallback = args[1];
    if (args.length > 2) {
        defaultPath = args[2];
    }
    else {
        defaultPath = null;
    }
    mediaInfo = null;
    listCdrom(defaultPath);

    dvdInfo = new Cdvdifo(plugin);
    cdInfo  = new Ccdinfo(plugin);
    vcdInfo = new Cvcdinfo(plugin);

    mediaType = TYPE_UNKNOWN;
    cdromPath = '';

    showDvdInfo();
}

function showDvdInfo()
{
    dvdInfo.clearList(-1); //clear all
    cdromPath = getCdromPath();
    if (cdromPath == null) {
        document.title = "Invalid cdrom path";
        return false;
    }
    mediaType = plugin.detectMediaType(cdromPath);
    tip = "";
    switch (mediaType) {
        case TYPE_DVD:
            tip = "DVD";
            mediaInfo = dvdInfo;
            break;
        case TYPE_CD:
            tip = "CD";
            mediaInfo = cdInfo;
            break;
        case TYPE_VCD:
            tip = "VCD";
            mediaInfo = vcdInfo;
            break;
        case TYPE_NOMEDIA:
            document.title = "There is no media in CDROM " + cdromPath[0] + ":";
            return false;
        default:
            document.title = "Unknown media type in CDROM " + cdromPath[0] + ":";
            return false;
    }

    document.title = tip + " in CDROM " + cdromPath[0] + ":";

    return mediaInfo.getinfo(cdromPath);
}

function updateDvdifo()
{
    if (mediaType == TYPE_DVD && mediaInfo != null) {
        mediaInfo.update();
    }
}

// dvd://,[track id],[chapter id],[audio id],[sub id],[media path]
// vcd://,[track id],[media path]
//  cd://,[track id],[media path]
// if track id == 0, mean main track.
function AddtoPlaylist()
{
    var trackId = get_item_id('track-list');
    var urls = new Array();

    switch(mediaType) {
        case TYPE_DVD:
            var chapterId = get_item_id('chapter-list');
            var audioId = get_item_id('audio-list');
            var subtitleId = get_item_id('subtitle-list');
            if (chapterId <= 0) { // all chapter
                var url = "dvd://," + trackId + "," + chapterId + "," + audioId + "," + subtitleId + "," + cdromPath[0];
                urls.push(url);
            }
            else {
                ids = get_item_ids('chapter-list');
                for (i = 0; i < ids.length; i++) {
                    chapterId = ids[i];
                    var url = "dvd://," + trackId + "," + chapterId + "," + audioId + "," + subtitleId + "," + cdromPath[0];
                    urls.push(url);
                }
            }
            break;
        case TYPE_CD:
        case TYPE_VCD:
            strMediaType = ((mediaType == TYPE_CD)?"cd":"vcd");
            if (trackId <= 0) { // all track
                var url =  strMediaType + "://," + trackId + "," + cdromPath[0];
                urls.push(url);
            }
            else {
                ids = get_item_ids('track-list');
                for (i = 0; i < ids.length; i++) {
                    trackId = ids[i];
                    var url =  strMediaType + "://," + trackId + "," + cdromPath[0];
                    urls.push(url);
                }
            }
            break;
        default:
            return false;
    }
    
    for (i = 0; i < urls.length; i++) {
        addFileCallback(urls[i]);        
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////
//CDROM helper function
function listCdrom(defaultPath) 
{
    var ifoXML = plugin.getCDROMInfo();
//	alert(ifoXML);
    ifoXmlDoc = new DOMParser().parseFromString(ifoXML, 'text/xml');
    var cdrompopup = document.getElementById('cdrom-popup');
    var cdromlist =  document.getElementById('cdrom-list');
    
    if (!cdrompopup || !cdromlist) return;

    var path = "//cdroms/cdrom";
    var iterator = ifoXmlDoc.evaluate(path,ifoXmlDoc,null,XPathResult.ANY_TYPE,null);
    var thisNode = iterator.iterateNext();
    var item;
    /*var savedPath = dvdrip.getPluginPref("overall.ui.cdromPath")*/;

    var n = 0, i = 0;
    while (thisNode != null) {
        var tmp = thisNode.getAttribute("info");
        item = document.createElement("menuitem");
        item.setAttribute("label",tmp);
        tmp = thisNode.getAttribute("path")
        item.setAttribute("value", tmp);
        if (defaultPath != null &&
            tmp[0].toUpperCase() == defaultPath[0].toUpperCase()) {
            i = n;
        }
        n++; 
        cdrompopup.appendChild(item);
        thisNode = iterator.iterateNext();
    }
    cdromlist.selectedIndex = i;	
}

function getCdromPath()
{
    cdromlist =  document.getElementById('cdrom-list');
    var item = cdromlist.selectedItem;
    if (item == null) return null;
    return item.value;
}