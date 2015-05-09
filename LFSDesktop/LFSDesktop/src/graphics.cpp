/******************************************************
*
*     ©keithhedger Tue  5 May 14:40:11 BST 2015
*     kdhedger68713@gmail.com
*
*     graphics.cpp
* 
******************************************************/

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>

#include "prefs.h"
#include "files.h"
#include "graphics.h"

void drawImage(char *type,char *label,int x,int y,bool mounted)
{
	ENTRY	testentry={NULL,NULL};
	ENTRY	*retentry=NULL;
	char	*tstr;

	if(mounted==true)
		{
			if(strcasecmp(type,"sata")==0)
				{
					//ENTRY sataitem={"sata",NULL};
					//ENTRY *retentry=NULL;
					testentry.key=(char*)"sata";
					testentry.data=NULL;
					retentry=hsearch(testentry,ENTER);
					if(retentry->data==NULL)
						{
						printf("XXXXXXXX\n");
							retentry->data=malloc(sizeof(diskIconStruct));
							tstr=pathToIcon("harddisk");
							makeImage((char*)tstr,(char*)"sata",SATA,(diskIconStruct*)(retentry->data));
							free(tstr);
						}
					//retentry=hsearch(testentry,FIND);
					XSetClipMask(display,gc,diskIconsPixmapMask[SATA][0]);
					//XSetClipMask(display,gc,((diskIconStruct*)retentry->data)->mask);
					XSetClipOrigin(display,gc,x,y);
					//XCopyArea(display,diskIconsPixmap[SATA][0],drawOnThis,gc,0,0,iconSize,iconSize,x,y);
				//	printf("+++%i+++\n",((diskIconStruct*)retentry->data)->pixmap);
				//	printf(">>%s<<\n",((diskIconStruct*)retentry->data)->name);
				//long unsigned int *xx=(long unsigned int*)(((diskIconStruct*)retentry->data)->pixmap);
					//Pixmap pm=((diskIconStruct*)retentry->data)->pixmap;
					//printf(">>%i<<\n",pm);
					//pm=diskIconsPixmap[SATA][0];
					//printf("--%i--\n",pm);
					
	XCopyArea(display,((diskIconStruct*)retentry->data)->pixmap,drawOnThis,gc,0,0,iconSize,iconSize,x,y);
				}
			if(strcasecmp(type,"usb")==0)
				{
					XSetClipMask(display,gc,diskIconsPixmapMask[USB][0]);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,diskIconsPixmap[USB][0],drawOnThis,gc,0,0,iconSize,iconSize,x,y);
				}
			if(strcasecmp(type,"cdrom")==0)
				{
					XSetClipMask(display,gc,diskIconsPixmapMask[CDROM][0]);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,diskIconsPixmap[CDROM][0],drawOnThis,gc,0,0,iconSize,iconSize,x,y);
				}
			}
		else
			{
				if(strcasecmp(type,"satax")==0)
					{
						XSetClipMask(display,gc,diskIconsPixmapMask[SATA][1]);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskIconsPixmap[SATA][1],drawOnThis,gc,0,0,iconSize,iconSize,x,y);
					}
				if(strcasecmp(type,"usb")==0)
					{
						XSetClipMask(display,gc,diskIconsPixmapMask[USB][1]);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskIconsPixmap[USB][1],drawOnThis,gc,0,0,iconSize,iconSize,x,y);
					}
				if(strcasecmp(type,"cdrom")==0)
					{
						XSetClipMask(display,gc,diskIconsPixmapMask[CDROM][1]);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskIconsPixmap[CDROM][1],drawOnThis,gc,0,0,iconSize,iconSize,x,y);
					}
			}
}