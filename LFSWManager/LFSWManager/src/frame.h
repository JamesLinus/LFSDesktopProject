
#ifndef _FRAME_
#define _FRAME_

struct frame *fcreate(struct client *);
void fdestroy(struct frame *);
void fupdate(struct frame *);
Window fgetwin(struct frame *);
struct geometry fgetgeom(struct frame *);
struct extents estimateframeextents(Window);

#endif