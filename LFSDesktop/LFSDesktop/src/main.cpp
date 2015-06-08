
//Mon 31 Jul 2006 12:30:55 BST
//
//LFSDesktop
//

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/MenuButton.h>

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
#include <X11/Xaw/Box.h>

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
	{"debug",0,0,'d'},
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

void doCustomIcon(void)
{
	char buffer[MAXBUFFER];
	FILE *fp;
	int	retval;

	sprintf(buffer,LFSDIALOGAPP " m 0 \"Path To Icon ...\"");
	fp=popen(buffer,"r");
	fgets(buffer,MAXBUFFER,fp);
	retval=pclose(fp);
	if(retval==0)
		{
	//		printf("%s\n",buffer);
			deskIconsArray[foundDiskNumber].iconhint=666;
			deskIconsArray[foundDiskNumber].icon=strdup(buffer);
			fileCustomIcon=deskIconsArray[foundDiskNumber].icon;
			if(isDisk==true)
				saveInfofile(DISKFOLDER,deskIconsArray[foundDiskNumber].label,NULL,NULL,deskIconsArray[foundDiskNumber].uuid,"customicon",deskIconsArray[foundDiskNumber].x,deskIconsArray[foundDiskNumber].y);

			fileCustomIcon=NULL;
			needsRefresh=true;
		}
}

void pushedButton(Widget w,XtPointer data,XtPointer  garbage)
{
	long	what=(long)data;

	if(what<BUTTONOPEN)
		mountDisk(what);
	if(what==BUTTONCUSTUMICON)
		{
			doCustomIcon();
		}
	needsRefresh=true;
	loop=false;
}

String	fallback_resources[]=
{
	(char*)"*boxmount.*.width:		85",
	(char*)"*boxmount*.background:	grey",
	(char*)"*boxmount*.foreground:	Black",
	NULL,
};

Widget mountMenu(XtAppContext *app,int x,int y,bool isdisk)
{
	Widget	toplevel,mount,unmount,eject,bmount,custom;
	int		dump=0;

	toplevel=XtVaAppInitialize(app,"appmenu",NULL,0,&dump,NULL,fallback_resources,NULL);

	bmount=XtVaCreateManagedWidget("boxmount",boxWidgetClass,toplevel,NULL);
	if(isdisk==true)
		{
			mount=XtVaCreateManagedWidget("Mount",commandWidgetClass,bmount,NULL);
			unmount=XtVaCreateManagedWidget("Unmount",commandWidgetClass,bmount,NULL);
			eject=XtVaCreateManagedWidget("Eject",commandWidgetClass,bmount,NULL);
			custom=XtVaCreateManagedWidget("Custom Icon",commandWidgetClass,bmount,NULL);

			XtAddCallback(mount,XtNcallback,pushedButton,(XtPointer)(long)BUTTONMOUNT);
			XtAddCallback(unmount,XtNcallback,pushedButton,(XtPointer)(long)BUTTONUNMOUNT);
			XtAddCallback(eject,XtNcallback,pushedButton,(XtPointer)(long)BUTTONEJECT);
			XtAddCallback(custom,XtNcallback,pushedButton,(XtPointer)(long)BUTTONCUSTUMICON);
		}
	else
		{
			mount=XtVaCreateManagedWidget("Open",commandWidgetClass,bmount,NULL);
			custom=XtVaCreateManagedWidget("Custom Icon",commandWidgetClass,bmount,NULL);
			XtAddCallback(mount,XtNcallback,pushedButton,(XtPointer)(long)BUTTONOPEN);
			XtAddCallback(custom,XtNcallback,pushedButton,(XtPointer)(long)BUTTONCUSTUMICON);
	}

	XtVaSetValues(toplevel,XmNmwmDecorations,0,NULL);
	XtVaSetValues(toplevel,XmNoverrideRedirect,TRUE,NULL);
	XtVaSetValues(toplevel,XmNx,x,XmNy,y,NULL);
	XtRealizeWidget(toplevel);
	return(toplevel);
}

