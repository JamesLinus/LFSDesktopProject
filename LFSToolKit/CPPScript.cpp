#if 0

#©keithhedger Mon 20 Jul 14:09:10 BST 2015 kdhedger68713@gmail.com

g++ "$0" -I/tmp/AAA/usr/local/include -L/tmp/AAA/usr/local/lib -llfstoolkit -lXm $(pkg-config --cflags --libs xt xext ice sm x11 xft) -lXm -lXaw3d -lXt -lXext -lICE -lSM -lX11||exit 1
./a.out "$@"
retval=$?
rm ./a.out
exit $retval
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include "LFSTKMenuButton.h"

#define BUFFERSIZE 2048
#define MAXCATS 14
#define MAXENTRYS 500

struct menuEntryStruct
{
	char				*name;
	char				*exec;
	bool				inTerm;
};

struct appMenuStruct
{
	const char			*name;
	menuEntryStruct		entry[MAXENTRYS];
	int					maxentrys;
};

const char				*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Audio","Video","Office","Shell","Game","System",NULL};
const char				*terminalCommand="xterm -e ";
appMenuStruct			mainMenus[MAXCATS];
bool					mainloop=false;
bool					makestatic=false;
LFSTK_windowClass		*wc;
LFSTK_menuButtonClass	*bc[MAXCATS];

void bcb(void *p,void* ud)
{
	char			buffer[BUFFERSIZE];
	menuEntryStruct	*menuitem;

	if((long)ud<0)
		return;

	menuitem=(menuEntryStruct*)ud;

	if(ud>0)
		{
			if(menuitem->inTerm==false)
				sprintf(buffer,"%s &",menuitem->exec);
			else
				sprintf(buffer,"%s %s &",terminalCommand,menuitem->exec);

			system(buffer);

			delete wc;
			for(int j=0;j<MAXCATS;j++)
				if(bc[j]!=NULL)
					delete bc[j];
			exit(0);
		}
}

void setCatagories(void)
{
	FILE	*fp;
	char	buffer[BUFFERSIZE];
	char	*splitstr=NULL;
	int		mycatcnt;
	bool	foundmatch;
	FILE	*filedata;
	int		foundcatmatch=-1;
	char	foundnamebuffer[BUFFERSIZE];
	char	foundexecbuffer[BUFFERSIZE];
	bool	overridefound=false;
	bool	interm=false;

	for(int j=0; j<MAXCATS; j++)
		{
			mainMenus[j].name=NULL;
			mainMenus[j].maxentrys=0;
			bc[j]=NULL;
			for(int k=0; k<MAXENTRYS; k++)
				{
					mainMenus[j].entry[k].name=NULL;
					mainMenus[j].entry[k].exec=NULL;
					mainMenus[j].entry[k].inTerm=false;
				}
		}

	fp=popen("find /usr/share/applications -name \"*.desktop\"|sort","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					buffer[strlen(buffer)-1]=0;
					filedata=fopen(buffer,"r");
					if(filedata!=NULL)
						{
							foundmatch=false;
							overridefound=false;
							interm=false;
							while(fgets(buffer,BUFFERSIZE,filedata))
								{
									if(buffer[strlen(buffer)-1]=='\n')
										buffer[strlen(buffer)-1]=0;
									splitstr=NULL;
									splitstr=strtok(buffer,"=");

									if(splitstr!=NULL)
										{
											if(strncmp(splitstr,"Categories",10)==0)
												{
													mycatcnt=0;
													foundcatmatch=-1;
													splitstr=strtok(NULL,"=");
													while(myCats[mycatcnt]!=NULL)
														{
															if(strstr(splitstr,myCats[mycatcnt])!=NULL)
																{
																	foundmatch=true;
																	foundcatmatch=mycatcnt;
																	break;
																}
															mycatcnt++;
														}
												}
											if(strcmp(splitstr,"Name")==0)
												{
													splitstr=strtok(NULL,"=");
													sprintf(foundnamebuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"Exec")==0)
												{
													splitstr=strtok(NULL,"=");
													sprintf(foundexecbuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"NoDisplay")==0)
												{
													splitstr=strtok(NULL,"=");
													if(strcasecmp(splitstr,"true")==0)
														overridefound=true;
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

							if((foundmatch==true) && (overridefound==false))
								{
									mainMenus[foundcatmatch].name=myCats[foundcatmatch];
									char *ptr=strstr(foundexecbuffer,"%");
									while(ptr!=NULL)
										{
											sprintf(ptr,"  ");
											ptr=strstr(foundexecbuffer,"%");
										}
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].name=strdup(foundnamebuffer);
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].exec=strdup(foundexecbuffer);
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].inTerm=interm;
									mainMenus[foundcatmatch].maxentrys++;
								}
							fclose(filedata);
						}
				}
			pclose(fp);
		}
}

