if(typeof(MEDIALIB_INCLUDE) != "boolean") {

MEDIALIB_INCLUDE = true;

var topDirLen;
function enumMediaFiles(stream, dir)
{
	var topDir = null;
	if(!dir) return;
	topDir = Io.getFileFrom(dir);
	var files = topDir.directoryEntries;
        var dirPath = replaceXmlChars(topDir.path);
        var i;
        i = dirPath.lastIndexOf("\\");
        var dirName = dirPath.substr(i + 1);
        var count = 0;
	while(files.hasMoreElements()) {
		var afile = files.getNext();
		afile.QueryInterface(Components.interfaces.nsIFile);
		if(afile.isFile()) {
                var fullPath = replaceXmlChars(afile.path);
                var ext = getFileExt(fullPath);
                var type = getContentType(ext);
                if (type != CONTENT_OTHER) {
                        var title = getFileTitle(afile.leafName);
                        var fileSize = afile.fileSize;
                        var time = afile.lastModifiedTime;
                        var date = new Date(time);
                        var ymd = date.getFullYear() + '-' + date.getMonth() + '-' + date.getDate();
                        var n = fullPath.lastIndexOf("\\");
                        if (n < 0) n = fullPath.lastIndexOf("/");

                        if (count == 0) {
							stream.writeString('<dir name=\"' + dirName + '\" path=\"' + dirPath + '\" >');
							//$("stats").label = dirPath;
                        }
                        stream.writeString('<item name=\"' + title + '\" ');
                        stream.writeString('size=\"' + fileSize + '\" ');
                        stream.writeString('ext=\"' + ext + '\" ');
                        stream.writeString('type=\"' + type + '\" ');
                        stream.writeString('date=\"' + ymd + '\" ');
                        stream.writeString('dir=\"' + fullPath.substr(0, n) + '\" ');
                        stream.writeString('path=\"' + fullPath.substr(n + 1) + '\" ');
                        stream.writeString('/>' + '\n');
                        count++;
                }
            }
	}
        if (count > 0) stream.writeString('</dir>');               

	files = topDir.directoryEntries;
	while(files.hasMoreElements()) {
		var afile = files.getNext();
		afile.QueryInterface(Components.interfaces.nsIFile);
                if(afile.isDirectory()) {
                        enumMediaFiles(stream, afile);
                }
	}
}

// Write media file info to xml
// Param: fileArr ---> an array of nsIFile objects (get from enumFolder function)
//        savefile---> where to save
function searchMediaFiles(dir, savefile)
{
	var charset = "UTF-8";
	var outputStream = getOutputStream(savefile);
	if(!outputStream) return;
	var os = Components.classes["@mozilla.org/intl/converter-output-stream;1"]
				   .createInstance(Components.interfaces.nsIConverterOutputStream);
	os.init(outputStream, charset, 0, 0x0000);
	
	os.writeString('<?xml version="1.0" encoding="UTF-8"?>\n<items>\n');
	topDirLen = dir.length + 1;
	enumMediaFiles(os, dir);
	os.writeString('</items>\n');
	os.close();
	outputStream.close();
	//$("stats").label = "";
}

function searchMedia()
{
	var dir = pick_dir();
	if (!dir) return;
	searchMediaFiles(dir, mediaLibPath + "\\filelist.xml");
	var xsl = loadXML("medialib/catalog.xsl");
	var xml = loadXML("file:///" + mediaLibPath + "\\filelist.xml");
	transformXML(xml, xsl, "file-list");
}

function onMediaLibClick(tree)
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
		if (items[i].getAttribute("container") == "true") {
			var subitems = items[i].getElementsByTagName("treeitem");
			for (x in subitems) {
				var filename = subitems[x].getAttribute("uri");
				if (filename && filename != "")
					addFile(filename);
			}
		} else {
			var filename = items[i].getAttribute("uri");
			if (filename && filename != "")
				addFile(items[i].getAttribute("uri"));
		}
	}
}

}