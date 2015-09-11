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

enum {BACTIVEFRAME=0,BACTIVEFILL,BINACTIVEFRAME,BINACTIVEFILL,BWIDGETCOLOUR,NOMOREBUTTONS};
enum {EACTIVEFRAME=0,EACTIVEFILL,EINACTIVEFRAME,EINACTIVEFILL,EWIDGETCOLOUR,ETHEMEPATH,ETERMCOMMAND,EPLACEMENT,ETITLEFONT,ENUMDESKS,ELIVEUPDATE,NOMOREEDITS};
enum {ACTIVEFRAME=0,ACTIVEFRAMEFILL,INACTIVEFRAME,INACTIVEFRAMEFILL,TEXTCOLOUR};
enum {EXIT=0,APPLY,SAVE,NOMORE};
enum {THEMELABEL=0,TERMLABEL,PLACELABEL,FONTLABEL,DESKLABEL,UPDATELABEL,NOMORELABELS};

const char			*buttonnames[]={"Active Frame","Active Fill","Inactive Frame","Inactive Fill","Text Colour"};
const char			*labelnames[]={"Theme Path","Term Command","Placement","Font","Desktops", "Update"};

enum {NOPLACE=0,UNDERMOUSE,CENTREMMONITOR,CENTRESCREEN,MOUSEMONITOR};
//prefs
int					placement=MOUSEMONITOR;
unsigned int		numberOfDesktops;
char				*titleFont;
char				*fontColours[5];
int					doswapdesk=-1;
int					liveUpdate=1;
char				*terminalCommand=NULL;

//theme
//themeStruct			theme;
char				*fontName;
int					fontSize;
char				*pathToTheme;

args				wmPrefs[]=
{
	{"wmactive_frame",TYPESTRING,&fontColours[ACTIVEFRAME]},
	{"wmactive_fill",TYPESTRING,&fontColours[ACTIVEFRAMEFILL]},
	{"wminactive_frame",TYPESTRING,&fontColours[INACTIVEFRAME]},
	{"wminactive_fill",TYPESTRING,&fontColours[INACTIVEFRAMEFILL]},
	{"widgetcolour",TYPESTRING,&fontColours[TEXTCOLOUR]},
	{"placement",TYPEINT,&placement},
	{"titlefont",TYPESTRING,&titleFont},
	{"desktops",TYPEINT,&numberOfDesktops},
	{"liveupdate",TYPEINT,&liveUpdate},
	{"theme",TYPESTRING,&pathToTheme},
	{"termcommand",TYPESTRING,&terminalCommand},
	{NULL,0,NULL}
};


bool				mainloop=false;
LFSTK_windowClass	*wc;
LFSTK_buttonClass	*bc[NOMOREBUTTONS]={NULL,};
LFSTK_buttonClass	*guibc[NOMORE]={NULL,};
LFSTK_lineEditClass	*le[NOMOREEDITS]={NULL,};
LFSTK_labelClass	*lb[NOMORELABELS]={NULL,};
char				*env;

