//============================================================================
// Name        : epolice_auto_check.cpp
// Author      : xiaohua.shan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <cxx_xml.h>

using namespace std;

#include "WebService/soapH.h"
//#include "WebService/soaprecive_auto_infoService.h"
#include "WebService/webservice.nsmap" // include the XML namespace mappings
#define SOUP_XML_SIZE 1024*1024*10
int http_get(struct soap * soap)
{
	FILE *fd = NULL;

	char *s = strchr(soap->path, '?');
	if (!s || strcmp(s, "?wsdl"))
		return SOAP_GET_METHOD;

	fd = fopen("webservice.wsdl", "rb");

	if (!fd)
		return 404;

	soap->http_content = "text/xml";

	soap_response(soap, SOAP_FILE);
	for (;;)
	{
		size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd);
		if (!r)
			break;
		if (soap_send_raw(soap, soap->tmpbuf, r))
			break;
	}

	fclose(fd);
	soap_end_send(soap);

	return SOAP_OK;
}

#include "./plug_ctrl/analysisXML.h"
#include "./plug_ctrl/c_plug_ctrl.h"

c_plug_ctrl * p_plug_ctrl = 0;
const char * fail = "failed";

int chIns(char ch[],char ch1[],int k){
	int i;
	int len_ch=strlen(ch);
	int len_ch1=strlen(ch1);
	if (k>len_ch) {
		printf("插入的位置大于第一个字符串的长度，程序退出!\n");
		return -1;
	}
	else {
		for (i=len_ch+len_ch1-1;i>=k+len_ch1;i--){
			ch[i]=ch[i-len_ch1];       /*移动字符将要插入的位置空出来*/
		}
		for (i=0;i<len_ch1;i++){
			ch[k+i]=ch1[i];     /*插入字符串*/
		}
		ch[len_ch+len_ch1]='\0';  /*设置字符结束符*/
		return 1;
	}
}

int ns2__rcvautoinfo(struct soap *add_soap, char *req, char **res)
{
	int ret = -1;

	memset(add_soap->inxml, 0, SOUP_XML_SIZE);
	//printf("req: %s\n", req);
	sprintf(add_soap->inxml, req);
	char *p = strstr(add_soap->inxml, "<head>");
	if(p != NULL)
	{
		char tmp[64] = {0};
		sprintf(tmp, "<ip value=\"%d.%d.%d.%d\"/>", (add_soap->ip >> 24)&0xFF, (add_soap->ip >> 16)&0xFF, (add_soap->ip >> 8)&0xFF, add_soap->ip&0xFF);
		chIns(add_soap->inxml, tmp, p-add_soap->inxml+6);
		//printf("INXML : %s\n", add_soap->inxml);
	}
	//printf("DataServer INXML : %s\n", req);
	if(p_plug_ctrl)
	{
	    ret = p_plug_ctrl->proc_info(add_soap->inxml,res);
	}

	if (ret == -1) *res = (char *)fail;

	return 0;
}


char gexedir[256];
void get_exe_dir(char * exefile)
{
	sprintf(gexedir,exefile);
	char * p = gexedir+strlen(gexedir);
	while(*p != '\\' && *p != '/')
	{
		p--;
	}
	*p = '\0';
}

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
using namespace std;

typedef struct _ASTAT_
{
   ADAPTER_STATUS adapt;
   NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

ASTAT Adapter;

void getMac(vector<string> * macList)
{
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM   lenum;
	string mac;

	memset( &Ncb, 0, sizeof(Ncb) );
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	uRetCode = Netbios( &Ncb );

	printf("=======MAC Count = %d ======\n", lenum.length);

	for(unsigned int i=0; i < lenum.length ;i++)
	{
		memset( &Ncb, 0, sizeof(Ncb) );
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];

		uRetCode = Netbios( &Ncb );

		memset( &Ncb, 0, sizeof (Ncb) );
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];

		strcpy( (char*)Ncb.ncb_callname,  "*               " );
		Ncb.ncb_buffer = (unsigned char *) &Adapter;
		Ncb.ncb_length = sizeof(Adapter);

		uRetCode = Netbios( &Ncb );

		if ( uRetCode == 0 )
		{
			char tmp[13] = {0};
			sprintf(tmp, "%02x%02x%02x%02x%02x%02x",
					Adapter.adapt.adapter_address[0],
					Adapter.adapt.adapter_address[1],
					Adapter.adapt.adapter_address[2],
					Adapter.adapt.adapter_address[3],
					Adapter.adapt.adapter_address[4],
					Adapter.adapt.adapter_address[5] );
			mac = tmp;
			printf("Mac: %s\n", mac.c_str());
			macList->push_back(mac);
		}
	}
}

