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
char					*logoutListIcon=NULL;

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

int  addLogout(int x,int y,int grav)
{
	int	xpos=0;
	int width=panelHeight+6;
	int	retval=width;
	char	*themedicon=NULL;

	if(logoutButton!=NULL)
		{
			printError("Duplicate logout");
			return(0);
		}
	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	logoutButton=new LFSTK_menuButtonClass(mainwind,"",xpos,0,panelHeight+6,panelHeight,grav);
	logoutListIcon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,logoutIconNames[NUMLOGOUTENTRYS],"");
	if(logoutListIcon!=NULL)
		logoutButton->LFSTK_setIconFromPath(logoutListIcon,panelHeight-6);
	else
		logoutButton->LFSTK_setIconFromPath(DATADIR "/pixmaps/exit.png",panelHeight-6);

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

			themedicon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,logoutIconNames[j],"");
			if(themedicon!=NULL)
				{
					mainwind->globalLib->LFSTK_setPixmapsFromPath(mainwind->display,mainwind->visual,mainwind->cm,mainwind->window,themedicon,&logoutItems[j].icon[0],&logoutItems[j].icon[1],16);
					free(themedicon);
				}
			else
				mainwind->globalLib->LFSTK_setPixmapsFromPath(mainwind->display,mainwind->visual,mainwind->cm,mainwind->window,logoutImages[j],&logoutItems[j].icon[0],&logoutItems[j].icon[1],16);
		}

	logoutButton->LFSTK_setStyle(BEVELOUT);
	logoutButton->LFSTK_setLabelOriention(CENTRE);
	logoutButton->LFSTK_setCallBack(NULL,logoutCB,NULL);
	logoutButton->LFSTK_addMenus(logoutItems,NUMLOGOUTENTRYS);

	return(retval);
}
