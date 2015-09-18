/*
 *
 * ©K. D. Hedger. Wed  5 Aug 13:53:33 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKGadget.h) is part of LFSToolKit.

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


#ifndef _LFSTKGADGET_
#define _LFSTKGADGET_

#include "LFSTKGlobals.h"
#include "LFSTKWindow.h"

enum bevelType {BEVELIN=0,BEVELOUT,BEVELNONE};
enum indicatorType {CHECK=0,RADIO,PICTURE,NOINDICATOR};

/**
 *
 * \brief Base class for LFSToolKit gadgets.
 *
 * Base class for LFSToolKit gadgets.
 *
 */
class LFSTK_gadgetClass
{
	public:
		LFSTK_gadgetClass();
		virtual ~LFSTK_gadgetClass();

		virtual void LFSTK_clearWindow(void);
		virtual void LFSTK_resizeWindow(int w,int h);

		virtual bool mouseUp(XButtonEvent *e);
		virtual bool mouseDown(XButtonEvent *e);
		virtual bool mouseExit(XButtonEvent *e);
		virtual bool mouseEnter(XButtonEvent *e);
		virtual bool keyRelease(XKeyEvent *e);
		virtual bool gotFocus(XEvent *e);
		virtual bool lostFocus(XEvent *e);

		Window LFSTK_getWindow(void);
		void LFSTK_setCommon(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity);
		void LFSTK_setCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud);

		void LFSTK_setColourName(int p,const char* colour);
		const char* LFSTK_getColourName(int p);
		void LFSTK_setFontString(const char *s);
		void LFSTK_setFontColourName(int p,const char* colour);
		void LFSTK_reloadColours(void);

		void LFSTK_setActive(bool active);
		void LFSTK_setLabelAutoColour(bool setauto);

		void LFSTK_setLabel(const char *newlabel);
		const char *LFSTK_getLabel(void);
		void LFSTK_setLabelOriention(int orient);
		geometryStruct *LFSTK_getGeom(void);

		LFSTK_windowClass	*wc;

	private:
		void initGadget(void);

	protected:
		listener* LFSTK_getListen(void);
		void LFSTK_drawString(XftFont* font,int x,int y,const char *col,const char *s);
		virtual void drawLabel(int state);
		void drawBox(geometryStruct* g,int state,bevelType bevel);
		void drawIndicator(geometryStruct* g,int state,indicatorType indic=NOINDICATOR);

		listener			listen;

		char				*label;
		int					x;
		int					y;
		int					w;
		int					h;
		int					pad;

		int					blackColour;
		int					whiteColour;

		Window				window;
		Display				*display;
		Window				parent;
		GC					gc;

		int					screen;
		Visual				*visual;
		Window				rootWindow;		
		Colormap			cm;

		buttonCB			callback;

		char				*fontString;
		fontStruct			*font;
		char				*fontColourNames[MAXCOLOURS];
		colourStruct		colourNames[MAXCOLOURS];
		bool				inWindow;
		bool				isActive;
		bool				autoLabelColour;
		int					labelOrientation;
		int					style;
		int					labelOffset;
};

#endif
