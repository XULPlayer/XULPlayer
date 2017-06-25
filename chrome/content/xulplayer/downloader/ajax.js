var XMLHttp = {
    _objPool: [],
 
    _getInstance: function ()
    {
        for (var i = 0; i < this._objPool.length; i++)
        {
            if (this._objPool[i].readyState == 0 || this._objPool[i].readyState == 4)
            {
                return this._objPool[i];
            }
        }
 
        // IE5 does not support push
        this._objPool[this._objPool.length] = this._createObj();
 
        return this._objPool[this._objPool.length - 1];
    },
 
    _createObj: function ()
    {
        if (window.XMLHttpRequest)
        {
            var objXMLHttp = new XMLHttpRequest();
 
        }
        else
        {
            var MSXML = ['MSXML2.XMLHTTP.5.0', 'MSXML2.XMLHTTP.4.0', 'MSXML2.XMLHTTP.3.0', 'MSXML2.XMLHTTP', 'Microsoft.XMLHTTP'];
            for(var n = 0; n < MSXML.length; n ++)
            {
                try
                {
                    var objXMLHttp = new ActiveXObject(MSXML[n]);
                    break;
                }
                catch(e)
                {
                }
            }
         }         
 
        if (objXMLHttp.readyState == null)
        {
            objXMLHttp.readyState = 0;
 
            objXMLHttp.addEventListener("load", function ()
                {
                    objXMLHttp.readyState = 4;
 
                    if (typeof objXMLHttp.onreadystatechange == "function")
                    {
                        objXMLHttp.onreadystatechange();
                    }
                }, false);
        }
 
        return objXMLHttp;
    },
 
    sendRequest: function (url, method, parameters, requestHeaders, callback, asynchronous)
    {
	if(asynchronous == null) asynchronous = true;
        var objXMLHttp = this._getInstance();
	//dumpErr("sendRequest:" + url);
	params = [];

	if (parameters != null) {
	    for (var parameter in parameters) {
		params.push(parameter + '=' + parameters[parameter]);
	    }
	}

	try
	{
	    if (method == 'get' && parameters != null) {
		if (url.indexOf("?") < 0) {
		    url = url + "?";
		}
		else {
		    url = url + "&";
		}
		url = url + params.join('&');
	    }
	    else {
		method = 'post';
	    }
	    //dumpErr(url);
	    objXMLHttp.open(method, url, asynchronous);

	    for (var header in requestHeaders) {
		objXMLHttp.setRequestHeader(header, requestHeaders[header]);
	    }

	    body = (method == 'post' && parameters != null) ? params.join('&'): null;
	    objXMLHttp.send(body);

	    objXMLHttp.onreadystatechange = function ()
	    {
		if (objXMLHttp.readyState == 4) {
		    if (typeof(callback) == "function") callback(objXMLHttp);
		}
	    }
	}
	catch(e)
	{
	    dumpErr(e);
	}
    },

    formSubmit : function(form , url , func) {
    	if(typeof form != 'object'){
    		form = document.getElementById(form);
    	}
    	var ele = form.elements;
    	var post = new Array();
 
    	for(var i = 0 ; i<ele.length ;   i++ ){
    		post[i] = ele[i].name + "=" + ele[i].value;
    	}
    	var data = post.join('&');
    	//alert(data);
    	this.sendReq('post' , url , data , func , false);
    }
};
