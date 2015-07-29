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
	free(this->label);
	XFreeGC(this->display,this->gc);
	XDestroyWindow(this->display,this->window);
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

void LFSTK_menuButtonClass::drawLabel()
{
	ftDrawString_Utf8(this->wc,this->window,(this->w/2)-(ftTextWidth_Utf8(this->wc,this->label)/2),(this->h/2)+((this->wc->font->ascent-2)/2),this->label);
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

	LFSTK_buttonClass	*bc;
	LFSTK_windowClass *wc=new LFSTK_windowClass(100,this->y+h+1,500,this->menuCount*this->h,"rgb:00/00/00","rgb:80/80/80");
	wc->LFSTK_setDecorated(true);
	wc->LFSTK_clearWindow();
	XMapWindow(wc->display,wc->window);
	int sy=0;
	int sx=0;
	for(int j=0;j<this->menuCount;j++)
		{
			printf("##%s##\n",this->menus[j].label);
			bc=new LFSTK_buttonClass(wc,this->menus[j].label,0,sy,100,this->h,0,"rgb:a0/a0/a0","rgb:d0/d0/d0","rgb:80/80/80");
			bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,j+1000);
			bc->LFSTK_setStyle(EMBOSSEDBUTTON);
			XMapWindow(wc->display,bc->window);
			bc->LFSTK_clearWindow();
			sy+=this->h;
		}

	XEvent event;
	while (true)
		{
			XNextEvent(wc->display,&event);
			listener *l=wc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);
			switch(event.type)
				{
				case ClientMessage:
					//if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
					//	keep_running = 0;
					break;

				case Expose:
					wc->LFSTK_clearWindow();
				//	bc->LFSTK_clearWindow();
					//bc1->LFSTK_clearWindow();
					break;
				case ConfigureNotify:
					wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
					wc->LFSTK_clearWindow();

				default:
					break;
				}


		}

}

void LFSTK_menuButtonClass::mouseUp()
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->mouseEnter();
			if(this->callback.releaseCallback!=NULL)
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
	this->LFSTK_setCallBack(NULL,NULL,-1);
	wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}