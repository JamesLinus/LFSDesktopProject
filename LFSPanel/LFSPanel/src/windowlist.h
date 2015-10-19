/*
 *
 * ©K. D. Hedger. Mon 12 Oct 16:45:32 BST 2015 kdhedger68713@gmail.com

 * This file (windowList.h) is part of LFSPanel.

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


#ifndef _WINDOWLIST_
#define _WINDOWLIST_

#include "globals.h"

#define MAXWINDOWSINLIST 512

extern LFSTK_menuButtonClass	*windowMenu;
extern const char				*possibleError;
extern char						*windowListIcon;

int addWindowMenu(int x,int y,int grav);
void updateWindowMenu(void);
void resetMenus(void);

#endif
