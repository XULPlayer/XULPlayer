
var Dialog = {
    _downloader:null,

    init: function AD_init() {
	if ("arguments" in window) {
	    this._downloader = window.arguments[0];
	}
    },
    
    download: function AD_download(startAtOnce) {
        //TODO:
        let url = $("address").value;

        try {
            MDialog.addDownload(url);
        }
        catch(ex) {
            dumpErr("addDownload():" + ex);
        }
        self.close();
        return true;
    }
};