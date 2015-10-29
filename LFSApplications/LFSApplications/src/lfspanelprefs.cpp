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

#define BIG col2-col1+bwidth
#define MAXPANELS 20

enum {EXIT=0,APPLY,NOMOREBUTTONS};
enum {PANELWIDTH=0,PANELPOS,PANELGRAV,OPTIONSCNT};
enum {WIDTHFILL=-1,WIDTHSHRINK=-2,WIDTHCNT=3};
enum {LEFTPOS=-1,CENTREPOS=-2,RIGHTPOS=-3,POSCNT=4};
enum {NORTHGRAV=1,EASTGRAV=2,SOUTHGRAV=3,WESTGRAV=4,GRAVCNT=4};

LFSTK_buttonClass		*guibc[NOMOREBUTTONS]={NULL,};
LFSTK_windowClass		*wc;
LFSTK_lineEditClass		*currentPanel=NULL;
LFSTK_menuButtonClass	*panelSelect=NULL;
LFSTK_menuButtonClass	*panelOptionsMenus[OPTIONSCNT]={NULL,};
LFSTK_lineEditClass		*panelOptionsEdit[OPTIONSCNT]={NULL,};
const char				*panelOptionString[OPTIONSCNT][4]={{"Fill","Shrink","Custom",""},{"Left","Centre","Right","Custom"},{"North","East","South","West"}};

menuItemStruct			*panels;
menuItemStruct			panelWidth[3];
menuItemStruct			panelPos[4];
menuItemStruct			panelgrav[4];

int					bwidth=96;
int					bigbwidth=128;
int					spacing=bwidth+10;
int					col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
bool				mainloop=false;
int					numGroups=0;
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

bool bcb(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);

	currentPanel->LFSTK_setBuffer(menuitem->label);
	currentPanel->LFSTK_clearWindow();
	return(true);
}

bool setWidthCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;
	char			buffer[16];
	if(menuitem==NULL)
		return(true);

	snprintf(buffer,16,"%i",(long)menuitem->userData);
	panelOptionsEdit[PANELWIDTH]->LFSTK_setBuffer(buffer);
	panelOptionsEdit[PANELWIDTH]->LFSTK_clearWindow();
	return(true);
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
	char			*lfspanels;
	char			buffer[512];

	wc=new LFSTK_windowClass(sx,sy,1,1,"LFSPanel Prefs",false);
	wc->LFSTK_setDecorated(true);
	wc->autoLabelColour=true;
	geom=wc->LFSTK_getGeom();
	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

//select panel
	sx=col1;
	sy=16;
	panelSelect=new LFSTK_menuButtonClass(wc,"Panel Config",sx,sy,bwidth,24,NorthWestGravity);
	panelSelect->LFSTK_setStyle(BEVELOUT);
	panelSelect->LFSTK_setLabelOriention(CENTRE);
	panels=new menuItemStruct[MAXPANELS];
	panelSelect->LFSTK_setCallBack(NULL,bcb,NULL);
	
	sx+=spacing;;
	currentPanel=new LFSTK_lineEditClass(wc,"lfspanel.rc",sx,sy-1,BIG,24,NorthWestGravity);

	int		cnt=0;
	snprintf(buffer,512,"find %s/.config/LFS -maxdepth 1 -mindepth 1 -type f -name \"lfspanel*.rc\"",getenv("HOME"));
	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,512,fp))
				{
					buffer[strlen(buffer)-1]=0;
					panels[cnt].label=strdup(basename(buffer));
					panels[cnt].userData=(void*)(long)cnt;
					panels[cnt].subMenus=NULL;
					panels[cnt].useIcon=false;
					panels[cnt].bc=NULL;
					cnt++;
				}
			pclose(fp);
		}

	panelSelect->LFSTK_addMenus(panels,cnt);
	sy+=vspacing;

//panel width
	for(int j=0;j<WIDTHCNT;j++)
		{
			panelWidth[j].label=panelOptionString[PANELWIDTH][j];
			//panelWidth[j].userData=(void*)(long)j;
			panelWidth[j].subMenus=NULL;
			panelWidth[j].useIcon=false;
			panelWidth[j].bc=NULL;
		}
			panelWidth[0].userData=(void*)-1;
			panelWidth[1].userData=(void*)-2;
			panelWidth[2].userData=(void*)0;
	sx=col1;
	panelOptionsMenus[PANELWIDTH]=new LFSTK_menuButtonClass(wc,"Panel Width",sx,sy,bwidth,24,NorthWestGravity);
	panelOptionsMenus[PANELWIDTH]->LFSTK_addMenus(panelWidth,WIDTHCNT);
	panelOptionsMenus[PANELWIDTH]->LFSTK_setCallBack(NULL,setWidthCB,NULL);
	sx+=spacing;
	panelOptionsEdit[PANELWIDTH]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

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

	delete currentPanel;
	delete panelSelect;

	delete wc;
	return(0);

	return(0);
}
