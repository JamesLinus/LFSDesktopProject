
#ifndef _X11FONT_
#define _X11FONT_

struct fontcolor *ftloadcolor(const char *);
void ftfreecolor(struct fontcolor *);
struct font *ftload(const char *);
void ftfree(struct font *);
void ftdrawstring(Drawable,struct font *,struct fontcolor *,int,int,const char *);
void ftdrawstring_utf8(Drawable,struct font *,struct fontcolor *,int,int,const char *);
int fttextwidth(struct font *,const char *);
int fttextwidth_utf8(struct font *,const char *);

#endif