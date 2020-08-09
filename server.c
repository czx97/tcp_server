#include<stdio.h>      //C标准输入输出
#include<ctype.h>      //toupper函数头文件
#include<sys/socket.h>   //socket通信
#include<arpa/inet.h>    //socket通信
#include<stdlib.h>        //C标准库
#include<sys/epoll.h>      //epoll函数头文件
#include<unistd.h>         //POSIX系统调用API
//#include<string.h>         //字符串操作，与string并列存在
#include<errno.h>          //通过错误码提示错误信息
//#include<pthread.h>

#define SERV_PORT 9999
void sys_err(const char *str){
	perror(str);	
	exit(1);
}


int main(int argc,char *argv[])
{
	int lfd ,cfd,epfd;
	int ret,n;
	printf("server waitting for connection...\n");
	char buf[BUFSIZ],client_IP[1024];
	struct sockaddr_in serv_addr,clit_addr;
	socklen_t clit_addr_len;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	lfd = socket(AF_INET,SOCK_STREAM,0);
	if(lfd==-1){
		sys_err("socket error");
	}
	
	ret = bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(ret ==-1)
		sys_err("bind error");
	listen(lfd,128);
	struct epoll_event tmp,array[1024];
	epfd = epoll_create(1024);
	tmp.events = EPOLLIN;
	tmp.data.fd = lfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&tmp);
	while(1){
		ret = epoll_wait(epfd,array,1024,-1);
		for(int i=0;i<ret;i++){
			if(array[i].data.fd == lfd){
				cfd = accept(lfd,(struct sockaddr*)&clit_addr,&clit_addr_len);
				printf("client ip:%s port:%d connected...\n ",
				inet_ntop(AF_INET,&clit_addr.sin_addr,client_IP,sizeof(client_IP)),
				ntohs(clit_addr.sin_port));
				tmp.events = EPOLLIN;
				tmp.data.fd = cfd;
				epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&tmp);
			}
			else{
				n = read(array[i].data.fd,buf,sizeof(buf));
				if(n==0){
					close(array[i].data.fd);
					epoll_ctl(epfd,EPOLL_CTL_DEL,array[i].data.fd,NULL);
				}
				else if(n>0){
					for(int i=0;i<n;i++){
						buf[i] = toupper(buf[i]);
					}
					write(array[i].data.fd,buf,sizeof(buf));
				}

			}
		}
	}

	return 0;
}
