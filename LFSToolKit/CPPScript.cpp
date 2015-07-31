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
//#include <X11/Xft/Xft.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include "LFSTKMenuButton.h"
//#include <LFSTKGlobals.h>
#define streq(a,b)	( strcmp((a),(b))==0 )
#define BUFFERSIZE 2048
#define MAXCATS 500
#define MAXENTRYS 500

struct			menuEntryStruct
{
	char			*name;
	char			*exec;
};

struct			appMenuStruct
{
	const char		*name;
	menuEntryStruct	entry[MAXENTRYS];
	int				maxentrys;
};

const char		*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Audio","Video","Office","Shell","Game","System",NULL};

appMenuStruct	mainMenus[MAXCATS];
bool			mainloop=false;
bool			makestatic=false;
LFSTK_windowClass *wc;

void bcb(void *p,void* ud)
{
	if((long)ud<0)
		return;
	menuEntryStruct	*menuitem;
	menuitem=(menuEntryStruct*)ud;

	if(ud>0)
		{
			printf(">>%s<<\n",menuitem->exec);
			delete wc;
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
	int		foundcatmatch;
	char	foundnamebuffer[BUFFERSIZE];
	char	foundexecbuffer[BUFFERSIZE];
	bool	overridefound;

	for(int j=0; j<MAXCATS; j++)
		{
			mainMenus[j].name=NULL;
			mainMenus[j].maxentrys=0;
			for(int k=0; k<MAXENTRYS; k++)
				{
					mainMenus[j].entry[k].name=NULL;
					mainMenus[j].entry[k].exec=NULL;

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
	XEvent event;
	setCatagories();
	LFSTK_buttonClass *bc;
	LFSTK_menuButtonClass *bc1;
	menuItemStruct *ms,*pms;
	int sx=0;
	int sy=0;

	wc=new LFSTK_windowClass(500,1100,800,400,true,"rgb:00/00/00","rgb:80/80/80");
	wc->LFSTK_setDecorated(true);

	for(int j=0; j<MAXCATS; j++)
		{
			if(mainMenus[j].name!=NULL)
				{
					bc1=new LFSTK_menuButtonClass(wc,(char*)mainMenus[j].name,sx,sy,100,28,0,"rgb:a0/a0/a0","rgb:d0/d0/d0","rgb:80/80/80");
					bc1->LFSTK_setCallBack(NULL,bcb,(void*)0-(j+1));
					bc1->LFSTK_setStyle(EMBOSSEDBUTTON);
					bc1->LFSTK_setLabelOriention(CENTRE);

					XMapWindow(wc->display,bc1->window);
					sy+=28;
					ms=new menuItemStruct[mainMenus[j].maxentrys];
					pms=ms;
					for(int k=0; k<mainMenus[j].maxentrys; k++)
						{
							pms->label=strdup(mainMenus[j].entry[k].name);
							pms->userData=(void*)&mainMenus[j].entry[k];
							pms++;
						}
					bc1->LFSTK_addMenus(ms,mainMenus[j].maxentrys);
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
	return 0;
}