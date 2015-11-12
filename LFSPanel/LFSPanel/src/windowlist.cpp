/*
 *
 * ©K. D. Hedger. Mon 12 Oct 16:45:21 BST 2015 kdhedger68713@gmail.com

 * This file (windowList.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not,see <http://www.gnu.org/licenses/>.
 */

#include "windowlist.h"

typedef struct
{
	int x, y;       /* location of window */
	int width, height;      /* width and height of window */
	int border_width;       /* border width of window */
	int depth;      /* depth of window */
	Visual *visual; /* the associated visual structure */
	Window root;    /* root of screen containing window */
	int classe;      /* InputOutput, InputOnly*/
	int bit_gravity;        /* one of the bit gravity values */
	int win_gravity;        /* one of the window gravity values */
	int backing_store;      /* NotUseful, WhenMapped, Always */
	unsigned long backing_planes;   /* planes to be preserved if possible */
	unsigned long backing_pixel;    /* value to be used when restoring planes */
	Bool save_under;        /* boolean, should bits under be saved? */
	Colormap colormap;      /* color map to be associated with window */
	Bool map_installed;     /* boolean, is color map currently installed*/
	int map_state;  /* IsUnmapped, IsUnviewable, IsViewable */
	long all_event_masks;   /* set of events all people have interest in*/
	long your_event_mask;   /* my event mask */
	long do_not_propagate_mask;     /* set of events that should not propagate */
	Bool override_redirect; /* boolean value for override-redirect */
	Screen *screen; /* back pointer to correct screen */
} MyXWindowAttributes;

LFSTK_menuButtonClass	*windowMenu=NULL;
LFSTK_menuButtonClass	*windowDeskMenu=NULL;
menuItemStruct			windowList[MAXWINDOWSINLIST];
menuItemStruct			windowDeskList[MAXWINDOWSINLIST];
char					windowBuffer[4096];
int						windowDeskListCnt=0;
int						windowListCnt=0;
int						updateWindowCnt=0;
const char				*possibleError="Unknown";
int						currentDesktop;

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long	mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(mainwind->display,msg,False);
	event.xclient.window=win;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;

	XSendEvent(mainwind->display,mainwind->rootWindow,False,mask,&event);
}

bool windowDeskMenuCB(void *p,void* ud)
{
	menuItemStruct	*menu=(menuItemStruct*)ud;
	Window			winid=0;
	unsigned long	desktop=0;

	if(menu==NULL)
		return(true);
//	alarm(0);
	winid=(Window)(menu->userData);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
//	alarm(refreshRate);
	return(true);
}

bool windowMenuCB(void *p,void* ud)
{
	menuItemStruct	*menu=(menuItemStruct*)ud;
	Window			winid=0;
	unsigned long	desktop=0;

	if(menu==NULL)
		return(true);
//	alarm(0);
	winid=(Window)(menu->userData);
	desktop=(unsigned long)menu->subMenuCnt;
	possibleError="Can't switch desktop";
	sendClientMessage(mainwind->rootWindow,"_NET_CURRENT_DESKTOP",desktop,0,0,0,0);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
//	alarm(refreshRate);
	return(true);
}

void resetMenus(void)
{
	for(int j=0; j<windowListCnt+1; j++)
		if(windowList[j].label!=NULL)
			{
				free((char*)windowList[j].label);
				windowList[j].label=NULL;
			}
	windowListCnt=-1;
}

/*
 * Check if window has given property
 */
bool hasProp(Display *dpy,Window win,Atom atom)
{
	Atom			typeret;
	int				formatret;
	unsigned char	*propret;
	unsigned long	bytesafter,numret;

	typeret=None;
	propret=NULL;
	XGetWindowProperty(dpy,win,atom,0,0,false,AnyPropertyType,&typeret,&formatret,&numret,&bytesafter,&propret);
	if (propret)
		XFree(propret);

	if(typeret!=None)
		return(true);

	return(false);
}


/*
 * Check if window is viewable
 */
bool isVisible(Display *dpy,Window win)
{
	bool ok;
	MyXWindowAttributes xwa;

	XGetWindowAttributes(dpy, win, (XWindowAttributes*)&xwa);

	ok=(xwa.classe == InputOutput) && (xwa.map_state == IsViewable);

	return ok;
}


