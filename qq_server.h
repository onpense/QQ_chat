#ifndef _QQ_SERVER_H_
#define _QQ_SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<pthread.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>

#include<sys/stat.h>
#include<fcntl.h>

#include"list.h"
#include </usr/local/include/json/json.h>
#include<sqlite3.h>
#define SQL_SIZE 128
#define BUFFER 1024
#define PORT 9999                 //端口
#define IP  "192.168.1.11"       //服务器IP

//颜色
#define  NONE   "\033[m"
#define   RED  "\033[1;31m"
#define  BLUE   "\033[1;32m"


char sql[SQL_SIZE];
char *errmsg;
char name[20];
/*用户数据*/
struct person{
      char name[20];
      long int account;
      char code[20];
      char gender[8];
      int status;
};
/*存储客户端信息*/
struct cinfo{
	int fd;
	struct sockaddr_in client;
};
//内核链表，用于检索用户
typedef struct kernelnode{
	long int account;
	char name[20];
	int listenfd;
//	struct sockaddr_in addr;
	struct list_head list;
}node,*pnode;

struct kernelnode head;         //声明头结点


//客户端

/*1,注册信息打包*/
extern void data_string_register(char *str_apply);
/*2,账号信息解包*/
extern void string_data(char *str,struct person *data);
/*3,客户端登陆数据打包*/
extern void data_string_login(char *str_login);
/*4,客户端登陆数据解包*/
extern void string_data_login(char *str,struct person *data);
/*5.结构体数据转json包*/
extern void data_string(char *str_ser,struct person stu_ser);


//数据库

/*打开数据库*/
//int sqlite3_open("./hoo.db",&db);
/*创建数据库表*/
extern void creat_table(sqlite3 *db,char *t_name);
/*插入一项如数据库*/
extern void insert(sqlite3 *db,struct person* pt,char *t_name);
/**/
extern void input(sqlite3 *db,struct person* pt,char *t_name);
/*取出数据库的全部信息*/
extern void info_table(sqlite3 *db,char *t_name,char *buf);
/*删除数据库表*/
extern void delete_table(sqlite3 *db,char *t_name);
/*更新数据库表*/
extern void update_data(sqlite3 *db,char *t_name,struct person p);
/*注册信息验证，检验账号是否被注册*/
extern int check_register(sqlite3*db,char *t_name,long int acc);
/*登陆账号密码验证*/
extern int check_login(sqlite3 *db,char *t_name,long int acc,char *code);
/*从账号中取出姓名信息*/
char *get_name(sqlite3 *db,char *t_name,long int acc);



//服务端
/*接受客户端发送命令*/
int order_get(int fd);        
/*新线程执行函数*/
void* new_process(void*arg);  
/*将登陆的用户加入在线链表中*/
void online_add(int acc,int fd);     
/*显示在线好友*/
void online_show(pnode h);    
/*通过账号在链表中检索套接字符fd,进而向对方发送消息*/
int account_fd(int acc);               
/*消息群发给在线用户*/
void msg_group(char *msg,int acc);      
/*发送在线好友给客户端*/
void send_online_info(int fd);              


  
#endif
