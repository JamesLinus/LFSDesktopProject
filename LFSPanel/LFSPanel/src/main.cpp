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
#include <signal.h>

#include "globals.h"
#include "appmenu.h"
#include "logout.h"
#include "clock.h"
#include "disks.h"
#include "cpu.h"
#include "windowlist.h"

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
	{"panelpos",TYPEINT,&panelPos},
	{"panelgrav",TYPEINT,&panelGravity},
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
						offset+=addAppmenu(offset,mons->y,panelGravity,true);
						break;
					case 'W':
						offset+=addWindowMenu(offset,mons->y,panelGravity,true);
						break;
					case 'w':
						offset+=addWindowDeskMenu(offset,mons->y,panelGravity,true);
						break;
					case 'L':
						offset+=addLogout(offset,mons->y,panelGravity,true);
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
					case 'S':
						offset+=SPACING;
						break;
				}
		}
	leftOffset=offset;
}

void addRightGadgets(void)
{
	int	offset=rightOffset;
	for(int j=strlen(rightGadgets)-1;j>=0;j--)
		{
			switch(rightGadgets[j])
				{
					case 'A':
						offset-=addAppmenu(offset,mons->y,panelGravity,false);
						break;
					case 'W':
						offset-=addWindowMenu(offset,mons->y,panelGravity,false);
						break;
					case 'w':
						offset-=addWindowDeskMenu(offset,mons->y,panelGravity,false);
						break;
					case 'L':
						offset-=addLogout(offset,mons->y,panelGravity,false);
						break;
					case 'C':
						offset-=addClock(offset,mons->y,NorthEastGravity);
						break;
					case 'D':
						offset-=addDiskData(offset,mons->y,NorthEastGravity);
						break;
					case 'M':
						offset-=addCpuData(offset,mons->y,NorthEastGravity);
						break;
					case 'S':
						offset-=SPACING;
						break;
				}
		}
	rightOffset=offset;
}

int errHandler(Display *dpy,XErrorEvent *e)
{
	char buf[128];

	buf[0]=0;
	XGetErrorText(dpy,e->error_code,buf,sizeof buf);
	fprintf(stderr,"Xlib Error: %s - %s\n",buf,possibleError);
	return 0;
}

void printError(const char *err)
{
	fprintf(stderr,">>>%s<<<\n",err);
}

void  alarmCallBack(int sig)
{
	if(clockButton!=NULL)
		updateClock();

	if(diskButton!=NULL)
		updateDiskStats();

	if(cpuButton!=NULL)
		updateCpuStats();

	if((windowMenu!=NULL) || (windowDeskMenu!=NULL))
		updateWindowMenu();

	signal(SIGALRM,SIG_IGN);
	signal(SIGALRM,alarmCallBack);
	XFlush(mainwind->display);
	alarm(refreshRate);
}

int main(int argc, char **argv)
{
	char			*env;
	XEvent			event;
	geometryStruct	*geom;
	fontStruct		*tfont;
	const char		*itemfont;
	int				psize;
	int				thold;
	int				px,py;

	terminalCommand=strdup("xterm -e ");
	logoutCommand=strdup("xterm");
	restartCommand=strdup("xterm");
	shutdownCommand=strdup("xterm");
	leftGadgets=strdup("A");
	rightGadgets=strdup("L");
	panelPos=PANELCENTRE;

	XSetErrorHandler(errHandler);

	mainwind=new LFSTK_windowClass(0,0,1,1,"lfs",true);

	itemfont=mainwind->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	tfont=mainwind->globalLib->LFSTK_loadFont(mainwind->display,mainwind->screen,itemfont);
	iconSize=tfont->size;
	free(tfont);

	if(argc>1)
		asprintf(&env,"%s/.config/LFS/%s-%s.rc",getenv("HOME"),RCNAME,argv[1]);
	else	
		asprintf(&env,"%s/.config/LFS/%s.rc",getenv("HOME"),RCNAME);
	mainwind->globalLib->LFSTK_loadVarsFromFile(env,panelPrefs);

	desktopTheme=mainwind->globalLib->LFSTK_oneLiner("cat %s/.config/LFS/lfsdesktop.rc|grep icontheme|awk '{print $2}'",getenv("HOME"));
	mons=mainwind->LFSTK_getMonitorData(onMonitor);

	rightOffset=0;
	leftOffset=0;

	signal(SIGALRM,alarmCallBack);
	addLeftGadgets();
	addRightGadgets();


	if((leftOffset==0) && (rightOffset==0))
		{
			fprintf(stderr,"Not using empty panel ...\n");
			exit(0);
		}

	alarm(refreshRate);

	psize=leftOffset+abs(rightOffset);

//	printf(">>psize %i<<\n",psize);
//	printf(">>panelWidth %i<<\n",panelWidth);
//	printf(">>panelHeight %i<<\n",panelHeight);
//	printf(">>panelGravity %i<<\n",panelGravity);

	px=mons->x;
	py=mons->y;
	switch(panelGravity)
		{
			case PANELSOUTH:
				py=mons->y+mons->h-panelHeight;
			case PANELNORTH:
				switch(panelWidth)
					{
						case PANELFULL:
							panelWidth=mons->w;
							panelPos=PANELLEFT;
							break;
						case PANELSHRINK:
							panelWidth=psize;
							break;
					}
				switch(panelPos)
					{
						case PANELLEFT:
							px=mons->x;
							break;
						case PANELCENTRE:
							px=((mons->w/2)-(psize/2))+mons->x;
							break;
						case PANELRIGHT:
							px=mons->x+mons->w-psize;
							break;
					}
				break;

			case PANELEAST:
				px=mons->x+mons->w-panelHeight;
			case PANELWEST:
				switch(panelWidth)
					{
						case PANELFULL:
							panelWidth=panelHeight;
							panelHeight=mons->h;
							panelPos=PANELLEFT;
							break;
						case PANELSHRINK:
							panelWidth=panelHeight;
							panelHeight=psize;
							break;
						default:
							thold=panelWidth;
							panelWidth=panelHeight;
							panelHeight=thold;
							break;
					}
				switch(panelPos)
					{
						case PANELLEFT:
							py=mons->y;
							break;
						case PANELCENTRE:
							py=((mons->h/2)-(panelHeight/2))+mons->y;
							break;
						case PANELRIGHT:
							py=mons->y+mons->h-panelHeight;
							break;
					}
				break;
		}

//	printf("-------\n");
//	printf(">>psize %i<<\n",psize);
//	printf(">>panelWidth %i<<\n",panelWidth);
//	printf(">>panelHeight %i<<\n",panelHeight);
//	printf(">>panelGravity %i<<\n",panelGravity);
//	printf(">>px %i py %i<<\n",px,py);

	mainwind->LFSTK_resizeWindow(panelWidth,panelHeight,true);
	mainwind->LFSTK_moveWindow(px,py,true);
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
					case Expose:
						mainwind->LFSTK_setActive(true);
						break;
					case ConfigureNotify:
						mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
				}
		}

	free(env);
	free(terminalCommand);
	if(appButton!=NULL)
		delete appButton;
	if(logoutButton!=NULL)
		delete logoutButton;
	if(clockButton!=NULL)
		delete clockButton;
	if(diskButton!=NULL)
		delete diskButton;
	if(cpuButton!=NULL)
		delete cpuButton;
	if(windowMenu!=NULL)
		delete windowMenu;
	if(windowDeskMenu!=NULL)
		delete windowDeskMenu;

	if(desktopTheme!=NULL)
		free(desktopTheme);

	delete mainwind;
	return 0;
}