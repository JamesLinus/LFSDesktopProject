/*
 *
 * ©K. D. Hedger. Fri 25 Sep 14:51:01 BST 2015 kdhedger68713@gmail.com

 * This file (lfsbackdropprefs.cpp) is part of LFSApplications.

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
#include <LFSTKToggleButton.h>
#include <LFSTKLabel.h>
#include <LFSTKLib.h>

enum labels {BACKDROP=0,MAINCOLOUR,MONITORBACKDROP,NOMORELABELS};
enum {EXIT=0,PRINT,APPLY,NOMOREGUIS};

LFSTK_windowClass		*wc=NULL;
LFSTK_lineEditClass		*backdropPath=NULL;
LFSTK_toggleButtonClass	*mult=NULL;
LFSTK_menuButtonClass	*mainMode=NULL;
LFSTK_lineEditClass		*mainColourEdit=NULL;
LFSTK_buttonClass		*guiButtons[NOMOREGUIS]={NULL,};

bool					mainLoop=true;
int						bwidth=96;
int						bigbwidth=128;
int						spacing=bwidth+10;
int						col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
char					*prefsPath;
char					*monitorRCPath;

//prefs
char					*wallpaperPath;
int						backdropMode;
char					*mainColour;
bool					multiMode;

args					prefs[]=
{
	{"backdrop",TYPESTRING,&wallpaperPath},
	{"mainmode",TYPEINT,&backdropMode},
	{"colour",TYPESTRING,&mainColour},
	{"multimode",TYPEBOOL,&multiMode},
	{NULL,0,NULL}
};

bool callback(void *p,void* ud)
{

	switch((long)ud)
		{
			case EXIT:
				
			wc->LFSTK_clearWindow();
//			setVars();
//			wc->globalLib->LFSTK_saveVarsToFile("-",desktopPrefs);
//			printf("\n");
			mainLoop=false;
			return(false);
			break;

			case PRINT:
				wc->LFSTK_clearWindow();
//				setVars();
//				wc->globalLib->LFSTK_saveVarsToFile("-",desktopPrefs);
//				printf("\n");
				break;

			case APPLY:
//				wc->LFSTK_clearWindow();
//				setVars();
//				wc->globalLib->LFSTK_saveVarsToFile(env,desktopPrefs);
//				system("lfsdesktop &");
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
	int				bhite=24;
	int				vspacing=bhite+10;
	FILE*			fp=NULL;
	char			*command;

	asprintf(&prefsPath,"%s/.config/LFS/lfssetwallpaper.rc",getenv("HOME"));
	asprintf(&monitorRCPath,"%s/.config/LFS/lfsmonitors.rc",getenv("HOME"));

	wc=new LFSTK_windowClass(0,0,800,600,"Wallpaper Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	wc->globalLib->LFSTK_loadVarsFromFile(prefsPath,prefs);

	guiButtons[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guiButtons[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guiButtons[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guiButtons[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guiButtons[PRINT]=new LFSTK_buttonClass(wc,"Print",(geom->w/2)-(64/2),geom->h-32,64,24,SouthGravity);
	guiButtons[PRINT]->LFSTK_setCallBack(NULL,callback,(void*)PRINT);

	//wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	XFlush(wc->display);

	mainLoop=true;
	while(mainLoop==true)
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

	for(int j=EXIT;j<NOMOREGUIS;j++)
		delete guiButtons[j];

	delete wc;
	free(prefsPath);
	free(monitorRCPath);
	return(0);
}