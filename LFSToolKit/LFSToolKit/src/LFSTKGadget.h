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

class LFSTK_gadgetClass
{
	public:
		LFSTK_gadgetClass();
		~LFSTK_gadgetClass();

		virtual void LFSTK_clearWindow();
		virtual void mouseUp();
		virtual void mouseDown();
		virtual void mouseExit();
		virtual void mouseEnter();
		virtual void lostFocus(XEvent *e);
		virtual void keyRelease(XKeyEvent *e);

		void LFSTK_setCallBack(void (*downcb)(void *,void*),void (*releasecb)(void *,void*),void* ud);

		void LFSTK_setCommon(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity);

		void LFSTK_setColourName(int p,char* colour);
		void LFSTK_setFontString(const char *s);
		void LFSTK_setFontColourName(int p,char* colour);
		Window LFSTK_getWindow(void);

		LFSTK_windowClass	*wc;
		char				*label;

	private:
		void initGadget(void);

	protected:
		listener* LFSTK_getListen(void);
		listener			listen;

		int					x;
		int					y;
		int					w;
		int					h;
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
		char				*fontColourNames[MAXFONTCOLS];
		colourStruct		colourNames[MAXCOLOURS];
		bool				inWindow;
};

#endif
