
/*

©keithhedger Thu 4 Jun 20:05:59 BST 2015 kdhedger68713@gmail.com


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
	bool			inTerm;
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
bool			mainloop=false;
bool			makestatic=false;
const char		*terminalCommand=NULL;

void entrySelectCB(Widget w,XtPointer data,XtPointer  garbage)
{
	int	cat=(int)(long)data/256;
	int ent=(int)(long)data-(cat*256);
	char	buffer[BUFFERSIZE];

	if(!streq(XtName(w),"Exit"))
		{
			//fprintf(stderr,"Name=%s\nExec=%s\n",mainMenus[cat].entry[ent].name,mainMenus[cat].entry[ent].exec);
			if(mainMenus[cat].entry[ent].inTerm==false)
				sprintf(buffer,"%s &",mainMenus[cat].entry[ent].exec);
			else
				sprintf(buffer,"%s %s &",terminalCommand,mainMenus[cat].entry[ent].exec);
				
			system(buffer);
		}
	else
		if(makestatic==true)
			XtAppSetExitFlag(XtWidgetToApplicationContext(w));
	mainloop=false;
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

int main(int argc,char *argv[])
{
	Widget			top;
	Widget			command;
	Widget			menu;
	Widget			entry;
	Widget			box;
	XtAppContext	app_con;
	int				x=0,y=0;
	Display			*display;
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
	XEvent			event;
	int				win=0,wout=0;
	bool			mdown=false;

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	setCatagories();
	makestatic=true;

/*
	if(argc>1)
		{
			if(argv[1][0]=='m')
				{
					if(XQueryPointer(display,DefaultRootWindow(display),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
						{
							x=win_x_return-10;
							y=win_y_return-10;
							makestatic=false;
						}
				}
			else
				{
					if(argc>1)
						x=atoi(argv[1]);
					if(argc>2)
						y=atoi(argv[2]);
				}
		}


*/
	
	if(XQueryPointer(display,DefaultRootWindow(display),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
						{
							x=win_x_return-10;
							y=win_y_return-10;
							makestatic=false;
						}
	terminalCommand=argv[1];

	top=XtVaAppInitialize(&app_con,"appmenu",NULL,ZERO,&argc,argv,fallback_resources,NULL);

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

	if(makestatic==true)
		{
			command = XtVaCreateManagedWidget("Exit", commandWidgetClass, box,NULL);
		    XtAddCallback(command, XtNcallback,entrySelectCB, NULL);
		}
   
	XtVaSetValues(top,XmNmwmDecorations,0,NULL);
	XtVaSetValues(top,XmNoverrideRedirect,TRUE,NULL);
	XtVaSetValues(top,XmNx,x,XmNy,y,NULL);

	XtRealizeWidget(top);
	if(makestatic==true)
		{
			XtAppMainLoop(app_con);
		}
	else
		{
			mainloop=true;
			while(mainloop==true)
				{
					XtAppNextEvent(app_con,&event);
					switch(event.type)
						{
							case ButtonPress:
								mdown=true;
								break;
							case ButtonRelease:
								mdown=false;
								break;
							case LeaveNotify:
								wout=event.xcrossing.subwindow;
								break;
							case EnterNotify:
								win=event.xcrossing.subwindow;
								break;
						}
					if(((win==0) && (wout!=0)) && (mdown==false))
						mainloop=false;
					XtDispatchEvent(&event);
				}

			XtDestroyWidget(top);
			while(XtAppPending(app_con)!=0)
				{
					XtAppNextEvent(app_con,&event);
					XtDispatchEvent(&event);
				}
		}
	XtAppSetExitFlag(app_con);
	return(0);
}

