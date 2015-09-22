#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ "$0" -I../LFSToolKit/src -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft) -llfstoolkit||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include <stdio.h>
#include <unistd.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"
#include "LFSTKLabel.h"
#include "LFSTKToggleButton.h"

#define WIDTH			400
#define HITE			600
#define HALF			WIDTH/2
#define BWIDTH			96
#define BHITE			24
#define BX				HALF-(BWIDTH/2)
#define BORDER			20
#define YSPACING		32
#define BGRAV			NorthWestGravity

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_buttonClass		*bc=NULL;
LFSTK_menuButtonClass	*mb=NULL;
LFSTK_toggleButtonClass *tb=NULL;
LFSTK_toggleButtonClass *tbnormal=NULL;
LFSTK_lineEditClass		*le=NULL;

LFSTK_buttonClass		*quit=NULL;

bool					mainLoop=true;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(ud!=NULL)
		printf(">>>%s<<<\n",(const char*)ud);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	
	wc=new LFSTK_windowClass(0,0,WIDTH,HITE,"Gadgets",false);

	label=new LFSTK_labelClass(wc,"Available Gadgets",BORDER,sy,WIDTH-(BORDER*2),BHITE,BGRAV);
	sy+=(BORDER*2);

	bc=new LFSTK_buttonClass(wc,"Button",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	bc->LFSTK_setCallBack(NULL,buttonCB,(void*)bc->LFSTK_getLabel());
	sy+=YSPACING;
	
	tb=new LFSTK_toggleButtonClass(wc,"Check",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	tb->LFSTK_setCallBack(NULL,buttonCB,(void*)tb->LFSTK_getLabel());
	sy+=YSPACING;
	
	tbnormal=new LFSTK_toggleButtonClass(wc,"Toggle",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	tbnormal->LFSTK_setCallBack(NULL,buttonCB,(void*)tbnormal->LFSTK_getLabel());
	tbnormal->LFSTK_setToggleStyle(TOGGLENORMAL);
	sy+=YSPACING;
	
	quit=new LFSTK_buttonClass(wc,"Quit",BX,HITE-BHITE-BORDER,BWIDTH,BHITE,SouthGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	wc->LFSTK_showWindow();

	mainLoop=true;
	while(mainLoop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
				}
		}

	delete wc;
	return 0;
}