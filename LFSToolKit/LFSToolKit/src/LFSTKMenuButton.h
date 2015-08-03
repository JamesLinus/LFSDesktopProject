/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:54:59 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKMenuButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKMENUBUTTON_
#define _LFSTKMENUBUTTON_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "LFSTKGlobals.h"
#include "LFSTKWindow.h"
#include "LFSTKButton.h"

struct menuItemStruct
{
	char				*label;
	void				*userData;
	LFSTK_buttonClass	*bc;
};


class LFSTK_menuButtonClass : public LFSTK_buttonClass
{
	public:
		LFSTK_menuButtonClass();
		~LFSTK_menuButtonClass();
		LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,char* label,int x,int y,int w,int h,int gravity);

		void LFSTK_clearWindow(void);		
		void mouseEnter();
		void mouseExit();
		void mouseDown();
		void mouseUp();
		void LFSTK_addMenus(menuItemStruct* menus,int cnt);
		void drawLabel(int p);

		void LFSTK_setMenuItemColours(int p,char* colour);
		void LFSTK_setColoursFromGlobals(void);

	private:
		void initMenuButton(void);
		menuItemStruct		*menus;
		void LFSTK_setMenuItemFontString(char *s);
		void LFSTK_setMenuItemsFontColourName(int p,char* colour);
		
		int					menuCount;
		int					x;
		int					y;
		int					w;
		int					h;
		int					blackColour;
		int					whiteColour;
		bool				inWindow;
		colourStruct		menuItemColours[MAXCOLOURS];
		char				*menuItemFontColourNames[MAXFONTCOLS];
		char				*menuItemFontString;
		fontStruct			*menuItemfont;
};

#endif
