const tv_tool_bar_height = 200;

function showTvToolBar()
{
    $e("tv_tool_bars").hidden = false;
    var h = $e("tv_tool_bars").style.heigt;   
    h = parseInt(h);
    /*if( h < tv_tool_bar_height)
    {
        $e("tv_tool_bars").style.heigt = (h + 10).toString() + "px";
        setTimeOut(showTvToolBar, 50);
    }*/
}

function encodeLoopCallbackTV()
{
    var percent = mc.getPercent();
    $e("tv_cov_progress").value = percent;
    $e("tv_cov_percent").value = getLocalFormatString("tips.convert.percentdone", percent.toFixed(1));
    var sec = mc.m_lasttime / 1000;
    var str = '';
    var unit1 = 60;
    var unit2 = 60 * unit1;
    var unit3 = 60 * unit2;
    var num1, num2, num3;
    num1 = num2 = num3 = 0;
    if(sec < unit3)
    {		
        num1 = parseInt(sec % unit1);
        num2 = sec % unit2; num2 = parseInt((num2 - num1) / unit1);
        num3 = parseInt(sec / unit2); 		
    }
    else
    {
        $e("tv_cov_time").value = "";
        return;
    }
    if(num2 != 0)
    {
        str = num2.toString() + getLocalString("time.minute.name.short");
        if(num2 < 10)
            str = ' ' + str;
    }
    if(num3 != 0)
    {
        str = num3.toString() + getLocalString("time.hour.name.short") + str;
        if(num3 < 10)
            str = ' ' + str;
    }
    if(num3 == 0 && num2 == 0)
    {
        str = '< 1' + getLocalString("time.minute.name.short");
    }
    $e("tv_cov_time").value = str;
	
	var status = "";
	if(mc.isAborted())
	{
		status = getLocalString("convert.status.aborted");
	}
	else if(mc.isConverting())
	{
		status = getLocalString("convert.status.converting");
	}
	else if(mc.isFailed())
	{
		status = getLocalString("convert.status.failed");
	}
	else if(mc.isWaiting())
	{
		status = getLocalString("convert.status.wait");
	}
	else if(mc.isFinished)
	{
		status = getLocalString("convert.status.finished");
	}
	$e("tv_cov_status").value = status;
}
function encodeStopCallbackTV()
{
	$e("tv_cov_progress").value = 100;
    $e("tv_cov_percent").value = "";
	$e("tv_cov_time").value = "";
	if(mc.isAborted()) {
		$e("tv_cov_percent").value = getLocalString("tips.convert.aborted");
		$e("tv_cov_status").value = getLocalString("convert.status.aborted");
	} else if(mc.isFailed()) {
		$e("tv_cov_percent").value = getLocalString("tips.convert.failed");
		$e("tv_cov_status").value = getLocalString("convert.status.failed");
	}
	else{
		transcodedFileObject[curTranscodeFile] = mc.m_plugin.covDestFile;
		$e("tv_cov_percent").value = getLocalString("tips.convert.successful");
		$e("tv_cov_status").value = getLocalString("convert.status.finished");
	}
}

function encodeStartTV()
{
    try{
	 //detect Mencoder    
	 var mencoderPath = mc.getWorkDirectory() + "\\codecs\\mencoder.exe";
	 var mencoderFile = Io.getFileFrom(mencoderPath);
	 if(!mencoderFile || !mencoderFile.exists())
        return;
    }
    catch(e){
        return; 
    }
    var filename = null;
   
    if(mm) {
        filename = mm.getSelectedFile();
    }
    if(!filename)
    return;

    var fileext =  getFileExt(filename);
    var contenttype = getContentType(fileext);
    if(   contenttype == CPlayer.CONTENT_AUDIO
       || contenttype == CPlayer.CONTENT_CD
       || contenttype == CPlayer.CONTENT_IMAGE
       || contenttype == CPlayer.CONTENT_OTHER)
    {
        return;
    }
    $e("tv_tool_bars").selectedIndex = 1;
    if($e("tv_tool_bars").hidden)
    {       
       showTvToolBar();
    }
    
    var convertfile = filename;
    
    curTranscodeFile = convertfile;
    if(filename.indexOf("dvd://") == 0 ||filename.indexOf("vcd://") == 0||filename.indexOf("cd://") == 0)
    {
        convertfile = get_dvd_file_xml(filename);
        var infos = filename.split(":");
        $e("tv_cov_title").value = infos[0].toUpperCase();
    }
    else
    $e("tv_cov_title").value = getFileTitle(filename);
    
    mc.setLoopCallback(encodeLoopCallbackTV);
    mc.setStopCallback(encodeStopCallbackTV);
    mc.start(convertfile);	
    if(mc.isFailed())
    {
        encodeStopTV();
        return;
    }
}

function encodeStopTV()
{
	if(mc.isConverting() || mc.isWaiting()) {
		mc.end();
	}
}

function showPlayInfoDeck()
{
    $e("tv_tool_bars").selectedIndex = 0;
    showTvToolBar();
	$e("pl_info_progress_front").style.width = "100px";
	//dumpMsg("proc:"+$e("pl_info_progress_front").style.width);
}

function showDownloadDeck()
{
    $e("tv_tool_bars").selectedIndex = 2;
    showTvToolBar();
    MDialog.init();
}