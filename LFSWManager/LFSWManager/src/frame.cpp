/*
 * Copyright 2010 Johan Veenhuizen
 *
 * Permission is hereby granted,free of charge,to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,including without limitation
 * the rights to use,copy,modify,merge,publish,distribute,sublicense,
 * and/or sell copies of the Software,and to permit persons to whom the
 * Software is furnished to do so,subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS",WITHOUT WARRANTY OF ANY KIND,EXPRESS OR
 * IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,DAMAGES OR
 * OTHER LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE,
 * ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

//#define NDEBUG
//#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/shape.h>

#include "wind.h"
#include "client.h"
#include "button.h"
#include "lib.h"
#include "ewmh.h"
#include "dragger.h"
#include "frame.h"
#include "atoms.h"

int		depth=0;
size_t	fcount;
Cursor	cursortopleft=None;
Cursor	cursortopright=None;
bool	swapdesk=false;
int		nx;
Window	windowToUpdate=None;
int		newwid,newhite;
int		updatecnt=0;
int		maxupdatecnt=10;
bool	fromDragger=false;

/*
 * Move and resize the frame,and update the client window.
 */

void moveresize(struct frame *f,int x,int y,int w,int h)
{
	int		offset;
	bool	left;
	int		newd=-1;
	Desk	d;

	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
	int				flipborder=10;
	int				flippedoffset=flipborder;
	int				monitorwidth;

	if (x==f->x && y==f->y && w==f->width && h==f->height)
		return;

	doswapdesk=-1;
	monitorwidth=monitorData[getMouseMonitor(f->client)].monW;

	XQueryPointer(dpy,DefaultRootWindow(dpy),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return);
	flippedoffset=win_x_return-x;

	struct geometry old=cgetgeom(f->client);
	geometry mynew;
	mynew.x=x + frameLeft;
	mynew.y=y + frameTop;
	mynew.width=w-frameLeft-frameRight;
	mynew.height=h-frameTop-frameBottom;
	mynew.borderwidth=old.borderwidth;

	if(mynew.height<0)
		mynew.height=0;

	if((win_x_return>flipborder) && (win_x_return<monitorwidth-flipborder))
		swapdesk=false;
	if(swapdesk==false)
		nx=x;

	if(((win_x_return<flipborder) || (win_x_return>(monitorwidth-flipborder))) && (swapdesk==false))
		{

			if(win_x_return>(monitorwidth-flipborder))
				{
					left=false;
					offset=flipborder-flippedoffset;
				}
			else
				{
					left=true;
					offset=monitorwidth-flippedoffset;
				}
			mynew.x=offset;
			swapdesk=true;
			nx=offset;
			d=f->client->desk;
			if(left==true)
				{
					newd=(int)d-1;
					if(newd<0)
						newd=numberOfDesktops-1;
				}
			else
				{
					newd=d+1;
					if(newd>(int)numberOfDesktops-1)
						newd=0;
				}
					doswapdesk=newd;
			f->client->desk=newd;
		}

	csetgeom(f->client,mynew);

	if(f->isShaded==false)
		{
			XMoveResizeWindow(dpy,f->window,nx,y,w,h);
			windowToUpdate=f->client->window;
			newwid=mynew.width;
			newhite=mynew.height;
			f->x=nx;
			f->y=y;
			f->width=w;
			f->height=h;
		}
	else
		{
			XMoveResizeWindow(dpy,f->window,nx,y,w,frameTop);
			windowToUpdate=f->client->window;
			newwid=mynew.width;
			newhite=mynew.height;
			f->x=nx;
			f->y=y;
			f->width=w;
			f->height=frameTop;
		}

	if (mynew.width==old.width && mynew.height==old.height)
		csendconf(f->client);
	else
		{
			if((liveUpdate>0 && updatecnt>liveUpdate) || (fromDragger==false))
				{
					XResizeWindow(dpy,f->client->window,mynew.width,mynew.height);
					updatecnt=0;
				}
			else
				updatecnt++;
		}

	if(newd!=-1)
		{
			if(win_x_return>(monitorwidth-flipborder))
				XWarpPointer(dpy,None,None,0,0,0,0,0-monitorwidth+flipborder,0);
			else
				XWarpPointer(dpy,None,None,0,0,0,0,monitorwidth-flipborder,0);
		}
}

void mydelete(void *myclient,Time t)
{
	cdelete((client*)myclient,t);
}

void minimizeWindow(void *myclient,Time t)
{
	XIconifyWindow(dpy,((client*)myclient)->window,screen);
}

