/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:54:59 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKBUTTON_
#define _LFSTKBUTTON_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "LFSTKGlobals.h"
#include "LFSTKWindow.h"
#include "LFSTKGadget.h"

/**
 *
 * \brief Button class for LFSToolKit.
 *
 * Basic button class for LFSToolKit.
 *
 */

class LFSTK_buttonClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_buttonClass();
		~LFSTK_buttonClass();
		LFSTK_buttonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity);

		void *LFSTK_getCallbackUD(void);
		void LFSTK_setStyle(int s);
		void LFSTK_setIgnoreCB(bool ignore);
		bool LFSTK_getIgnoreCB(void);
		void LFSTK_setLabelOriention(int o);
		char *LFSTK_getLabel(void);

		void LFSTK_clearWindow(void);		
		bool mouseEnter(XButtonEvent *e);
		bool mouseExit(XButtonEvent *e);
		bool mouseDown(XButtonEvent *e);
		bool mouseUp(XButtonEvent *e);

		virtual void drawLabel(int p);
		virtual void LFSTK_setColoursFromGlobals(void);

	protected:
		unsigned long setColour(const char *name);

		int					style;
		int					labelOrientation;
};

#endif
