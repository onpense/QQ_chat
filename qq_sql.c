#include "qq_server.h"

void creat_table(sqlite3 *db,char *t_name)
{
	int ret = -1;
	
	sprintf(sql,"create table if not exists %s(name varchar(20),account int,code varchar(20),gender varchar(10),status int)",t_name);//gender未处理。
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		printf("create:%s\n",errmsg);
		return;
	}
}

void insert(sqlite3 *db,struct person* pt,char *t_name)
{
    int ret3 =-1;
	sprintf(sql,"insert into %s(name,account,code,gender,status) values('%s',%ld,'%s','%s',%d)",t_name,pt->name,pt->account,pt->code,pt->gender,pt->status);//状态
	ret3 = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret3)
	{
		sqlite3_close(db);
		printf("insert : %s\n",errmsg);
		exit(0);
	}
	  	
}



void info_table(sqlite3 *db,char *t_name,char* buf)
{
	char **result;
	int nrow = -1;
	int ncolumn = -1;
	int ret = -1;
	int i = 0;
	char info[100];
//	sprintf(sql,"select account from %s where %d",t_name,&pt->);
//	ret = sqlite3_get_table(db,sql,&result,NULL,&errmsg);
	

	sprintf(sql,"select * from %s",t_name);
	ret = sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg);
		
	printf("用户信息：\n");
	//strcat(buf,BLUE);
	for(i=0;i<(nrow+1)*(ncolumn);i++)
	{
		if(i%5==0)
		{
			printf("\n");
			strcat(buf,"\n");
			
		}	
		printf("%s\t",result[i]);
		bzero(info,100);
//		strcpy(info,result[i]);
		sprintf(info,"%s\t",result[i]);
		strcat(buf,info);
		
	}
	//strcat(buf,NONE);
	printf("\n\n");
}
/*删除用户*/
void delete_table(sqlite3 *db,char *t_name)
{
	int ret = -1;
	long int account = 0;
	
	printf("删除用户 account:\n");
	scanf("%ld",&account);
	sprintf(sql,"delete from %s where account=%ld",t_name,account);
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		printf("delete:%s\n",errmsg);
		exit(0);
	}
	printf("成功删除账户%ld的信息 \n",account);
//	search_table(db,t_name);
}
/*更新数据库的内容，把p的信息重新存储*/
void update_data(sqlite3 *db,char *t_name,struct person p)
{
	int ret;
	memset(sql,0,128);
	sprintf(sql,"update %s set code = '%s' where account=%ld",t_name,p.code,p.account);
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_close(db);
		printf("update:%s\n",errmsg);
		exit(0);
	}

	memset(sql,0,128);
	sprintf(sql,"update %s set name = '%s' where account=%ld",t_name,p.name,p.account);
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_close(db);
		printf("update:%s\n",errmsg);
		exit(0);
	}



	memset(sql,0,128);
	sprintf(sql,"update %s set gender = '%s' where account=%ld",t_name,p.gender,p.account);
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(SQLITE_OK != ret)
	{
		sqlite3_close(db);
		printf("update:%s\n",errmsg);
		exit(0);
	}
		
}

/*注册信息验证，检验账号是否被注册*/
/*若返回值为1 账号存在，不能注册，返回值为0，可以注册*/
int check_register(sqlite3*db,char *t_name,long int acc)
{
	int ret;
	int nrow = -1;
	int ncolumn = -1;
	char **result;
	int i = 0;
	sprintf(sql,"select account from %s ",t_name);

	ret = sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg);
	for(i=0;i<nrow;i++)
	{
		if(acc== atoi(result[i]))//result是字符串格式，转化成整形才可以比对。
		{
			printf("该账号已存在，请重新注册新账号\n");
			return 1;
		}
	}		
	return 0;
	
}

/*登陆账号密码验证*/
/*账号密码核对成功返回1，否则返回0*/
int check_login(sqlite3 *db,char *t_name,long int acc,char *code)
{
	int ret1 = -1;
	int ret2 = -1;
	int nrow = -1;
	int ncolumn = -1;
	char **result1;
	char **result2;
	char a[20];	
//	char b[20];
	int i = 0;
	int j=0;
	int t1,t2;	

	
	sprintf(a,"%ld",acc);
	printf("kdfkefl=%s\n",a);
	//从数据库中获取所有注册人的账号
	sprintf(sql,"select account from %s",t_name);
	ret1 = sqlite3_get_table(db,sql,&result1,&nrow,&ncolumn,&errmsg);

	for(i=0;i<(nrow+1);i++)
	{	
		t1=strcmp(a,result1[i]);
		if(t1 == 0)
			{
				sprintf(sql,"select code from %s",t_name);
				ret2 = sqlite3_get_table(db,sql,&result2,&nrow,&ncolumn,&errmsg);
				for(j=0;j<(nrow+1);j++)
				{
					t2 = strcmp(code,result2[i]);
					if(t2 == 0)
						return 1;
				}
			}
	
	}
	printf("账号/密码错误\n");
	return 0;
}
/*根据账号从数据库中取出姓名信息*/
char *get_name(sqlite3 *db,char *t_name,long int acc)
{
	int ret = -1;
	char buf[20] ={0};
	char **result;
	char *q =buf;
	int nrow = -1;
	int ncolumn = -1;
	int i;

	bzero(name,20);
	sprintf(sql,"select * from %s",t_name);
	ret = sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg);
	for(i=0;i<(nrow+1)*ncolumn;i++)
	{
		if(acc==atoi(result[i]))
		{
			strcpy(name,result[i-1]);
			printf("%s\n",result[i-1]);
		//	printf("%s\n",buf);
			break;
		}
	}

	return q;
}


