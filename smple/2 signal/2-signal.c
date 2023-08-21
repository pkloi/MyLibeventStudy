#include <stdio.h>
#include <signal.h>
#include <event.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int signal_count=0;

void signal_handler(evutil_socket_t fd, short events, void *arg)
{
	struct event *ev = (struct event*)arg;
	printf("收到信号 %d\n", fd);
	
	signal_count++;
	if(signal_count >= 2)
	{
		//把事件从集合中删除
		event_del(ev);
	}
}

int main()
{
	//创建事件集合
	struct event_base *base = event_base_new();
	
	//创建事件集合
	struct event ev;
	//把事件和信号绑定
	int ret = event_assign(&ev, base, SIGINT, EV_SIGNAL | EV_PERSIST, signal_handler, &ev);
	if(ret == -1)
	{
		perror("event_assign");
		_exit(1);
	}
	
	//把事件添加到集合中
	event_add(&ev, NULL);

	//监听自己的集合使用event_base_dispatch，监听全局变量中的集合使用event_dispatch
	event_base_dispatch(base);
	
	//自己创建的集合需要释放
	event_base_free(base);
	
	return 0;
}
