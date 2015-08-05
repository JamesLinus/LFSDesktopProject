/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:06 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKLineEdit.h) is part of LFSToolKit.

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


#ifndef _LFSTKLINEEDIT_
#define _LFSTKLINEEDIT_

#include "LFSTKGlobals.h"
#include "LFSTKWindow.h"

class LFSTK_lineEditClass
{
	public:
		LFSTK_lineEditClass();
		~LFSTK_lineEditClass();
		LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity);

		void LFSTK_clearWindow();
		listener* LFSTK_getListen(void);
		void LFSTK_setFontString(const char *s);
		void initGadget(void);
		void LFSTK_setColourName(int p,char* colour);
		void LFSTK_setCallBack(void (*downcb)(void *,void*),void (*releasecb)(void *,void*),void* ud);
		void mouseUp();
		void mouseDown();
		void mouseExit();
		void mouseEnter();




		Display				*display;
		Window				parent;
		Window				window;
		GC					gc;

		int					screen;
		Visual				*visual;
		Window				rootWindow;		
		Colormap			cm;

	private:
		int					x;
		int					y;
		int					w;
		int					h;
		int					blackColour;
		int					whiteColour;

		char				*label;
		listener			listen;
		buttonCB			callback;

		LFSTK_windowClass	*wc;
		char				*fontString;
		fontStruct			*font;
		char				*fontColourNames[MAXFONTCOLS];
		colourStruct		colourNames[MAXCOLOURS];
		bool				inWindow;
};

#endif
