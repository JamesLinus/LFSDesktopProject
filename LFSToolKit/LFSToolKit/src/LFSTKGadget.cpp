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

LFSTK_gadgetClass::~LFSTK_gadgetClass()
{
	if(this->label!=NULL)
		free(this->label);
	XDestroyWindow(this->display,this->window);
}

LFSTK_gadgetClass::LFSTK_gadgetClass()
{
	pad=2;
}

/**
* Set the colour name for font.
* \param p Font state.
* \param colour Colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_gadgetClass::LFSTK_setFontColourName(int p,const char* colour)
{
	this->fontColourNames[p]=strdup(colour);
}

/**
* Get the gadget's window ID.
* \return Returns windows XID.
*/
Window LFSTK_gadgetClass::LFSTK_getWindow(void)
{
	return(this->window);
}

/**
* Set default font string.
* \param s Font string.
* \note eg:
* \note "sans-serif:size=8".
*/
void LFSTK_gadgetClass::LFSTK_setFontString(const char *s)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);
	this->font=this->wc->globalLib->LFSTK_loadFont(this->display,this->screen,s);
}

/**
* Set the colour name for gadget.
* \param p Gadget state.
* \param colour Colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_gadgetClass::LFSTK_setColourName(int p,const char* colour)
{
	XColor tc,sc;
	if(this->colourNames[p].name!=NULL)
		free(this->colourNames[p].name);
	this->colourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->colourNames[p].pixel=sc.pixel;
}

/**
* Get the colour name for gadget.
* \param p Gadget state.
* \return colour Const colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note Donot free returned value.
*/
const char* LFSTK_gadgetClass::LFSTK_getColourName(int p)
{
	return(this->colourNames[p].name);
}
//needs re vamping
void LFSTK_gadgetClass::initGadget(void)
{
	for(int j=0;j<MAXCOLOURS;j++)
		this->fontColourNames[j]=NULL;

	for(int j=0;j<MAXCOLOURS;j++)
		this->colourNames[j].name=NULL;

	this->fontString=NULL;

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));

	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->LFSTK_setActive(true);
	this->LFSTK_setLabelAutoColour(false);
	this->style=EMBOSSEDBUTTON;
	this->labelOrientation=CENTRE;
	this->inWindow=false;
	this->autoLabelColour=this->wc->autoLabelColour;
	this->labelOffset=0;
}

/**
* Get the listener for gadget.
* \return listener.
*/
listener* LFSTK_gadgetClass::getListen(void)
{
	return(&(this->listen));
}

/**
* Do common setup for gadget.
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
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

/**
* Set callback for widget.
* \param downcb Mouse down callback.
* \param releasecb Mouse up callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
}

/**
* Set gadget active state.
* \param active Gadget active or not.
* \note Setting to false deactivates widget;
*/
void LFSTK_gadgetClass::LFSTK_setActive(bool active)
{
	this->isActive=active;
}

