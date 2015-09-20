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

#define BWIDTH 64

LFSTK_menuButtonClass	*logoutButton=NULL;
int						logoutX,logoutY;
menuItemStruct			*logoutItems;
const char				*logoutLabels[]={"Logout","Restart","Shutdown"};

bool logoutCB(void *p,void* ud)
{
	printf(">>>%p<<<\n",ud);
	return(true);
}

void addLogout(int x,int y)
{
	logoutX=x-BWIDTH;

	logoutButton=new LFSTK_menuButtonClass(mainwind,"Logout",logoutX,0,BWIDTH,panelHeight,NorthWestGravity);
	logoutButton->LFSTK_setCallBack(logoutCB,NULL,NULL);
	logoutItems=new menuItemStruct[NUMLOGOUTENTRYS];
	
	for(int j=LOGOUT;j<NUMLOGOUTENTRYS;j++)
		{
			logoutItems[j].label=strdup(logoutLabels[j]);
			logoutItems[j].userData=(void*)(long)(j+1);
			logoutItems[j].bc=NULL;
		}

	logoutButton->LFSTK_setStyle(EMBOSSEDBUTTON);
	logoutButton->LFSTK_setLabelOriention(CENTRE);
	logoutButton->LFSTK_setCallBack(NULL,logoutCB,NULL);
	logoutButton->LFSTK_addMenus(logoutItems,NUMLOGOUTENTRYS);
}
