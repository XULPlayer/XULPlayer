/* Create Main menu and process events in main html page
   Created by Zoominla, 12-04-2008         
   
*/

if (typeof(MAIN_MENU_INCLUDE) != "boolean") {
	MAIN_MENU_INCLUDE = true;

	var cur_id = "";
	var flag = 0,
	sflag = 0;
	var menuHtml = "";      // html content of "menuPanel"
    var itemIcon = "/images/mainmenu/item_icon.gif";
    
    //------- Create Menu -----------
    function buildMainMenu()
    {
        var theMenuPanel = $('menuPanel');
        menuHtml = "<div id='menuHead'></div> \
            <div id='mainMenu' class='menuType'>\
            <ul id='menuBody'> ";
        var  menuDoc = LoadXMLFile("mainMenu.xml");
        var firstClassNodes = menuDoc.selectNodes("//Nodes/FirstClass");
        var firstClassCount = 0;
        for(i=0; i<firstClassNodes.length; ++i) {
            var nodeText = firstClassNodes[i].getAttribute("text");
            firstClassCount ++;
            var firstClassId = "m" + firstClassCount;
            menuHtml += "<li class='L1'><a href='javascript:menuClick(\"" +
            firstClassId + "\")' id='" + firstClassId + "'><span><img src='" + itemIcon + 
            "'/>" + nodeText + "</span></a></li>\
            <li style='list-style: none; display: block'>" +
            "<ul id='" + firstClassId + "Node" + "' style='display:none;' class='U1'> ";
            var childNode = getChildNode(firstClassNodes[i]);
            var childCount = 1;
            while (childNode) {
                branchNodeToHtml(childNode, firstClassId + "_" + childCount);
                var nextNode = getNextNode(childNode);
                childNode = nextNode;
                childCount ++;
            }
            menuHtml += "</ul> </li>" ;
        }
        menuHtml += "</ul> </div> <div id='menuBottom'></div>";
        theMenuPanel.innerHTML = menuHtml;
    }
    
    function branchNodeToHtml(branchNode, id)
    {
        if(!branchNode.hasChildNodes()) {   // if it has no child (leaf node)
            leafNodeToHtml(branchNode, id);
        } else {
            var theMenuPanel = $('menuPanel');
            var nodeText = branchNode.getAttribute("text");
            menuHtml += "<li class='menuBranch'><a href='javascript:menuClick(\"" + id +
            "\")' id='" + id + "'><span><img src='" + itemIcon + "'/>" + nodeText + "</span></a></li> \
                     <li style='list-style: none; display: block'> \
                        <ul id='" + id + "Node" + "' style='display: none;'> ";
            var childNode = getChildNode(branchNode);
            var childCount = 1;
            while (childNode) {
                branchNodeToHtml(childNode, id + "_" + childCount);
                var nextNode = getNextNode(childNode);
                childNode = nextNode;
                childCount ++;
            }
            menuHtml += "</ul> </li>" ;
        }
    }
    
    function leafNodeToHtml(leafNode, id)
    {
        var theMenuPanel = $('menuPanel');
        var nodeMethod = leafNode.getAttribute("method");
        var nodeText = leafNode.getAttribute("text");
        menuHtml += "<li class='menuLeaf'><a href='javascript:setCurrent(\"" +
        id + "\");" + nodeMethod + "' id='" + id + "'><span><img src='" + itemIcon + "'/>" + nodeText + "</span></a></li> "
    }
    
	//-------- Menu Click processor -------
	function menuClick(srcId)
	{
		var targetid, targetelement;
		var strbuf;
		var srcElem = $(srcId);
		targetid = srcId + "Node";
		targetelement = document.getElementById(targetid);
		if (targetelement.style.display == "none") {
			srcElem.className = "active";
			targetelement.style.display = '';
		} else {
			srcElem.className = "";
			targetelement.style.display = "none";
		}
	}
	
	function setCurrent(id) {
		cur_link = document.getElementById(cur_id);
	    if (cur_link) cur_link.className = "";
		cur_link = document.getElementById(id);
		if (cur_link) cur_link.className = "active";
		cur_id = id;
	}
	
	//-------- 打开网址 -------
	function openURL1(URL, id) 
	{
		set_current(id);
		if (URL.substr(0, 7) != "http://" && URL.substr(0, 6) != "ftp://") URL = "/general/" + URL;
		parent.openURL(URL, 0);
	}
	
	function openURL2(URL, id) 
	{
		set_current(id);
		URL = "/app/" + URL;
		parent.openURL(URL, 0);
	}
	
	//-------- 菜单全部展开/收缩 -------
	/*var menu_flag = 1;
	function menu_expand() {
		if (menu_flag == 1) expand_text.innerHTML = "收缩";
		else expand_text.innerHTML = "展开";

		menu_flag = 1 - menu_flag;

		var links = document.getElementsByTagName("A");
		for (i = 0; i < links.length; i++) {
			srcelement = links[i];
			if (srcelement.parentNode.className.toUpperCase() == "L1" || srcelement.parentNode.className.toUpperCase() == "menuBranch") {
				targetelement = document.getElementById(srcelement.id + "d");
				if (menu_flag == 0) {
					targetelement.style.display = '';
					srcelement.className = "active";
				} else {
					targetelement.style.display = "none";
					srcelement.className = "";
				}
			}
		}
	}*/
	
	/*menu_flag = 1;
			//expand_text.innerHTML = "展开";
			var links = document.getElementsByTagName("A");
			for (i = 0; i < links.length; i++) {
				srcElem = links[i];
				if (srcElem.parentNode.className.toUpperCase() == "L1" && srcelement.className == "active" && srcElem.id.substr(0, 1) == "m") {
					menu_flag = 0;
					//expand_text.innerHTML = "收缩";
					break;
				}
			}*/
}