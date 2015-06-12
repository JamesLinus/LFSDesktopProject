
#ifndef _DRAGGER_
#define _DRAGGER_

struct dragger *dcreate(Window,int,int,int,int,int,Cursor,void (*)(void *,int,int,unsigned long,Time),void *);
void ddestroy(struct dragger *);

#endif
