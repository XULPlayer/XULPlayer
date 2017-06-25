/*emule.js*/

var EMULE_CONST = {
    ADMIN: 0,
    FINFO: 1,
    ED2K: 2,
    DOWNSTATE: 3,
    ISGETFLC: 4,
    FNAME: 5,
    SESSION: 6,
    FILEHASH: 7,
    FCOMMENTS: 8,
    DOWNLOADABLE: 9   
};

var eMule_sessionId = '';

var eMule = {
    URL: "http://127.0.0.1:4711/",
    password: "admin",

//--------------------- Interface functions -------------------------
    getDownload: function () {
	//dumpErr(eMule_sessionId);
	if (!eMule_sessionId) {
	    this.sendLogin();
	    return; //please retry
	}
	this.request({
                ses: eMule_sessionId,
                w: "transfer"
            }, this.parsingEd2k);
    },
    addDownload: function(url) {
        if (url != "") {
	    var retry = 0;
	    while (eMule_sessionId == '' && ++retry < 4) this.sendLogin();
	    dumpErr('session:' + eMule_sessionId + '\n' +
		    'ed2k:' + url);
            this.request({
			 ses: eMule_sessionId,
			 w: 'transfer',
			 ed2k: url}
			 );
        }
    },
    resumeItem: function(ids) {
	this.perform("resume", ids);
    },
    pauseItem: function(ids) {
	this.perform("pause", ids);
    },
    stopItem: function(ids) {
	this.perform("stop", ids);
    },
    removeItem: function(ids) {
	this.perform("cancel", ids);
    },
    cancelItem: function(ids) {
	this.perform("cancel", ids);
    },
    setItemPriority: function(id, prio) {
	if (prio == 'low') p = 'priolow';
	else if (prio == 'normal') p = 'prionormal';
	else if (prio == 'high') p = 'priohigh';
	else p = 'prioauto';

        this.request({
	    ses: eMule_sessionId,
	    w: 'transfer',
	    op: p,
	    file:id
	});
    },
//--------------------- Helper functions -------------------------
    postRequest: function(params, callback) {
	XMLHttp.sendRequest(
	    this.URL,
	    'post',
	    params,
	    {},
	    callback ? callback: this.empty,
	    true
	);
    },
    request: function(params, callback) {
	XMLHttp.sendRequest(
	    this.URL,
	    'get',
	    params,
	    {},
	    callback ? callback: this.empty,
	    true
	);
    },
    request2: function(path, callback) {
	var url = this.URL + "?" + path;
	dumpErr(url);
	XMLHttp.sendRequest(
	    url,
	    'get',
	    {},
	    {},
	    callback ? callback: this.empty,
	    true
	);
    },
    empty: function() {
    },
    perform: function(action, ids) {
        if (!ids) {
            return;
        }
	if (typeof(ids) == "string") {
	    ids = [ids];
	}
	if (ids.length == 0) return;
        if (action.toLowerCase() == 'cancel') {
	    //clear and refresh
	    Tree._downloads = [];	    
        }
	this.request2("ses=" + eMule_sessionId + "&w=transfer&op=" + action + "&file=" + ids.join("&file="));
	
        this.getDownload();
    },
    sendLogin: function() {
	this.postRequest(
			 {
			    p: this.password,
			    w: 'password'
			 },
			 this.getSessionId);
    },
    getSessionId: function(objXMLHttp) {
	if (objXMLHttp.responseText == "") {
	    //try to start uTorrent
	    if (typeof(MDialog) == 'undefined') return;
	    MDialog._downloader.start_eMule();
	    return;
	}
	var TOKEN = 'location.href = "/?ses=';
	var pos1 = objXMLHttp.responseText.indexOf(TOKEN);
	if (pos1 < 0) return;
	pos1 = pos1 + TOKEN.length;
	var pos2 = objXMLHttp.responseText.indexOf('&', pos1);
	if (pos2 < 0) return;
	eMule_sessionId = objXMLHttp.responseText.substr(pos1, pos2 - pos1);
	//dumpErr(eMule_sessionId);
    },
    /*getDownload call back*/
    parsingEd2k: function(objXMLHttp) {
	//dumpErr("Response:\n" + transport.responseText);
	if (objXMLHttp.responseText == "") {
	    //try to start uTorrent
	    MDialog._downloader.start_eMule();
	    eMule_sessionId = '';
	    //this.sendLogin();
	    return;
	}
	else if (objXMLHttp.responseText.indexOf('<form action="" method="POST" name="login">') >= 0) {
	    eMule_sessionId = '';
	    //this.sendLogin();
	    return;
	}
	function _getState(strState)
	{
	    if (strState == 'complete') return COMPLETE;
	    else if (strState == 'downloading') return RUNNING;
	    else if (strState == 'paused') return PAUSED;
	    return RUNNING; /*FIXME*/
	}
	function _getNumber(strNum)
	{
	    if (typeof(strNum) != 'string') return 0;
	    if (strNum == '-') return 0;
	    var pos = strNum.indexOf(' ');
	    if (pos < 0) return parseFloat(strNum);
	    var num = strNum.substr(0, pos);
	    pos = pos + 1;
	    var strUnit = strNum.substr(pos, strNum.length - pos)
	    var unit = 1;
	    if (strUnit == 'GB') unit = 1024 * 1024 * 1024;
	    else if (strUnit == 'MB') unit = 1024 * 1024;
	    else if (strUnit == 'KB') unit = 1024;
	    return parseFloat(num) * unit;
	}
	function _getNumOfSrc(strNum)
	{
	    if (typeof(strNum) != 'string') return 0;
	    var pos = strNum.indexOf('&nbsp;/&nbsp;');
	    if (pos < 0) return [parseInt(strNum), 0];
	    var src = strNum.substr(0, pos);
	    pos = pos + '&nbsp;/&nbsp;';
	    var pos1 = strNum.indexOf('(', pos);
	    if (pos1 < 0) pos1 = strNum.length;
	    var client = strNum.substr(pos, pos1 - pos);
	    return [parseInt(src), parseInt(client)];
	}
	var TOKEN = 'onMouseover="downmenu(event,';
	var pos1 = objXMLHttp.responseText.indexOf(TOKEN);
        var infos = [];
	while (pos1 >= 0) {
	    pos1 = pos1 + TOKEN.length;
	    var TOKEN_TEMP = ')" onMouseout="delayhidemenu()"';
	    pos2 = objXMLHttp.responseText.indexOf(TOKEN_TEMP, pos1);
	    if (pos2 < 0) {
		pos1 = objXMLHttp.responseText.indexOf(TOKEN, pos1);
		continue;
	    }
	    var item = objXMLHttp.responseText.substr(pos1, pos2 - pos1);
	    //dumpErr(item);
	    var d = item.split("','");
/*	    for (i = 0; i < d.length; i++) {
		dumpErr(i + ':' + d[i]);
	    }*/
	    /*file Size*/
	    pos2 = pos2 + TOKEN_TEMP.length;
	    TOKEN_TEMP = ' nowrap style="font-size: 7.5pt">';
	    pos1 = objXMLHttp.responseText.indexOf(TOKEN_TEMP, pos2);
	    if (pos1 >= 0) {
		pos1 = pos1 + TOKEN_TEMP.length;
		pos2 = objXMLHttp.responseText.indexOf('</td>', pos1);
		strSize = objXMLHttp.responseText.substr(pos1, pos2 - pos1);
	    }
	    else strSize = '0';
	    /*downloaded*/
	    pos2 = pos2 + '</td>'.length;
	    TOKEN_TEMP = ' nowrap>';
	    pos1 = objXMLHttp.responseText.indexOf(TOKEN_TEMP, pos2);
	    if (pos1 >= 0) {
		pos1 = pos1 + TOKEN_TEMP.length;
		pos2 = objXMLHttp.responseText.indexOf('</td>', pos1);
		strDownloaded = objXMLHttp.responseText.substr(pos1, pos2 - pos1);		
	    }
	    else strDownloaded = '0';
	    /*speed*/
	    pos2 = pos2 + '</td>'.length;
	    TOKEN_TEMP = ' nowrap>';
	    pos1 = objXMLHttp.responseText.indexOf(TOKEN_TEMP, pos2);
	    if (pos1 >= 0) {
		pos1 = pos1 + TOKEN_TEMP.length;
		pos2 = objXMLHttp.responseText.indexOf('</td>', pos1);
		strSpeed = objXMLHttp.responseText.substr(pos1, pos2 - pos1);		
	    }
	    else strSpeed = '0';
	    /*numofSrc*/
	    pos2 = pos2 + '</td>'.length;
	    TOKEN_TEMP = ' nowrap style="font-size: 7.5pt">';
	    pos1 = objXMLHttp.responseText.indexOf(TOKEN_TEMP, pos2);
	    if (pos1 >= 0) {
		pos1 = pos1 + TOKEN_TEMP.length;
		pos2 = objXMLHttp.responseText.indexOf('</td>', pos1);
		numOfSrc = objXMLHttp.responseText.substr(pos1, pos2 - pos1);
	    }
	    else numOfSrc = '0';
	    srcclient = _getNumOfSrc(numOfSrc);
	    /*link*/
	    var fileInfo = d[EMULE_CONST.FINFO];
	    TOKEN_TEMP = 'ed2k://';
	    _pos1 = fileInfo.indexOf(TOKEN_TEMP) + TOKEN_TEMP.length;
	    _pos2 = fileInfo.indexOf("'", _pos1);
	    link = fileInfo.substr(_pos1, _pos2 - _pos1);
/*	    
	    dumpErr('id:' + d[EMULE_CONST.FILEHASH] + '\n' +
		    'name:' + d[EMULE_CONST.FNAME] + '\n' +
		    'link:' + link + '\n' +
		    'size:' + (strSize) + '\n' +
		    'downloaded:' + (strDownloaded) + '\n' +
		    'numOfSrc:' + (numOfSrc) + '\n' +
		    'speed:' + (strSpeed) + '\n' +
		    'state:' + d[EMULE_CONST.DOWNSTATE]);
	    dumpErr('id:' + d[EMULE_CONST.FILEHASH] + '\n' +
		    'name:' + d[EMULE_CONST.FNAME] + '\n' +
		    'link:' + link + '\n' +
		    'size:' + _getNumber(strSize) + '\n' +
		    'downloaded:' + _getNumber(strDownloaded) + '\n' +
		    'numOfSrc:' + srcclient[0] + '\n' +
		    'numOfClient:' + srcclient[1] + '\n' +
		    'speed:' + _getNumber(strSpeed) + '\n' +
		    'state:' + _getState(d[EMULE_CONST.DOWNSTATE]));
*/
	    var info = {
		id: d[EMULE_CONST.FILEHASH],
		name: d[EMULE_CONST.FNAME],
		size: _getNumber(strSize),
		format: "Unknown",
		link: link,
		downloaded: _getNumber(strDownloaded),
		numOfSrc: srcclient[0],
		numOfClient: srcclient[1],
		speed: (strSpeed)*1024,
		state: _getState(d[EMULE_CONST.DOWNSTATE]),
		priority: 0,
		fileAge: 0 //FIXME
	    };
	    infos.push(info);

	    pos1 = objXMLHttp.responseText.indexOf(TOKEN, pos2);
	}
	MDialog.updateDownloads(infos, eMule);
    },
    showError: function() {
        alert("error!");
    }
};