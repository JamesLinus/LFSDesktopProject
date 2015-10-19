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

bool callback(void *p,void* ud)
{
	char			*path;
	geometryStruct	*geom=appButton->LFSTK_getGeom();

	XFlush(mainwind->display);
	asprintf(&path,"%s \"%s\" %i %i",APPMENUPATH,terminalCommand,geom->x,geom->y+geom->h);
	system(path);
	free(path);
	delete geom;
	return(true);
}

int addAppmenu(int x,int y,int grav)
{
	int addto=mainwind->font->ascent+mainwind->font->descent+8;
	int maxwid=0;
	int	catnum=0;
	int	xpos=0;
	int width=panelHeight+2;
	int	retval=width;

	if(appButton!=NULL)
		{
			printError("Duplicate app menu");
			return(0);
		}

	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	appButton=new LFSTK_buttonClass(mainwind,"",xpos,0,panelHeight+2,panelHeight,grav);
	appButton->LFSTK_setIconFromPath("/usr/share/pixmaps/LFSTux.png",panelHeight-6);
	XMapWindow(mainwind->display,appButton->LFSTK_getWindow());
	appButton->LFSTK_setCallBack(callback,NULL,NULL);
	return(retval);
}
