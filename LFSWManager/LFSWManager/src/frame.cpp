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

#include <cairo.h>
#include <cairo-xlib.h>

#include "wind.h"
#include "client.h"
#include "button.h"
#include "lib.h"
#include "ewmh.h"
#include "dragger.h"
#include "frame.h"
#include "atoms.h"

#define EXT_TOP (lineheight + 2)
#define EXT_BOTTOM (halfleading + 1)
#define EXT_LEFT (halfleading + 1)
#define EXT_RIGHT (halfleading + 1)

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
	Region			rg;
	XRectangle		rect;

	if (x==f->x && y==f->y && w==f->width && h==f->height)
		return;

	doswapdesk=-1;
	monitorwidth=monitorData[getMouseMonitor(f->client)].monW;

	XQueryPointer(dpy,DefaultRootWindow(dpy),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return);
	flippedoffset=win_x_return-x;

	struct geometry old=cgetgeom(f->client);
	struct geometry mynew =
	{
		.x=x + EXT_LEFT,
		.y=y + EXT_TOP,
		.width=w-EXT_LEFT-EXT_RIGHT,
		.height=h-EXT_TOP-EXT_BOTTOM,
		.borderwidth=old.borderwidth,
	};

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
			XMoveResizeWindow(dpy,f->window,nx,y,w,EXT_TOP);
			windowToUpdate=f->client->window;
			newwid=mynew.width;
			newhite=mynew.height;
			f->x=nx;
			f->y=y;
			f->width=w;
			f->height=EXT_TOP;
		}

	if (mynew.width==old.width && mynew.height==old.height)
		csendconf(f->client);
	else
		{
			if(theme.useTheme==true)
				{
					rect.x=0;
					rect.y=0;
					rect.width=f->width;
					rect.height=f->height;
					rg=XCreateRegion();
					XUnionRectWithRegion(&rect,rg, rg);
					XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg,ShapeSet);
					XDestroyRegion(rg);
				}

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
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->x,((client*)myclient)->frame->y,((client*)myclient)->frame->width,EXT_TOP);
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
		.top=EXT_TOP,
		 .bottom=EXT_BOTTOM,
		  .left=EXT_LEFT,
		   .right=EXT_RIGHT
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
			*dx=borderwidth-(EXT_LEFT + EXT_RIGHT) / 2;
			*dy=0;
			break;
		case NorthEastGravity:
			*dx=(2 * borderwidth)-(EXT_LEFT + EXT_RIGHT);
			*dy=0;
			break;
		case WestGravity:
			*dx=0;
			*dy=borderwidth-(EXT_TOP + EXT_BOTTOM) / 2;
			break;
		case CenterGravity:
			*dx=borderwidth-(EXT_LEFT + EXT_RIGHT) / 2;
			*dy=borderwidth-(EXT_TOP + EXT_BOTTOM) / 2;
			break;
		case EastGravity:
			*dx=(2 * borderwidth)-(EXT_LEFT + EXT_RIGHT);
			*dy=borderwidth-(EXT_TOP + EXT_BOTTOM) / 2;
			break;

		case SouthWestGravity:
			*dx=0;
			*dy=(2 * borderwidth)-(EXT_TOP + EXT_BOTTOM);
			break;
		case SouthGravity:
			*dx=borderwidth-(EXT_LEFT + EXT_RIGHT) / 2;
			*dy=(2 * borderwidth)-(EXT_TOP + EXT_BOTTOM);
			break;
		case SouthEastGravity:
			*dx=(2 * borderwidth)-(EXT_LEFT + EXT_RIGHT);
			*dy=(2 * borderwidth)-(EXT_TOP + EXT_BOTTOM);
			break;

		case StaticGravity:
			*dx=borderwidth-EXT_LEFT;
			*dy=borderwidth-EXT_TOP;;
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
	int		namewidth=f->namewidth;
	int		partoffset;

	namewidth=MIN(namewidth,f->width-2 * (1 + font->size));
	namewidth=MAX(namewidth,0);

	if(theme.useTheme==true)
		{
//			double	alpha=1.0;
//			cairo_pattern_t *pattern;
//			cairo_surface_t *sfc;
//			sfc=cairo_xlib_surface_create(dpy,f->window,visual,f->width,f->height);
//			cairo_xlib_surface_set_size(sfc,f->width,f->height);
//			cairo_t	*cr=cairo_create(sfc);
			
#if 0
			cairo_save(cr);
				cairo_translate(cr,0,0);
				cairo_set_source_surface(cr,theme.parts[TOPLEFTACTIVE],0,0);
				cairo_paint_with_alpha(cr,alpha);
			cairo_restore(cr);
#else
			if(chasfocus(f->client)==true)
				partoffset=0;
			else
				partoffset=1;
//top corners
			XSetClipMask(dpy,*f->background,theme.masks[TOPLEFTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,0,0);
			XCopyArea(dpy,theme.pixmaps[TOPLEFTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);

			XSetClipMask(dpy,*f->background,theme.masks[TOPRIGHTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.pixmaps[TOPRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XGCValues values;

			values.fill_style=FillTiled;
			values.tile=theme.pixmaps[TITLE1ACTIVE+partoffset];
			GC gc=XCreateGC(dpy,f->window,GCTile|GCFillStyle,&values);
			XSetClipMask(dpy,gc,None);
			XFillRectangle(dpy,f->window,gc,theme.partsWidth[TOPLEFTACTIVE+partoffset],0,f->width-theme.partsWidth[TOPLEFTACTIVE+partoffset]-theme.partsWidth[TOPRIGHTACTIVE+partoffset],theme.partsHeight[TITLE1ACTIVE+partoffset]);

#endif
		//	XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,theme.masks[TOPLEFTACTIVE],ShapeSet);
			//XShapeCombineMask(dpy,f->window,ShapeBounding,f->width-theme.partsWidth[TOPRIGHTACTIVE],0,theme.masks[TOPRIGHTACTIVE],ShapeUnion);
		//	XShapeCombineMask(dpy,f->window,ShapeBounding,100,0,theme.masks[TOPRIGHTACTIVE],ShapeSet);
#if 0
			double	alpha=1.0;
			cairo_pattern_t *pattern;
	cairo_surface_t *sfc;
	sfc=cairo_xlib_surface_create(dpy,f->window,visual,f->width,f->height);
	cairo_xlib_surface_set_size(sfc,f->width,f->height);
	cairo_t	*cr=cairo_create(sfc);

			//int blackColor=BlackPixel(dpy,screen);
			//f->pixmap=None;
			//f->background=None;
			//GC gc=XCreateGC(dpy,f->window,0,NULL);
			//XSetForeground(dpy,gc,blackColor);
			//XSetFillStyle(dpy,gc,FillSolid);
			//XFillRectangle(display,bottomSnow.pixmap,gc,0,0,displayWidth,bottomSnow.maxHeight);
			//	XSetClipOrigin(dpy,gc,0,0);
//XSetClipMask(dpy,gc,None);			
			//XFillRectangle(dpy,f->window,gc,0,0,f->width,f->height);
			//XClearWindow(dpy,f->window);
			XClearArea(dpy,f->window,0,0,f->width,f->height,false);
//XShapeCombineMask(dpy, w, ShapeBounding, 0, 0, p, ShapeSubtract);			
/*
			cairo_save(cr);
				cairo_translate(cr,0,0);
				cairo_set_source_surface(cr,theme.parts[TOPLEFTACTIVE],0,0);
				cairo_paint_with_alpha(cr,alpha);
			cairo_restore(cr);
			XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,theme.masks[TOPLEFTACTIVE],ShapeSubtract);
			cairo_save(cr);
				cairo_translate(cr,f->width-theme.partsWidth[TOPRIGHTACTIVE],0);
				cairo_set_source_surface(cr,theme.parts[TOPRIGHTACTIVE],0,0);
				cairo_paint_with_alpha(cr,alpha);
			cairo_restore(cr);
*/
			cairo_save(cr);
				pattern=cairo_pattern_create_for_surface(theme.parts[TITLE1ACTIVE]);
				cairo_pattern_set_extend(pattern,CAIRO_EXTEND_REPEAT);
				cairo_translate(cr,0+theme.partsWidth[TOPLEFTACTIVE],0);
				cairo_set_source (cr, pattern);
				cairo_rectangle (cr, 0, 0,f->width-theme.partsWidth[TOPRIGHTACTIVE]-theme.partsWidth[TOPLEFTACTIVE],theme.titleBarHeight);
				cairo_fill (cr);
			cairo_restore(cr);
#endif
#if 0
Region rg=XCreateRegion();
//					XShapeCombineRegion(dpy,f->window,ShapeInput,0,0,rg,ShapeSet);
		//	XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,theme.masks[TITLE1ACTIVE],ShapeSubtract);
		//	XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,theme.masks[TITLE1ACTIVE],ShapeSet);
			XRectangle				rect;
			rect.x=0;
			rect.y=0;
			rect.width=f->width;
			rect.height=f->height;
			XDestroyRegion(rg);
			rg=XCreateRegion();
			XUnionRectWithRegion(&rect,rg, rg);

			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg,ShapeSet);
			rect.width=200;
			rect.height=32;
			XDestroyRegion(rg);
			rg=XCreateRegion();
			XUnionRectWithRegion(&rect,rg, rg);
			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg,ShapeSubtract);
			
//			XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,theme.masks[TITLE1ACTIVE],ShapeSubtract);
#endif
			XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,theme.inverseMasks[TOPLEFTACTIVE],ShapeSubtract);
			XShapeCombineMask(dpy,f->window,ShapeBounding,f->width-theme.partsWidth[TOPRIGHTACTIVE],0,theme.inverseMasks[TOPRIGHTACTIVE],ShapeSubtract);
		}
	else
		{
	// Title area
			int x=1;
			XFillRectangle(dpy,f->window,*f->background,x,1,font->size,lineheight);
			x += font->size;
			if (f->pixmap != None)
				XCopyArea(dpy,f->pixmap,f->window,foreground,0,0,namewidth,lineheight,x,1);
			x += namewidth;
			XFillRectangle(dpy,f->window,*f->background,x,1,f->width-1-x,lineheight);

	// Border
			XDrawRectangle(dpy,f->window,foreground,0,0,f->width-1,f->height-1);

	// Title bottom border
			XDrawLine(dpy,f->window,foreground,EXT_LEFT,EXT_TOP-1,f->width-EXT_RIGHT-1,EXT_TOP-1);

	// Window area
			XFillRectangle(dpy,f->window,*f->background,1,EXT_TOP,f->width-2,f->height-1-EXT_TOP);

			XFillRectangle(dpy,f->window,*f->background,1,EXT_TOP-1,EXT_LEFT-1,1);
			XFillRectangle(dpy,f->window,*f->background,f->width-EXT_RIGHT,EXT_TOP-1,EXT_RIGHT-1,1);

#if 1
			Region rg,rg2;
			XRectangle				rect;
#if 0
			rect.x=0;
			rect.y=0;
			rect.width=f->width;
			rect.height=f->height;
			rg=XCreateRegion();
			XUnionRectWithRegion(&rect,rg, rg);
			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg,ShapeSet);
#endif
#if 1
#if 0
			rect.x=0;
			rect.y=0;
			rect.width=200;
			rect.height=32;
			rg2=XCreateRegion();
			XUnionRectWithRegion(&rect,rg2, rg2);
			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg2,ShapeSubtract);
			rect.x=500;
			rg2=XCreateRegion();
			XUnionRectWithRegion(&rect,rg2, rg2);
			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg2,ShapeSubtract);
