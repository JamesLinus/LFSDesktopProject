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

#include "prefs.h"
#include "files.h"

void mountDisk(int x, int y)
{
	FILE	*fp;
	FILE	*fd;
	char	*command;
	char	line[2048];
	int		dx,dy;
	char	label[256];
	char	uuid[256];
	char	dataline[256];

	asprintf(&command,"find %s -mindepth 1",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					loadVarsFromFile(line,diskData);
					if(strlen(line)>0)
						{
							if(strlen(diskUUID)>1)
								{
									if((x>=(diskXPos*gridSize+gridBorder))&&(x<=(diskXPos*gridSize+gridBorder)+iconSize)&&(y>=(diskYPos*gridSize+gridBorder))&&(y<=(diskYPos*gridSize+gridBorder)+iconSize))
										{
											asprintf(&command,"udevil mount `findfs UUID=%s`",diskUUID);
											system(command);
											free(command);
											asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|xargs xdg-open",diskUUID);
											system(command);
											free(command);
											
											pclose(fp);
											return;
										}
								}
						}
				}
			pclose(fp);
		}
}

void createDiskInfo(void)
{
	FILE	*fp;
	FILE	*fd;

	char	*command;
	char	line[1024];
	char	*devname;
	char	*label;
	char	*uuid;
	char	*type;
	char	*disktype;
	char	*diskfilepath;
	int		posx;
	int		posy;

	posx=0;
	posy=0;

	asprintf(&command,"lsblk -n --output=TYPE,NAME,UUID,LABEL,TRAN -lps");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					devname=NULL;
					label=NULL;
					uuid=NULL;
					type=NULL;
					disktype=NULL;
					sscanf(line,"%as %as %as %a[^\n^\t^ ]s",&disktype,&devname,&uuid,&label);
					fgets(line,1024,fp);
					sscanf(line,"%*s %*s %a[^\n]s",&type);
					if((disktype!=NULL) && (strcmp(disktype,"rom")==0))
						{
							if(type!=NULL)
								{
									free(type);
									asprintf(&type,"cdrom");
								}
							if(label==NULL)
								asprintf(&label,"CDROM");
						}
					if((type!=NULL) && (strcmp(type,"sata")==0))
						{
							free(type);
							asprintf(&type,"harddisk");
						}

					if((type!=NULL) && (strcmp(type,"usb")==0))
						{
							free(type);
							asprintf(&type,"harddisk-usb");
						}

					if(label==NULL)
						asprintf(&label,"IGNOREDISK");

					asprintf(&diskfilepath,"%s/%s",diskInfoPath,uuid);

					if((uuid!=NULL) && (strlen(uuid)>1))
						{
							fd=fopen(diskfilepath,"r");
							if(fd==NULL)
								{
									fd=fopen(diskfilepath,"w");
									if(fd==NULL)
										{
											printf("Can't open disk folder ...\n");
											return;
										}

									while(xySlot[posx][posy]!=0)
										{
											posy++;
											if(posy>MAXGRIDY)
												{
													posy=0;
													posx++;
												}
										}
									makeDiskInfofile(diskfilepath,label,uuid,posx,posy,type);
									xySlot[posx][posy]=1;
									fclose(fd);
								}
							free(diskfilepath);
							free(disktype);
						}
					line[0]=0;
				}
			pclose(fp);
		}
	free(command);
}

void getDiskList(args *diskdata)
{
	FILE		*fp;
	FILE		*fd;
	char		*diskfilepath;
	char		*command;
	char		line[2048];
	int			diskx;
	int			disky;
	char		dataline[256];
	char		uuid[256];
	char		label[256];

	XColor		colour;
	int			fontheight;
	int			stringwidth;

	int			boxx,boxy,boxw,boxh;
	XRectangle	rect;

	XDestroyRegion(rg);
	rg=XCreateRegion();

	asprintf(&command,"lsblk -n --output=UUID -lpds");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					if(strlen(line)>0)
						{
							asprintf(&diskfilepath,"%s/%s",diskInfoPath,line);
							if(fileExists(diskfilepath)!=0)
								createDiskInfo();

							loadVarsFromFile(diskfilepath,diskdata);

							char *dname,*duid,*dtype;

							args ag;
							ag=diskdata[2];
							diskx=(int)(*(int*)(ag.data));

							ag=diskdata[3];
							disky=(int)(*(int*)(ag.data));

							xySlot[diskx][disky]=1;
							diskx=diskx*gridSize+gridBorder;
							disky=disky*gridSize+gridBorder;

							ag=diskdata[0];
							dname=strdup((char*)(*((char**)ag.data)));
							ag=diskdata[1];
							duid=strdup((char*)(*((char**)ag.data)));
							ag=diskdata[4];
							dtype=strdup((char*)(*((char**)ag.data)));

							if(strcmp(dname,"IGNOREDISK")!=0)
								{
									FILE	*tp;
									char	*com;

									asprintf(&com,"findmnt -fn $(findfs UUID=%s)",duid);
									line[0]=0;
									tp=popen(com,"r");
									free(com);
									fgets(line,2048,tp);
									pclose(tp);


									if(strlen(line)>0)
										drawImage(dtype,dname,diskx,disky,true);
									else
										drawImage(dtype,dname,diskx,disky,false);

									rect.x=diskx;
									rect.y=disky;
									rect.width=iconSize;
									rect.height=iconSize;
									XUnionRectWithRegion(&rect,rg, rg);

									XSetClipMask(display,gc,0);

									fontheight=labelFont->ascent+labelFont->descent;
									stringwidth=XTextWidth(labelFont,label,strlen(dname));

									boxx=diskx+(iconSize/2)-(stringwidth/2)-1;
									boxy=disky+iconSize+1;
									boxw=stringwidth+2;
									boxh=fontheight-2;

									XSetForeground(display,gc,labelBackground);
									XSetFillStyle(display,gc,FillSolid);
									XFillRectangle(display,drawOnThis,gc,boxx,disky+iconSize,boxw,boxh);

									XSetForeground(display,labelGC,labelForeground);
									XSetBackground(display,labelGC,labelBackground);

									XDrawString(display,drawOnThis,labelGC,boxx+1,disky+iconSize+boxh-1,dname,strlen(dname));
								}
						}
				}
			fclose(fp);
		}
	free(command);
	XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);
}
