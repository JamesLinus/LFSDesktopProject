/******************************************************
*
*     ©keithhedger Fri 22 May 14:47:56 BST 2015
*     kdhedger68713@gmail.com
*
*     globals.h
* 
******************************************************/

#ifndef _GLOBALS_
#define _GLOBALS_

#define DEBUGFUNC(x,...) errLine=__LINE__,errFile=__FILE__,errFunc=__func__;debugFunc((const char*)x,__VA_ARGS__)
#define debugfunc  DEBUGFUNC

#define RESERVED 2
#define MAXBUFFER 512

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};
enum {CACHEFOLDER,DISKFOLDER};
enum {HOMEDATA=0,ROOTDATA};
enum {BUTTONMOUNT=1,BUTTONUNMOUNT,BUTTONEJECT,BUTTONOPEN,BUTTONADDICON,BUTTONREMOVEICON};

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

struct deskIcons
{
	char			*label;
	char			*uuid;
	char			*dev;
	char			*mountpoint;
	char			*mime;
	char			*icon;
	int				x;
	int				y;
	bool			mounted;
	char			*partname;
	bool			ignore;
	bool			dvd;
	bool			cdrom;
	bool			usb;
	bool			file;
	int				iconhint;
	bool			installed;
	bool			customicon;
};

extern int			deskIconsCnt;
extern int			deskIconsMaxCnt;
extern deskIcons	*deskIconsArray;

extern int			errLine;
extern const char	*errFile;
extern const char	*errFunc;

extern bool shapeset;



//save/load file
extern args		globalFileData[];
//global file data
extern char		*fileDiskLabel;
extern char		*fileDiskMime;
extern char		*fileDiskPath;
extern char		*fileDiskUUID;
extern char		*fileDiskType;
extern char		*fileCustomIcon;
extern int		fileDiskXPos;
extern int		fileDiskYPos;
extern bool		fileGotCustomIcon;

extern int		foundDiskNumber;
extern bool		isDisk;
extern bool		debugDeskFlag;

extern int		xCnt;
extern int		yCnt;

extern char		*rootDev;

#endif
