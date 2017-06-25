/* All operations and setting related to user interface, detached from
   player.js and convert.js 
   Created by Zoominla, 01-03-2009         
*/

if(typeof(WEBENCODER_GUI_INCLUDE) != "boolean") {
	WEBENCODER_GUI_INCLUDE = true;

var videoHeight = 270;
var videoWidth = 480;

var converter = null;
var player = null;
var workdir;
var selcecteItemNumber = -1;

var isIEBrowser = false;
var divpop = null;			// Process popup window
var interop = null;			// Interoperation of plugin and Mediacoder
var mp = null;				// Instance of CPlayer
var mc = null;				// Instance of CConvert
var progress = null;		// Instance of progress bar
var progressBackImg = '/images/stories/progress_bg.png';
var progressFrontImg = '/images/stories/progress_front.png';

var pluginDiv = null;		// Div contain plugin and relative control panel
var pluginCloseImg = '/media/system/images/closebox.png';	// close button image
var PLUGIN_DIVID = 'xulplugin_id';

var covDiv = null;
var covMsgDiv = null;
var COV_DIVID = 'cov_id';
var COV_TITLEID = 'cov_title';
var COV_MSGDIV_ID = 'cov_msg_div';
var covproc = null;
var COV_PERCENTID = 'cov_percent';
var COV_TIMEID = 'cov_time';
var covStartImg = '/images/stories/cov_start.gif';
var covExitImg = '/images/stories/cov_exit.gif';
var covPauseImg = '/images/stories/cov_pause.gif';

var fileList = null;		//Instance of CFileList

function SetPlayerSize(screenWidth, screenHeight)
{
	if(isIEBrowser) {
		setAttr("MCoder", "width", screenWidth);
		setAttr("MCoder", "height", screenHeight);
    } else {
		setAttr("lcd", "width", screenWidth);
		setAttr("lcd", "height", screenHeight);
    }
}

function UnInit()
{
	if (typeof(window.extUninit) == 'function') extUninit();
   // Save extension preference
    if(extPrefs)
        mc.createXmlFile(extPrefs, workdir+PREFS_XML);
	// Save file list
	if(fileList) fileList.save();
	
	if(mp) {
		 mp.unInitialize();
	}
}

function Init()
{
	var isExtPage = (typeof(window.setDefaults) == 'function' || typeof(window.getDefaultPrefs) == 'function');
	if (!isExtPage) return;
	
	switch (getBrowserType()) {
		case BROWSER_FF:
		case BROWSER_GECKO:
			break;
		case BROWSER_IE:
			isIEBrowser = true;
			break;
		default:
			alert("Unknown Browser");
			return;
	}
	
	window.onunload=UnInit;

	interop = new Interop(isUsingMc);

	divpop = new DivPop(isIEBrowser);
	divpop.setCloseCallback(hidePlugin);
	//isUsingMc is defined in extension.js
	if(!isUsingMc) {
		player = $((isIEBrowser ? "MCoder" : "lcd"));
		if (!player) {	
			createPluginDiv();
			createCovDiv();
			createCovMsgDiv();		
			// Register drag event of pluginDiv;
			dragElement(pluginDiv);
			player = $(isIEBrowser ? "MCoder" : "lcd");
		}
		
		if (!player) {
			alert("Plugin not installed");
			return;
		}
		
		converter = player;
	
		try{
			converter.init(0);
		}
		catch(e)
		{
			if(isIEBrowser) {				
				showIEPluginDownload();			
			}
			return;	
		}
		workdir = converter.workDirectory;
		
		mp = new CPlayer(converter);
		mp.setLoopCallback(playLoop);
		mp.setStopCallback(playStopCallback);
		mp.enablePlayNext(true);
	
		mc = new CConvert(converter);
		mc.setLoopCallback(convertLoop);
		mc.setStopCallback(convertStop);
		mc.revertPrefs();
	}
	
	// initialize MediaCoder Extenion RPC
	extInit();
	
	// load extension default values
	if (typeof(window.getDefaultPrefs) == 'function')
		PostPrefsXML(window.getDefaultPrefs());
	if (typeof(window.setDefaults) == 'function')
		window.setDefaults();

	if (document.getElementById('fileNames')) {
		fileList = new CFileList('fileNames', workdir+XML_DIR+QUEUE_XML);
		fileList.initialize();
	}
}

function startPlay(fName)
{
    if (mp.isPlaying()) {
        mp.stop();
    }
	mp.enablePlayNext(true);
	if(!fName && fileList) fName = fileList.getSelectedFile();
	if(fName) mp.setPlayingFile(fName);
	
    //bDetached is a checkbox on html form. To determine whether the video is detached form the embed.
	var detached = false;
    var opts = " -framedrop -autosync 30 -vf screenshot";
    if(!detached){
        SetPlayerSize(videoWidth, videoHeight);
    }
    mp.play(opts, !detached);
}

function playLoop()
{
	progress.moving(mp.getPercentage());
}

function playStopCallback()
{
	if(mp.playNextEnabled()) playNext();
}

function stopCmd()
{
	mp.enablePlayNext(false);
	mp.stop();
}

function takeScreenshot()
{
	var ret = mp.screenshot();
	alert(ret);
}
function playNext()
{
	if (fileList) {
		fileList.moveNext();
		startPlay(fileList.getSelectedFile());
	}
}

function playPrevious()
{
	if (fileList) {
		fileList.movePrevious();
		startPlay(fileList.getSelectedFile());
	}
}


function leaveWarning(e)
{
	e = e || window.event;
	if(e) {
		e.returnValue=confirm("Transcoding is in progress, are you sure to stop?");
	}
}

//============================ Transcoding part ===============================
var outputDir;
var fileNum = 0;
var isover = false;
var extPrefs = null;            //Store extension settings
var curVideoW = 0;              //current selected video width 
var curVideoH = 0;              //current selected video height 

var QUEUE_XML = "\\queue.xml";
var LOCAL_XML = "\\local.xml";
var PREFS_XML = "\\prefs.xml";
var XML_DIR   = "\\xml_plugin";

//get prefs by hq
function postToWebEncoder(data)
{	
	var prefsDoc = new XMLDom();
	prefsDoc.loadXML(data);	
	var prefsNode = DomHelper.getChildNode(prefsDoc);
		
	var extDoc = new XMLDom();
	if(extPrefs == null)
	{		
		extDoc.loadXML(data);
		DomHelper.removeChildren(extDoc);
		var root = extDoc.createElement("MediaCoderPrefs");
		extDoc.appendChild(root);
		extPrefs = DomHelper.GetXmlContent(extDoc);	
	}
	else
		extDoc.loadXML(extPrefs);	
	var extRoot = DomHelper.getChildNode(extDoc);		
	
	//update changes parameters
	var childNode = DomHelper.getChildNode(prefsNode);
	for(; childNode ;) {
		var keyval = childNode.getAttribute("key");
		var valval = childNode.getAttribute("value");
		DomHelper.changeNode(extDoc, extRoot, keyval, valval);
		childNode = DomHelper.getNextNode(childNode);
	}
	extPrefs = DomHelper.GetXmlContent(extDoc);
	if(typeof(mc.setPrefs) == 'function') {
		mc.setPrefs(extPrefs);
		//if (!mc.setPrefs(extPrefs)) alert("Error apply settings\n\n"+extPrefs);
	}
}

function convert()
{
	if(!fileList || fileList.isEmpty()) return;
	var commandXML = makeParseFile();
	$(COV_TITLEID).innerHTML = fileList.getSelectedFileName();	
	divpop.showDiv(covDiv);
	var ofp = 0.0;
	divpop.showPercentDiv($(COV_PERCENTID), ofp);
	divpop.showTimeDiv($(COV_TIMEID), 0);
	mc.start(commandXML);	
}

function convertLoop()
{
	var floatPercent = mc.getPercent();
	if(floatPercent) {
		divpop.showTimeDiv($(COV_TIMEID), mc.m_lasttime*0.001);
		divpop.showPercentDiv($(COV_PERCENTID), floatPercent);
		covproc.moving(floatPercent.toFixed(2));
	}
}

function convertStop()
{
	divpop.showPercentDiv($(COV_PERCENTID), 100.0);
	divpop.showPercentDiv($(COV_PERCENTID), 100.0);
	divpop.showDiv(covMsgDiv);
	divpop.showPopup(covMsgDiv);	
}

function updateInfo()
{

}

function addMediaFile()
{	
	if (fileList) {
		var strFileName = mc.getMediaFile();
		if(!strFileName) return;
		fileList.addFile(strFileName);
	}
}

function genFileInfo()
{
	if(fileList) fileList.generateFileInfo();
}

function addFolder()
{
	var dirName = mc.getFolder();
	if(!dirName) return;
	mp.enumDir(dirName, 'enumDirCallback', false);
}

function enumDirCallback(filename, filesize)
{
	if(filename ) {
		var mtype = getContentType(getFileExt(filename));
		if(mtype == CPlayer.CONTENT_VIDEO || mtype == CPlayer.CONTENT_AUDIO)
			fileList.addFile(filename);
	}
}

function setOutputDir()
{
	outputDir = mc.getFolder();
}

function setPriority(shortname)
{
   var filepath = workdir +  XML_DIR + LOCAL_XML;
   doc = LoadLocalXMLFile(filepath);
   if(doc == null) return;    
   var root = getChildNode(doc);
   var node = getChildNode(root);
   node = getNodeByAttribute(node, "node", "key", "overall.coder.priority");
   node.setAttribute("value",shortname);
   var xmlContent = DomHelper.GetXmlContent(doc);
   mc.createXmlFile(xmlContent, filepath);
}

//! change priority when change select item
function changePriority()
{
   var sel = $("coderpriority");
   setPriority(sel.options[sel.selectedIndex].value);
}

function setCovCode(id)
{	
   if(fileList.isEmpty())return;
}

function setUsage(id)
{
   if(fileList.isEmpty())return;

}

function makeParseFile() 
{
	var path = workdir + XML_DIR;
	var doc = new XMLDom();
	doc.async = false;
    doc.preserveWhiteSpace=false;
	var head = doc.createProcessingInstruction('xml','version="1.0" encoding="UTF-8"');     	
	doc.appendChild(head);
    
    // Process extension setting
	if(extPrefs == null) {
		alert("Please, choose a solution !");
	}	
	// Process selected file info
	resNode = fileList.getCurItemNode();
	doc.appendChild(resNode.cloneNode(true));

	return DomHelper.GetXmlContent(doc);
}

//====================== Plugin Div and related operation =====================
var hidePluginLeft = '-5000px';			// hide plugin via move it out of page
var curPluginLeft = '';
function forbiddenPop()
{
	if(event.stopPropagation)
	{
		event.stopPropagation();
	}
	else
	{
		event.cancelBubble=true;
	}
}
function createPluginDiv()
{
	var objWidth = videoWidth+8;
	var winWidth = 0;
	if(isIEBrowser) {
		winWidth = parseInt(document.body.scrollWidth);
	} else {
		winWidth = parseInt(document.documentElement.scrollWidth);
	}
	
	var objPos = {x: (winWidth-objWidth)/2, y: 100};
	curPluginLeft = objPos.x + 'px';
	//------------------- Create popup plugin div --------------------
    pluginDiv = document.createElement('div');
    pluginDiv.id = PLUGIN_DIVID;
    styleStr = "background:#338899; left: " +  curPluginLeft + ";top:"
		+ objPos.y + "px;position: absolute; width: " + objWidth + "px;";
    pluginDiv.style.cssText = styleStr;
	
    // -----------------popup window top---------------------
    var winTop = document.createElement('div');
    winTop.style.cssText = "margin-left:auto;margin-right:auto;padding:3px;font-weight:bold;text-align:left;font-size:14px;";
        //Create top table
        var closeImg = HtmHelper.make('img', {'src':pluginCloseImg,'style.cursor':'pointer', 'style.height':'16px', 'style.width':'16px'});
        if(isIEBrowser) {
            closeImg.attachEvent('onclick', function(){divpop.closeWindow()});closeImg.attachEvent('onmousedown', function(){forbiddenPop();});
        } else {
            closeImg.addEventListener('click', function(){divpop.closeWindow()}, false);
        }
        var tClose = HtmHelper.make('td', {'width':'20px', 'align':'right'}, closeImg);
        var tTitle = HtmHelper.make('td', {'width':'98%'}, 'WebEncoder');
        var thead = HtmHelper.make("thead", {"style.height" : "0px"});
        var tfoot = HtmHelper.make("tfoot", {"style.height" : "0px"});
        var tRow = HtmHelper.make("tr", [tTitle, tClose]);
        var topBar = HtmHelper.make("table", [thead, HtmHelper.make('tbody',null,tRow), tfoot])

    winTop.appendChild(topBar);

	// Plugin object
	var pluginObj = null;
	if(!isIEBrowser) {
		pluginObj = HtmHelper.make('embed', {'id':'lcd', 'width':'0px', 'height':'0px', 'type':'application/x-xulplayer-scriptable-plugin',
							   'pluginspage':'http://www.mcex.org/webencoder/webencoder.xpi'});
	} else {
		pluginObj = HtmHelper.make('object', {'id':'MCoder', 'width':'1px', 'height':'1px',
								   'classid':'CLSID:2BBC8331-53EA-42E4-96A0-243B88AB1FD5'});
	}

	winTop.appendChild(pluginObj);
    pluginDiv.appendChild(winTop);
	
    // ------------------popup window content-------------------
    var winContent = document.createElement('div');
    winContent.style.cssText = "margin-left:auto; margin-right:auto; font-size:12px;";
	
	// Generate progress bar
	progress = new ProgressBar(videoWidth, 12, progressBackImg, progressFrontImg);
	progress.init();
	if(isIEBrowser) {
		var pdiv = progress.getContainer();
		pdiv.attachEvent('onmousedown', function(){forbiddenPop();});
		}
	
	//Add play control buttons to popup div window
	var input = HtmHelper.maker('input');
	var playBtn = input({'value':'Play', 'type':'button'});
	var pauseBtn = input({'value':'Pause', 'type':'button'});
	var stopBtn = input({'value':'Stop', 'type':'button'});
	var forwardBtn = input({'value':'Forward', 'type':'button'});
	var rewindBtn = input({'value':'Rewind', 'type':'button'});
	var nextBtn = input({'value':'Next', 'type':'button'});
	var previousBtn = input({'value':'Previous', 'type':'button'});
	var screenShotBtn = input({'value':'Screenshot', 'type':'button'});
	if(isIEBrowser) {	
		playBtn.attachEvent('onclick', function(){mp.play()});playBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		pauseBtn.attachEvent('onclick', function(){mp.pause()});pauseBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		stopBtn.attachEvent('onclick', function(){mp.stop()});stopBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		forwardBtn.attachEvent('onclick', function(){mp.forward(10)});forwardBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		rewindBtn.attachEvent('onclick', function(){mp.rewind(10)});rewindBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		nextBtn.attachEvent('onclick', function(){playNext()});nextBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		previousBtn.attachEvent('onclick', function(){playPrevious()});previousBtn.attachEvent('onmousedown', function(){forbiddenPop();});
		screenShotBtn.attachEvent('onclick', function(){takeScreenshot()});screenShotBtn.attachEvent('onmousedown', function(){forbiddenPop();});
	} else {
		playBtn.addEventListener('click', function(){startPlay()}, false);
		pauseBtn.addEventListener('click', function(){mp.pause()}, false);
		stopBtn.addEventListener('click', function(){stopCmd()}, false);
		forwardBtn.addEventListener('click', function(){mp.forward(10)}, false);
		rewindBtn.addEventListener('click', function(){mp.rewind(10)}, false);
		nextBtn.addEventListener('click', function(){playNext()}, false);
		previousBtn.addEventListener('click', function(){playPrevious()}, false);
		screenShotBtn.addEventListener('click', function(){takeScreenshot()}, false);
	}
	
	// Add transcode buttons to popup div window
	/*var addFileBtn = input({'value':'Add File', 'type':'button'});
	var addFolderBtn = input({'value':'Add Folder', 'type':'button'});
	var startBtn = input({'value':'Start', 'type':'button'});
	if(isIEBrowser) {
		playBtn.attachEvent('onclick', function(){mp.play()});
		pauseBtn.attachEvent('onclick', function(){mp.pause()});
		stopBtn.attachEvent('onclick', function(){mp.stop()});
		forwardBtn.attachEvent('onclick', function(){mp.forward(10)});
		rewindBtn.attachEvent('onclick', function(){mp.rewind(10)});
	} else {
		playBtn.addEventListener('click', function(){mp.play()}, false);
		pauseBtn.addEventListener('click', function(){mp.pause()}, false);
		stopBtn.addEventListener('click', function(){mp.stop()}, false);
		forwardBtn.addEventListener('click', function(){mp.forward(10)}, false);
		rewindBtn.addEventListener('click', function(){mp.rewind(10)}, false);
	}*/
	var ctrlPanel = HtmHelper.make('div', {'marginLeft':'auto', 'marginRight':'auto','style.textAlign':'center'},
					[progress.getContainer(), HtmHelper.make('br'),	// progress bar
					 playBtn, pauseBtn, stopBtn, forwardBtn, rewindBtn,
					 HtmHelper.make('br'), nextBtn, previousBtn, screenShotBtn,
					 HtmHelper.make('hr', {'width':'85%'}) 		// splitter
					]);	
	
    winContent.appendChild(ctrlPanel);
    pluginDiv.appendChild(winContent);
	hidePlugin();
    document.body.appendChild(pluginDiv);
}

function showPlugin()
{
	if(isIEBrowser) {
		pluginDiv.style.left = curPluginLeft;
	} else {
		pluginDiv.style.visibility = '';
	}
}
function hidePlugin()
{
	if(isIEBrowser) {
		pluginDiv.style.left = hidePluginLeft;		
	} else {
		pluginDiv.style.visibility = 'hidden';
	}
	if(mp) stopCmd();
}

function dragElement(obj)
{
	var x = 0, y = 0;
	var enableDrag = false;
    obj.onmousedown=function(evt){
        var d = document;
		if(!evt) evt = window.event;  
        x = evt.layerX? evt.layerX : evt.offsetX;
		y = evt.layerY? evt.layerY : evt.offsetY;  
        if(obj.setCapture)  
            obj.setCapture();  
        else if(window.captureEvents)  
            window.captureEvents(Event.MOUSEMOVE|Event.MOUSEUP);
		enableDrag = true;
	};
  
	obj.onmousemove=function(evt){
		if(enableDrag) {
			if(!evt) evt = window.event;  
			if(!evt.pageX) evt.pageX = evt.clientX;  
			if(!evt.pageY) evt.pageY = evt.clientY;  
			var tx = evt.pageX-x, ty = evt.pageY-y;  
			obj.style.left = tx + 'px';  
			obj.style.top = ty + 'px';  
		}
	};  

	obj.onmouseup=function(){
		if(obj.releaseCapture)  
			obj.releaseCapture();  
		else if(window.captureEvents)  
			window.captureEvents(Event.MOUSEMOVE|Event.MOUSEUP);
		enableDrag = false;
	};    
}

function createCovDiv()
{
	var width = 980;
	var height = width * 0.1;
	var left = (1002 - width) * 0.5;
	covDiv = document.createElement('div');
	covDiv.id = COV_DIVID;
	covDiv.style.width = width + 'px';
	covDiv.style.height = height + 'px';
	covDiv.style.left = left + 'px';
	var top = 0;	
	top = parseInt(document.body.scrollHeight);
	if(isIEBrowser)
	{
		top = 235 - top
	}
	else
	{
		top = 225 - top;
	}
	covDiv.style.top = top + "px";	
	//add other element
	//with div style
	var divL, divM, divR;
	divL = document.createElement('div'); divL.className = 'left'; 
	divL.style.height = height; divL = covDiv.appendChild(divL);
	divM = document.createElement('div'); divM.className = 'mid';  
	divM.style.height = height; divM = covDiv.appendChild(divM);
	divR = document.createElement('div'); divR.className = 'left'; 
	divR.style.height = height; divR = covDiv.appendChild(divR);
	
	//left part
	var percent = document.createElement('div');
	percent.id = COV_PERCENTID;
	percent.className = "numBar";
	percent.style.left = '5px';
	percent.style.top  = height * 0.33 + 'px';
	divL.appendChild(percent);
	
	var btn1 = document.createElement('img');
	btn1.className = "covBotton";
	btn1.src = covPauseImg;
	btn1.style.top = height * 0.57 + 'px';
	btn1.style.left = width * 0.05 + 'px';
	btn1.onclick = function()
	{
		mc.pause();
		if(!mc.isWaiting())
		{
			btn1.src = covStartImg;
		}	
		else
		{
			btn1.src = covPauseImg;
		}
	};
	divL.appendChild(btn1);
	
	var btn2 = document.createElement('img');
	btn2.className = "covBotton";
	btn2.src = covExitImg;
	btn2.style.top = height * 0.57 + 'px';
	btn2.style.left = width * 0.9 + 'px';
	btn2.onclick = function()
	{
		mc.end();
		divpop.hideDiv(covDiv);
	};
	divR.appendChild(btn2);
		
	//middle part
	covproc = new ProgressBar(width * 0.7, 12, progressBackImg, progressFrontImg);
	covproc.init();	
	var tdiv = covproc.getContainer();
	tdiv.style.left = width * 0.15 + 'px';
	tdiv.style.top  = height * 0.35 + 'px';
	divM.appendChild(covproc.getContainer());	
	var title = document.createElement('span');
	title.id = COV_TITLEID;
	title.className = "textTitle";
	title.style.position = 'relative';
	title.style.height = '22px'
	title.style.top = height * 0.7 + 'px';
	if(!isIEBrowser)
	{
		title.style.left = width * 0.1 + 'px';
	}
	divM.appendChild(title);
	
	//right part
	var timebar = document.createElement('div');
	timebar.id = COV_TIMEID;
	timebar.className = "timeBar";
	timebar.style.left = width * 0.88 + 'px';
	timebar.style.top  = height * 0.33 + 'px';
	divR.appendChild(timebar);
	
	covDiv = divpop.madeRoundBox(covDiv);
	divpop.hideDiv(covDiv);	
	$("center").appendChild(covDiv);
	//document.body.insertBefore(covDiv, obj);
}

function createCovMsgDiv()
{
	var objWidth = 400;
	var winWidth = parseInt(document.body.scrollWidth);
	var objPos = {x: (winWidth-objWidth)/2, y: 200};
	covMsgDiv= document.createElement('div');
    var styleStr = "background:#aaffcc; left: " +  objPos.x + "px;top:"
		+ objPos.y + "px;position: absolute; width: " + objWidth + "px;";
    covMsgDiv.style.cssText = styleStr;
	covMsgDiv.id = COV_MSGDIV_ID;
	
    // -----------------popup window top---------------------
    var winTop = document.createElement('div');
    winTop.style.cssText = "margin-left:auto;margin-right:auto;padding:3px;font-weight:bold;text-align:left;font-size:14px;";
        //Create top table
        var closeImg = HtmHelper.make('img', {'src':pluginCloseImg,'style.cursor':'pointer', 'style.height':'16px', 'style.width':'16px'});
         if(isIEBrowser) {
            closeImg.attachEvent('onclick', function(){
				divpop.closeWindow();
				divpop.hideDiv(covMsgDiv);
				mc.end();
				divpop.hideDiv(covDiv);
				});
			closeImg.attachEvent('onmousedown', function(){forbiddenPop();});
        } else {
            closeImg.addEventListener('click', function(){
				divpop.closeWindow();
				divpop.hideDiv(covMsgDiv);
				mc.end();
				divpop.hideDiv(covDiv);
				}, false);
        }		
		var tClose = HtmHelper.make('td', {'width':'20px', 'align':'right'}, closeImg);
        var tTitle = HtmHelper.make('td', {'width':'95%'}, 'Congratuelations To You:');
        var thead = HtmHelper.make("thead", {"style.height" : "0px"});
        var tfoot = HtmHelper.make("tfoot", {"style.height" : "0px"});
        var tRow = HtmHelper.make("tr", [tTitle, tClose]);
        var topBar = HtmHelper.make("table", [thead, HtmHelper.make('tbody',null,tRow), tfoot]);
	winTop.appendChild(topBar);
	
	var successInfo = HtmHelper.make('p', 'Convert Completed ! File Save under C:\Converted. Click the close to continue!');	 
	covMsgDiv.appendChild(winTop);
	covMsgDiv.appendChild(HtmHelper.make('hr'));
	covMsgDiv.appendChild(successInfo);		
	document.body.appendChild(covMsgDiv);
	divpop.hideDiv(covMsgDiv);	
}

//======================== IE Plugin Download div ===========================
function showIEPluginDownload()
{
	var ieDownPopup = new DivPop(true);
	var objWidth = 260;
	var winWidth = parseInt(document.body.scrollWidth);
	var objPos = {x: (winWidth-objWidth)/2, y: 200};
	var iePluginDownDiv = document.createElement('div');
    var styleStr = "background:#aaffcc; left: " +  objPos.x + "px;top:"
		+ objPos.y + "px;position: absolute; width: " + objWidth + "px;";
    iePluginDownDiv.style.cssText = styleStr;
	iePluginDownDiv.id = 'iePluginDownDiv';
	
    // -----------------popup window top---------------------
    var winTop = document.createElement('div');
    winTop.style.cssText = "margin-left:auto;margin-right:auto;padding:3px;font-weight:bold;text-align:left;font-size:14px;";
        //Create top table
        var closeImg = HtmHelper.make('img', {'src':pluginCloseImg,'style.cursor':'pointer', 'style.height':'16px', 'style.width':'16px'});
        closeImg.attachEvent('onclick', function(){
								ieDownPopup.closeWindow();
								var tempDiv = document.getElementById('iePluginDownDiv');
								if (tempDiv != null) tempDiv.parentNode.removeChild(tempDiv);
							});
        var tClose = HtmHelper.make('td', {'width':'20px', 'align':'right'}, closeImg);
        var tTitle = HtmHelper.make('td', {'width':'95%'}, 'Plugin Download');
        var thead = HtmHelper.make("thead", {"style.height" : "0px"});
        var tfoot = HtmHelper.make("tfoot", {"style.height" : "0px"});
        var tRow = HtmHelper.make("tr", [tTitle, tClose]);
        var topBar = HtmHelper.make("table", [thead, HtmHelper.make('tbody',null,tRow), tfoot]);
	winTop.appendChild(topBar);
	var downAdrr = HtmHelper.make('a', {'href':'http://www.mcex.org/webencoder/iecoder.msi'},
								  'WebCoder Installer.msi(Ver 1.0.0)');
	var paragraph = HtmHelper.make('p', '  You should download and install the plgunin first. Click the link to download!');
	paragraph.style.cssText = 'margin:6px;';
	iePluginDownDiv.appendChild(winTop);
	iePluginDownDiv.appendChild(HtmHelper.make('hr'));
	iePluginDownDiv.appendChild(downAdrr);
	iePluginDownDiv.appendChild(paragraph);
	document.body.appendChild(iePluginDownDiv);
	ieDownPopup.showPopup(iePluginDownDiv);
}

}