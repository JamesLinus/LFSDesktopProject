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

enum {EXIT=0,APPLY,THEMELABEL,THEMEBOX,BNORMAL,ENORMAL,BPRELIGHT,EPRELIGHT,BACTIVE,EACTIVE,BINACTIVE,EINACTIVE,MENUNORM,EMENUNORM,MENUPRELITE,EMENUPRELITE,MENUACTIVE,EMENUACTIVE,MENUINACTIVE,EMENUINACTIVE,NOMORE};

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
	int				spacing;

	wc=new LFSTK_windowClass(sx,sy,800,400,"appmenu",false);
	wc->LFSTK_setDecorated(true);

	geom=wc->LFSTK_getGeom();
	bc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	bc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);
	XMapWindow(wc->display,bc[EXIT]->LFSTK_getWindow());

	bc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	bc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);
	XMapWindow(wc->display,bc[APPLY]->LFSTK_getWindow());

	int bwidth=96;
	int bhite=24;
	int vspacing=bhite+10;
	sx=10;
	sy=10;
	spacing=bwidth+10;
	
	bc[BNORMAL]=new LFSTK_buttonClass(wc,"Normal",sx,sy,bwidth,24,NorthWestGravity);
	bc[BNORMAL]->LFSTK_setActive(false);
	XMapWindow(wc->display,bc[BNORMAL]->LFSTK_getWindow());

	sx+=spacing;
	bc[ENORMAL]=new LFSTK_lineEditClass(wc,bc[BNORMAL]->LFSTK_getColourName(NORMALCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[ENORMAL])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[ENORMAL]->LFSTK_getWindow());

	sx=10;
	sy+=vspacing;
	bc[BPRELIGHT]=new LFSTK_buttonClass(wc,"Prelight",sx,sy,bwidth,24,NorthWestGravity);
	bc[BPRELIGHT]->LFSTK_setActive(false);
	XMapWindow(wc->display,bc[BPRELIGHT]->LFSTK_getWindow());

	sx+=spacing;
	bc[EPRELIGHT]=new LFSTK_lineEditClass(wc,bc[BPRELIGHT]->LFSTK_getColourName(PRELIGHTCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BPRELIGHT]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EPRELIGHT])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EPRELIGHT]->LFSTK_getWindow());


	sx=10;
	sy+=vspacing;
	bc[BACTIVE]=new LFSTK_buttonClass(wc,"Active",sx,sy,bwidth,24,NorthWestGravity);
	bc[BACTIVE]->LFSTK_setActive(false);
	XMapWindow(wc->display,bc[BACTIVE]->LFSTK_getWindow());

	sx+=spacing;
	bc[EACTIVE]=new LFSTK_lineEditClass(wc,bc[BACTIVE]->LFSTK_getColourName(ACTIVECOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EACTIVE])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EACTIVE]->LFSTK_getWindow());

	sx=10;
	sy+=vspacing;
	bc[BINACTIVE]=new LFSTK_buttonClass(wc,"Inactive",sx,sy,bwidth,24,NorthWestGravity);
	bc[BINACTIVE]->LFSTK_setActive(false);
	XMapWindow(wc->display,bc[BINACTIVE]->LFSTK_getWindow());

	sx+=spacing;
	bc[EINACTIVE]=new LFSTK_lineEditClass(wc,bc[BINACTIVE]->LFSTK_getColourName(INACTIVECOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[BINACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EINACTIVE])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EINACTIVE]->LFSTK_getWindow());

	sx=10;
	sy+=vspacing;
	bc[THEMELABEL]=new LFSTK_buttonClass(wc,"Theme",sx,sy,bwidth,24,NorthWestGravity);
	bc[THEMELABEL]->LFSTK_setActive(false);
	bc[THEMELABEL]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->globalWindowColours[NORMALCOLOUR]);
	static_cast<LFSTK_buttonClass*>(bc[THEMELABEL])->LFSTK_setStyle(FLATBUTTON);
	XMapWindow(wc->display,bc[THEMELABEL]->LFSTK_getWindow());

	sx+=spacing;
	XMapWindow(wc->display,bc[BINACTIVE]->LFSTK_getWindow());
	bc[THEMEBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->globalThemePath,sx,sy-1,(bwidth*2)+spacing+40,24,NorthWestGravity);
	XMapWindow(wc->display,bc[THEMEBOX]->LFSTK_getWindow());