#include <dbmepolice.h>
#include <string.h>
#include "xxbase64.h"
MYSQL * m_sql = 0;

void Reg()
{
	//connectMySQL(m_sql);

	vector<string> macList;
	string mac;
	string checkingNum = "";
	char tmp[128] = {0};
	//获取MAC
	getMac(&macList);

	//读数据库
	string sql = "SELECT checkingnum FROM xxdb_info_base WHERE id = 1";
	int ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) checkingNum = row[0];
			else checkingNum = "";
		}
		mysql_free_result(result);
	}

	for(vector<string>::iterator iter = macList.begin(); iter != macList.end(); iter++)
	{
		mac = (*iter).c_str();
		mac += "a78d2r";
		//printf("first: %s\n", mac.c_str());

		//MD5
		memset(tmp, 0, 128);
		b64_encode_string((const unsigned char *)mac.c_str(), mac.size(), (unsigned char*)tmp);
		mac = tmp;
		//printf("second: %s\n", tmp);

		//将第二步结果再md5加密
		memset(tmp, 0, 128);
		b64_encode_string((const unsigned char *)mac.c_str(), mac.size(), (unsigned char*)tmp);
		//printf("third: %s\n", tmp);

		//验证
		if(checkingNum.compare(tmp) == 0)
		{
			printf("Checking OK!\n");
			return;
		}
		else
		{
			printf("Checking ERROR!");
			ret = -1;
		}
	}

CHECK:
		if(checkingNum.compare(tmp) == 0)
		{
			printf("Checking OK!\n");

			//写数据库下次不用验证
			sql = "UPDATE xxdb_info_base SET checkingnum = '";
			sql += checkingNum;
			sql += "'";
			sql += " WHERE id = 1";
			xx_sql_query(m_sql, sql.c_str());
			ret = 0;
		}
		else
		{
			printf("Checking ERROR!");
			ret = -1;
		}

		if(ret == -1)
		{	//无验证码或验证码错误
			char str[64] = {0};
			printf("\n************************\n\n");
			printf("Regcode: %s\n", mac.c_str());
			printf("\n************************\n");
			printf("Please input CheckingNum: ");
			fgets(str, 64, stdin);
			str[strlen(str)-1] = '\0';
			checkingNum = str;
			goto CHECK;
		}
}

#include <pthread.h>
#define BACKLOG (100) // Max. request backlog
#define MAX_THR (30) // Size of thread pool
#define MAX_QUEUE (1000) // Max. size of request queue

typedef struct
{
	SOAP_SOCKET sock_queue; // The global request queue of sockets
	long ip;
}Queue;

Queue queue[MAX_QUEUE];

int head = 0, tail = 0; // Queue head and tail
void *process_queue(void*);
int enqueue(SOAP_SOCKET sock, long ip);
void dequeue(soap * soap);
pthread_mutex_t queue_cs;
pthread_cond_t queue_cv;

