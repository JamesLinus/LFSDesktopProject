/*
 *
 * ©K. D. Hedger. Sun 20 Sep 18:36:44 BST 2015 kdhedger68713@gmail.com

 * This file (logout.cpp) is part of LFSPanel.

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

#include "logout.h"

LFSTK_menuButtonClass	*logoutButton=NULL;
menuItemStruct			*logoutItems;
const char				*logoutLabels[]={"Logout","Restart","Shutdown"};
const char				*logoutImages[]={DATADIR "/pixmaps/logout.png",DATADIR "/pixmaps/reboot.png",DATADIR "/pixmaps/shutdown.png"};
const char				*logoutIconNames[]={"system-log-out","system-restart","system-shutdown","application-exit"};

Pixmap					pix[3][2];
char					*logoutIcons[NUMLOGOUTENTRYS+1];

bool logoutCB(void *p,void* ud)
{
	menuItemStruct	*menu=(menuItemStruct*)ud;

	if(menu==NULL)
		return(true);;

	switch((long)menu->userData)
		{
			case LOGOUT:
				system(logoutCommand);
				break;
			case RESTART:
				system(restartCommand);
				break;
			case SHUTDOWN:
				system(shutdownCommand);
				break;
		}
	return(true);
}

int  addLogout(int x,int y,int grav,bool fromleft)
{
	const char	*themedicon=NULL;
	const char	*icon=NULL;
	int			xpos=x;
	int			ypos=y;
	int			width=0;
	int			height=0;
	int			thisgrav=grav;
	int			iconsize=16;

	if(logoutButton!=NULL)
		{
			printError("Duplicate logout");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	logoutButton=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,logoutIconNames[NUMLOGOUTENTRYS],"");
	if(icon!=NULL)
//		logoutButton->LFSTK_setIconFromPath(icon,iconsize);
		logoutButton->LFSTK_setImageFromPath(icon,iconsize,iconsize);
	else
		logoutButton->LFSTK_setIconFromPath(DATADIR "/pixmaps/exit.png",iconsize);

	logoutButton->LFSTK_setCallBack(logoutCB,NULL,NULL);
	logoutItems=new menuItemStruct[NUMLOGOUTENTRYS];

	for(int j=LOGOUT;j<NUMLOGOUTENTRYS;j++)
		{
			logoutItems[j].label=strdup(logoutLabels[j]);
			logoutItems[j].userData=(void*)(long)j;
			logoutItems[j].bc=NULL;
			logoutItems[j].subMenus=NULL;
			logoutItems[j].subMenuCnt=0;
			logoutItems[j].useIcon=true;
			logoutItems[j].iconSize=iconSize;

			themedicon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,logoutIconNames[j],"");
			if(themedicon!=NULL)
				mainwind->globalLib->LFSTK_setPixmapsFromPath(mainwind->display,mainwind->visual,mainwind->cm,mainwind->window,themedicon,&logoutItems[j].icon[0],&logoutItems[j].icon[1],iconSize);
			else
				mainwind->globalLib->LFSTK_setPixmapsFromPath(mainwind->display,mainwind->visual,mainwind->cm,mainwind->window,logoutImages[j],&logoutItems[j].icon[0],&logoutItems[j].icon[1],iconSize);
		}

	logoutButton->LFSTK_setStyle(BEVELOUT);
	logoutButton->LFSTK_setLabelOriention(CENTRE);
	logoutButton->LFSTK_setCallBack(NULL,logoutCB,NULL);
	logoutButton->LFSTK_addMenus(logoutItems,NUMLOGOUTENTRYS);
	return(width);
}
