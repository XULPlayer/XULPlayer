#include "config.h"
#include "device.h"
#include <devguid.h>
#include <string>

DWORD drvmap = 0;

// Copy from HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
static const GUID GUID_DEVINTERFACE_LIST[] = 
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },

	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },

	// GUID_DEVINTERFACE_HID, 
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
			 
	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } },

	// GUID_CD_ROM
	//{ 0x4D36E925, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

	//// GUID_DEVINTERFACE_COMPORT
	//{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },

	//// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	//{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },

	//// GUID_DEVINTERFACE_PARALLEL
	//{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },

	//// GUID_DEVINTERFACE_PARCLASS
	//{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};

HDEVNOTIFY* hDevNotify = 0;

char FindFirstDriveFromMask (DWORD unitmask)
{
	char i;
	for (i = 0; i < 26; ++i) 
	{
		if (unitmask & 0x1) 
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}

bool RegisterDeviceEvents(HWND hwnd)
{
	hDevNotify = new HDEVNOTIFY[sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID)];
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for(int i=0; i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID); i++) {
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify[i] = RegisterDeviceNotification(hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if( !hDevNotify ) {
			return false;
		}
	}
	
	drvmap = GetLogicalDrives();
	return true;
}

void UnregisterDeviceEvents()
{
	if (hDevNotify)
	{
		for(int i=0; i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID); i++) {
			UnregisterDeviceNotification(hDevNotify[i]);
		}
		delete hDevNotify;
	}
	
	hDevNotify = NULL;
}

bool FindDevice(HDEVINFO hDevInfo, std::string& szDevId, SP_DEVINFO_DATA& spDevInfoData)
{
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for(int i=0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) {
		DWORD nSize=0 ;
		TCHAR buf[MAX_PATH];

		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize) ) {
			break;
		} 
		if ( szDevId == buf ) {
			// OK, device found
			return true;
		}
	}
	return false;
}
