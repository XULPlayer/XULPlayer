if(typeof(SKIN_PROCESS) != "boolean") {
	SKIN_PROCESS = true;
    
	function initSkinMenuItems()				//Initialize setSkin menu
	{
		var skinDir = Io.getSpecialDir("skin", "AChrom");
        if(!skinDir) alert("Error!");
        var files = skinDir.directoryEntries;
        var pmenu = $e("skin-popup");
        while(files.hasMoreElements()) {
            var afile = files.getNext();
            afile.QueryInterface(Components.interfaces.nsIFile);
            if(afile.isFile()) {
                var fileName = afile.leafName;
                if(fileName.indexOf(".zip") != -1){
                    var item = document.createElement("menuitem");
                    var skinName = getFileTitle(fileName);
                    item.setAttribute("label", skinName);
                    item.setAttribute("type", "radio");
                    item.setAttribute("id", skinName);
                    item.setAttribute("oncommand", "changeSkin(getAttribute('label'));");
                    pmenu.appendChild(item);
                }
            }
        }
	}
	
	function changeSkin(skinName)
	{
		var chromeDir = Io.getSpecialDir(null, "AChrom");
		
		if(!chromeDir) {return;}
			chromeDir.append("chrome.manifest");
		
		var lines = Io.openTextFile(chromeDir);
		if(lines instanceof Array) {
			for (var x in lines) {
				if (lines[x].substr(0, 4) == "skin")
					lines[x] = "skin xulplayer classic/1.0 jar:skin/" + skinName + ".zip!/";
			}
			Io.saveFile(lines.join(' '), chromeDir, true);
		}
		msgbox(getLocalString("setLangNotice"));
	}
}