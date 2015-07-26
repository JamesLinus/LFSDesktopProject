/*
 *
 * ©K. D. Hedger. Fri 24 Jul 20:38:19 BST 2015 kdhedger68713@gmail.com

 * This file (lib.cpp) is part of LFSToolKit.

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
#include <stdio.h>

#include "LFSTKlib.h"
#include "LFSTKButton.h"

void gadgetEvent(void *self,XEvent *e)
{
int ud=-1;

	switch (e->type)
		{
		case EnterNotify:
		ud=reinterpret_cast<LFSTK_buttonClass*>(self)->listen.userData;
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseEnter();
			break;
		case LeaveNotify:
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseExit();
		ud=reinterpret_cast<LFSTK_buttonClass*>(self)->listen.userData;
			break;
		case ButtonRelease:
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseUp();
		fprintf(stderr,"1111111111111111\n");
			break;
		case MotionNotify:
		fprintf(stderr,"2222222222222222222\n");
			break;
		case ButtonPress:
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseDown();
		fprintf(stderr,"XXXXXXXXXXXXXXX\n");
			break;
		}
printf("00000000000000000000 -- %i\n",ud);
}