void shadeWindow(void *myclient,Time t)
{
	if(((client*)myclient)->frame->isShaded==false)
		{
			((client*)myclient)->frame->oldHeight=((client*)myclient)->frame->height;
			((client*)myclient)->frame->isShaded=true;
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->x,((client*)myclient)->frame->y,((client*)myclient)->frame->width,frameTop);
		}
	else
		{
			((client*)myclient)->frame->isShaded=false;
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->x,((client*)myclient)->frame->y,((client*)myclient)->frame->width,((client*)myclient)->frame->oldHeight);
 		}		
}

int getFrameMonitor(frame *f)
{
	for(unsigned int j=0;j<numberOfMonitors;j++)
		{
			if((f->x>monitorData[j].monX) && (f->x<monitorData[j].monW+monitorData[j].monX) && (f->y>monitorData[j].monY) && (f->y<monitorData[j].monH+monitorData[j].monY))
				return(j);
		}
	return(0);

}

void maximizeWindow(void *myclient,Time t)
{
	int		monnum=getFrameMonitor(((client*)myclient)->frame);

	if(((client*)myclient)->frame->isMaximized==false)
		{
			((client*)myclient)->frame->oldX=((client*)myclient)->frame->x;
			((client*)myclient)->frame->oldY=((client*)myclient)->frame->y;
			((client*)myclient)->frame->oldWidth=((client*)myclient)->frame->width;
			((client*)myclient)->frame->oldHeight=((client*)myclient)->frame->height;
			((client*)myclient)->frame->isMaximized=true;
			moveresize(((client*)myclient)->frame,monitorData[monnum].monX,monitorData[monnum].monY,monitorData[monnum].monW,monitorData[monnum].monH);
			changestate(((client*)myclient)->window,NET_WM_STATE_ADD,NET_WM_STATE_MAXIMIZED_VERT);
			changestate(((client*)myclient)->window,NET_WM_STATE_ADD,NET_WM_STATE_MAXIMIZED_HORZ);
		}
	else
		{
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->oldX,((client*)myclient)->frame->oldY,((client*)myclient)->frame->oldWidth,((client*)myclient)->frame->oldHeight);
			((client*)myclient)->frame->x=((client*)myclient)->frame->oldX;
			((client*)myclient)->frame->y=((client*)myclient)->frame->oldY;
			((client*)myclient)->frame->width=((client*)myclient)->frame->oldWidth;
			((client*)myclient)->frame->height=((client*)myclient)->frame->oldHeight;
			((client*)myclient)->frame->isMaximized=false;
			changestate(((client*)myclient)->window,NET_WM_STATE_REMOVE,NET_WM_STATE_MAXIMIZED_VERT);
			changestate(((client*)myclient)->window,NET_WM_STATE_REMOVE,NET_WM_STATE_MAXIMIZED_HORZ);
		}
}

/*
 * XXX: We cheat here and always estimate normal frame
 * extents,even if the window is of a type that will
 * not get a frame. This is hopefully okay since most
 * clients requesting estimates of frame extents will
 * probably be interested in having a frame.
 */
struct extents estimateframeextents(Window w)
{
	return (struct extents)
	{
		.top=frameTop,
		 .bottom=frameBottom,
		  .left=frameLeft,
		   .right=frameRight
	};
}

void reorder(Window ref,Window below)
{
	Window	w[2]= {ref,below};
	XRestackWindows(dpy,w,2);
}

void setgrav(Window win,int grav)
{
	XSetWindowAttributes	wa;
	wa.win_gravity=grav;

	XChangeWindowAttributes(dpy,win,CWWinGravity,&wa);
}

void gravitate(int wingrav,int borderwidth,int *dx,int *dy)
{
	switch (wingrav)
		{
		case NorthWestGravity:
			*dx=0;
			*dy=0;
			break;
		case NorthGravity:
			*dx=borderwidth-(frameLeft + frameRight) / 2;
			*dy=0;
			break;
		case NorthEastGravity:
			*dx=(2 * borderwidth)-(frameLeft + frameRight);
			*dy=0;
			break;
		case WestGravity:
			*dx=0;
			*dy=borderwidth-(frameTop + frameBottom) / 2;
			break;
		case CenterGravity:
			*dx=borderwidth-(frameLeft + frameRight) / 2;
			*dy=borderwidth-(frameTop + frameBottom) / 2;
			break;
		case EastGravity:
			*dx=(2 * borderwidth)-(frameLeft + frameRight);
			*dy=borderwidth-(frameTop + frameBottom) / 2;
			break;

		case SouthWestGravity:
			*dx=0;
			*dy=(2 * borderwidth)-(frameTop + frameBottom);
			break;
		case SouthGravity:
			*dx=borderwidth-(frameLeft + frameRight) / 2;
			*dy=(2 * borderwidth)-(frameTop + frameBottom);
			break;
		case SouthEastGravity:
			*dx=(2 * borderwidth)-(frameLeft + frameRight);
			*dy=(2 * borderwidth)-(frameTop + frameBottom);
			break;

		case StaticGravity:
			*dx=borderwidth-frameLeft;
			*dy=borderwidth-frameTop;;
			break;

		default:
			errorf("unknown window gravity %d",wingrav);
			*dx=0;
			*dy=0;
			break;
		}
}

