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
enum {STRETCHMODE=0,TILEMODE,CENTREMODE,NOMOREMODES};

const char				*labelNames[]={"Main Backdrop","Root Colour","Monitor Backdrop"};

LFSTK_windowClass		*wc=NULL;
LFSTK_lineEditClass		*backdropPath=NULL;
LFSTK_toggleButtonClass	*multi=NULL;
LFSTK_menuButtonClass	*mainMode=NULL;
LFSTK_lineEditClass		*mainModeEdit=NULL;
LFSTK_menuButtonClass	*monitorMode=NULL;
LFSTK_lineEditClass		*monitorModeEdit=NULL;
LFSTK_lineEditClass		*monitorBackdropPath=NULL;
LFSTK_lineEditClass		*mainColourEdit=NULL;
LFSTK_buttonClass		*guiButtons[NOMOREGUIS]={NULL,};
LFSTK_labelClass		*labels[NOMORELABELS]={NULL,};
LFSTK_menuButtonClass	*monitorNumber=NULL;

bool					mainLoop=true;
int						bwidth=100;
int						bigbwidth=128;
int						spacing=bwidth+10;
int						col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
char					*prefsPath=NULL;
char					*monitorRCPath=NULL;

//prefs
char					*wallpaperPath=NULL;
int						backdropMode;
char					*mainColour=NULL;
bool					multiMode;

//monitors
menuItemStruct			*modeMenus=NULL;
menuItemStruct			*monitorMenus=NULL;
const char				*modeName[]={"Stretch","Tile","Centre"};
int						currentMonitor=0;

args					prefs[]=
{
	{"backdrop",TYPESTRING,&wallpaperPath},
	{"mainmode",TYPEINT,&backdropMode},
	{"colour",TYPESTRING,&mainColour},
	{"multimode",TYPEBOOL,&multiMode},
	{NULL,0,NULL}
};

struct					monitors
{
	int		monW;
	int		monH;
	int		monX;
	int		monY;
	int		monMode;
	char	*monitorPath;
};

monitors				*monitorData=NULL;

#define BIG col3-col1

void setVars()
{
	if(wallpaperPath!=NULL)
		free(wallpaperPath);
	wallpaperPath=strdup(backdropPath->LFSTK_getBuffer()->c_str());

	if(mainColour!=NULL)
		free(mainColour);
	mainColour=strdup(mainColourEdit->LFSTK_getBuffer()->c_str());
	multiMode=multi->LFSTK_getValue();

	if(monitorData[currentMonitor].monitorPath!=NULL)
		free(monitorData[currentMonitor].monitorPath);
	monitorData[currentMonitor].monitorPath=strdup(monitorBackdropPath->LFSTK_getBuffer()->c_str());
}

void printMonitorData(bool tofile)
{
	FILE*	fd=NULL;
	char	buffer[2048];

	if(tofile==true)
		{
			fd=fopen(monitorRCPath,"w");
			if(fd==NULL)
				fd=stdout;
		}
	else
		fd=stdout;

	for(int j=0;j<wc->LFSTK_getMonitorCount();j++)
		fprintf(fd,"%i\n%s\n",monitorData[j].monMode,monitorData[j].monitorPath);

	if((tofile==true) && (fd!=NULL))
		fclose(fd);
}

bool callback(void *p,void* ud)
{

	switch((long)ud)
		{
			case EXIT:
				
			wc->LFSTK_clearWindow();
			setVars();
			wc->globalLib->LFSTK_saveVarsToFile("-",prefs);
			printf("\n");
			mainLoop=false;
			return(false);
			break;

			case PRINT:
				wc->LFSTK_clearWindow();
				setVars();
				printf("\n");
				wc->globalLib->LFSTK_saveVarsToFile("-",prefs);
				printMonitorData(false);
				break;

			case APPLY:
				wc->LFSTK_clearWindow();
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile(prefsPath,prefs);
				printMonitorData(true);
				system("lfssetwallpaper");
				break;
		}
	return(true);
}

bool mainModeCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);

	backdropMode=(int)(long)(menuitem->userData);
	mainModeEdit->LFSTK_setBuffer(menuitem->label);
	mainModeEdit->LFSTK_clearWindow();
	return(true);
}

bool monitorModeCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);

	monitorData[currentMonitor].monMode=(int)(long)(menuitem->userData);
	monitorModeEdit->LFSTK_setBuffer(menuitem->label);
	monitorModeEdit->LFSTK_clearWindow();
	return(true);
}

bool selectMonitor(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;
	char			*buffer;
	FILE			*fp;

	if(menuitem==NULL)
		return(true);

	asprintf(&buffer,"Monitor %i",(int)(long)menuitem->userData);
	currentMonitor=(int)(long)menuitem->userData;
	monitorNumber->LFSTK_setLabel(buffer);
	monitorBackdropPath->LFSTK_setBuffer(monitorData[currentMonitor].monitorPath);
	monitorModeEdit->LFSTK_setBuffer(modeName[monitorData[currentMonitor].monMode]);
	monitorModeEdit->LFSTK_clearWindow();
	monitorBackdropPath->LFSTK_clearWindow();
	monitorNumber->LFSTK_clearWindow();
	free(buffer);

	return(true);
}

