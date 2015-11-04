/*
 *
 * ©K. D. Hedger. Mon 12 Oct 16:45:21 BST 2015 kdhedger68713@gmail.com

 * This file (windowList.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not,see <http://www.gnu.org/licenses/>.
 */

#include "windowlist.h"

LFSTK_menuButtonClass	*windowMenu=NULL;
LFSTK_menuButtonClass	*windowDeskMenu=NULL;
menuItemStruct			windowList[MAXWINDOWSINLIST];
menuItemStruct			windowDeskList[MAXWINDOWSINLIST];
char					windowBuffer[512];
int						windowDeskListCnt=0;
int						windowListCnt=0;
int						updateWindowCnt=0;
const char				*possibleError="Unknown";
int						currentDesktop;

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long	mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(mainwind->display,msg,False);
	event.xclient.window=win;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;

	XSendEvent(mainwind->display,mainwind->rootWindow,False,mask,&event);
}

bool windowMenuCB(void *p,void* ud)
{
	menuItemStruct	*menu=(menuItemStruct*)ud;
	Window			winid=0;
	unsigned long	desktop=0;

	if(menu==NULL)
		return(true);

	winid=(Window)(menu->userData);
	desktop=(unsigned long)menu->subMenuCnt;
	possibleError="Can't switch desktop";
	sendClientMessage(mainwind->rootWindow,"_NET_CURRENT_DESKTOP",desktop,0,0,0,0);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	return(true);
}

void resetMenus(void)
{
	for(int j=0; j<windowListCnt; j++)
		if(windowList[j].label!=NULL)
			{
				free((char*)windowList[j].label);
				windowList[j].label=NULL;
			}
	windowListCnt=-1;
}

int getCurrentDesktop(void)
{
	char	*desknum=NULL;

	desknum=mainwind->globalLib->LFSTK_oneLiner("%s","xprop -root |grep '_NET_CURRENT_DESKTOP(CARDINAL)'|head -n1|awk -F' = ' '{print $2}'");
	if(desknum!=NULL)
		{
			currentDesktop=atoi(desknum);
			free(desknum);
		}
	else
		currentDesktop=1;
}

void updateWindowMenu(void)
{
	FILE	*fp=NULL;

	updateWindowCnt++;
	if(updateWindowCnt>=WINDOWREFRESH)
		updateWindowCnt=0;
	else
		return;

	alarm(0);
	if(windowListCnt>-1)
		resetMenus();

	getCurrentDesktop();
	windowListCnt=0;
	fp=popen(WINHELPER,"r");
	if(fp!=NULL)
		{
			windowBuffer[0]=0;
			fgets(windowBuffer,511,fp);
			windowBuffer[strlen(windowBuffer)-1]=0;
			windowListCnt=atoi(windowBuffer);
			if(windowListCnt>MAXWINDOWSINLIST)
				windowListCnt=MAXWINDOWSINLIST;

			windowDeskListCnt=0;

			for(int j=0; j<windowListCnt; j++)
				{
//full window
					windowBuffer[0]=0;
					fgets(windowBuffer,511,fp);//id
					windowBuffer[strlen(windowBuffer)-1]=0;
					windowList[j].userData=(void*)strtol(windowBuffer,NULL,16);
//this desk
					windowDeskList[windowDeskListCnt].userData=(void*)strtol(windowBuffer,NULL,16);

//full window
					windowBuffer[0]=0;
					fgets(windowBuffer,511,fp);//name
					windowBuffer[strlen(windowBuffer)-1]=0;
					windowList[j].label=strdup(windowBuffer);
//this desk
					windowDeskList[windowDeskListCnt].label=strdup(windowBuffer);

//full window
					windowBuffer[0]=0;
					fgets(windowBuffer,511,fp);//desktop -- TODO
					windowBuffer[strlen(windowBuffer)-1]=0;
					windowList[j].bc=NULL;
					windowList[j].subMenus=NULL;
					windowList[j].subMenuCnt=atoi(windowBuffer);
//this desk
					if(atoi(windowBuffer)==currentDesktop)
						{
							windowDeskList[windowDeskListCnt].bc=NULL;
							windowDeskList[windowDeskListCnt].subMenus=NULL;
							windowDeskList[windowDeskListCnt].subMenuCnt=atoi(windowBuffer);
							windowDeskListCnt++;
						}
					else
						{
							if(windowDeskList[windowDeskListCnt].label!=NULL)
								free((char*)windowDeskList[windowDeskListCnt].label);
								windowDeskList[windowDeskListCnt].label=NULL;
						}
				}
			pclose(fp);
			if(windowMenu!=NULL)
				windowMenu->LFSTK_updateMenus(windowList,windowListCnt);
			if(windowDeskMenu!=NULL)
				{
					if(windowDeskListCnt>0)
						{
							windowDeskMenu->LFSTK_updateMenus(windowDeskList,windowDeskListCnt);
							windowDeskMenu->LFSTK_setActive(true);
						}
					else
						windowDeskMenu->LFSTK_setActive(false);
					windowDeskMenu->LFSTK_clearWindow();
				}
		}
	alarm(refreshRate);
}

int addWindowDeskMenu(int x,int y,int grav,bool fromleft)
{
	const char	*icon=NULL;
	int			xpos=x;
	int			ypos=y;
	int			width=0;
	int			height=0;
	int			thisgrav=grav;
	int			iconsize=16;


	if(windowDeskMenu!=NULL)
		{
			printError("Duplicate current desktop window selector");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	windowDeskMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
	if(icon!=NULL)
		windowDeskMenu->LFSTK_setIconFromPath(icon,iconsize);
	else
		windowDeskMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",iconsize);
	windowDeskMenu->LFSTK_setCallBack(NULL,windowMenuCB,NULL);

	windowDeskListCnt=-1;
	windowListCnt=-1;
	updateWindowCnt=WINDOWREFRESH;
	updateWindowMenu();
	useAlarm=true;
	return(width);
}

int addWindowMenu(int x,int y,int grav,bool fromleft)
{
	const char	*icon=NULL;
	int			xpos=x;
	int			ypos=y;
	int			width=0;
	int			height=0;
	int			thisgrav=grav;
	int			iconsize=16;

	if(windowMenu!=NULL)
		{
			printError("Duplicate window selector");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	windowMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"computer","");
	if(icon!=NULL)
		windowMenu->LFSTK_setIconFromPath(icon,iconsize);
	else
		windowMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",iconsize);
	windowMenu->LFSTK_setCallBack(NULL,windowMenuCB,NULL);

	windowDeskListCnt=-1;
	windowListCnt=-1;
	updateWindowCnt=WINDOWREFRESH;
	updateWindowMenu();
	useAlarm=true;
	return(width);
}

