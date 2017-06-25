/* Popup a div window to show something, and meanwhile darken the background
   grdually. The effect is similar to SF download page.
   Created by Zoominla, 12-27-2008
*/

// When create object, you must pass instance name
function DivPop(isIE)
{
    this.isIEBrowser = isIE;
    this.m_timeInterval = 40;
    this.m_timerId = null;
    this.m_closeCallback = null;
    this.m_closeImg = null;
    //---------------------- Private Methods ------------------------------
    /*Hide select elements on the background, because in IE browser the select
     elements will hide the popup div (it's a IE bug)*/
    this.hideSelect = function(bHide) {
       var state = bHide? 'hidden' : '';
       var objl = document.getElementsByTagName('select');
       for (var i = 0; i < objl.length; i++) {
           objl[i].style.visibility = state;
       }
    }
}

//------------ Class properties ------------
DivPop.BGPAGE_ID = "temp_background";
DivPop.POPWIN_ID = "popup_window";

//------------ public methods ------------
DivPop.prototype.mousePosition = function(ev) {
    if (ev.pageX || ev.pageY) {
        return { x: ev.pageX, y: ev.pageY };
    }
    return {
        x: ev.clientX + document.body.scrollLeft - document.body.clientLeft,
        y: ev.clientY + document.body.scrollTop - document.body.clientTop
    };
};

DivPop.prototype.setCloseCallback = function(func) {
    this.m_closeCallback = func;
};

DivPop.prototype.setTimeInterval = function(interval) {
    this.m_timeInterval = interval;
};

DivPop.prototype.setCloseImage = function(strImgFile) {
    this.m_closeImg = strImgFile;
};

//Popup method
DivPop.prototype.showPopup = function(wTitle, contentObjs, pos, wWidth) {
    var bWidth = parseInt(document.documentElement.scrollWidth);
    var bHeight = parseInt(document.documentElement.scrollHeight);
    if (this.isIEBrowser) {
        this.hideSelect(true);
    }
    //-------------------- Create background layer---------------------------
    var back = document.createElement("div");
    back.id = DivPop.BGPAGE_ID;
    var styleStr = "top:0px; left:0px; position:absolute; background:#333;\
        width: " + bWidth + "px; height: " + bHeight + "px; _width:100%;_height:200%;_right:0;_bottom:0;_position:absolute;";
    styleStr += this.isIeBrowser ? "filter:alpha(opacity = 0);" : "opacity:0;";
    back.style.cssText = styleStr;
    document.body.appendChild(back);
    this.darkenBackground(back, 50);
 
    //--------------------- Create popup div --------------------------------
    var popWin = document.createElement('div');
    popWin.id = DivPop.POPWIN_ID;
    styleStr = "border:#666 1px solid;background:#338899; left: " +  pos.x + "px; \
        top: " + pos.y + "px;position: absolute; width: " + wWidth + "px;";
    popWin.style.cssText = styleStr;
    
    // popup window top
    var winTop = document.createElement('div');
    winTop.style.cssText = "border-bottom:#aaa 1px solid;margin-left:4px;padding:3px;font-weight:bold;text-align:left;font-size:14px;";
        //Create top table
        var closeImg = HtmHelper.make('img', {'style.cursor':'pointer', 'style.height':'16px', 'style.width':'16px'});
        closeImg.src = this.m_closeImg;
        var me = this;
        if(this.isIEBrowser) {
            closeImg.attachEvent('onclick', function(){me.closeWindow();});
        } else {
            closeImg.addEventListener('click', function(){me.closeWindow();}, false);
        }
    
        var tClose = HtmHelper.make('td', {'width':'20px', 'align':'right'}, closeImg);
        var tTitle = HtmHelper.make('td', {'width':'100%'}, wTitle);
        var thead = HtmHelper.make("thead", {"style.height" : "0px"});
        var tfoot = HtmHelper.make("tfoot", {"style.height" : "0px"});
        var tRow = HtmHelper.make("tr", [tTitle, tClose]);
        var topBar = HtmHelper.make("table", [thead, HtmHelper.make('tbody',null,tRow), tfoot])

    winTop.appendChild(topBar);
    popWin.appendChild(winTop);
    // popup window content
    var winContent = document.createElement('div');
    winContent.style.cssText = "margin:4px;font-size:12px;";
   
    if(contentObjs instanceof Array) {  // If it really is an array
        for(var i = 0; i < contentObjs.length; i++) { // Loop through kids
            var child = contentObjs[i];
            winContent.appendChild(child);
        }
    } else {
        winContent.appendChild(contentObjs);
    }
    popWin.appendChild(winContent);
    //popup window bottom
    //var winBottom = document.createElement('div');
    //winBottom.style.cssText
    document.body.appendChild(popWin);
};

DivPop.prototype.closeWindow = function() {
    clearTimeout(this.m_timerId);
    this.m_timerId = null;
    var popwin = document.getElementById(DivPop.POPWIN_ID);
    if (popwin != null) {
        popwin.parentNode.removeChild(popwin);
    }
   
    if(typeof(this.m_closeCallback) == "function") {
        this.m_closeCallback();
    }
    var tempBackDiv = document.getElementById(DivPop.BGPAGE_ID);
    if (tempBackDiv != null) {
        tempBackDiv.parentNode.removeChild(tempBackDiv);
    }
    if (this.isIEBrowser) {
        hideSelect(false);
    }
};
    
//Darken the background gradually (Global function)
DivPop.prototype.darkenBackground = function(obj, endInt) {
    var me = this;
    if (this.isIEBrowser) {
        var topacity = 0;
        var sopac = obj.style.filter.toString();
        if(sopac) {
            var idx = sopac.indexOf('=');					 
            topacity = parseInt(sopac.substr(idx+1));
        }		
        topacity += 3;
        obj.style.filter = 'alpha(opacity=' + topacity + ')';
        if (topacity < endInt) {
            this.m_timerId = setTimeout(function(){me.darkenBackground(obj, endInt);}, this.m_timeInterval);
        }			
    } else {
        var al = parseFloat(obj.style.opacity);
        al += 0.02;
        obj.style.opacity = al;
        if (al < (endInt / 100)) {
            this.m_timerId = setTimeout(function(){me.darkenBackground(obj, endInt);}, this.m_timeInterval);
        }
    }
}
    

