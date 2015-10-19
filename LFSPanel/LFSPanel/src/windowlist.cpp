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
menuItemStruct			windowList[MAXWINDOWSINLIST];
char					windowBuffer[512];
int						windowListCnt=0;
int						updateWindowCnt=0;
const char				*possibleError="Unknown";

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

			for(int j=0; j<windowListCnt; j++)
				{
					windowBuffer[0]=0;
					fgets(windowBuffer,511,fp);//id
					windowBuffer[strlen(windowBuffer)-1]=0;
					windowList[j].userData=(void*)strtol(windowBuffer,NULL,16);

					windowBuffer[0]=0;
					fgets(windowBuffer,511,fp);//name
					windowBuffer[strlen(windowBuffer)-1]=0;
					windowList[j].label=strdup(windowBuffer);

					windowBuffer[0]=0;
					fgets(windowBuffer,511,fp);//desktop -- TODO
					windowBuffer[strlen(windowBuffer)-1]=0;
					windowList[j].bc=NULL;
					windowList[j].subMenus=NULL;
					windowList[j].subMenuCnt=atoi(windowBuffer);
				}
			pclose(fp);
			windowMenu->LFSTK_updateMenus(windowList,windowListCnt);
		}
	alarm(refreshRate);
}

int addWindowMenu(int x,int y,int grav)
{
	int	xpos=0;
	int width=panelHeight+6;
	int	retval=width;

	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	windowMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,0,width,panelHeight,grav);
	windowMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",panelHeight-6);
	windowMenu->LFSTK_setCallBack(NULL,windowMenuCB,NULL);

	windowListCnt=-1;
	updateWindowCnt=WINDOWREFRESH;
	updateWindowMenu();
	return(retval);
}