Window doTreeWalk(Window wind)
{
	Window root, parent;
	Window *children;
	Window thewin;
	unsigned int n_children;
	int i;

	if (!XQueryTree(mainwind->display,wind,&root,&parent,&children,&n_children))
		return None;

	if (!children)
		return None;

	/* Check each child for WM_STATE and other validity */
	thewin=None;
	for (int j=n_children-1;j>=0;j--)
		{
			if (!isVisible(mainwind->display, children[j]))
				{
					children[j]=None; /* Don't bother descending into this one */
					continue;
				}
			if (!hasProp(mainwind->display, children[j],WM_STATE))
				continue;

			/* Got one */
			thewin=children[j];
			printf("window=%p",children[j]);
			char *wname;
			wname=NULL;
			XFetchName(mainwind->display,children[j],&wname);
			if(wname!=NULL)
				{
					printf(" name=%s\n",wname);
					XFree(wname);
				}
			else
				printf("\n");

		}
	thewin=None;
	/* No children matched, now descend into each child */
	for (i=(int) n_children - 1; i >= 0; i--)
		{
			if (children[i] == None)
				continue;
			thewin=doTreeWalk(children[i]);
			if (thewin != None)
				break;
		}

	XFree(children);

	return thewin;

}
#if 0
/*
 * Find virtual roots (_NET_VIRTUAL_ROOTS)
 */
