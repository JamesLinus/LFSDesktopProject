
/*
#©keithhedger Tue 12 May 13:22:36 BST 2015 kdhedger68713@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int checkForNTFS(const char *arg)
{
	char	*command;
	int		ret;

	asprintf(&command,"lsblk -n --fs -l|sed -n '/%s/p'|grep ntfs &>/dev/null",arg);
	ret=system(command);
	free(command);
	return(ret);
}

int main(int argc, char **argv)
{
	char	*command;
	uid_t	user=getuid(); 
	int		isNTFS=1;
	int		what;

	if(argc==4)
		{
			isNTFS=checkForNTFS(argv[1]);
			setresuid(0,user,user);
			asprintf(&command,"mkdir -p %s &>/dev/null",argv[2]);
			system(command);
			free(command);
			what=atoi(argv[3]);
			switch(what)
				{
					case 1:
						if(isNTFS==0)
							asprintf(&command,"mount -o user,uid=%i,gid=%i UUID=\"%s\" \"%s\"",user,user,argv[1],argv[2]);
						else
							asprintf(&command,"mount UUID=\"%s\" \"%s\"",argv[1],argv[2]);
						break;
					case 2:
						asprintf(&command,"umount UUID=\"%s\"",argv[1]);
						system(command);
						free(command);			
						asprintf(&command,"rmdir \"%s\"",argv[2]);
						break;
					case 3:
						asprintf(&command,"eject UUID=\"%s\" &",argv[1]);
						system(command);
						free(command);			
						asprintf(&command,"rmdir \"%s\"",argv[2]);
						break;
				}
			system(command);
			free(command);			
		}
	return 0;
}