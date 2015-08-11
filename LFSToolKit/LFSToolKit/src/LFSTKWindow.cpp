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
#include <X11/Xatom.h>

#include <string.h>

#include "LFSTKWindow.h"
#include "lib.h"

#define _NET_WM_STATE_REMOVE	0
#define _NET_WM_STATE_ADD		1
#define _NET_WM_STATE_TOGGLE	2

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

	if(this->windowName!=NULL)
		free(this->windowName);

	XFreeGC(this->display,this->gc);
	XDeleteContext(this->display,this->window,this->listeners);
	XDestroyWindow(this->display,this->window);
	XCloseDisplay(this->display);
}

LFSTK_windowClass::LFSTK_windowClass()
{
	this->initWindow();
}

void LFSTK_windowClass::LFSTK_loadGlobalColours(const char *path)
{
	loadVarsFromFile(path,lfsToolKitGlobals);
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,globalWindowColours[j].name);
	this->LFSTK_setFontString(gFontString);

	for(int j=0;j<MAXFONTCOLS;j++)
		this->LFSTK_setFontColourName(j,gFontColourNames[j]);
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
	this->font=ftload(this->display,this->screen,s);
}

void LFSTK_windowClass::LFSTK_setDecorated(bool isDecorated)
{
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

void LFSTK_windowClass::LFSTK_setFontColourName(int p,const char *colour)
{
	this->fontColourNames[p]=strdup(colour);
}

void LFSTK_windowClass::LFSTK_setColourName(int p,const char* colour)
{
	XColor tc,sc;

	this->colourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->colourNames[p].pixel=sc.pixel;
}

void LFSTK_windowClass::LFSTK_setSticky(bool set)
{
	XClientMessageEvent	xclient;
	Atom				xa,xa1;

	memset(&xclient,0,sizeof(xclient) );
	xa=XInternAtom(this->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(display,"_NET_WM_STATE_STICKY",False);

	xclient.type=ClientMessage;
	xclient.window=this->window;
	xclient.message_type=xa;
	xclient.format=32;
	if(set==true)
		xclient.data.l[0] =_NET_WM_STATE_ADD;
	else
		xclient.data.l[0] =_NET_WM_STATE_REMOVE;
	xclient.data.l[1] =xa1;
	xclient.data.l[2] = 0;
	XSendEvent(this->display,this->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
}

void LFSTK_windowClass::LFSTK_setKeepAbove(bool set)
{
	XClientMessageEvent	xclient;
	Atom				xa,xa1;

	memset(&xclient,0,sizeof(xclient) );
	xa=XInternAtom(this->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(display,"_NET_WM_STATE_ABOVE",False);

	xclient.type=ClientMessage;
	xclient.window=this->window;
	xclient.message_type=xa;
	xclient.format=32;
	if(set==true)
		xclient.data.l[0] =_NET_WM_STATE_ADD;
	else
		xclient.data.l[0] =_NET_WM_STATE_REMOVE;
	xclient.data.l[1] =xa1;
	xclient.data.l[2] = 0;
	XSendEvent(this->display,this->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
}

LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XClassHint				classHint;
	Atom					xa;
	Atom					xa_prop[3];

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
	this->draw=XftDrawCreate(this->display,this->rootWindow,this->visual,this->cm);

	wa.win_gravity=NorthWestGravity;
	wa.override_redirect=override;
	wm_delete_window=XInternAtom(this->display,"WM_DELETE_WINDOW",0);

	this->window=XCreateWindow(this->display,this->rootWindow,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWOverrideRedirect,&wa);
	XSelectInput(this->display,this->window,StructureNotifyMask|ButtonPressMask | ButtonReleaseMask | ExposureMask|LeaveWindowMask|FocusChangeMask);

	XSetWMProtocols(this->display,this->window,&wm_delete_window,1);

	xa=XInternAtom(this->display,"_NET_WM_ALLOWED_ACTIONS",False);
	xa_prop[0]=XInternAtom(this->display,"_NET_WM_STATE_STICKY",False);
	xa_prop[1]=XInternAtom(this->display,"_NET_WM_STATE_ABOVE",False);
	xa_prop[2]=XInternAtom(this->display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);
	if(xa!=None)
		XChangeProperty(this->display,this->window,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,3);

	this->windowName=strdup(name);
	XStoreName(this->display,this->window,this->windowName);
	classHint.res_name=this->windowName;
	classHint.res_class=(char*)"LFSToolKit";
	XSetClassHint(this->display,this->window,&classHint);

	this->gc=XCreateGC(this->display,this->rootWindow,0,NULL);

	this->listeners=XUniqueContext();
	this->LFSTK_setFontString((char*)DEFAULTFONT);

	this->LFSTK_setDecorated(true);
	this->initWindow();
}