#endif
#if 0
Pixmap p;
GC shape_gc;
int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

p = XCreatePixmap(dpy,f->window, 200, 32, 1);
shape_gc = XCreateGC(dpy, p, 0, NULL);
XSetForeground(dpy, shape_gc, whiteColor);


Pixmap inverse;
GC inversegc;

inverse=XCreatePixmap(dpy,f->window,theme.partsWidth[TOPLEFTACTIVE],theme.partsHeight[TOPLEFTACTIVE], 1);
inversegc=XCreateGC(dpy,inverse,0,NULL);
//inversegc=*f->background;
XSetForeground(dpy,inversegc,whiteColor);
XFillRectangle(dpy,inverse,inversegc,0,0,theme.partsWidth[TOPLEFTACTIVE],theme.partsHeight[TOPLEFTACTIVE]);
XSetForeground(dpy,inversegc,blackColor);
XSetClipOrigin(dpy,inversegc,0,0);
XSetClipMask(dpy,inversegc,theme.masks[TOPLEFTACTIVE]);
XFillRectangle(dpy,inverse,inversegc,0,0,theme.partsWidth[TOPLEFTACTIVE],theme.partsHeight[TOPLEFTACTIVE]);

//XFillRectangle(dpy, p, shape_gc, 0, 0, 200, 32);
#endif
	//XShapeCombineMask(dpy,f->window,ShapeBounding,500,0,p,ShapeSubtract);		
