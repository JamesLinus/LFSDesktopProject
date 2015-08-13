
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

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
//#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"

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
	{"tidy",0,0,'t'},
	{"theme",1,0,'a'},
	{"term-command",1,0,'x'},
	{"show-extension",0,0,'s'},
	{"font",1,0,'f'},
	{"fore-colour",1,0,'4'},
	{"back-colour",1,0,'b'},
	{"ignore",1,0,'i'},
	{"appmenu",0,0,'m'},
	{"debug",0,0,'d'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0,0,0,0}
};

bool	needsRefresh=true;
bool	loop=true;
bool	retVal=false;
int		width=300;
int		hite=60;

void printhelp(void)
{
	printf("LFSDesktop - " VERSION "\n"
			"Usage: lfsdesktop [OPTION] ...\n" 
			" -c,--clean			Clean disk info/desktop cache data\n"
			" -t,--tidy				Tidy disk info/desktop cache data preserving existing\n"
			" -a,--theme			Set theme\n"
			" -x,--term-command		Set terminal command ( default xterm -e )\n"
			" -s,--show-extension	Set terminal command ( default xterm -e )\n"
			" -f,--font				Font face ( Sans;0;0;10 - Fontname;Weight(0/1);Slant(0/1/2);Fontsize )\n"
			" -4,--fore-colour		Fore colour for label in RGBA hex notation ( default 0xffffffff )\n"
			" -b,--back-colour		Back colour for label in RGBA hex notation ( default 0x0 )\n"
			" -i,--ignore			List of ';' seperated disk labels to NOT show on desktop\n"
			" -m,--appmenu			Show the application menu when right clicking on desktop\n"
			" -d,--debug			Debug\n"
			" -v,--version			output version information and exit\n"
			" -h,-?,--help			print this help\n\n"
			"Report bugs to kdhedger68713@gmail.com\n"
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

bool setIconCallback(void *p,void* ud)
{
	if((long)ud==1)
		retVal=false;
	else
		retVal=true;

	return(false);
}

void doCustomIcon(bool useicon)
{
	XEvent				event;
	LFSTK_buttonClass	*bc,*bc1;
	bool				firstrun=true;
	LFSTK_windowClass	*mainwind;
	LFSTK_lineEditClass	*le;
	bool				retfromlib=true;

	if(useicon==true)
		{
			mainwind=new LFSTK_windowClass(0,0,width,hite,"Enter Path To Icon",false);
			mainwind->LFSTK_loadGlobalColours(tkConfigPath);

			le=new LFSTK_lineEditClass(mainwind,"",0,0,width,24,NorthWestGravity);
			XMapWindow(mainwind->display,le->LFSTK_getWindow());

			bc=new LFSTK_buttonClass(mainwind,"Apply",4,24+4+4,75,24,SouthWestGravity);
			bc->LFSTK_setCallBack(NULL,setIconCallback,(void*)1);
			XMapWindow(mainwind->display,bc->LFSTK_getWindow());

			bc1=new LFSTK_buttonClass(mainwind,"Cancel",width-4-75,24+4+4,75,24,SouthEastGravity);
			bc1->LFSTK_setCallBack(NULL,setIconCallback,(void*)2);
			XMapWindow(mainwind->display,bc1->LFSTK_getWindow());

			XMapWindow(mainwind->display,mainwind->window);
			mainwind->LFSTK_setKeepAbove(true);

			while(retfromlib==true)
				{
					listener *l=mainwind->LFSTK_getListener(event.xany.window);
					if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
						retfromlib=l->function(l->pointer,&event,l->type);

					XNextEvent(mainwind->display,&event);
					switch(event.type)
						{
							case LeaveNotify:
								break;
							case Expose:
								mainwind->LFSTK_clearWindow();
								if(firstrun==true)
									{
										firstrun=false;
										le->LFSTK_setFocus();
									}
								break;

							case ConfigureNotify:
								mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
								mainwind->LFSTK_clearWindow();
								le->LFSTK_resizeWindow(event.xconfigurerequest.width,24);
								le->LFSTK_clearWindow();
								break;

							case ClientMessage:
								if (event.xclient.message_type == XInternAtom(mainwind->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(mainwind->display, "WM_DELETE_WINDOW", 1))
									retfromlib=false;
						}
				}

			if(retVal==0)
				{
					deskIconsArray[foundDiskNumber].icon=strdup(le->LFSTK_getBuffer()->c_str());
					fileCustomIcon=deskIconsArray[foundDiskNumber].icon;
					fileGotCustomIcon=true;
					deskIconsArray[foundDiskNumber].customicon=true;
				}

			delete bc;
			delete bc1;
			delete le;
			delete mainwind;
		}
	else
		{
			if(deskIconsArray[foundDiskNumber].icon!=NULL)
				free(deskIconsArray[foundDiskNumber].icon);
			deskIconsArray[foundDiskNumber].icon=NULL;
			deskIconsArray[foundDiskNumber].customicon=false;
			fileCustomIcon=NULL;
			fileGotCustomIcon=false;
		}


	if(isDisk==true)
		saveInfofile(DISKFOLDER,deskIconsArray[foundDiskNumber].label,NULL,NULL,deskIconsArray[foundDiskNumber].uuid,(char*)iconDiskType[deskIconsArray[foundDiskNumber].iconhint],deskIconsArray[foundDiskNumber].x,deskIconsArray[foundDiskNumber].y,foundDiskNumber);
	else
		saveInfofile(CACHEFOLDER,deskIconsArray[foundDiskNumber].label,deskIconsArray[foundDiskNumber].mime,deskIconsArray[foundDiskNumber].mountpoint,NULL,NULL,deskIconsArray[foundDiskNumber].x,deskIconsArray[foundDiskNumber].y,foundDiskNumber);

	needsRefresh=true;
}

void pushedButton(Widget w,XtPointer data,XtPointer  garbage)
{
	long	what=(long)data;

	switch(what)
		{
			case BUTTONOPEN:
			case BUTTONMOUNT:
			case BUTTONUNMOUNT:
			case BUTTONEJECT:
				mountDisk(what);
				break;
			case BUTTONADDICON:
				doCustomIcon(true);
				break;
			case BUTTONREMOVEICON:
				doCustomIcon(false);
				break;
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

#if 0
Widget mountMenuX(XtAppContext *app,int x,int y,bool isdisk)
{
	Widget	toplevel,mount,unmount,eject,bmount,custom,remove;
	int		dump=0;

	toplevel=XtVaAppInitialize(app,"appmenu",NULL,0,&dump,NULL,fallback_resources,NULL);

	bmount=XtVaCreateManagedWidget("boxmount",boxWidgetClass,toplevel,NULL);
	if(isdisk==true)
		{
			mount=XtVaCreateManagedWidget("Mount",commandWidgetClass,bmount,NULL);
			unmount=XtVaCreateManagedWidget("Unmount",commandWidgetClass,bmount,NULL);
			eject=XtVaCreateManagedWidget("Eject",commandWidgetClass,bmount,NULL);

			XtAddCallback(mount,XtNcallback,pushedButton,(XtPointer)(long)BUTTONMOUNT);
			XtAddCallback(unmount,XtNcallback,pushedButton,(XtPointer)(long)BUTTONUNMOUNT);
			XtAddCallback(eject,XtNcallback,pushedButton,(XtPointer)(long)BUTTONEJECT);
		}
	else
		{
			mount=XtVaCreateManagedWidget("Open",commandWidgetClass,bmount,NULL);
			XtAddCallback(mount,XtNcallback,pushedButton,(XtPointer)(long)BUTTONOPEN);
		}

	custom=XtVaCreateManagedWidget("Custom Icon",commandWidgetClass,bmount,NULL);
	XtAddCallback(custom,XtNcallback,pushedButton,(XtPointer)(long)BUTTONADDICON);
	remove=XtVaCreateManagedWidget("Remove Icon",commandWidgetClass,bmount,NULL);
	XtAddCallback(remove,XtNcallback,pushedButton,(XtPointer)(long)BUTTONREMOVEICON);

	XtVaSetValues(toplevel,XmNmwmDecorations,0,NULL);
	XtVaSetValues(toplevel,XmNoverrideRedirect,TRUE,NULL);
	XtVaSetValues(toplevel,XmNx,x,XmNy,y,NULL);
	XtRealizeWidget(toplevel);
	return(toplevel);
}
#endif

LFSTK_windowClass	*wc;

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
			if((root_x_return>g->x) && (root_x_return<(g->x+(int)(g->w))) && (root_y_return>g->y) && (root_y_return<(g->y+(int)(g->h))))
				return(true);
		}
	return(false);
}
//bool callback(void *p,void* ud)
bool pushedButtonCB(void *p,void* ud)
{
	long	what=(long)ud;

	switch(what)
		{
			case BUTTONOPEN:
			case BUTTONMOUNT:
			case BUTTONUNMOUNT:
			case BUTTONEJECT:
				mountDisk(what);
				break;
			case BUTTONADDICON:
				doCustomIcon(true);
				break;
			case BUTTONREMOVEICON:
				doCustomIcon(false);
				break;
		}

	needsRefresh=true;
	loop=false;
	return(false);
}

const char	*mountMenuData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};

void doPopUp(int x,int y)
{
	LFSTK_buttonClass	*bc[6]={NULL,};
	int					sx=x,sy=y;
	XEvent				event;
	char				*fontandsize;

	if(findIcon(x,y)==false)
		return;

	wc=new LFSTK_windowClass(sx,sy,64,400,"appmenu",true);
	wc->LFSTK_loadGlobalColours(tkConfigPath);
	asprintf(&fontandsize,"%s:size=%i",fontName,fontSize);

	wc->LFSTK_setFontString(fontandsize);
	free(fontandsize);
	sx=0;
	sy=0;

	int addto=wc->font->ascent+wc->font->descent+8;
	int maxwid=0;
	
	while(mountMenuData[sx]!=NULL)
		{
			XftFont *font=(XftFont*)wc->font->data;
			XGlyphInfo info;
			XftTextExtentsUtf8(wc->display,font,(XftChar8 *)mountMenuData[sx],strlen(mountMenuData[sx]),&info);
			sx++;
			if((info.width-info.x)>maxwid)
				maxwid=info.width;
		}

	maxwid+=8;
	sx=0;
	sy=0;
	if(isDisk==true)
		{

			bc[0]=new LFSTK_buttonClass(wc,mountMenuData[0],sx,sy,maxwid,24,NorthWestGravity);
			bc[0]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)BUTTONMOUNT);
			XMapWindow(wc->display,bc[0]->LFSTK_getWindow());
			sy+=addto;
			bc[1]=new LFSTK_buttonClass(wc,mountMenuData[1],sx,sy,maxwid,24,NorthWestGravity);
			bc[1]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)BUTTONUNMOUNT);
			XMapWindow(wc->display,bc[1]->LFSTK_getWindow());
			sy+=addto;
			bc[2]=new LFSTK_buttonClass(wc,mountMenuData[2],sx,sy,maxwid,24,NorthWestGravity);
			bc[2]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)BUTTONEJECT);
			XMapWindow(wc->display,bc[2]->LFSTK_getWindow());
			sy+=addto;

		}
	else
		{
			bc[3]=new LFSTK_buttonClass(wc,mountMenuData[3],sx,sy,maxwid,24,NorthWestGravity);
			bc[3]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)BUTTONOPEN);
			XMapWindow(wc->display,bc[3]->LFSTK_getWindow());
			sy+=addto;
		}
	bc[4]=new LFSTK_buttonClass(wc,mountMenuData[4],sx,sy,maxwid,24,NorthWestGravity);
	bc[4]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)BUTTONADDICON);
	XMapWindow(wc->display,bc[4]->LFSTK_getWindow());
	sy+=addto;

	bc[5]=new LFSTK_buttonClass(wc,mountMenuData[5],sx,sy,maxwid,24,NorthWestGravity);
	bc[5]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)BUTTONREMOVEICON);
	XMapWindow(wc->display,bc[5]->LFSTK_getWindow());
	sy+=addto;

	XResizeWindow(wc->display,wc->window,maxwid,sy);
	wc->LFSTK_resizeWindow(maxwid,sy);
	wc->LFSTK_clearWindow();
	XMapWindow(wc->display,wc->window);

	loop=true;
	while(loop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);

			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						loop=inWindow();
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

	for(int j=0;j<6;j++)
		if(bc[j]!=NULL)
			delete bc[j];
	delete wc;
}

