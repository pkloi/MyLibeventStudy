#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <event2/listener.h>
#include <stddef.h>
#include <unistd.h>

//其他事件的回调函数
void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	printf("异常发生 %x!\n", what);
	if(what & BEV_EVENT_EOF)
	{
		printf("客户端%d下线\n", *(int *)ctx);
		bufferevent_free(bev);	//释放bufferevent对象
	}
	else
	{
		printf("未知错误！\n");
	}
	
}

//读取数据
void read_cb(struct bufferevent *bev, void *ctx)
{
	int fd = *(int *)ctx;
	char buf[128] = {0};
	size_t ret = bufferevent_read(bev, buf, sizeof(buf));
	if(ret < 0)
	{
		printf("bufferevent_read error\n");
	}
	else
	{
		printf("read from %d %s\n", fd, buf);
	}
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	printf("接受%d的连接\n", fd);
	
	struct event_base *base = arg;

	//针对已存在的socket创建bufferevent对象
	//base:事件集合（从主函数传递来的
	//fd：代表TCP连接
	//BEV_OPT_CLOSE_ON_FREE：如果释放bufferevent对象，则关闭连接
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if(bev == NULL)
	{
		printf("bufferevent_socket_new error\n");
		_exit(1);
	}
	
	//给每一个bufferevent设置回调函数
	//bev:bufferevent对象
	//read_cb：读事件的回调函数
	//NULL：写事件的回调函数
	//event_cb：其他事件的回调函数
	//NULL：参数
	bufferevent_setcb(bev, read_cb, NULL, event_cb, &fd);
	
	//使能事件类型，让对象生效
	bufferevent_enable(bev, EV_READ);
}

/* 
 * socket,bind,listen,accept被封装成了evconnlistener_new_bind函数
 * */
int main()
{
	//创建一个事件集合
	struct event_base *base = event_base_new();
	if(base == NULL)
	{
		printf("event_base_new error\n");
		_exit(1);
	}
	
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 80;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//创建socket、绑定、监听、接受连接
	//创建监听对象，在指定的地址上监听接下来的TCP连接
	//base：事件集合
	//listener_cb：当有连接时调用的函数
	//NULL：回调函数的参数
	//LEV_OPT_CLOSE_ON_FREE：释放监听对象关闭socket
	//LEV_OPT_REUSEABLE：端口重复使用
	//10：监听队列长度
	//server_addr：绑定信息
	struct evconnlistener *listener = evconnlistener_new_bind(base ,listener_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr *)&server_addr, sizeof(server_addr));
	
	if(listener == NULL)
	{
		printf("evconnlistener_new_bind error\n");
		_exit(1);
	}
	
	//监听集合中的事件
	event_base_dispatch(base);
	
	//释放两个对象
	evconnlistener_free(listener);
	event_base_free(base);
	
	
	return 0;
}


