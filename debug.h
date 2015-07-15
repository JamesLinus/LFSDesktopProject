
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdlib.h>
#include <stdio.h>

#define MAXBUFFER 512
#define DEBUGFUNC(x,...) errLine=__LINE__,errFile=__FILE__,errFunc=__func__;debugFunc((const char*)x,__VA_ARGS__)
#define debugfunc  DEBUGFUNC

int			errLine;
const char	*errFile;
const char	*errFunc;

void debugFunc(const char *fmt, ...)
{
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(MAXBUFFER);
	subbuffer=(char*)alloca(MAXBUFFER);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);
	printf("File: %s,Func: %s,Line: %i\n",errFile,errFunc,errLine);
	printf(">>%s<<\n",buffer);
}


#endif
