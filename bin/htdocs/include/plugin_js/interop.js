/* Interoperation of MediaCoder and Plugin for online transcoding.
   Provide unique interface for MediaCoder and Plugin transcodeing, used in
   www.mcex.org.
   Created by Zoominla, 12-27-2008         
*/
   
if(typeof(TRANSCODE_INTEROP_INCLUDE) != "boolean") {
	TRANSCODE_INTEROP_INCLUDE = true;
    
    function Interop(isMediaCoder)
	{
		//Indicate using Mediacoder or plugin to do transcoding.
		this.isMc = isMediaCoder;
	}
	
	Interop.prototype.addFile = function () {
		if(this.isMc) {
			callRPC('ui.list.addFile');
		} else {
			addMediaFile();
		}
	};
	Interop.prototype.addFolder = function () {
		if(this.isMc) {
			callRPC('ui.list.addDir');
		} else {
			addFolder();
		}
	};
	
	Interop.prototype.play = function() {
		if(this.isMc) {
			callRPC('ui.player.play'); 
		} else {
			this.showPluginDiv();
			startPlay();
		}
	};
	
	Interop.prototype.pause = function() {
		if(this.isMc) {
			callRPC('ui.player.pause'); 
		} else {
			mp.pause();
		}
	};
	
	Interop.prototype.stop = function() {
		if(this.isMc) {
			callRPC('ui.player.pause'); 
		} else {
			stopCmd();
		}
	};
	
	Interop.prototype.showCropper = function() {
		if(this.isMc) {
			callRPC('ui.dialogs.showCropper'); 
		}
	};
	
	Interop.prototype.showTime = function() {
		if(this.isMc) {
			callRPC('ui.dialogs.showTime'); 
		}
	};
	
	Interop.prototype.startTranscode = function() {
		if(this.isMc) {
			callRPC('ui.jobs.start'); 
		} else {
			convert();
			//this.showPluginDiv();
		}
	};
	
	//When using plugin to play video, move play part to popup div
	Interop.prototype.showPluginDiv = function() {
		if(pluginDiv) {
			divpop.showPopup(pluginDiv);
			showPlugin();
		}
	};
	
	//=========================== Parameter Setting============================
	// Every option's value of the element should be the format: 25,1
	Interop.prototype.setFPS = function(id) {
		var element = $(id);
		if(!element) return;
		var fps = element.value.split(',');
		if(this.isMc) {
			
		} else {
			
		}
	};
	
	Interop.prototype.setFormats = function(id) {
		
	};
	
	// Every option's value of the element should be the format: 320,240,4,3
	Interop.prototype.setRes = function(id) {		//Resolution
		
	};
}
