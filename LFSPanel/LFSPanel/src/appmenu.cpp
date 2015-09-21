/*
 *
 * ©K. D. Hedger. Fri 31 Jul 17:35:44 BST 2015 kdhedger68713@gmail.com

 * This file (appMenu.cpp) is part of LFSWManager.

 * LFSWManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSWManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWManager.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include <LFSTKMenuButton.h>

#include "appmenu.h"

LFSTK_buttonClass	*appButton=NULL;
int					appx,appy;

bool callback(void *p,void* ud)
{
	char *path;

	appButton->LFSTK_drawLabel(NORMALCOLOUR);
	asprintf(&path,"%s \"%s\" %i %i",APPMENUPATH,terminalCommand,appx,appy);
	system(path);
	free(path);
	return(true);
}

void addAppmenu(int x,int y)
{
	int addto=mainwind->font->ascent+mainwind->font->descent+8;
	int maxwid=0;
	int	catnum=0;

	appx=x;
	appy=y+panelHeight;

	appButton=new LFSTK_buttonClass(mainwind,"LFS",0,0,32,panelHeight,NorthWestGravity);
	XMapWindow(mainwind->display,appButton->LFSTK_getWindow());
	appButton->LFSTK_setCallBack(callback,NULL,NULL);
}