long   __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
	printf("Error   address   %x\n",excp->ExceptionRecord->ExceptionAddress);
	printf("CPU   register:\n");
	printf("eax   %x   ebx   %x   ecx   %x   edx   %x\n",excp->ContextRecord->Eax,
	excp->ContextRecord->Ebx,excp->ContextRecord->Ecx,
	excp->ContextRecord->Edx);
	return   EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char **argv)
{
	SetUnhandledExceptionFilter(callback);
	connectMySQL(m_sql);
	//Reg();
	p_plug_ctrl = new c_plug_ctrl(".");

	struct soap soap;
	soap_init(&soap);

	struct soap *soap_thr[MAX_THR]; // each thread needs a runtime context
	pthread_t tid[MAX_THR];
	int port = 8081; // first command-line arg is port
	SOAP_SOCKET m, s;
	int i;

	string sql = "SELECT SeverPort FROM xxdb_info_base WHERE id = 1";
	int ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) port = atoi(row[0]);
		}
		mysql_free_result(result);
	}
	printf("==== SERVER PORT = %d ====\n", port);

	soap_set_mode(&soap, SOAP_C_UTFSTRING);
	soap.fget = http_get;
	soap.version = 1.1;
	soap_set_namespaces(&soap, namespaces);

	m = soap_bind(&soap, NULL, port, BACKLOG);
	if (!soap_valid_socket(m))
	exit(1);
	fprintf(stderr, "Socket connection successful %d\n", m);
	pthread_mutex_init(&queue_cs, NULL);
	pthread_cond_init(&queue_cv, NULL);

	for (i = 0; i < MAX_THR; i++)
	{
		soap_thr[i] = soap_copy(&soap);
		soap_thr[i]->inxml = new char[SOUP_XML_SIZE];
		//fprintf(stderr, "Starting thread %d\n", i);
		pthread_create(&tid[i], NULL, (void*(*)(void*))process_queue, (void*)soap_thr[i]);
	}

	while(1)
	{
		s = soap_accept(&soap);
		if (!soap_valid_socket(s))
		{
			if (soap.errnum)
			{
				soap_print_fault(&soap, stderr);
				continue; // retry
			}
			else
			{
				fprintf(stderr, "Server timed out\n");
				break;
			}
		}
		//fprintf(stderr, "Thread %d accepts socket %d connection from IP %d.%d.%d.%d\n", i, s, (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF);
		//fprintf(stderr, "IP %d.%d.%d.%d\n", (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF);
		while (enqueue(s, soap.ip) == SOAP_EOM)
		Sleep(1000);
	}
	for (i = 0; i < MAX_THR; i++)
	{
		while (enqueue(SOAP_INVALID_SOCKET, 0) == SOAP_EOM)
			Sleep(1000);
	}
	for (i = 0; i < MAX_THR; i++)
	{
		fprintf(stderr, "Waiting for thread %d to terminate... ", i);
		pthread_join(tid[i], NULL);
		fprintf(stderr, "terminated\n");
		soap_done(soap_thr[i]);
		free(soap_thr[i]);
	}
	pthread_mutex_destroy(&queue_cs);
	pthread_cond_destroy(&queue_cv);

	soap_done(&soap);

	//cout << "Server close!" << endl;
	printf("Server close!\n");

	delete p_plug_ctrl;
	p_plug_ctrl = 0;
	return 0;
}
void *process_queue(void *soap)
{
   struct soap *tsoap = (struct soap*)soap;
   for (;;)
   {
	  dequeue(tsoap);
	  //printf("sock = %d\n", tsoap->socket);
	  //printf("ip = %d.%d.%d.%d\n", (tsoap->ip >> 24)&0xFF, (tsoap->ip >> 16)&0xFF, (tsoap->ip >> 8)&0xFF, tsoap->ip&0xFF);
      if (!soap_valid_socket(tsoap->socket))
         break;
      soap_serve(tsoap);
      soap_destroy(tsoap);
      soap_end(tsoap);
      //fprintf(stderr, "served\n");
   }
   return NULL;
}
int enqueue(SOAP_SOCKET sock, long ip)
{
   int status = SOAP_OK;
   int next;
   pthread_mutex_lock(&queue_cs);
   next = tail + 1;
   if (next >= MAX_QUEUE)
      next = 0;
   if (next == head)
      status = SOAP_EOM;
   else
   {
      queue[tail].sock_queue = sock;
      queue[tail].ip = ip;
      //printf("[enqueue] sock[%d] = %d\n", tail, sock);
      //printf("[enqueue] ip[%d] = %d.%d.%d.%d\n", tail, (ip >> 24)&0xFF, (ip >> 16)&0xFF, (ip >> 8)&0xFF, ip&0xFF);
      tail = next;
   }
   pthread_cond_signal(&queue_cv);
   pthread_mutex_unlock(&queue_cs);
   return status;
}

void dequeue(soap * soap)
{
   pthread_mutex_lock(&queue_cs);
   while (head == tail)       pthread_cond_wait(&queue_cv, &queue_cs);
   soap->socket = queue[head].sock_queue;
   soap->ip = queue[head].ip;
   //printf("[dequeue] sock[%d] = %d\n", head, queue[head].sock_queue);
   //printf("[dequeue] ip[%d] = %d.%d.%d.%d\n", head, (queue[head].ip >> 24)&0xFF, (queue[head].ip >> 16)&0xFF, (queue[head].ip >> 8)&0xFF, queue[head].ip&0xFF);
   head++;
   if (head >= MAX_QUEUE)
      head = 0;
   pthread_mutex_unlock(&queue_cs);
}
