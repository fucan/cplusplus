#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_EVENTS 10
#define PORT 80
#define BUFSIZE 1024
//设置socket连接为非阻塞模式
void setnonblocking(int sockfd)
{
	int opts;
	opts=fcntl(sockfd,F_GETFL);
	if(opts<0)
	{
		perror("fcntl(F_GETFL)\n");
		exit(1);
	}
	opts=(opts |O_NONBLOCK);
	if(fcntl(sockfd,F_SETFL,opts)<0)
	{
		perror("fcntl(F_SETFL)\n");
		exit(1);
	}
}

int main()
{
	struct epoll_event ev,events[MAX_EVENTS];
	int addrlen,listenfd,conn_sock,nfds,epfd,fd,i,nread,n;
	struct sockaddr_in local,remote;
	char buf[BUFSIZE];
	
	//创建listen socket
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("sockfd\n");
		exit(1);
	}
	setnonblocking(listenfd);
	bzero(&local,sizeof(local));
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(PORT);
	if(bind(listenfd,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind\n");
		exit(1);
	}
	
	listen(listenfd,20);
	
	
	epfd=epoll_create(MAX_EVENTS);
	if(epfd ==-1)
	{
		perror("epoll_create");  
        exit(EXIT_FAILURE);  
	}
	
	/*int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
	op：要进行的操作例如注册事件，可能的取值EPOLL_CTL_ADD 注册、EPOLL_CTL_MOD 修 改、EPOLL_CTL_DEL 删除
	*/
	/*
	events可以是以下几个宏的集合：
			 EPOLLIN：            触发该事件，表示对应的文件描述符上有可读数据。(包括对端SOCKET正常关闭)；
			 EPOLLOUT：         触发该事件，表示对应的文件描述符上可以写数据；
			EPOLLPRI：           表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
			EPOLLERR：        表示对应的文件描述符发生错误；
			 EPOLLHUP：        表示对应的文件描述符被挂断；
			EPOLLET：           将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
			EPOLLONESHOT：  只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里。
	*/
	//设置要处理的事件相关的文件描述符
	ev.data.fd=listenfd;
	ev.events=EPOLLIN;
	//注册epoll事件
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev)==-1)
	{
		perror("epoll_ctl: listen_sock");  
        exit(EXIT_FAILURE);
	}
	
	//第三步，等待事件触发
	/*int epoll_wait(int epfd, struct epoll_event * events, intmaxevents, int timeout);
	参数：
	epfd:由epoll_create 生成的epoll专用的文件描述符；
	epoll_event:用于回传代处理事件的数组；
	maxevents:每次能处理的事件数；
	timeout:等待I/O事件发生的超时值(单位我也不太清楚)；-1相当于阻塞，0相当于非阻塞。一般用-1即可
	epoll_wait运行的原理是
	等侍注册在epfd上的socket fd的事件的发生，如果发生则将发生的sokct fd和事件类型放入到events数组中。
	并 且将注册在epfd上的socket fd的事件类型给清空，所以如果下一个循环你还要关注这个socket fd的话，则需要用epoll_ctl(epfd,EPOLL_CTL_MOD,listenfd,&ev)来重新设置socket fd的事件类型。这时不用EPOLL_CTL_ADD,因为socket fd并未清空，只是事件类型清空。这一步非常重要。

	从man手册中，得到ET和LT的具体描述如下
	EPOLL事件有两种模型：
	Edge Triggered(ET)       //高速工作方式，错误率比较大，只支持no_block socket (非阻塞socket)
	LevelTriggered(LT)       //缺省工作方式，即默认的工作方式,支持blocksocket和no_blocksocket，错误率比较小。
	*/
	printf("run ...\n");
	for(;;)
	{
		nfds=epoll_wait(epfd,events,MAX_EVENTS,-1);
		if(nfds == -1)
		{
			perror("epoll_pwait");  
            exit(EXIT_FAILURE);  
		}
		
		//处理事件
		for(i=0;i<nfds;++i)
		{
			fd=events[i].data.fd;
			if(fd == listenfd)
			{
				while((conn_sock=accept(listenfd,(struct sockaddr *)&remote,(size_t *)&addrlen))>0)
				{
					setnonblocking(conn_sock);
					ev.events=EPOLLIN|EPOLLET;
					ev.data.fd=conn_sock;
					//printf("0x%x\n",ev.events);
					if(epoll_ctl(epfd,EPOLL_CTL_ADD,conn_sock,&ev)== -1)
					{
						perror("epoll_ctl: add");  
                        exit(EXIT_FAILURE);  
					}
					//inet_ntoa64位崩溃
					printf("receive a connnect from ip = %s port=%d\n",inet_ntoa(remote.sin_addr),ntohs(remote.sin_port));
				}	//end while
				if(conn_sock == -1)
				{
					if(errno != EAGAIN && errno !=ECONNABORTED && errno != EPROTO && errno !=EINTR)
						perror("accept");
				}
				continue;
			}
			
			if(events[i].events & EPOLLIN)
			{
				bzero(buf,sizeof(buf));
				n=0;
				while ((nread=read(fd,buf+n,BUFSIZE-1))>0)
				{
					n+=nread;
				}
				if (nread == -1 && errno != EAGAIN) {
					perror("read error\n");
				}
				else if(n  >0)
				{
					printf("%s\n",buf);
				}
				else if(n == 0 )
				{
					printf("closed!%d\n",errno);
					close(fd);
					continue;
				}
				//printf("%d,errno=%d\n",nread,errno);
				//ev.data.fd=fd;
				//ev.events=events[i].events | EPOLLOUT |EPOLLET;
				//printf("0x%x\n",ev.events);
				//if(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)==-1)//添加写事件
				//{
				//	perror("epoll_ctl: mod");
				//}
				/*sprintf(buf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World",11);
				int nwrite ,data_size=strlen(buf);
				n =data_size;
				while (n>0)
				{
					nwrite =write(fd,buf+data_size-n,n);
					if(nwrite <n)
					{
						if(nwrite ==-1 && errno != EAGAIN)
						{
							perror("write error");
						}
						break;
					}
					n-=nwrite;
				}*/
			}
			
			if(events[i].events & EPOLLOUT) 
			{
				sprintf(buf,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World",11);
				int nwrite ,data_size=strlen(buf);
				n =data_size;
				while (n>0)
				{
					nwrite =write(fd,buf+data_size-n,n);
					if(nwrite <n)
					{
						if(nwrite ==-1 && errno != EAGAIN)
						{
							perror("write error");
						}
						break;
					}
					n-=nwrite;
				}
				/*printf("0x%x\n",ev.events);
				printf("0x%x\n",EPOLLOUT);
				printf("0x%x\n",~EPOLLOUT);
				printf("0x%x\n",ev.events&~EPOLLOUT);
				*/
				//ev.events=EPOLLIN|EPOLLET;
				//ev.data.fd=fd;
				/*printf("0x%x\n",ev.events);*/
				//if(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)==-1)//添加写事件
				//{
	 			//		perror("epoll_ctl: mod");
	            //}
				//close(fd);//http方式，发完就关闭连接,server端不主动close
			}//end for EPOLLOUT
		}
	}

	return 0;
}
