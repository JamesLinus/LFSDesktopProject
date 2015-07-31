/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:55:09 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKMenuButton.cpp) is part of LFSToolKit.

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

#include <X11/Xlib.h>

#include "LFSTKMenuButton.h"
#include "lib.h"

LFSTK_menuButtonClass::~LFSTK_menuButtonClass()
{
	delete this->menus;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

void LFSTK_menuButtonClass::drawLabel()
{
	switch(this->labelOrientation)
		{
			case LEFT:
				ftDrawString_Utf8(this->wc,this->window,2,(this->h/2)+((this->wc->font->ascent-2)/2),this->label);
				break;
			case CENTRE:
				ftDrawString_Utf8(this->wc,this->window,(this->w/2)-(ftTextWidth_Utf8(this->wc,this->label)/2),(this->h/2)+((this->wc->font->ascent-2)/2),this->label);
				break;
		}
}

void LFSTK_menuButtonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->normalColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->style==EMBOSSEDBUTTON)
		{
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
			XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
			XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
		}
	this->drawLabel();
}

void LFSTK_menuButtonClass::mouseDown()
{
	LFSTK_buttonClass	*bc;
	geometryStruct		*g;
	LFSTK_windowClass	*wc;
	int					maxwid=0;
	XEvent				event;
	bool				run=true;

	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->activeColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->style==EMBOSSEDBUTTON)
		{
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
			XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
			XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
		}

	this->drawLabel();

	if(this->callback.pressCallback!=NULL)
		this->callback.pressCallback(this,this->callback.userData);

	for(int j=0;j<this->menuCount;j++)
		{
			if(ftTextWidth_Utf8(this->wc,this->menus[j].label)>maxwid)
				maxwid=ftTextWidth_Utf8(this->wc,this->menus[j].label);
		}
	maxwid+=4;
	g=this->wc->LFSTK_getGeom();
	wc=new LFSTK_windowClass(this->x+g->x,this->y+g->y+this->h,maxwid,this->menuCount*this->h,true,"rgb:00/00/00","rgb:80/80/80");
	wc->LFSTK_clearWindow();
	XMapWindow(wc->display,wc->window);
	delete g;

	int sy=0;
	for(int j=0;j<this->menuCount;j++)
		{
			bc=new LFSTK_buttonClass(wc,this->menus[j].label,0,sy,maxwid,this->h,0,"rgb:a0/a0/a0","rgb:d0/d0/d0","rgb:80/80/80");
			this->menus[j].bc=bc;
			bc->LFSTK_setLabelOriention(LEFT);
			bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,this->menus[j].userData);
			bc->LFSTK_setStyle(FLATBUTTON);
			XMapWindow(wc->display,bc->window);
			bc->LFSTK_clearWindow();
			sy+=this->h;
		}

	while (run==true)
		{
			XNextEvent(wc->display,&event);
			listener *l=wc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			switch(event.type)
				{
				case LeaveNotify:
					if(event.xany.window==wc->window)
						run=false;
					break;

				case Expose:
					wc->LFSTK_clearWindow();
					break;

				default:
					break;
				}
		}

	for(int j=0;j<this->menuCount;j++)
		delete this->menus[j].bc;
	delete wc;
}

void LFSTK_menuButtonClass::mouseUp()
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->mouseEnter();
			if((this->callback.releaseCallback!=NULL) && ((this->callback.ignoreCallback==false)))
				this->callback.releaseCallback(this,this->callback.userData);
		}
}

void LFSTK_menuButtonClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

void LFSTK_menuButtonClass::mouseEnter()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->highlightColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->style==EMBOSSEDBUTTON)
		{
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
			XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
			XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
		}

	this->inWindow=true;
	this->drawLabel();
}

void LFSTK_menuButtonClass::LFSTK_addMenus(menuItemStruct* menus,int cnt)
{
	this->menus=menus;
	this->menuCount=cnt;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass(LFSTK_windowClass* wc,char* label,int x,int y,int w,int h,int gravity,char* colnorm,char* colhi,char* colact)
{
	XSetWindowAttributes	wa;
	XGCValues	gcv;

	this->display=wc->display;
	this->parent=wc->window;

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->screen=wc->screen;
	this->visual=wc->visual;
	this->rootWindow=wc->rootWindow;
	this->cm=wc->cm;

	this->label=strdup(label);

	wa.bit_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window,SubstructureRedirectMask|Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->normalColour=this->setColour(colnorm);
	this->highlightColour=this->setColour(colhi);
	this->activeColour=this->setColour(colact);

	this->gc=XCreateGC(this->display,this->parent,0,NULL);

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=2;

	this->wc=wc;
	this->style=EMBOSSEDBUTTON;
	this->LFSTK_setLabelOriention(1000);
	this->LFSTK_setCallBack(NULL,NULL,(void*)-1);
	wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}