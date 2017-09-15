#include"qq_server.h"

sqlite3 *db;
char *t_name="qq_chat";	// 数据库的表名

int main(void)
{
	int sockfd, listenfd;
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
	struct kernelnode* kp;
	struct list_head *pos;
	
	struct cinfo* arg;    //用于存储客户端信息，fd和addr
	struct person p;
	char buf[BUFFER];
	int size;
	int opt;                //  设置socket的状态的参数
	pthread_t th;
	
	/*创建套接字*/
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "Sock Error: %s", strerror(errno));
		exit(1);
	}
	printf("socket...\n");
	
	
	/*设置套接字允许重用本地地址和端口*/
	opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//	s_addr.sin_addr.s_addr=inet_addr(IP);
	
	/*绑定套接字与描述符*/
	if(bind(sockfd, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1)
	{
		fprintf(stderr, "bind error: %s", strerror(errno));
		exit(1);
	}
	printf("Bind...\n");
	
	/*监听连接*/
	if(listen(sockfd, 10) < 0)
	{
		fprintf(stderr, "Listen Error: %s", strerror(errno));
		exit(1);
	}
	printf("listen...\n");
	/*开启数据库*/
	sqlite3_open("./qq.db",&db);
	creat_table(db,t_name);       //创建表，表名为qq.db
	INIT_LIST_HEAD(&head.list);   //初始化头结点
//	delete_table(db,"chat");


	info_table(db,t_name,buf);
	
	
	while(1)
	{
		/*阻塞服务器，获取连接*/
		size = sizeof(struct sockaddr);
		printf("等待客户连接...\n");
		if((listenfd =  accept(sockfd, (struct sockaddr*)&c_addr, &size)) < 0)
		{
			perror("accepet\n");
			exit(1);
		}
		printf("客户接受成功，listenfd=%d\n",listenfd);
		
		
		/*填充传入线程的参数*/
		printf("Creat pthread arg\n");
		arg = (struct cinfo*)malloc(sizeof(struct cinfo));
		/*将客户端的套接字符传入新申请的空间*/
		arg->fd = listenfd;
		/*将客户端地址信息赋值入新申请的存储空间*/
		memcpy((void*)&(arg->client), &c_addr, sizeof(c_addr));   
		
		/*创建新的线程，与客户端进行对接,并进行传参数（套接字符，地址信息） */
		
		if(pthread_create(&th,NULL,new_process,(void*)arg)!=0)
		{
			perror("pthread_create");
			break;		
		}
		
		
		/*函数：检测客户端下线，将其从链表中删除*/
		
		
		
		
	}
		
	close(sockfd);
	exit(0);
}