static unsigned long *
Find_Roots(Display * dpy, Window root, unsigned int *num)
{
	Atom type_ret;
	int format_ret;
	unsigned char *prop_ret;
	unsigned long bytes_after, num_ret;
	Atom atom;

	*num=0;
	atom=XInternAtom(dpy, "_NET_VIRTUAL_ROOTS", False);
	if (!atom)
		return NULL;

	type_ret=None;
	prop_ret=NULL;
	if (XGetWindowProperty(dpy, root, atom, 0, 0x7fffffff, False,
	                       XA_WINDOW, &type_ret, &format_ret, &num_ret,
	                       &bytes_after, &prop_ret) != Success)
		return NULL;

	if (prop_ret && type_ret == XA_WINDOW && format_ret == 32)
		{
			*num=num_ret;
			return ((unsigned long *) prop_ret);
		}
	if (prop_ret)
		XFree(prop_ret);

	return NULL;
}
Window
Find_Client(Display * dpy, Window root, Window subwin)
{
	unsigned long *roots;
	unsigned int i, n_roots;
	Window win;

	/* Check if subwin is a virtual root */
	roots=Find_Roots(dpy, root, &n_roots);
	for (i=0; i < n_roots; i++)
		{
			if (subwin != roots[i])
				continue;
			win=Find_Child_At_Pointer(dpy, subwin);
			if (win == None)
				return subwin;      /* No child - Return virtual root. */
			subwin=win;
			break;
		}


	unsigned long *roots;
	unsigned int i, n_roots;
	//  Window win;

	Window win=mainwind->rootWindow;
	atom_wm_state=XInternAtom(mainwind->display, "WM_STATE", False);

//    roots=Find_Roots(mainwind->display, mainwind->window, &n_roots);
//   printf("--num=%i\n",n_roots);
	//  for (i=0; i < n_roots; i++)
	// {
	//win=roots[i];
	while(win!=None)
		{
			win=doTreeWalk(win);
		}
//	}
	exit(0);






#endif


	int getCurrentDesktop(void)
	{
		char	*desknum=NULL;

		desknum=mainwind->globalLib->LFSTK_oneLiner("%s","xprop -root |grep '_NET_CURRENT_DESKTOP(CARDINAL)'|head -n1|awk -F'=' '{print $2}'");
		if(desknum!=NULL)
			{
				currentDesktop=atoi(desknum);
				free(desknum);
			}
		else
			currentDesktop=1;
	}

	void updateWindowMenu(void)
	{
//return;
		FILE			*fp=NULL;
		unsigned long	*roots;
		unsigned int	n_roots;
		Window			win;

		updateWindowCnt++;
		if(updateWindowCnt>=WINDOWREFRESH)
			updateWindowCnt=0;
		else
			return;

//	alarm(0);
		if(windowListCnt>-1)
			resetMenus();

		getCurrentDesktop();

		usleep(10000);
		win=mainwind->rootWindow;
		while(win!=None)
			{
				printf("wind=%p\n",win);
				win=doTreeWalk(win);
				printf("win=%p\n",win);
			}
		win=None;
		printf("done\n");



		windowListCnt=0;
		fp=popen(WINHELPER,"r");
		usleep(10000);
		if(fp!=NULL)
			{
				windowBuffer[0]=0;
				fgets(windowBuffer,511,fp);
				windowBuffer[strlen(windowBuffer)-1]=0;
				windowListCnt=atoi(windowBuffer);
				if(windowListCnt>MAXWINDOWSINLIST)
					windowListCnt=MAXWINDOWSINLIST;

				windowDeskListCnt=0;

				for(int j=0; j<windowListCnt; j++)
					{
//full window
						windowBuffer[0]=0;
						fgets(windowBuffer,511,fp);//id
						usleep(100);
						windowBuffer[strlen(windowBuffer)-1]=0;
						windowList[j].userData=(void*)strtol(windowBuffer,NULL,16);
//this desk
						windowDeskList[windowDeskListCnt].userData=(void*)strtol(windowBuffer,NULL,16);

//full window
						windowBuffer[0]=0;
						fgets(windowBuffer,511,fp);//name
						usleep(100);
						windowBuffer[strlen(windowBuffer)-1]=0;
						windowList[j].label=strdup(windowBuffer);
//this desk
						windowDeskList[windowDeskListCnt].label=strdup(windowBuffer);

//full window
						windowBuffer[0]=0;
						fgets(windowBuffer,511,fp);//desktop
						usleep(100);
						windowBuffer[strlen(windowBuffer)-1]=0;
						windowList[j].bc=NULL;
						windowList[j].subMenus=NULL;
						windowList[j].subMenuCnt=atoi(windowBuffer);
						windowList[j].useIcon=false;
//this desk
						if(atoi(windowBuffer)==currentDesktop)
							{
								windowDeskList[windowDeskListCnt].bc=NULL;
								windowDeskList[windowDeskListCnt].subMenus=NULL;
								windowDeskList[windowDeskListCnt].subMenuCnt=atoi(windowBuffer);
								windowDeskList[windowDeskListCnt].useIcon=false;
								windowDeskListCnt++;
							}
						else
							{
								if(windowDeskList[windowDeskListCnt].label!=NULL)
									free((char*)windowDeskList[windowDeskListCnt].label);
								windowDeskList[windowDeskListCnt].label=NULL;
								windowDeskList[windowDeskListCnt].useIcon=false;
								windowDeskList[windowDeskListCnt].subMenus=NULL;
								windowDeskList[windowDeskListCnt].bc=NULL;
							}
					}
				fflush(fp);
				pclose(fp);
				usleep(100);

				if(windowMenu!=NULL)
					windowMenu->LFSTK_updateMenus(windowList,windowListCnt);
				if(windowDeskMenu!=NULL)
					{
						if(windowDeskListCnt>0)
							{
								windowDeskMenu->LFSTK_updateMenus(windowDeskList,windowDeskListCnt);
								windowDeskMenu->LFSTK_setActive(true);
							}
						else
							windowDeskMenu->LFSTK_setActive(false);
					}
			}
//	alarm(refreshRate);
	}

	int addWindowDeskMenu(int x,int y,int grav,bool fromleft)
	{
		const char	*icon=NULL;
		int			xpos=x;
		int			ypos=y;
		int			width=0;
		int			height=0;
		int			thisgrav=grav;
		int			iconsize=16;


		if(windowDeskMenu!=NULL)
			{
				printError("Duplicate current desktop window selector");
				return(0);
			}

		setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

		windowDeskMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
		icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
		if(icon!=NULL)
			windowDeskMenu->LFSTK_setImageFromPath(icon,iconsize,iconsize);
		else
			windowDeskMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",iconsize);
		windowDeskMenu->LFSTK_setCallBack(NULL,windowDeskMenuCB,NULL);

		windowDeskListCnt=-1;
		windowListCnt=-1;
		updateWindowCnt=WINDOWREFRESH;
		updateWindowMenu();
		useAlarm=true;
		return(width);
	}

	int addWindowMenu(int x,int y,int grav,bool fromleft)
	{
		const char	*icon=NULL;
		int			xpos=x;
		int			ypos=y;
		int			width=0;
		int			height=0;
		int			thisgrav=grav;
		int			iconsize=16;

		if(windowMenu!=NULL)
			{
				printError("Duplicate window selector");
				return(0);
			}

		setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

		windowMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
		icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"computer","");
		if(icon!=NULL)
			windowMenu->LFSTK_setIconFromPath(icon,iconsize);
		else
			windowMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",iconsize);
		windowMenu->LFSTK_setCallBack(NULL,windowMenuCB,NULL);

		windowDeskListCnt=-1;
		windowListCnt=-1;
		updateWindowCnt=WINDOWREFRESH;
		updateWindowMenu();
		useAlarm=true;
		return(width);
	}