bool callback(void *p,void* ud)
{

	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case SAVE:
				//asprintf(&env,"%s/.config/LFS/lfswmanager.rc",getenv("HOME"));
				wc->globalLib->LFSTK_saveVarsToFile(env,wc->globalLib->LFSTK_getTKArgs());
				break;

			case APPLY:
//				wc->LFSTK_setWindowColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
//				wc->globalLib->LFSTK_setGlobalString(NORMALCOLOUR,TYPEWINDOW,wc->windowColourNames[NORMALCOLOUR].name);
				wc->LFSTK_clearWindow();
				for(int j=BACTIVEFRAME;j<NOMOREBUTTONS;j++)
					if(bc[j]!=NULL)
						{
							bc[j]->LFSTK_setColourName(INACTIVECOLOUR,le[j]->LFSTK_getBuffer()->c_str());
							bc[j]->LFSTK_clearWindow();
						}
				for(int j=EACTIVEFRAME;j<NOMOREEDITS;j++)
					if(le[j]!=NULL)
						le[j]->LFSTK_clearWindow();
				for(int j=THEMELABEL;j<NOMORELABELS;j++)
					if(lb[j]!=NULL)
						lb[j]->LFSTK_clearWindow();
						printf(">>%s<<\n",le[ETHEMEPATH]->LFSTK_getLabel());
				break;	

		}
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bwidth=96;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;

	fontColours[ACTIVEFRAME]=strdup("rgb:00/00/00");
	fontColours[ACTIVEFRAMEFILL]=strdup("rgb:00/ff/ff");
	fontColours[INACTIVEFRAME]=strdup("rgb:00/00/00");
	fontColours[INACTIVEFRAMEFILL]=strdup("rgb:ff/ff/ff");
	fontColours[TEXTCOLOUR]=strdup("rgb:ff/ff/ff");

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS WM Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	asprintf(&env,"%s/.config/LFS/lfswmanager.rc",getenv("HOME"));
	wc->globalLib->LFSTK_loadVarsFromFile(env,wmPrefs);

	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibc[SAVE]=new LFSTK_buttonClass(wc,"Save",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	guibc[SAVE]->LFSTK_setCallBack(NULL,callback,(void*)SAVE);
	sx=col1;
	sy=10;
	int state=0;
	for (int j=BACTIVEFRAME;j<NOMOREBUTTONS;j++)
		{
			bc[j]=new LFSTK_buttonClass(wc,buttonnames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setActive(false);
			bc[j]->LFSTK_setLabelAutoColour(true);
			bc[j]->LFSTK_setColourName(INACTIVECOLOUR,fontColours[state]);
			sx+=spacing;
			le[j]=new LFSTK_lineEditClass(wc,fontColours[state],sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
			state++;
		}
xxxxxxxxxxxxxxxxxxxxxxxxxxxx
	sx=col1;
	state=0;
	for(int j=THEMELABEL;j<NOMORELABELS;j++)
		{
			lb[j]=new LFSTK_labelClass(wc,labelnames[j],sx,sy,bwidth,24,NorthWestGravity);
			lb[j]->LFSTK_setLabelAutoColour(true);
			sx+=spacing;
			le[ETHEMEPATH+j]=new LFSTK_lineEditClass(wc,labelnames[j],sx,sy-1,col2-col1+bwidth,24,NorthWestGravity);
			printf("==%s=%i===\n",labelnames[j],ETHEMEPATH+j);
			sy+=vspacing;
			sx=col1;
			state++;
		}
	state=0;
printf("//////////////////////////////////////////////\n");
	le[5]->LFSTK_setLabel("XXXXXXXXXX");
	lb[0]->LFSTK_setLabel("XXXXXXXXXX");
	//le[EACTIVEFRAME]->LFSTK_setLabel("XXXXXXXXXX");
//	le[ETHEMEPATH]->LFSTK_clearWindow();
	le[5]->LFSTK_clearWindow();
	lb[0]->LFSTK_clearWindow();
	//le[ETHEMEPATH]->drawLabel();
	//printf(">>%s<<\n",le[ETHEMEPATH]->LFSTK_getLabel());
//	lb[THEMELABETHEMEPATH]EL]=new LFSTK_labelClass(wc,"Theme Path",sx,sy,bwidth,24,NorthWestGravity);
//	lb[THEMELABEL]->LFSTK_setLabelAutoColour(true);
//	sx+=spacing;
//	le[ETHEMEPATH]=new LFSTK_lineEditClass(wc,pathToTheme,sx,sy-1,col2-col1+bwidth,24,NorthWestGravity);

/*
	{"placement",TYPEINT,&placement},
	{"titlefont",TYPESTRING,&titleFont},
	{"desktops",TYPEINT,&numberOfDesktops},
	{"liveupdate",TYPEINT,&liveUpdate},
	{"theme",TYPESTRING,&pathToTheme},
	{"termcommand",TYPESTRING,&terminalCommand},

*/
//	sy+=vspacing;
//	sx=col1;
//	lb[TERMLABEL]=new LFSTK_labelClass(wc,labelnames[TERMLABEL],sx,sy,bwidth,24,NorthWestGravity);
//	lb[TERMLABEL]->LFSTK_setLabelAutoColour(true);
//	sx+=spacing;
//	le[ETERMCOMMAND]=new LFSTK_lineEditClass(wc,terminalCommand,sx,sy-1,col2-col1+bwidth,24,NorthWestGravity);
//
//	sy+=vspacing;
//	sx=col1;



	wc->LFSTK_setKeepAbove(true);
	sy+=(vspacing*2);
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();

//	printf("Current Settings:\n\n");
	callback(NULL,(void*)APPLY);
//	printf("\n\n");

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
						wc->LFSTK_setActive(true);
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
					default:
						break;
				}
		}

	delete wc;
	for(int j=0;j<NOMOREBUTTONS;j++)
		if(bc[j]!=NULL)
			delete bc[j];
	return 0;

}