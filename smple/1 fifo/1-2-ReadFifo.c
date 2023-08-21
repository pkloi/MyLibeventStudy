#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <event.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{	
	int fd = open("fifo.tmp", O_WRONLY);
	if(ret == -1)
	{
		perror("open fifo");
		_exit(1);
	}
	
	char buf[32] = {0};
	while(1)
	{
		scanf("%s", buf);
		ret = write(fd, buf, strlen(buf));
		if(ret == -1)
		{
			perror("write fifo");
			_exit(1);
		}
		
		if(!strcmp(buf, ":wq"))
		{
			break;
		}
		
		memset(buf, 0, sizeof(buf));
	}
	
}