void repaint(struct frame *f)
{
	int			namewidth=f->namewidth;
	int			partoffset;
	XGCValues	values;
	int			leftoffset=0;
	int			riteoffset=f->width;
	GC			gc;
	GC			gcmask;
	GC			maskgc;

//TODO//
int buttonwidth=theme.partsHeight[TOPLEFTACTIVE];
	namewidth=MIN(namewidth,f->width-2 * (1 + font->size));
	namewidth=MAX(namewidth,0);

	values.fill_style=FillTiled;

	if(theme.useTheme==true)
		{
			if(chasfocus(f->client)==true)
				partoffset=0;
			else
				partoffset=1;

			if(f->mask!=None)
				XFreePixmap(dpy,f->mask);

			gc=XCreateGC(dpy,f->window,GCFillStyle,&values);
			XSetClipMask(dpy,gc,None);


			f->mask=XCreatePixmap(dpy,f->window,f->width,f->height,1);
			XSetForeground(dpy,f->maskGC,blackColor);
			XSetClipMask(dpy,f->maskGC,None);
			XSetFillStyle(dpy,f->maskGC,FillSolid);
			XFillRectangle(dpy,f->mask,f->maskGC,0,0,f->width+100,f->height+100);
			XSetFillStyle(dpy,f->maskGC,FillTiled);
	
//			maskgc=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);
			XSetFillStyle(dpy,f->maskGC,FillTiled);
	
//			maskgc=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);

//top corners
//left

			XSetClipMask(dpy,*f->background,theme.masks[TOPLEFTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,0,0);
			XCopyArea(dpy,theme.pixmaps[TOPLEFTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);
			XCopyArea(dpy,theme.masks[TOPLEFTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);
			leftoffset=theme.partsWidth[TOPLEFTACTIVE+partoffset];

//rite
			XSetClipMask(dpy,*f->background,theme.masks[TOPRIGHTACTIVE+partoffset]);
			XSetClipMask(dpy,*f->background,None);
			//XSetClipOrigin(dpy,*f->background,f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],sh);
			XCopyArea(dpy,theme.pixmaps[TOPRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.masks[TOPRIGHTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);




			XSetClipMask(dpy,*f->background,theme.masks[TOPRIGHTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.pixmaps[TOPRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPRIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.masks[TOPRIGHTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPRIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);

			riteoffset-=theme.partsWidth[TOPRIGHTACTIVE+partoffset];

/*
			XSetClipMask(dpy,*f->background,theme.masks[TOPLEFTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,0,0);
			XCopyArea(dpy,theme.pixmaps[TOPLEFTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);
			XCopyArea(dpy,theme.masks[TOPLEFTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);
			leftoffset=theme.partsWidth[TOPLEFTACTIVE+partoffset];

			XSetClipMask(dpy,*f->background,theme.masks[TOPRIGHTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.pixmaps[TOPRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.masks[TOPRIGHTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			riteoffset-=theme.partsWidth[TOPRIGHTACTIVE+partoffset];
*/
				XSetTSOrigin(dpy,f->maskGC,0,0);

//title1
			if(theme.gotPart[TITLE1ACTIVE+partoffset]==true)
				{
					values.tile=theme.pixmaps[TITLE1ACTIVE+partoffset];
					XSetTile(dpy,gc,values.tile);
					XFillRectangle(dpy,f->window,gc,leftoffset,0,theme.partsWidth[TITLE1ACTIVE+partoffset],theme.partsHeight[TITLE1ACTIVE+partoffset]);
	
					values.tile=theme.masks[TITLE1ACTIVE+partoffset];
					XSetTile(dpy,f->maskGC,values.tile);
/*					
					gcmask=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);
					XSetClipMask(dpy,gcmask,None);
					XFillRectangle(dpy,f->mask,gcmask,leftoffset,0,theme.partsWidth[TITLE1ACTIVE+partoffset],theme.partsHeight[TITLE1ACTIVE+partoffset]);
					XFreeGC(dpy,gcmask);
*/
					//gcmask=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);
					//XSetClipMask(dpy,f->maskGC,None);
					XFillRectangle(dpy,f->mask,f->maskGC,leftoffset,0,theme.partsWidth[TITLE1ACTIVE+partoffset],theme.partsHeight[TITLE1ACTIVE+partoffset]);
					//XFreeGC(dpy,gcmask);

					leftoffset+=theme.partsWidth[TITLE1ACTIVE+partoffset];
				}

//title2 end cap
			if(theme.gotPart[TITLE2ACTIVE+partoffset]==true)
				{
					XSetClipMask(dpy,*f->background,None);
					XCopyArea(dpy,theme.pixmaps[TITLE2ACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TITLE2ACTIVE+partoffset],theme.partsHeight[TITLE2ACTIVE+partoffset],leftoffset,0);
					XCopyArea(dpy,theme.masks[TITLE2ACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TITLE2ACTIVE+partoffset],theme.partsHeight[TITLE2ACTIVE+partoffset],leftoffset,0);
					leftoffset+=theme.partsWidth[TITLE2ACTIVE+partoffset];
				}

//title5 button back
			if(theme.gotPart[TITLE5ACTIVE+partoffset]==true)
				{
					values.tile=theme.pixmaps[TITLE5ACTIVE+partoffset];
					XSetTile(dpy,gc,values.tile);
					XFillRectangle(dpy,f->window,gc,riteoffset-theme.partsWidth[TITLE5ACTIVE+partoffset],0,theme.partsWidth[TITLE5ACTIVE+partoffset],theme.partsHeight[TITLE5ACTIVE+partoffset]);

					values.tile=theme.masks[TITLE5ACTIVE+partoffset];
					XSetTile(dpy,f->maskGC,values.tile);
					//XSetClipMask(dpy,f->maskGC,None);

					//gcmask=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);
					//XSetClipMask(dpy,gcmask,None);
					//XFillRectangle(dpy,f->mask,gcmask,riteoffset-theme.partsWidth[TITLE5ACTIVE+partoffset],0,theme.partsWidth[TITLE5ACTIVE+partoffset],100);
					
					//theme.partsHeight[TITLE5ACTIVE+partoffset]);
					//XFreeGC(dpy,gcmask);
					XSetTile(dpy,f->maskGC,theme.masks[TITLE5ACTIVE+partoffset]);
					XFillRectangle(dpy,f->mask,f->maskGC,riteoffset-theme.partsWidth[TITLE5ACTIVE+partoffset],0,theme.partsWidth[TITLE5ACTIVE+partoffset],theme.partsHeight[TITLE5ACTIVE+partoffset]);

					riteoffset-=theme.partsWidth[TITLE5ACTIVE+partoffset];
				}


//title4 rite endcap
			if(theme.gotPart[TITLE4ACTIVE+partoffset]==true)
				{
					XSetClipMask(dpy,*f->background,None);
					riteoffset-=theme.partsWidth[TITLE4ACTIVE+partoffset];
					XCopyArea(dpy,theme.pixmaps[TITLE4ACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TITLE4ACTIVE+partoffset],theme.partsHeight[TITLE4ACTIVE+partoffset],riteoffset,0);
					XCopyArea(dpy,theme.masks[TITLE4ACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TITLE4ACTIVE+partoffset],theme.partsHeight[TITLE4ACTIVE+partoffset],riteoffset,0);
				}

//title3 main centre bit
			if(theme.gotPart[TITLE3ACTIVE+partoffset]==true)
				{
					values.tile=theme.pixmaps[TITLE3ACTIVE+partoffset];
					XSetTile(dpy,gc,values.tile);
					XFillRectangle(dpy,f->window,gc,leftoffset,0,riteoffset-leftoffset,theme.partsHeight[TITLE3ACTIVE+partoffset]);

					XSetTile(dpy,f->maskGC,theme.masks[TITLE3ACTIVE]);
					XFillRectangle(dpy,f->mask,f->maskGC,leftoffset,0,riteoffset-leftoffset,theme.partsHeight[TITLE3ACTIVE+partoffset]);
					
/*
					values.tile=theme.masks[TITLE3ACTIVE+partoffset];
					gcmask=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);
					XSetClipMask(dpy,gcmask,None);
					XFillRectangle(dpy,f->mask,gcmask,leftoffset,0,riteoffset-leftoffset,theme.partsHeight[TITLE3ACTIVE+partoffset]);
					XFreeGC(dpy,gcmask);
*/
				}

//left
			if(theme.gotPart[LEFTACTIVE+partoffset]==true)
				{
					values.tile=theme.pixmaps[LEFTACTIVE+partoffset];
					XSetTile(dpy,gc,values.tile);
					XFillRectangle(dpy,f->window,gc,0,theme.partsHeight[TOPLEFTACTIVE+partoffset],theme.partsWidth[LEFTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]-theme.partsHeight[TOPLEFTACTIVE+partoffset]);

/*
					values.tile=theme.masks[LEFTACTIVE+partoffset];
					gcmask=XCreateGC(dpy,f->mask,GCTile|GCFillStyle,&values);
					XSetClipMask(dpy,gcmask,None);
					XFillRectangle(dpy,f->mask,gcmask,0,theme.partsHeight[TOPLEFTACTIVE+partoffset],theme.partsWidth[LEFTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]-theme.partsHeight[TOPLEFTACTIVE+partoffset]);
					XFreeGC(dpy,gcmask);
*/
			//	XSetFillStyle(dpy,f->maskGC,FillTiled);
			//	XSetClipMask(dpy,f->maskGC,None);
				XSetTile(dpy,f->maskGC,theme.masks[LEFTACTIVE+partoffset]);
				XSetTSOrigin(dpy,f->maskGC,0,0);
				
				XFillRectangle(dpy,f->mask,f->maskGC,0,theme.partsHeight[TOPLEFTACTIVE+partoffset],theme.partsWidth[LEFTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]-theme.partsHeight[TOPLEFTACTIVE+partoffset]);

				}
//right
			if(theme.gotPart[RIGHTACTIVE+partoffset]==true)
				{
					values.tile=theme.pixmaps[RIGHTACTIVE+partoffset];
					values.ts_x_origin=f->width-theme.partsWidth[RIGHTACTIVE+partoffset];
					XSetTSOrigin(dpy,gc,values.ts_x_origin,0);
					XSetTile(dpy,gc,values.tile);
					XFillRectangle(dpy,f->window,gc,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],theme.partsWidth[RIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]-theme.partsHeight[TOPRIGHTACTIVE+partoffset]);

/*
					values.tile=theme.masks[RIGHTACTIVE+partoffset];
					values.ts_x_origin=f->width-theme.partsWidth[RIGHTACTIVE+partoffset];
					values.fill_style=FillSolid;
					gcmask=XCreateGC(dpy,theme.masks[RIGHTACTIVE+partoffset],GCTile|GCFillStyle|GCTileStipXOrigin,&values);
					XSetClipMask(dpy,gcmask,theme.masks[RIGHTACTIVE+partoffset]);

				XSetForeground(dpy,gcmask,blackColor);
				XSetBackground(dpy,gcmask,blackColor);

				XSetForeground(dpy,f->maskGC,blackColor);
				XSetBackground(dpy,f->maskGC,blackColor);



					XFillRectangle(dpy,f->mask,gcmask,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],theme.partsWidth[RIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]-theme.partsHeight[TOPRIGHTACTIVE+partoffset]);
					XFreeGC(dpy,gcmask);
*/
/*
					values.ts_x_origin=f->width-theme.partsWidth[RIGHTACTIVE+partoffset];
					XSetTSOrigin(dpy,gc,values.ts_x_origin,0);
					XSetTile(dpy,f->maskGC,theme.masks[RIGHTACTIVE+partoffset]);
					//XSetTSOrigin(dpy,f->maskGC,values.ts_x_origin,0);
					XFillRectangle(dpy,f->mask,f->maskGC,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],theme.partsWidth[RIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]-theme.partsHeight[TOPRIGHTACTIVE+partoffset]);
*/
				
				//XSetForeground(dpy,f->maskGC,blackColor);
				//XSetFillStyle(dpy,f->maskGC,FillSolid);
				//XSetForeground(dpy,f->maskGC,blackColor);
				XSetTSOrigin(dpy,f->maskGC,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],0);
				XSetTile(dpy,f->maskGC,theme.masks[RIGHTACTIVE+partoffset]);
				XFillRectangle(dpy,f->mask,f->maskGC,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],theme.partsWidth[RIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]-theme.partsHeight[TOPRIGHTACTIVE+partoffset]);
				//XFillRectangle(dpy,f->mask,f->maskGC,f->width-100,0,1000,1000);
			}
//left bottom
			if(theme.gotPart[BOTTOMLEFTACTIVE+partoffset]==true)
				{
					XSetClipMask(dpy,*f->background,None);
					XCopyArea(dpy,theme.pixmaps[BOTTOMLEFTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[BOTTOMLEFTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],0,f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]);
					XCopyArea(dpy,theme.masks[BOTTOMLEFTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[BOTTOMLEFTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],0,f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]);
					leftoffset=theme.partsWidth[BOTTOMLEFTACTIVE+partoffset];
				}
//right bottom
			if(theme.gotPart[BOTTOMRIGHTACTIVE+partoffset]==true)
				{
					XSetClipMask(dpy,*f->background,None);
					XCopyArea(dpy,theme.pixmaps[BOTTOMRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],f->width-theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]);
					XCopyArea(dpy,theme.masks[BOTTOMRIGHTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],f->width-theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]);

					riteoffset=f->width-theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset];
				}

//bottom
			if(theme.gotPart[BOTTOMACTIVE+partoffset]==true)
				{
					values.tile=theme.pixmaps[BOTTOMACTIVE+partoffset];
					values.ts_y_origin=f->height-theme.partsHeight[BOTTOMACTIVE+partoffset];
					XSetTSOrigin(dpy,gc,0,values.ts_y_origin);
					XSetTile(dpy,gc,values.tile);
					XFillRectangle(dpy,f->window,gc,leftoffset,f->height-theme.partsHeight[BOTTOMACTIVE+partoffset],riteoffset-leftoffset,theme.partsHeight[BOTTOMACTIVE+partoffset]);

/*
					values.tile=theme.masks[BOTTOMACTIVE+partoffset];
					values.ts_y_origin=f->height-theme.partsHeight[BOTTOMACTIVE+partoffset];
					gcmask=XCreateGC(dpy,f->mask,GCTile|GCFillStyle|GCTileStipYOrigin,&values);
					XSetClipMask(dpy,gcmask,None);
					XFillRectangle(dpy,f->mask,gcmask,leftoffset,f->height-theme.partsHeight[BOTTOMACTIVE+partoffset],riteoffset-leftoffset,theme.partsHeight[BOTTOMACTIVE+partoffset]);
					XFreeGC(dpy,gcmask);
*/
					XSetTile(dpy,f->maskGC,theme.masks[BOTTOMACTIVE+partoffset]);
					XSetTSOrigin(dpy,f->maskGC,0,f->height-theme.partsHeight[BOTTOMACTIVE+partoffset]);
					XFillRectangle(dpy,f->mask,f->maskGC,leftoffset,f->height-theme.partsHeight[BOTTOMACTIVE+partoffset],riteoffset-leftoffset,theme.partsHeight[BOTTOMACTIVE+partoffset]);

				}

			XSetFillStyle(dpy,f->maskGC,FillSolid);
			XSetForeground(dpy,f->maskGC,whiteColor);
			XFillRectangle(dpy,f->mask,f->maskGC,theme.leftWidth,theme.titleBarHeight,f->width-theme.rightWidth-theme.leftWidth,f->height-theme.titleBarHeight-theme.bottomHeight);
			XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,f->mask,ShapeSet);
		}
	else
		{
	// Title area
			int x=1;
			XFillRectangle(dpy,f->window,*f->background,x,1,font->size,frameTop);
			x += font->size;
			if (f->pixmap != None)
				XCopyArea(dpy,f->pixmap,f->window,foreground,0,0,namewidth,frameTop,x,1);
			x += namewidth;
			XFillRectangle(dpy,f->window,*f->background,x,1,f->width-1-x,frameTop);

	// Border
			XDrawRectangle(dpy,f->window,foreground,0,0,f->width-1,f->height-1);

	// Title bottom border
			XDrawLine(dpy,f->window,foreground,frameLeft,frameTop-1,f->width-frameRight-1,frameTop-1);

	// Window area
			XFillRectangle(dpy,f->window,*f->background,1,frameTop,f->width-2,f->height-1-frameTop);

			XFillRectangle(dpy,f->window,*f->background,1,frameTop-1,frameLeft-1,1);
			XFillRectangle(dpy,f->window,*f->background,f->width-frameRight,frameTop-1,frameRight-1,1);
		}
}

void fupdate(struct frame *f)
{
	int sz=frameTop;
	int	buttonspacing=sz+100;

	if (chaswmproto(f->client,WM_DELETE_WINDOW))
		{
			if (f->deletebutton==NULL)
				{
					f->deletebutton=bcreate(mydelete,f->client,deletebitmap,f->window,f->width-1-font->size-buttonspacing,0,sz,sz,NorthEastGravity);
					buttonspacing+=sz;
				}
		}
	else if (f->deletebutton != NULL)
		{
			bdestroy(f->deletebutton);
			f->deletebutton=NULL;
		}

	if(f->client->canMaximize==true)
		{
			if (f->maximize==NULL)
				{
					f->maximize=bcreate(maximizeWindow,f->client,maximizeBitmap,f->window,f->width-font->size-buttonspacing,0,sz,sz,NorthEastGravity);
					buttonspacing+=sz;
				}
		}

	if(f->client->canMinimize==true)
		{
			if (f->minimize==NULL)
				{
					f->minimize=bcreate(minimizeWindow,f->client,minimizeBitmap,f->window,f->width-buttonspacing-font->size,0,sz,sz,NorthEastGravity);
					buttonspacing+=sz;
				}
		}

	if (f->shade==NULL)
		{
			f->shade=bcreate(shadeWindow,f->client,shadeBitmap,f->window,f->width-buttonspacing-font->size,0,sz,sz,NorthEastGravity);
		}

	Bool hasfocus=chasfocus(f->client);

	if(hasfocus)
		f->background=&hlbackground;
	else
		f->background=&background;

	if (f->pixmap != None)
		{
			XFreePixmap(dpy,f->pixmap);
			f->pixmap=None;
		}
	f->namewidth=namewidth(font,f->client);
	if (f->namewidth>0)
		{
			f->pixmap=XCreatePixmap(dpy,root,f->namewidth,frameTop,DefaultDepth(dpy,screen));
			XFillRectangle(dpy,f->pixmap,*f->background,0,0,f->namewidth,frameTop);
			drawname(f->pixmap,font,hasfocus ? fhighlight: fnormal,0,2 + font->ascent,f->client);

			if (f->client->desk==DESK_ALL)
				{
					int y=2 + font->ascent + font->descent / 2;
					XDrawLine(dpy,f->pixmap,hasfocus ? hlforeground : foreground,0,y,f->namewidth,y);
				}
		}

	repaint(f);
}

void confrequest(struct frame *f,XConfigureRequestEvent *e)
{
	struct geometry g=cgetgeom(f->client);

	if (e->value_mask & CWBorderWidth)
		{
			g.borderwidth=e->border_width;
			csetgeom(f->client,g);
		}

	int dx,dy;
	gravitate(cgetgrav(f->client),g.borderwidth,&dx,&dy);

	int x=f->x;
	int y=f->y;

	// Fetch requested geometry
	if (e->value_mask & CWX)
		x=e->x + dx;
	if (e->value_mask & CWY)
		y=e->y + dy;
	if (e->value_mask & CWWidth)
		g.width=e->width;
	if (e->value_mask & CWHeight)
		g.height=e->height;

	int width=g.width + frameLeft + frameRight;
	int height=g.height + frameTop + frameBottom;

	moveresize(f,x,y,width,height);
}

void buttonpress(struct frame *f,XButtonEvent *e)
{
	if (e->button==Button1)
		{
			cpopapp(f->client);
			cfocus(f->client,e->time);

			if (e->y<frameTop || (e->state & Mod1Mask) != 0)
				{
					f->grabbed=True;
					f->downx=e->x;
					f->downy=e->y;
					XGrabPointer(dpy,f->window,False,Button1MotionMask | ButtonReleaseMask,GrabModeAsync,GrabModeAsync,None,None,e->time);
				}
		}
}

void buttonrelease(struct frame *f,XButtonEvent *e)
{
	if (e->button==Button1 && f->grabbed)
		{
			XUngrabPointer(dpy,e->time);
			f->grabbed=False;
		}
}

void motionnotify(struct frame *f,XMotionEvent *e)
{
	moveresize(f,e->x_root-f->downx,e->y_root-f->downy,f->width,f->height);
}

void maprequest(struct frame *f,XMapRequestEvent *e)
{
	Window win=f->client->window;
	if (e->window==win)
		redirect((XEvent *)e,win);
}

void expose(struct frame *f,XExposeEvent *e)
{
	if (e->count==0)
		repaint(f);
}

void frameevent(void *self,XEvent *e)
{
//printf("frame event\n");
	switch (e->type)
		{
		case Expose:
//printf("expose event\n");
			expose((frame*)self,&e->xexpose);
			break;
		case MotionNotify:
			motionnotify((frame*)self,&e->xmotion);
			break;
		case ButtonPress:
			buttonpress((frame*)self,&e->xbutton);
			break;
		case ButtonRelease:
			buttonrelease((frame*)self,&e->xbutton);
			break;
		case ConfigureRequest:
//printf("ConfigureRequest event\n");
			confrequest((frame*)self,&e->xconfigurerequest);
			break;
		case MapRequest:
//printf("MapRequest event\n");
			maprequest((frame*)self,&e->xmaprequest);
			break;
		}
}

void resizetopleft(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
	struct frame *f=(frame*)self;

	int w=f->width-(xdrag-f->x);
	int h=f->height-(ydrag-f->y);

	w -= frameLeft + frameRight;
	h -= frameTop + frameBottom;
	chintsize(f->client,w,h,&w,&h);
	w += frameLeft + frameRight;
	h += frameTop + frameBottom;

	int x=f->x + f->width-w;
	int y=f->y + f->height-h;
	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,x,y,w,h);
}

