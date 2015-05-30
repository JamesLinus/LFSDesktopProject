
//Mon 31 Jul 2006 12:30:55 BST
//
//LFSDesktop
//

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <libudev.h>

#include <sys/inotify.h>
#include <limits.h>
#include <poll.h>

#include <Xm/PushB.h>
#include <Xm/Form.h>

#include "config.h"

#include "globals.h"
#include "prefs.h"
#include "files.h"
#include "disks.h"
#include "graphics.h"

#define UNKNOWNARG -100

struct	option long_options[] =
{
	{"clean",0,0,'c'},
	{"theme",1,0,'t'},
	{"theme",0,0,'d'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0,0,0,0}
};

bool	needsRefresh=true;
bool	loop=true;

void printhelp(void)
{
	printf("Usage: lfsdesktop [OPTION]\n"
	       "A CLI application\n"
	       " -c,--clean		Clean disk info data\n"
	       " -t,--theme		Set theme\n"
	       " -d,--debug		Debug\n"
	       " -v,--version	output version information and exit\n"
	       " -h,-?,--help	print this help\n\n"
	       "Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

bool findIcon(int x,int y)
{

	for(int j=0;j<deskIconsCnt;j++)
		{
			if((x>=(deskIconsArray[j].x*gridSize+gridBorder))&&(x<=(deskIconsArray[j].x*gridSize+gridBorder)+iconSize)&&(y>=(deskIconsArray[j].y*gridSize+gridBorder))&&(y<=(deskIconsArray[j].y*gridSize+gridBorder)+iconSize))
				{
					fileDiskXPos=deskIconsArray[j].x;
					fileDiskYPos=deskIconsArray[j].y;
					foundDiskNumber=j;
					isDisk=!deskIconsArray[j].file;
					return(true);
				}
		}
	return(false);
}

void  alarmCallBack(int sig)
{
	signal(SIGALRM,SIG_IGN);
	needsRefresh=true;
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
}

void pushedButton(Widget w,XtPointer client_data,XmPushButtonCallbackStruct *cbs)
{
	long	what=(long)client_data;

	mountDisk(what);
	needsRefresh=true;
	loop=false;
}

Widget mountMenu(XEvent ev,XtAppContext *app)
{
	Widget	toplevel,form,mount,unmount,eject;
	int		dump=0;

	toplevel=XtVaAppInitialize(app,"Mount",NULL,0,&dump,NULL,NULL,NULL);

	form=XtVaCreateManagedWidget("form",xmFormWidgetClass,toplevel,NULL);

	mount=XtVaCreateManagedWidget("Mount",xmPushButtonWidgetClass,form,
				XmNtopAttachment,XmATTACH_FORM,
				XmNbottomAttachment,XmATTACH_NONE,
				XmNleftAttachment,XmATTACH_FORM,
				XmNrightAttachment,XmATTACH_FORM,
				NULL);

	unmount=XtVaCreateManagedWidget("Unmount",xmPushButtonWidgetClass,form,
				XmNtopWidget,mount,
				XmNtopAttachment,XmATTACH_WIDGET,
				XmNbottomAttachment,XmATTACH_NONE,
				XmNleftAttachment,XmATTACH_FORM,
				XmNrightAttachment,XmATTACH_FORM,
				NULL);

	eject=XtVaCreateManagedWidget("Eject",xmPushButtonWidgetClass,form,
				XmNtopWidget,unmount,
				XmNtopAttachment,XmATTACH_WIDGET,
				XmNbottomAttachment,XmATTACH_NONE,
				XmNleftAttachment,XmATTACH_FORM,
				XmNrightAttachment,XmATTACH_FORM,
				NULL);

	XtAddCallback(mount,XmNactivateCallback,(XtCallbackProc)pushedButton,(XtPointer)1);
	XtAddCallback(unmount,XmNactivateCallback,(XtCallbackProc)pushedButton,(XtPointer)2);
	XtAddCallback(eject,XmNactivateCallback,(XtCallbackProc)pushedButton,(XtPointer)3);

	XtVaSetValues(toplevel,XmNmwmDecorations,0,NULL);
	XtVaSetValues(toplevel,XmNoverrideRedirect,TRUE,NULL);
	XtVaSetValues(toplevel,XmNx,ev.xbutton.x,XmNy,ev.xbutton.y,NULL);
	XtRealizeWidget(toplevel);
	return(toplevel);
}

void doPopUp(XEvent ev)
{
	Widget			mountmenu;
	XEvent			event;
	XtAppContext	app;
	bool			foundicon;

	foundicon=findIcon(ev.xbutton.x,ev.xbutton.y);
	if(foundicon==false)
		return;

	if(isDisk==false)
		return;
	loop=true;

	mountmenu=mountMenu(ev,&app);
	/* enter processing loop */
	while(loop==true)
		{
			XtAppNextEvent(app,&event);
			switch(event.type)
				{
				case LeaveNotify:
				if(event.xcrossing.detail==NotifyAncestor)
					loop=false;
					break;
				}
			XtDispatchEvent(&event);
		}
	XtDestroyWidget(mountmenu);
	while(XtAppPending(app)!=0)
		{
			XtAppNextEvent(app,&event);
			XtDispatchEvent(&event);
		}
}

int main(int argc,char **argv)
{
	int				c;
	XEvent			ev;
	char			*command;
	FILE			*fw;
	char			*path;
	char			buffer[100];
	pid_t			pid=getpid();
	bool			done=true;
	Time			time=0;
	bool			firstClick=false;
	bool			foundIcon=false;
	pollfd			pollstruct;
	int				fd;
	pollfd			polldisks;
	int				fhfordisks;
	long			numRead=0;
	int				oldx=-1,oldy=-1;
	bool	buttonDown=false;
	int		oldboxx=-1,oldboxy=-1;
	bool	dragging=false;

	asprintf(&path,"%s/.config/LFS/pidfile",getenv("HOME"));
	fw=fopen(path,"r");
	if(fw!=NULL)
		{
			fgets(buffer,100,fw);
			kill(atoi(buffer),SIGKILL);
			fclose(fw);
		}

	fw=fopen(path,"w");
	if(fw!=NULL)
		{
			fprintf(fw,"%i",pid);
			fclose(fw);
		}
	free(path);
	nice(15);

	cairo_surface_t *sfc;

	signal(SIGALRM,alarmCallBack);

	done=true;
	asprintf(&diskInfoPath,"%s/.config/LFS/disks",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",diskInfoPath);
	system(command);
	free(command);

	asprintf(&cachePath,"%s/.config/LFS/cache",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",cachePath);
	system(command);
	free(command);

	asprintf(&desktopPath,"%s/Desktop",getenv("HOME"));

	asprintf(&prefsPath,"%s/.config/LFS/lfsdesktop.rc",getenv("HOME"));
	loadVarsFromFile(prefsPath,desktopPrefs);

	while (1)
		{
			int option_index=0;
			c=getopt_long (argc,argv,"v?hcdt:",long_options,&option_index);
			if (c==-1)
				break;

			switch (c)
				{
				case 'd':
					debugDeskFlag=true;
					break;

				case 'c':
					asprintf(&command,"rm %s/*",diskInfoPath);
					system(command);
					free(command);
					asprintf(&command,"rm %s/*",cachePath);
					system(command);
					free(command);
					break;

				case 't':
					if(iconTheme!=NULL)
						free(iconTheme);
					iconTheme=strdup(optarg);
					break;

				case 'v':
					printf("lfsdesktop %s\n",VERSION);
					return 0;
					break;

				case '?':
				case 'h':
					printhelp();
					return 0;
					break;

				default:
					fprintf(stderr,"?? Unknown argument ??\n");
					return UNKNOWNARG;
					break;
				}
		}

	if (optind < argc)
		{
			printf("non-option ARGV-elements: ");
			while (optind < argc)
				printf("%s ",argv[optind++]);
			printf("\n");
		}

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	XSynchronize(display,true);
	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	screen=DefaultScreen(display);
	rootWin=DefaultRootWindow(display);
	visual=DefaultVisual(display,screen);
	cm=DefaultColormap(display,screen);

	createDesktopWindow();

	sfc=cairo_xlib_surface_create(display,drawOnThis,visual,displayWidth,displayHeight);
	cairo_xlib_surface_set_size(sfc,displayWidth,displayHeight);
	cr=cairo_create(sfc);

	gc=XCreateGC(display,drawOnThis,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask);

	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);

	hcreate(100);

	createColours();

	xCnt=displayWidth/gridSize;
	yCnt=displayHeight/gridSize;

	xySlot=(int**)malloc(xCnt*sizeof(int*));
	for(int j=0; j<xCnt; j++)
		xySlot[j]=(int*)malloc(yCnt*sizeof(int));

	for(int yy=0; yy<yCnt; yy++)
		for(int xx=0; xx<xCnt; xx++)
			xySlot[xx][yy]=0;

	fd=inotify_init();
	pollstruct.fd =fd;
	pollstruct.events=POLLIN;
	pollstruct.revents=0;
	inotify_add_watch(fd,desktopPath,IN_CREATE|IN_DELETE|IN_MODIFY);

	fhfordisks=inotify_init();
	polldisks.fd=fhfordisks;
	polldisks.events=POLLIN;
	polldisks.revents=0;
	inotify_add_watch(fhfordisks,"/dev/disk/by-uuid",IN_CREATE|IN_DELETE|IN_MODIFY);

	deskIconsArray=(deskIcons*)calloc(deskIconsMaxCnt,sizeof(deskIcons));
	deskIconsCnt=0;
	asprintf(&command,"%s/Home",cachePath);

	if(fileExists(command)!=0)
		{
			deskIconsCnt=1;
			deskIconsArray[0].x=0;
			deskIconsArray[0].y=0;
			deskIconsArray[0].installed=true;
			deskIconsArray[0].label=strdup("Home");
			deskIconsArray[0].mime=strdup("inode/directory");
			deskIconsArray[0].mountpoint=strdup(getenv("HOME"));
			deskIconsArray[0].file=true;
			saveInfofile(CACHEFOLDER,deskIconsArray[0].label,deskIconsArray[0].mime,deskIconsArray[0].mountpoint,NULL,NULL,deskIconsArray[0].x,deskIconsArray[0].y);
			xySlot[0][0]=1;
		}
	else
		readDesktopFile("Home");
	free(command);

	fillDesk();

	alarm(refreshRate);
	XdbeSwapBuffers(display,&swapInfo,1);

	while(done)
		{
			if(needsRefresh==true)
				{
					int ret=poll(&pollstruct,POLLIN,20);
					if(ret!=0)
						{
							numRead=read(fd,buffer,100);
							if(numRead>0)
								fillDesk();
						}
					
					ret=poll(&polldisks,POLLIN,20);
					if(ret!=0)
						{
							numRead=read(fhfordisks,buffer,100);
							if(numRead>0)
								fillDesk();
						}

					drawIcons();
					XdbeSwapBuffers(display,&swapInfo,1);
					needsRefresh=false;
				}

			if(dragging==false)
				usleep(25000);

			XCheckWindowEvent(display,rootWin,ButtonPress|ButtonReleaseMask|PointerMotionMask,&ev);

			switch(ev.type)
				{
				case ButtonPress:
					if(ev.xbutton.button==Button3)
						{
							doPopUp(ev);
							break;
						}
					if(ev.xbutton.button!=Button1)
						break;
					dragging=true;
					buttonDown=true;
					if(firstClick==false)
						{
							firstClick=true;
							time=ev.xbutton.time;
						}
					else
						{
							firstClick=false;
							if(ev.xbutton.time-time<800)
								{
									mountDisk(1);
									needsRefresh=true;
								}
							else
								{
									time=ev.xbutton.time;
									firstClick=true;
								}
						}

					if(foundIcon==false)
						{
							oldx=-1;
							oldy=-1;
							foundDiskNumber=-1;
							foundIcon=findIcon(ev.xbutton.x,ev.xbutton.y);
						}

					if(foundIcon==true)
						{
							oldx=fileDiskXPos;
							oldy=fileDiskYPos;
							oldboxx=ev.xbutton.x;
							oldboxy=ev.xbutton.y;
						}
					else
						{
							fileDiskLabel=NULL;
							fileDiskUUID=NULL;
							fileDiskType=NULL;
							oldx=-1;
							oldy=-1;
							foundDiskNumber=-1;
						}

					break;
				case ButtonRelease:
					if(ev.xbutton.button!=Button1)
							break;

					dragging=false;
					buttonDown=false;
					if(foundIcon==true)
						{
							int newx,newy;
							newx=(ev.xbutton.x-gridBorder)/gridSize;
							newy=(ev.xbutton.y-gridBorder)/gridSize;
							foundIcon=false;
							if(xySlot[newx][newy]==0)
								{
									xySlot[oldx][oldy]=0;
									deskIconsArray[foundDiskNumber].x=newx;
									deskIconsArray[foundDiskNumber].y=newy;
									xySlot[newx][newy]=1;
									if(isDisk==true)
										saveInfofile(DISKFOLDER,deskIconsArray[foundDiskNumber].label,NULL,NULL,deskIconsArray[foundDiskNumber].uuid,(char*)iconDiskType[deskIconsArray[foundDiskNumber].iconhint],newx,newy);
									else
										saveInfofile(CACHEFOLDER,deskIconsArray[foundDiskNumber].label,deskIconsArray[foundDiskNumber].mime,deskIconsArray[foundDiskNumber].mountpoint,NULL,NULL,newx,newy);
									fileDiskLabel=NULL;
									fileDiskUUID=NULL;
									fileDiskXPos=-1;
									fileDiskYPos=-1;
									fileDiskType=NULL;
									needsRefresh=true;
								}

							XSetForeground(display,gc,0);
							XSetFillStyle(display,gc,FillSolid);
							newx=((ev.xbutton.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
							newy=((ev.xbutton.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
							XDrawRectangle(display,rootWin,gc,newx,newy,gridSize,gridSize);

							fileDiskLabel=NULL;
							fileDiskUUID=NULL;
							fileDiskType=NULL;
							oldx=-1;
							oldy=-1;
							alarm(refreshRate);
						}

					break;
				case MotionNotify:
				{
					int newboxx,newboxy;

					if(foundIcon==true && buttonDown==true)
						{
							alarm(0);
							if((ev.xmotion.x>oldboxx+iconSize) || (ev.xmotion.x<oldboxx-iconSize) || (ev.xmotion.y>oldboxy+iconSize) || (ev.xmotion.y<oldboxy-iconSize))
								{
									int obx,oby;

									XSetForeground(display,gc,0);
									XSetFillStyle(display,gc,FillSolid);
									obx=((oldboxx-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									oby=((oldboxy-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									XDrawRectangle(display,rootWin,gc,obx,oby,gridSize,gridSize);

									oldboxx=ev.xmotion.x;
									oldboxy=ev.xmotion.y;

									XSetForeground(display,gc,labelBackground);
									XSetFillStyle(display,gc,FillSolid);
									newboxx=((ev.xmotion.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									newboxy=((ev.xmotion.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									XDrawRectangle(display,rootWin,gc,newboxx,newboxy,gridSize,gridSize);
								}
						}
				}
				break;

				default:
					break;
				}
			ev.type=-1;
		}

	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	for(int j=0; j<xCnt; j++)
		free(xySlot[j]);

	return 0;
}

