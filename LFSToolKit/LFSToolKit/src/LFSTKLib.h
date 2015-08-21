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

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct args
{
	const char*		name;
	int				type;
	void*			data;
};

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
		LFSTK_lib();
		~LFSTK_lib();
		bool LFSTK_loadVarsFromFile(const char* filepath,args* dataptr);

		const char *LFSTK_getThemeName(void);

		colourStruct	globalWindowColours[MAXCOLOURS];
		colourStruct	globalButtonColours[MAXCOLOURS];
		colourStruct	globalMenuItemColours[MAXCOLOURS];
		bool			globalColoursSet;

		char			*gFontString;
		char			*gFontColourNames[MAXFONTCOLS];
		char			*gMenuItemFontString;
		char			*gMenuItemFontColourNames[MAXCOLOURS];
		char			*gThemePath;

	private:
		args			lfsToolKitGlobals[];

	protected:

};

#endif
