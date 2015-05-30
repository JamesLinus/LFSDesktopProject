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

#define DEBUGSTR(x) errLine=__LINE__,errFile=__FILE__,errFunc=__func__;printString((char*)x)
#define DEBUGVAL(x) errLine=__LINE__,errFile=__FILE__,errFunc=__func__;printInt((int)x)
#define debugstr(x) DEBUGSTR(x)

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};
enum {CACHEFOLDER,DISKFOLDER};

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

/*
struct fileInfo
{
	char		*label;
	char		*mime;
	char		*path;
	char		*icon;
	char		*uuid;
	char		*type;
	int			x;
	int			y;
};
*/

struct deskIcons
{
	char			*label;
	char			*uuid;
	char			*dev;
	char			*mountpoint;
	char			*mime;
	int				x;
	int				y;
	bool			mounted;
	char			*partname;
	bool			ignore;
	bool			dvd;
	bool			cdrom;
	bool			usb;
	//bool			harddisk;
	bool			file;
	int				iconhint;
	bool			installed;
};

extern int			deskIconsCnt;
extern int			deskIconsMaxCnt;
extern deskIcons	*deskIconsArray;

extern int			errLine;
extern const char	*errFile;
extern const char	*errFunc;

//file icon data
//extern fileInfo	*disksDataPtr;
//extern int		desktopFilesCnt;
//extern int		desktopFilesCntMax;

//save/load file
extern args		globalFileData[];
//global file data
extern char		*fileDiskLabel;
extern char		*fileDiskMime;
extern char		*fileDiskPath;
extern char		*fileDiskUUID;
extern char		*fileDiskType;
extern int		fileDiskXPos;
extern int		fileDiskYPos;

extern int		foundDiskNumber;
extern bool		isDisk;
extern bool		debugDeskFlag;
//extern bool		firstRun;

extern int		xCnt;
extern int		yCnt;

#endif