void loadMonitorData(void)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		monnum=0;

	fd=fopen(monitorRCPath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					buffer[strlen(buffer)-1]=0;
					monitorData[monnum].monMode=atoi(buffer);
					fgets(buffer,2048,fd);
					buffer[strlen(buffer)-1]=0;
					monitorData[monnum].monitorPath=strdup(buffer);
					monnum++;
				}
			fclose(fd);
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

	asprintf(&prefsPath,"%s/.config/LFS/lfssetwallpaper.rc",getenv("HOME"));
	asprintf(&monitorRCPath,"%s/.config/LFS/lfsmonitors.rc",getenv("HOME"));

	wc=new LFSTK_windowClass(0,0,800,600,"Wallpaper Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	wc->globalLib->LFSTK_loadVarsFromFile(prefsPath,prefs);

	monitorData=(monitors*)calloc(sizeof(monitors),wc->LFSTK_getMonitorCount());
	loadMonitorData();

	guiButtons[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guiButtons[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guiButtons[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guiButtons[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guiButtons[PRINT]=new LFSTK_buttonClass(wc,"Print",(geom->w/2)-(64/2),geom->h-32,64,24,SouthGravity);
	guiButtons[PRINT]->LFSTK_setCallBack(NULL,callback,(void*)PRINT);

	sx=col1;
	sy=10;
	labels[BACKDROP]=new LFSTK_labelClass(wc,labelNames[BACKDROP],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	backdropPath=new LFSTK_lineEditClass(wc,wallpaperPath,sx,sy,BIG,24,NorthWestGravity);

	sx=col1;
	sy+=vspacing;
	labels[MAINCOLOUR]=new LFSTK_labelClass(wc,labelNames[MAINCOLOUR],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	mainColourEdit=new LFSTK_lineEditClass(wc,mainColour,sx,sy,bwidth,24,NorthWestGravity);

	sx=col1;
	sy+=vspacing;
	modeMenus=new menuItemStruct[NOMOREMODES];
	for(int j=0;j<NOMOREMODES;j++)
		{
			modeMenus[j].label=modeName[j];
			modeMenus[j].userData=(void*)(long)j;
			modeMenus[j].bc=NULL;
			modeMenus[j].subMenus=NULL;
			modeMenus[j].subMenuCnt=0;
		}

	mainMode=new LFSTK_menuButtonClass(wc,"Main Mode",sx,sy,bwidth,24,NorthWestGravity);
	mainMode->LFSTK_addMenus(modeMenus,NOMOREMODES);
	mainMode->LFSTK_setCallBack(NULL,mainModeCB,NULL);

	sx+=spacing;
	mainModeEdit=new LFSTK_lineEditClass(wc,modeName[backdropMode],sx,sy,bwidth,24,NorthWestGravity);

	sy+=vspacing;
	sx=col1;
	multi=new LFSTK_toggleButtonClass(wc,"Multiple Monitors",col1+spacing,sy,bwidth*2,24,NorthWestGravity);
	multi->LFSTK_setValue(multiMode);
	sy+=vspacing;
	sx=col1;	

//monitors
	monitorMenus=new menuItemStruct[wc->LFSTK_getMonitorCount()];
	for(int j=0;j<wc->LFSTK_getMonitorCount();j++)
		{
			asprintf((char**)&(monitorMenus[j].label),"Monitor %i",j);
			monitorMenus[j].userData=(void*)(long)j;
			monitorMenus[j].bc=NULL;
			monitorMenus[j].subMenus=NULL;
			monitorMenus[j].subMenuCnt=0;
		}

	monitorNumber=new LFSTK_menuButtonClass(wc,"Monitor 0",sx,sy,bwidth,24,NorthWestGravity);
	monitorNumber->LFSTK_addMenus(monitorMenus,wc->LFSTK_getMonitorCount());
	monitorNumber->LFSTK_setCallBack(NULL,selectMonitor,NULL);

	sx=col1;
	sy+=vspacing;
	labels[MONITORBACKDROP]=new LFSTK_labelClass(wc,labelNames[MONITORBACKDROP],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	monitorBackdropPath=new LFSTK_lineEditClass(wc,monitorData[0].monitorPath,sx,sy,BIG,24,NorthWestGravity);

	sx=col1;
	sy+=vspacing;
	monitorMode=new LFSTK_menuButtonClass(wc,"Monitor Mode",sx,sy,bwidth,24,NorthWestGravity);
	monitorMode->LFSTK_setCallBack(NULL,monitorModeCB,NULL);
	monitorMode->LFSTK_addMenus(modeMenus,NOMOREMODES);
	sx+=spacing;
	monitorModeEdit=new LFSTK_lineEditClass(wc,modeName[monitorData[0].monMode],sx,sy,bwidth,24,NorthWestGravity);
	sy+=vspacing;

	sy+=vspacing;
	sy+=vspacing;
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	wc->LFSTK_resizeWindow(col1+BIG+bwidth+20,sy);

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

	delete backdropPath;
	delete multi;
	delete mainMode;
	delete mainModeEdit;
	delete monitorMode;
	delete monitorModeEdit;
	delete monitorBackdropPath;
	delete mainColourEdit;
	delete monitorNumber;

	for(int j=BACKDROP;j<NOMORELABELS;j++)
		delete labels[j];

	for(int j=0;j<wc->LFSTK_getMonitorCount();j++)
		{
			if(monitorMenus[j].label!=NULL)
				free((char*)monitorMenus[j].label);
		}

	delete monitorMenus;
	delete modeMenus;
	delete wc;
	free(prefsPath);
	free(monitorRCPath);
	return(0);
}