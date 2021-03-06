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

#include <string>

#include <X11/Xatom.h>

#include "LFSTKGadget.h"

/**
 *
 * \brief Line edit class for LFSToolKit.
 *
 * Basic line editing class for LFSToolKit.
 *
 */
class LFSTK_lineEditClass  : public  LFSTK_gadgetClass
{
	public:
		LFSTK_lineEditClass();
		~LFSTK_lineEditClass();
		LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity);

		void LFSTK_clearWindow(void);
		void LFSTK_resizeWindow(int w,int h);

		const std::string* LFSTK_getBuffer(void);
		void LFSTK_setBuffer(const char *str);

		bool mouseDown(XButtonEvent *e);
		bool mouseEnter(XButtonEvent *e);
		bool keyRelease(XKeyEvent *e);
		bool lostFocus(XEvent *e);
		bool gotFocus(XEvent *e);
		void LFSTK_setFocus(void);

	private:
		void drawLabel(void);
		void getClip(void);

		Atom XA_CLIPBOARD;
		Atom XA_COMPOUND_TEXT;
		Atom XA_UTF8_STRING;
		Atom XA_TARGETS;

		std::string	buffer;
		unsigned	cursorPos;
		bool		isFocused;
};

#endif
