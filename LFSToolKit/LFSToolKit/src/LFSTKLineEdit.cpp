/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:15 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKLineEdit.cpp) is part of LFSToolKit.

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

#include "LFSTKLineEdit.h"
#include "lib.h"


LFSTK_lineEditClass::~LFSTK_lineEditClass()
{
}

LFSTK_lineEditClass::LFSTK_lineEditClass()
{
	if(this->label!=NULL)
		free(this->label);
	XDestroyWindow(this->display,this->window);
}

void LFSTK_lineEditClass::LFSTK_setFontString(const char *s)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);
	this->font=ftload(this->display,this->screen,s);
}

void LFSTK_lineEditClass::LFSTK_setColourName(int p,char* colour)
{
	XColor tc,sc;
	if(this->colourNames[p].name!=NULL)
		free(this->colourNames[p].name);
	this->colourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->colourNames[p].pixel=sc.pixel;
}

void LFSTK_lineEditClass::initGadget(void)
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

listener* LFSTK_lineEditClass::LFSTK_getListen(void)
{
	return(&(this->listen));
}

LFSTK_lineEditClass::LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;
	XGCValues	gcv;

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

	wa.bit_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->initGadget();
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);

	this->gc=this->wc->gc;
	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=LINEEDITGADGET;

	this->LFSTK_setCallBack(NULL,NULL,(void*)-1);
	this->wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}

void LFSTK_lineEditClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
}

void LFSTK_lineEditClass::LFSTK_setCallBack(void (*downcb)(void *,void*),void (*releasecb)(void *,void*),void* ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
}

void LFSTK_lineEditClass::mouseUp()
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
void LFSTK_lineEditClass::mouseDown()
{
	if(this->callback.pressCallback!=NULL)
		this->callback.pressCallback(this,this->callback.userData);

}

void LFSTK_lineEditClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

void LFSTK_lineEditClass::mouseEnter()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[PRELIGHTCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	this->inWindow=true;
}







