
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

#include "config.h"

/*
GC  ggcc;
int cc=(int)GXcopyInverted;
ggcc=XCreateGC(display,drawOnThis,GCFunction,(XGCValues*) &cc);
XChangeGC(display, gc, GCFunction,(XGCValues*) &cc);
	XColor tmp;

	XParseColor(display, DefaultColormap(display,screen), "white", &tmp);
	XAllocColor(display,DefaultColormap(display,screen),&tmp);

XSetForeground(display,gc,tmp.pixel);
XSetBackground(display,gc,blackColor);

					XDrawString(display,drawOnThis,gc,diskx,disky+48+20,label,strlen(label)-1);

*/

#define UNKNOWNARG -100

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

void getDiskList(void)
{
	FILE		*fp;
	FILE		*fd;
	char		*diskfilepath;
	char		*command;
	char		line[2048];
	int			diskx;
	int			disky;
	char		dataline[256];
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
							fd=fopen(diskfilepath,"r");
							if(fd!=NULL)
								{
									fgets(label,256,fd);//name
									fgets(dataline,256,fd);//uuid
									fgets(dataline,256,fd);//x
									diskx=atoi(dataline);
									fgets(dataline,256,fd);//y
									disky=atoi(dataline);
									fclose(fd);
									free(diskfilepath);
								}
							if(strcmp(label,"IGNOREDISK\n")!=0)
								{
									XSetClipMask(display,gc,diskPixmapMask);
									XSetClipOrigin(display,gc,diskx,disky);
									
									//XSetForeground(display,gc,labelBackground);
									//XSetFillStyle(display,gc,FillSolid);
									//XSetForeground(display,labelGC,labelForeground);
									//XSetBackground(display,labelGC,labelBackground);

									
									
									XCopyArea(display,diskPixmapOffline,drawOnThis,gc,0,0,48,48,diskx,disky);
									//XSetForeground(display,gc,labelBackground);
									//XFillRectangle(display,drawOnThis,gc,diskx,disky,48,48);


									rect.x=diskx;
									rect.y=disky;
									rect.width=48;
									rect.height=48;
									XUnionRectWithRegion(&rect,rg, rg);

									XSetClipMask(display,gc,0);

									fontheight=labelFont->ascent+labelFont->descent;
									stringwidth=XTextWidth(labelFont,label,strlen(label)-1);

									boxx=diskx+(48/2)-(stringwidth/2)-1;
									boxy=disky+48+1;
									boxw=stringwidth+2;
									boxh=fontheight-2;

									XSetForeground(display,gc,labelBackground);
									XSetFillStyle(display,gc,FillSolid);
									XFillRectangle(display,drawOnThis,gc,boxx,disky+48,boxw,boxh);

									XSetForeground(display,labelGC,labelForeground);
									XSetBackground(display,labelGC,labelBackground);

									XDrawString(display,drawOnThis,labelGC,boxx+1,disky+48+boxh-1,label,strlen(label)-1);
									disky=disky+50;
							}
						}
				}
			fclose(fp);
		}
	free(command);
	XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);
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
	int		posx=48;
	int		posy=48;

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
							asprintf(&uuid,"CDROM");
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
									fprintf(fd,"%s\n%s\n%i\n%i\n",label,uuid,posx,posy);
									posy=posy+64;
									if(posy>512-64)
										{
											posy=48;
											posx=posx+128;
										}
									fclose(fd);
								}
							free(diskfilepath);
							free(devname);
							free(uuid);
							free(label);
						}
					line[0]=0;
				}
			pclose(fp);
		}
	free(command);
}

