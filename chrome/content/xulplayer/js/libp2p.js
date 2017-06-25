/*
 libp2p.js
*/

const RUNNING	= 0;
const PAUSED	= 1;
const COMPLETE	= 2;
const SHARED	= 3;
const CANCELED	= 4;
const NEW	= 5;
const ABORTED	= 6;
const QUEUED	= 7;
const FINISHING	= 8; //FIXME: mlnet has no this state

const MLNET_IDLE	= 0;
const MLNET_STARTED	= 1;
const MLNET_CONNECTED	= 2;
const MLNET_STOPPING	= 3;

function CDownloader(pluginObj) {
    this._plugin = pluginObj;
    this._eMuleSessionId ='';
}

CDownloader.prototype = {
    start_Mlnet: function DLR_start(){
        return this._plugin.P2P_start_Mlnet();
    },
    stop_Mlnet: function DLR_stop() {
	this._plugin.P2P_stop_Mlnet();
    },
    isOn_Mlnet: function () {
	return this._plugin.P2P_getState() >= MLNET_STARTED;
    },
    state_Mlnet: function() {
        return this._plugin.P2P_getState();
    },

    start_uTorrent: function() {
	return this._plugin.P2P_start_uTorrent();
    },
    stop_uTorrent: function() {
	this._plugin.P2P_stop_uTorrent();
    },

    start_eMule: function() {
	return this._plugin.P2P_start_eMule();
    },
    stop_eMule: function() {
	this._plugin.P2P_stop_eMule();
    },

    addDownload_Mlnet: function DLR_addDownload(url) {
        return this._plugin.P2P_addDownload(url);
    },
    getDownloadings_Mlnet: function DLR_getDownloadings() {
	return this._plugin.P2P_getDownloadings();
    },
    pollDownloadings_Mlnet: function DLR_pollDownloadings() {
	return this._plugin.P2P_pollDownloadings();
    },
    getDownloadeds_Mlnet: function DLR_getDownloadeds() {
        return this._plugin.P2P_getDownloadeds();
    },
    pollDownloadeds_Mlnet: function DLR_pollDownloadeds() {
        return this._plugin.P2P_pollDownloadeds();	
    },
// functon for download items
    removeItem_Mlnet: function DLR_removeItem(id) {
	this._plugin.P2P_removeDownload(parseInt(id));
    },
    pauseItem_Mlnet: function DLR_pauseItem(id) {
	this._plugin.P2P_pauseDownload(parseInt(id));
	this._plugin.P2P_getDownloadings();
    },
    resumeItem_Mlnet: function DLR_resumeItem(id) {
	this._plugin.P2P_resumeDownload(parseInt(id));
	this._plugin.P2P_getDownloadings();
    },
// helper function
    eMuleSaveSessionId: function(sid) {
	this._eMuleSessionId = sid;
    },
    eMuleGetSessionId: function(sid) {
	return this._eMuleSessionId;
    }
};