void doPopUp(int x,int y)
{
	Widget			mountmenu;
	XEvent			event;
	XtAppContext	app;
	bool			foundicon;
	int				win=0,wout=0;
	bool			mdown=false;

	foundicon=findIcon(x,y);
	if(foundicon==false)
		return;

	//if(isDisk==false)
	//	return;
	loop=true;

	mountmenu=mountMenu(&app,x,y,isDisk);

	//mainloop=true;
	while(loop==true)
		{
			XtAppNextEvent(app,&event);
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
				loop=false;
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
	char			buffer[MAXBUFFER];
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
	bool			buttonDown=false;
	int				oldboxx=-1,oldboxy=-1;
	bool			dragging=false;
	FILE			*fp;
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;

	asprintf(&path,"%s/.config/LFS/pidfile",getenv("HOME"));
	fw=fopen(path,"r");
	if(fw!=NULL)
		{
			fgets(buffer,MAXBUFFER,fw);
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

	asprintf(&command,"findmnt -nf /|awk '{print $2}'");
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,MAXBUFFER,fp);
			sscanf(buffer,"%as",&rootDev);
			pclose(fp);
		}

	deskIconsArray=(deskIcons*)calloc(deskIconsMaxCnt,sizeof(deskIcons));

//home icon
	deskIconsCnt=HOMEDATA;
	asprintf(&command,"%s/Home",cachePath);
	if(fileExists(command)==0)
		readDesktopFile("Home");
	else
		{
			deskIconsArray[HOMEDATA].x=0;
			deskIconsArray[HOMEDATA].y=0;
			deskIconsArray[HOMEDATA].installed=true;
			deskIconsArray[HOMEDATA].label=strdup("Home");
			deskIconsArray[HOMEDATA].mime=strdup("inode-directory");
			deskIconsArray[HOMEDATA].mountpoint=strdup(getenv("HOME"));
			saveInfofile(CACHEFOLDER,deskIconsArray[HOMEDATA].label,deskIconsArray[HOMEDATA].mime,deskIconsArray[HOMEDATA].mountpoint,NULL,NULL,deskIconsArray[HOMEDATA].x,deskIconsArray[HOMEDATA].y);
		}
	deskIconsArray[HOMEDATA].file=true;
	deskIconsArray[HOMEDATA].iconhint=COMPUTER;
	xySlot[deskIconsArray[HOMEDATA].x][deskIconsArray[HOMEDATA].y]=1;
	free(command);

//computer icon
	deskIconsCnt=ROOTDATA;
	asprintf(&command,"%s/Computer",cachePath);
	if(fileExists(command)==0)
		readDesktopFile("Computer");
	else
		{
			deskIconsArray[ROOTDATA].x=0;
			deskIconsArray[ROOTDATA].y=1;
			deskIconsArray[ROOTDATA].installed=true;
			deskIconsArray[ROOTDATA].label=strdup("Computer");
			deskIconsArray[ROOTDATA].mime=strdup("computer");
			deskIconsArray[ROOTDATA].mountpoint=strdup("/");
			saveInfofile(CACHEFOLDER,deskIconsArray[ROOTDATA].label,deskIconsArray[ROOTDATA].mime,deskIconsArray[ROOTDATA].mountpoint,NULL,NULL,deskIconsArray[ROOTDATA].x,deskIconsArray[ROOTDATA].y);
		}
	deskIconsArray[ROOTDATA].file=true;
	deskIconsArray[ROOTDATA].iconhint=COMPUTER;
	xySlot[deskIconsArray[ROOTDATA].x][deskIconsArray[ROOTDATA].y]=1;
	free(command);

	deskIconsCnt=RESERVED;

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
							numRead=read(fd,buffer,MAXBUFFER);
							if(numRead>0)
								fillDesk();
						}
					
					ret=poll(&polldisks,POLLIN,20);
					if(ret!=0)
						{
							numRead=read(fhfordisks,buffer,MAXBUFFER);
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
			if(XQueryPointer(display,DefaultRootWindow(display),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
				{
					if(mask_return & Button3Mask)
						{
							if(child_return==0)
								system(MAINMENURAPP " m");
						}
				}

			switch(ev.type)
				{
				case ButtonPress:
				//debugstr("button down");
					if(ev.xbutton.button==Button3)
						{
							doPopUp(ev.xbutton.x,ev.xbutton.y);
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
									if(isDisk==true)
										mountDisk(BUTTONMOUNT);
									else
										mountDisk(BUTTONOPEN);
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
						//debugstr("motion");
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

