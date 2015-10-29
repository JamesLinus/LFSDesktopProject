/*
 *
 * ©K. D. Hedger. Thu 29 Oct 15:23:23 GMT 2015 kdhedger68713@gmail.com

 * This file (lfspanelprefs.cpp) is part of LFSApplications.

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

enum {EXIT=0,APPLY,NOMOREBUTTONS};

LFSTK_buttonClass	*guibc[NOMOREBUTTONS]={NULL,};
LFSTK_windowClass	*wc;
int					bwidth=96;
int					bigbwidth=128;
int					spacing=bwidth+10;
int					col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
bool				mainloop=false;
int					maxGroups=20;
int					numGroups=0;
menuItemStruct		*groups;
char				currentBuffer[256];

bool callback(void *p,void* ud)
{
	printf(">>%p<<\n",ud);
	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bhite=24;
	int				vspacing=bhite+10;
	FILE*			fp=NULL;
	char			*command;

	wc=new LFSTK_windowClass(sx,sy,1,1,"LFSPanel Prefs",false);
	wc->LFSTK_setDecorated(true);
	wc->autoLabelColour=true;
	geom=wc->LFSTK_getGeom();
	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	sy+=vspacing;
	sy+=vspacing;
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	XFlush(wc->display);
	XSync(wc->display,false);

	mainloop=true;
	while(mainloop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);

			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
					default:
						break;
				}
		}

	wc->LFSTK_hideWindow();
	for(int j=0;j<NOMOREBUTTONS;j++)
		if(guibc[j]!=NULL)
			delete guibc[j];

	delete wc;
	return(0);

	return(0);
}