void resizetopright(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
	struct frame *f=(frame*)self;

	int w=xdrag + 1-f->x;
	int h=f->height-(ydrag-f->y);

	w -= frameLeft + frameRight;
	h -= frameTop + frameBottom;
	chintsize(f->client,w,h,&w,&h);
	w += frameLeft + frameRight;
	h += frameTop + frameBottom;

	int x=f->x;
	int y=f->y + f->height-h;
	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,x,y,w,h);
}

struct frame *fcreate(struct client *c)
{
	XSetWindowAttributes	wa;
//	GC						gc;
	if (fcount==0)
		{
			cursortopleft=XCreateFontCursor(dpy,XC_top_left_corner);
			cursortopright=XCreateFontCursor(dpy,XC_top_right_corner);
		}
	fcount++;

	struct frame *f=(frame*)xmalloc(sizeof *f);

	f->client=c;
	f->pixmap=None;
	f->namewidth=0;

	struct geometry g=cgetgeom(c);
	int dx,dy;
	gravitate(cgetgrav(c),g.borderwidth,&dx,&dy);

	f->x=g.x + dx;
	f->y=g.y + dy;

	f->width=g.width + frameLeft + frameRight;

	f->height=g.height + frameTop + frameBottom;

	f->grabbed=False;
	f->oldX=f->x;
	f->oldY=f->y;
	f->oldWidth=f->width;
	f->oldHeight=f->height;
	f->isMaximized=false;
	f->isShaded=false;


