/*
 * Copyright 2010 Johan Veenhuizen
 *
 * Permission is hereby granted,free of charge,to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,including without limitation
 * the rights to use,copy,modify,merge,publish,distribute,sublicense,
 * and/or sell copies of the Software,and to permit persons to whom the
 * Software is furnished to do so,subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS",WITHOUT WARRANTY OF ANY KIND,EXPRESS OR
 * IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,DAMAGES OR
 * OTHER LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE,
 * ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "wind.h"
#include "lib.h"
#include "client.h"
#include "frame.h"

#define DEFAULT_NUMBER_OF_DESKTOPS 12

#define xatom(name) XInternAtom(dpy,(name),False)

#define NET_WM_STATE_REMOVE 0
#define NET_WM_STATE_ADD 1
#define NET_WM_STATE_TOGGLE 2

/*
 * The list of supported properties. Note that we need to
 * include some properties that we actually never use in
 * the _NET_SUPPORTED list in order to show applications
 * that we do indeed know about them. For example,pagers
 * act strangely if we don't mention _NET_WM_STATE_HIDDEN.
 *
 * NB: Keep this list sorted.
 */
Atom NET_ACTIVE_WINDOW;
Atom NET_CLIENT_LIST;
Atom NET_CLIENT_LIST_STACKING;
Atom NET_CLOSE_WINDOW;
Atom NET_CURRENT_DESKTOP;
Atom NET_DESKTOP_GEOMETRY;
Atom NET_DESKTOP_VIEWPORT;
Atom NET_FRAME_EXTENTS;
Atom NET_NUMBER_OF_DESKTOPS;
Atom NET_REQUEST_FRAME_EXTENTS;
Atom NET_SUPPORTED;
Atom NET_SUPPORTING_WM_CHECK;
Atom NET_WM_ACTION_CHANGE_DESKTOP;
Atom NET_WM_ACTION_CLOSE;
Atom NET_WM_ACTION_FULLSCREEN;
Atom NET_WM_ACTION_MINIMIZE;
Atom NET_WM_ALLOWED_ACTIONS;
Atom NET_WM_DESKTOP;
Atom NET_WM_ICON_NAME;
Atom NET_WM_NAME;
Atom NET_WM_STATE;
Atom NET_WM_STATE_ABOVE;
Atom NET_WM_STATE_BELOW;
Atom NET_WM_STATE_FULLSCREEN;
Atom NET_WM_STATE_HIDDEN;
Atom NET_WM_STATE_SKIP_TASKBAR;
Atom NET_WM_VISIBLE_ICON_NAME;
Atom NET_WM_VISIBLE_NAME;
Atom NET_WM_WINDOW_TYPE;
Atom NET_WM_WINDOW_TYPE_DOCK;
Atom NET_WORKAREA;
Atom NET_WM_WINDOW_TYPE_DESKTOP;

Atom UTF8_STRING;

Window wmcheckwin=None;

struct
{
	Window *v;
	size_t n;
	size_t lim;
} clientlist= { NULL,0,0 };

void ewmh_notifyclientdesktop(Window w,unsigned long i)
{
	setprop(w,NET_WM_DESKTOP,XA_CARDINAL,32,&i,1);
}

void ewmh_notifycurdesk(unsigned long n)
{
	setprop(root,NET_CURRENT_DESKTOP,XA_CARDINAL,32,&n,1);
}

void ewmh_notifyframeextents(Window w,struct extents e)
{
	unsigned long v[4]= { (unsigned long)e.left,(unsigned long)e.right,(unsigned long)e.top,(unsigned long)e.bottom };
	setprop(w,NET_FRAME_EXTENTS,XA_CARDINAL,32,v,NELEM(v));
}

void addclient(Window w)
{
	if (clientlist.n == clientlist.lim)
		{
			clientlist.lim += 32;
			clientlist.v=(Window*)xrealloc(clientlist.v,clientlist.lim * sizeof clientlist.v[0]);
		}
	clientlist.v[clientlist.n++]=w;
	setprop(root,NET_CLIENT_LIST,XA_WINDOW,32,clientlist.v,clientlist.n);
}

