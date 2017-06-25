if(typeof(DEVICE_INCLUDE) != "boolean") {
	DEVICE_INCLUDE = true;
	
var canAutoTranscode = false;
var curDeviceName = null;
var curDrive = new Array();
var curDeviceVendor = null;
var curDeviceProduct = null;
var curDeviceID = null;
var curRootDirs = null;
var curDiskVendor = null;
var curDiskProduct = null;
var deviceRecords = "";
var deviceDirRetrieved = false;
var curDiscType = null;
var curDiscDiscId = null;
var curDiscName = null;

function addDvd_to_Tree(dvdpath)
{
	if(mm) {
		mm.addDiscTrack(dvdpath);
	}
}

function onDiscChange(action, dir, volumeName, serialNum)
{
	var info = null;	
	switch (action) {
		case DBT_DEVICEARRIVAL:
			{
				//dumpMsg(dir+"+"+volumeName);
				var mediaType = mc.m_plugin.detectMediaType(dir);
				var mediaInfo = null;
				curDiscType = null;
				curDiscName = volumeName;
				curDiscDiscId = serialNum.toUpperCase();
				switch (mediaType) {
					case DISC_TYPE_DVD:
						mediaInfo = mc.m_plugin.getDVDInfo(dir);
						curDeviceName = "DVD : " + volumeName;
						curDiscType = "DVD";
						break;
					case DISC_TYPE_CD:
						mediaInfo = mc.m_plugin.getCDInfo(dir);
						curDeviceName = "CD : " + volumeName;
						curDiscType = "CD";
						break;
					case DISC_TYPE_VCD:
						mediaInfo = mc.m_plugin.getVCDInfo(dir);
						curDeviceName = "VCD : " + volumeName;
						curDiscType = "VCD";
						break;					
					default:
						curDeviceName = "Data : " + volumeName;
						break;
				}						
				if(mediaInfo)
				{
					var xmlDoc = new XMLDom();
					xmlDoc.loadXML(mediaInfo);
					var rootNode = DomHelper.getChildNode(xmlDoc);
					var node = DomHelper.findChildNode(rootNode, 'vmg');
					if(node)
					{
						node = DomHelper.findChildNode(node, 'discID');
						if(node)curDiscDiscId = DomHelper.getNodeValue(node);
					}						
					window.openDialog('dvdpicker/dvdpicker.xul','DVD picker',
						'chrome, titlebar, centerscreen,modal=no,resizable=no,depended=yes,alwaysRaised=yes,outerHeight=0,outerWidth=0',
						player, addDvd_to_Tree, dir);
				}
				//dumpMsg(mediaInfo);
				curDrive.push(dir);
				if(mm)
				{						
					mm.onDiscArrive();
				}
				info = curDeviceName + "in";
			}
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			//dumpMsg(dir+"-"+volumeName);
			if(mm) { mm.onDiscRemove(); }
			info = curDeviceName + "out";
			break;
	}
	if (info)
		$e("stats").label = info;
		
}

function CopyDiscToLib_Loop()
{
	var status = mc.m_plugin.getDirTransmitInfo(0);
	var percent = mc.m_plugin.getDirTransmitInfo(1);
	var percentsingle = mc.m_plugin.getTransmitInfo(0); 
	var btnlabel = getLocalString('transmit.status.cancel');
	if(status == TST_Idel)
	{
		status = getLocalString("transmit.status.idel");
	}
	else if(status == TST_Copying)
	{
		status =  getLocalString("transmit.status.copying");
	}
	else if(status == TST_Cancel)
	{
		status = getLocalString("transmit.status.cancel");
	}
	else if(status == TST_Failed)
	{
		status = getLocalString("transmit.status.failed");
	}
	else if(status == TST_Failed_DeviceFull)
	{
		status = getLocalString("transmit.status.failed.device.full");
	}
	else if(status == TST_Failed_DeviceLocked)
	{
		status = getLocalString('transmit.status.failed.device.locked');
	}
	else if(status == TST_Over)
	{		
		status = getLocalString("transmit.status.finished");
		btnlabel = getLocalString('btn.close.name');
	}
	
	return {progress1:percentsingle,
		progress:percent,		
		info1:status,
		btn_close:btnlabel};
}

function CopyDiscToLib_Close()
{
	mc.m_plugin.directoryTransmitCancel();
}

function CopyDiscToLib()
{
	if(curDrive.length == 0 || !curDiscType || !mm.libaryDirPath) {
		return;
	}
	var destDir = mm.libaryDirPath + "\\Media Libray\\DISC\\" + curDiscType + "\\" + curDiscDiscId + "_" ;
	if(curDiscName && curDiscName != "") {
		destDir += "[" + curDiscName + "]";
	} else {
		var curtime = new Date();
		var nowTime = "[" + curtime.getFullYear()+ "_"
						  + (curtime.getMonth()+ 1) + "_"
						  + curtime.getDate() + "_"
						  + curtime.getHours() + "_"
						  + curtime.getMinutes() + "_"
						  + curtime.getSeconds()
						  + "]";
		destDir += nowTime;
	}
	//stop player
	mp.stop();
	
	var srcDir = curDrive[0];
	mc.m_plugin.directoryTransmit(srcDir,  destDir);
	var discType  = CPlayer.CONTENT_OTHER;
	if(curDiscType == "DVD") {
		discType = CPlayer.CONTENT_DVD;
	} else if(curDiscType == "VCD") {
		discType = CPlayer.CONTENT_VCD;
	} else if(curDiscType == "CD") {
		discType = CPlayer.CONTENT_CD;
	}
	if(mm) {mm.onDiscCopy(destDir, curDiscName, discType);}
	
	window.openDialog('information_box.xul','DiscTransform',
                    'chrome, titlebar=no, centerscreen,modal=no,resizable=no,depended=yes,alwaysRaised=yes,outerHeight=0,outerWidth=0',
                     {progress1:0, progress:0,
					 title:getLocalString("transmit.infobox.title"),
					 info1:getLocalString("transmit.label.status"),
					 btn_close: getLocalString('transmit.status.cancel')
					 },
					 CopyDiscToLib_Loop,CopyDiscToLib_Close);//
}

function CopyDiscToLibAfterCov()
{
	
}

function onDeviceChange(action, devid, devname)
{
	var i = devid.lastIndexOf("#{");
	if (i > 0) devid = devid.substr(0, i);
	dumpMsg("action: " + action + "\nid: " + devid + "\nname: " + devname)
        var info = null;
	switch (action) {
	case DBT_DEVICEARRIVAL:
		if (curDeviceID) {
			// a device is already plugged in
			//if (curDeviceName) alert("Please unplug " + curDeviceName + " and plugging a new device.")
			if (devname.indexOf("Windows Mobile") >= 0) {
				delay = 7000;
				dumpMsg("Schedule to retrieve device dirs");
				self.setTimeout("getDeviceDirs()", 7000);
			}
			return;
		}
		if (identifyDevice(devid, devname)) {
			// the device is recognized
			info = curDeviceName + " plugged";
			curDeviceID = devid;
		}
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		if (curDeviceID == devid) {
			// the removed device is a previous detected one
			info = curDeviceName + " unplugged";
			removeDevice();
		}
		break;
	}
	if (info) $e("stats").label = info;
}

function onDriveChange(drvchange)
{
	dumpMsg("drive change: " + drvchange);
	var drvlist = drvchange.split(" ");
	var newfound = false;
	for (var i in drvlist) {
		switch (drvlist[i][1]) {
		case '+':
			newfound = true;
			for (x in curDrive) {
				if (curDrive[x] == drvlist[i][0]) {
					newfound = false;
					break;
				}
			}
			if (newfound) {
				curDrive.push(drvlist[i][0]);
				getDeviceDirs(drvlist[i][0]);
			}
			break;
		case '-':
			for (x in curDrive) {
				if (curDrive[x] == drvlist[i][0]) curDrive.splice(x, 1);
			}
			if(mm) {
				mm.onDeviceRemove();
			}
			break;
		}
	}
	if(newfound) {
		if(mm) {
			mm.onDeviceArrive();
		}
	}
}

function getDeviceDirs(drive)
{
	dumpMsg("Retrieving device dirs...");
	curRootDirs = mc.getDeviceDirs(drive);
	dumpMsg("Device dirs: " + curRootDirs);
	identfyDeviceByRootFS(curRootDirs);
}

function removeDevice(id)
{
	canAutoTranscode = false;
	curDeviceName = null;
	curDeviceProduct = null;
	curDeviceVendor = null;
	curDeviceID = null;
	curDrive = [];
}

function identfyDeviceByRootFS(rootdirs)
{
	if (curDeviceVendor == "045e") {
		if (rootdirs.indexOf("MioMap") >= 0) {
			deviceName = "MIO GPS";
			if (loadDeviceExtension("gps-mio"))
				browseNavigate("http://www.mcex.org/index.php?option=com_content&view=category&id=54&Itemid=67")
		} else if (rootdirs.indexOf("__TFAT_HIDDEN_ROOT_DIR__") >= 0) {
			deviceName = "GiPiS GPS";
			if (loadDeviceExtension("gps-gipis"))
				browseNavigate("http://www.mcex.org/index.php?option=com_content&view=category&id=54&Itemid=67")
		}
	}
}

function loadDeviceExtension(name)
{
	var url = localhost + "extensions/" + name + ".7z/index.htm";
	var xmlhttp = new XMLHttpRequest;
	try {
		xmlhttp.open("GET", url, false);
		xmlhttp.send(null);
		if (xmlhttp.status == 200) {
			if (!open_osd_panel(3, url)) {
				browseNavigate(url);
				return true;
			}
		}
		return false;
	} catch (e) {
		return false;
	}
}

function identifyDevice(id, name)
{
	var deviceName = null;
	var mcexid = null;
	var tvexid = null;
	var ret = false;	
	var i;
	if (id.substr(0, 7) == "USBSTOR") {
		// USB massive storage devices
		if ((id.indexOf("SONY") > 0 || id.indexOf("Sony") > 0)) {
			if (id.indexOf("PSP") > 0) {
				mcexid = "id=63%3Apsp&catid=50%3Asony&Itemid=68";
				deviceName = "Sony PSP";
			}
		} else if (curDeviceVendor == "Nokia") {
			if (curDeviceProduct == "Prod_E71") {
				mcexid = "id=70%3Ae71&catid=36%3Anokia-n70&Itemid=57";
				deviceName = "Nokia E71";
			}
		}
		ret = true;
	} else if (id.substr(0, 4) == "USB#") {
		i = id.indexOf("Vid_");
		curDeviceVendor = i >= 0 ? id.substr(i + 4, 4) : null;
		i = id.indexOf("Pid_");
		curDeviceProduct = i >= 0 ? id.substr(i + 4, 4) : null;
		dumpMsg("Vendor ID: " + curDeviceVendor + "  Product ID: " + curDeviceProduct);
		// retrive vendor id and product id from usb id string
		switch (curDeviceVendor) {
		case "045e":
			// Windows Mobile Device
			break;
		case "0421":
			switch (curDeviceProduct) {
			case "012d":
				deviceName = "Nokia 6122C";
				mcexid = "id=77&catid=36&Itemid=57";
				break;
			default:
				deviceName = "Nokia Mobile Phones";
			}
			break;
		default:
			// un-identified vendor id
			if (name.indexOf("Windows Mobile") >= 0) {
				deviceName = name;
			}
		}
	} else {
		if (name.indexOf("iPod") > 0) {
			mcexid = "id=68&catid=25&Itemid=54";
			deviceName = name;
		}
	}

	curDeviceName = deviceName ? deviceName : name;
	if (deviceName) {
		canAutoTranscode = true;
		if (id.substr(0, 4) == "USB#") {
			curDeviceID = id;
			// find usb id/extension id record
			/*
			if (!mcexid) {
				var extid = deviceGetURL();
				if (extid) mcexid = extid;
			}
			*/
		}
		ret = true;
	}
	
	if (!loadDeviceExtension(curDeviceVendor + "-" + curDeviceProduct)) {
		// local device extension is not loaded
		if (mcexid) browseNavigate("http://www.mcex.org/index.php?option=com_content&view=article&" + mcexid);
	}
	
	return ret;
}

function transmitCallback()
{
	var percent = mc.m_plugin.getTransmitInfo(0);
	var status =  mc.m_plugin.getTransmitInfo(1);
	var time   = mc.m_plugin.getTransmitInfo(2);
	var speed  =  mc.m_plugin.getTransmitInfo(3);
	var btnlabel = getLocalString('transmit.status.cancel');
	if(status == TST_Idel)
	{
		status = getLocalString("transmit.status.idel");
	}
	else if(status == TST_Copying)
	{
		status =  getLocalString("transmit.status.copying");
	}
	else if(status == TST_Cancel)
	{
		status = getLocalString("transmit.status.cancel");
	}
	else if(status == TST_Failed)
	{
		status = getLocalString("transmit.status.failed");
	}
	else if(status == TST_Failed_DeviceFull)
	{
		status = getLocalString("transmit.status.failed.device.full");
	}
	else if(status == TST_Failed_DeviceLocked)
	{
		status = getLocalString('transmit.status.failed.device.locked');
	}
	else if(status == TST_Over)
	{		
		status = getLocalString("transmit.status.finished");
		btnlabel = getLocalString('btn.close.name');
	}
	if(time < 0)time='--';
	else time = mp.getTimeString(time / 1000);
	return {progress:percent,
		info1:speed.toFixed(0)+'k/' + getLocalString("time.seconds.name.short"),
		info3:time, info2:status,
		btn_close:btnlabel};
}

function closeCallback()
{
	mc.m_plugin.deviceTransmitCancel();
}

function deviceTransmit(filename)
{
	alert(curDrive.length)
	//deviceSaveURL();
	if (!filename) filename = mc.m_plugin.covDestFile;
	if (!filename) {
		alert("No file is transcoded yet. You can manually choose a file to transmit.");
		var filters =  ['All Files|*.*'];
		var localFiles = Io.loadFile("Choose a file", filters, true);
		if (localFiles.length > 0) filename = localFiles[0].path;
	}
	if (!filename) return;

	if (curDrive.length == 0) {
		alert("No supported/detected device")
		return;
	}
	if (!canAutoTranscode)
		alert("Automatic transcoding for the plugged device is not supported.. The device will be treated as a generic USB mass storage device.")

	//$e("state").label = "Transmitting to " + curDeviceName;
	//$e("stats").label = filename;
	var success = mc.m_plugin.deviceTransmit(filename, curDrive[0] + ':');
	window.openDialog('information_box.xul','MediaInfo',
                    'chrome, titlebar=no, centerscreen,modal=no,resizable=no,depended=yes,alwaysRaised=yes,outerHeight=0,outerWidth=0',
                     {progress:0,
					 title:getLocalString("transmit.infobox.title"),
					 info1:getLocalString("transmit.label.speed"),
					 info2:getLocalString("transmit.label.status"),
					 info3:getLocalString("transmit.label.time_remain"),
					 btn_close: getLocalString('transmit.status.cancel')
					 },
					 transmitCallback,closeCallback);//
}

function deviceSaveURL()
{
	if (curDeviceID) {
		var i = deviceRecords.indexOf(curDeviceID);
		if (i < 0) {
			url = browserGetCurrentUrl();
			i = url.indexOf("&id=");
			var sectionid = parseInt(url.substr(i + 4, 3));
			i = url.indexOf("&catid=");
			var catid = parseInt(url.substr(i + 7, 3));
			i = url.indexOf("&Itemid=");
			var itemid = parseInt(url.substr(i + 8, 3));
			var ext = "&id=" + sectionid + "&catid=" + catid + "&Itemid=" + itemid;
			dumpMsg("adding usb/extid record: " + curDeviceID + " " + ext);
			deviceRecords += curDeviceID + " " + ext + " ";
		}
	}
}

function deviceGetURL()
{
	if (curDeviceID) {
		var i = deviceRecords.indexOf(curDeviceID);
		if (i >= 0) {
			dumpMsg("usb/exid record found for curDeviceID")
			i = deviceRecords.indexOf(" ", i);
			j = deviceRecords.indexOf(" ", ++i);
			alert(deviceRecords.substr(i, j - i));
			return deviceRecords.substr(i, j - i);
		}
	}
	return null;
}

}