/*
 *
 * ©K. D. Hedger. Fri 21 Aug 12:29:04 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTK_lib.h) is part of LFSToolKit.

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

#ifndef _LFSTK_LIB_
#define _LFSTK_LIB_

#include "LFSTKGlobals.h"

#define	MAXVARS 100
//enum	{TYPEWINDOW=0,TYPEBUTTON,TYPEMENUITEM,TYPEFONTCOLOUR,TYPEMENUITEMFONTCOLOUR,TYPEMENUITEMFONT,TYPEFONT,TYPETHEME,TYPEWMFRAME};
enum	{TYPEWINDOW=0,TYPEBUTTON,TYPEMENUITEM,TYPEFONTCOLOUR,TYPEMENUITEMFONTCOLOUR,TYPEMENUITEMFONT,TYPEFONT};

/**
 *
 * \brief Library class for LFSToolKit.
 *
 * Library class for LFSToolKit.
 *
 */
class LFSTK_lib
{
	public:
		LFSTK_lib(bool loadvars);
		~LFSTK_lib();
		bool LFSTK_loadVarsFromFile(const char* filepath,args* dataptr);
		void LFSTK_saveVarsToFile(const char* filepath,const args* dataptr);
		const args *LFSTK_getTKArgs(void);
		const char *LFSTK_getGlobalString(int state,int type);
		void LFSTK_setGlobalString(int state,int type,const char *str);

		const char* bestFontColour(long pixel);
		int LFSTK_getTextwidth(Display *disp,XftFont *font,const char *str);
		fontStruct* LFSTK_loadFont(Display *disp,int scr,const char *name);
		static bool LFSTK_gadgetEvent(void *self,XEvent *e,int type);
	private:
		args	*lfsToolKitGlobals;
//window stirngs
		char	*globalWindowColours[MAXCOLOURS];
		char	*globalButtonColours[MAXCOLOURS];
		char	*globalMenuItemColours[MAXCOLOURS];
		char	*globalFontString;

//gadget strings
		char	*globalFontColourNames[MAXCOLOURS];
		char	*globalMenuItemFontString;
		char	*globalMenuItemFontColourNames[MAXCOLOURS];

//theme strings
//		char	*globalFrameColours[MAXFRAMECOLOURS];
//		char	*globalThemePath;

};
#endif
