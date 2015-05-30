/******************************************************
*
*     ©keithhedger Tue 12 May 10:46:15 BST 2015
*     kdhedger68713@gmail.com
*
*     disks.h
* 
******************************************************/

#ifndef _DISKS_
#define _DISKS_

#define READFROM "ls -1 /sys/class/block"

enum {HARDDIVE=0,USB,CDROM,DVD,STICK,IPOD,CARD,HOME};

/*
//struct disks
{
	char			*uuid;
	char			*label;
	int				x;
	int				y;
	char			*mountpoint;
	bool			mounted;
	char			*dev;
	char			*sysname;
	bool			ignore;
	bool			dvd;
	bool			cdrom;
	bool			usb;
	bool			harddisk;
	int				type;
};

struct	saveDisks
{
	char			*uuid;
	int				x;
	int				y;
};
*/
//extern disks		*attached;
//extern saveDisks	*saved;
extern int			numberOfDisksAttached;
extern const char	*iconDiskType[];

void mountDisk(int what);
//void scanForMountableDisks(void);
void fillDesk(void);

#endif
