#if 0

#©keithhedger Mon 20 Jul 14:09:10 BST 2015 kdhedger68713@gmail.com
g++ -Wall "$0" -I../LFSToolKit/src -L../LFSToolKit/app/.libs -llfstoolkit -lXm $(pkg-config --cflags --libs xt xext ice sm x11 xft) -lXm -lXaw3d -lXt -lXext -lICE -lSM -lX11||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"
#include "LFSTKLib.h"

enum {EXIT=0,APPLY,THEMELABEL,THEMEBOX,BNORMAL,ENORMAL,BPRELIGHT,EPRELIGHT,BACTIVE,EACTIVE,BINACTIVE,EINACTIVE,MENUNORM,EMENUNORM,MENUPRELITE,EMENUPRELITE,MENUACTIVE,EMENUACTIVE,MENUINACTIVE,EMENUINACTIVE,WNORMAL,EWNORMAL,NOMORE};
//,FONTNORMCOL,EFONTNORMCOL,FONTPRELIRECOL,EFONTPRELITECOL,FONTACTIVECOL,EFONTACTIVECOL,FONTINACTIVECOL,EFONTINACTIVECOL,NOMORE};

bool					mainloop=false;
LFSTK_windowClass		*wc;
LFSTK_gadgetClass		*bc[NOMORE]={NULL,};

bool callback(void *p,void* ud)
{
	printf("button=%p\n",ud);
	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case APPLY:
				wc->LFSTK_setWindowColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
				wc->LFSTK_clearWindow();
				bc[THEMELABEL]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
				bc[THEMELABEL]->LFSTK_clearWindow();

				for(int j=BNORMAL;j<NOMORE;j+=2)
					bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));

				for(int j=BNORMAL;j<NOMORE;j++)
					if(bc[j]!=NULL)
						bc[j]->LFSTK_clearWindow();
				break;
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bwidth=96;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+40,col4;

	wc=new LFSTK_windowClass(sx,sy,800,400,"appmenu",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();
	bc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	bc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	bc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	bc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);
	sx=col1;
	sy=10;
	
	bc[BNORMAL]=new LFSTK_buttonClass(wc,"Normal",sx,sy,bwidth,24,NorthWestGravity);

	sx+=spacing;
	bc[ENORMAL]=new LFSTK_lineEditClass(wc,bc[BNORMAL]->LFSTK_getColourName(NORMALCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[ENORMAL])->LFSTK_getBuffer()->c_str()));

	sx=col1;
	sy+=vspacing;
	bc[BPRELIGHT]=new LFSTK_buttonClass(wc,"Prelight",sx,sy,bwidth,24,NorthWestGravity);

	sx+=spacing;
	bc[EPRELIGHT]=new LFSTK_lineEditClass(wc,bc[BPRELIGHT]->LFSTK_getColourName(PRELIGHTCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BPRELIGHT]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EPRELIGHT])->LFSTK_getBuffer()->c_str()));

	sx=col1;
	sy+=vspacing;
	bc[BACTIVE]=new LFSTK_buttonClass(wc,"Active",sx,sy,bwidth,24,NorthWestGravity);

	sx+=spacing;
	bc[EACTIVE]=new LFSTK_lineEditClass(wc,bc[BACTIVE]->LFSTK_getColourName(ACTIVECOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EACTIVE])->LFSTK_getBuffer()->c_str()));

	sx=col1;
	sy+=vspacing;
	bc[BINACTIVE]=new LFSTK_buttonClass(wc,"Inactive",sx,sy,bwidth,24,NorthWestGravity);

	sx+=spacing;
	bc[EINACTIVE]=new LFSTK_lineEditClass(wc,bc[BINACTIVE]->LFSTK_getColourName(INACTIVECOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BINACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EINACTIVE])->LFSTK_getBuffer()->c_str()));

//menus
	sx=col2;
	sy=10;
	bc[MENUNORM]=new LFSTK_buttonClass(wc,"Menu Normal",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUNORM]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEM));
	static_cast<LFSTK_buttonClass*>(bc[MENUNORM])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[EMENUNORM]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEM),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUNORM]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUNORM])->LFSTK_getBuffer()->c_str()));

	sx=col2;
	sy+=vspacing;
	bc[MENUPRELITE]=new LFSTK_buttonClass(wc,"Menu Prelight",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUPRELITE]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEMENUITEM));
	static_cast<LFSTK_buttonClass*>(bc[MENUPRELITE])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[EMENUPRELITE]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEMENUITEM),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUPRELITE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUPRELITE])->LFSTK_getBuffer()->c_str()));

	sx=col2;
	sy+=vspacing;
	bc[MENUACTIVE]=new LFSTK_buttonClass(wc,"Menu Active",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEMENUITEM));
	static_cast<LFSTK_buttonClass*>(bc[MENUACTIVE])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[EMENUACTIVE]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEMENUITEM),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUACTIVE])->LFSTK_getBuffer()->c_str()));

	sx=col2;
	sy+=vspacing;
	bc[MENUINACTIVE]=new LFSTK_buttonClass(wc,"Menu Inactive",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUINACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEMENUITEM));
	static_cast<LFSTK_buttonClass*>(bc[MENUINACTIVE])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[EMENUINACTIVE]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEMENUITEM),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUINACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUINACTIVE])->LFSTK_getBuffer()->c_str()));


	sx=col3;
	bc[WNORMAL]=new LFSTK_buttonClass(wc,"Window",sx,sy,bwidth,24,NorthWestGravity);
	bc[WNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
	static_cast<LFSTK_buttonClass*>(bc[WNORMAL])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[EWNORMAL]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EWNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));

	sx=col1;
	sy+=vspacing;
	bc[THEMELABEL]=new LFSTK_buttonClass(wc,"Theme",sx,sy,bwidth,24,NorthWestGravity);
	bc[THEMELABEL]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
	static_cast<LFSTK_buttonClass*>(bc[THEMELABEL])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[THEMEBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPETHEME),sx,sy-1,(bwidth*2)+spacing+20,24,NorthWestGravity);



	XMapSubwindows(wc->display,wc->window);
	XMapRaised(wc->display,wc->window);
	wc->LFSTK_setKeepAbove(true);

	for(int j=THEMELABEL;j<NOMORE;j+=2)
		bc[j]->LFSTK_setActive(false);

	mainloop=true;
	while(mainloop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);

			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						//mainloop=inWindow();
						break;
					case Expose:
						wc->LFSTK_setActive(true);
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
					//printf("configureWindow\n");
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;
					default:
						//XNextEvent(wc->display,&event);
						break;
				}
		}

	delete wc;
	for(int j=0;j<NOMORE;j++)
		if(bc[j]!=NULL)
			delete bc[j];
	return 0;
}