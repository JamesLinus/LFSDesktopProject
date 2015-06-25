
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#ifndef _EWMH_
#define _EWMH_

extern Atom NET_ACTIVE_WINDOW;
extern Atom NET_CLIENT_LIST;
extern Atom NET_CLIENT_LIST_STACKING;
extern Atom NET_CLOSE_WINDOW;
extern Atom NET_CURRENT_DESKTOP;
extern Atom NET_DESKTOP_GEOMETRY;
extern Atom NET_DESKTOP_VIEWPORT;
extern Atom NET_FRAME_EXTENTS;
extern Atom NET_NUMBER_OF_DESKTOPS;
extern Atom NET_REQUEST_FRAME_EXTENTS;
extern Atom NET_SUPPORTED;
extern Atom NET_SUPPORTING_WM_CHECK;
extern Atom NET_WM_ACTION_CHANGE_DESKTOP;
extern Atom NET_WM_ACTION_CLOSE;
extern Atom NET_WM_ACTION_FULLSCREEN;
extern Atom NET_WM_ACTION_MINIMIZE;
extern Atom NET_WM_ALLOWED_ACTIONS;
extern Atom NET_WM_DESKTOP;
extern Atom NET_WM_ICON_NAME;
extern Atom NET_WM_NAME;
extern Atom NET_WM_STATE;
extern Atom NET_WM_STATE_ABOVE;
extern Atom NET_WM_STATE_BELOW;
extern Atom NET_WM_STATE_FULLSCREEN;
extern Atom NET_WM_STATE_HIDDEN;
extern Atom NET_WM_STATE_SKIP_TASKBAR;
extern Atom NET_WM_VISIBLE_ICON_NAME;
extern Atom NET_WM_VISIBLE_NAME;
extern Atom NET_WM_WINDOW_TYPE;
extern Atom NET_WM_WINDOW_TYPE_DOCK;
extern Atom NET_WORKAREA;
extern Atom NET_WM_WINDOW_TYPE_DESKTOP;
extern Atom NET_WM_STATE_STICKY;

void ewmh_notifyndesk(unsigned long);
void ewmh_notifycurdesk(unsigned long);
void ewmh_notifyclientdesktop(Window,unsigned long);
void ewmh_notifyframeextents(Window,struct extents);
void ewmh_startwm(void);
void ewmh_stopwm(void);
void ewmh_maprequest(struct client *);
void ewmh_notifyfull(Window,Bool);
void ewmh_manage(struct client *);
void ewmh_unmanage(struct client *);
void ewmh_withdraw(struct client *);
void ewmh_notifyfocus(Window,Window);
void ewmh_notifyrestack(void);
void ewmh_propertynotify(struct client *,XPropertyEvent *);
void ewmh_clientmessage(struct client *,XClientMessageEvent *);
void ewmh_rootclientmessage(XClientMessageEvent *);
void reloadwindowdesktop(struct client *c);

#endif
