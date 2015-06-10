
//©keithhedger Mon 8 Jun 13:41:20 BST 2015 kdhedger68713@gmail.com

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xaw/Cardinals.h>
#include <Xm/Xm.h>
#include <X11/Xaw/Dialog.h>

String			fallback_resources[]=
{
	(char*)"appmenu*background:		grey",
	(char*)"*dialog.*.width:		48",
	(char*)"*dialog*value.width:	240",
	(char*)"*dialog*label.width:	240",
	NULL,
};

bool			mainloop=false;
bool			makestatic=false;
int				retcode=0;
Widget			dialog;
XtAppContext	app_con;

void entrySelectCB(Widget w,XtPointer data,XtPointer  garbage)
{
	String str;

	if((long)data==1)
		{
			str=XawDialogGetValueString((Widget) dialog);
			printf("%s",str);
			retcode=0;
		}
	else
		retcode=1;
	XtAppSetExitFlag(app_con);
}

int main(int argc,char *argv[])
{
	Widget			top;
	int				x=0,y=0;
	Display			*display;
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;

	const char	*label="Enter Value";
	const char	*value="";

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	makestatic=true;

	if(argc>1)
		{
			if(argv[1][0]!='c')
			{
			if(argv[1][0]=='m')
				{
					if(XQueryPointer(display,DefaultRootWindow(display),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
						{
							x=win_x_return+32;
							y=win_y_return+32;
							makestatic=false;
						}
				}
			else
				{
					if(argc>1)
						x=atoi(argv[1]);
					if(argc>2)
						y=atoi(argv[2]);
				}
				}
			if((argc>2) && (argv[3]!=NULL))
				label=argv[3];
			if((argc>3) && (argv[4]!=NULL))
				value=argv[4];	
		}

	top=XtVaAppInitialize(&app_con,"appmenu",NULL,ZERO,&argc,argv,fallback_resources,NULL);  
	dialog = XtVaCreateManagedWidget("dialog", dialogWidgetClass, top,"label",label,"value",value,NULL);

    XawDialogAddButton(dialog,"OK",entrySelectCB, (XtPointer) 1);
    XawDialogAddButton(dialog,"Cancel",entrySelectCB,(XtPointer)2);

	XtVaSetValues(top,XmNx,x,XmNy,y,NULL);

	XtRealizeWidget(top);
    XtAppMainLoop(app_con);
	XtDestroyApplicationContext(app_con);

	return(retcode);
}