Time time=0;
bool	firstclick=false;

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

	asprintf(&command,"find %s",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					if(strlen(line)>0)
						{
							//asprintf(&command,"%s",line);
							fd=fopen(line,"r");
							//printf("line=%s\n",line);
							//free(command);
							if(fd!=NULL)
								{
									label[0]=0;
									uuid[0]=0;
									dataline[0]=0;
									fgets(label,256,fd);//name
									label[strlen(label)-1]=0;
									fgets(uuid,256,fd);//uuid
									uuid[strlen(uuid)-1]=0;
									fgets(dataline,256,fd);//x
									dataline[strlen(dataline)-1]=0;
									dx=atoi(dataline);
									fgets(dataline,256,fd);//y
									dataline[strlen(dataline)-1]=0;
									dy=atoi(dataline);
									fclose(fd);
								//printf("uuid=%s label=%s dx=%i dy=%i\n",uuid,label,dx,dy);
								}
							if(strlen(uuid)>1)
								{
								if((x>=dx)&&(x<=dx+48)&&(y>=dy)&&(y<=dy+48))
								{
									//asprintf(&command,"sudo -A mkdir -p /media/%s 2>&1 >/dev/null;sudo -A mount -U %s /media/%s",label,uuid,label);
									asprintf(&command,"udevil mount `findfs UUID=%s`",uuid);
									system(command);
									printf("uuid=%s label=%s\n",uuid,label);
									pclose(fp);
									return;
								}
								}
						}
				}
			pclose(fp);
		}
}

int main(int argc,char **argv)
{
	int		c;
	XEvent	ev;
	char	*command;

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
	createDiskInfo();
int cnt=0;
unsigned long  timer=0;

			getDiskList();
			XdbeSwapBuffers(display,&swapInfo,1);
	while (done)
		{
			//getDiskList();
			//XdbeSwapBuffers(display,&swapInfo,1);
			//while (XPending(display))
				XNextEvent(display,&ev);

			switch(ev.type)
				{
//				case ClientMessage:
//					if (ev.xclient.message_type == XInternAtom(display,"WM_PROTOCOLS",1) && (Atom)ev.xclient.data.l[0] == XInternAtom(display,"WM_DELETE_WINDOW",1))
//						done=false;
//					continue;
//
//					break;

				case MapNotify:
                    break;
                case Expose:
                     printf("expose\n");
                   // If this is not the last expose event break
                    if (ev.xexpose.count != 0)
                        break;
                    else
                    {
                    printf("expose\n");
                    getDiskList();
			XdbeSwapBuffers(display,&swapInfo,1);
                        break;
                        }
                case ConfigureNotify:
                    break;
                case VisibilityNotify:
                    break;
				case ButtonPress:
					if(firstclick==false)
						{
							firstclick=true;
							time=ev.xbutton.time;
						}
					else
						{
							printf("t1=%i t2=%i tot=%i\n",time,ev.xbutton.time,ev.xbutton.time-time);
							firstclick=false;
							if(ev.xbutton.time-time<800)
								{
									printf("x=%i y=%i\n",ev.xbutton.x,ev.xbutton.y);
								printf("double click\n");
								mountDisk(ev.xbutton.x,ev.xbutton.y);
								}
							time=ev.xbutton.time;
						}
					//ev.type=-1;
					//printf("bdown\n");
					break;
				case ButtonRelease:
					//cnt++;
					//printf("bup %i\n",cnt);
					break;
				case MotionNotify:
					//printf("bmove\n");
					break;
				default:
			//printf("default\n");
			timer++;
			if(timer>19)
			{
			printf("default\n");
			timer=0;
			getDiskList();
			XdbeSwapBuffers(display,&swapInfo,1);
			}
				//	printf("xxx\n");
				break;
				}
//XSendEvent(display,DefaultRootWindow(display),false,ExposureMask | KeyPressMask | ButtonPress |
//                          StructureNotifyMask | ButtonReleaseMask |
 //                         KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
  //                        PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
   //                       ColormapChangeMask,&ev);

//			usleep(100000);

			//getDiskList();
			//XdbeSwapBuffers(display,&swapInfo,1);

		}

	XClearWindow(display,rootWin);
	XCloseDisplay(display);

	return 0;
}

