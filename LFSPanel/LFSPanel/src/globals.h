/*
 *
 * ©K. D. Hedger. Sun 20 Sep 14:41:04 BST 2015 kdhedger68713@gmail.com

 * This file (globals.h) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include <LFSTKMenuButton.h>
#include <LFSTKLineEdit.h>
#include <LFSTKLabel.h>
#include <LFSTKLib.h>

#ifndef _GLOBALS_
#define _GLOBALS_

#define WINHELPER DATADIR "/scripts/PanelHelperWinList"
//#define WINHELPER "/media/LinuxData/Development/Projects/LFSDesktopProject/LFSPanel/LFSPanel/resources/scripts/PanelHelperWinList"
#define BWIDTH 64
#define SPACING 10
#define WINDOWREFRESH 2

enum PANELXPOS {PANELLEFT=-1,PANELCENTRE=-2,PANELRIGHT=-3};
enum PANELYPOS {PANELTOP=-1,PANELBOTTOM=-3};
enum PANELSIZE {PANELFULL=-1,PANELSHRINK=-2};
enum PANELGRAVITY {PANELABS=0,PANELNORTH,PANELEAST,PANELSOUTH,PANELWEST};

//prefs
extern char					*terminalCommand;
extern int					panelHeight;
extern int					panelWidth;
extern const monitorStruct	*mons;
extern int					onMonitor;
extern char					*logoutCommand;
extern char					*restartCommand;
extern char					*shutdownCommand;
extern char					*rightGadgets;
extern char					*leftGadgets;
extern int					panelPos;
extern int					panelGravity;
extern char					*desktopTheme;
extern const char			*panelID;

extern int					refreshRate;
extern int					iconSize;
extern bool					useAlarm;

//panel window
extern LFSTK_windowClass	*mainwind;
extern int					rightOffset;
extern int					leftOffset;

//atoms
extern Atom					WM_STATE;
extern Atom					NET_WM_WINDOW_TYPE_NORMAL;
extern Atom					NET_WM_WINDOW_TYPE_DIALOG;
extern Atom					NET_WM_DESKTOP;

void printError(const char *err);
void setSizes(int *x,int *y,int *w,int *h,int *size,int *grav,bool fromleft);

#endif
