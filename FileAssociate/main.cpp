#include "RegDefine.h"
#include <stdio.h>
#include <winreg.h>
#include <vector>
#include "resource.h"

#include "../StrPro/xmlaccessorTiny.h"

#pragma comment (lib, "StrPro.lib")
#pragma comment (lib, "Reglib.lib")

using namespace std;
using namespace StrPro;

static HWND hwnd = NULL;
static char curPath[512] = {0};
static vector<char*> selectFormats;

//Dialog msg process
static INT_PTR CALLBACK MainDialogProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam);
//Set all the checkbox checked
static void CheckAll(HWND hwndDlg);
//Set all the checkbox unchecked
static void UnCheckAll(HWND hwndDlg);
//Associate the selected formats via registry operation
static void Associate();
//Get open file name
bool GetOpenFilePath(HWND hwnd, char* fPath, int len, const char* ext = "Reg Backup File\0*.xml");
//Restore file association registry
void RestoreAssoc();
//Backup file association registry
void BackupAssoc();

bool isDirExists(const char* dir)
{
	DWORD attr=GetFileAttributes(dir);
	return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) ? true : false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	//To add icon to dialog, use special class instead
	WNDCLASS wc;
	wc.style = CS_DBLCLKS | CS_SAVEBITS | CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc = DefDlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MyDlgClass";
	RegisterClass(&wc);
	
	//Get the path of current module
	GetModuleFileName(GetModuleHandle(NULL), curPath, 512);
	char* p = strrchr(curPath, '\\');
	if (p) *p = 0;

	//Show dialog box
	hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), 0, MainDialogProc);
	if (!hwnd) return -1;
	ShowWindow(hwnd, SW_SHOW);

	//Process messages
	MSG msg;
	while (GetMessage(&msg,0,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

static UINT checkIds[] = { IDC_AVI, IDC_RM,  IDC_RAM,  IDC_RMVB,  IDC_RMP,  IDC_MPG,  IDC_MPEG,  IDC_MPE,
	IDC_RA,  IDC_WMV,  IDC_WMP,  IDC_ASF,  IDC_WM,  IDC_WMA,  IDC_WAV,  IDC_MID,  IDC_RMI,  IDC_AIF,
	IDC_RT,  IDC_RP,  IDC_SMI,  IDC_SMIL,  IDC_MP4,  IDC_M4V,  IDC_M4P,  IDC_MPA,  IDC_MP2,  IDC_M4A,
	IDC_AAC,  IDC_MOV,  IDC_QT,  IDC_MR,  IDC_3GP,  IDC_3GPP,  IDC_3G2,  IDC_3GP2,  IDC_MP3,  IDC_OGM,
	IDC_FLV,  IDC_VOB,  IDC_SWF,  IDC_MKV,  IDC_OGG,  IDC_CDA,  IDC_APE,  IDC_FLAC};

static char *formats[] = { "AVI", "RM",  "RAM",  "RMVB",  "RMP",  "MPG",  "MPEG",  "MPE",
	"RA",  "WMV",  "WMP",  "ASF",  "WM",  "WMA",  "WAV",  "MID",  "RMI",  "AIF",
	"RT",  "RP",  "SMI",  "SMIL",  "MP4",  "M4V",  "M4P",  "MPA",  "MP2",  "M4A",
	"AAC",  "MOV",  "QT",  "MR",  "3GP",  "3GPP",  "3G2",  "3GP2",  "MP3",  "OGM",
	"FLV",  "VOB",  "SWF",  "MKV",  "OGG",  "CDA",  "APE",  "FLAC"};

INT_PTR CALLBACK MainDialogProc(HWND hwndDlg,UINT msg,WPARAM wParam,LPARAM lParam)
{

	switch (msg) {
	case WM_INITDIALOG:
		hwnd = hwndDlg;	
		CheckAll(hwndDlg);
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		PostMessage(hwndDlg, WM_QUIT, 0, 0);
		hwnd = 0;
		break;
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) 
		{
			switch(LOWORD(wParam)){
			case IDOK:{
					for (int i = 0; i < sizeof(checkIds) / sizeof(checkIds[0]); i++) {
						DWORD ret = (DWORD)SendDlgItemMessage(hwndDlg, checkIds[i], BM_GETCHECK, 0, 0L);
						if(ret) selectFormats.push_back(formats[i]);
					}
					/*char temp[5] = {0};
					sprintf_s(temp, 5, "%d", (int)selectFormats.size());
					MessageBox(NULL, temp, NULL, NULL);*/
					Associate();
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				} 
				break;
			case IDCANCEL:
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			case IDC_SELECT:
				CheckAll(hwndDlg);
				break;
			case IDC_DESELECT:
				UnCheckAll(hwndDlg);
				break;
			case IDC_BACKUP:
				BackupAssoc();
				break;
			case IDC_RESTORE:
				RestoreAssoc();
				break;
			}
		}
		break;
	case IDCANCEL:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	
	default:
		return 0;
	}
	return 1;
}

void CheckAll(HWND hwndDlg)
{
	for (int i = 0; i < sizeof(checkIds) / sizeof(checkIds[0]); i++) {
		SendDlgItemMessage(hwndDlg, checkIds[i], BM_SETCHECK, 1, 0L);
	}
}

void UnCheckAll(HWND hwndDlg)
{
	for (int i = 0; i < sizeof(checkIds) / sizeof(checkIds[0]); i++) {
		SendDlgItemMessage(hwndDlg, checkIds[i], BM_SETCHECK, 0, 0L);
	}
}
void Associate()
{
	char exePath [512] = {0};
	sprintf_s(exePath, 512, "%s\\xulplayer.exe", curPath);
	
	char tempKey[255] = {0};

	for (size_t i=0; i<selectFormats.size(); ++i)
	{
		char suffix[6] = {0};
		sprintf_s(suffix, 6, ".%s", selectFormats[i]);
		//set root key first
		SetRootKey(HKEY_CLASSES_ROOT);
		SetKey(_strlwr(suffix), TRUE);

		//Set the format key's default value to "xulplayer.xxx"
		char xulplayerFormat[15] = {0};
		sprintf_s(xulplayerFormat, 15, "xulplayer%s", suffix);
		WriteString("", xulplayerFormat);

		//Write xulplayer format key
		SetKey(xulplayerFormat, TRUE);
		char mediaType[12] = {0};                
		sprintf_s(mediaType, 12, "%s Media", selectFormats[i]);
		WriteString("", mediaType);

		//Default icon (a sub key of xulplayer format)
		char iconPath[512] = {0};
		ZeroMemory(tempKey, 255);
		sprintf_s(tempKey, 255, "%s\\DefaultIcon", xulplayerFormat);
		CreateKey(tempKey);
		sprintf_s(iconPath, 512, "%s,0", exePath);
		WriteString("", iconPath);

		//Shell key (a sub key of xulplayer format)
		ZeroMemory(tempKey, 255);
		sprintf_s(tempKey, 255, "%s\\shell", xulplayerFormat);
		CreateKey(tempKey);
		WriteString("", "Open");

		//Open key (a sub key of Shell)
		ZeroMemory(tempKey, 255);
		sprintf_s(tempKey, 255, "%s\\shell\\open", xulplayerFormat);
		CreateKey(tempKey);
		WriteString("", "Play");

		//Command key (a sub key of Open)
		char cmdLine[512] = {0};
		ZeroMemory(tempKey, 255);
		sprintf_s(tempKey, 255, "%s\\shell\\open\\command", xulplayerFormat);
		CreateKey(tempKey);
		sprintf_s(cmdLine, 512, "\"%s\" \"%%1\"", exePath);
		WriteString("", cmdLine);
	}
	MessageBox(NULL, "Association is successfully done!", "Ok!--xulplayer", NULL);
}

bool GetOpenFilePath(HWND hwnd, char* fPath, int len, const char* ext)
{
	OPENFILENAME ofn;					// common dialog box structure
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = fPath;
	ofn.nMaxFile = len;
	ofn.lpstrFilter = ext;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags =  OFN_FILEMUSTEXIST | OFN_EXPLORER ;		/*Explorer-style dialog*/
	ofn.hInstance = NULL;
	return GetOpenFileName(&ofn) ? true : false;
}
void BackupAssoc()
{
	char fPath[255] = {0};
	int iCount = 0;
	FILE* fp = NULL;

	iCount = sprintf_s(fPath, 255, "%s\\defaults\\regBackup\\", curPath);
	CreateDirectory(fPath, NULL);
	
	//Adjust backup file name
	for (int i=1; i<100; ++i)
	{
		sprintf_s(fPath+iCount, 255-iCount, "bak%03d.xml", i);
		fp = fopen(fPath, "r");
		if(!fp) break;
		fclose(fp);
		fp = NULL;
	}

	//Write backup file (xml file format)
	CXMLTiny xmlCreate;
	char bakStr[255] = {0};
	xmlCreate.Init();
	xmlCreate.New("1.0", "RegBackup");
	for (size_t i=0; i<sizeof(formats)/sizeof(formats[0]); ++i)
	{
		char suffix[6] = {0};
		sprintf_s(suffix, 6, ".%s", formats[i]);
		//set root key first
		SetRootKey(HKEY_CLASSES_ROOT);
		ZeroMemory(bakStr, 255);
		if(KeyExists(_strlwr(suffix))) {
			SetKey(suffix, false);
			ReadString("", NULL, bakStr);
		} else {
			sprintf_s(bakStr, 255, "Delete");
		}
		
		xmlCreate.addChild("Node", bakStr);
		xmlCreate.goParent();
	}
	xmlCreate.Save(fPath, "UTF-8");
	xmlCreate.Cleanup();
	MessageBox(NULL, "Backup is successfully done!", "Ok!--xulplayer", NULL);
}

void RestoreAssoc()
{
	char bakPath[255] = {0};
	int iCount = 0;
	FILE* fp = NULL;

	iCount = sprintf_s(bakPath, 255, "%s\\defaults\\regBackup", curPath);
	if(isDirExists(bakPath)){
		sprintf_s(bakPath+iCount, 255-iCount, "\\bak001.xml");
		// Get backup file path
		if(!GetOpenFilePath(hwnd, bakPath, 255)){
			ZeroMemory(bakPath, 255);
			return;
		}
		
		// Parse backup file (XML format)
		CXMLTiny xmlOpen;
		xmlOpen.Init();
		xmlOpen.Open(bakPath);
		if(strcmp(xmlOpen.getNodeName(), "RegBackup") != 0){
			MessageBox(NULL, "File type is incorrect!", NULL, NULL);
			xmlOpen.Close();
			xmlOpen.Cleanup();
			return;
		}

		vector<const char*> bakStr;
		if(xmlOpen.goChild()) {
			xmlOpen.goFirst();
		} else {
			xmlOpen.Close();
			xmlOpen.Cleanup();
			return;
		}
		do{
			bakStr.push_back(xmlOpen.getNodeValue());
		}while(xmlOpen.goNext());
		xmlOpen.Cleanup();

		// Write backup info to registry
		for (size_t i=0; i<bakStr.size(); ++i)
		{
			//set root key first
			char suffix[6] = {0};
			sprintf_s(suffix, 6, ".%s", formats[i]);
			SetRootKey(HKEY_CLASSES_ROOT);
			SetKey(_strlwr(suffix), TRUE);
			if(strcmp(bakStr[i], "Delete") == 0){
				DeleteValue("");
			} else {
				WriteString("", bakStr[i]);
			}
		}
		MessageBox(NULL, "Successfully Restored!", "Ok!--xulplayer", NULL);
	} else {
		MessageBox(NULL, "You never create file association backup!\nCreate backup first!", NULL, NULL);
	}
	
	
}