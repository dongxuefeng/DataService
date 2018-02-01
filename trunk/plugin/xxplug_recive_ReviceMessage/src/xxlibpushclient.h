// 本头文件定义了与消息推送服务器相关的通信功能
// 部分代码借用开源项目mosquittto
// 西安翔迅版权所有
// 
#ifndef _XXLIBPUSHCLIENT_H_
#define _XXLIBPUSHCLIENT_H_

#ifdef XXLIBPUSHCLIENT_EXPORTS
#define XXLIBPUSHCLIENT_API __declspec(dllexport)
#else
#define XXLIBPUSHCLIENT_API __declspec(dllimport)
#endif

typedef void(*On_Message)(char * topic, int topic_len, char * message, int msg_len, void * obj);

extern "C"
{
	XXLIBPUSHCLIENT_API void * Init(char * username, char * IP, int port, On_Message onMessage);
	XXLIBPUSHCLIENT_API int publish(void * obj, char * topic, char * message, bool retain);
	XXLIBPUSHCLIENT_API int subscribe(void * obj, char * topic);
	XXLIBPUSHCLIENT_API int FInit(void * obj);
}

#endif