void* new_process(void* arg)
{
	int fd,ret;
	long int to_account;
	char buf[BUFFER];
	
	struct person p;
	struct cinfo* t=arg;
	/*对应于客户端的fd*/
	fd=t->fd;            
	printf("用户套接字符:%d\n",fd);
	while(1)
	{
		
		printf("\n\t1.注册\t2.登陆\t3.退出\n");
		/*接受请求*/
		int order=order_get(fd);
		if(order==0)
			return;
		switch(order){
			/*用户注册*/
			case 1:
				bzero(buf,BUFFER);
				if((ret=recv(fd,buf,sizeof(buf),0))==-1)//接收字符数据
				{
					perror("recv");
					exit(1);
				}
				if(!ret)
				{
					printf("用户%ld关闭了客户端\n",p.account);
					return ;
				}
				if(strncmp(buf,"quit",4)==0)      //如果接受 “quit”字符，则退出登录模式
				{
					break;
				}
				string_data(buf,&p);              //json将字符数据转化为数据
				/*判断账号是否被注册*/
				if(check_register(db,t_name,p.account)==0)
				{
					if(send(fd,"ok",2,0)==-1)             //给客户端返回ok，表示账号注册成功   
					{
						perror("send");
					}
				}else
				{
					if(send(fd,"fail",4,0)==-1)             //给客户端返fail，表示账号已被注册，注册失败   
					{
						perror("send");
					}
					break;
				}
					
				
				/*将注册信息录入数据库*/
				insert (db,&p,t_name); 
				printf("%s:%ld ",p.name,p.account);
				printf("注册成功\n");
				//printf("%ld\n",p.account);
				break;
			
			case 2:
			
				//登陆
				bzero(buf,BUFFER);
				if((ret=recv(fd,buf,sizeof(buf),0))==-1)    //接受客户登录信息，账号和密码
				{
					perror("recv log");
					exit(1);
				}
				if(!ret)
				{
					printf("用户%ld关闭了客户端\n",p.account);
					return;
				}
				//printf("%s\n",buf);
				/*解包用户的登陆信息*/
				string_data_login(buf,&p);
				printf("账号:%ld    密码：%s\n",p.account,p.code);
							
				/*判断账号密码是否正确*/
						
				if(check_login(db,t_name,p.account,p.code)==1)  //密码正确
				{
					if(send(fd,"ok",2,0)==-1)         //给客户端返回ok值，表示账号密码核对成功
					{
						perror("send");
					}
				}else{
					if(send(fd,"fail",4,0)==-1)         //给客户端返回fail，表示账号密码核对失败
					{
						perror("send");
					}
					break;
				}
					
				printf("登陆成功\n");
				/*从数据库取出对方姓名并发给客户端*/
				get_name(db,t_name,p.account);
				bzero(buf,sizeof(buf));
				strcpy(buf,name);
				if(send(fd,buf,sizeof(buf),0)==-1)
				{
					perror("send");
				}
				/*将登陆用户存于内核链表中*/
				online_add(p.account,fd);
				/*显示在线好友信息，账号*/
				online_show(&head);      
				 /*接收登陆后的命令，1 私聊   2群聊  3修改客户信息*/
				 while(1)
				 {
					printf("1.私聊 2.群聊 3.修改客户信息 4.返回\n");
					order=order_get(fd);   //等待接受命令
					if(order==4)
						break ;
					switch(order)
					{	/*私聊*/
						case 1:
						/*
							if(send(fd,"1",1,0)==-1)         //给客户端返回1值，表示成功进入私聊模式
							{
								perror("send");
							}
						*/
							/*发送在线好友列表：账号和姓名*/
					
							send_online_info(fd);		
					
						printf("接受私聊请求:\n");
						
						to_account=order_get(fd);           //获取对方的account
						
						/*进入聊天模式*/
						int chat_fd=account_fd(to_account);     //根据对方账号获取对方套接字符fd
						while(1)
						{
							/*接受客户发送信息*/
							bzero(buf,BUFFER);
							if((ret=recv(fd,buf,sizeof(buf),0))==-1)    
							{
								perror("recv log");
								exit(1);
							}
							if(!ret)
							{
								printf("用户%ld关闭了客户端\n",p.account);
								return;
							}
							if(strncmp(buf,"quit",4)==0)      //如果接受 “quit”字符，则返回上一级
							{
									break;
							}
							printf("%s\n",buf);
							
							/*给客户发送信息*/
							if(send(chat_fd,buf,sizeof(buf),0)==-1)    
							{
								bzero(buf,sizeof(buf));
								strcpy(buf,"对方现在不在线");
								send(fd,buf,sizeof(buf),0);
								printf("%ld现在不在线\n",to_account);
								break;
							}
						}
						break;
					/*群聊*/
					case 2:
					/*
						if(send(fd,"1",1,0)==-1)         //给客户端返回1值，表示成功进入群聊模式
						{
							perror("send");
						}
					*/
						/*发送在线好友信息，账号，姓名*/
						send_online_info(fd);
						
						printf("欢迎进入群聊模式：\n");
						while(1)
						{
							//接受客户发送信息
							bzero(buf,BUFFER);
							if((ret=recv(fd,buf,sizeof(buf),0))==-1)    
							{
								perror("recv log");
								exit(1);
							}
							if(!ret)
							{
								printf("用户%ld关闭了客户端\n",p.account);
								return;
							}
							if(strncmp(buf,"quit",4)==0)      //如果接受 “quit”字符，则返回上一级
							{
								break;
							}
							printf("%s\n",buf);
							
							//群发给在线用户
							msg_group(buf,p.account);
							
						}
						
						break;
						
					/*修改账户信息*/
					case 3:
					/*
					//给客户端返回1值，表示可以开始修改信息
						if(send(fd,"1",1,0)==-1)         
						{
							perror("send");
						}
					*/
						/*将数据库所有信息发送给客户端*/
						bzero(buf,BUFFER);
						info_table(db,t_name,buf);
						if(send(fd,buf,sizeof(buf),0)==-1)
						{
							perror("send");
						}
						printf("准备修改\n");			
						bzero(buf,BUFFER);
						/*接收修改后的用户信息*/
						if(recv(fd,buf,sizeof(buf),0)==-1)
						{
							perror("recv");
							exit(1);
						}
						/*将用户信息进行解包*/
						string_data(buf,&p);
							
						//更新用户信息
						printf("你要修改的账户为：%ld\n",p.account);
						update_data(db,t_name, p);
						printf("更新完成\n");
						
						break;
						
						
					default:
						break;
				}
			 }
			
			 break;
			
			case 3:
		
			
				//结束线程
				printf("有人退出。。。\n");
				sleep(1);
				pthread_exit(0);
				break;
				
				
			default:
				break;
		}
			
	}	
	close(fd);
		
}
/*接受客户端发送命令*/
int order_get(int fd)          
{
	int ret;
	char buf[BUFFER];
	bzero(buf,BUFFER);
	if((ret=recv(fd,buf,sizeof(buf),0))==-1) 
	{
		perror("recv");
	}
	if(!ret)
	{
		printf("客户退出\n");
		return 0;
	}
	/*
	if(strncmp(buf,"quit",4)==0)      //如果接受 “quit”字符，则退出登录模式
	{
			return -1;
	}
	*/
	printf("%s\n",buf);
	ret=atoi(buf);                    //将字符串命令转换为整型
	return ret;
	
}             
/*将登陆用户存于内核链表中*/
void online_add(int acc,int fd)
{
	pnode kp;
	kp=(pnode)malloc(sizeof(node));
	if(kp==NULL)
	{
		perror("malloc failed");
		exit(1);
	}
	kp->account=acc;
	kp->listenfd=fd;
	list_add_tail(&(kp->list),&(head.list)); //将元素插入头结点的前面，即队伍最后面
	
	
}
 /*显示在线好友信息，账号*/
