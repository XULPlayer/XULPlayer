var CONST = {
    DLG_TORRENTPROP_00: 102,
    DLG_TORRENTPROP_01: 103,
    DLG_TORRENTPROP_02: 104,
    DLG_TORRENTPROP_1_GEN_01: 105,
    DLG_TORRENTPROP_1_GEN_03: 106,
    DLG_TORRENTPROP_1_GEN_04: 107,
    DLG_TORRENTPROP_1_GEN_06: 108,
    DLG_TORRENTPROP_1_GEN_08: 109,
    DLG_TORRENTPROP_1_GEN_10: 110,
    DLG_TORRENTPROP_1_GEN_11: 111,
    DLG_TORRENTPROP_1_GEN_12: 112,
    DLG_TORRENTPROP_1_GEN_14: 113,
    DLG_TORRENTPROP_1_GEN_16: 114,
    DLG_TORRENTPROP_1_GEN_17: 115,
    DLG_TORRENTPROP_1_GEN_18: 116,
    DLG_TORRENTPROP_1_GEN_19: 117,
    FI_COL_DONE: 118,
    FI_COL_NAME: 119,
    FI_COL_PCT: 120,
    FI_COL_PRIO: 121,
    FI_COL_SIZE: 122,
    FI_PRI0: 123,
    FI_PRI1: 124,
    FI_PRI2: 125,
    FI_PRI3: 126,
    GN_TP_01: 127,
    GN_TP_02: 128,
    GN_TP_03: 129,
    GN_TP_04: 130,
    GN_TP_05: 131,
    GN_TP_06: 132,
    GN_TP_07: 133,
    GN_TP_08: 134,
    GN_TRANSFER: 135,
    GN_XCONN: 136,
    MAIN_TITLEBAR_SPEED: 137,
    MENU_SHOW_CATEGORY: 138,
    MENU_SHOW_DETAIL: 139,
    ML_DELETE_DATA: 140,
    ML_FORCE_RECHECK: 141,
    ML_FORCE_START: 142,
    ML_LABEL: 143,
    ML_PAUSE: 144,
    ML_PROPERTIES: 145,
    ML_REMOVE: 146,
    ML_REMOVE_AND: 147,
    ML_START: 148,
    ML_STOP: 149,
    OV_CAT_ACTIVE: 150,
    OV_CAT_ALL: 151,
    OV_CAT_COMPL: 152,
    OV_CAT_DL: 153,
    OV_CAT_INACTIVE: 154,
    OV_CAT_NOLABEL: 155,
    OV_COL_AVAIL: 156,
    OV_COL_DONE: 157,
    OV_COL_DOWNLOADED: 158,
    OV_COL_DOWNSPD: 159,
    OV_COL_ETA: 160,
    OV_COL_LABEL: 161,
    OV_COL_NAME: 162,
    OV_COL_ORDER: 163,
    OV_COL_PEERS: 164,
    OV_COL_REMAINING: 165,
    OV_COL_SEEDS: 166,
    OV_COL_SEEEDS_PEERS: 167,
    OV_COL_SHARED: 168,
    OV_COL_SIZE: 169,
    OV_COL_STATUS: 170,
    OV_COL_UPPED: 171,
    OV_COL_UPSPD: 172,
    OV_CONFIRM_DELETEDATA_MULTIPLE: 173,
    OV_CONFIRM_DELETEDATA_ONE: 174,
    OV_CONFIRM_DELETE_MULTIPLE: 175,
    OV_CONFIRM_DELETE_ONE: 176,
    OV_FL_CHECKED: 177,
    OV_FL_DOWNLOADING: 178,
    OV_FL_ERROR: 179,
    OV_FL_FINISHED: 180,
    OV_FL_PAUSED: 181,
    OV_FL_QUEUED: 182,
    OV_FL_SEEDING: 183,
    OV_FL_STOPPED: 184,
    OV_NEWLABEL_CAPTION: 185,
    OV_NEWLABEL_TEXT: 186,
    OV_NEW_LABEL: 187,
    OV_REMOVE_LABEL: 188,
    OV_TABS: 189,
    OV_TB_ADDTORR: 190,
    OV_TB_PAUSE: 191,
    OV_TB_PREF: 192,
    OV_TB_REMOVE: 193,
    OV_TB_START: 194,
    OV_TB_STOP: 195,
    SIZE_GB: 196,
    SIZE_KB: 197,
    SIZE_MB: 198,
    ST_CAPT_ADVANCED: 199,
    ST_CAPT_BANDWIDTH: 200,
    ST_CAPT_CONNECTION: 201,
    ST_CAPT_DISK_CACHE: 202,
    ST_CAPT_FOLDER: 203,
    ST_CAPT_GENERAL: 204,
    ST_CAPT_SCHEDULER: 205,
    ST_CAPT_SEEDING: 206,
    ST_CAPT_TRANSFER: 207,
    ST_CAPT_WEBUI: 208,
    ST_CBO_ENCRYPTIONS: 209,
    ST_CBO_PROXY: 210,
    ST_COL_NAME: 211,
    ST_COL_VALUE: 212,
    ST_SEEDTIMES_HOURS: 213,
    ST_SEEDTIMES_IGNORE: 214,
    ST_SEEDTIMES_MINUTES: 215,
    TIME_DAYS_HOURS: 216,
    TIME_HOURS_MINS: 217,
    TIME_MINS_SECS: 218,
    TIME_SECS: 219,
    TIME_WEEKS_DAYS: 220,
    TIME_YEARS_WEEKS: 221,
    TORRENT_HASH: 0,
    TORRENT_STATUS: 1,
    TORRENT_NAME: 2,
    TORRENT_SIZE: 3,
    TORRENT_PROGRESS: 4,
    TORRENT_DOWNLOADED: 5,
    TORRENT_UPLOADED: 6,
    TORRENT_RATIO: 7,
    TORRENT_UPSPEED: 8,
    TORRENT_DOWNSPEED: 9,
    TORRENT_ETA: 10,
    TORRENT_LABEL: 11,
    TORRENT_PEERS_CONNECTED: 12,
    TORRENT_PEERS_SWARM: 13,
    TORRENT_SEEDS_CONNECTED: 14,
    TORRENT_SEEDS_SWARM: 15,
    TORRENT_AVAILABILITY: 16,
    TORRENT_QUEUE_POSITION: 17,
    TORRENT_REMAINING: 18,
    FILEPRIORITY_SKIP: 0,
    FILEPRIORITY_LOW: 1,
    FILEPRIORITY_NORMAL: 2,
    FILEPRIORITY_HIGH: 3,
    STATE_STARTED: 1,
    STATE_CHECKING: 2,
    STATE_ERROR: 16,
    STATE_PAUSED: 32,
    STATE_QUEUED: 64
};

