#if 0

#©keithhedger Mon 20 Jul 14:09:10 BST 2015 kdhedger68713@gmail.com

g++ "$0" -I/tmp/AAA/usr/local/include -L/tmp/AAA/usr/local/lib -llfstoolkit -lXm $(pkg-config --cflags --libs xt xext ice sm x11) -lXm -lXaw3d -lXt -lXext -lICE -lSM -lX11||exit 1
./a.out "$@"
retval=$?
rm ./a.out
exit $retval
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include <LFSTKGlobals.h>

int main(int argc, char **argv)
{
	int keep_running=1;

	printf("Hello World!\n");

	LFSTK_windowClass *wc=new LFSTK_windowClass(100,100,800,400,"rgb:ff/00/ff","rgb:80/80/80");

	XMapWindow(wc->display,wc->window);
	XEvent event;

	LFSTK_buttonClass *bc=new LFSTK_buttonClass(wc->display,wc->window,50,50,100,100,0,"rgb:a0/a0/a0","rgb:d0/d0/d0");
	LFSTK_buttonClass *bc1=new LFSTK_buttonClass(wc,"button 2",250,250,100,50,0,"rgb:f0/a0/a0","rgb:e0/d0/d0");

	bc->listen.userData=87654321;
	XMapWindow(wc->display,bc->window);
	wc->LFSTK_setListener(bc->window,bc->LFSTK_getListen());

	bc1->listen.userData=666;
	XMapWindow(wc->display,bc1->window);
	wc->LFSTK_setListener(bc1->window,bc1->LFSTK_getListen());

	while (keep_running)
		{
			XNextEvent(wc->display,&event);
			listener *l=wc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->userData!=-1))
				l->function(l->pointer,&event);

			switch(event.type)
				{
				case ClientMessage:
					if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
						keep_running = 0;
					break;

				case Expose:
					wc->LFSTK_clearWindow();
					bc->LFSTK_clearWindow();
					bc1->LFSTK_clearWindow();
					break;
				case ConfigureNotify:
					wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
					wc->LFSTK_clearWindow();

				default:
					break;
				}
		}

	delete bc;
	delete wc;
	return 0;
}