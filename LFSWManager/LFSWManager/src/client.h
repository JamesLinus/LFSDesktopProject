
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#ifndef _CLIENT_
#define _CLIENT_

struct client *manage(Window);
extern Desk curdesk;
extern Desk ndesk;

void manageall(void);
void unmanageall(void);
void cpopapp(struct client *);
void cpushapp(struct client *);
void cdelete(struct client *,Time);
void csetdesk(struct client *,Desk);
void csetappdesk(struct client *,Desk);
Desk cgetdesk(struct client *);
void csetdock(struct client *,Bool);
void csetfull(struct client *,Bool);
void csetundecorated(struct client *,Bool);
void csetappfollowdesk(struct client *,Bool);
struct client *getfocus(void);
void cfocus(struct client *,Time);
struct client *refocus(Time);
Bool cistask(struct client *);
Bool cisvisible(struct client *);
void setndesk(Desk);
void gotodesk(Desk);
void getwindowstack(Window **,size_t *);
int namewidth(struct font *,struct client *);
void drawname(Drawable,struct font *,struct fontcolor *,int,int,struct client *);
Bool chaswmproto(struct client *,Atom);
void restack(void);
int cgetgrav(struct client *);
struct geometry cgetgeom(struct client *);
void csetgeom(struct client *,struct geometry);
void csendconf(struct client *);
Bool chasfocus(struct client *);
Window cgetwin(struct client *);
void csetnetwmname(struct client *,const char *);
void cignoreunmap(struct client *);
Bool cismapped(struct client *);
Bool cisurgent(struct client *);
void csetskiptaskbar(struct client *,Bool);
void chintsize(struct client *,int,int,int *,int *);

#endif
