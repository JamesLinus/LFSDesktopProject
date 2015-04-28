
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

#define UNKNOWNARG -100

Display			*display;
Window			rootWin;
int				displayWidth;
int				displayHeight;
GC				gc;
Region			rg;
XdbeBackBuffer	buffer;
XdbeSwapInfo	swapInfo;

Visual			*visual=NULL;
int				depth=0;
int				screen;
int				blackColor;
int				whiteColor;

bool			done=true;
Pixmap			diskPixmap;
Pixmap			diskPixmapMask;
Imlib_Image		diskImage;
const char		*diskImagePath="/usr/share/icons/gnome/48x48/devices/drive-harddisk.png";

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

			rootWin=XCreateWindow(display,DefaultRootWindow(display),0,0,displayWidth,displayHeight,0,depth,InputOutput,visual,CWColormap | CWBorderPixel | CWBackPixel,&attr);
			XSelectInput(display,rootWin,StructureNotifyMask);

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
			XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);

			buffer=XdbeAllocateBackBufferName(display,rootWin,XdbeBackground);
			swapInfo.swap_window=rootWin;
			swapInfo.swap_action=XdbeBackground;
			XdbeSwapBuffers(display,&swapInfo,1);
//			if((XdbeSwapBuffers(display,&swapInfo,1)) && (useDBOveride==true))
//				{
//					useBuffer=true;
//					drawOnThis=buffer;
//				}
//			else
//				{
//					printf("no double buffering\n");
//					useBuffer=false;
//					drawOnThis=rootWin;
//				}
		}

}

#if 0
void blitToMonitorPos(int mode,char* path,int x,int y,int w,int h)
{
	Imlib_Image	image;
	int			imagew,imageh;

	imlib_context_set_blend(1);

	image=imlib_load_image_immediately_without_cache(path);
	if(image==NULL)
		return;
	imlib_context_set_image(image);
	imagew=imlib_image_get_width();
	imageh=imlib_image_get_height();

	switch(mode)
		{
		case STRETCH:
			imlib_context_set_image(buffer);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,x,y,w,h);
			break;
		case TILE:
			int	numx,numy;

			numx=(w/imagew)+1;
			numy=(h/imageh)+1;
			imlib_context_set_image(buffer);
			for(int yy=0; yy<numy; yy++)
				{
					for(int xx=0; xx<numx; xx++)
						imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,(xx*imagew)+x,(yy*imageh)+y,imagew,imageh);
				}
			break;
		case CENTRE:
			int xoffset,yoffset;
			xoffset=(w/2)-(imagew/2);
			yoffset=(h/2)-(imageh/2);

			imlib_context_set_image(buffer);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,xoffset+x,yoffset+y,imagew,imageh);
			break;
		}
	imlib_context_set_image(image);
	imlib_free_image();
}
#endif

void getDiskList(void)
{
	FILE	*fp;
	char	*command;
	char	line[2048];
	int		diskx=10;
	int		disky=100;
printf("111111111\n");
	imlib_context_set_blend(1);
	asprintf(&command,"blkid -o device");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					XSetClipMask(display,gc,diskPixmapMask);
					XSetClipOrigin(display,gc,diskx,disky);
					XCopyArea(display,diskPixmap,rootWin,gc,0,0,48,48,diskx,disky);
					diskx=diskx+50;
				}
			fclose(fp);
		}
}

int main(int argc,char **argv)
{
	int c;
	XEvent	ev;

	done=true;

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
	gc=XCreateGC(display,buffer,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);
	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);


//	diskImage=imlib_load_image(diskImagePath);
//	imlib_context_set_drawable(rootWin);
//	imlib_image_set_has_alpha(0);
//	imlib_render_pixmaps_for_whole_image(&backDropPixmap,NULL);
//	imlib_free_image();
	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);
	diskImage=imlib_load_image(diskImagePath);
	imlib_context_set_image(diskImage);
	imlib_context_set_drawable(rootWin);
	imlib_image_set_has_alpha(1);
printf("2222222222222\n");
	imlib_render_pixmaps_for_whole_image(&diskPixmap,&diskPixmapMask);
printf("33333333\n");
	imlib_free_image();

#if 0
	if(diskImage!=NULL)
		{
			imlib_context_set_image(diskImage);
			imlib_image_set_has_alpha(1);
			imlib_context_set_drawable(rootWin);
			imlib_render_pixmaps_for_whole_image(&diskPixmap,&diskPixmapMask);
			imlib_free_image();
		}
#endif
	while (done)
		{
			while (XPending(display))
				XNextEvent(display,&ev);

			switch(ev.type)
				{
				case ClientMessage:
					if (ev.xclient.message_type == XInternAtom(display,"WM_PROTOCOLS",1) && (Atom)ev.xclient.data.l[0] == XInternAtom(display,"WM_DELETE_WINDOW",1))
						done=false;
					continue;

					break;
				}

			usleep(1000000);

			getDiskList();
			//XdbeSwapBuffers(display,&swapInfo,1);

		}

	XClearWindow(display,rootWin);
	XCloseDisplay(display);

	return 0;
}

