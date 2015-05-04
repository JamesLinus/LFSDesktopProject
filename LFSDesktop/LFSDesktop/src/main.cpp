
//Mon 31 Jul 2006 12:30:55 BST
//
//LFSDesktop
//

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <X11/extensions/Xinerama.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xdbe.h>

#include <Imlib2.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"

#include "prefs.h"
#include "files.h"

#define UNKNOWNARG -100
#define REFRESHRATE 2
#define GRIDSIZE 80
#define MAXGRIDY 4
#define GRIDBORDER 32

Display			*display;
Window			rootWin;
int				displayWidth;
int				displayHeight;
GC				gc;
Region			rg;
XdbeBackBuffer	buffer;
XdbeSwapInfo	swapInfo;
Drawable		drawOnThis;
Colormap cm;

Visual			*visual=NULL;
int				depth=0;
int				screen;
int				blackColor;
int				whiteColor;

bool			done=true;
Pixmap			diskPixmap;
Pixmap			diskPixmapMask;
Pixmap			diskPixmapOffline;
Pixmap			diskPixmapMaskOffline;
Imlib_Image		diskImage;
Imlib_Image		diskImageOffline;
//const char		*diskImagePath="/usr/share/icons/gnome/48x48/devices/drive-harddisk.png";
const char		*diskImagePath="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/drive-removable-media.png";
const char		*diskImagePathOffline="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/drive-removable-media-offline.png";
char			*diskInfoPath;

unsigned long	labelBackground;
unsigned long	labelForeground;
GC				labelGC;
XFontStruct		*labelFont;

int				xCnt;
int				yCnt;
int				**xySlot;

Time			time=0;
bool			firstClick=false;
bool			foundIcon=false;

bool			needsRefresh=true;

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};


struct option long_options[] =
{
	{"long1",1,0,'l'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0,0,0,0}
};

