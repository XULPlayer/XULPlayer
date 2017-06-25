/* Copyright (c) 2007 Stanley Huang, All Rights Reserved
   See the file LICENSE.txt for licensing information. */
var dualmode;

function dualChooseFile(idx)
{
    //the type of localFile is nsILocalFile
    var filters =  ['All|*.*', 'AVI|*.avi', 'MP4|*.mp4', 'Windows Media|*.asf; *.wmv; *.wma',
					'MPEG|*.mpg', 'MP3|*.mp3', 'FLAC|*.flac',
					'RealMedia|*.rmvb; *.rm', 'QuickTime|*.mov; *.mp4'
				   ];
	var localFiles = Io.loadFile("Play File", filters, true);
    var filename = null;
    var filesize = null;
	if(localFiles instanceof Array) {
        filesize = localFiles[0].fileSize;
        filesize /= 1024;	//Convert to KB
        filename = localFiles[0].path;
    }
    var menu = $e("menuDualFiles"+idx);
    var item = document.createElement("menuitem");
    item.setAttribute("label", filename);
    item.setAttribute("type", "radio");
    item.setAttribute("checked", "true");
    item.setAttribute("oncommand", "switchPlayer("+idx+");startPlay(getAttribute('label'));");
    menu.appendChild(item);
    
    switchPlayer(idx);
    startPlay(filename);
    addFile(filename, filesize);
}

function dualChooseItem(idx, filename)
{
    if(!filename) filename = plMan.getSelectedFile();
    
    var menu = $e("menuDualFiles"+idx);
    var item = document.createElement("menuitem");
    item.setAttribute("label", filename);
    item.setAttribute("type", "radio");
    item.setAttribute("checked", "true");
    item.setAttribute("oncommand", "switchPlayer("+idx+");startPlay(getAttribute('label'));");
    menu.appendChild(item);
   
    switchPlayer(idx);
    startPlay(filename);
}

function setCrossVolume()
{
    var n = getValueInt("crossfade");
    mp.sendCommand("volume " + (100 - n) + " 1", false, 0);
    mp.sendCommand("volume " + n + " 1", false, 1);
}

function syncToPlayer1()
{
    var pos = mp.m_plugin.getInfo(CPlayer.PLAYER_POS, 0)/1000;
    var seekCmd = 'seek ' + pos + ' 2';
    mp.sendCommand(seekCmd, false, 0);
    mp.sendCommand(seekCmd, false, 1);
}

function syncToPlayer2()
{
    var pos = mp.m_plugin.getInfo(CPlayer.PLAYER_POS, 1)/1000;
    var seekCmd = 'seek ' + pos + ' 2';
    mp.sendCommand(seekCmd, false, 0);
    mp.sendCommand(seekCmd, false, 1);
}

function syncReplay()
{
    if(mp.isPlaying()) {
        var seekCmd = 'seek 0 2';
        mp.sendCommand(seekCmd, false, 0);
        mp.sendCommand(seekCmd, false, 1);
    } else {
        mp.setActivePlayer(0);
        startPlay(mp.m_playingFile[0]);
        mp.setActivePlayer(1);
        startPlay(mp.m_playingFile[1]);
    }
}

function syncToPos()
{
    var pos = prompt("Please a time position(can be float numer). Unit: second", "Synchronize Player");
    var seekCmd = 'seek ' + pos + ' 2';
    mp.sendCommand(seekCmd, false, 0);
    mp.sendCommand(seekCmd, false, 1);
}
