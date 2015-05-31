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

enum {HARDDIVE=0,USB,CDROM,DVD,STICK,IPOD,CARD,HOME,COMPUTER};

extern const char	*iconDiskType[];

void mountDisk(int what);
void fillDesk(void);

#endif