void printhelp(void)
{
	printf("Usage: lfsdesktop [OPTION]\n"
	       "A CLI application\n"
	       " -l,--long1	Do somthing good\n"
	       " -v,--version	output version information and exit\n"
	       " -h,-?,--help	print this help\n\n"
	       "Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

void createColours(void)
{
	XColor	colour;
	int		cc=(int)GXcopyInverted;

	XParseColor(display,DefaultColormap(display,screen),"grey40",&colour);
	XAllocColor(display,DefaultColormap(display,screen),&colour);
	labelBackground=colour.pixel;
	XParseColor(display,DefaultColormap(display,screen),"white",&colour);
	XAllocColor(display,DefaultColormap(display,screen),&colour);
	labelForeground=colour.pixel;

	labelGC=XCreateGC(display,drawOnThis,GCFunction,(XGCValues*) &cc);
	labelFont=XQueryFont(display,XGContextFromGC(gc));
	if(!labelFont)
		fprintf(stderr,"XLoadQueryFont: failed loading default font");
}

int get_argb_visual(Visual** vis,int *depth)
{
	/* code from gtk project,gdk_screen_get_rgba_visual */
	XVisualInfo visual_template;
	XVisualInfo *visual_list=NULL;
	int nxvisuals=0,i;
	visual_template.screen=screen;
	visual_list=XGetVisualInfo (display,0,&visual_template,&nxvisuals);

	for (i=0; i < nxvisuals; i++)
		{
			if (visual_list[i].depth==32 &&
			        (visual_list[i].red_mask==0xff0000 &&
			         visual_list[i].green_mask==0x00ff00 &&
			         visual_list[i].blue_mask==0x0000ff ))
				{
					*vis=visual_list[i].visual;
					*depth=visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	printf("no rgb\n");
	XFree(visual_list);
	return 1;
}

void createDesktopWindow(void)
{
	int	rc=0;
	Atom	xa;
	Atom	xa_prop[10];
	Hints	hints;

	rc=get_argb_visual(&visual,&depth);
	if(rc==0)
		{
			XSetWindowAttributes attr;
			attr.colormap=XCreateColormap(display,DefaultRootWindow(display),visual,AllocNone);
			attr.border_pixel=0;
			attr.background_pixel=0;

			rootWin=XCreateWindow(display,DefaultRootWindow(display),0,0,displayWidth,displayHeight,0,depth,InputOutput,visual,CWEventMask |CWColormap | CWBorderPixel | CWBackPixel ,&attr);
			//XSelectInput(display,rootWin,StructureNotifyMask);
			//XSelectInput(display,rootWin,StructureNotifyMask| ButtonPress| ButtonReleaseMask|PointerMotionMask);

			xa=XInternAtom(display,"_NET_WM_STATE",False);
			xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
			xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
			xa_prop[2]=XInternAtom(display,"_NET_WM_STATE_SKIP_PAGER",False);
			xa_prop[3]=XInternAtom(display,"_NET_WM_STATE_SKIP_TASKBAR",False);
			xa_prop[4]=XInternAtom(display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);
			xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);

			xa=XInternAtom(display,"_NET_WM_STATE",False);
			if(xa!=None)
				XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,5);

			hints.flags=2;
			hints.decorations=0;
			XChangeProperty(display,rootWin,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);

			rg=XCreateRegion();
			XMapWindow(display,rootWin);
			XSync(display,False);

			XMoveWindow(display,rootWin,0,0);

			buffer=XdbeAllocateBackBufferName(display,rootWin,XdbeBackground);
			swapInfo.swap_window=rootWin;
			swapInfo.swap_action=XdbeBackground;
			XdbeSwapBuffers(display,&swapInfo,1);
			drawOnThis=buffer;
		}
}

void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y)
{
	char	*filepath;

	diskName=label;
	diskUUID=uuid;
	diskXPos=x;
	diskYPos=y;
	if(diskfilepath==NULL)
		{
			asprintf(&filepath,"%s/%s",diskInfoPath,uuid);
			saveVarsToFile(filepath,diskData);
			free(filepath);
		}
	else
		saveVarsToFile(diskfilepath,diskData);
}

void createDiskInfo(void)
{
	FILE	*fp;
	FILE	*fd;

	char	*command;
	char	line[1024];
	char	*devname;
	char	*label;
	char	*uuid;
	char	*diskfilepath;
	int		posx;
	int		posy;

	posx=0;
	posy=0;

	asprintf(&command,"lsblk -n --output=NAME,UUID,LABEL -lpds");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					devname=NULL;
					label=NULL;
					uuid=NULL;
					sscanf(line,"%as %as %a[^\n]s",&devname,&uuid,&label);
					if((devname!=NULL) && (strcmp(devname,"/dev/sr0")==0))
						{
							if(label==NULL)
								asprintf(&label,"CDROM");
						}

					if(label==NULL)
						asprintf(&label,"IGNOREDISK");

					asprintf(&diskfilepath,"%s/%s",diskInfoPath,uuid);

					if((uuid!=NULL) && (strlen(uuid)>1))
						{
							fd=fopen(diskfilepath,"r");
							if(fd==NULL)
								{
									fd=fopen(diskfilepath,"w");
									if(fd==NULL)
										{
											printf("Can't open disk folder ...\n");
											return;
										}

									while(xySlot[posx][posy]!=0)
										{
											posy++;
											if(posy>MAXGRIDY)
												{
													posy=0;
													posx++;
												}
										}
									makeDiskInfofile(diskfilepath,label,uuid,posx,posy);
									xySlot[posx][posy]=1;
									fclose(fd);
								}
							free(diskfilepath);
						}
					line[0]=0;
				}
			pclose(fp);
		}
	free(command);
}

void getDiskList(args *diskdata)
{
	FILE		*fp;
	FILE		*fd;
	char		*diskfilepath;
	char		*command;
	char		line[2048];
	int			diskx;
	int			disky;
	char		dataline[256];
	char		uuid[256];
	char		label[256];
	XColor		colour;
	int			fontheight;
	int			stringwidth;

	int			boxx,boxy,boxw,boxh;
	XRectangle	rect;

	XDestroyRegion(rg);
	rg=XCreateRegion();

	asprintf(&command,"lsblk -n --output=UUID -lpds");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					if(strlen(line)>0)
						{
							asprintf(&diskfilepath,"%s/%s",diskInfoPath,line);
							if(fileExists(diskfilepath)!=0)
								createDiskInfo();

							loadVarsFromFile(diskfilepath,diskdata);
							xySlot[diskXPos][diskYPos]=1;
							diskx=diskXPos*GRIDSIZE+GRIDBORDER;
							disky=diskYPos*GRIDSIZE+GRIDBORDER;

							if(strcmp(diskName,"IGNOREDISK")!=0)
								{
									XSetClipMask(display,gc,diskPixmapMask);
									XSetClipOrigin(display,gc,diskx,disky);

									FILE	*tp;
									char	*com;

									asprintf(&com,"findmnt -fn $(findfs UUID=%s)",diskUUID);
									line[0]=0;
									tp=popen(com,"r");
									free(com);
									fgets(line,2048,tp);
									pclose(tp);
									if(strlen(line)>0)
										XCopyArea(display,diskPixmap,drawOnThis,gc,0,0,48,48,diskx,disky);
									else
										XCopyArea(display,diskPixmapOffline,drawOnThis,gc,0,0,48,48,diskx,disky);

									rect.x=diskx;
									rect.y=disky;
									rect.width=48;
									rect.height=48;
									XUnionRectWithRegion(&rect,rg, rg);

									XSetClipMask(display,gc,0);

									fontheight=labelFont->ascent+labelFont->descent;
									stringwidth=XTextWidth(labelFont,label,strlen(diskName));

									boxx=diskx+(48/2)-(stringwidth/2)-1;
									boxy=disky+48+1;
									boxw=stringwidth+2;
									boxh=fontheight-2;

									XSetForeground(display,gc,labelBackground);
									XSetFillStyle(display,gc,FillSolid);
									XFillRectangle(display,drawOnThis,gc,boxx,disky+48,boxw,boxh);

									XSetForeground(display,labelGC,labelForeground);
									XSetBackground(display,labelGC,labelBackground);

									XDrawString(display,drawOnThis,labelGC,boxx+1,disky+48+boxh-1,diskName,strlen(diskName));
								}
						}
				}
			fclose(fp);
		}
	free(command);
	XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);
}

