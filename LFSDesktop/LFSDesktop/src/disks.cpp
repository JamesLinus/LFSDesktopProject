/******************************************************
*
*     ©keithhedger Tue 12 May 10:46:15 BST 2015
*     kdhedger68713@gmail.com
*
*     disks.cpp
*
******************************************************/

#include <X11/extensions/shape.h>

#include <stdio.h>
#include <stdlib.h>

#include <libudev.h>

#include "config.h"

#include "prefs.h"
#include "files.h"
#include "disks.h"

disks		*attached=NULL;
saveDisks	*saved=NULL;
int			numberOfDisksAttached=-1000;
const char	*iconDiskType[]= {"harddisk","harddisk-usb","dev-cdrom","dev-dvd","media-removable","multimedia-player","flash","user-home"};

void mountDisk(int x, int y)
{
	char	*command;

	if(isDisk==false)
		{
			asprintf(&command,"xdg-open \"%s\"",fileInfoPtr[foundDiskNumber].path);
			system(command);
			free(command);
			return;
		}
	else
		{
#ifdef _USESUIDHELPER_
			asprintf(&command,"%s %s /media/%s",HELPERAPP,attached[foundDiskNumber].uuid,attached[foundDiskNumber].label);
#else
			asprintf(&command,"udevil mount `findfs UUID=%s`",attached[foundDiskNumber].uuid);
#endif
			system(command);
			free(command);
			asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|xargs xdg-open",attached[foundDiskNumber].uuid);
			system(command);
			free(command);
		}
}

int getUSBData(const char *ptr)
{
	if(ptr!=NULL)
		{
			if(strcasestr(ptr,"apple")!=NULL)
				return(IPOD);
			if(strcasestr(ptr,"sandisk")!=NULL)
				return(CARD);
			if(strcasestr(ptr,"generic")!=NULL)
				return(CARD);
		}
	return(USB);
}

void deleteDiskInfo(void)
{
	for(int j=0; j<numberOfDisksAttached; j++)
		{
			if(attached[j].uuid!=NULL)
				free(attached[j].uuid);
			if(attached[j].label!=NULL)
				free(attached[j].label);
			if(attached[j].mountpoint!=NULL)
				free(attached[j].mountpoint);
			if(attached[j].dev!=NULL)
				free(attached[j].dev);
			if(attached[j].sysname!=NULL)
				free(attached[j].sysname);
		}
	free(attached);
}

bool getDiskPos(char* uuid,int* xptr,int* yptr)
{
	for (int j=0;j<savedFileCount;j++)
		{
			if(strcmp(uuid,saved[j].uuid)==0)
				{
					*xptr=saved[j].x;
					*yptr=saved[j].y;
					return(true);
				}
		}
	return(false);
}

void scanForMountableDisks(void)
{
	struct udev *udev;
	char		buffer[BUFFERSIZE];
	FILE		*fp;
	int			numofdisks=0;
	const char	*ptr;
	int			xpos=0,ypos=0;
	udev_device *usbdev;
	udev_device	*thedev;

	/* Create the udev object */
	udev=udev_new();
	if (!udev)
		{
			printf("Can't create udev\n");
			exit(1);
		}

	fp=popen(READFROM "|wc -l","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,BUFFERSIZE,fp);
			numofdisks=atoi(buffer);
			pclose(fp);
		}

	if(numberOfDisksAttached<numofdisks)
		{
			deleteDiskInfo();
			attached=(disks*)calloc(numofdisks,sizeof(disks));
			numberOfDisksAttached=numofdisks;
		}

	fp=popen(READFROM,"r");
	if(fp!=NULL)
		{
			for(int j=0;j<numberOfDisksAttached;j++)
				{
					buffer[0]=0;
					fgets(buffer,BUFFERSIZE,fp);
					buffer[strlen(buffer)-1]=0;
					thedev=udev_device_new_from_subsystem_sysname(udev,"block",buffer);
					if(thedev!=NULL)
						{
							attached[j].ignore=true;
							if(udev_device_get_property_value(thedev,"ID_FS_UUID")!=NULL)
								{
//get uuid
									attached[j].uuid=strdup(udev_device_get_property_value(thedev,"ID_FS_UUID"));
//partname
									attached[j].sysname=strdup(buffer);
									asprintf(&attached[j].dev,"/dev/%s",buffer);
//is a file system
									if(strcmp(udev_device_get_property_value(thedev,"ID_FS_USAGE"),"filesystem")==0)
										{
											attached[j].ignore=false;
											ptr=udev_device_get_property_value(thedev,"ID_FS_LABEL");
											if(ptr!=NULL)
												attached[j].label=strdup(ptr);
											else
												{
													ptr=udev_device_get_property_value(thedev,"ID_SERIAL");
													attached[j].label=strndup(ptr,16);
												}

											if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_DVD")!=NULL)
												{
													attached[j].dvd=true;
													attached[j].type=DVD;
												}

											if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_CD")!=NULL)
												{
													attached[j].dvd=true;
													attached[j].type=CDROM;
												}

											usbdev=udev_device_get_parent_with_subsystem_devtype(thedev,"usb","usb_device");
											if(usbdev!=NULL)
												{
													attached[j].usb=true;
													attached[j].type=getUSBData(udev_device_get_property_value(thedev,"ID_VENDOR"));
													if(udev_device_get_property_value(thedev,"ID_DRIVE_THUMB")!=NULL)
														attached[j].type=STICK;
												}

											if(getDiskPos(attached[j].uuid,&xpos,&ypos))
												{
													attached[j].x=xpos;
													attached[j].y=ypos;
												}
											else
												{
													xpos=0;
													ypos=0;
													while(xySlot[xpos][ypos]!=0)
														{
															ypos++;
															if(ypos>MAXGRIDY)
																{
																	ypos=0;
																	xpos++;
																}
														}
													attached[j].x=xpos;
													attached[j].y=ypos;
													sprintf(buffer,"%s/%s",diskInfoPath,attached[j].uuid);
													saveInfofile(DISKFOLDER,attached[j].label,NULL,NULL,attached[j].uuid,(char*)iconDiskType[attached[j].type],attached[j].x,attached[j].y);
													xySlot[xpos][ypos]=1;
													getSavedDiskData();
												}
											xySlot[xpos][ypos]=1;
										}
								}
							udev_device_unref(thedev);
						}
				}
			pclose(fp);
		}
	udev_unref(udev);
}

