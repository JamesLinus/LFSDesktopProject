/******************************************************
*
*     ©keithhedger Sun  3 May 19:46:41 BST 2015
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#ifndef _PREFS_
#define _PREFS_

#define REFRESHRATE 2
#define MAXGRIDY 4
#define GRIDBORDER 32
#define ICONSIZE 48
#define GRIDSIZE (ICONSIZE+32)

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

extern args		diskData[];
extern char		*diskName;
extern char		*diskUUID;
extern int		diskXPos;
extern int		diskYPos;

void saveVarsToFile(const char* filepath,args* dataptr);
void loadVarsFromFile(char* filepath,args* dataptr);

#endif