bool findIcon(int x, int y)
{
	FILE	*fp;
	FILE	*fd;
	char	*command;
	char	line[2048];
	int		dx,dy;
	char	label[256];
	char	uuid[256];
	char	dataline[256];

	asprintf(&command,"find %s -mindepth 1",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					loadVarsFromFile(line,diskData);
					if(strlen(line)>0)
						{
							if(strlen(diskUUID)>1)
								{
									if((x>=(diskXPos*GRIDSIZE+GRIDBORDER))&&(x<=(diskXPos*GRIDSIZE+GRIDBORDER)+48)&&(y>=(diskYPos*GRIDSIZE+GRIDBORDER))&&(y<=(diskYPos*GRIDSIZE+GRIDBORDER)+48))
										{
											pclose(fp);
											return(true);
										}
								}
						}
				}
			pclose(fp);
		}
	return(false);
}

void mountDisk(int x, int y)
{
	FILE	*fp;
	FILE	*fd;
	char	*command;
	char	line[2048];
	int		dx,dy;
	char	label[256];
	char	uuid[256];
	char	dataline[256];

	asprintf(&command,"find %s -mindepth 1",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					loadVarsFromFile(line,diskData);
					if(strlen(line)>0)
						{
							if(strlen(diskUUID)>1)
								{
									if((x>=(diskXPos*GRIDSIZE+GRIDBORDER))&&(x<=(diskXPos*GRIDSIZE+GRIDBORDER)+48)&&(y>=(diskYPos*GRIDSIZE+GRIDBORDER))&&(y<=(diskYPos*GRIDSIZE+GRIDBORDER)+48))
										{
											asprintf(&command,"udevil mount `findfs UUID=%s`",diskUUID);
											system(command);
											pclose(fp);
											return;
										}
								}
						}
				}
			pclose(fp);
		}
}

void  alarmCallBack(int sig)
{
	signal(SIGALRM,SIG_IGN);
	needsRefresh=true;
	signal(SIGALRM,alarmCallBack);
	alarm(REFRESHRATE);
}

