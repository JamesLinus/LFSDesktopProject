/*
 *
 * ©K. D. Hedger. Sun 20 Sep 15:05:07 BST 2015 kdhedger68713@gmail.com

 * This file (appmenu.h) is part of LFSPanel.

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
 
#ifndef _APPMENU_
#define _APPMENU_

#include "globals.h"

#define BUFFERSIZE 2048
#define MAXCATS 14
#define MAXENTRYS 500

struct menuEntryStruct
{
	char				*name;
	char				*exec;
	bool				inTerm;
};

extern LFSTK_menuButtonClass	*appButton;

int addAppmenu(int x,int y,int grav);

#endif