void delclient(Window w)
{
	unsigned int i;
	for (i=0; i<clientlist.n && clientlist.v[i] != w; i++)
		;
	if (i<clientlist.n)
		{
			for (; i<clientlist.n - 1; i++)
				clientlist.v[i]=clientlist.v[i + 1];
			clientlist.n--;
		}
	setprop(root,NET_CLIENT_LIST,XA_WINDOW,32,clientlist.v,clientlist.n);
	if (clientlist.n == 0)
		{
			free(clientlist.v);
			clientlist.v=NULL;
			clientlist.lim=0;
		}
}

unsigned long ewmh_getndesktops(void)
{
	unsigned long ndesk=DEFAULT_NUMBER_OF_DESKTOPS;
	unsigned long n;
	unsigned long *p=(long unsigned int*)getprop(root,NET_NUMBER_OF_DESKTOPS,XA_CARDINAL,32,&n);
	if (p != NULL)
		{
			if (n == 1)
				ndesk=*p & 0xffffffffUL;
			XFree(p);
		}
	return ndesk;
}

void ewmh_notifyndesk(unsigned long n)
{
	long *viewport=(long int*)xmalloc(n * 2 * sizeof (long));
	long *workarea=(long int*)xmalloc(n * 4 * sizeof (long));
	for (unsigned long i=0; i<n; i++)
		{
			viewport[2 * i + 0]=0;
			viewport[2 * i + 1]=0;

			workarea[4 * i + 0]=0;
			workarea[4 * i + 1]=0;
			workarea[4 * i + 2]=DisplayWidth(dpy,scr);
			workarea[4 * i + 3]=DisplayHeight(dpy,scr);
		}
	setprop(root,NET_DESKTOP_VIEWPORT,XA_CARDINAL,32,viewport,n * 2);
	setprop(root,NET_WORKAREA,XA_CARDINAL,32,workarea,n * 4);
	free(workarea);
	free(viewport);

	setprop(root,NET_NUMBER_OF_DESKTOPS,XA_CARDINAL,32,&n,1);
}

void setcurrentdesktop(unsigned long i)
{
	setprop(root,NET_CURRENT_DESKTOP,XA_CARDINAL,32,&i,1);
}