/**
* Set gadget auto colour for label.
* \param setauto Auto label colour active or not.
* \note Setting to true overrides the label colour;
* \note and tries to set it to black or white depending on background.
* \note defaults to false; 
*/
void LFSTK_gadgetClass::LFSTK_setLabelAutoColour(bool setauto)
{
	this->autoLabelColour=setauto;
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_gadgetClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	if(this->isActive==true)
		XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
	else
		XSetForeground(this->display,this->gc,this->colourNames[INACTIVECOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseUp(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

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
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseDown(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseExit(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->inWindow=false;
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseEnter(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[PRELIGHTCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	this->inWindow=true;
	return(true);
}

void LFSTK_gadgetClass::LFSTK_resizeWindow(int w,int h)
{
	this->w=w;
	this->h=h;
	XResizeWindow(this->display,this->window,w,h);
	this->LFSTK_clearWindow();
}

/**
* Key release callback.
* \param e XKeyEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::keyRelease(XKeyEvent *e)
{
	return(true);
}

/**
* Lost focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::lostFocus(XEvent *e)
{
	return(true);
}

/**
* Got focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::gotFocus(XEvent *e)
{
	return(true);
}

/**
* Draw string.
* \param font Font to use..
* \param x Xpos.
* \param y Ypos.
* \param col Colour.
* \param s String to draw.
*/
void LFSTK_gadgetClass::LFSTK_drawString(XftFont* font,int x,int y,const char *col,const char *s)
{
	XftColor colour;

	if (!XftColorAllocName(this->wc->display,this->wc->visual,this->wc->cm,col,&colour))
		return;

	XftDrawChange(this->wc->draw,this->window);
	XftDrawStringUtf8(this->wc->draw,&colour,font,x,y,(XftChar8 *)s,strlen(s));
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_gadgetClass::LFSTK_drawLabel(int state)
{
	const char *holdcol=this->fontColourNames[state];

	if(this->autoLabelColour==true)
		holdcol=this->wc->globalLib->bestFontColour(this->colourNames[state].pixel);

	switch(this->labelOrientation)
		{
			case LEFT:
				this->LFSTK_drawString((XftFont*)(this->font->data),2+this->labelOffset,(this->h/2)+((this->wc->font->ascent-2)/2),holdcol,this->label);
				break;
			case RIGHT:
				this->LFSTK_drawString((XftFont*)(this->font->data),this->w-2-(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->label))+this->labelOffset,(this->h/2)+((this->wc->font->ascent-2)/2),holdcol,this->label);
				break;
			default://centre
				this->LFSTK_drawString((XftFont*)(this->font->data),(this->w/2)-(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)wc->font->data,this->label)/2)+this->labelOffset,(this->h/2)+((this->wc->font->ascent-2)/2),holdcol,this->label);
				break;
		}
}

void LFSTK_gadgetClass::LFSTK_setLabel(const char *newlabel)
{
	if(this->label!=NULL)
		free(this->label);
	this->label=strdup(newlabel);
	this->LFSTK_clearWindow();
}

/**
* Get the label.
* \return char* Returned string must not be freed.
*/
const char *LFSTK_gadgetClass::LFSTK_getLabel(void)
{
	return(this->label);
}

/**
* Set label orientation.
* \param o.
* \note o=LEFT=0,CENTRE=1,RIGHT=2.
*/
void LFSTK_gadgetClass::LFSTK_setLabelOriention(int orient)
{
	this->labelOrientation=orient;
}

/**
* Get gadget geometry.
* \return geometry structure.
* \note Caller should free structure after use.
*/
geometryStruct *LFSTK_gadgetClass::LFSTK_getGeom(void)
{
	geometryStruct		*g=new geometryStruct;
	XWindowAttributes	xwa;
	int					x,y;
	Window				child;

	XTranslateCoordinates(this->display,this->window,this->rootWindow,0,0,&x,&y,&child );
	XGetWindowAttributes(this->display,this->window,&xwa);

	g->x=x;
	g->y=y;
	g->w=xwa.width;
	g->h=xwa.height;

	return(g);
}

/**
* Reload global colours from main window.
*/
void LFSTK_gadgetClass::LFSTK_reloadColours(void)
{
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));
	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->autoLabelColour=this->wc->autoLabelColour;
}

/**
* Draw indicator.
* \param g Geometry Struture.
* \param state Button state.
* \param indic Indicator type CHECK=0,RADIO=1,PICTURE=2,NOINDICATOR=3.
*/
void LFSTK_gadgetClass::drawIndicator(geometryStruct* g,int state,indicatorType indic)
{

	switch(indic)
		{
			case CHECK:
				XSetForeground(this->display,this->gc,this->blackColour);
				XDrawLine(this->display,this->window,this->gc,g->x,g->y+1,g->x+g->w-1,g->y+g->h);
				XDrawLine(this->display,this->window,this->gc,g->x+1,g->y+g->h,g->x+g->w,g->y+1);

				XSetForeground(this->display,this->gc,this->whiteColour);
				XDrawLine(this->display,this->window,this->gc,g->x,g->y,g->x+g->w,g->y+g->h);
				XDrawLine(this->display,this->window,this->gc,g->x,g->y+g->h,g->x+g->w,g->y);
		}
}

/**
* Draw box.
* \param g Geometry Struture.
* \param state Button state.
*/
void LFSTK_gadgetClass::drawBox(geometryStruct* g,int state,bevelType bevel)
{
	int tlcolour;
	int brcolour;
	int fillcolour;

	switch(bevel)
		{
			case BEVELIN:
				tlcolour=this->blackColour;
				brcolour=this->whiteColour;
				break;
			case BEVELOUT:
				tlcolour=this->whiteColour;
				brcolour=this->blackColour;
				break;
			case BEVELNONE:
				tlcolour=this->colourNames[state].pixel;
				brcolour=this->colourNames[state].pixel;
				break;
		}

	fillcolour=this->colourNames[state].pixel;

	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[state].pixel);
	XFillRectangle(this->display,this->window,this->gc,g->x,g->y,g->w,g->h);

//draw bootom rite
	XSetForeground(this->display,this->gc,brcolour);
	XDrawLine(this->display,this->window,this->gc,g->x,g->y+g->h,g->x+g->w,g->y+g->h);
	XDrawLine(this->display,this->window,this->gc,g->x+g->w,g->y+g->h,g->x+g->w,g->y);

//draw top left
	XSetForeground(this->display,this->gc,tlcolour);
	XDrawLine(this->display,this->window,this->gc,g->x,g->y,g->x,g->y+g->h);
	XDrawLine(this->display,this->window,this->gc,g->x,g->y,g->x+g->w,g->y);
}

