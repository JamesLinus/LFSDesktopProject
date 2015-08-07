/*
 *
 * ©K. D. Hedger. Wed  5 Aug 13:53:44 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKGadget.cpp) is part of LFSToolKit.

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

#include "LFSTKGadget.h"
#include "lib.h"


LFSTK_gadgetClass::~LFSTK_gadgetClass()
{
	if(this->label!=NULL)
		free(this->label);
	XDestroyWindow(this->display,this->window);
}

LFSTK_gadgetClass::LFSTK_gadgetClass()
{
}

void LFSTK_gadgetClass::LFSTK_setFontColourName(int p,char* colour)
{
	this->fontColourNames[p]=strdup(colour);
}

Window LFSTK_gadgetClass::LFSTK_getWindow(void)
{
	return(this->window);
}

void LFSTK_gadgetClass::LFSTK_setFontString(const char *s)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);
	this->font=ftload(this->display,this->screen,s);
}

void LFSTK_gadgetClass::LFSTK_setColourName(int p,char* colour)
{
	XColor tc,sc;
	if(this->colourNames[p].name!=NULL)
		free(this->colourNames[p].name);
	this->colourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->colourNames[p].pixel=sc.pixel;
}

void LFSTK_gadgetClass::initGadget(void)
{
	for(int j=0;j<MAXFONTCOLS;j++)
		this->fontColourNames[j]=NULL;

	for(int j=0;j<MAXCOLOURS;j++)
		this->colourNames[j].name=NULL;

	this->fontString=NULL;

	for(int j=0;j<MAXFONTCOLS;j++)
		this->fontColourNames[j]=strdup(this->wc->fontColourNames[j]);

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,this->wc->colourNames[j].name);

	this->LFSTK_setFontString(wc->fontString);
}

//
listener* LFSTK_gadgetClass::LFSTK_getListen(void)
{
	return(&(this->listen));
}

void LFSTK_gadgetClass::LFSTK_setCommon(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	this->wc=parentwc;
	this->display=this->wc->display;
	this->parent=this->wc->window;

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->screen=this->wc->screen;
	this->visual=this->wc->visual;
	this->rootWindow=this->wc->rootWindow;
	this->cm=this->wc->cm;

	this->label=strdup(label);

	this->gc=this->wc->gc;
	this->initGadget();
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);

	this->LFSTK_setCallBack(NULL,NULL,(void*)-1);
}

void LFSTK_gadgetClass::LFSTK_setCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
}

void LFSTK_gadgetClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
}

bool LFSTK_gadgetClass::mouseUp()
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->mouseEnter();
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

bool LFSTK_gadgetClass::mouseDown()
{
	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
}

bool LFSTK_gadgetClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

bool LFSTK_gadgetClass::mouseEnter()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[PRELIGHTCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	this->inWindow=true;
}

bool LFSTK_gadgetClass::keyRelease(XKeyEvent *e)
{
	return(true);
}

bool LFSTK_gadgetClass::lostFocus(XEvent *e)
{
	return(true);
}