int main(int argc,char **argv)
{
	int				c;
	XEvent			ev;
	char			*command;
	unsigned long	timer=0;

	signal(SIGALRM,alarmCallBack);

	done=true;
	asprintf(&diskInfoPath,"%s/.config/LFS/disks",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",diskInfoPath);
	system(command);
	free(command);

	while (1)
		{
			int option_index=0;
			c=getopt_long (argc,argv,"v?h:l:",long_options,&option_index);
			if (c==-1)
				break;

			switch (c)
				{
				case 'l':
					printf("Arg=%s\n",optarg);
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

	createDesktopWindow();

	gc=XCreateGC(display,drawOnThis,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask);

	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);

	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);
	imlib_context_set_drawable(drawOnThis);

	diskImage=imlib_load_image(diskImagePath);
	imlib_context_set_image(diskImage);
	imlib_image_set_has_alpha(1);
	imlib_render_pixmaps_for_whole_image(&diskPixmap,&diskPixmapMask);
	imlib_free_image();

	diskImageOffline=imlib_load_image(diskImagePathOffline);
	imlib_context_set_image(diskImageOffline);
	imlib_image_set_has_alpha(1);
	imlib_render_pixmaps_for_whole_image(&diskPixmapOffline,&diskPixmapMaskOffline);
	imlib_free_image();

	createColours();

	xCnt=displayWidth/GRIDSIZE;
	yCnt=displayHeight/GRIDSIZE;

	xySlot=(int**)malloc(xCnt*sizeof(int*));
	for(int j=0; j<xCnt; j++)
		xySlot[j]=(int*)malloc(yCnt*sizeof(int));

	for(int yy=0; yy<yCnt; yy++)
		for(int xx=0; xx<xCnt; xx++)
			xySlot[xx][yy]=0;

	alarm(REFRESHRATE);

	getDiskList(diskData);
	createDiskInfo();
	XdbeSwapBuffers(display,&swapInfo,1);

	char	*fdiskname=NULL;
	char	*fdiskuuid=NULL;
	int		oldx=-1,oldy=-1;
	bool	buttonDown=false;
	int	oldboxx=-1,oldboxy=-1;

	while(done)
		{
			//if((buttonDown==true) && (foundIcon==true))
			//{
			//getDiskList();
			//		XdbeSwapBuffers(display,&swapInfo,1);
			//}
			//	needsRefresh=true;
			if(needsRefresh==true)
				{
					getDiskList(diskData);
					XdbeSwapBuffers(display,&swapInfo,1);
					needsRefresh=false;
				}

			XCheckWindowEvent(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask,&ev);

			switch(ev.type)
				{
				case MapNotify:
					break;
				case Expose:
					break;
				case ConfigureNotify:
					break;
				case VisibilityNotify:
					break;
				case ButtonPress:
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
									mountDisk(ev.xbutton.x,ev.xbutton.y);
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
							foundIcon=findIcon(ev.xbutton.x,ev.xbutton.y);
						}

					if(foundIcon==true)
						{
							if(fdiskname!=NULL)
								free(fdiskname);
							fdiskname=strdup(diskName);
							if(fdiskuuid!=NULL)
								free(fdiskuuid);
							fdiskuuid=strdup(diskUUID);
							oldx=diskXPos;
							oldy=diskYPos;
							oldboxx=ev.xbutton.x;
							oldboxy=ev.xbutton.y;
						}
					else
						{
							if(fdiskname!=NULL)
								free(fdiskname);
							fdiskname=NULL;
							if(fdiskuuid!=NULL)
								free(fdiskuuid);
							fdiskuuid=NULL;
							diskName=NULL;
							diskUUID=NULL;
							oldx=-1;
							oldy=-1;
						}

					break;
				case ButtonRelease:
					buttonDown=false;
					if(foundIcon==true)
						{
							int newx,newy;
							newx=(ev.xbutton.x-GRIDBORDER)/GRIDSIZE;
							newy=(ev.xbutton.y-GRIDBORDER)/GRIDSIZE;
							//printf("t1=%i x=%i name=%s slot x=%i sloty=%i xyslot=%i\n",ev.xbutton.time,ev.xbutton.x,fdiskname,newx,newy,xySlot[newx][newy]);
							foundIcon=false;
							if(xySlot[newx][newy]==0)
								{
									if((oldx!=-1) && (oldy!=-1))
										xySlot[diskXPos][diskYPos]=0;
									makeDiskInfofile(NULL,fdiskname,fdiskuuid,newx,newy);
									needsRefresh=true;
								}
							if(fdiskname!=NULL)
								free(fdiskname);
							fdiskname=NULL;
							if(fdiskuuid!=NULL)
								free(fdiskuuid);
							fdiskuuid=NULL;
							diskName=NULL;
							diskUUID=NULL;
							oldx=-1;
							oldy=-1;
						}

					break;
				case MotionNotify:
					if(foundIcon==true && buttonDown==true)
						{
							if((ev.xmotion.x>oldboxx+48) || (ev.xmotion.x<oldboxx-48) || (ev.xmotion.y>oldboxy+48) || (ev.xmotion.y<oldboxy-48))
							{
							int newboxx,newboxy;
							oldboxx=ev.xmotion.x;
							oldboxy=ev.xmotion.y;
							char	*dn=NULL;
							char	*du=NULL;
							int		dx=0;
							int dy	=0;
							args			diskdata[]=
{
	{"diskname",TYPESTRING,&dn},
	{"diskuuid",TYPESTRING,&du},
	{"diskx",TYPEINT,&dx},
	{"disky",TYPEINT,&dy},

	{NULL,0,NULL}
};


							//oldboxx=((ev.xmotion.x-GRIDBORDER)/GRIDSIZE)*GRIDSIZE;
							XSetForeground(display,gc,labelBackground);
							XSetFillStyle(display,gc,FillSolid);
							newboxx=((ev.xmotion.x-GRIDBORDER)/GRIDSIZE)*GRIDSIZE+(GRIDBORDER/2);
							newboxy=((ev.xmotion.y-GRIDBORDER)/GRIDSIZE)*GRIDSIZE+(GRIDBORDER/2);
							XDrawRectangle(display,drawOnThis,gc,newboxx,newboxy,GRIDSIZE,GRIDSIZE);
							getDiskList(diskData);
							XdbeSwapBuffers(display,&swapInfo,1);
}
						//printf("xxxx\n");
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

