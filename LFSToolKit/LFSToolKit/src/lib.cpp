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
#include <X11/Xft/Xft.h>

#include "LFSTKButton.h"
#include "LFSTKWindow.h"
#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"

#include "lib.h"
/*
bool gadgetEvent(void *self,XEvent *e,int type)
{
	bool				retval=true;
	LFSTK_gadgetClass	*gadget=NULL;

//printf("---%i---\n",type);
//printf("===type=%i====\n",e->type);
	gadget=static_cast<LFSTK_gadgetClass*>(self);

	switch (e->type)
		{
			case EnterNotify:
				retval=gadget->mouseEnter(&e->xbutton);
				break;
			case LeaveNotify:
				retval=gadget->mouseExit(&e->xbutton);
				break;
			case ButtonRelease:
				retval=gadget->mouseUp(&e->xbutton);
				break;
			case ButtonPress:
				XSetInputFocus(gadget->wc->display,e->xbutton.window,RevertToNone,CurrentTime);
				retval=gadget->mouseDown(&e->xbutton);
				break;

			case MotionNotify:
				break;
			case Expose:
//			 		printf("xpose from lib\n");
				gadget->LFSTK_clearWindow();
				break;

			case FocusIn:
				//printf("focus in libev\n");
				retval=gadget->gotFocus(e);
				break;
			case FocusOut:
				//printf("focus out libev\n");
				retval=gadget->lostFocus(e);
				break;

			case KeyRelease:
				retval=gadget->keyRelease(&e->xkey);
				//printf("KeyRelease\n");
				break;
	
			case ConfigureNotify:
//				printf("conf>>>>>>>>>\n");
				break;
			case GravityNotify:
//				printf("grav>>>>>>>>>>>\n");
//				retval=true;
				break;
			case ResizeRequest:
				//XCheckTypedWindowEvent
//				printf("resize\n");
				break;
			case ClientMessage:
//				printf("ClientMessage\n");
				break;
				
		}
//	printf("%i\n",ud);
	if(retval==false)
		XSendEvent(gadget->wc->display,gadget->wc->window,False,0L,e);
	return(retval);
}
*/