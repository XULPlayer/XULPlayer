/* Processing key press under different situations. 
   Created by Zoominla, 03-17-2009         
*/
if(typeof(KEY_PROCESS_INCLUDE) != "boolean") {
	KEY_PROCESS_INCLUDE = true;
//------------------------ Key code definition (keyboard)----------------------
const KEY_CODE_ESC = 27;
const KEY_CODE_SPACE = 32;
const KEY_CODE_ENTER = 13;
const KEY_CODE_SHIFT = 16;
const KEY_CODE_CTRL = 17;
const KEY_CODE_LEFT = 37;
const KEY_CODE_RIGHT = 39;
const KEY_CODE_UP = 38;
const KEY_CODE_DOWN = 40;
const KEY_CODE_PAGEUP = 33;
const KEY_CODE_PAGEDOWN = 34;
const KEY_CODE_BACKSPACE = 8;
const KEY_CODE_PLUS = 107;
const KEY_CODE_MINUS = 109;

const KEY_CODE_CONTEXT = 93;

const KEY_CODE_C = 67;
const KEY_CODE_D = 68;
const KEY_CODE_E = 69;
const KEY_CODE_F = 70;
const KEY_CODE_G = 71;
const KEY_CODE_H = 72;
const KEY_CODE_I = 73;
const KEY_CODE_J = 74;
const KEY_CODE_M = 77;
const KEY_CODE_O = 79;
const KEY_CODE_P = 80;
const KEY_CODE_R = 82;
const KEY_CODE_S = 83;
const KEY_CODE_T = 84;
const KEY_CODE_X = 88;
const KEY_CODE_Z = 90;
const KEY_CODE_V = 86;
const KEY_CODE_B = 66;

//---------------------- Remote control code definition (remote)-------------
const REMOTE_CODE_SLEEP = 255;
const REMOTE_CODE_ESC = 27;
const REMOTE_CODE_ENTER = 13;

const REMOTE_CODE_LEFT = 37;
const REMOTE_CODE_RIGHT = 39;
const REMOTE_CODE_UP = 38;
const REMOTE_CODE_DOWN = 40;
const REMOTE_CODE_PAGEUP = 33;
const REMOTE_CODE_PAGEDOWN = 34;

const REMOTE_CODE_RETURN = 76;
const REMOTE_CODE_PLUS = 106;
const REMOTE_CODE_MINUS = 111;
const REMOTE_CODE_MENU = 77;
const REMOTE_CODE_PREV = 70;
const REMOTE_CODE_NEXT = 71;

const REMOTE_CODE_MUTE = 67;
const REMOTE_CODE_AUDIO_CH = 9;
const REMOTE_CODE_TV = 83;
const REMOTE_CODE_BROADCAST = 89;
const REMOTE_CODE_INFO = 79;
const REMOTE_CODE_FAVORITE = 87;
const REMOTE_CODE_VOD = 144;
const REMOTE_CODE_PROGRAM_LIST = 46;

const REMOTE_CODE_STOP = 81;
const REMOTE_CODE_PLAY = 80;

const REMOTE_CODE_0 = 45;
const REMOTE_CODE_1 = 97;
const REMOTE_CODE_2 = 98;
const REMOTE_CODE_3 = 99;
const REMOTE_CODE_4 = 100;
const REMOTE_CODE_5 = 101;
const REMOTE_CODE_6 = 102;
const REMOTE_CODE_7 = 103;
const REMOTE_CODE_8 = 104;
const REMOTE_CODE_9 = 105;

const REMOTE_CODE_SHUT = 255;

//---------------------- USB Remote control code definition (remote)-------------
// common keycode
const USBREMOTE_CODE_ESC = 27;
const USBREMOTE_CODE_ENTER = 13;

const USBREMOTE_CODE_LEFT = 37;
const USBREMOTE_CODE_RIGHT = 39;
const USBREMOTE_CODE_UP = 38;
const USBREMOTE_CODE_DOWN = 40;
const USBREMOTE_CODE_PAGEUP = 33;
const USBREMOTE_CODE_PAGEDOWN = 34;

const USBREMOTE_CODE_BACKSPACE = 8;
const USBREMOTE_CODE_PLUS = 175;
const USBREMOTE_CODE_MINUS = 174;

// play control keycode
const USBREMOTE_CODE_TVORVIDEO = 122;
const USBREMOTE_CODE_MENU = 0x45A0A2;
const USBREMOTE_CODE_OSD = 0x44A0A2;
const USBREMOTE_CODE_ZOOM = 0x5AA0A2;
const USBREMOTE_CODE_SCREENSHOT = 0x43A0A2;
const USBREMOTE_CODE_RECORD = 0x52A0A2;

const USBREMOTE_CODE_MUTE = 173;
const USBREMOTE_CODE_STOP = 178;
const USBREMOTE_CODE_PLAY = 179;
const USBREMOTE_CODE_PREV = 177;
const USBREMOTE_CODE_NEXT = 176;
const USBREMOTE_CODE_FORWARD = 0x46A0A2;
const USBREMOTE_CODE_REWIND = 0x57A0A2;
const USBREMOTE_CODE_SUBTITLE = 0x53A0A2;

// categorys
const USBREMOTE_CODE_MEDIACENTER = 0x43A4A0A2;
const USBREMOTE_CODE_PICTURE = 0x50A4A0A2;
const USBREMOTE_CODE_MUSIC = 0x4DA4A0A2;
const USBREMOTE_CODE_VIDEO = 0x56A4A0A2;
const USBREMOTE_CODE_SURF = 0x56A0A2;
const USBREMOTE_CODE_TV = 0x54A4A0A2;
const USBREMOTE_CODE_DVD = 181;

// Numbers
const USBREMOTE_CODE_0 = 48;
const USBREMOTE_CODE_1 = 49;
const USBREMOTE_CODE_2 = 50;
const USBREMOTE_CODE_3 = 51;
const USBREMOTE_CODE_4 = 52;
const USBREMOTE_CODE_5 = 53;
const USBREMOTE_CODE_6 = 54;
const USBREMOTE_CODE_7 = 55;
const USBREMOTE_CODE_8 = 56;
const USBREMOTE_CODE_9 = 57;

//------------------------ Situation definition -----------------------------
const CONTROL_BY_KEYBOARD = 0;
const CONTROL_BY_REMOTE = 1;
const CONTROL_BY_USBREMOTE = 2;

var ctrlType = CONTROL_BY_KEYBOARD;

//------------------------ Map common key code--------------------------
const VK_ESC = 27;
const VK_ENTER = 13;
const VK_LEFT = 37;
const VK_RIGHT = 39;
const VK_UP = 38;
const VK_DOWN = 40;
const VK_PAGEUP = 33;
const VK_PAGEDOWN = 34;
const VK_BACKSPACE = 8;
const VK_CONTEXT = 93;

// play control
var VK_TVORVIDEO = -1;
var VK_MENU = -1;
var VK_OSD = -1;
var VK_ZOOM = -1;
var VK_SCREENSHOT = -1;
var VK_RECORD = -1;

var VK_MUTE = -1;
var VK_STOP = -1;
var VK_PLAY = -1;
var VK_PAUSE = -1;
var VK_PREV = -1;
var VK_NEXT = -1;
var VK_FORWARD = -1;
var VK_REWIND = -1;
var VK_SUBTITLE = -1;
var VK_PLUS = -1;
var VK_MINUS = -1;

// categorys
var VK_MEDIACENTER = -1;
var VK_PICTURE = -1;
var VK_MUSIC = -1;
var VK_VIDEO = -1;
var VK_SURF = -1;
var VK_TV = -1;
var VK_DVD = -1;

var VK_0 = 48;
var VK_1 = 49;
var VK_2 = 50;
var VK_3 = 51;
var VK_4 = 52;
var VK_5 = 53;
var VK_6 = 54;
var VK_7 = 55;
var VK_8 = 56;
var VK_9 = 57;

switch(ctrlType) {
    case CONTROL_BY_KEYBOARD:
        VK_TVORVIDEO = KEY_CODE_V;
        VK_MENU = KEY_CODE_M;
        VK_OSD = KEY_CODE_O;
        VK_ZOOM = KEY_CODE_Z;
        VK_SCREENSHOT = KEY_CODE_S;
        VK_SUBTITLE = KEY_CODE_J;
        VK_PLUS = KEY_CODE_PLUS;
        VK_MINUS = KEY_CODE_MINUS;
		VK_PAUSE = KEY_CODE_SPACE;
		VK_ESC = KEY_CODE_ESC;
        // categorys
        break;
    case CONTROL_BY_REMOTE:
        VK_MENU = REMOTE_CODE_MENU;
        VK_OSD = REMOTE_CODE_INFO;

        VK_MUTE = REMOTE_CODE_MUTE;
        VK_STOP = REMOTE_CODE_STOP;
        VK_PLAY = REMOTE_CODE_PLAY;
        VK_PREV = REMOTE_CODE_PREV;
        VK_NEXT = REMOTE_CODE_NEXT;
        VK_TV = REMOTE_CODE_TV;
        
        VK_PLUS = REMOTE_CODE_PLUS;
        VK_MINUS = REMOTE_CODE_MINUS;
        VK_0 = 45;
        VK_1 = 97;
        VK_2 = 98;
        VK_3 = 99;
        VK_4 = 100;
        VK_5 = 101;
        VK_6 = 102;
        VK_7 = 103;
        VK_8 = 104;
        VK_9 = 105;
        break;
    case CONTROL_BY_USBREMOTE:
        VK_TVORVIDEO = USBREMOTE_CODE_VIDEO;
        VK_MENU = USBREMOTE_CODE_MENU;
        VK_OSD = USBREMOTE_CODE_OSD;
        VK_ZOOM = USBREMOTE_CODE_ZOOM;
        VK_SCREENSHOT = USBREMOTE_CODE_SCREENSHOT;
        VK_RECORD = USBREMOTE_CODE_RECORD;
        
        VK_MUTE = USBREMOTE_CODE_MUTE;
        VK_STOP = USBREMOTE_CODE_STOP;
        VK_PLAY = USBREMOTE_CODE_PLAY;
        VK_PREV = USBREMOTE_CODE_PREV;
        VK_NEXT = USBREMOTE_CODE_NEXT;
        VK_FORWARD = USBREMOTE_CODE_FORWARD;
        VK_REWIND = USBREMOTE_CODE_REWIND;
        VK_SUBTITLE = USBREMOTE_CODE_SUBTITLE;
        VK_PLUS = USBREMOTE_CODE_PLUS;
        VK_MINUS = USBREMOTE_CODE_MINUS;
        // categorys
        VK_MEDIACENTER = USBREMOTE_CODE_MEDIACENTER;
        VK_PICTURE = USBREMOTE_CODE_PICTURE;
        VK_MUSIC = USBREMOTE_CODE_MUSIC;
        VK_VIDEO = USBREMOTE_CODE_VIDEO;
        VK_SURF = USBREMOTE_CODE_SURF;
        VK_TV = REMOTE_CODE_TV;
        VK_DVD = USBREMOTE_CODE_DVD;
        break;
}

function onAppKeyEvent(keycode)
{
    //dumpMsg("You press "+keycode);
	//let pop menu get foucs
	if(captureKeyEvents_menu(keycode)) return;
	switch(keycode) {
    case KEY_CODE_D:
        if(curViewMode == VIEW_CENTER) setViewMode(VIEW_FULL);
        break;
	
    case VK_LEFT:
		mp.rewind(10);
        break;
	
    case VK_RIGHT :
		mp.forward(10);
        break;
    
    case VK_SUBTITLE:
        mp.switchSubtitle();
        break;
    case VK_OSD :
        mp.switchOsd();
        break;
    
    case VK_PLUS :
        mp.increaseVolume();
        break;
    case VK_MINUS :
        mp.decreaseVolume();
        break;
    case VK_ENTER :
        break;
    case VK_UP:
		mp.forward(60);
        break;
    case VK_DOWN:
		mp.rewind(60);
        break;
    case VK_PAGEDOWN:
		next_cmd();
        break;
    case VK_PAGEUP:
		prev_cmd();
        break;
    case VK_BACKSPACE:
        break;
    case VK_MENU :
        break;
    
    case VK_PLAY:
        play_cmd();
        break;
	case VK_PAUSE:
        pause_cmd();
        break;
	case VK_ESC:
		if(fs) toggleFullScreen();
		break;
    case VK_STOP:
        stop_cmd();
        break;
    case VK_FORWARD:
        mp.forward(20);
        break;
    case VK_REWIND:
        mp.rewind(20);
        break;
    case VK_PREV:
        prev_cmd();
        break;
    case VK_NEXT:
        next_cmd();
        break;
    case VK_ZOOM:           // picture zoomin/zoomout
        break;
    case VK_MEDIACENTER:
        stop_cmd();
        break;
    case VK_0:
        toggleFullScreen();
        break;
	}
}

}