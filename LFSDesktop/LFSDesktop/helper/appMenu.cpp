#if 0

#©keithhedger Thu 4 Jun 20:05:59 BST 2015 kdhedger68713@gmail.com

g++ "$0" -o appMenu -lXm $(pkg-config --cflags --libs xt xext ice sm x11) -lXm -lXaw7 -lXt -lXext -lICE -lSM -lX11 ||exit 1
./appMenu "$@"
retval=$?
       rm ./appMenu
       exit $retval

#endif

       /*
        * A simple Simple Menu Widget example.
        *
        * November 30,1989 - Chris D. Peterson
        */

       /*
        * $XConsortium: xmenu1.c,v 1.8 89/12/15 19:33:15 kit Exp $
        *
        * Copyright 1989 Massachusetts Institute of Technology
        *
        * Permission to use,copy,modify,distribute,and sell this software and its
        * documentation for any purpose is hereby granted without fee,provided that
        * the above copyright notice appear in all copies and that both that
        * copyright notice and this permission notice appear in supporting
        * documentation,and that the name of M.I.T. not be used in advertising or
        * publicity pertaining to distribution of the software without specific,
        * written prior permission.  M.I.T. makes no representations about the
        * suitability of this software for any purpose.  It is provided "as is"
        * without express or implied warranty.
        *
        * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,INCLUDING ALL
        * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,IN NO EVENT SHALL M.I.T.
        * BE LIABLE FOR ANY SPECIAL,INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
        * WHATSOEVER RESULTING FROM LOSS OF USE,DATA OR PROFITS,WHETHER IN AN ACTION
        * OF CONTRACT,NEGLIGENCE OR OTHER TORTIOUS ACTION,ARISING OUT OF OR IN
        * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
        */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>

#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>

#include <Xm/Xm.h>

#define streq(a,b)	( strcmp((a),(b)) == 0 )
#define BUFFERSIZE 512
#define MAXCATS 50
#define MAXENTRYS 50

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

String			fallback_resources[]=
{
					(char*)"*box*.background:		grey",
					(char*)"*box.Cancel.background:	grey40",
					(char*)"*box.*.width:			100",
					NULL,
};

appMenuStruct	mainMenus[MAXCATS];

void entrySelectCB(Widget w,XtPointer data,XtPointer  garbage)
{
	int	cat=(int)(long)data/256;
	int ent=(int)(long)data-(cat*256);
	char	buffer[BUFFERSIZE];

	if(!streq(XtName(w),"Cancel"))
		{
			//fprintf(stderr,"Name=%s\nExec=%s\n",mainMenus[cat].entry[ent].name,mainMenus[cat].entry[ent].exec);
			sprintf(buffer,"%s &",mainMenus[cat].entry[ent].exec);
			system(buffer);
		}
	XtAppSetExitFlag(XtWidgetToApplicationContext(w));
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

int main(int argc,char *argv[])
{
	Widget			top;
	Widget			command;
	Widget			menu;
	Widget			entry;
	Widget			box;
	XtAppContext	app_con;
	int				x=0,y=0;

	setCatagories();

	if(argc>1)
		x=atoi(argv[1]);
	if(argc>2)
		y=atoi(argv[2]);

	top=XtVaAppInitialize(&app_con,"Xmenu1",NULL,ZERO,&argc,argv,fallback_resources,NULL);

	box=XtVaCreateManagedWidget("box",boxWidgetClass,top,NULL);

	for(int j=0; j<MAXCATS; j++)
		{
			if(mainMenus[j].name!=NULL)
				{
					command=XtVaCreateManagedWidget(mainMenus[j].name,menuButtonWidgetClass,box,NULL);
					menu=XtVaCreatePopupShell("menu",simpleMenuWidgetClass,command,NULL);
					for(int k=0; k<mainMenus[j].maxentrys; k++)
						{
							entry=XtVaCreateManagedWidget(mainMenus[j].entry[k].name,smeBSBObjectClass,menu,NULL);
							XtAddCallback(entry,XtNcallback,entrySelectCB,(XtPointer)(long)(j*256+k));
						}
				}
		}

	command = XtVaCreateManagedWidget("Cancel", commandWidgetClass, box,NULL);
    XtAddCallback(command, XtNcallback,entrySelectCB, NULL);
   
	XtVaSetValues(top,XmNmwmDecorations,0,NULL);
	XtVaSetValues(top,XmNoverrideRedirect,TRUE,NULL);
	XtVaSetValues(top,XmNx,x,XmNy,y,NULL);

	XtRealizeWidget(top);
	XtAppMainLoop(app_con);
	XtAppSetExitFlag(app_con);
	return(0);
}

