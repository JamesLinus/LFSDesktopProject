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

#include "globals.h"
#include "appmenu.h"
#include "logout.h"
#include "clock.h"
#include "disks.h"
#include "cpu.h"

#define RCNAME "lfspanel"

bool	mainLoop=true;

args	panelPrefs[]=
{
	{"panelheight",TYPEINT,&panelHeight},
	{"panelwidth",TYPEINT,&panelWidth},
	{"onmonitor",TYPEINT,&onMonitor},
	{"termcommand",TYPESTRING,&terminalCommand},
	{"logoutcommand",TYPESTRING,&logoutCommand},
	{"restartcommand",TYPESTRING,&restartCommand},
	{"shutdowncommand",TYPESTRING,&shutdownCommand},
	{"gadgetsright",TYPESTRING,&rightGadgets},
	{"gadgetsleft",TYPESTRING,&leftGadgets},
	{"panelxpos",TYPEINT,&panelXPos},
	{NULL,0,NULL}
};

void addLeftGadgets(void)
{
	int	offset=mons->x+leftOffset;

	for(int j=0;j<strlen(leftGadgets);j++)
		{
			switch(leftGadgets[j])
				{
					case 'A':
						offset+=addAppmenu(offset,mons->y,NorthWestGravity);
						break;
					case 'L':
						offset+=addLogout(offset,mons->y,NorthWestGravity);
						break;
					case 'C':
						offset+=addClock(offset,mons->y,NorthWestGravity);
						break;
					case 'D':
						offset+=addDiskData(offset,mons->y,NorthWestGravity);
						break;
					case 'M':
						offset+=addCpuData(offset,mons->y,NorthWestGravity);
						break;
				}
		}
	leftOffset=offset;
}

void addRightGadgets(void)
{
	geometryStruct *geom=mainwind->LFSTK_getGeom();
//	int	offset=geom->w-rightOffset;
	int	offset=-rightOffset;
	for(int j=0;j<strlen(rightGadgets);j++)
		{
			switch(rightGadgets[j])
				{
					case 'A':
						offset-=addAppmenu(offset,mons->y,NorthEastGravity);
						break;
					case 'L':
						offset-=addLogout(offset,mons->y,NorthEastGravity);
						break;
					case 'C':
						offset-=addClock(offset,mons->y,NorthEastGravity);
						break;
					case 'D':
						offset-=addDiskData(offset-BWIDTH,mons->y,NorthEastGravity);
						break;
					case 'M':
						offset-=addCpuData(offset,mons->y,NorthEastGravity);
						break;
				}
		}
	rightOffset=offset;
}

int main(int argc, char **argv)
{
	char			*env;
	XEvent			event;
	geometryStruct	*geom;

	terminalCommand=strdup("xterm -e ");
	logoutCommand=strdup("xterm");
	restartCommand=strdup("xterm");
	shutdownCommand=strdup("xterm");
	leftGadgets=strdup("");
	rightGadgets=strdup("");
	panelXPos=PANELCENTRE;

	mainwind=new LFSTK_windowClass(0,0,1,1,"lfs",true);

	if(argc>1)
		asprintf(&env,"%s/.config/LFS/%s-%s.rc",getenv("HOME"),RCNAME,argv[1]);
	else	
		asprintf(&env,"%s/.config/LFS/%s.rc",getenv("HOME"),RCNAME);
	mainwind->globalLib->LFSTK_loadVarsFromFile(env,panelPrefs);

	mons=mainwind->LFSTK_getMonitorData(onMonitor);

	rightOffset=BWIDTH;
	leftOffset=0;

	addLeftGadgets();
	addRightGadgets();

	switch(panelWidth)
		{
			case PANELFULL:
				panelXPos=mons->x;
				mainwind->LFSTK_resizeWindow(mons->w,panelHeight);
				break;
			case PANELSHRINK:
				mainwind->LFSTK_resizeWindow(leftOffset+abs(rightOffset)-BWIDTH,panelHeight);
				break;
			default:
				mainwind->LFSTK_resizeWindow(panelWidth,panelHeight);
		}

	geom=mainwind->LFSTK_getGeom();
	switch(panelXPos)
		{
			case PANELLEFT:
				mainwind->LFSTK_moveWindow(mons->x,mons->y,true);
				break;
			case PANELCENTRE:
				mainwind->LFSTK_moveWindow((mons->w/2)-((geom->w)/2),mons->y,true);
				break;
			case PANELRIGHT:
				mainwind->LFSTK_moveWindow(mons->w-(geom->w),mons->y,true);
				break;
			default:
				mainwind->LFSTK_moveWindow(panelXPos,mons->y,true);
		}

	mainwind->LFSTK_showWindow(true);
	mainwind->LFSTK_setKeepAbove(true);
	delete geom;

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
					break;
				case ConfigureNotify:
						//printf("here\n");
					mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
					break;
				default:
					//	printf("here\n");

					break;
				}
		}

	free(env);
	free(terminalCommand);
	delete mainwind;
	delete appButton;
	return 0;
}