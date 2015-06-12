
#ifndef _EWMH_
#define _EWMH_

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

#endif