void doPopUpXX(int x,int y)
{
#if 0
	Widget			mountmenu;
	XEvent			event;
	XtAppContext	app;
	bool			foundicon;
	int				win=0,wout=0;
	bool			mdown=false;

	foundicon=findIcon(x,y);
	if(foundicon==false)
		return;

	loop=true;

	mountmenu=mountMenu(&app,x,y,isDisk);

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
#endif
}

void setFontEtc(void)
{
	char	*ptr;

	ptr=strtok(fontFace,";");
	if(ptr!=NULL)
		fontName=strdup(ptr);
	else
		fontName=strdup("Sans");
	ptr=strtok(NULL,";");
	if(ptr!=NULL)
		weight=(cairo_font_weight_t)atoi(ptr);
	else
		weight=CAIRO_FONT_WEIGHT_NORMAL;
	ptr=strtok(NULL,";");
	if(ptr!=NULL)
		slant=(cairo_font_slant_t)atoi(ptr);
	else
		slant=CAIRO_FONT_SLANT_NORMAL;
	ptr=strtok(NULL,";");
	if(ptr!=NULL)
		fontSize=atoi(ptr);
	else
		fontSize=10;

	backColour.r=((strtol(backCol,NULL,16)>>24) & 0xff)/256.0;
	backColour.g=((strtol(backCol,NULL,16)>>16) & 0xff)/256.0;
	backColour.b=((strtol(backCol,NULL,16)>>8) & 0xff)/256.0;
	backColour.a=((strtol(backCol,NULL,16)>>0) & 0xff)/256.0;

	foreColour.r=((strtol(foreCol,NULL,16)>>24) & 0xff)/256.0;
	foreColour.g=((strtol(foreCol,NULL,16)>>16) & 0xff)/256.0;
	foreColour.b=((strtol(foreCol,NULL,16)>>8) & 0xff)/256.0;
	foreColour.a=((strtol(foreCol,NULL,16)>>0) & 0xff)/256.0;
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
	bool			dotidy=false;
	bool			doShowAppmenu=false;

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
	asprintf(&fontFace,"Sans;0;0;10");
	asprintf(&foreCol,"0xffffffff");
	asprintf(&backCol,"0x00000000");
	showSuffix=false;
	asprintf(&terminalCommand,"xterm -e ");

	asprintf(&tkConfigPath,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));

	loadVarsFromFile(prefsPath,desktopPrefs);

	while (1)
		{
			int option_index=0;
			c=getopt_long (argc,argv,"v?hctdsma:x:f:4:b:i:",long_options,&option_index);
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
					dotidy=true;
					break;

				case 'a':
					if(iconTheme!=NULL)
						free(iconTheme);
					iconTheme=strdup(optarg);
					break;

				case 'x':
					if(terminalCommand!=NULL)
						free(terminalCommand);
					terminalCommand=strdup(optarg);
					break;

				case 's':
					showSuffix=true;
					break;

				case 'f':
					if(fontFace!=NULL)
						free(fontFace);
					fontFace=strdup(optarg);
					break;

				case '4':
					if(foreCol!=NULL)
						free(foreCol);
					foreCol=strdup(optarg);
					break;

				case 'b':
					if(backCol!=NULL)
						free(backCol);
					backCol=strdup(optarg);
					break;

				case 'i':
					if(ignores!=NULL)
						free(ignores);
					ignores=strdup(optarg);
					break;

				case 'm':
					doShowAppmenu=true;
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

	setFontEtc();

	XSynchronize(display,true);
	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
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
			saveInfofile(CACHEFOLDER,deskIconsArray[HOMEDATA].label,deskIconsArray[HOMEDATA].mime,deskIconsArray[HOMEDATA].mountpoint,NULL,NULL,deskIconsArray[HOMEDATA].x,deskIconsArray[HOMEDATA].y,HOMEDATA);
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
			saveInfofile(CACHEFOLDER,deskIconsArray[ROOTDATA].label,deskIconsArray[ROOTDATA].mime,deskIconsArray[ROOTDATA].mountpoint,NULL,NULL,deskIconsArray[ROOTDATA].x,deskIconsArray[ROOTDATA].y,ROOTDATA);
		}
	deskIconsArray[ROOTDATA].file=true;
	deskIconsArray[ROOTDATA].iconhint=COMPUTER;
	xySlot[deskIconsArray[ROOTDATA].x][deskIconsArray[ROOTDATA].y]=1;
	free(command);

	deskIconsCnt=RESERVED;

	fillDesk();

	if(dotidy==true)
		{
			command=oneLiner("rm \"%s\"/*;rm \"%s\"/*",cachePath,diskInfoPath);
			for (int j=0;j<deskIconsCnt;j++)
				{
					if(deskIconsArray[j].file==true)
						saveInfofile(CACHEFOLDER,deskIconsArray[j].label,deskIconsArray[j].mime,deskIconsArray[j].mountpoint,NULL,NULL,deskIconsArray[j].x,deskIconsArray[j].y,j);
					else
						saveInfofile(DISKFOLDER,deskIconsArray[j].label,NULL,NULL,deskIconsArray[j].uuid,(char*)iconDiskType[deskIconsArray[j].iconhint],deskIconsArray[j].x,deskIconsArray[j].y,j);
			
					fileDiskLabel=NULL;
					fileDiskUUID=NULL;
					fileDiskXPos=-1;
					fileDiskYPos=-1;
					fileDiskType=NULL;
				}
		}

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
			if(doShowAppmenu==true)
				{
					if(XQueryPointer(display,DefaultRootWindow(display),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
						{
							if(mask_return & Button3Mask)
								{
									if(child_return==0)
										{
											sprintf(buffer,"%s \"%s\"",MAINMENUAPP,terminalCommand);
											system(buffer);
										}
								}
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
										saveInfofile(DISKFOLDER,deskIconsArray[foundDiskNumber].label,NULL,NULL,deskIconsArray[foundDiskNumber].uuid,(char*)iconDiskType[deskIconsArray[foundDiskNumber].iconhint],newx,newy,foundDiskNumber);
									else
										saveInfofile(CACHEFOLDER,deskIconsArray[foundDiskNumber].label,deskIconsArray[foundDiskNumber].mime,deskIconsArray[foundDiskNumber].mountpoint,NULL,NULL,newx,newy,foundDiskNumber);
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

	free(diskInfoPath);
	free(cachePath);
	free(prefsPath);
	free(desktopPath);
	free(tkConfigPath);
	return 0;
}