void online_show(pnode h)         
{
	pnode kp;
	struct list_head *pos;
	list_for_each(pos,&(*h).list){
		kp=list_entry(pos,struct kernelnode,list);
		printf("kernel:%ld\n",kp->account);
	}
}
 /*通过账号在链表中检索套接字符fd,进而向对方发送消息*/
int account_fd(int acc)
{
	pnode kp;
	struct list_head *pos;
	list_for_each(pos,&(head).list){
		kp=list_entry(pos,struct kernelnode,list);
		if(kp->account==acc)
			return kp->listenfd;
		else{
			continue;
		}
	}
	
}      
/*将消息群发给在线用户*/
void msg_group(char *msg,int acc)
{
	pnode kp;
	char buf[BUFFER];
	bzero(buf,BUFFER);
	strcpy(buf,msg);
	struct list_head *pos;
	list_for_each(pos,&(head).list){
		kp=list_entry(pos,struct kernelnode,list);
		if(kp->account==acc)
			continue;
		if(send(kp->listenfd,buf,strlen(buf),0)==-1)
		{
			perror("send");
			continue;
		}
	}
}      
/*将在线好友的姓名和账号进行打包发送给指定的客户端*/ 
void send_online_info(int fd)
{
	pnode kp;
	char buf[BUFFER];
	struct person p;
	
	struct list_head *pos;
	bzero(buf,BUFFER);
	strcpy(buf,"\n****当前在线好友信息 姓名和账号****\n");
	if(send(fd,buf,strlen(buf),0)==-1)
	{
		perror("send");
	}
	/*遍历内核链表，将数据进行打包发送*/
	list_for_each(pos,&(head).list){
		kp=list_entry(pos,struct kernelnode,list);
		p.account=kp->account;
		/*根据账户从数据库中提取账号的名字*/
		get_name(db,t_name,p.account);
		strcpy(p.name,name);
	
		bzero(buf,BUFFER);
		sprintf(buf,"\t%s<==>%ld\n",p.name,p.account);
		/*将在线用户的信息发送给客户端*/
		if(send(fd,buf,strlen(buf),0)==-1)
		{
			perror("send");
			continue;
		}
	}
	printf("在线用户信息发送完毕\n");
}