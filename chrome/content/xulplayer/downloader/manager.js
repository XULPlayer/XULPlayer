const REFRESH_FREQ = 1500;

const TOOLTIP_FREQ = 500;
const SPEED_COUNT = 60;
const SPEED_NUMAVG = 10;

var MDialog = {
    _initialized: false,
    _downloader: null,

    init: function D_init() {
        Tree.init($("downloads"));
        this.start();
    },
    openAdd: function D_openAdd() {
        if (!this._initialized) {
            this.init();
        }
        window.openDialog('chrome://xulplayer/content/downloader/addurl.xul', 'Add URL', 'chrome, centerscreen, Dialog=no, dependent=yes', window.arguments[0]);
    },
    start: function D_start() {
        if ("arguments" in window && typeof(window.arguments[0]) == "object") {
            this._downloader = window.arguments[0];
	    if (window.arguments.length > 1) {
		for (i = 1;  i < window.arguments.length; i++) {
			if (window.arguments[i]) this.addDownload(window.arguments[i]);
		}
	    }
        }
        else {
	    this._downloader = downloader;
	    if (window.arguments.length > 0) {
		for (i = 0;  i < window.arguments.length; i++) {
			if (window.arguments[i]) this.addDownload(window.arguments[i]);
		}
	    }
        }
        this._initialized = true;

        //this._updTimer = new Timer("MDialog.checkDownloads();", REFRESH_FREQ, true, true);
        this._updTimer = new Timer("MDialog.refresh();", REFRESH_FREQ, true, true);
        if ($('loadingbox')) {
            $('loadingbox').parentNode.removeChild($('loadingbox'));
        }
    },

    refresh: function D_refresh() {
        try {
	    eMule.getDownload();
	    uTorrent.getDownload();
        }
        catch(ex) {
            dumpErr("refresh():" + ex);
        }
    },

    addDownload: function D_addDownload(url) {
//        try {
	    if (url.substring(0, 4).toLowerCase() == 'ed2k') {
		eMule.addDownload(url);
	    }
	    else {
		uTorrent.addDownload(url);
	    }
//        }
//        catch(ex) {
//            dumpErr("addDownload():" + ex);
//        }
    },

    updateDownloads: function D_updateDownloads(infos, p2pObj) {
        for (i = 0; i < infos.length; i++) {

            items = Tree._downloads.filter(function(t) {
                return t.id == infos[i]["id"];
            });
            if (items.length > 0) {
                let d = items[0]
                d.name = infos[i]["name"];
                d.downloaded = infos[i]["downloaded"];
                d.numOfSrc = infos[i]["numOfSrc"];
                d.numOfClient = infos[i]["numOfClient"];
                d.speed = infos[i]["speed"];
                d.state = infos[i]["state"];
                d.priority = infos[i]["priority"];
                d.fileAge = infos[i]["fileAge"];
                // Calculate estimated time
                let strStatus = ["running", "paused", "completed", "shared",
				 "canceled", "new", "aborted", "queued"];
                if (d.size > 0) {
                    if (d.state == RUNNING) {
                        let remaining = Math.ceil((d.size - d.downloaded) / d.speed);
                        if (!isFinite(remaining)) {
                            d.status = _("unknown");
                        }
                        else {
                            d.status = formatTimeDelta(remaining);
                        }
                    } else d.status = _(strStatus[d.state]);
                }
                else {
                    d.status = _("unknown");
                }
                //d.dumpItem();
            }
            else {
                let item = new QueueItem(infos[i]["id"], infos[i]["name"],
					 infos[i]["size"], infos[i]["format"], infos[i]["link"],
					 p2pObj);
                Tree.add(item);
            }
        }
        //dumpErr("updateDownloads():" + Tree._downloads.length);
    },
    close: function D_close() {
        if (!this._initialized) {
            return true;
        }

        // stop everything!
        // enumerate everything we'll have to wait for!
        if (this._updTimer) {
            this._updTimer.kill();
            delete this._updTimer;
        }
        close();
        return true;
    },
    unload: function D_unload() {
        TimerManager.killAll();
        return true;
    }
};
