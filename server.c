#include<stdio.h>      //C标准输入输出
#include<ctype.h>      //toupper函数头文件
#include<sys/socket.h>   //socket通信
#include<arpa/inet.h>    //socket通信
#include<stdlib.h>        //C标准库
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
	int lfd ,cfd;
	int ret;
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
	fd_set rset,allset;
	int maxfd=0;
	maxfd = lfd;
	FD_ZERO(&allset);
	FD_SET(lfd,&allset);
	while(1){
		rset = allset;
		ret = select(maxfd+1,&rset,NULL,NULL,NULL);
		if(ret<0){
		      	sys_err("select error");
		}
		if(FD_ISSET(lfd,&rset)){
			clit_addr_len = sizeof(clit_addr);
			cfd = accept(lfd,(struct sockaddr*)&clit_addr,&clit_addr_len);
			printf("client ip:%s port:%d connected...\n ",
			inet_ntop(AF_INET,&clit_addr.sin_addr,client_IP,sizeof(client_IP)),
			ntohs(clit_addr.sin_port));
			FD_SET(cfd,&allset);
			if(cfd>maxfd)
			       	maxfd = cfd;
			if(0==--ret)
			       	continue;
		}
		for(int i =lfd+1;i<=maxfd && ret;i++){
			if(FD_ISSET(i,&rset)){
				--ret;
				int n = read(i,buf,sizeof(buf));
				if(n==0){
					printf("client ip:%s port:%d leave...\n",
					inet_ntop(AF_INET,&clit_addr.sin_addr,client_IP,sizeof(client_IP)),
					ntohs(clit_addr.sin_port));
					close(i);
					FD_CLR(i,&allset);
				}
				for(int j = 0;j<n;j++){
					buf[j]=toupper(buf[j]);
				}
				write(cfd,buf,n);
				write(STDOUT_FILENO,buf,n);

			}
		}

	}

	return 0;
}
