/*
 *
 * ©K. D. Hedger. Wed 16 Sep 11:55:33 BST 2015 kdhedger68713@gmail.com

 * This file (lfsappearance) is part of LFSApplications.

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

enum {EXIT=0,APPLY,NEWGROUP,UPDATEGROUP,NOMOREBUTTONS};
enum {WALLPAPER=0,TOOLKIT,DESKTOP,WMANAGER,NOMORELAUNCHERS};
enum {LAUNCHWALLPAPER=100,LAUNCHTOOLKIT,LAUNCHDESKTOP,LAUNCHWMANAGER};
LFSTK_windowClass	*wc;
LFSTK_lineEditClass	*current=NULL;
LFSTK_lineEditClass	*newgroup=NULL;
LFSTK_menuButtonClass *mb=NULL;
LFSTK_buttonClass	*guibc[NOMOREBUTTONS]={NULL,};
LFSTK_buttonClass	*launch[NOMORELAUNCHERS]={NULL,};
LFSTK_labelClass	*label[NOMORELAUNCHERS]={NULL,};

int					bwidth=96;
int					bigbwidth=128;
int					spacing=bwidth+10;
int					col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
bool				mainloop=false;
int					maxGroups=20;
int					numGroups=0;
menuItemStruct		*groups;
#define BIG col2-col1+bwidth

int fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

void makeGroup(const char *grpname)
{
	char	*grp;
	char	*command;

	asprintf(&grp,"%s/.config/LFS/lfsgroupsprefs/%s",getenv("HOME"),grpname);
	if(fileExists(grp)!=0)
		mkdir(grp,0755);

	asprintf(&command,"cp %s/.config/LFS/lfs*.rc \"%s\"",getenv("HOME"),grp);
	system(command);

	free(command);
	free(grp);
}

void setGroup(void)
{
	char	*command;
	char	*grp;

	asprintf(&grp,"%s/.config/LFS/lfsgroupsprefs/%s",getenv("HOME"),current->LFSTK_getBuffer()->c_str());
	if(fileExists(grp)==0)
		{
			asprintf(&command,"cp \"%s/.config/LFS/lfsgroupsprefs/%s/\"lfs*.rc %s/.config/LFS",getenv("HOME"),current->LFSTK_getBuffer()->c_str(),getenv("HOME"));
			system(command);
			free(command);
		}
	free(grp);
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
			case LAUNCHWALLPAPER:
				wc->LFSTK_hideWindow();
				system("lfswallpaperprefs");
				wc->LFSTK_showWindow(false);
				break;
			case LAUNCHTOOLKIT:
				wc->LFSTK_hideWindow();
				system("lfstkprefs");
				wc->LFSTK_showWindow(false);
				break;
			case LAUNCHDESKTOP:
				wc->LFSTK_hideWindow();
				system("lfsdesktopprefs");
				wc->LFSTK_showWindow(false);
				break;
			case LAUNCHWMANAGER:
				wc->LFSTK_hideWindow();
				system("lfswmprefs");
				wc->LFSTK_showWindow(false);
				break;

			case UPDATEGROUP:
				if(strlen(current->LFSTK_getBuffer()->c_str())>0)
					makeGroup(current->LFSTK_getBuffer()->c_str());
				break;

			case NEWGROUP:
				if(strlen(newgroup->LFSTK_getBuffer()->c_str())<1)
					return(true);
				groups[numGroups].label=(char*)strdup(newgroup->LFSTK_getBuffer()->c_str());
				groups[numGroups].userData=(void*)(long)(numGroups+1);
				groups[numGroups].bc=NULL;
				numGroups++;
				mb->LFSTK_addMenus(groups,numGroups);
				makeGroup(newgroup->LFSTK_getBuffer()->c_str());
				break;

			case APPLY:
				if(strlen(current->LFSTK_getBuffer()->c_str())>0)
					setGroup();
				system("lfsdesktop &");
				system("lfssetwallpaper");
				system("killall lfswmanager;lfswmanager &");
				break;
		}

	wc->LFSTK_reloadGlobals();
	for(int j=WALLPAPER;j<NOMORELAUNCHERS;j++)
		launch[j]->LFSTK_reloadColours();
	for(int j=EXIT;j<NOMOREBUTTONS;j++)
		guibc[j]->LFSTK_reloadColours();
	mb->LFSTK_reloadColours();

	return(true);
}

void loadGroups()
{
	char	*groupfolder;
	char	*command;
	FILE*	fp=NULL;
	char	buffer[2048];
	int		cnt=0;
	char	*ptr;

//init group array
	for(int j=0;j<maxGroups;j++)
		{
			groups[j].label=NULL;
			groups[j].userData=NULL;
			groups[j].bc=NULL;
		}

	asprintf(&groupfolder,"%s/.config/LFS/lfsgroupsprefs",getenv("HOME"));
	if(fileExists(groupfolder)!=0)
		{
			mkdir(groupfolder,0755);
			makeGroup("Current Group");
		}

	asprintf(&command,"%s/Current Set",groupfolder);
	if(fileExists(command)!=0)
		makeGroup("Current Set");
	free(command);

	asprintf(&command,"find %s -mindepth 1 -maxdepth 1 -type d -printf \"%%f\\n\"",groupfolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							groups[cnt].label=strdup(buffer);
							groups[cnt].userData=(void*)(long)(cnt+1);
							groups[cnt].bc=NULL;
							cnt++;
						}
				}
			pclose(fp);
		}
	numGroups=cnt;
	free(command);
	free(groupfolder);
}

bool bcb(void *p,void* ud)
{
	menuItemStruct	*menuitem;

	if(((long)ud<0) || ((long)ud>numGroups))
		return(true);

	menuitem=(menuItemStruct*)ud;

	if((long)ud>0)
		{
			current->LFSTK_setBuffer(groups[(long)ud-1].label);
			current->LFSTK_clearWindow();
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

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Group Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();
	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibc[UPDATEGROUP]=new LFSTK_buttonClass(wc,"Update",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	guibc[UPDATEGROUP]->LFSTK_setCallBack(NULL,callback,(void*)UPDATEGROUP);

	sx=col1;
	sy=10;
//enum {WALLPAPER=0,TOOLKIT,DESKTOP,WMANAGER,NOMORELAUNCHERS};
//wallpaper
	launch[WALLPAPER]=new LFSTK_buttonClass(wc,"Wallpaper Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[WALLPAPER]->LFSTK_setActive(false);
	launch[WALLPAPER]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHWALLPAPER);
	sx+=spacing+20;
	label[WALLPAPER]=new LFSTK_labelClass(wc,"Launch Wallpaper Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//toolkit
	sx=col1;
	sy+=vspacing;
	launch[TOOLKIT]=new LFSTK_buttonClass(wc,"Toolkit Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[TOOLKIT]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHTOOLKIT);
	sx+=spacing+20;
	label[TOOLKIT]=new LFSTK_labelClass(wc,"Launch Toolkit Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//desktop
	sx=col1;
	sy+=vspacing;
	launch[DESKTOP]=new LFSTK_buttonClass(wc,"Desktop Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[DESKTOP]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHDESKTOP);
	sx+=spacing+20;
	label[DESKTOP]=new LFSTK_labelClass(wc,"Launch Desktop Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//wmanager
	sx=col1;
	sy+=vspacing;
	launch[WMANAGER]=new LFSTK_buttonClass(wc,"WM Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[WMANAGER]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHWMANAGER);
	sx+=spacing+20;
	label[WMANAGER]=new LFSTK_labelClass(wc,"Launch Window Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);

	sx=col1;
	sy+=vspacing;
	mb=new LFSTK_menuButtonClass(wc,"Load Set",sx,sy,bwidth,24,NorthWestGravity);
	mb->LFSTK_setCallBack(NULL,NULL,NULL);
	mb->LFSTK_setStyle(EMBOSSEDBUTTON);
	mb->LFSTK_setLabelOriention(CENTRE);
	groups=new menuItemStruct[maxGroups];
	mb->LFSTK_setCallBack(NULL,bcb,groups);
	
	loadGroups();
	mb->LFSTK_addMenus(groups,numGroups);

	sx+=spacing;;
	current=new LFSTK_lineEditClass(wc,"",sx,sy-1,BIG,24,NorthWestGravity);
	sy+=vspacing;
	
	sx=col1;
	guibc[NEWGROUP]=new LFSTK_buttonClass(wc,"New Set",sx,sy,bwidth,24,NorthWestGravity);
	guibc[NEWGROUP]->LFSTK_setCallBack(NULL,callback,(void*)NEWGROUP);
	sx+=spacing;;
	newgroup=new LFSTK_lineEditClass(wc,"",sx,sy-1,BIG,24,NorthWestGravity);

	sy+=vspacing;
	sy+=vspacing;
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	current->LFSTK_setBuffer(groups[0].label);
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

	delete wc;
	for(int j=0;j<NOMOREBUTTONS;j++)
		if(guibc[j]!=NULL)
			delete guibc[j];
	for(int j=0;j<maxGroups;j++)
		if(groups[j].label!=NULL)
			free(groups[j].label);
	return(0);
}
