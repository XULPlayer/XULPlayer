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
DivPop.prototype.showPopup = function(popDiv) {
    if(!popDiv) return;
    var bWidth = 0, bHeight = 0;
    
    if (this.isIEBrowser) {
        this.hideSelect(true);
        bWidth = parseInt(document.body.scrollWidth);
        bHeight = parseInt(document.body.scrollHeight);
    } else {
        bWidth = parseInt(document.documentElement.scrollWidth);
        bHeight = parseInt(document.documentElement.scrollHeight);
    }
  
    //-------------------- Create background layer---------------------------
    var back = document.createElement("div");
    back.id = DivPop.BGPAGE_ID;
    var styleStr = "top:0px; left:0px; position:absolute; background:#333;\
        width: " + bWidth + "px; height: " + bHeight + "px; _right:0;_bottom:0;_position:absolute;";
    styleStr += this.isIEBrowser ? "filter:alpha(opacity = 0);" : "opacity:0;";
    back.style.cssText = styleStr;
    document.body.insertBefore(back, popDiv);
    // document.body.appendChild(back);
    if (this.isIEBrowser) {
        var me = this;
		window.onresize =  function(){me.resizeBackground(DivPop.BGPAGE_ID); };
	}
    this.darkenBackground(back, 50);
};

DivPop.prototype.resizeBackground = function(id)
{
	var mback = document.getElementById(id);
	mback.style.width = parseInt(document.body.scrollWidth);
	mback.style.height = parseInt(document.body.scrollHeight);
}

DivPop.prototype.closeWindow = function() {
    clearTimeout(this.m_timerId);
    this.m_timerId = null;
    
    if(typeof(this.m_closeCallback) == "function") {
        this.m_closeCallback();
    }
    var tempBackDiv = document.getElementById(DivPop.BGPAGE_ID);
    if (tempBackDiv != null) {
        tempBackDiv.parentNode.removeChild(tempBackDiv);
    }
    if (this.isIEBrowser) {
        this.hideSelect(false);
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
    
/*show round corner*/
DivPop.prototype.madeRoundBox = function(userDiv)
{
	  //-------------------- Create roundbox layer---------------------------
    var rbox = document.createElement("div");
	rbox.className = "divContainer";
	var rbuppers = document.createElement("div");
	rbuppers.className = "uppers";
	rbuppers = rbox.appendChild(rbuppers);
		
	var rbtop = document.createElement("b");
	rbtop.className = "rctop";
	rbtop = rbox.appendChild(rbtop);
	var rbcontent = document.createElement("div");
	rbcontent.className = "opacityBG";
	rbcontent = rbox.appendChild(rbcontent);
	var rbbottom = document.createElement("b");
	rbbottom.className = "rcbottom";
	rbbottom = rbox.appendChild(rbbottom);
	for(i = 1; i <= 4; i++)
	{
		var rcb = document.createElement("b");
		rcb.className = "rcb" + i.toString();
		rbtop.appendChild(rcb);
		rcb = document.createElement("b");
		rcb.className = "rcb" + (5 - i).toString();
		rbbottom.appendChild(rcb);
	}
	if(userDiv != null)
	{
		rbuppers.appendChild(userDiv);
		//here is the entry point
		var empty = document.createElement("div");
		empty.className = "empty";
		empty = rbuppers.appendChild(empty); 
		
		rbox.style.top = userDiv.style.top;
		rbox.style.left = userDiv.style.left;
		rbox.style.width = userDiv.style.width;
		rbcontent.style.height = (parseInt(userDiv.style.height) - 10) + 'px';
	}
	return rbox;
}
DivPop.prototype.showDiv = function(div)
{
	div.style.display = '';
}

DivPop.prototype.hideDiv = function(div)
{
	div.style.display = 'none';
}

DivPop.prototype.showPercentDiv = function(div, nfloat)
{
	div.innerHTML = "";
	var str = nfloat.toFixed(2);
	str = str.toString();	
	var number = document.createElement("div");
	number.className = "numTextR";
	number.innerHTML = '%';
	div.appendChild(number);
	for(var i = str.length; i > 0; i--)
	{
		number = document.createElement("div");
		number.className = "numTextR";
		number.innerHTML = str.substring(i - 1, i);
		if(number.innerHTML == '.')
		{
			number.style.width = '5px';
		}
		div.appendChild(number);
	}
}

DivPop.prototype.showTimeDiv = function(div, sec)
{
	div.innerHTML = "";
	var str = '--:--:--';
	var unit1 = 60;
	var unit2 = 60 * unit1;
	var unit3 = 60 * unit2;
	if(sec < unit3)
	{
		var num1, num2, num3;
		num1 = parseInt(sec % unit1);
		num2 = sec % unit2; num2 = parseInt((num2 - num1) / unit1);
		num3 = parseInt(sec / unit2); 
		str = '';
		if(num3 < 10)
		{
			str = '0'
		}
		str += num3.toString() + ':'; 
		if(num2 < 10)
		{
			str += '0'
		}
		str += num2.toString() + ':'; 
		if(num1 < 10)
		{
			str += '0'
		}
		str += num1.toString(); 		
	}
	for(var i = 0;i < str.length; i++ )
	{		
		var number = document.createElement("div");
		number.className = "numTextL";
		number.innerHTML = str.substring(i, i + 1);
		if(number.innerHTML == ':')
		{
			number.style.width = '6px';
		}
		div.appendChild(number);
	}
	 
}