	wa.bit_gravity=NorthWestGravity;

	f->window=XCreateWindow(dpy,root,f->x,f->y,f->width,f->height,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);

	f->mask=XCreatePixmap(dpy,f->window,f->width,f->height,1);
	f->maskGC=XCreateGC(dpy,f->mask,0,NULL);
	XSetForeground(dpy,f->maskGC,blackColor);
	XFillRectangle(dpy,f->mask,f->maskGC,0,0,f->width,f->height);

	Window clientwin=f->client->window;

	reorder(clientwin,f->window);

	f->listener.function=frameevent;
	f->listener.pointer=f;
	setlistener(f->window,&f->listener);

	XSelectInput(dpy,f->window,SubstructureRedirectMask |
	             ButtonPressMask |
	             ButtonReleaseMask |
	             ExposureMask);

	grabbutton(Button1,Mod1Mask,f->window,False,ButtonReleaseMask,GrabModeAsync,GrabModeAsync,None,None);

	/*
	 * The order in which the resizers and the delete button
	 * are created is important since it determines their
	 * stacking order. For very small windows it is important
	 * that the right resizer and the delete button are above
	 * the left resizer.
	 */

	int dw=font->size + 1;
	int dh=frameTop + 2;
	f->topleftresizer=dcreate(f->window,0,0,dw,dh,NorthWestGravity,cursortopleft,resizetopleft,f);
	f->toprightresizer=dcreate(f->window,f->width-dw,0,dw,dh,NorthEastGravity,cursortopright,resizetopright,f);

