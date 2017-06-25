if(typeof(WINDOW_CHANGE) != "boolean") {
	WINDOW_CHANGE = true;

if (typeof(Cc) == "undefined")
  var Cc = Components.classes;
if (typeof(Ci) == "undefined")
  var Ci = Components.interfaces;
  
var fs = false;
var curViewMode;
var tvMode = TV_START;
var noEventTime = 5100;
var winMaxed = false;
var resizers = ["resizer-left", "resizer-right", "resizer-top", "resizer-bottom", "resizer-topleft", "resizer-bottomright"];

function toggleFullScreen()
{
    fs = !fs;
	if(curViewMode == VIEW_CENTER) {
		mediaLibFullScreen();
		return;
	}
	
    noEventTime = 4100;
    if (fs) {
        for(var i in resizers)
            $e(resizers[i]).hidden = true;
        //mp.setWindowTop(true);
        hideAllExceptVideo(true);
        window.maximize();
    } else {
		for(var i in resizers){
			$e(resizers[i]).hidden = false;
		}
		window.restore();
		setViewMode();
    }
	if(isTvMode())
	{
		 if (fs) {
			getTvFrame().setFullScreen();
		 }
		 else
		 {
			getTvFrame().setNoFullScreen();
		 }
	}
}

function toggleMaximize()
{
	winMaxed = !winMaxed;
	if (winMaxed)
		window.maximize();
	else
		window.restore();
}

function toggleDualMode()
{
    dualmode = isChecked("dualMode");
    $e('dualVideoOptions').hidden = !dualmode;
    if (!dualmode) {
        mp.setActivePlayer(0);
		mp.exitDualMode();
    } 
}

/*
function slideViewMode(mode)
{
	if (!curViewMode) return;
	switch (mode) {
	case VIEW_FULL:
		$e("fullView").setAttribute("checked", true);
		break;
	case VIEW_SIMPLE:
		$e("simpleView").setAttribute("checked", true);
		break;
	case VIEW_DEVICE:
		$e("deviceView").setAttribute("checked", true);
		break;
	case VIEW_COMPACT:
		$e("compactView").setAttribute("checked", true);
		break;
	}
	setViewMode(mode);
}
*/

function isCenterMode()
{
	return curViewMode == VIEW_CENTER;
}

function isTvMode()
{
	return curViewMode == VIEW_TV;
}

function getTvFrame()
{
	return window.frames["net_tv_frame"];
}

function setViewMode(mode)
{
	var titlebar = $e('titlebar');
	var tabs = $e('mainTab');
	var playToolbar = $e("bottomBar");
	var splitter = $e("panelSplit");
	var mcexContainer = $e("mcexBox");
	var mcexSplitter = $e("mcexSplitter");
	
	if (!mode) {
		mode = parseInt($e('optViewMode').value);
	} else {
		$e('optViewMode').value = mode;
	}
	
	switch (mode) {
	case VIEW_FULL:
	case VIEW_DEVICE:
		titlebar.hidden = false;
		tabs.hidden = false;
		playToolbar.hidden = false;
		splitter.hidden = false;
		break;
	case VIEW_SIMPLE:
		titlebar.hidden = false;
		tabs.hidden = true;
		playToolbar.hidden = true;
		splitter.hidden = true;
		break;
	case VIEW_TV:
	case VIEW_COMPACT:
		titlebar.hidden = false;
		tabs.hidden = true;
		playToolbar.hidden = false;
		splitter.hidden = true;
		break;
	/*case VIEW_CENTER:
		titlebar.hidden = true;
		tabs.hidden = true;
		playToolbar.hidden = true;
		splitter.hidden = true;
		break;*/
	case 0:
		titlebar.hidden = false;
		tabs.hidden = true;
		playToolbar.hidden = true;
		splitter.hidden = true;
		break;
	}
		
	if (mode != curViewMode) {
		// view mode has been changed
		var isDeviceMode = (mode == VIEW_DEVICE);
		mcexContainer.hidden = !isDeviceMode;
		mcexSplitter.hidden = !isDeviceMode;
		
		curViewMode = mode;
		if (mode == VIEW_TV) {
			mp.stop();
			openNetTv();
		} else if (mode == 0) {
			openDiySoftware();
		} else {
			homeURL = getLocalString("homeURL");
			if (!homeURL) homeURL = "http://xulplayer.sourceforge.net/start";
			if(!mp.isPlaying()) {
				loadPage(homeURL);
			}
		}
	}
	onResize();
}

/*
function setTVMode(aTvMode, category)
{
	tvMode = aTvMode;
	switch(tvMode) {
		case TV_START:
			$e('mediaManager').hidden = true;
			$e('mediaList').hidden = true;
			loadPage("tv_utility/start/start.html");
			showVideoBoxFullScreen();
			break;
		case TV_FULL:
			$e('mediaManager').hidden = false;
			$e('mediaList').hidden = false;
			loadPage("tv_utility/start/mediacenter.html");
			// split the screen into 3 parts (MediaManager & video: 40%,
			// MediaList: 60%), and display 2 parts simutaneously
			hideMediaManager();
			showMediaListLeft();
			showVideoBox();
			break;
	}
	
	if(mm && category != undefined) {
		mm.LocateTopCategoryById(category);
	}
}*/

function openNetTv()
{
	if(mp.isPlaying())
	{
		mp.stop();
	}
	var nettv = $e("net_tv_frame");
	changeVideoBoxPage(INDEX_TVU);
	
	if (curViewMode == VIEW_CENTER) $e("tabMediaLib").hidden = false;
	if(nettv.getAttribute("src") == "")
	{
		var tvuUrl;
		// check TVU plugin	
		if (checkPlugin("TVU")) {
			tvuUrl = getLocalString("tvu.url.start");
		} else {
			tvuUrl = getLocalString("tvu.url.noplugin");
		}

		nettv.setAttribute("src","html/loading.html");
		setTimeout(function(){nettv.setAttribute("src", tvuUrl);}, 100);
	}
}

function openDiySoftware()
{
	var _frame = $e("gps_main");
	changeVideoBoxPage(6);
	_frame.setAttribute("src","tv_utility/gps_xul/start.html");
}

function openDownloadNav()
{
	var nav = $e("downloadNav");
	changeVideoBoxPage(INDEX_DOWNLOAD_NAV);
	//$e("tabMediaLib").hidden = true; /*(curViewMode != VIEW_CENTER);*/
	nav.setAttribute("src","http://127.0.0.1/bt/");
}

function zoomVideo()
{
    if (isChecked("keep25")) {
        mp.zoom(0.25);
    } else if (isChecked("keep50")) {
        mp.zoom(0.5);
    } else if (isChecked("keep150")) {
        mp.zoom(1.5);
    } else if (isChecked("keep200")) {
        mp.zoom(2);
    } else if (isChecked("keep100")) {
        mp.zoom(1);
    } else if (isChecked("keepvideo")){
        mp.zoom(-1.5);  // Need a number < 0 , but can't be negative integer
        fitVideoSize(false);
    } else {
        mp.zoom(0);
    }
}

function fitVideoSize(ifOriginal)
{
    var vh = 0, vw = 0;
    if(ifOriginal){
        vh = mp.getVideoHeight(); vw = mp.getVideoWidth();
    } else {
        vw = mp.getRealVideoWidth(); vh = mp.getRealVideoHeight(); 
    }
    if(vh <= 0 || vw <= 0 || vh > window.screen.height || vw > window.screen.width) return;
    var boxh = $e("videoBox").boxObject.height;
    var boxw = $e("videoBox").boxObject.width;
    var h = window.innerHeight, w = window.innerWidth;
    window.innerWidth = w - boxw + vw;
    window.innerHeight = h - boxh + vh;
    onResize();
}

function setWindowTop()
{
	var winTopmost = isChecked("windowTop");
    mp.setWindowTop(winTopmost);
}

function setWindowTopByKey()
{
	var winTopmost = isChecked("windowTop");
	$e('windowTop').setAttribute('checked', !winTopmost);
	mp.setWindowTop(!winTopmost);
}

function focusMethod()
{
    if(curViewMode == VIEW_CENTER) return;
	if(focusFromPlugin){
		focusFromPlugin = false;
        $e('windowIcon').focus();
        if(fs){
            noEventTime = 0;
            $e("bottomBar").hidden = false;
        }
    }
}

function onResize()
{
    switch (curViewMode) {
    case VIEW_DEVICE:
        if (window.outerWidth < 1000)
            window.outerWidth=1000;
        if (window.outerHeight < 550)
            window.outerHeight=550;
	break;
    case VIEW_FULL:
        if (window.outerWidth < 720)
            window.outerWidth=720;
        if (window.outerHeight < 510)
            window.outerHeight=510;
	break;
    case VIEW_COMPACT:
    case VIEW_SIMPLE:
        if (window.outerWidth < 720 )
            window.outerWidth=720;
        if (window.outerHeight < 270)
            window.outerHeight=270;
		break;
	case VIEW_CENTER:
		if(!winMaxed) toggleMaximize();
		return true;
    }
	//resizeVideoBox();
    return true;  
}

function hideAllExceptVideo(ifHide)
{
    var elements = [$e("titlebar"), $e('mainTab'), 
                    $e('bottomBar'), $e("panelSplit")];
    for each(var elem in elements){
		elem.hidden = ifHide;
    }
}
function showOrHideProgressbar()    //when noEvent time is bigger than 1500, hide progressbar
{
    if(noEventTime > 6000) return;
	noEventTime += mp.m_timeInterval;
    if(noEventTime > 4000){
        $e("bottomBar").hidden = true;
		noEventTime = 6500;
    } else {
		if($e("bottomBar").hidden){
			$e("bottomBar").hidden = false;
		}
    }
}       

/*
function toggleMediaLib()
{
    var mlbar = $e("media-lib-bar");
	mlbar.hidden = true;
    mlbar.nextSibling.hidden = true;
    if ($e("menu-show-ml").getAttribute("checked") == "true") {
            mlbar.hidden = false;
            mlbar.nextSibling.hidden = false;
            var xsl = loadXML("medialib/catalog.xsl");
            var xml = loadXML("file:///" + mediaLibPath + "\\filelist.xml");
            if (xsl && xml) transformXML(xml, xsl, "file-list");
    } else {
            mlbar.hidden = true;
            mlbar.nextSibling.hidden = true;
    }
}*/

// Attempt to restart the app
function restartApp()
{
	// Notify all windows that an application quit has been requested.
	var os = Cc["@mozilla.org/observer-service;1"].getService(Ci.nsIObserverService);
	var cancelQuit = Cc["@mozilla.org/supports-PRBool;1"].createInstance(Ci.nsISupportsPRBool);
	os.notifyObservers(cancelQuit, "quit-application-requested", "restart");
  
	// Something aborted the quit process.
	if (cancelQuit.data)
	  return;
  
	// attempt to restart
	var as = Cc["@mozilla.org/toolkit/app-startup;1"].getService(Ci.nsIAppStartup);
	as.quit(Components.interfaces.nsIAppStartup.eRestart |
			Components.interfaces.nsIAppStartup.eAttemptQuit);
  
	closeWindow();
}

function quitApp(aForceQuit)
{
  var appStartup = Components.classes['@mozilla.org/toolkit/app-startup;1'].
    getService(Components.interfaces.nsIAppStartup);

  // eAttemptQuit will try to close each XUL window, but the XUL window can cancel the quit
  // process if there is unsaved data. eForceQuit will quit no matter what.
  var quitSeverity = aForceQuit ? Components.interfaces.nsIAppStartup.eForceQuit :
                                  Components.interfaces.nsIAppStartup.eAttemptQuit;
  appStartup.quit(quitSeverity);
}


function closeWindow()
{
	mp.setTrayIcon(false);
	window.close();
}


function getStyle(elem,styleProp)
{
  var v = null;
  if (elem) {
    var s = document.defaultView.getComputedStyle(elem,null);
    v = s.getPropertyValue(styleProp);
  }
  return v;
}

/**
 * Simple JS container that holds rectangle information
 * for (x, y) and (width, height).
 */
function CScreenRect(inWidth, inHeight, inX, inY) {
  this.width = inWidth;
  this.height = inHeight;
  this.x = inX;
  this.y = inY;
}

/**
 * Get the current maximum available screen rect based on which screen
 * the window is currently on.
 * @return A |CScreenRect| object containing the max available 
 *         coordinates of the current screen.
 */
function getCurMaxScreenRect() {
  var screenManager = Cc["@mozilla.org/gfx/screenmanager;1"]
                        .getService(Ci.nsIScreenManager);
                        
  var curX = parseInt(document.documentElement.boxObject.screenX);
  var curY = parseInt(document.documentElement.boxObject.screenY);
  var curWidth = parseInt(document.documentElement.boxObject.width);
  var curHeight = parseInt(document.documentElement.boxObject.height);
  
  var curScreen = screenManager.screenForRect(curX, curY, curWidth, curHeight);
  var x = {}, y = {}, width = {}, height = {};
  curScreen.GetAvailRect(x, y, width, height);
  
  return new CScreenRect(width.value, height.value, x.value, y.value);
}

}
