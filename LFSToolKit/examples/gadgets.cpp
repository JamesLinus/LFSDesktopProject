#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ "$0" -O3 -I../LFSToolKit/src -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft ) -llfstoolkit -lImlib2||exit 1
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

#define MAXMAINMENUS	4
#define MAXSUBMENUS		3

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_buttonClass		*bc=NULL;
LFSTK_menuButtonClass	*mb=NULL;
LFSTK_menuButtonClass	*mbwithsubs=NULL;
LFSTK_toggleButtonClass *tb=NULL;
LFSTK_toggleButtonClass *tbnormal=NULL;
LFSTK_lineEditClass		*le=NULL;
LFSTK_buttonClass		*iconButton=NULL;

menuItemStruct			*mainMenus;
menuItemStruct			*mainMenusWithSubs;
menuItemStruct			*subMenus;

const char				*mainMenuNames[]={"Menu 1","Menu 2","Menu 3","Menu 4"};
const char				*subMenuNames[]={"Sub Menu 1","Sub Menu 2","Sub Menu 3","Sub Menu 4"};
LFSTK_buttonClass		*quit=NULL;

bool					mainLoop=true;
int						iconSize=16;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(ud!=NULL)
		printf(">>>%s<<<\n",(const char*)ud);
	printf("Line edit=%s\n",le->LFSTK_getBuffer()->c_str());
	return(true);
}

bool menuCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	printf("Selected Menu Label:%s\n",menuitem->label);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	Pixmap	ic[2];
	
	wc=new LFSTK_windowClass(0,0,WIDTH,HITE,"Gadgets",false);

	label=new LFSTK_labelClass(wc,"Available Gadgets",BORDER,sy,WIDTH-(BORDER*2),BHITE,BGRAV);
	sy+=(BORDER*2);

//button
	bc=new LFSTK_buttonClass(wc,"Button",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	bc->LFSTK_setCallBack(NULL,buttonCB,(void*)bc->LFSTK_getLabel());
	//bc->LFSTK_setStyle(BEVELIN);
	//bc->LFSTK_setActive(true);
	sy+=YSPACING;

//check button	
	tb=new LFSTK_toggleButtonClass(wc,"Check",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	tb->LFSTK_setCallBack(NULL,buttonCB,(void*)tb->LFSTK_getLabel());
	sy+=YSPACING;

//toggle button	
	tbnormal=new LFSTK_toggleButtonClass(wc,"Toggle",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	tbnormal->LFSTK_setCallBack(NULL,buttonCB,(void*)tbnormal->LFSTK_getLabel());
	tbnormal->LFSTK_setToggleStyle(TOGGLENORMAL);
	tbnormal->LFSTK_setIconFromPath("./api.png");
	sy+=YSPACING;

//menu button
	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"/usr/share/icons/gnome/48x48/devices/computer.png",&ic[0],&ic[1],iconSize);
	mainMenus=new menuItemStruct[MAXMAINMENUS];

	for(int j=0;j<MAXMAINMENUS;j++)
		{
			mainMenus[j].label=mainMenuNames[j];
			mainMenus[j].userData=NULL;
			mainMenus[j].bc=NULL;
			mainMenus[j].subMenus=NULL;
			mainMenus[j].useIcon=true;
			mainMenus[j].icon[0]=ic[0];
			mainMenus[j].icon[1]=ic[1];
			mainMenus[j].iconSize=iconSize;
		}

	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"./ROOTKKEdit.png",&mainMenus[2].icon[0],&mainMenus[2].icon[1],16);

	mb=new LFSTK_menuButtonClass(wc,"Main Menu",BORDER,sy,BWIDTH,BHITE,BGRAV);
	mb->LFSTK_setIconFromPath("./BookMark.png");
	mb->LFSTK_setCallBack(NULL,menuCB,NULL);
	mb->LFSTK_addMenus(mainMenus,MAXMAINMENUS);
	mb->LFSTK_setLabelOriention(LEFT);
	sy+=YSPACING;

//menu button with sub menu
	mainMenusWithSubs=new menuItemStruct[MAXMAINMENUS];
	for(int j=0;j<MAXMAINMENUS;j++)
		{
			mainMenusWithSubs[j].label=mainMenuNames[j];
			mainMenusWithSubs[j].userData=NULL;
			mainMenusWithSubs[j].bc=NULL;
			mainMenusWithSubs[j].subMenus=NULL;
			mainMenusWithSubs[j].useIcon=false;
		}
//sub menus
	subMenus=new menuItemStruct[MAXSUBMENUS];
	for(int j=0;j<MAXSUBMENUS;j++)
		{
			subMenus[j].label=subMenuNames[j];
			subMenus[j].userData=NULL;
			subMenus[j].bc=NULL;
			subMenus[j].subMenus=NULL;
			subMenus[j].useIcon=true;
			subMenus[j].icon[0]=ic[0];
			subMenus[j].icon[1]=ic[1];
			subMenus[j].iconSize=iconSize;
		}
	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"/usr/share/icons/gnome/48x48/devices/audio-speakers.png",&subMenus[2].icon[0],&subMenus[2].icon[1],16);