//menus
	sx=10+bwidth+spacing+40;
	sy=10;
	bc[MENUNORM]=new LFSTK_buttonClass(wc,"Menu Normal",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUNORM]->LFSTK_setActive(false);
	bc[MENUNORM]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->globalMenuItemColours[NORMALCOLOUR]);
	static_cast<LFSTK_buttonClass*>(bc[MENUNORM])->LFSTK_setStyle(FLATBUTTON);
	XMapWindow(wc->display,bc[MENUNORM]->LFSTK_getWindow());

	sx+=spacing;
	bc[EMENUNORM]=new LFSTK_lineEditClass(wc,wc->globalLib->globalMenuItemColours[NORMALCOLOUR],sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUNORM]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUNORM])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EMENUNORM]->LFSTK_getWindow());

	sx=10+bwidth+spacing+40;
	sy+=vspacing;
	bc[MENUPRELITE]=new LFSTK_buttonClass(wc,"Menu Prelight",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUPRELITE]->LFSTK_setActive(false);
	bc[MENUPRELITE]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->globalMenuItemColours[PRELIGHTCOLOUR]);
	static_cast<LFSTK_buttonClass*>(bc[MENUPRELITE])->LFSTK_setStyle(FLATBUTTON);
	XMapWindow(wc->display,bc[MENUPRELITE]->LFSTK_getWindow());

	sx+=spacing;
	bc[EMENUPRELITE]=new LFSTK_lineEditClass(wc,wc->globalLib->globalMenuItemColours[PRELIGHTCOLOUR],sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUPRELITE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUPRELITE])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EMENUPRELITE]->LFSTK_getWindow());

	sx=10+bwidth+spacing+40;
	sy+=vspacing;
	bc[MENUACTIVE]=new LFSTK_buttonClass(wc,"Menu Active",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUACTIVE]->LFSTK_setActive(false);
	bc[MENUACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->globalMenuItemColours[ACTIVECOLOUR]);
	static_cast<LFSTK_buttonClass*>(bc[MENUACTIVE])->LFSTK_setStyle(FLATBUTTON);
	XMapWindow(wc->display,bc[MENUACTIVE]->LFSTK_getWindow());

	sx+=spacing;
	bc[EMENUACTIVE]=new LFSTK_lineEditClass(wc,wc->globalLib->globalMenuItemColours[ACTIVECOLOUR],sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUACTIVE])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EMENUACTIVE]->LFSTK_getWindow());

	sx=10+bwidth+spacing+40;
	sy+=vspacing;
	bc[MENUINACTIVE]=new LFSTK_buttonClass(wc,"Menu Inactive",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUINACTIVE]->LFSTK_setActive(false);
	bc[MENUINACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->globalMenuItemColours[INACTIVECOLOUR]);
	static_cast<LFSTK_buttonClass*>(bc[MENUINACTIVE])->LFSTK_setStyle(FLATBUTTON);
	XMapWindow(wc->display,bc[MENUINACTIVE]->LFSTK_getWindow());

	sx+=spacing;
	bc[EMENUINACTIVE]=new LFSTK_lineEditClass(wc,wc->globalLib->globalMenuItemColours[INACTIVECOLOUR],sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EMENUINACTIVE]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EMENUINACTIVE])->LFSTK_getBuffer()->c_str()));
	XMapWindow(wc->display,bc[EMENUINACTIVE]->LFSTK_getWindow());

	XMapWindow(wc->display,wc->window);
	wc->LFSTK_clearWindow();
	wc->LFSTK_setKeepAbove(true);

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