void ewmh_startwm(void)
{
	UTF8_STRING=xatom("UTF8_STRING");

	Atom v[] =
	{
		// Keep sorted
		NET_ACTIVE_WINDOW=xatom("_NET_ACTIVE_WINDOW"),NET_CLIENT_LIST=xatom("_NET_CLIENT_LIST"),NET_CLIENT_LIST_STACKING=xatom("_NET_CLIENT_LIST_STACKING"),NET_CLOSE_WINDOW=xatom("_NET_CLOSE_WINDOW"),NET_CURRENT_DESKTOP=xatom("_NET_CURRENT_DESKTOP"),NET_DESKTOP_GEOMETRY=xatom("_NET_DESKTOP_GEOMETRY"),NET_DESKTOP_VIEWPORT=xatom("_NET_DESKTOP_VIEWPORT"),NET_FRAME_EXTENTS=xatom("_NET_FRAME_EXTENTS"),NET_NUMBER_OF_DESKTOPS=xatom("_NET_NUMBER_OF_DESKTOPS"),NET_REQUEST_FRAME_EXTENTS =
		xatom("_NET_REQUEST_FRAME_EXTENTS"),NET_SUPPORTED=xatom("_NET_SUPPORTED"),NET_SUPPORTING_WM_CHECK=xatom("_NET_SUPPORTING_WM_CHECK"),NET_WM_ACTION_CHANGE_DESKTOP =
		xatom("_NET_WM_ACTION_CHANGE_DESKTOP"),NET_WM_ACTION_CLOSE=xatom("_NET_WM_ACTION_CLOSE"),NET_WM_ACTION_FULLSCREEN=xatom("_NET_WM_ACTION_FULLSCREEN"),NET_WM_ACTION_MINIMIZE=xatom("_NET_WM_ACTION_MINIMIZE"),NET_WM_ALLOWED_ACTIONS=xatom("_NET_WM_ALLOWED_ACTIONS"),NET_WM_DESKTOP=xatom("_NET_WM_DESKTOP"),NET_WM_ICON_NAME=xatom("_NET_WM_ICON_NAME"),NET_WM_NAME=xatom("_NET_WM_NAME"),NET_WM_STATE=xatom("_NET_WM_STATE"),NET_WM_STATE_ABOVE=xatom("_NET_WM_STATE_ABOVE"),NET_WM_STATE_BELOW=xatom("_NET_WM_STATE_BELOW"),NET_WM_STATE_FULLSCREEN=xatom("_NET_WM_STATE_FULLSCREEN"),NET_WM_STATE_HIDDEN=xatom("_NET_WM_STATE_HIDDEN"),NET_WM_STATE_SKIP_TASKBAR =
		xatom("_NET_WM_STATE_SKIP_TASKBAR"),NET_WM_VISIBLE_ICON_NAME=xatom("_NET_WM_VISIBLE_ICON_NAME"),NET_WM_VISIBLE_NAME=xatom("_NET_WM_VISIBLE_NAME"),NET_WM_WINDOW_TYPE=xatom("_NET_WM_WINDOW_TYPE"),NET_WM_WINDOW_TYPE_DOCK=xatom("_NET_WM_WINDOW_TYPE_DOCK"),NET_WORKAREA=xatom("_NET_WORKAREA"),NET_WM_WINDOW_TYPE_DESKTOP=xatom("_NET_WM_WINDOW_TYPE_DESKTOP")
	};
	setprop(root,NET_SUPPORTED,XA_ATOM,32,v,NELEM(v));

	long geometry[2]= { DisplayWidth(dpy,scr),DisplayHeight(dpy,scr) };
	setprop(root,NET_DESKTOP_GEOMETRY,XA_CARDINAL,32,geometry,2);

	setndesk(ewmh_getndesktops());

	unsigned long n=0;
	unsigned long *deskp=(long unsigned int*)getprop(root,NET_CURRENT_DESKTOP,XA_CARDINAL,32,&n);
	setcurrentdesktop(0);
	if (deskp != NULL)
		{
			if (n == 1)
				{
					gotodesk(*deskp & 0xffffffffUL);
					refocus(CurrentTime);
				}
			XFree(deskp);
		}

	Window none=None;
	setprop(root,NET_ACTIVE_WINDOW,XA_WINDOW,32,&none,1);

	// Finally create the WM_CHECK window to announce our EWMH support.
	wmcheckwin=XCreateWindow(dpy,root,0,0,1,1,0,CopyFromParent,InputOnly,CopyFromParent,0,NULL);
	setprop(wmcheckwin,NET_SUPPORTING_WM_CHECK,XA_WINDOW,32,
	        &wmcheckwin,1);
	setprop(wmcheckwin,NET_WM_NAME,UTF8_STRING,8,(void*)PACKAGE_NAME,strlen(PACKAGE_NAME));
	setprop(root,NET_SUPPORTING_WM_CHECK,XA_WINDOW,32,
	        &wmcheckwin,1);
}

void ewmh_stopwm(void)
{
	XDestroyWindow(dpy,wmcheckwin);
}

void reloadwindowname(struct client *c)
{
	unsigned long n=0;
	char *name=(char*)getprop(cgetwin(c),NET_WM_NAME,UTF8_STRING,8,&n);
	csetnetwmname(c,name);
	if (name != NULL)
		XFree(name);
}

/*
 * Removes a _NET_WM_STATE property (including duplicates).
 */
void removestate(Window w,Atom state)
{
	unsigned long n=0;
	Atom *v=(Atom*)getprop(w,NET_WM_STATE,XA_ATOM,32,&n);
	unsigned long k=0;
	for (unsigned long i=0; i<n; i++)
		if (v[i] != state)
			v[k++]=v[i];
	setprop(w,NET_WM_STATE,XA_ATOM,32,v,k);
	if (v != NULL)
		XFree(v);
}

