/*
 *
 * ©K. D. Hedger. Mon 14 Sep 16:17:46 BST 2015 kdhedger68713@gmail.com

 * This file (lfsprefsgroup.cpp) is part of LFSApplications.

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

enum {LOADMENU,NUMPREFS};
enum {EXIT=0,APPLY,NEWGROUP,NOMOREBUTTONS};

LFSTK_windowClass	*wc;
//LFSTK_lineEditClass	*le[NUMPREFS]={NULL,};
//LFSTK_labelClass	*lb[NUMPREFS]={NULL,};
LFSTK_menuButtonClass *mb=NULL;
LFSTK_buttonClass	*guibc[NOMOREBUTTONS]={NULL,};

//					bc[menucount]->LFSTK_addMenus(ms,mainMenus[j].maxentrys);
int					bwidth=96;
int					spacing=bwidth+10;
int					col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
bool				mainloop=false;
int					maxGroups=20;
int					numGroups=0;
menuItemStruct		*groups;

bool callback(void *p,void* ud)
{
	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case NEWGROUP:
				break;

			case APPLY:
				break;
		}
	return(true);
}

int fileExists(char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
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
			return;
		}
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
							groups[cnt].userData=(void*)(long)cnt+1;
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
			printf(">>>%i<<<\n",(long)ud-1);
			printf(">>>%s<<<\n",groups[(long)ud-1].label);
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

	guibc[NEWGROUP]=new LFSTK_buttonClass(wc,"New",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	guibc[NEWGROUP]->LFSTK_setCallBack(NULL,callback,(void*)NEWGROUP);

	sx=col1;
	sy=10;

	mb=new LFSTK_menuButtonClass(wc,"Group",sx,sy,bwidth,24,NorthWestGravity);
	mb->LFSTK_setCallBack(NULL,NULL,NULL);
	mb->LFSTK_setStyle(EMBOSSEDBUTTON);
	mb->LFSTK_setLabelOriention(CENTRE);
	groups=new menuItemStruct[maxGroups];
	mb->LFSTK_setCallBack(NULL,bcb,groups);
	
	loadGroups();
	mb->LFSTK_addMenus(groups,numGroups);
	sy+=vspacing;

	sy+=vspacing;
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

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