bool inWindow(void)
{
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;

	if(XQueryPointer(wc->display,wc->rootWindow,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
		{
			geometryStruct *g=wc->LFSTK_getGeom();
			if((root_x_return>g->x) && (root_x_return<(g->x+g->w)) && (root_y_return>g->y) && (root_y_return<(g->y+g->h)))
				return(true);
		}
	return(false);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				menucount=0;
	menuItemStruct *ms,*pms;
	int				sx=0;
	int				sy=0;
	Window			dumpwind;
	int				dumpint;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
	Display			*disp;

	setCatagories();
	makestatic=true;

	disp=XOpenDisplay(NULL);
	if(disp==NULL)
		exit(1);
	
	if(XQueryPointer(disp,DefaultRootWindow(disp),&dumpwind,&dumpwind,&dumpint,&dumpint,&win_x_return,&win_y_return,&mask_return)==true)
		{
			sx=win_x_return-10;
			sy=win_y_return-10;
			//makestatic=false;
		}
//	terminalCommand=argv[1];

	//wc=new LFSTK_windowClass(sx,sy,800,400,true,"rgb:00/00/00","rgb:80/80/80");
	wc=new LFSTK_windowClass(sx,sy,800,400,true);
	wc->LFSTK_setDecorated(true);
//	wc->LFSTK_setFontColourName(FONTNORMALCOL,"white");
//	wc->LFSTK_setFontColourName(FONTHILITECOL,"black");
//	wc->LFSTK_setFontColourName(FONTACTIVECOL,"white");

//	wc->LFSTK_setColourName(NORMALCOLOUR,"#5E3300");
//	wc->LFSTK_setColourName(PRELIGHTCOLOUR,"#6B4C26");
//	wc->LFSTK_setColourName(ACTIVECOLOUR,"#412502");

	sx=0;
	sy=0;
	for(int j=0; j<MAXCATS; j++)
		{
			bc[j]=NULL;
			if(mainMenus[j].name!=NULL)
				{
	//				bc[menucount]=new LFSTK_menuButtonClass(wc,(char*)mainMenus[j].name,sx,sy,100,28,0,wc->colourNames[NORMALCOLOUR].name,wc->colourNames[PRELIGHTCOLOUR].name,wc->colourNames[ACTIVECOLOUR].name);
					bc[menucount]=new LFSTK_menuButtonClass(wc,(char*)mainMenus[j].name,sx,sy,100,28,0);
					bc[menucount]->LFSTK_setCallBack(NULL,bcb,(void*)0-(j+1));
					bc[menucount]->LFSTK_setStyle(EMBOSSEDBUTTON);
					bc[menucount]->LFSTK_setLabelOriention(CENTRE);
					bc[menucount]->LFSTK_setMenuItemColours(NORMALCOLOUR,"blue");
					bc[menucount]->LFSTK_setMenuItemColours(PRELIGHTCOLOUR,"red");
					bc[menucount]->LFSTK_setMenuItemColours(ACTIVECOLOUR,"#f00080");

					XMapWindow(wc->display,bc[menucount]->window);
					sy+=28;
					ms=new menuItemStruct[mainMenus[j].maxentrys];
					pms=ms;
					for(int k=0; k<mainMenus[j].maxentrys; k++)
						{
							pms->label=mainMenus[j].entry[k].name;
							pms->userData=(void*)&mainMenus[j].entry[k];
							pms++;
						}
					bc[menucount]->LFSTK_addMenus(ms,mainMenus[j].maxentrys);
					menucount++;
				}
		}
	XResizeWindow(wc->display,wc->window,100,sy);
	wc->LFSTK_resizeWindow(100,sy);
	wc->LFSTK_clearWindow();
	XMapWindow(wc->display,wc->window);

	mainloop=true;
	while(mainloop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);

			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						mainloop=inWindow();
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;
				}
		}

	delete wc;
	for(int j=0;j<MAXCATS;j++)
		if(bc[j]!=NULL)
			delete bc[j];
	return 0;
}