#endif
#endif


		}
}

void fupdate(struct frame *f)
{
	int sz=lineheight+2;
	int	buttonspacing=sz;

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
			f->pixmap=XCreatePixmap(dpy,root,f->namewidth,lineheight,DefaultDepth(dpy,screen));
			XFillRectangle(dpy,f->pixmap,*f->background,0,0,f->namewidth,lineheight);
			drawname(f->pixmap,font,hasfocus ? fhighlight: fnormal,0,halfleading + font->ascent,f->client);

			if (f->client->desk==DESK_ALL)
				{
					int y=halfleading + font->ascent + font->descent / 2;
					XDrawLine(dpy,f->pixmap,hasfocus ? hlforeground : foreground,0,y,f->namewidth,y);
				}
		}

	repaint(f);
#if 0
			Region rg,rg2;
			XRectangle				rect;
			rect.x=0;
			rect.y=0;
			rect.width=200;
			rect.height=32;
			rg2=XCreateRegion();
			XUnionRectWithRegion(&rect,rg2, rg2);
			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg2,ShapeSubtract);
#endif


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

	int width=g.width + EXT_LEFT + EXT_RIGHT;
	int height=g.height + EXT_TOP + EXT_BOTTOM;

	moveresize(f,x,y,width,height);
}

void buttonpress(struct frame *f,XButtonEvent *e)
{
	if (e->button==Button1)
		{
			cpopapp(f->client);
			cfocus(f->client,e->time);

			if (e->y<EXT_TOP || (e->state & Mod1Mask) != 0)
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

	w -= EXT_LEFT + EXT_RIGHT;
	h -= EXT_TOP + EXT_BOTTOM;
	chintsize(f->client,w,h,&w,&h);
	w += EXT_LEFT + EXT_RIGHT;
	h += EXT_TOP + EXT_BOTTOM;

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

	w -= EXT_LEFT + EXT_RIGHT;
	h -= EXT_TOP + EXT_BOTTOM;
	chintsize(f->client,w,h,&w,&h);
	w += EXT_LEFT + EXT_RIGHT;
	h += EXT_TOP + EXT_BOTTOM;

	int x=f->x;
	int y=f->y + f->height-h;
	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,x,y,w,h);
}
#include <X11/extensions/shape.h>
int				depth=0;

