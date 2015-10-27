/*
 *
 * ©K. D. Hedger. Sun 20 Sep 14:41:10 BST 2015 kdhedger68713@gmail.com

 * This file (globals.cpp) is part of LFSPanel.

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

#include "globals.h"

//prefs
char				*terminalCommand=NULL;
int					panelHeight=16;
int					panelWidth=-1;
const monitorStruct	*mons=NULL;
int					onMonitor=0;
char				*logoutCommand=NULL;
char				*restartCommand=NULL;
char				*shutdownCommand=NULL;
char				*rightGadgets=NULL;
char				*leftGadgets=NULL;
int					panelXPos=PANELCENTRE;
char				*desktopTheme=NULL;

int					refreshRate=1;
int					iconSize=16;

//panel window
LFSTK_windowClass	*mainwind=NULL;
int					rightOffset=0;
int					leftOffset=0;