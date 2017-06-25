/* Wrapper for controlling toggle media manager/ mediaList / video box window
   with animation
   Created by Zoominla, 04-29-2009         
*/

if (typeof(MEDIA_WINDOW_INCLUDE) != "boolean") {
    MEDIA_WINDOW_INCLUDE = true;
    
    var screenWidth = window.screen.width;
    var screenHeight = window.screen.height;
    var videoBOxWidth = Math.floor(screenWidth * 0.45);
	var mediaListWidth = screenWidth - videoBOxWidth;
    var isMediaListMode = true;
    
    function hideMediaManager() {
        // Hide mediaManager
        var elem = $e("mediaManager");
        elem.left = -videoBOxWidth;
        elem.width = videoBOxWidth;
        elem.height = screenHeight;
    }
	
	function showMediaManager() {
        // Hide mediaManager
        var elem = $e("mediaManager");
        elem.left = 0;
        elem.width = videoBOxWidth;
        elem.height = screenHeight;
    }
    
	function showMediaListLeft() {
        // Show meidaList at left side
        var elem = $e("mediaList");
        elem.left = 0;
        elem.width = mediaListWidth;
        elem.height = screenHeight;
    }
	function showMediaListRight() {
        // Show meidaList at right side
        var elem = $e("mediaList");
        elem.left = videoBOxWidth;
        elem.width = mediaListWidth;
        elem.height = screenHeight;
    }	

    function hideVideoBox() {
        // Hide mediaManager
        var elem = $e("mainDisplayArea");
        elem.left = screenWidth;
        elem.width = videoBOxWidth;
        elem.height = screenHeight;
    }
    
    function showVideoBox() {
        // Show VideoBox
        var elem = $e("mainDisplayArea");
        elem.left = mediaListWidth;
        elem.width = videoBOxWidth;
        elem.height = screenHeight;
    }
    
    function showVideoBoxFullScreen() {
        // Show VideoBox
        var elem = $e("mainDisplayArea");
        elem.left = 0;
        elem.width = screenWidth;
        elem.height = screenHeight;
    }
	
    function resizeVideoBox() {
        var elem = $e("mainDisplayArea");
        elem.left = 0;
        var parentElem = elem.parentNode;
        if(parentElem ) {
            elem.width = parentElem.boxObject.width;
            elem.height = parentElem.boxObject.height;
        }
    }
    
    function mediaLibFullScreen() {
        var elem = $e("mainDisplayArea");
        if(fs) {
			elem.left = 0;
            elem.width = screenWidth;
		} else {
			if(isMediaListMode) {
				elem.left = mediaListWidth;
                elem.width = videoBOxWidth;
			} else {
				elem.left = screenWidth;
                elem.width = videoBOxWidth;
			}
		}
    }
	//---------Animation: Slide window--------------
	const MEDIA_SLIDE_STEP = screenWidth/10;
	const MEDIA_SLIDE_THRETHOLD = screenWidth/8;
	const MEDIA_SLIDE_TIMER = 40;
    var variableStep = MEDIA_SLIDE_STEP;
    
    function slideMediaLibLeft() {
        _slideMediaLibLeftRecurse();
        isMediaListMode = true;
        variableStep = MEDIA_SLIDE_STEP
    }
    
    function slideMediaLibRight() {
        _slideMediaLibRightRecurse();
        isMediaListMode = false;
        variableStep = MEDIA_SLIDE_STEP
    }
    
	function _slideMediaLibLeftRecurse() {
        var mediaManElem = $e('mediaManager');
        var mediaListElem = $e('mediaList');
        var videoBoxElem = $e('mainDisplayArea');
        
        mediaManElem.left = parseInt(mediaManElem.left)-variableStep;
        mediaListElem.left = parseInt(mediaListElem.left)-variableStep;
        videoBoxElem.left = parseInt(videoBoxElem.left)-variableStep;
        
        var mlLeft = parseInt(mediaListElem.left);
        if(mlLeft > MEDIA_SLIDE_THRETHOLD) {
            setTimeout(_slideMediaLibLeftRecurse, MEDIA_SLIDE_TIMER);
        } else {
            hideMediaManager();
            showMediaListLeft();
            showVideoBox();
        }
        variableStep += 10;
	}
	
	function _slideMediaLibRightRecurse() {
        var mediaManElem = $e('mediaManager');
        var mediaListElem = $e('mediaList');
        var videoBoxElem = $e('mainDisplayArea');
    
        mediaManElem.left = parseInt(mediaManElem.left)+variableStep;
        mediaListElem.left = parseInt(mediaListElem.left)+variableStep;
        videoBoxElem.left = parseInt(videoBoxElem.left)+variableStep;
        
        var mmLeft = parseInt(mediaManElem.left);
        if(mmLeft < -MEDIA_SLIDE_THRETHOLD) {
            setTimeout(_slideMediaLibRightRecurse, MEDIA_SLIDE_TIMER);
        } else {
            showMediaManager();
            showMediaListRight();
            hideVideoBox();
        }
        variableStep += 10;
	}
	
	
}