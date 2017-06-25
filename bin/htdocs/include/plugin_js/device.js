if(typeof(DEVICE_INCLUDE) != "boolean") {
	DEVICE_INCLUDE = true;
	
var curDeviceName = null;
var curDrive = null;
var curDeviceVendor = null;
var curDeviceProduct = null;
var curDeviceID = null;
var curRootDirs = null;
var curDiskVendor = null;
var curDiskProduct = null;
var deviceRecords = "";

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
			return;
		}
		if (identifyDevice(devid, devname)) {
			// the device is recognized
			info = (curDeviceName ? curDeviceName : devname) + " plugged";
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
        //if (info) $("stats").label = info;
}

function onDriveChange(drvchange, rootdirs)
{
	dumpMsg("drive change: " + drvchange);
	var drvlist = drvchange.split(" ");
	var newfound = false;
	for (var i in drvlist) {
		switch (drvlist[i][1]) {
		case '+':
			curDrive = drvlist[i][0];
			dumpMsg("root dirs: " + rootdirs);
			curRootDirs = rootdirs;
			newfound = true;
			break;
		case '-':
			if (curDrive == drvlist[i][0]) curDrive = null;
			break;
		}
		if (newfound) break;
	}
	if (newfound) {
		if (curDeviceVendor == "045e") {
			if (curRootDirs.indexOf("MioMap")) {
				deviceName = "MIO GPS";
				browseNavigate("http://www.mcex.org/index.php?option=com_content&view=category&id=54%3Amio&Itemid=67")
			}
		}
	}
}

function removeDevice(id)
{
	curDeviceName = null;
	curDeviceProduct = null;
	curDeviceVendor = null;
	curDeviceID = null;
	curDrive = null;
}

function identifyDevice(id, name)
{
	var deviceName = null;
	var mcexid= null;
	if (id.substr(0, 7) == "USBSTOR") {
		// USB massive storage devices
		if ((id.indexOf("SONY") > 0 || id.indexOf("Sony") > 0)) {
			if (id.indexOf("PSP") > 0) {
				mcexid = "id=63%3Apsp&catid=50%3Asony&Itemid=68";
	                        deviceName = "Sony PSP";
			}
		} else if (id.indexOf("Ven_Nokia") > 0) {
			if (id.indexOf("Prod_E71") > 0) {
				mcexid = "id=70%3Ae71&catid=36%3Anokia-n70&Itemid=57";
				deviceName = "Nokia E71";
			}

		}
	} else if (id.substr(0, 4) == "USB#") {
		// retrive vendor id and product id from usb id string
		var i;
		i = id.indexOf("Vid_");
		curDeviceVendor = i >= 0 ? id.substr(i + 4, 4) : null;
		i = id.indexOf("Pid_");
		curDeviceProduct = i >= 0 ? id.substr(i + 4, 4) : null;
		
		switch (curDeviceVendor) {
		case "045e":
			deviceName = "Windows Mobile Device";
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
		}
	} else {
		if (name.indexOf("iPod") > 0) {
			mcexid = "id=68&catid=25&Itemid=54";
			deviceName = name;
		}
	}

	var ret;	
	if (deviceName) {
		curDeviceName = deviceName
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
	} else {
		ret = false;
	}
	if (mcexid) browseNavigate("http://www.mcex.org/index.php?option=com_content&view=article&" + mcexid);
	return ret;
}

function transmitCallback()
{
	var percent = mc.m_plugin.getTransmitInfo(0);
	var status =  mc.m_plugin.getTransmitInfo(1);
	var time   = mc.m_plugin.getTransmitInfo(2);
	var speed  =  mc.m_plugin.getTransmitInfo(3);
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
	else if(status == TST_Over)
	{
		status = getLocalString("transmit.status.finished");;
	}
	if(time < 0)time='--';
	else time = mp.getTimeString(time / 1000);
	return {progress:percent,
		info1:speed.toFixed(0)+'k/' + getLocalString("time.seconds.name.short"),
		info3:time, info2:status};
}

function closeCallback()
{
	mc.m_plugin.deviceTransmitCancel();
}

function deviceTransmit(filename)
{
	//deviceSaveURL();
	if (!filename) filename = mc.m_plugin.covDestFile;
	if (!filename) {
		alert("No file is transcoded yet. You can manually choose a file to transmit.");
		var filters =  ['All Files|*.*'];
		var localFiles = Io.loadFile("Choose a file", filters, true);
		if (localFiles.length > 0) filename = localFiles[0].path;
	}
	if (!filename) return;

	if (!curDrive) {
		alert("No supported/detected device")
		return;
	}
	if (!curDeviceName)
		alert("No recognized device. The device will be treated as a generic USB mass storage device.")

	//$("state").label = "Transmitting to " + curDeviceName;
	//$("stats").label = filename;
	var success = mc.m_plugin.deviceTransmit(filename, curDrive + ':');
	window.openDialog('information_box.xul','MediaInfo',
                    'chrome, titlebar=no, centerscreen,modal=no,resizable=no,depended=yes,alwaysRaised=yes,outerHeight=0,outerWidth=0',
                     {progress:0,
					 title:getLocalString("transmit.infobox.title"),
					 info1:getLocalString("transmit.label.speed"),
					 info2:getLocalString("transmit.label.status"),
					 info3:getLocalString("transmit.label.time_remain")},
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