	f->deletebutton=NULL;
	f->maximize=NULL;
	f->minimize=NULL;
	f->shade=NULL;

	XSetWindowBorderWidth(dpy,clientwin,0);
	setgrav(clientwin,NorthWestGravity);
	if (f->client->ismapped)
		cignoreunmap(f->client);

	XReparentWindow(dpy,clientwin,f->window,frameLeft,frameTop);

	g.x += frameLeft;
	g.y += frameTop;
	csetgeom(f->client,g);
	csendconf(f->client);

	extents	ext;

	ext.top=frameTop;
	ext.bottom=frameBottom;
	ext.right=frameRight;
	ext.left=frameLeft;

	ewmh_notifyframeextents(clientwin,ext);

	fupdate(f);

	if (f->client->ismapped)
		XMapWindow(dpy,f->window);

	return f;
}

void fdestroy(struct frame *f)
{
	Bool hadfocus=chasfocus(f->client);

	XUnmapWindow(dpy,f->window);

	struct geometry g=cgetgeom(f->client);
	Window clientwin=f->client->window;

	XSetWindowBorderWidth(dpy,clientwin,g.borderwidth);
	int grav=cgetgrav(f->client);
	setgrav(clientwin,grav);
	int dx,dy;
	gravitate(grav,g.borderwidth,&dx,&dy);
	if (f->client->ismapped)
		cignoreunmap(f->client);
	g.x=f->x-dx;
	g.y=f->y-dy;
	csetgeom(f->client,g);
	XReparentWindow(dpy,clientwin,root,g.x,g.y);

	ewmh_notifyframeextents(clientwin,(struct extents)
	{
		.top=0,
		 .bottom=0,
		  .left=0,
		   .right=0
	});

	reorder(f->window,clientwin);
	if (hadfocus)
		cfocus(f->client,CurrentTime);
	setlistener(f->window,NULL);
	ddestroy(f->topleftresizer);
	ddestroy(f->toprightresizer);
	if (f->deletebutton != NULL)
		bdestroy(f->deletebutton);
	if (f->pixmap != None)
		XFreePixmap(dpy,f->pixmap);
	XDestroyWindow(dpy,f->window);
	free(f);

	//assert(fcount>0);
	fcount--;
	if (fcount==0)
		{
			XFreeCursor(dpy,cursortopleft);
			XFreeCursor(dpy,cursortopright);
		}
}

Window fgetwin(struct frame *f)
{
	return f->window;
}

struct geometry fgetgeom(struct frame *f)
{
	return (struct geometry)
	{
		.x=f->x,
		 .y=f->y,
		  .width=f->width,
		   .height=f->height,
		    .borderwidth=0
	};
}

