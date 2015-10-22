/*
 *
 * ©K. D. Hedger. Fri 31 Jul 17:35:44 BST 2015 kdhedger68713@gmail.com

 * This file (appMenu.cpp) is part of LFSWManager.

 * LFSWManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSWManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWManager.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include <LFSTKMenuButton.h>

#include "appmenu.h"

//const char				*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Audio","Video","Office","Shell","Game","System",NULL};
//const char				*catImageNames[]={"preferences-desktop","applications-utilities","applications-development","applications-science","applications-graphics","applications-internet","applications-multimedia","applications-multimedia","applications-multimedia","applications-office","applications-engineering","applications-games","applications-system",NULL};
const char				*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Office","Shell","Game","System",NULL};
const char				*catImageNames[]={"preferences-desktop","applications-utilities","applications-development","applications-science","applications-graphics","applications-internet","applications-multimedia","applications-office","applications-engineering","applications-games","applications-system",NULL};

LFSTK_menuButtonClass	*appButton=NULL;
menuItemStruct			*catagoryMenus;
menuItemStruct			*catagorySubMenus[MAXCATS];
int						subMenusCnt[MAXCATS];
int						catPtr[MAXCATS];

bool callback(void *p,void* ud)
{
	menuEntryStruct	*menu;
	char			*command;

	if(ud==NULL)
		return(true);;

	menu=(menuEntryStruct*)((menuItemStruct*)ud)->userData;

/*
	char			*path;
	geometryStruct	*geom=appButton->LFSTK_getGeom();

	XFlush(mainwind->display);
	asprintf(&path,"%s \"%s\" %i %i",APPMENUPATH,terminalCommand,geom->x,geom->y+geom->h);
	system(path);
	free(path);
	delete geom;
*/
	if(menu->inTerm==false)
		asprintf(&command,"%s &",menu->exec);
	else
		asprintf(&command,"%s %s &",terminalCommand,menu->exec);

	system(command);
	free(command);
	return(true);
}
/*
//menu button with sub menu
	mainMenusWithSubs=new menuItemStruct[MAXMAINMENUS];
	for(int j=0;j<MAXMAINMENUS;j++)
		{
			mainMenusWithSubs[j].label=mainMenuNames[j];
			mainMenusWithSubs[j].userData=NULL;
			mainMenusWithSubs[j].bc=NULL;
			mainMenusWithSubs[j].subMenus=NULL;
			mainMenusWithSubs[j].useIcon=false;
		}
//sub menus
	subMenus=new menuItemStruct[MAXSUBMENUS];
	for(int j=0;j<MAXSUBMENUS;j++)
		{
			subMenus[j].label=subMenuNames[j];
			subMenus[j].userData=NULL;
			subMenus[j].bc=NULL;
			subMenus[j].subMenus=NULL;
			subMenus[j].useIcon=true;
			subMenus[j].icon[0]=ic[0];
			subMenus[j].icon[1]=ic[1];
		}
	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"/usr/share/icons/gnome/48x48/devices/audio-speakers.png",&subMenus[2].icon[0],&subMenus[2].icon[1],16);
//add sub menus
	mainMenusWithSubs[3].subMenus=subMenus;
	mainMenusWithSubs[3].subMenuCnt=MAXSUBMENUS;

	mainMenusWithSubs[1].subMenus=subMenus;
	mainMenusWithSubs[1].subMenuCnt=MAXSUBMENUS;
	
*/
#include <ftw.h>
#include <fnmatch.h>

int	catagoryCnt=0;
int		catcnt=0;

int ftwCallback(const char *fpath,const struct stat *sb,int typeflag)
{
	char	command[512];
	char	*splitstr=NULL;
	FILE	*fp=NULL;
	bool	interm=false;
	int		mycatcnt=0;
	bool	gotcat;
	bool	gotname;
	bool	goticon;
	bool	gotexec;
	char	*holdname=NULL;
	char	*holdicon=NULL;
	char	*holdexec=NULL;
	bool	nodisplay=false;

	if(typeflag!=FTW_F)
		return(0);

	sprintf(command,"cat %s",fpath);
	gotcat=false;
	gotname=false;
	goticon=false;
	gotexec=false;
	interm=false;
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(command,512,fp))
				{
					if(command[strlen(command)-1]=='\n')
						command[strlen(command)-1]=0;
					splitstr=NULL;
					splitstr=strtok(command,"=");
					if(splitstr!=NULL)
						{
							if(strncmp(splitstr,"Categories",10)==0)
								{
									mycatcnt=0;
									splitstr=strtok(NULL,"=");
									while(myCats[mycatcnt]!=NULL)
										{
											if(strstr(splitstr,myCats[mycatcnt])!=NULL)
												{
													gotcat=true;
													break;
												}
											mycatcnt++;
										}
									}

							if(strcmp(splitstr,"Name")==0)
								{
									splitstr=strtok(NULL,"=");
									gotname=true;
									holdname=strdup(splitstr);
								}

							if(strcmp(splitstr,"Icon")==0)
								{
									splitstr=strtok(NULL,"=");
									holdicon=strdup(splitstr);
								}
							if(strcmp(splitstr,"Exec")==0)
								{
									splitstr=strtok(NULL,"=");
									holdexec=strdup(splitstr);
								}
							if(strcmp(splitstr,"NoDisplay")==0)
								{
									splitstr=strtok(NULL,"=");
									if(strcasecmp(splitstr,"true")==0)
										nodisplay=true;
								}
							if(strcmp(splitstr,"Terminal")==0)
								{
									splitstr=strtok(NULL,"=");
									if(strcasecmp(splitstr,"true")==0)
										interm=true;
									else
										interm=false;
								}
						}
				}

			if((gotcat==true) && (nodisplay==false))
				{
					menuEntryStruct	*ms=new menuEntryStruct;

					catagorySubMenus[catPtr[mycatcnt]][subMenusCnt[catPtr[mycatcnt]]].userData=(void*)ms;
					if(gotname==true)
						{
							catagorySubMenus[catPtr[mycatcnt]][subMenusCnt[catPtr[mycatcnt]]].label=holdname;
							ms->name=holdname;
						}

					if(holdexec!=NULL)
						ms->exec=holdexec;
					if(interm==true)
						ms->inTerm=true;
					else
						ms->inTerm=false;
					subMenusCnt[catPtr[mycatcnt]]++;
				}

			pclose(fp);
		}
	//free(tbuffer);
