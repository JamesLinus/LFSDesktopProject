/*
 *
 * ©K. D. Hedger. Fri 28 Aug 19:17:00 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKLabel.cpp) is part of LFSToolKit.

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

#include "LFSTKLabel.h"

LFSTK_labelClass::~LFSTK_labelClass()
{
}

LFSTK_labelClass::LFSTK_labelClass()
{
}

void LFSTK_labelClass::LFSTK_clearWindow(void)
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	if(this->isActive==true)
		XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->isActive==true)
		this->drawLabel(NORMALCOLOUR);
	else
		this->drawLabel(INACTIVECOLOUR);
}

/**
* Main Label constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_labelClass::LFSTK_labelClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=FLATBUTTON;
	this->listen.function=NULL;
	this->listen.pointer=NULL;
	this->listen.type=LABELGADGET;
	this->LFSTK_setActive(true);
	this->LFSTK_setFontColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONTCOLOUR));
	this->LFSTK_setColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_labelClass::drawLabel(int p)
{
	const char *holdcol=this->fontColourNames[p];

	if(this->autoLabelColour==true)
		holdcol=this->wc->globalLib->bestFontColour(this->colourNames[p].pixel);
//			printf("----%s-----\n",this->label);

	switch(this->labelOrientation)
		{
			case LEFT:
				this->LFSTK_drawString((XftFont*)(this->font->data),2,(this->h/2)+((this->wc->font->ascent-2)/2),holdcol,this->label);
				break;
			default://centre
				this->LFSTK_drawString((XftFont*)(this->font->data),(this->w/2)-(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)wc->font->data,this->label)/2),(this->h/2)+((this->wc->font->ascent-2)/2),holdcol,this->label);
				break;
		}
}
