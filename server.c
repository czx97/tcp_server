#include<stdio.h>
#include<ctype.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<pthread.h>

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
	listen(lfd,10);

	clit_addr_len = sizeof(clit_addr);
	cfd = accept (lfd,(struct sockaddr *)&clit_addr,&clit_addr_len);

	if(cfd==-1){
		sys_err("accept error");
	}
	printf("client ip:%s port:%d\n",
	inet_ntop(AF_INET,&clit_addr.sin_addr,client_IP,sizeof(client_IP)),
	ntohs(clit_addr.sin_port));
	while(1){
		ret = read(cfd,buf,sizeof(buf));
		write(STDOUT_FILENO,buf,ret);
	        char send_msg[1024] ={"12.345"};
		write(cfd,&send_msg,sizeof(send_msg));
	}
	close(lfd);
	close(cfd);
		
	return 0;
}