var uTorrent = {
    URL: "http://127.0.0.1:8888/gui/",
    username: "admin",
    password: "admin",
    on: false,
    torrents: [],

//--------------------- Interface functions -------------------------
    getDownload: function () {
	this.request({
                token: "tmD80kaCUMBfgH8NwHcjQZrozXqr-tu-O53nLGtOtldWsYYr2nlfS_l-4Ek=",
                list: 1,
                cid: "60472545"
            }, this.parsingTorrent);
    },
    addDownload: function(url) {
        url = escape(url);
        if (url != "") {
            /*TODO: cookie
	     var cookie = "";
            if (cookie != "") {
                url += ":COOKIE:" + cookie
            }
	    */
	    //dumpErr("addDownload():\n download url:\n" + url);
            this.request2("action=add-url&s=" + url);
        }
    },
    resumeItem: function(ids) {
	this.perform("start", ids);
    },
    pauseItem: function(ids) {
	this.perform("pause", ids);
    },
    stopItem: function(ids) {
	this.perform("stop", ids);
    },
    removeItem: function(ids) {
	this.perform("remove", ids);
    },
    cancelItem: function(ids) {
	this.perform("remove", ids);
    },
    setItemPriority: function(id, prio) {
        this.request({
	    action:"setprio",
	    hash:id,
	    p:prio
	});
    },
//--------------------- Helper functions -------------------------
    request: function(params, callback) {
	XMLHttp.sendRequest(
	    this.URL,
	    'get',
	    params,
	    {
                'Authorization': "Basic " + this.getAuthenticationString(),
                'X-Request': "JSON"
            },
	    callback?callback: this.empty,
	    true
	);
    },
    request2: function(path, callback) {
	var url = this.URL + "?" + path;
	XMLHttp.sendRequest(
	    url,
	    'get',
	    null,
	    {
                'Authorization': "Basic " + this.getAuthenticationString(),
                'X-Request': "JSON"
            },
	    callback?callback: this.empty,
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
        if (action.toLowerCase() == 'remove') {
	    //clear and refresh
	    Tree._downloads = [];	    
        }
	this.request2("action=" + action + "&hash=" + ids.join("&hash="));
	
        this.getDownload();
    },
    getState: function(C, A) {
        var B = -1;
        if (C & CONST.STATE_STARTED) {
            if (C & CONST.STATE_PAUSED) {
                B = PAUSED;
            } else
	    {
                B = (A == 1000) ? SHARED: RUNNING;
            }
        } else {
            if (C & CONST.STATE_CHECKING) {
                B = (C & CONST.STATE_PAUSED) ? PAUSED: RUNNING; //FIX: Checking
            } else
	    {
                if (C & CONST.STATE_ERROR) {
                    B = ABORTED;
                } else {
                    if (C & CONST.STATE_QUEUED) {
                        //B = (A == 1000) ? "Status_Queued_Up": "Status_Queued_Down";
			B = QUEUED;
                    }
                }
            }
        }
        if ((A == 1000) && (B == -1)) {
            B = COMPLETE;
        } else {
            if ((A < 1000) && (B == -1)) {
                B = CANCELED;
            }
        }
	//dumpErr(B);
        return B;
    },
    getAuthenticationString: function() {
	return Base64.encode(this.username + ":" + this.password);
    },
    /*getDownload call back*/
    parsingTorrent: function(objXMLHttp) {
	//dumpErr("Response:\n" + objXMLHttp.responseText);

	if (objXMLHttp.responseText == "") {
	    //try to start uTorrent
	    MDialog._downloader.start_uTorrent();
	    return;
	}
	this.on = true;

	var nativeJSON = Components.classes["@mozilla.org/dom/json;1"]
                 .createInstance(Components.interfaces.nsIJSON);

        var resp =  nativeJSON.decode(objXMLHttp.responseText);

        if (!has(resp, "torrents")) {
            this.torrents = resp.torrentp;
        } else {
            this.torrents = resp.torrents;
        }
        var infos = [];
	for (var i = 0, N = this.torrents.length; i < N; i++) {
	    var torrent = this.torrents[i];
	    /*dumpErr("hash:" + torrent[CONST.TORRENT_HASH] + "\n" +
		    "name:" + torrent[CONST.TORRENT_NAME] + "\n" +
		    "downloaded:" + torrent[CONST.TORRENT_DOWNLOADED] + "\n" +
		    "numOfSrc:" + torrent[CONST.TORRENT_SEEDS_CONNECTED] + "\n" +
		    "numOfClient:" + torrent[CONST.TORRENT_PEERS_CONNECTED] + "\n" +
		    "speed:" + torrent[CONST.TORRENT_DOWNSPEED] + "\n" +
		    "state:" + torrent[CONST.TORRENT_STATUS] + "\n" +
		    "Progress:" + torrent[CONST.TORRENT_PROGRESS]);*/
	    var info = {
		id: torrent[CONST.TORRENT_HASH],
		name: torrent[CONST.TORRENT_NAME],
		size: torrent[CONST.TORRENT_SIZE],
		format: "Unknown",
		link: "",
		downloaded: torrent[CONST.TORRENT_DOWNLOADED],
		numOfSrc: torrent[CONST.TORRENT_SEEDS_CONNECTED],
		numOfClient: torrent[CONST.TORRENT_PEERS_CONNECTED],
		speed: torrent[CONST.TORRENT_DOWNSPEED],
		state: uTorrent.getState(torrent[CONST.TORRENT_STATUS],
					 torrent[CONST.TORRENT_PROGRESS]), //TIP: can not call this.foo()
		priority: 0,
		fileAge: 0 //FIXME
	    };
	    infos.push(info);
	}
	MDialog.updateDownloads(infos, uTorrent);
    },
    showError: function() {
        alert("error!");
    },
};