/*

	char	*tbuffer=NULL;
	int retval=0;
	if(typeflag!=FTW_F)
		return(0);

//	printf("----sed -n '/categories.*%s/Ip' %s---\n",myCats[catcnt],fpath);
	tbuffer=mainwind->globalLib->LFSTK_oneLiner("sed -n '/categories.*%s/Ip' %s",myCats[catcnt],fpath);
	if(tbuffer!=NULL)
		{
		printf(">>>%s - %s<<<\n",tbuffer,fpath);
		if(strlen(tbuffer)>0)
			{
			free(tbuffer);
			retval=1;
			}
			
		}
	return retval;
*/
	return(0);
}

void addEntries(void)
{
	for(int j=0;j<catagoryCnt;j++)
		{
			subMenusCnt[j]=0;
			for(int k=0;k<MAXENTRYS;k++)
				{
			//catagorySubMenus[j]=new menuItemStruct[MAXENTRYS];
					catagorySubMenus[j][k].label=NULL;
					catagorySubMenus[j][k].userData=NULL;
					catagorySubMenus[j][k].bc=NULL;
					catagorySubMenus[j][k].subMenus=NULL;
					catagorySubMenus[j][k].useIcon=false;
				}
		}
	ftw("/usr/share/applications",ftwCallback,16);
	for(int j=0;j<MAXCATS;j++)
		{
			catagoryMenus[j].subMenus=catagorySubMenus[j];
			catagoryMenus[j].subMenuCnt=subMenusCnt[j];
		}
}

void addCatagories(void)
{
	char	*tbuffer=NULL;

	catagoryCnt=0;
	catcnt=0;
	catagoryMenus=new menuItemStruct[MAXCATS];
	while(myCats[catcnt]!=NULL)
		{
			//if(ftw("/usr/share/applications",ftwCallback,16)==1);
			tbuffer=mainwind->globalLib->LFSTK_oneLiner("find /usr/share/applications -follow -type f -print0 |xargs -0 sed -n '/categories.*%s/Ip'",myCats[catcnt]);
			if(tbuffer!=NULL)
				{
					if(strlen(tbuffer)>0)
						{
							catagorySubMenus[catagoryCnt]=new menuItemStruct[MAXENTRYS];
							catagoryMenus[catagoryCnt].label=myCats[catcnt];
							catagoryMenus[catagoryCnt].userData=NULL;
							catagoryMenus[catagoryCnt].bc=NULL;
							catagoryMenus[catagoryCnt].subMenus=NULL;
							catagoryMenus[catagoryCnt].subMenuCnt=0;
							catagoryMenus[catagoryCnt].useIcon=false;
							catPtr[catcnt]=catagoryCnt;
							catagoryCnt++;
						}
					free(tbuffer);
				}
			catcnt++;
		}
}

int addAppmenu(int x,int y,int grav)
{
	int addto=mainwind->font->ascent+mainwind->font->descent+8;
	int maxwid=0;
	int	catnum=0;
	int	xpos=0;
	int width=panelHeight+2;
	int	retval=width;

	if(appButton!=NULL)
		{
			printError("Duplicate app menu");
			return(0);
		}

	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	appButton=new LFSTK_menuButtonClass(mainwind,"",xpos,0,panelHeight+2,panelHeight,grav);
	appButton->LFSTK_setIconFromPath("/usr/share/pixmaps/LFSTux.png",panelHeight-6);
	addCatagories();
	addEntries();
	appButton->LFSTK_addMenus(catagoryMenus,catagoryCnt);
	XMapWindow(mainwind->display,appButton->LFSTK_getWindow());
	appButton->LFSTK_setCallBack(NULL,callback,NULL);
	appButton->LFSTK_clearWindow();
	return(retval);
}
