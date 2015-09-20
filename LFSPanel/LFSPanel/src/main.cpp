/*
 *
 * ©K. D. Hedger. Mon  7 Sep 13:20:24 BST 2015 kdhedger68713@gmail.com

 * This file (lfswmprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
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

#include "globals.h"

bool	mainLoop=true;

args	panelPrefs[]=
{
	{"panelheight",TYPEINT,&panelHeight},
	{"panelwidth",TYPEINT,&panelWidth},
	{"onmonitor",TYPEINT,&onMonitor},
	{"termcommand",TYPESTRING,&terminalCommand},
	{NULL,0,NULL}
};

LFSTK_windowClass *mainwind=NULL;

int main(int argc, char **argv)
{
	char	*env;
	XEvent	event;

	mainwind=new LFSTK_windowClass(0,0,1,1,"lfs panel",true);
	mainwind->LFSTK_setDecorated(false);
	asprintf(&env,"%s/.config/LFS/lfspanel.rc",getenv("HOME"));
	mainwind->globalLib->LFSTK_loadVarsFromFile(env,panelPrefs);

	mons=mainwind->LFSTK_getMonitorData(onMonitor);
			printf(">>mon=%i %i %i %i %i<<\n",onMonitor,mons->x,mons->y,mons->w,mons->h);
	if(panelWidth==-1)
		mainwind->LFSTK_resizeWindow(mons->w,panelHeight);
	else
		mainwind->LFSTK_resizeWindow(panelWidth,panelHeight);

	mainwind->LFSTK_moveWindow(mons->x,mons->y);
	mainwind->LFSTK_showWindow();
	mainwind->LFSTK_setKeepAbove(true);

	for(int j=0;j<mainwind->LFSTK_getMonitorCount();j++)
		{
			mons=mainwind->LFSTK_getMonitorData(j);
			printf(">>mon=%i %i %i %i %i<<\n",j,mons->x,mons->y,mons->w,mons->h);
		}

	mainLoop=true;
	while(mainLoop==true)
		{
			listener *l=mainwind->LFSTK_getListener(event.xany.window);

			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(mainwind->display,&event);
			switch(event.type)
				{
					break;
				case Expose:
					mainwind->LFSTK_setActive(true);
					//callback(NULL,(void*)APPLY);
					break;
				case ConfigureNotify:
					mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
					break;
				default:
					break;
				}
		}

	free(env);
	return 0;
}