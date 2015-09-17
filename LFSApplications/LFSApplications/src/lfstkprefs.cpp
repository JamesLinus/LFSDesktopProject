/*
 *
 * ©K. D. Hedger. Wed 16 Sep 11:51:14 BST 2015 kdhedger68713@gmail.com

 * This file (lfstkprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include <LFSTKMenuButton.h>
#include <LFSTKLineEdit.h>
#include <LFSTKLabel.h>
#include <LFSTKLib.h>

enum {EXIT=0,APPLY,PRINT,BUTTONLABEL,NULLLABEL,MENUITEMLABEL,NULLLABEL2,FONTLABEL,FONTBOX,MENUFONTLABEL,MENUFONTBOX,WNORMAL,EWNORMAL,AUTOTEXT,EAUTOTEXT,

BNORMAL,ENORMAL,BPRELIGHT,EPRELIGHT,BACTIVE,EACTIVE,BINACTIVE,EINACTIVE,MENUNORM,EMENUNORM,MENUPRELITE,EMENUPRELITE,MENUACTIVE,EMENUACTIVE,MENUINACTIVE,EMENUINACTIVE,

FONTNORMCOL,EFONTNORMCOL,FONTPRELIRECOL,EFONTPRELITECOL,FONTACTIVECOL,EFONTACTIVECOL,FONTINACTIVECOL,EFONTINACTIVECOL,
ITEMFONTNORMCOL,EITEMFONTNORMCOL,ITEMFONTPRELIRECOL,EITEMFONTPRELITECOL,ITEMFONTACTIVECOL,EITEMFONTACTIVECOL,ITEMFONTINACTIVECOL,EITEMFONTINACTIVECOL,
NOMORE};

//enum {LAUTOTEXT=0,NOMORELABELS};

const char	*buttonnames[]={"Button Normal","Button Prelight","Button Active","Button Inactive"};
const char	*menunames[]={"Menu Normal","Menu Prelight","Menu Active","Menu Inactive"};
const char	*fontnames[]={"Font Normal","Font Prelight","Font Active","Font Inactive"};
const char	*itemfontnames[]={"Menu Font Norm","Menu Font Prel","Menu Font Activ","Menu Font Inact"};
//const char	*labels[]={"Auto Colour"};

bool					mainloop=false;
LFSTK_windowClass		*wc;
LFSTK_gadgetClass		*bc[NOMORE]={NULL,};
//LFSTK_labelClass		*lb[]=NOMORELABELS]={NULL,};

void setVars(void)
{
				wc->LFSTK_setWindowColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
				wc->globalLib->LFSTK_setGlobalString(NORMALCOLOUR,TYPEWINDOW,wc->windowColourNames[NORMALCOLOUR].name);
				wc->LFSTK_clearWindow();

				int butonnum=ENORMAL;
//set global strings
				wc->globalLib->LFSTK_setAutoLabelColour(atoi(static_cast<LFSTK_lineEditClass*>(bc[EAUTOTEXT])->LFSTK_getBuffer()->c_str()));
				wc->autoLabelColour=wc->globalLib->LFSTK_getAutoLabelColour();
				for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
					{
						wc->globalLib->LFSTK_setGlobalString(j,TYPEBUTTON,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum])->LFSTK_getBuffer()->c_str()));
						wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEM,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum+MENUNORM-BNORMAL])->LFSTK_getBuffer()->c_str()));
						wc->globalLib->LFSTK_setGlobalString(j,TYPEFONTCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum+FONTNORMCOL-BNORMAL])->LFSTK_getBuffer()->c_str()));
						wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEMFONTCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum+ITEMFONTNORMCOL-BNORMAL])->LFSTK_getBuffer()->c_str()));
						butonnum+=2;
					}
				wc->globalLib->LFSTK_setGlobalString(-1,TYPEFONT,(char*)(static_cast<LFSTK_lineEditClass*>(bc[FONTBOX])->LFSTK_getBuffer()->c_str()));
				wc->globalLib->LFSTK_setGlobalString(-1,TYPEMENUITEMFONT,(char*)(static_cast<LFSTK_lineEditClass*>(bc[MENUFONTBOX])->LFSTK_getBuffer()->c_str()));
				wc->globalLib->LFSTK_setAutoLabelColour(atoi(static_cast<LFSTK_lineEditClass*>(bc[EAUTOTEXT])->LFSTK_getBuffer()->c_str()));
				wc->autoLabelColour=wc->globalLib->LFSTK_getAutoLabelColour();

//buttons
				for(int j=BNORMAL;j<MENUNORM;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[FONTBOX])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setLabelAutoColour(true);
					}
//menu items
				for(int j=MENUNORM;j<FONTNORMCOL;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[MENUFONTBOX])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setLabelAutoColour(true);
					}

//font button colours
				int x=ENORMAL-2;
				int state=0;
				for(int j=FONTNORMCOL;j<ITEMFONTNORMCOL;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[x+=2])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR));
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[FONTBOX])->LFSTK_getBuffer()->c_str()));
						state++;
					}

//fonmt menu item colours
				x=EMENUNORM-2;
				state=0;
				for(int j=ITEMFONTNORMCOL;j<NOMORE;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[x+=2])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR));
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[MENUFONTBOX])->LFSTK_getBuffer()->c_str()));
						state++;
					}

//set label backgrounds to window back ground
				for(int j=BUTTONLABEL;j<BNORMAL;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setLabelAutoColour(wc->autoLabelColour);
					}

				for(int j=BUTTONLABEL;j<NOMORE;j++)
					if(bc[j]!=NULL)
						bc[j]->LFSTK_clearWindow();
}

bool callback(void *p,void* ud)
{
	char *env;

	if((long)ud==EXIT)
		{
			setVars();
			wc->globalLib->LFSTK_saveVarsToFile("-",wc->globalLib->LFSTK_getTKArgs());
			printf("\n");
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case PRINT:
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile("-",wc->globalLib->LFSTK_getTKArgs());
				printf("\n");
				break;

			case APPLY:
				wc->LFSTK_clearWindow();
				setVars();
				asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
				wc->globalLib->LFSTK_saveVarsToFile(env,wc->globalLib->LFSTK_getTKArgs());
				free(env);
				break;
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bwidth=96;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Tool Kit Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();
	bc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	bc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	bc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	bc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	bc[PRINT]=new LFSTK_buttonClass(wc,"Print",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	bc[PRINT]->LFSTK_setCallBack(NULL,callback,(void*)PRINT);

//labels
	sx=col1;
	sy=10;

	bc[BUTTONLABEL]=new LFSTK_labelClass(wc,"Normal Buttons",sx,sy,bwidth,24,NorthWestGravity);

	sx=col2;
	bc[MENUITEMLABEL]=new LFSTK_labelClass(wc,"Menu Items",sx,sy,bwidth,24,NorthWestGravity);
	sy+=vspacing;

//buttons
	sx=col1;
//	sy=10;
	int state=0;	
	for (int j=BNORMAL;j<EINACTIVE+1;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,buttonnames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setColourName(state,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON));
			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON),sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
			state++;
		}

//menu items
	sx=col2;
	sy=10;
	sy+=vspacing;
	state=0;	
	for (int j=MENUNORM;j<EMENUINACTIVE+1;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,menunames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setColourName(state,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON));

			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEM),sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col2;
			state++;
		}

//button font
	sx=col1;
	bc[FONTLABEL]=new LFSTK_buttonClass(wc,"Font",sx,sy,bwidth,24,NorthWestGravity);
	static_cast<LFSTK_buttonClass*>(bc[FONTLABEL])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[FONTBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT),sx,sy-1,(bwidth*2)+spacing+20,24,NorthWestGravity);

//mennu item font
	sy+=vspacing;
	sx=col1;
	bc[MENUFONTLABEL]=new LFSTK_buttonClass(wc,"Menu Item Font",sx,sy,bwidth,24,NorthWestGravity);
	static_cast<LFSTK_buttonClass*>(bc[MENUFONTLABEL])->LFSTK_setStyle(FLATBUTTON);
	sx+=spacing;
	bc[MENUFONTBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT),sx,sy-1,(bwidth*2)+spacing+20,24,NorthWestGravity);
	sy+=vspacing;

//font colours
	sx=col1;
	state=0;
	int vhold=sy;
	for (int j=FONTNORMCOL;j<ITEMFONTNORMCOL;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,fontnames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setLabelAutoColour(false);

			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);

			bc[j]->LFSTK_gadgetClass::LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR));
			sy+=vspacing;
			sx=col1;
			state++;
		}

//menu item font colours
	sx=col2;
	state=0;
	sy=vhold;
	for (int j=ITEMFONTNORMCOL;j<NOMORE;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,itemfontnames[state],sx,sy,bwidth,24,NorthWestGravity);
			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_gadgetClass::LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR));
			bc[j]->LFSTK_setLabelAutoColour(false);
			sy+=vspacing;
			sx=col2;
			state++;
		}

//window
	sx=col1;
	bc[WNORMAL]=new LFSTK_buttonClass(wc,"Window Colour",sx,sy,bwidth,24,NorthWestGravity);
	bc[WNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
	static_cast<LFSTK_buttonClass*>(bc[WNORMAL])->LFSTK_setStyle(FLATBUTTON);

	sx+=spacing;
	bc[EWNORMAL]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EWNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));

//auto text colour
	sx=col2;
	bc[AUTOTEXT]=new LFSTK_labelClass(wc,"Auto Colour",sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	char	*buffer;
	asprintf(&buffer,"%i",wc->autoLabelColour);
	bc[EAUTOTEXT]=new LFSTK_lineEditClass(wc,buffer,sx,sy-1,bwidth,24,NorthWestGravity);
//	bc[EAUTOTEXT]=new LFSTK_lineEditClass(wc,"wc->autoLabelColour",sx,sy-1,bwidth,24,NorthWestGravity);
//	bc[EAUTOTEXT]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
	
	sy+=(vspacing*2);
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	
	for(int j=BUTTONLABEL;j<NOMORE;j+=2)
		bc[j]->LFSTK_setActive(false);

	printf("Current Settings:\n\n");
	callback(NULL,(void*)PRINT);
	printf("\n\n");

	mainloop=true;
	while(mainloop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);

			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
						break;
					case Expose:
						wc->LFSTK_setActive(true);
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
					default:
						break;
				}
		}

	delete wc;
	for(int j=0;j<NOMORE;j++)
		if(bc[j]!=NULL)
			delete bc[j];
	return 0;
}