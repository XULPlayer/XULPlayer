/* Module for parsing and displaying lyrics 
   Created by Zoominla, 08-23-2007         
   
   @Function: Parse lyric file, display lyric in the sidebar
   @Usage:    Call the function findLyricAndDisplay() to read file and parse lyric
              Call the function updateLyricDisplay() in timeProc() to update
              the lyric according to playing progress
*/
   
if(typeof(LYRIC_PARSING) != "boolean") {
	LYRIC_PARSING = true;   
	
  var tiArr = new Array();							
  var words = new Array();								
  var curIndex = -1;
  var offset = 0;
  var ifDisplayLyric = false;
  var lyricPath = null;
  
  function timeAndIndex (time, wordsIndex)
  {
  	this.time = time;
  	this.wordsIndex = wordsIndex;
  }
  function timeSortFunc(timeAndIndex1, timeAndIndex2)
  {
    return timeAndIndex1.time - timeAndIndex2.time;
  }
  function parseLyric(wholeContent)
  {
  	var indexCount = 0;
    clearArray(tiArr);
    clearArray(words);
  	for(var i=0; i<wholeContent.length; ++i) {
  		var wordsLine = wholeContent[i];
  		if(wordsLine.length > 2 && isNaN(wordsLine.charAt(2))){ //if there is no time label
            if(wordsLine.indexOf("offset") >= 0) {              //get offset
                var index1 = wordsLine.indexOf(':');
                var index2 = wordsLine.indexOf(']');
                if(index1 >=0 && index2 >= 0)
                    offset = parseInt(wordsLine.substr(index1 + 1, index2));
            }
            continue;
        }
  		
        //parse main content 
  		var leftIndex = wordsLine.indexOf('['); 
	  	while( leftIndex >= 0) {
	  		var rightIndex = wordsLine.indexOf(']');
	  		var timeTab = wordsLine.substr(leftIndex+1, rightIndex);
	  		var mins = parseInt(timeTab.substr(0,2));
	  		var secs = parseInt(timeTab.substr(3,5));
	  		var ms   = (mins*60 + secs)*1000;  ms += offset;
	  		
	  		//there exists 2 kind of time labels  ( [xx:xx.xx] or [xx:xx] )
	  		//the 3rd part of time label is optional, so need to be treated specially
	  		var dotIndex = timeTab.indexOf('.');
	  		if(dotIndex >= 0) ms += parseInt(timeTab.substr(dotIndex+1,dotIndex+3))*10;
  		
  			wordsLine = wordsLine.substr(rightIndex+1);
  			leftIndex = wordsLine.indexOf('[');
  			tiArr.push(new timeAndIndex(ms, indexCount));
  		}
  		indexCount++;
  		words.push(wordsLine);
  	}
  	tiArr.sort(timeSortFunc);			
  }
  
  
  
  function readLyricFile(fnameOrIFile)
  {
  	var localFile ;
    if(fnameOrIFile && fnameOrIFile.isFile()){
        localFile = fnameOrIFile;
    } else {
        localFile = Io.getFileFrom(lyricPath);
        localFile.append(fnameOrIFile);
    }

  	try {
  		var wholeContent = Io.openUTF8File(localFile);
		parseLyric(wholeContent);
  	}
  	catch(excpt){
  		alert("File reading error\n" + excpt.toString());
  	}
  }
    
    function findLyricAndDisplay()
    {
        if(!mp.hasVideo()) {
            var fname = getFileTitle(mp.getPlayingFile());
            var lyricDir = Io.getFileFrom(lyricPath);
            if(lyricDir)
                lyricDir.append(fname + ".lrc");
            
            if(!lyricDir || !lyricDir.exists()){    //if no lyric file in lyrics then search the dir of playing file
                var fDir = getFileDir(mp.getPlayingFile());
                lyricDir = Io.getFileFrom(fDir);
                if(lyricDir && lyricDir.exists())
                    lyricDir.append(fname + ".lrc");
            }
            
            if(lyricDir && lyricDir.exists()) {
               changeVideoBoxPage(INDEX_LYRIC);
                readLyricFile(lyricDir);
                displayLyric();
                ifDisplayLyric = true;
                return;
            }
            //loadPage(homeURL);
        }
        ifDisplayLyric = false;
    }
    
    function displayLyric()
    {
        var txtArea = $e("textList");
        var i=0;
        var len = txtArea.getRowCount();
        for(; i<len; ++i) {
            txtArea.removeItemAt(0);
        }
        for(i=0; i<tiArr.length; ++i) {
            txtArea.appendItem(words[tiArr[i].wordsIndex]);
        }
    }
    function updateLyricDisplay(playerPos)
    {
        var i=0;
        for(; i<tiArr.length; ++i) {
            if(playerPos <= tiArr[i].time){
                break;
            }
        }
        if(curIndex == i-1) return;
        curIndex = i-1<0 ? 0 : i-1;
        var txtArea = $e("textList");
        txtArea.selectItem(txtArea.getItemAtIndex(curIndex));
        txtArea.ensureIndexIsVisible(curIndex);
    }
}
