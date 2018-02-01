// ��ͷ�ļ�����������Ϣ���ͷ�������ص�ͨ�Ź���
// ���ִ�����ÿ�Դ��Ŀmosquittto
// ������Ѹ��Ȩ����
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