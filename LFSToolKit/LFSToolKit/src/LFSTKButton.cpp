/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:55:09 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKButton.cpp) is part of LFSToolKit.

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

#include "LFSTKButton.h"
#include "lib.h"

LFSTK_buttonClass::~LFSTK_buttonClass()
{
	free(this->label);
	XFreeGC(this->display,this->gc);
	XDestroyWindow(this->display,this->window);
}

LFSTK_buttonClass::LFSTK_buttonClass()
{
}

void LFSTK_buttonClass::drawLabel()
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

void LFSTK_buttonClass::LFSTK_clearWindow()
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

void LFSTK_buttonClass::mouseDown()
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
}

void LFSTK_buttonClass::mouseUp()
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

int LFSTK_buttonClass::LFSTK_getCallbackUD()
{
	return(this->callback.userData);
}

void LFSTK_buttonClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

void LFSTK_buttonClass::mouseEnter()
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

unsigned long LFSTK_buttonClass::setColour(const char *name)
{
	XColor tc,sc;
	XAllocNamedColor(this->display,this->cm,name,&sc,&tc);
	return sc.pixel;
}

listener* LFSTK_buttonClass::LFSTK_getListen(void)
{
	return(&(this->listen));
}

void LFSTK_buttonClass::LFSTK_setCallBack(void (*downcb)(void *,int),void (*releasecb)(void *,int),long ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
}

void LFSTK_buttonClass::LFSTK_setStyle(int s)
{
	this->style=s;
}

void LFSTK_buttonClass::LFSTK_setIgnoreCB(bool ignore)
{
	this->callback.ignoreCallback=ignore;
}

bool LFSTK_buttonClass::LFSTK_getIgnoreCB(void)
{
	return(this->callback.ignoreCallback);
}

void LFSTK_buttonClass::LFSTK_setLabelOriention(int o)
{
	this->labelOrientation=o;
}

LFSTK_buttonClass::LFSTK_buttonClass(LFSTK_windowClass* wc,char* label,int x,int y,int w,int h,int gravity,char* colnorm,char* colhi,char* colact)
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
	this->listen.type=1;

	this->wc=wc;
	this->style=EMBOSSEDBUTTON;
	this->LFSTK_setCallBack(NULL,NULL,-1);
	wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}