void reloadwindowstate(struct client *c)
{
	Window w=cgetwin(c);
	Bool skiptaskbar=False;
	Bool isfullscreen=False;

	unsigned long n=0;
	Atom *states=(Atom*)getprop(w,NET_WM_STATE,XA_ATOM,32,&n);
	for (unsigned int i=0; i<n; i++)
		if (states[i] == NET_WM_STATE_SKIP_TASKBAR)
			skiptaskbar=True;
		else if (states[i] == NET_WM_STATE_FULLSCREEN)
			isfullscreen=True;
		else
			removestate(w,states[i]);
	if (states != NULL)
		XFree(states);

	csetskiptaskbar(c,skiptaskbar);
	csetfull(c,isfullscreen);
}

void reloadwindowtype(struct client *c)
{
	Bool isdock=False;

	unsigned long n=0;
	Atom *types=(Atom*)getprop(cgetwin(c),NET_WM_WINDOW_TYPE,XA_ATOM,32,&n);
	if (types != NULL)
		{
			for (unsigned long i=0; i<n; i++)
				if (types[i] == NET_WM_WINDOW_TYPE_DOCK)
					isdock=True;
			XFree(types);
		}

	csetdock(c,isdock);
}

void reloadwindowdesktop(struct client *c)
{
	Window w=cgetwin(c);
	unsigned long n=0;
	long *deskp=(long*)getprop(w,NET_WM_DESKTOP,XA_CARDINAL,32,&n);
	if (deskp != NULL)
		{
			if (n == 1)
				csetdesk(c,*deskp & 0xffffffffUL);
			XFree(deskp);
		}
	else
		ewmh_notifyclientdesktop(w,cgetdesk(c));
}

void ewmh_maprequest(struct client *c)
{
	/*
	 * The order of the following calls is optimized
	 * for visual appearance.
	 */
	reloadwindowdesktop(c);
	reloadwindowstate(c);
	reloadwindowtype(c);
}

void ewmh_manage(struct client *c)
{
	Window w=cgetwin(c);

	addclient(w);

	// Remove properties that other window managers may have set.
	XDeleteProperty(dpy,w,NET_WM_VISIBLE_NAME);
	XDeleteProperty(dpy,w,NET_WM_VISIBLE_ICON_NAME);

	Atom v[] =
	{
		NET_WM_ACTION_CHANGE_DESKTOP,NET_WM_ACTION_CLOSE,NET_WM_ACTION_FULLSCREEN,
	};
	setprop(w,NET_WM_ALLOWED_ACTIONS,XA_ATOM,32,v,NELEM(v));

	/*
	 * The order of the following calls is optimized
	 * for visual appearance.
	 */
	reloadwindowdesktop(c);
	reloadwindowstate(c);
	reloadwindowname(c);
	reloadwindowtype(c);
}

/*
 * Notify change in focus. The focus change is only
 * accepted if 'old' matches the last recorded focus
 * window,or if it is None.
 *
 * The reason this function takes two arguments is to
 * avoid race conditions between FocusIn and FocusOut
 * events.
 *
 * A FocusIn handler should use None as 'old' and the
 * event window as 'new',while a FocusOut handler
 * should use the event window as 'old' and None as
 * 'new'. This way,it doesn't matter in which order
 * the events are reported.
 */
void ewmh_notifyfocus(Window old,Window mynew)
{
	// The last recorded focus window
	Window current=None;

	if (old == None || old == current)
		{
			setprop(root,NET_ACTIVE_WINDOW,XA_WINDOW,32,&mynew,1);
			current=mynew;
		}
}

void ewmh_unmanage(struct client *c)
{
	Window w=cgetwin(c);
	ewmh_notifyfocus(w,None);
	delclient(w);
	XDeleteProperty(dpy,w,NET_WM_ALLOWED_ACTIONS);
}

void ewmh_withdraw(struct client *c)
{
	Window w=cgetwin(c);
	ewmh_notifyfocus(w,None);
	delclient(w);
	XDeleteProperty(dpy,w,NET_WM_ALLOWED_ACTIONS);
	XDeleteProperty(dpy,w,NET_WM_DESKTOP);
	XDeleteProperty(dpy,w,NET_WM_STATE);
}