//add sub menus
	mainMenusWithSubs[3].subMenus=subMenus;
	mainMenusWithSubs[3].subMenuCnt=MAXSUBMENUS;

	mainMenusWithSubs[0].useIcon=true;
	mainMenusWithSubs[0].icon[0]=ic[0];
	mainMenusWithSubs[0].icon[1]=ic[1];
	mainMenusWithSubs[0].iconSize=iconSize;
	mainMenusWithSubs[1].useIcon=true;
	mainMenusWithSubs[1].icon[0]=ic[0];
	mainMenusWithSubs[1].icon[1]=ic[1];
	mainMenusWithSubs[1].iconSize=iconSize;

	mainMenusWithSubs[1].subMenus=subMenus;
	mainMenusWithSubs[1].subMenuCnt=MAXSUBMENUS;
	
	mbwithsubs=new LFSTK_menuButtonClass(wc,"Sub Menus",BORDER,sy,BWIDTH,BHITE,BGRAV);
	mbwithsubs->LFSTK_setIconFromPath("./ManPageEditor.png");
	mbwithsubs->LFSTK_setCallBack(NULL,menuCB,NULL);
	mbwithsubs->LFSTK_setLabelOriention(LEFT);
	mbwithsubs->LFSTK_addMenus(mainMenusWithSubs,MAXMAINMENUS);
	sy+=YSPACING;

//icon button
	iconButton=new LFSTK_buttonClass(wc,"",BORDER,sy,68,64+4,NorthWestGravity);
	iconButton->LFSTK_setCallBack(NULL,buttonCB,(void*)iconButton->LFSTK_getLabel());
	iconButton->LFSTK_setIconFromPath("/usr/share/pixmaps/LFSTux.png",64);
	sy+=68;

//line edit
	le=new LFSTK_lineEditClass(wc,"Hello World",BORDER,sy,BWIDTH*2,BHITE,BGRAV);
	
	quit=new LFSTK_buttonClass(wc,"Quit",BX,HITE-BHITE-BORDER,BWIDTH,BHITE,SouthGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

	//wc->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/bc.png",-1);
//	wc->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	iconButton->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	iconButton->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/buttons/button1.png",-1);
//	quit->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/buttons/button1.png",-1);
//	mbwithsubs->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/menus/menufill.png",-1);
//	label->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/bc.png",-1);
	//mb->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	mb->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/buttons/button1.png",-1);

//	label->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	iconButton->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
//	mb->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
//	mbwithsubs->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);

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

	delete bc;
	delete tb;
	delete tbnormal;
	delete mb;
	delete wc;
	return 0;
}