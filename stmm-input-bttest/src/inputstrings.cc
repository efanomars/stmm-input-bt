/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   inputstrings.cc
 */

#include "inputstrings.h"

namespace stmi
{

void InputStrings::initKeyMap()
{
	// Generate:
	// from <stmm-input/hardwarekey.h> file copy all keys,
	// remove line 'HK_NULL = 0' (the first enum)
	// and transform each line:
	//	, HK_XXX = n
	// to:
	//	addKey(stmi::hk::HK_XXX, "XXX");
	// with:
	//   Find:     <^	, HK_([^ ]+).*$>
	//   Replace:  <	addKey(stmi::hk::HK_\1, "\1");>

	addKey(stmi::hk::HK_ESC, "ESC");
	addKey(stmi::hk::HK_1, "1");
	addKey(stmi::hk::HK_2, "2");
	addKey(stmi::hk::HK_3, "3");
	addKey(stmi::hk::HK_4, "4");
	addKey(stmi::hk::HK_5, "5");
	addKey(stmi::hk::HK_6, "6");
	addKey(stmi::hk::HK_7, "7");
	addKey(stmi::hk::HK_8, "8");
	addKey(stmi::hk::HK_9, "9");
	addKey(stmi::hk::HK_0, "0");
	addKey(stmi::hk::HK_MINUS, "MINUS");
	addKey(stmi::hk::HK_EQUAL, "EQUAL");
	addKey(stmi::hk::HK_BACKSPACE, "BACKSPACE");
	addKey(stmi::hk::HK_TAB, "TAB");
	addKey(stmi::hk::HK_Q, "Q");
	addKey(stmi::hk::HK_W, "W");
	addKey(stmi::hk::HK_E, "E");
	addKey(stmi::hk::HK_R, "R");
	addKey(stmi::hk::HK_T, "T");
	addKey(stmi::hk::HK_Y, "Y");
	addKey(stmi::hk::HK_U, "U");
	addKey(stmi::hk::HK_I, "I");
	addKey(stmi::hk::HK_O, "O");
	addKey(stmi::hk::HK_P, "P");
	addKey(stmi::hk::HK_LEFTBRACE, "LEFTBRACE");
	addKey(stmi::hk::HK_RIGHTBRACE, "RIGHTBRACE");
	addKey(stmi::hk::HK_ENTER, "ENTER");
	addKey(stmi::hk::HK_LEFTCTRL, "LEFTCTRL");
	addKey(stmi::hk::HK_A, "A");
	addKey(stmi::hk::HK_S, "S");
	addKey(stmi::hk::HK_D, "D");
	addKey(stmi::hk::HK_F, "F");
	addKey(stmi::hk::HK_G, "G");
	addKey(stmi::hk::HK_H, "H");
	addKey(stmi::hk::HK_J, "J");
	addKey(stmi::hk::HK_K, "K");
	addKey(stmi::hk::HK_L, "L");
	addKey(stmi::hk::HK_SEMICOLON, "SEMICOLON");
	addKey(stmi::hk::HK_APOSTROPHE, "APOSTROPHE");
	addKey(stmi::hk::HK_GRAVE, "GRAVE");
	addKey(stmi::hk::HK_LEFTSHIFT, "LEFTSHIFT");
	addKey(stmi::hk::HK_BACKSLASH, "BACKSLASH");
	addKey(stmi::hk::HK_Z, "Z");
	addKey(stmi::hk::HK_X, "X");
	addKey(stmi::hk::HK_C, "C");
	addKey(stmi::hk::HK_V, "V");
	addKey(stmi::hk::HK_B, "B");
	addKey(stmi::hk::HK_N, "N");
	addKey(stmi::hk::HK_M, "M");
	addKey(stmi::hk::HK_COMMA, "COMMA");
	addKey(stmi::hk::HK_DOT, "DOT");
	addKey(stmi::hk::HK_SLASH, "SLASH");
	addKey(stmi::hk::HK_RIGHTSHIFT, "RIGHTSHIFT");
	addKey(stmi::hk::HK_KPASTERISK, "KPASTERISK");
	addKey(stmi::hk::HK_LEFTALT, "LEFTALT");
	addKey(stmi::hk::HK_SPACE, "SPACE");
	addKey(stmi::hk::HK_CAPSLOCK, "CAPSLOCK");
	addKey(stmi::hk::HK_F1, "F1");
	addKey(stmi::hk::HK_F2, "F2");
	addKey(stmi::hk::HK_F3, "F3");
	addKey(stmi::hk::HK_F4, "F4");
	addKey(stmi::hk::HK_F5, "F5");
	addKey(stmi::hk::HK_F6, "F6");
	addKey(stmi::hk::HK_F7, "F7");
	addKey(stmi::hk::HK_F8, "F8");
	addKey(stmi::hk::HK_F9, "F9");
	addKey(stmi::hk::HK_F10, "F10");
	addKey(stmi::hk::HK_NUMLOCK, "NUMLOCK");
	addKey(stmi::hk::HK_SCROLLLOCK, "SCROLLLOCK");
	addKey(stmi::hk::HK_KP7, "KP7");
	addKey(stmi::hk::HK_KP8, "KP8");
	addKey(stmi::hk::HK_KP9, "KP9");
	addKey(stmi::hk::HK_KPMINUS, "KPMINUS");
	addKey(stmi::hk::HK_KP4, "KP4");
	addKey(stmi::hk::HK_KP5, "KP5");
	addKey(stmi::hk::HK_KP6, "KP6");
	addKey(stmi::hk::HK_KPPLUS, "KPPLUS");
	addKey(stmi::hk::HK_KP1, "KP1");
	addKey(stmi::hk::HK_KP2, "KP2");
	addKey(stmi::hk::HK_KP3, "KP3");
	addKey(stmi::hk::HK_KP0, "KP0");
	addKey(stmi::hk::HK_KPDOT, "KPDOT");

	addKey(stmi::hk::HK_ZENKAKUHANKAKU, "ZENKAKUHANKAKU");
	addKey(stmi::hk::HK_102ND, "102ND");
	addKey(stmi::hk::HK_F11, "F11");
	addKey(stmi::hk::HK_F12, "F12");
	addKey(stmi::hk::HK_RO, "RO");
	addKey(stmi::hk::HK_KATAKANA, "KATAKANA");
	addKey(stmi::hk::HK_HIRAGANA, "HIRAGANA");
	addKey(stmi::hk::HK_HENKAN, "HENKAN");
	addKey(stmi::hk::HK_KATAKANAHIRAGANA, "KATAKANAHIRAGANA");
	addKey(stmi::hk::HK_MUHENKAN, "MUHENKAN");
	addKey(stmi::hk::HK_KPJPCOMMA, "KPJPCOMMA");
	addKey(stmi::hk::HK_KPENTER, "KPENTER");
	addKey(stmi::hk::HK_RIGHTCTRL, "RIGHTCTRL");
	addKey(stmi::hk::HK_KPSLASH, "KPSLASH");
	addKey(stmi::hk::HK_SYSRQ, "SYSRQ");
	addKey(stmi::hk::HK_RIGHTALT, "RIGHTALT");
	addKey(stmi::hk::HK_LINEFEED, "LINEFEED");
	addKey(stmi::hk::HK_HOME, "HOME");
	addKey(stmi::hk::HK_UP, "UP");
	addKey(stmi::hk::HK_PAGEUP, "PAGEUP");
	addKey(stmi::hk::HK_LEFT, "LEFT");
	addKey(stmi::hk::HK_RIGHT, "RIGHT");
	addKey(stmi::hk::HK_END, "END");
	addKey(stmi::hk::HK_DOWN, "DOWN");
	addKey(stmi::hk::HK_PAGEDOWN, "PAGEDOWN");
	addKey(stmi::hk::HK_INSERT, "INSERT");
	addKey(stmi::hk::HK_DELETE, "DELETE");
	addKey(stmi::hk::HK_MACRO, "MACRO");
	addKey(stmi::hk::HK_MUTE, "MUTE");
	addKey(stmi::hk::HK_VOLUMEDOWN, "VOLUMEDOWN");
	addKey(stmi::hk::HK_VOLUMEUP, "VOLUMEUP");
	addKey(stmi::hk::HK_POWER, "POWER");
	addKey(stmi::hk::HK_KPEQUAL, "KPEQUAL");
	addKey(stmi::hk::HK_KPPLUSMINUS, "KPPLUSMINUS");
	addKey(stmi::hk::HK_PAUSE, "PAUSE");
	addKey(stmi::hk::HK_SCALE, "SCALE");

	addKey(stmi::hk::HK_KPCOMMA, "KPCOMMA");
	addKey(stmi::hk::HK_HANGEUL, "HANGEUL");
		//, HK_HANGUEL = HK_HANGEUL
	addKey(stmi::hk::HK_HANJA, "HANJA");
	addKey(stmi::hk::HK_YEN, "YEN");
	addKey(stmi::hk::HK_LEFTMETA, "LEFTMETA");
	addKey(stmi::hk::HK_RIGHTMETA, "RIGHTMETA");
	addKey(stmi::hk::HK_COMPOSE, "COMPOSE");

	addKey(stmi::hk::HK_STOP, "STOP");
	addKey(stmi::hk::HK_AGAIN, "AGAIN");
	addKey(stmi::hk::HK_PROPS, "PROPS");
	addKey(stmi::hk::HK_UNDO, "UNDO");
	addKey(stmi::hk::HK_FRONT, "FRONT");
	addKey(stmi::hk::HK_COPY, "COPY");
	addKey(stmi::hk::HK_OPEN, "OPEN");
	addKey(stmi::hk::HK_PASTE, "PASTE");
	addKey(stmi::hk::HK_FIND, "FIND");
	addKey(stmi::hk::HK_CUT, "CUT");
	addKey(stmi::hk::HK_HELP, "HELP");
	addKey(stmi::hk::HK_MENU, "MENU");
	addKey(stmi::hk::HK_CALC, "CALC");
	addKey(stmi::hk::HK_SETUP, "SETUP");
	addKey(stmi::hk::HK_SLEEP, "SLEEP");
	addKey(stmi::hk::HK_WAKEUP, "WAKEUP");
	addKey(stmi::hk::HK_FILE, "FILE");
	addKey(stmi::hk::HK_SENDFILE, "SENDFILE");
	addKey(stmi::hk::HK_DELETEFILE, "DELETEFILE");
	addKey(stmi::hk::HK_XFER, "XFER");
	addKey(stmi::hk::HK_PROG1, "PROG1");
	addKey(stmi::hk::HK_PROG2, "PROG2");
	addKey(stmi::hk::HK_WWW, "WWW");
	addKey(stmi::hk::HK_MSDOS, "MSDOS");
	addKey(stmi::hk::HK_SCREENLOCK, "SCREENLOCK");
		//, HK_COFFEE = HK_SCREENLOCK
	addKey(stmi::hk::HK_DIRECTION, "DIRECTION");
	addKey(stmi::hk::HK_CYCLEWINDOWS, "CYCLEWINDOWS");
	addKey(stmi::hk::HK_MAIL, "MAIL");
	addKey(stmi::hk::HK_BOOKMARKS, "BOOKMARKS");
	addKey(stmi::hk::HK_COMPUTER, "COMPUTER");
	addKey(stmi::hk::HK_BACK, "BACK");
	addKey(stmi::hk::HK_FORWARD, "FORWARD");
	addKey(stmi::hk::HK_CLOSECD, "CLOSECD");
	addKey(stmi::hk::HK_EJECTCD, "EJECTCD");
	addKey(stmi::hk::HK_EJECTCLOSECD, "EJECTCLOSECD");
	addKey(stmi::hk::HK_NEXTSONG, "NEXTSONG");
	addKey(stmi::hk::HK_PLAYPAUSE, "PLAYPAUSE");
	addKey(stmi::hk::HK_PREVIOUSSONG, "PREVIOUSSONG");
	addKey(stmi::hk::HK_STOPCD, "STOPCD");
	addKey(stmi::hk::HK_RECORD, "RECORD");
	addKey(stmi::hk::HK_REWIND, "REWIND");
	addKey(stmi::hk::HK_PHONE, "PHONE");
	addKey(stmi::hk::HK_ISO, "ISO");
	addKey(stmi::hk::HK_CONFIG, "CONFIG");
	addKey(stmi::hk::HK_HOMEPAGE, "HOMEPAGE");
	addKey(stmi::hk::HK_REFRESH, "REFRESH");
	addKey(stmi::hk::HK_EXIT, "EXIT");
	addKey(stmi::hk::HK_MOVE, "MOVE");
	addKey(stmi::hk::HK_EDIT, "EDIT");
	addKey(stmi::hk::HK_SCROLLUP, "SCROLLUP");
	addKey(stmi::hk::HK_SCROLLDOWN, "SCROLLDOWN");
	addKey(stmi::hk::HK_KPLEFTPAREN, "KPLEFTPAREN");
	addKey(stmi::hk::HK_KPRIGHTPAREN, "KPRIGHTPAREN");
	addKey(stmi::hk::HK_NEW, "NEW");
	addKey(stmi::hk::HK_REDO, "REDO");

	addKey(stmi::hk::HK_F13, "F13");
	addKey(stmi::hk::HK_F14, "F14");
	addKey(stmi::hk::HK_F15, "F15");
	addKey(stmi::hk::HK_F16, "F16");
	addKey(stmi::hk::HK_F17, "F17");
	addKey(stmi::hk::HK_F18, "F18");
	addKey(stmi::hk::HK_F19, "F19");
	addKey(stmi::hk::HK_F20, "F20");
	addKey(stmi::hk::HK_F21, "F21");
	addKey(stmi::hk::HK_F22, "F22");
	addKey(stmi::hk::HK_F23, "F23");
	addKey(stmi::hk::HK_F24, "F24");

	addKey(stmi::hk::HK_PLAYCD, "PLAYCD");
	addKey(stmi::hk::HK_PAUSECD, "PAUSECD");
	addKey(stmi::hk::HK_PROG3, "PROG3");
	addKey(stmi::hk::HK_PROG4, "PROG4");
	addKey(stmi::hk::HK_DASHBOARD, "DASHBOARD");
	addKey(stmi::hk::HK_SUSPEND, "SUSPEND");
	addKey(stmi::hk::HK_CLOSE, "CLOSE");
	addKey(stmi::hk::HK_PLAY, "PLAY");
	addKey(stmi::hk::HK_FASTFORWARD, "FASTFORWARD");
	addKey(stmi::hk::HK_BASSBOOST, "BASSBOOST");
	addKey(stmi::hk::HK_PRINT, "PRINT");
	addKey(stmi::hk::HK_HP, "HP");
	addKey(stmi::hk::HK_CAMERA, "CAMERA");
	addKey(stmi::hk::HK_SOUND, "SOUND");
	addKey(stmi::hk::HK_QUESTION, "QUESTION");
	addKey(stmi::hk::HK_EMAIL, "EMAIL");
	addKey(stmi::hk::HK_CHAT, "CHAT");
	addKey(stmi::hk::HK_SEARCH, "SEARCH");
	addKey(stmi::hk::HK_CONNECT, "CONNECT");
	addKey(stmi::hk::HK_FINANCE, "FINANCE");
	addKey(stmi::hk::HK_SPORT, "SPORT");
	addKey(stmi::hk::HK_SHOP, "SHOP");
	addKey(stmi::hk::HK_ALTERASE, "ALTERASE");
	addKey(stmi::hk::HK_CANCEL, "CANCEL");
	addKey(stmi::hk::HK_BRIGHTNESSDOWN, "BRIGHTNESSDOWN");
	addKey(stmi::hk::HK_BRIGHTNESSUP, "BRIGHTNESSUP");
	addKey(stmi::hk::HK_MEDIA, "MEDIA");

	addKey(stmi::hk::HK_SWITCHVIDEOMODE, "SWITCHVIDEOMODE");
	addKey(stmi::hk::HK_KBDILLUMTOGGLE, "KBDILLUMTOGGLE");
	addKey(stmi::hk::HK_KBDILLUMDOWN, "KBDILLUMDOWN");
	addKey(stmi::hk::HK_KBDILLUMUP, "KBDILLUMUP");

	addKey(stmi::hk::HK_SEND, "SEND");
	addKey(stmi::hk::HK_REPLY, "REPLY");
	addKey(stmi::hk::HK_FORWARDMAIL, "FORWARDMAIL");
	addKey(stmi::hk::HK_SAVE, "SAVE");
	addKey(stmi::hk::HK_DOCUMENTS, "DOCUMENTS");

	addKey(stmi::hk::HK_BATTERY, "BATTERY");

	addKey(stmi::hk::HK_BLUETOOTH, "BLUETOOTH");
	addKey(stmi::hk::HK_WLAN, "WLAN");
	addKey(stmi::hk::HK_UWB, "UWB");

	addKey(stmi::hk::HK_UNKNOWN, "UNKNOWN");

	addKey(stmi::hk::HK_VIDEO_NEXT, "VIDEO_NEXT");
	addKey(stmi::hk::HK_VIDEO_PREV, "VIDEO_PREV");
	addKey(stmi::hk::HK_BRIGHTNESS_CYCLE, "BRIGHTNESS_CYCLE");
	addKey(stmi::hk::HK_BRIGHTNESS_ZERO, "BRIGHTNESS_ZERO");
		//, HK_BRIGHTNESS_AUTO = HK_BRIGHTNESS_ZERO
	addKey(stmi::hk::HK_DISPLAY_OFF, "DISPLAY_OFF");

	addKey(stmi::hk::HK_WIMAX, "WIMAX");
		//, HK_WWAN = HK_WIMAX
	addKey(stmi::hk::HK_RFKILL, "RFKILL");

	addKey(stmi::hk::HK_MICMUTE, "MICMUTE");

		//addKey(stmi::hk::HK_BTN_MISC, "BTN_MISC");
	addKey(stmi::hk::HK_BTN_0, "BTN_0");
	addKey(stmi::hk::HK_BTN_1, "BTN_1");
	addKey(stmi::hk::HK_BTN_2, "BTN_2");
	addKey(stmi::hk::HK_BTN_3, "BTN_3");
	addKey(stmi::hk::HK_BTN_4, "BTN_4");
	addKey(stmi::hk::HK_BTN_5, "BTN_5");
	addKey(stmi::hk::HK_BTN_6, "BTN_6");
	addKey(stmi::hk::HK_BTN_7, "BTN_7");
	addKey(stmi::hk::HK_BTN_8, "BTN_8");
	addKey(stmi::hk::HK_BTN_9, "BTN_9");

		//, HK_BTN_MOUSE = 0x110
	addKey(stmi::hk::HK_BTN_LEFT, "BTN_LEFT");
	addKey(stmi::hk::HK_BTN_RIGHT, "BTN_RIGHT");
	addKey(stmi::hk::HK_BTN_MIDDLE, "BTN_MIDDLE");
	addKey(stmi::hk::HK_BTN_SIDE, "BTN_SIDE");
	addKey(stmi::hk::HK_BTN_EXTRA, "BTN_EXTRA");
	addKey(stmi::hk::HK_BTN_FORWARD, "BTN_FORWARD");
	addKey(stmi::hk::HK_BTN_BACK, "BTN_BACK");
	addKey(stmi::hk::HK_BTN_TASK, "BTN_TASK");

		//, HK_BTN_JOYSTICK = 0x120
	addKey(stmi::hk::HK_BTN_TRIGGER, "BTN_TRIGGER");
	addKey(stmi::hk::HK_BTN_THUMB, "BTN_THUMB");
	addKey(stmi::hk::HK_BTN_THUMB2, "BTN_THUMB2");
	addKey(stmi::hk::HK_BTN_TOP, "BTN_TOP");
	addKey(stmi::hk::HK_BTN_TOP2, "BTN_TOP2");
	addKey(stmi::hk::HK_BTN_PINKIE, "BTN_PINKIE");
	addKey(stmi::hk::HK_BTN_BASE, "BTN_BASE");
	addKey(stmi::hk::HK_BTN_BASE2, "BTN_BASE2");
	addKey(stmi::hk::HK_BTN_BASE3, "BTN_BASE3");
	addKey(stmi::hk::HK_BTN_BASE4, "BTN_BASE4");
	addKey(stmi::hk::HK_BTN_BASE5, "BTN_BASE5");
	addKey(stmi::hk::HK_BTN_BASE6, "BTN_BASE6");
	addKey(stmi::hk::HK_BTN_DEAD, "BTN_DEAD");

		//addKey(stmi::hk::HK_BTN_GAMEPAD, "BTN_GAMEPAD");
	addKey(stmi::hk::HK_BTN_A, "BTN_A");
		//, HK_BTN_SOUTH = HK_BTN_A
	addKey(stmi::hk::HK_BTN_B, "BTN_B");
		//, HK_BTN_EAST = HK_BTN_B
	addKey(stmi::hk::HK_BTN_C, "BTN_C");
	addKey(stmi::hk::HK_BTN_X, "BTN_X");
		//, HK_BTN_NORTH = HK_BTN_X
	addKey(stmi::hk::HK_BTN_Y, "BTN_Y");
		//, HK_BTN_WEST = HK_BTN_Y
	addKey(stmi::hk::HK_BTN_Z, "BTN_Z");
	addKey(stmi::hk::HK_BTN_TL, "BTN_TL");
	addKey(stmi::hk::HK_BTN_TR, "BTN_TR");
	addKey(stmi::hk::HK_BTN_TL2, "BTN_TL2");
	addKey(stmi::hk::HK_BTN_TR2, "BTN_TR2");
	addKey(stmi::hk::HK_BTN_SELECT, "BTN_SELECT");
	addKey(stmi::hk::HK_BTN_START, "BTN_START");
	addKey(stmi::hk::HK_BTN_MODE, "BTN_MODE");
	addKey(stmi::hk::HK_BTN_THUMBL, "BTN_THUMBL");
	addKey(stmi::hk::HK_BTN_THUMBR, "BTN_THUMBR");

		//addKey(stmi::hk::HK_BTN_DIGI, "BTN_DIGI");
	addKey(stmi::hk::HK_BTN_TOOL_PEN, "BTN_TOOL_PEN");
	addKey(stmi::hk::HK_BTN_TOOL_RUBBER, "BTN_TOOL_RUBBER");
	addKey(stmi::hk::HK_BTN_TOOL_BRUSH, "BTN_TOOL_BRUSH");
	addKey(stmi::hk::HK_BTN_TOOL_PENCIL, "BTN_TOOL_PENCIL");
	addKey(stmi::hk::HK_BTN_TOOL_AIRBRUSH, "BTN_TOOL_AIRBRUSH");
	addKey(stmi::hk::HK_BTN_TOOL_FINGER, "BTN_TOOL_FINGER");
	addKey(stmi::hk::HK_BTN_TOOL_MOUSE, "BTN_TOOL_MOUSE");
	addKey(stmi::hk::HK_BTN_TOOL_LENS, "BTN_TOOL_LENS");
	addKey(stmi::hk::HK_BTN_TOOL_QUINTTAP, "BTN_TOOL_QUINTTAP");
	addKey(stmi::hk::HK_BTN_TOUCH, "BTN_TOUCH");
	addKey(stmi::hk::HK_BTN_STYLUS, "BTN_STYLUS");
	addKey(stmi::hk::HK_BTN_STYLUS2, "BTN_STYLUS2");
	addKey(stmi::hk::HK_BTN_TOOL_DOUBLETAP, "BTN_TOOL_DOUBLETAP");
	addKey(stmi::hk::HK_BTN_TOOL_TRIPLETAP, "BTN_TOOL_TRIPLETAP");
	addKey(stmi::hk::HK_BTN_TOOL_QUADTAP, "BTN_TOOL_QUADTAP");

		//addKey(stmi::hk::HK_BTN_WHEEL, "BTN_WHEEL");
	addKey(stmi::hk::HK_BTN_GEAR_DOWN, "BTN_GEAR_DOWN");
	addKey(stmi::hk::HK_BTN_GEAR_UP, "BTN_GEAR_UP");

	addKey(stmi::hk::HK_OK, "OK");
	addKey(stmi::hk::HK_SELECT, "SELECT");
	addKey(stmi::hk::HK_GOTO, "GOTO");
	addKey(stmi::hk::HK_CLEAR, "CLEAR");
	addKey(stmi::hk::HK_POWER2, "POWER2");
	addKey(stmi::hk::HK_OPTION, "OPTION");
	addKey(stmi::hk::HK_INFO, "INFO");
	addKey(stmi::hk::HK_TIME, "TIME");
	addKey(stmi::hk::HK_VENDOR, "VENDOR");
	addKey(stmi::hk::HK_ARCHIVE, "ARCHIVE");
	addKey(stmi::hk::HK_PROGRAM, "PROGRAM");
	addKey(stmi::hk::HK_CHANNEL, "CHANNEL");
	addKey(stmi::hk::HK_FAVORITES, "FAVORITES");
	addKey(stmi::hk::HK_EPG, "EPG");
	addKey(stmi::hk::HK_PVR, "PVR");
	addKey(stmi::hk::HK_MHP, "MHP");
	addKey(stmi::hk::HK_LANGUAGE, "LANGUAGE");
	addKey(stmi::hk::HK_TITLE, "TITLE");
	addKey(stmi::hk::HK_SUBTITLE, "SUBTITLE");
	addKey(stmi::hk::HK_ANGLE, "ANGLE");
	addKey(stmi::hk::HK_ZOOM, "ZOOM");
	addKey(stmi::hk::HK_MODE, "MODE");
	addKey(stmi::hk::HK_KEYBOARD, "KEYBOARD");
	addKey(stmi::hk::HK_SCREEN, "SCREEN");
	addKey(stmi::hk::HK_PC, "PC");
	addKey(stmi::hk::HK_TV, "TV");
	addKey(stmi::hk::HK_TV2, "TV2");
	addKey(stmi::hk::HK_VCR, "VCR");
	addKey(stmi::hk::HK_VCR2, "VCR2");
	addKey(stmi::hk::HK_SAT, "SAT");
	addKey(stmi::hk::HK_SAT2, "SAT2");
	addKey(stmi::hk::HK_CD, "CD");
	addKey(stmi::hk::HK_TAPE, "TAPE");
	addKey(stmi::hk::HK_RADIO, "RADIO");
	addKey(stmi::hk::HK_TUNER, "TUNER");
	addKey(stmi::hk::HK_PLAYER, "PLAYER");
	addKey(stmi::hk::HK_TEXT, "TEXT");
	addKey(stmi::hk::HK_DVD, "DVD");
	addKey(stmi::hk::HK_AUX, "AUX");
	addKey(stmi::hk::HK_MP3, "MP3");
	addKey(stmi::hk::HK_AUDIO, "AUDIO");
	addKey(stmi::hk::HK_VIDEO, "VIDEO");
	addKey(stmi::hk::HK_DIRECTORY, "DIRECTORY");
	addKey(stmi::hk::HK_LIST, "LIST");
	addKey(stmi::hk::HK_MEMO, "MEMO");
	addKey(stmi::hk::HK_CALENDAR, "CALENDAR");
	addKey(stmi::hk::HK_RED, "RED");
	addKey(stmi::hk::HK_GREEN, "GREEN");
	addKey(stmi::hk::HK_YELLOW, "YELLOW");
	addKey(stmi::hk::HK_BLUE, "BLUE");
	addKey(stmi::hk::HK_CHANNELUP, "CHANNELUP");
	addKey(stmi::hk::HK_CHANNELDOWN, "CHANNELDOWN");
	addKey(stmi::hk::HK_FIRST, "FIRST");
	addKey(stmi::hk::HK_LAST, "LAST");
	addKey(stmi::hk::HK_AB, "AB");
	addKey(stmi::hk::HK_NEXT, "NEXT");
	addKey(stmi::hk::HK_RESTART, "RESTART");
	addKey(stmi::hk::HK_SLOW, "SLOW");
	addKey(stmi::hk::HK_SHUFFLE, "SHUFFLE");
	addKey(stmi::hk::HK_BREAK, "BREAK");
	addKey(stmi::hk::HK_PREVIOUS, "PREVIOUS");
	addKey(stmi::hk::HK_DIGITS, "DIGITS");
	addKey(stmi::hk::HK_TEEN, "TEEN");
	addKey(stmi::hk::HK_TWEN, "TWEN");
	addKey(stmi::hk::HK_VIDEOPHONE, "VIDEOPHONE");
	addKey(stmi::hk::HK_GAMES, "GAMES");
	addKey(stmi::hk::HK_ZOOMIN, "ZOOMIN");
	addKey(stmi::hk::HK_ZOOMOUT, "ZOOMOUT");
	addKey(stmi::hk::HK_ZOOMRESET, "ZOOMRESET");
	addKey(stmi::hk::HK_WORDPROCESSOR, "WORDPROCESSOR");
	addKey(stmi::hk::HK_EDITOR, "EDITOR");
	addKey(stmi::hk::HK_SPREADSHEET, "SPREADSHEET");
	addKey(stmi::hk::HK_GRAPHICSEDITOR, "GRAPHICSEDITOR");
	addKey(stmi::hk::HK_PRESENTATION, "PRESENTATION");
	addKey(stmi::hk::HK_DATABASE, "DATABASE");
	addKey(stmi::hk::HK_NEWS, "NEWS");
	addKey(stmi::hk::HK_VOICEMAIL, "VOICEMAIL");
	addKey(stmi::hk::HK_ADDRESSBOOK, "ADDRESSBOOK");
	addKey(stmi::hk::HK_MESSENGER, "MESSENGER");
	addKey(stmi::hk::HK_BRIGHTNESS_TOGGLE, "BRIGHTNESS_TOGGLE");
		//, HK_DISPLAYTOGGLE = HK_BRIGHTNESS_TOGGLE
	addKey(stmi::hk::HK_SPELLCHECK, "SPELLCHECK");
	addKey(stmi::hk::HK_LOGOFF, "LOGOFF");

	addKey(stmi::hk::HK_DOLLAR, "DOLLAR");
	addKey(stmi::hk::HK_EURO, "EURO");

	addKey(stmi::hk::HK_FRAMEBACK, "FRAMEBACK");
	addKey(stmi::hk::HK_FRAMEFORWARD, "FRAMEFORWARD");
	addKey(stmi::hk::HK_CONTEXT_MENU, "CONTEXT_MENU");
	addKey(stmi::hk::HK_MEDIA_REPEAT, "MEDIA_REPEAT");
	addKey(stmi::hk::HK_10CHANNELSUP, "10CHANNELSUP");
	addKey(stmi::hk::HK_10CHANNELSDOWN, "10CHANNELSDOWN");
	addKey(stmi::hk::HK_IMAGES, "IMAGES");

	addKey(stmi::hk::HK_DEL_EOL, "DEL_EOL");
	addKey(stmi::hk::HK_DEL_EOS, "DEL_EOS");
	addKey(stmi::hk::HK_INS_LINE, "INS_LINE");
	addKey(stmi::hk::HK_DEL_LINE, "DEL_LINE");

	addKey(stmi::hk::HK_FN, "FN");
	addKey(stmi::hk::HK_FN_ESC, "FN_ESC");
	addKey(stmi::hk::HK_FN_F1, "FN_F1");
	addKey(stmi::hk::HK_FN_F2, "FN_F2");
	addKey(stmi::hk::HK_FN_F3, "FN_F3");
	addKey(stmi::hk::HK_FN_F4, "FN_F4");
	addKey(stmi::hk::HK_FN_F5, "FN_F5");
	addKey(stmi::hk::HK_FN_F6, "FN_F6");
	addKey(stmi::hk::HK_FN_F7, "FN_F7");
	addKey(stmi::hk::HK_FN_F8, "FN_F8");
	addKey(stmi::hk::HK_FN_F9, "FN_F9");
	addKey(stmi::hk::HK_FN_F10, "FN_F10");
	addKey(stmi::hk::HK_FN_F11, "FN_F11");
	addKey(stmi::hk::HK_FN_F12, "FN_F12");
	addKey(stmi::hk::HK_FN_1, "FN_1");
	addKey(stmi::hk::HK_FN_2, "FN_2");
	addKey(stmi::hk::HK_FN_D, "FN_D");
	addKey(stmi::hk::HK_FN_E, "FN_E");
	addKey(stmi::hk::HK_FN_F, "FN_F");
	addKey(stmi::hk::HK_FN_S, "FN_S");
	addKey(stmi::hk::HK_FN_B, "FN_B");

	addKey(stmi::hk::HK_BRL_DOT1, "BRL_DOT1");
	addKey(stmi::hk::HK_BRL_DOT2, "BRL_DOT2");
	addKey(stmi::hk::HK_BRL_DOT3, "BRL_DOT3");
	addKey(stmi::hk::HK_BRL_DOT4, "BRL_DOT4");
	addKey(stmi::hk::HK_BRL_DOT5, "BRL_DOT5");
	addKey(stmi::hk::HK_BRL_DOT6, "BRL_DOT6");
	addKey(stmi::hk::HK_BRL_DOT7, "BRL_DOT7");
	addKey(stmi::hk::HK_BRL_DOT8, "BRL_DOT8");
	addKey(stmi::hk::HK_BRL_DOT9, "BRL_DOT9");
	addKey(stmi::hk::HK_BRL_DOT10, "BRL_DOT10");

	addKey(stmi::hk::HK_NUMERIC_0, "NUMERIC_0");
	addKey(stmi::hk::HK_NUMERIC_1, "NUMERIC_1");
	addKey(stmi::hk::HK_NUMERIC_2, "NUMERIC_2");
	addKey(stmi::hk::HK_NUMERIC_3, "NUMERIC_3");
	addKey(stmi::hk::HK_NUMERIC_4, "NUMERIC_4");
	addKey(stmi::hk::HK_NUMERIC_5, "NUMERIC_5");
	addKey(stmi::hk::HK_NUMERIC_6, "NUMERIC_6");
	addKey(stmi::hk::HK_NUMERIC_7, "NUMERIC_7");
	addKey(stmi::hk::HK_NUMERIC_8, "NUMERIC_8");
	addKey(stmi::hk::HK_NUMERIC_9, "NUMERIC_9");
	addKey(stmi::hk::HK_NUMERIC_STAR, "NUMERIC_STAR");
	addKey(stmi::hk::HK_NUMERIC_POUND, "NUMERIC_POUND");

	addKey(stmi::hk::HK_CAMERA_FOCUS, "CAMERA_FOCUS");
	addKey(stmi::hk::HK_WPS_BUTTON, "WPS_BUTTON");

	addKey(stmi::hk::HK_TOUCHPAD_TOGGLE, "TOUCHPAD_TOGGLE");
	addKey(stmi::hk::HK_TOUCHPAD_ON, "TOUCHPAD_ON");
	addKey(stmi::hk::HK_TOUCHPAD_OFF, "TOUCHPAD_OFF");

	addKey(stmi::hk::HK_CAMERA_ZOOMIN, "CAMERA_ZOOMIN");
	addKey(stmi::hk::HK_CAMERA_ZOOMOUT, "CAMERA_ZOOMOUT");
	addKey(stmi::hk::HK_CAMERA_UP, "CAMERA_UP");
	addKey(stmi::hk::HK_CAMERA_DOWN, "CAMERA_DOWN");
	addKey(stmi::hk::HK_CAMERA_LEFT, "CAMERA_LEFT");
	addKey(stmi::hk::HK_CAMERA_RIGHT, "CAMERA_RIGHT");

	addKey(stmi::hk::HK_ATTENDANT_ON, "ATTENDANT_ON");
	addKey(stmi::hk::HK_ATTENDANT_OFF, "ATTENDANT_OFF");
	addKey(stmi::hk::HK_ATTENDANT_TOGGLE, "ATTENDANT_TOGGLE");
	addKey(stmi::hk::HK_LIGHTS_TOGGLE, "LIGHTS_TOGGLE");

	addKey(stmi::hk::HK_BTN_DPAD_UP, "BTN_DPAD_UP");
	addKey(stmi::hk::HK_BTN_DPAD_DOWN, "BTN_DPAD_DOWN");
	addKey(stmi::hk::HK_BTN_DPAD_LEFT, "BTN_DPAD_LEFT");
	addKey(stmi::hk::HK_BTN_DPAD_RIGHT, "BTN_DPAD_RIGHT");

	addKey(stmi::hk::HK_ALS_TOGGLE, "ALS_TOGGLE");

	addKey(stmi::hk::HK_BUTTONCONFIG, "BUTTONCONFIG");
	addKey(stmi::hk::HK_TASKMANAGER, "TASKMANAGER");
	addKey(stmi::hk::HK_JOURNAL, "JOURNAL");
	addKey(stmi::hk::HK_CONTROLPANEL, "CONTROLPANEL");
	addKey(stmi::hk::HK_APPSELECT, "APPSELECT");
	addKey(stmi::hk::HK_SCREENSAVER, "SCREENSAVER");
	addKey(stmi::hk::HK_VOICECOMMAND, "VOICECOMMAND");

	addKey(stmi::hk::HK_BRIGHTNESS_MIN, "BRIGHTNESS_MIN");
	addKey(stmi::hk::HK_BRIGHTNESS_MAX, "BRIGHTNESS_MAX");

	addKey(stmi::hk::HK_KBDINPUTASSIST_PREV, "KBDINPUTASSIST_PREV");
	addKey(stmi::hk::HK_KBDINPUTASSIST_NEXT, "KBDINPUTASSIST_NEXT");
	addKey(stmi::hk::HK_KBDINPUTASSIST_PREVGROUP, "KBDINPUTASSIST_PREVGROUP");
	addKey(stmi::hk::HK_KBDINPUTASSIST_NEXTGROUP, "KBDINPUTASSIST_NEXTGROUP");
	addKey(stmi::hk::HK_KBDINPUTASSIST_ACCEPT, "KBDINPUTASSIST_ACCEPT");
	addKey(stmi::hk::HK_KBDINPUTASSIST_CANCEL, "KBDINPUTASSIST_CANCEL");

		//addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY, "BTN_TRIGGER_HAPPY");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY1, "BTN_TRIGGER_HAPPY1");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY2, "BTN_TRIGGER_HAPPY2");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY3, "BTN_TRIGGER_HAPPY3");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY4, "BTN_TRIGGER_HAPPY4");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY5, "BTN_TRIGGER_HAPPY5");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY6, "BTN_TRIGGER_HAPPY6");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY7, "BTN_TRIGGER_HAPPY7");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY8, "BTN_TRIGGER_HAPPY8");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY9, "BTN_TRIGGER_HAPPY9");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY10, "BTN_TRIGGER_HAPPY10");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY11, "BTN_TRIGGER_HAPPY11");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY12, "BTN_TRIGGER_HAPPY12");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY13, "BTN_TRIGGER_HAPPY13");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY14, "BTN_TRIGGER_HAPPY14");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY15, "BTN_TRIGGER_HAPPY15");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY16, "BTN_TRIGGER_HAPPY16");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY17, "BTN_TRIGGER_HAPPY17");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY18, "BTN_TRIGGER_HAPPY18");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY19, "BTN_TRIGGER_HAPPY19");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY20, "BTN_TRIGGER_HAPPY20");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY21, "BTN_TRIGGER_HAPPY21");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY22, "BTN_TRIGGER_HAPPY22");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY23, "BTN_TRIGGER_HAPPY23");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY24, "BTN_TRIGGER_HAPPY24");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY25, "BTN_TRIGGER_HAPPY25");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY26, "BTN_TRIGGER_HAPPY26");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY27, "BTN_TRIGGER_HAPPY27");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY28, "BTN_TRIGGER_HAPPY28");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY29, "BTN_TRIGGER_HAPPY29");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY30, "BTN_TRIGGER_HAPPY30");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY31, "BTN_TRIGGER_HAPPY31");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY32, "BTN_TRIGGER_HAPPY32");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY33, "BTN_TRIGGER_HAPPY33");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY34, "BTN_TRIGGER_HAPPY34");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY35, "BTN_TRIGGER_HAPPY35");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY36, "BTN_TRIGGER_HAPPY36");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY37, "BTN_TRIGGER_HAPPY37");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY38, "BTN_TRIGGER_HAPPY38");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY39, "BTN_TRIGGER_HAPPY39");
	addKey(stmi::hk::HK_BTN_TRIGGER_HAPPY40, "BTN_TRIGGER_HAPPY40");

	addKey(stmi::hk::HK_X_SCROLL_LEFT, "X_SCROLL_LEFT");
	addKey(stmi::hk::HK_X_SCROLL_RIGHT, "X_SCROLL_RIGHT");
}

} // namespace stmi
