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

#include "lib.h"

void gadgetEvent(void *self,XEvent *e)
{
	switch (e->type)
		{
		case ButtonRelease:
		fprintf(stderr,"1111111111111111\n");
			break;
		case MotionNotify:
		fprintf(stderr,"2222222222222222222\n");
			break;
		case ButtonPress:
		fprintf(stderr,"XXXXXXXXXXXXXXX\n");
			break;
		}
}