int get_argb_visual(Visual** vis, int *depth)
{
	/* code from gtk project, gdk_screen_get_rgba_visual */
	XVisualInfo visual_template;
	XVisualInfo *visual_list=NULL;
	int nxvisuals = 0, i;
	visual_template.screen = screen;
	visual_list = XGetVisualInfo (dpy,0,&visual_template, &nxvisuals);

	for (i = 0; i < nxvisuals; i++)
		{
			if (visual_list[i].depth == 32 &&
			        (visual_list[i].red_mask   == 0xff0000 &&
			         visual_list[i].green_mask == 0x00ff00 &&
			         visual_list[i].blue_mask  == 0x0000ff ))
				{
					*vis = visual_list[i].visual;
					*depth = visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	printf("no rgb\n");
	XFree(visual_list);
	return 1;
}

struct frame *fcreate(struct client *c)
{
	XSetWindowAttributes wa;
	cairo_surface_t *sfc;
	int		rc=0;

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
	f->width=g.width + EXT_LEFT + EXT_RIGHT;
	f->height=g.height + EXT_TOP + EXT_BOTTOM;

	f->grabbed=False;
	f->oldX=f->x;
	f->oldY=f->y;
	f->oldWidth=f->width;
	f->oldHeight=f->height;
	f->isMaximized=false;
	f->isShaded=false;


	wa.bit_gravity=NorthWestGravity;
//	f->window=XCreateWindow(dpy,root,f->x,f->y,f->width,f->height,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
#if 0
if(theme.useTheme==true)
{
			rc=get_argb_visual(&visual,&depth);
			if(rc==0)
				{
					XSetWindowAttributes attr;
					attr.colormap=XCreateColormap(dpy,DefaultRootWindow(dpy),visual,AllocNone);
					attr.border_pixel=0;
					attr.background_pixel=0;
					attr.bit_gravity=NorthWestGravity;

					f->window=XCreateWindow(dpy,DefaultRootWindow(dpy),f->x,f->y,f->width,f->height,0,depth,InputOutput,visual,CWColormap | CWBorderPixel | CWBackPixel|CWBitGravity,&attr);
}
}
else
#endif
{
	f->window=XCreateWindow(dpy,root,f->x,f->y,f->width,f->height,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
}


	//sfc=cairo_xlib_surface_create(dpy,f->window,visual,f->width,f->height);
	//cairo_xlib_surface_set_size(sfc,f->width,f->height);
	//f->cr=cairo_create(sfc);

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
	int dh=lineheight + 2;
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
	XReparentWindow(dpy,clientwin,f->window,EXT_LEFT,EXT_TOP);

	g.x += EXT_LEFT;
	g.y += EXT_TOP;
	csetgeom(f->client,g);
	csendconf(f->client);

	extents	ext;

	ext.top=EXT_TOP;
	ext.bottom=EXT_BOTTOM;
	ext.right=EXT_RIGHT;
	ext.left=EXT_LEFT;

	ewmh_notifyframeextents(clientwin,ext);

	fupdate(f);

	if (f->client->ismapped)
		XMapWindow(dpy,f->window);
//Region rg=XCreateRegion();
//	XShapeCombineRegion(dpy,f->window,ShapeInput,0,0,rg,ShapeSet);
#if 0
			Region rg,rg2;
			XRectangle				rect;
			rect.x=0;
			rect.y=0;
			rect.width=200;
			rect.height=32;
			rg2=XCreateRegion();
			XUnionRectWithRegion(&rect,rg2, rg2);
			XShapeCombineRegion(dpy,f->window,ShapeBounding,0,0,rg2,ShapeSubtract);
#endif

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

