#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ "$0"  -I/tmp/AAA/usr/include -L/tmp/AAA/usr/lib $(pkg-config --cflags --libs x11 xft) -llfstoolkit||exit 1
./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"

bool				mainloop=true;
int					width=200;
int					hite=60;
LFSTK_lineEditClass	*le;

bool callback(void *p,void* ud)
{
	printf("callback = %p - %s\n",ud,le->LFSTK_getBuffer()->c_str());
	if((long)ud==1)
		{
			mainloop=false;
			return(false);
		}
	else
		return(true);
}

int main(int argc, char **argv)
{
	printf("Hello World!\n");
	LFSTK_windowClass	*mainwind=new LFSTK_windowClass(0,0,width,hite,false);
	XEvent				event;
	LFSTK_buttonClass	*bc,*bc1;
	bool				firstrun=true;

	le=new LFSTK_lineEditClass(mainwind,"",2,4,width-4,24,0);
	XMapWindow(mainwind->display,le->LFSTK_getWindow());

	bc=new LFSTK_buttonClass(mainwind,"Apply",4,24+4+4,75,24,SouthEastGravity);
	bc->LFSTK_setCallBack(NULL,callback,(void*)1);
	XMapWindow(mainwind->display,bc->LFSTK_getWindow());

	bc1=new LFSTK_buttonClass(mainwind,"Cancel",width-4-75,24+4+4,75,24,SouthWestGravity);
	bc1->LFSTK_setCallBack(NULL,callback,(void*)2);
	XMapWindow(mainwind->display,bc1->LFSTK_getWindow());

	XMapWindow(mainwind->display,mainwind->window);

	mainloop=true;
	while(mainloop==true)
		{
			listener *l=mainwind->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(mainwind->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						mainwind->LFSTK_clearWindow();
						if(firstrun==true)
							{
								firstrun=false;
								le->LFSTK_setFocus();
							}
						break;
					case ConfigureNotify:
						mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						mainwind->LFSTK_clearWindow();
						break;
					case ClientMessage:
						if (event.xclient.message_type == XInternAtom(mainwind->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(mainwind->display, "WM_DELETE_WINDOW", 1))
							mainloop=false;
				}
		}

	delete bc;
	delete bc1;
	delete mainwind;
	return 0;
	
}