
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#ifndef _FRAME_
#define _FRAME_

struct frame *fcreate(struct client *);
void fdestroy(struct frame *);
void fupdate(struct frame *);
Window fgetwin(struct frame *);
struct geometry fgetgeom(struct frame *);
struct extents estimateframeextents(Window);

#endif