void ewmh_notifyrestack(void)
{
	Window *v;
	size_t n;
	getwindowstack(&v,&n);
	setprop(root,NET_CLIENT_LIST_STACKING,XA_WINDOW,32,v,n);
	free(v);
}

void ewmh_propertynotify(struct client *c,XPropertyEvent *e)
{
	if (e->atom == NET_WM_NAME)
		reloadwindowname(c);
}

Bool hasstate(Window w,Atom state)
{
	unsigned long n=0;
	Atom *v=(Atom*)getprop(w,NET_WM_STATE,XA_ATOM,32,&n);
	Bool found=False;
	for (unsigned long i=0; i<n; i++)
		if (v[i] == state)
			{
				found=True;
				break;
			}
	if (v != NULL)
		XFree(v);
	return found;
}

/*
 * Adds a _NET_WM_STATE property,unless it is already present.
 */
void addstate(Window w,Atom state)
{
	unsigned long n=0;
	Atom *old=(Atom*)getprop(w,NET_WM_STATE,XA_ATOM,32,&n);
	Bool present=False;
	for (unsigned long i=0; i<n; i++)
		if (old[i] == state)
			{
				present=True;
				break;
			}
	if (!present)
		{
			Atom *mynew=(Atom*)xmalloc((n + 1) * sizeof (Atom));
			memcpy(mynew,old,n * sizeof (Atom));
			mynew[n]=state;
			setprop(w,NET_WM_STATE,XA_ATOM,32,mynew,n + 1);
			free(mynew);
		}
	if (old != NULL)
		XFree(old);
}

void changestate(Window w,int how,Atom state)
{
	switch (how)
		{
		case NET_WM_STATE_REMOVE:
			removestate(w,state);
			break;
		case NET_WM_STATE_ADD:
			addstate(w,state);
			break;
		case NET_WM_STATE_TOGGLE:
			if (hasstate(w,state))
				removestate(w,state);
			else
				addstate(w,state);
			break;
		}
}

void ewmh_notifyfull(Window w,Bool full)
{
	if (full)
		{
			if (!hasstate(w,NET_WM_STATE_FULLSCREEN))
				addstate(w,NET_WM_STATE_FULLSCREEN);
		}
	else
		removestate(w,NET_WM_STATE_FULLSCREEN);
}

void ewmh_clientmessage(struct client *c,XClientMessageEvent *e)
{
	if (e->message_type == NET_ACTIVE_WINDOW && e->format == 32)
		{
			cpopapp(c);
			gotodesk(cgetdesk(c));
			cfocus(c,(Time)e->data.l[1]);
		}
	else if (e->message_type == NET_CLOSE_WINDOW && e->format == 32)
		{
			cdelete(c,(Time)e->data.l[0]);
		}
	else if (e->message_type == NET_WM_DESKTOP && e->format == 32)
		{
			csetappdesk(c,e->data.l[0] & 0xffffffff);
		}
	else if (e->message_type == NET_WM_STATE && e->format == 32)
		{
			int how=e->data.l[0];
			for (int i=1; i <= 2; i++)
				if (e->data.l[i] != 0)
					changestate(cgetwin(c),how,e->data.l[i]);
			reloadwindowstate(c);
		}
}

void ewmh_rootclientmessage(XClientMessageEvent *e)
{
	if (e->message_type == NET_CURRENT_DESKTOP && e->format == 32)
		{
			gotodesk(e->data.l[0]);
			refocus((Time)e->data.l[1]);
		}
	else if (e->message_type == NET_REQUEST_FRAME_EXTENTS)
		{
			struct extents ext=estimateframeextents(e->window);
			ewmh_notifyframeextents(e->window,ext);
		}
	else if (e->message_type == NET_NUMBER_OF_DESKTOPS &&
	         e->format == 32)
		{
			setndesk(e->data.l[0]);
			refocus(CurrentTime);
		}
}
