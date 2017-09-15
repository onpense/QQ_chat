#include "qq_server.h"


/*1.注册账号*/
void data_string_register(char *str_apply)
{
      //创建一个用户数据结构体
      struct person my_person;

      //输入获取用户注册信息
      printf("请输入姓名:");
      scanf("%s",my_person.name);
      printf("请输入账号:");
      scanf("%ld",&my_person.account);
      printf("请设置密码:");
      scanf("%s",my_person.code);
      printf("请输入性别:");
      scanf("%s",my_person.gender);

      //创建一个json结构体
      struct json_object *json_apply,*name,*account,*code,*gender,*status;

      //把用户数据转换成json格式
      name=json_object_new_string(my_person.name);
      account=json_object_new_int(my_person.account);
      code=json_object_new_string(my_person.code);
      gender=json_object_new_string(my_person.gender);
      status=json_object_new_int(0);

      //将用户注册信息打包成json结构体
      json_apply=json_object_new_object();
      json_object_object_add(json_apply,"name",name);
      json_object_object_add(json_apply,"account",account);
      json_object_object_add(json_apply,"code",code);
      json_object_object_add(json_apply,"gender",gender);
      json_object_object_add(json_apply,"status",status);

      //创建一个字符串指针
      char *json_str=NULL;

      //将json结构体转换为字符串发送
      json_str=(char *)json_object_to_json_string(json_apply);

      //将获取的字符串拷贝出来
      strcpy(str_apply,json_str);

      json_object_put(name);
      json_object_put(account);
      json_object_put(code);
      json_object_put(gender);
      json_object_put(status);
      json_object_put(json_apply);
}

/*2.账户信息解包*/
void string_data(char *str,struct person *data)
{
      //创建json结构体
      struct json_object *json_data,*j_name,*j_account,*j_code,*j_gender,*j_status;

      char *name;
      int account;
      char *code;
      char *gender;
      int status;

      //将数组转入json结构体
      json_data=json_tokener_parse(str);

      //解析json元素
      j_name=json_object_object_get(json_data,"name");
      j_account=json_object_object_get(json_data,"account");
      j_code=json_object_object_get(json_data,"code");
      j_gender=json_object_object_get(json_data,"gender");
      j_status=json_object_object_get(json_data,"status");

      //将json元素传入数组
      name = (char *)json_object_get_string(j_name);
      strcpy(data->name,name);
      data->account=(int)json_object_get_int(j_account);
      code=(char *)json_object_get_string(j_code);
      strcpy(data->code,code);
      gender=(char *)json_object_get_string(j_gender);
      strcpy(data->gender,gender);
      data->status=(int)json_object_get_int(j_status);
}

/*3.客户端登陆数据打包*/
void data_string_login(char *str_login)  
{
      //创建一个用户数据结构体
      struct person my_stu;
      
      //输入获取用户登陆信息
      printf("请输入账号:");
      scanf("%ld",&my_stu.account);
      printf("请输入密码:"); 
      scanf("%s",my_stu.code);

      //创建json结构体
      struct json_object *json_apply,*account,*code,*status;

      //用户数据转换成json格式
      account=json_object_new_int(my_stu.account);
      code=json_object_new_string(my_stu.code);
      status=json_object_new_int(1);

      //将用户登陆信息打包成json结构体
      json_apply=json_object_new_object();

      json_object_object_add(json_apply,"account",account);
      json_object_object_add(json_apply,"code",code);
      json_object_object_add(json_apply,"status",status);

      //创建一个字符串指针
      char *json_str=NULL;

      //将json结构体转换为字符串发送
      json_str=(char *)json_object_to_json_string(json_apply);
      strcpy(str_login,json_str);

      json_object_put(account);
      json_object_put(code);
      json_object_put(status);
}

/*4.客户端登陆数据解包*/
void string_data_login(char *str,struct person *data)
{
      //创建json结构体
      struct json_object *json_data,*j_account,*j_code,*j_status;

      int account;
      char *code;
      int status;

      //将数组转入json结构体
      json_data=json_tokener_parse(str);

      //解析json元素
      j_account=json_object_object_get(json_data,"account");
      j_code=json_object_object_get(json_data,"code");
      j_status=json_object_object_get(json_data,"status");

      data->account=(int)json_object_get_int(j_account); 
      data->status=(int)json_object_get_int(j_status);
      code=(char *)json_object_get_string(j_code);
      strcpy(data->code,code);

}

/*5.结构体数据转json包*/
void data_string(char *str_ser,struct person stu_ser)
{

      //创建一个json结构体
      struct json_object *json_data,*name,*account,*code,*gender,*status;

      //把用户数据转换成json格式
      name=json_object_new_string(stu_ser.name);
      account=json_object_new_int(stu_ser.account);
      code=json_object_new_string(stu_ser.code);
      gender=json_object_new_string(stu_ser.gender);
      status=json_object_new_int(stu_ser.status);

      //将用户注册信息打包成json结构体
      json_data=json_object_new_object();
      json_object_object_add(json_data,"name",name);
      json_object_object_add(json_data,"account",account);
      json_object_object_add(json_data,"code",code);
      json_object_object_add(json_data,"gender",gender);
      json_object_object_add(json_data,"status",status);

      //创建一个字符串指针
      char *json_str=NULL;

      //将json结构体转换为字符串发送
      json_str=(char *)json_object_to_json_string(json_data);

      //将获取的字符串拷贝出来
      strcpy(str_ser,json_str);

      json_object_put(name);
      json_object_put(account);
      json_object_put(code);
      json_object_put(gender);
      json_object_put(status);
      json_object_put(json_data);
}
