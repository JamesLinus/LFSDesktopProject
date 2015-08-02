/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:14:48 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKwindow.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <string.h>

#include "LFSTKWindow.h"
#include "lib.h"

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

void LFSTK_windowClass::initWindow(void)
{
	this->fontColourNames[FONTNORMALCOL]=strdup("white");
	this->fontColourNames[FONTHILITECOL]=strdup("black");
	this->fontColourNames[FONTACTIVECOL]=strdup("white");


	this->colourNames[NORMALCOLOUR].name=strdup("grey50");
	this->colourNames[PRELIGHTCOLOUR].name=strdup("grey80");
	this->colourNames[ACTIVECOLOUR].name=strdup("grey40");
}

LFSTK_windowClass::~LFSTK_windowClass()
{
	if(this->fontString!=NULL)
		free(this->fontString);

	for(int j=0;j<MAXFONTCOLS;j++)
		if(this->fontColourNames[j]!=NULL)
			free(this->fontColourNames[j]);

	for(int j=0;j<MAXCOLOURS;j++)
		if(this->colourNames[j].name!=NULL)
			free(this->colourNames[j].name);

	XFreeGC(this->display,this->gc);
	XDeleteContext(this->display,this->window,this->listeners);
	XDestroyWindow(this->display,this->window);
	XCloseDisplay(this->display);
}

LFSTK_windowClass::LFSTK_windowClass()
{
	this->initWindow();
}

void LFSTK_windowClass::LFSTK_loadGlobalColours(char* path)
{
	loadVarsFromFile(path,lfsToolKitGlobals);
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,globalWindowColours[j].name);
	globalColoursSet=true;
}

unsigned long LFSTK_windowClass::LFSTK_setColour(const char *name)
{
	XColor tc,sc;
	XAllocNamedColor(this->display,this->cm,name,&sc,&tc);
	return sc.pixel;
}

void LFSTK_windowClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);
	XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
}

void LFSTK_windowClass::LFSTK_setListener(Window w,listener *l)
{
	if (l==NULL)
		XDeleteContext(this->display,w,this->listeners);
	else
		XSaveContext(this->display,w,this->listeners,(XPointer)l);
}

void LFSTK_windowClass::LFSTK_resizeWindow(int w,int h)
{
	this->w=w;
	this->h=h;
	this->LFSTK_clearWindow();
}

listener* LFSTK_windowClass::LFSTK_getListener(Window w)
{
	listener *l=NULL;
	if (XFindContext(this->display,w,this->listeners,(XPointer *)&l)==0)
		return l;
	else
		return NULL;
}

void LFSTK_windowClass::LFSTK_setFontString(char *s)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);
	this->font=ftload(this,s);
}

void LFSTK_windowClass::LFSTK_setDecorated(bool isDecorated)
{
	Atom	xa;
	Atom	xa_prop[10];
	Hints	hints;

	if(isDecorated==false)
		{
			hints.flags=2;
			hints.decorations=0;
			hints.functions=0;
			hints.inputMode=0;
			hints.status=0;
			xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);
			XChangeProperty(this->display,this->window,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);
		}
}

geometryStruct *LFSTK_windowClass::LFSTK_getGeom()
{
	geometryStruct		*g=new geometryStruct;
	XWindowAttributes	xa;

	XGetWindowAttributes(this->display,this->window,&xa);

	g->x=xa.x;
	g->y=xa.y;
	g->w=xa.width;
	g->h=xa.height;
	return(g);
}

void LFSTK_windowClass::LFSTK_setFontColourName(int p,char* colour)
{
	this->fontColourNames[p]=strdup(colour);
}

void LFSTK_windowClass::LFSTK_setColourName(int p,char* colour)
{
	XColor tc,sc;

	this->colourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->colourNames[p].pixel=sc.pixel;
}

LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h,bool override)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XGCValues				gcv;

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		exit(1);

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	this->fontString=NULL;

	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);
	this->cm=DefaultColormap(this->display,this->screen);

	wa.bit_gravity=NorthWestGravity;
	wa.override_redirect=override;
	wm_delete_window=XInternAtom(this->display,"WM_DELETE_WINDOW",0);

	this->window=XCreateWindow(this->display,this->rootWindow,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity|CWOverrideRedirect,&wa);
	XSelectInput(this->display,this->window, ButtonPressMask | ButtonReleaseMask | ExposureMask|StructureNotifyMask|LeaveWindowMask|FocusChangeMask);

	XSetWMProtocols(this->display,this->window,&wm_delete_window,1);

	this->gc=XCreateGC(this->display,this->rootWindow,0,NULL);

	this->listeners=XUniqueContext();
	this->LFSTK_setFontString((char*)DEFAULTFONT);

	this->LFSTK_setDecorated(true);
	this->initWindow();
}


