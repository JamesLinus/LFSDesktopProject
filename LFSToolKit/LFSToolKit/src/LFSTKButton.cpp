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
}

LFSTK_buttonClass::LFSTK_buttonClass()
{
}

/**
* Draw label.
* \param p Button state.
* \note State FONTNORMALCOL=0,FONTHILITECOL=1,FONTACTIVECOL=2.
*/
void LFSTK_buttonClass::drawLabel(int p)
{
	switch(this->labelOrientation)
		{
			case LEFT:
				drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),2,(this->h/2)+((this->font->ascent-2)/2),this->fontColourNames[p],this->label);
				break;
			default://centre
				drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),(this->w/2)-(ftTextWidth_Utf8(this->wc,this->label)/2),(this->h/2)+((this->font->ascent-2)/2),this->fontColourNames[p],this->label);
				break;
		}
}

/**
* Set colours from global colours if loaded.
*/
void LFSTK_buttonClass::LFSTK_setColoursFromGlobals(void)
{
	if(globalColoursSet==true)
		{
			for(int j=0;j<MAXCOLOURS;j++)
				this->LFSTK_setColourName(j,globalButtonColours[j].name);
			for(int j=0;j<MAXFONTCOLS;j++)
				this->LFSTK_setFontColourName(j,gFontColourNames[j]);
		}
}

void LFSTK_buttonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
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
	this->drawLabel(FONTNORMALCOL);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_buttonClass::mouseDown(XButtonEvent *e)
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[ACTIVECOLOUR].pixel);
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

	this->drawLabel(FONTACTIVECOL);

	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_buttonClass::mouseUp(XButtonEvent *e)
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->mouseEnter(e);
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

/**
* Return callback user data.
* \note Returns a void* so must be cast.
* \note return value must not be freed.
*/
void* LFSTK_buttonClass::LFSTK_getCallbackUD()
{
	return(this->callback.userData);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_buttonClass::mouseExit(XButtonEvent *e)
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_buttonClass::mouseEnter(XButtonEvent *e)
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[PRELIGHTCOLOUR].pixel);
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
	this->drawLabel(FONTHILITECOL);
	return(true);
}

/**
* Set clour from name.
* \param name.
* \return Pixel value.
*/
unsigned long LFSTK_buttonClass::setColour(const char *name)
{
	XColor tc,sc;
	XAllocNamedColor(this->display,this->cm,name,&sc,&tc);
	return sc.pixel;
}

/**
* Set buton style.
* \param s.
* \note s=FLATBUTTON=0,EMBOSSEDBUTTON=1.
*/
void LFSTK_buttonClass::LFSTK_setStyle(int s)
{
	this->style=s;
}

/**
* Ignore callbacks.
* \param ignore.
*/
void LFSTK_buttonClass::LFSTK_setIgnoreCB(bool ignore)
{
	this->callback.ignoreCallback=ignore;
}

/**
* Get the current ignore callbacks state.
*/
bool LFSTK_buttonClass::LFSTK_getIgnoreCB(void)
{
	return(this->callback.ignoreCallback);
}

/**
* Set label orientation.
* \param o.
* \note o=LEFT=0,CENTRE=1,RIGHT=2.
*/
void LFSTK_buttonClass::LFSTK_setLabelOriention(int o)
{
	this->labelOrientation=o;
}

/**
* Get the label.
* \return char* Returned string must not be freed.
*/
char* LFSTK_buttonClass::LFSTK_getLabel(void)
{
	return(this->label);
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_buttonClass::LFSTK_buttonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=EMBOSSEDBUTTON;
	this->labelOrientation=CENTRE;
	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=BUTTONGADGET;

	this->wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}
