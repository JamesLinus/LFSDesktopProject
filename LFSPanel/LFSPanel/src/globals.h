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

#ifndef _GLOBALS_
#define _GLOBALS_

//prefs
extern	char				*terminalCommand;
extern	int					panelHeight;
extern	int					panelWidth;
extern	const monitorStruct	*mons;
extern	int					onMonitor;

#endif
