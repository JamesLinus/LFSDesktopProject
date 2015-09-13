/*
 *
 * ©K. D. Hedger. Sun 13 Sep 14:10:19 BST 2015 kdhedger68713@gmail.com

 * This file (lfsdesktopprefs.cpp) is part of LFSApplications.

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

enum {ICONTHEME,ICONSIZE,GRIDSIZE,GRIDBORDER,REFRESHRATE,SHOWSUFFIX,FORECOLOUR,BACKCOLOUR,TERMCOMMAND,FONTFACE,IGNORES,NUMPREFS};
enum {EXIT=0,APPLY,SAVE,NOMOREBUTTONS};

LFSTK_windowClass	*wc;
//LFSTK_buttonClass	*bc[NOMOREBUTTONS]={NULL,};
//LFSTK_buttonClass	*guibc[NOMORE]={NULL,};
LFSTK_lineEditClass	*le[NUMPREFS]={NULL,};
LFSTK_labelClass	*lb[NUMPREFS]={NULL,};
LFSTK_buttonClass	*guibc[NOMOREBUTTONS]={NULL,};

char				*prefs[NUMPREFS]={NULL,};
const char			*labelNames[]={"Icon Theme","Icon Size","Grid Size","Border","Refresh","Show Ext","Fore Colour","Back Colour","Term Command","Font","Ignore"};

args				desktopPrefs[]=
{
	{"icontheme",TYPESTRING,&prefs[ICONTHEME]},
	{"iconsize",TYPESTRING,&prefs[ICONSIZE]},
	{"gridsize",TYPESTRING,&prefs[GRIDSIZE]},
	{"gridborder",TYPESTRING,&prefs[GRIDBORDER]},
	{"refreshrate",TYPESTRING,&prefs[REFRESHRATE]},
	{"termcommand",TYPESTRING,&prefs[TERMCOMMAND]},
	{"showextension",TYPESTRING,&prefs[SHOWSUFFIX]},
	{"fontface",TYPESTRING,&prefs[FONTFACE]},
	{"labelforeground",TYPESTRING,&prefs[FORECOLOUR]},
	{"labelbackground",TYPESTRING,&prefs[BACKCOLOUR]},
	{"noshow",TYPESTRING,&prefs[IGNORES]},
	{NULL,0,NULL}
};

bool				mainloop=false;
char				*env;

void setVars(void)
{
	for(int j=ICONTHEME; j<NUMPREFS; j++)
		if(lb[j]!=NULL)
			lb[j]->LFSTK_clearWindow();

}

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
				wc->LFSTK_clearWindow();
				//setVars();
				//wc->globalLib->LFSTK_saveVarsToFile(env,wmPrefs);
				break;

			case APPLY:
				wc->LFSTK_clearWindow();
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile("-",desktopPrefs);
				printf("\n");
				break;
		}
	return(true);
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

	prefs[ICONTHEME]=strdup("gnome");
	prefs[ICONSIZE]=strdup("32");
	prefs[GRIDSIZE]=strdup("64");
	prefs[GRIDBORDER]=strdup("32");
	prefs[REFRESHRATE]=strdup("2");
	prefs[TERMCOMMAND]=strdup("xterm -e ");
	prefs[SHOWSUFFIX]=strdup("0");
	prefs[FONTFACE]=strdup("Sans;0;0;10");
	prefs[FORECOLOUR]=strdup("0xffffffff");
	prefs[BACKCOLOUR]=strdup("0x00000000");
	prefs[IGNORES]=strdup("");

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Desktop Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	asprintf(&env,"%s/.config/LFS/lfsdesktop.rc",getenv("HOME"));
	wc->globalLib->LFSTK_loadVarsFromFile(env,desktopPrefs);

	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibc[SAVE]=new LFSTK_buttonClass(wc,"Save",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	guibc[SAVE]->LFSTK_setCallBack(NULL,callback,(void*)SAVE);
	sx=col1;
	sy=10;
	//int state=0;
/*
	asprintf(&fontFace,"Sans;0;0;10");
	asprintf(&foreCol,"0xffffffff");
	asprintf(&backCol,"0x00000000");
	showSuffix=false;
	asprintf(&terminalCommand,"xterm -e ");
	{"icontheme",TYPESTRING,&prefs[ICONTHEME]},
	{"iconsize",TYPESTRING,&prefs[ICONSIZE]},
	{"gridsize",TYPESTRING,&prefs[GRIDSIZE]},
	{"gridborder",TYPESTRING,&prefs[GRIDBORDER]},
	{"refreshrate",TYPESTRING,&prefs[REFRESHRATE]},
	{"termcommand",TYPESTRING,&prefs[TERMCOMMAND]},
	{"showextension",TYPESTRING,&prefs[SHOWSUFFIX]},
	{"fontface",TYPESTRING,&prefs[FONTFACE]},
	{"labelforeground",TYPESTRING,&prefs[FORECOLOUR]},
	{"labelbackground",TYPESTRING,&prefs[BACKCOLOUR]},
	{"noshow",TYPESTRING,&prefs[IGNORES]},

	asprintf(&fontFace,"Sans;0;0;10");
	asprintf(&foreCol,"0xffffffff");
	asprintf(&backCol,"0x00000000");
	showSuffix=false;
	asprintf(&terminalCommand,"xterm -e ");


int				iconSize=40;
int				gridBorder=32;
int				gridSize=iconSize+gridBorder;
int				refreshRate=2;
char			*terminalCommand=NULL;
bool			showSuffix=false;
char			*fontFace=NULL;
char			*foreCol;
char			*backCol;
char			*ignores=NULL;


*/

	for(int j=ICONTHEME;j<NUMPREFS;j++)
		{
			lb[j]=new LFSTK_labelClass(wc,labelNames[j],sx,sy,bwidth,24,NorthWestGravity);
			lb[j]->LFSTK_setLabelAutoColour(true);
			sx+=spacing;
			le[j]=new LFSTK_lineEditClass(wc,prefs[j],sx,sy-1,col2-col1+bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
			//state++;
		}
	sy+=(vspacing*2);
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	printf("Current Settings:\n\n");
	callback(NULL,(void*)APPLY);
	printf("\n\n");

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
						//wc->LFSTK_clearWindow();
						//wc->LFSTK_setActive(true);
						//callback(NULL,(void*)APPLY);
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
		if(guibc[j]!=NULL)
			delete guibc[j];

	free(env);
	return(0);
};