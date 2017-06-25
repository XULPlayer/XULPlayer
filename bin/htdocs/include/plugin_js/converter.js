/*Wrapper class for npxulplayer plugin (converter part)
  Thus the converter class stands alone from gui specific elements.
  Created by zoominla, 01-03-2008 
*/

function CConvert(pluginObj)
{
   this.m_plugin = pluginObj;
   this.m_timeInterval = 100;
   this.m_timerId = null;
   
   this.m_status = 2;
   this.m_percent = 0;
   //calculate last time
   this.m_timeDur	= 0
   this.m_percentOld = 0;
   this.m_lasttime = 0;
   
   // Call back functions
   this.startCallback = null;
   this.stopCallback = null;
   this.pauseCallback = null;
   this.loopCallback = null;
}

CConvert.CONVERT_WAITING = 0;
CConvert.CONVERT_CONVERTING = 1;
CConvert.CONVERT_FINISHED = 2;

CConvert.prototype.initialize = function() {
   //this.m_plugin.init();
};

CConvert.prototype.unInitialize = function() {
   //this.m_plugin.stop();
   this.m_plugin.uninit();
   clearTimeout(this.m_timerId);
   this.m_timerId = null;
};

CConvert.prototype.start = function(xmlCommand) {
   try{
   this.m_plugin.runCov(xmlCommand);
   }catch(e){alert(e);}
   this.m_status = this.m_plugin.covStatus;
   if(this.isConverting()) {
      this.convertLoop();
      if(typeof(this.startCallback) == 'function') {
         this.startCallback();
      }
   }
};

CConvert.prototype.pause = function(){
	this.m_plugin.pauseConvert();
}

CConvert.prototype.end = function(){
	this.m_plugin.stopConvert();
}

//Query status
CConvert.prototype.isConverting = function() {
   return this.m_status == CConvert.CONVERT_CONVERTING;
};
CConvert.prototype.isWaiting = function() {
   return this.m_status == CConvert.CONVERT_WAITING;
};
CConvert.prototype.isFinished = function() {
   return this.m_status == CConvert.CONVERT_FINISHED;
};

//Get convert percentage (result is int type, multiplied by 100)
CConvert.prototype.getPercent = function() {
   return this.m_percent;
};

// Set Callback
CConvert.prototype.setStartCallback = function(func) {
    this.startCallback = func;
};

CConvert.prototype.setPauseCallback = function(func) {
    this.pauseCallback = func;
};

CConvert.prototype.setStopCallback = function(func) {
    this.stopCallback = func;
};
CConvert.prototype.setLoopCallback = function(func) {
   this.loopCallback = func;
};

CConvert.prototype.lastTimeLoop = function(duration) {
 	if(this.m_timeDur >= 2000)
	{
		var perDur = this.m_percent - this.m_percentOld;
		this.m_lasttime =  (100 -  this.m_percent) / perDur * 1000;  
		this.m_percentOld = this.m_percent;
		this.m_timeDur = 0;
	}
	else
	{
		this.m_timeDur += duration;
	}
}

// Main loop when Converting file
CConvert.prototype.convertLoop = function() {  
   this.m_percent = this.m_plugin.covPercent;   
   this.m_status = this.m_plugin.covStatus;
   this.lastTimeLoop(this.m_timeInterval);
   if(!this.isFinished()) {
      if(typeof(this.loopCallback) == "function")
         this.loopCallback();
      var me = this;
      this.m_timerId = setTimeout(function(){me.convertLoop();}, this.m_timeInterval);
   } else {
      if(typeof(this.stopCallback) == "function") {
         this.stopCallback();
      }
   }
};

//-------------------------- Helper functions ---------------------------------
CConvert.prototype.getFolder = function() {
   return this.m_plugin.getFolder(0);
};
CConvert.prototype.getMediaFile = function() {
   return this.m_plugin.getMediaFile(0);
};
CConvert.prototype.getWorkDirectory = function() {
   return this.m_plugin.workDirectory;
};

CConvert.prototype.openXmlFile = function(filename) {
   return this.m_plugin.openXmlFile(filename);
};

CConvert.prototype.createXmlFile = function(xmlText, filename) {
   this.m_plugin.createXmlFile(xmlText, filename);
};

CConvert.prototype.dumpMediaInfo = function(filename) {
   return this.m_plugin.dumpMediaInfo(filename);
};

CConvert.prototype.getPref = function(key) {
   return this.m_plugin.getPrefsValue(key);
};

CConvert.prototype.setPrefs = function(prefsxml) {
   return this.m_plugin.setPrefs(prefsxml);
};

CConvert.prototype.setPref = function(key, value) {
   var xml = '<?xml version="1.0" encoding="UTF-8"?><MediaCoderPrefs>';
   xml += '<node key="' + key + '" value="' + value + '"/>';
   xml += "</MediaCoderPrefs>";
   return this.m_plugin.setPrefs(xml);
}

CConvert.prototype.setPrefs = function(prefsxml) {
   return this.m_plugin.setPrefs(prefsxml);
};

CConvert.prototype.revertPrefs = function(reserves) {
   if (this.m_plugin.revertPrefs) this.m_plugin.revertPrefs(reserves);
};