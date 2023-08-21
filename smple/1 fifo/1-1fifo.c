#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <event.h>
#include <fcntl.h>
#include <unistd.h>

//当监听的事件满足条件时，会触发回调函数，通过回调函数读取数据
void fifo_read(evutil_socket_t fd, short events, void *arg)
{
	char buf[32] = {0};
	int ret = read(fd, buf, sizeof(buf));
	if(ret == -1)
	{
		perror("open fifo");
		_exit(1);
	}
	printf("从管道读取数据：%s\n", buf);
}

int main()
{
	//创建管道
	int ret = mkfifo("fifo.tmp", 00700);
	if(ret == -1)
	{
		perror("mkfifo");
		_exit(1);
	}
	
	//读取管道
	int fd = open("fifo.tmp", O_RDONLY);
	if(fd == -1)
	{
		perror("open fifo");
		_exit(1);
	}
	
	//创建事件
	struct event ev;
	
	//初始化事件集合
	event_init();
	
	//初始化事件（把fd和事件ev绑定）
	//&ev:事件
	//fd:关联的文件描述符
	//EV_READ:事件类型
	//fifo_read:需要实现的回调函数
	//NULL:回调函数的参数
	event_set(&ev, fd, EV_READ | EV_PERSIST, fifo_read, NULL);
	
	//把事件添加到集合中
	event_add(&ev, NULL);
	
	//开始监听
	event_dispatch();//死循环，如果集合中没有事件可以监听，则返回
	
	return 0;
}


