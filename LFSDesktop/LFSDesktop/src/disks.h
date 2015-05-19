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
#define BUFFERSIZE 256


struct disks
{
	char		*uuid;
	char		*label;
	int			x;
	int			y;
	char		*mountpoint;
	bool		mounted;
	char		*dev;
	char		*sysname;
	bool		ignore;
	bool		dvd;
	bool		cdrom;
	bool		usb;
	bool		harddisk;
};

extern disks	*attached;
extern int		numberOfDisksAttached;

void mountDisk(int x, int y);
void createDiskInfo(void);
void getDiskList(args *diskdata);
void scanForMountableDisks(void);
void drawIcons(void);

#endif
