#include "qq_server.h"
/*为什么用文件IP写入数据会失败，不会产生文件表示符，但是会产生文件,可能和共享盘oops有关*/
/*该程序采用标准IO进行聊天记录的存储*/
static int sockfd;


/*子线程用于读取服务器返回的数据*/
void* new_process(void* arg)
{
	int ret;
	char buf[BUFFER];    //用户接受和发送信息
	while(1)
	{
		bzero(buf,BUFFER);
		if((ret=recv(sockfd,buf,sizeof(buf),0))<0)
		{
			perror("recv log");
			break;
		}
		if(!ret)
		{
			printf(RED"哇靠，服务器崩掉了\n"NONE);
			exit(1);
		}
		/*
		if(strncmp(buf,"register_ok",11)==0)
			REGISTER_OK=1;
		if(strncmp(buf,"login_ok",8)==0)
			LOGIN_OK=1;
		*/
		printf("%s\n",buf);
		
		
	}
}


int main(void)
{
	struct sockaddr_in s_addr;
	struct person p;
	pthread_t th;
	static int once=1;
	int* arg; 
	char *t;
	time_t tm;
	char buf[BUFFER];    //用户接受和发送信息
	char myname[20];     //用于存储自己的名字
	char myinfo[BUFFER];  //用于储存发送信息
//	int filefd;       //文件描述符，用于存储聊天记录
	FILE *fp;
	char txtname[20];

	/*创建套接字*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		fprintf(stderr, "socket() error : %s", strerror(errno));
		exit(1);
	}
	/*填充服务器地址*/
	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	s_addr.sin_addr.s_addr =inet_addr(IP);

	/*连接服务器*/
	if(connect(sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1)
	{
		fprintf(stderr, "connect() error : %s", strerror(errno));
		exit(1);
	}
	/*	
		创建新的线程，与客户端进行对接,并进行传参数（套接字符，地址信息） 
		if(pthread_create(&th,NULL,new_process,(void*)arg)!=0)
		{
			perror("pthread_create");
			exit(1);		
		}
		
	*/	
	
	/*主程序用于与服务端命令对接*/
	while(1)
	{		
		system("clear");
		printf(BLUE"\t***********1.注册 2.登陆 3.退出************\n\n\n"NONE);
//		REGISTER_OK=-1;
//		LOGIN_OK=-1;
		int i,ret;
		
		printf("选择:");
		scanf("%s",buf);
		send(sockfd,buf,sizeof(buf),0);	
		if(buf[0]=='3')
			break;
		switch(buf[0])
		{
			case '1':
				//提交注册信息
				bzero(buf,BUFFER);
				data_string_register(buf);   
				if(send(sockfd,buf,sizeof(buf),0)==-1)
				{
					perror("send");
				}
				/*接受服务端返回信息，该账户是否被注册*/
				bzero(buf,BUFFER);
				if((ret=recv(sockfd,buf,sizeof(buf),0))==-1)
				{
					perror("recv log");
				}
				if(strncmp(buf,"ok",2)==0)
					printf("恭喜，注册成功\n");
				else 
					printf("该账户已被注册，请重试，亲\n");
					sleep(1);
				sleep(1);
				break;
				

			case '2':
				/*向服务器发送登录账号密码*/
				bzero(buf,BUFFER);
				data_string_login(buf);
				if(send(sockfd,buf,sizeof(buf),0)==-1)
				{
					perror("send");
					exit(1);
				}
				
				/*根据服务器返回命令，判断是否登录成功*/
				bzero(buf,BUFFER);
				if((ret=recv(sockfd,buf,sizeof(buf),0))==-1)
				{
					perror("recv log");
				}
				if(strncmp(buf,"ok",2)==0)
					printf("恭喜，登录成功\n");
				else 	
				{
					printf("账户密码不匹配，请重试，亲\n");
					sleep(1);
					break;
				}
				sleep(1);
				/*接受服务器查询到的自己账户的姓名*/
				bzero(buf,sizeof(buf));
				if(recv(sockfd,buf,sizeof(buf),0)==-1)
				{
					perror("recv");
				}
				strcpy(myname,buf);
				printf("%s\n\n",myname);
				
				/*打开或者创建聊天记录的文本文件，当前目录的 chat_record 文件中*/
				strcpy(txtname,"./chat_record/");
				strcat(txtname,buf);
				strcat(txtname,".txt");   //创建  myname.txt 的文本文档，用于存储聊天记录
				
				/*if((filefd=open(buf,O_RDWR|O_CREAT|O_APPEND,0777))<0)
				{
					printf("open file failed\n");
				}
				printf("filefd=%d\n",filefd);
				*/
				fp=fopen(txtname,"a+");
				/*创建新的线程，与客户端进行对接,并进行传参数（套接字符，地址信息） */
				if(once==1)
				{
					
					if(pthread_create(&th,NULL,new_process,(void*)arg)!=0)
					{
						perror("pthread_create");
						exit(1);		
					}
					printf("创建读线程成功\n");
					once++;
				}
				
				/*登录后可选择私聊，群聊，修改客户信息*/
				while(1)
				{
					system("clear");
					printf(BLUE"=====1.私聊  2.群聊  3.修改客户信息 4.返回=====\n\n\n"NONE);
					printf("选择:");
					scanf("%s",buf);
					send(sockfd,buf,sizeof(buf),0);
					/*输入4则返回上一级目录*/
					if(buf[0]=='4')
						break;
					switch(buf[0])
					{
						case '1':
							printf("进入私聊\n");
							/*接受服务器的在线好友列表*/
							
							sleep(1);
							printf("请输入对方账号\n");
							bzero(buf,sizeof(buf));
							scanf("%s",buf);
							if(send(sockfd,buf,sizeof(buf),0)<0)
							{
								perror("send");
								exit(1);
							}
							printf("开始聊天\n");
							strcpy(buf,"\n私聊记录:\n");
							fwrite(buf,1,strlen(buf),fp);
							while(1)
							{
								printf("==>");
								bzero(buf,sizeof(buf));
								scanf("%s",buf);
								time(&tm);       //获取日历时间
								t=ctime(&tm);  //将日历时间转化为字符串形式
								strcpy(myinfo,myname);
								strcat(myinfo,"::");
								strcat(myinfo,t);
								strcat(myinfo,buf);
								if((ret=send(sockfd,myinfo,sizeof(myinfo),0))==-1)
								{
									perror("send");
									exit(1);
								}
								if(strncmp(buf,"quit",4)==0)
									break;
								/*将聊天记录存储*/
								strcat(myinfo,"\n");  //每一句话后换行
								fwrite(myinfo,1,strlen(myinfo),fp);
							}
							break;
							
						case '2':
								printf("进入群聊\n");
							
							/*接受服务器的在线好友列表*/
							sleep(1);
							strcpy(buf,"\n群聊记录:\n");
							fwrite(buf,1,strlen(buf),fp);
							while(1)
							{
								printf("==>");
								bzero(buf,sizeof(buf));
								scanf("%s",buf);
								time(&tm);       //获取日历时间
								t=ctime(&tm);  //将日历时间转化为字符串形式
								strcpy(myinfo,myname);
								strcat(myinfo,"::");
								strcat(myinfo,t);
								strcat(myinfo,buf);
								if((ret=send(sockfd,myinfo,sizeof(myinfo),0))==-1)
								{
									perror("send");
									exit(1);
								}
								if(strncmp(buf,"quit",4)==0)
									break;
								/*将聊天记录存储*/
								strcat(myinfo,"\n");  //每一句话后换行
								fwrite(myinfo,1,strlen(myinfo),fp);
							}
							break;
							
						case '3':
							sleep(1);   //等待服务器将数据库的内容全部发送过来
							printf("\n可以开始修改\n\n");
							printf("请输入要修改的账号\n");
							scanf("%ld",&p.account);
							printf("请输入更改后的姓名\n");
							scanf("%s",p.name);
							printf("请输入更改后的密码\n");
							scanf("%s",p.code);
							printf("请输入更改后的备注\n");
							scanf("%s",p.gender);
							p.status=1;
							printf("修改信息已提交\n");
							bzero(buf,BUFFER);
							data_string(buf,p);
							if(send(sockfd,buf,sizeof(buf),0)==-1)
							{
								perror("send");
								exit(1);
							}
						
							break;
							
						default:
							break;
					}
				}
				
			default:
				break;
		}

	}
	fclose(fp);
	pthread_cancel(th);
	return 0;
}

