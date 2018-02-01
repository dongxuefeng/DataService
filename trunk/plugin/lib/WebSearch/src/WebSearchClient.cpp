/*
 * WebSearchClient.cpp
 *
 *  Created on: 2014-7-15
 *      Author: XXSP
 */

#include "WebSearchClient.h"
#include "xxxml.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <xxbase64.h>
#include <dbmepolice.h>

using namespace std;

int g_Quit = 0;

char b64test[512*3] = {0};
int testNet(pServer ps)
{
	char tmp[128] = {0};
	string rq = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
				<userdata>\
				<head>\
				<type value=\"0001\"/><user value=\"\"/><dev value=\"\"/><Request_Index value=\"\"/>\
				</head>\
				<data>\
				<type value=\"0003\"/>";
	rq += "</data></userdata>";
	string ru;

	memset(b64test, 0, 512*3);
	b64_encode_string((const unsigned char *)rq.c_str(), rq.size(), (unsigned char *)b64test);
	rq = b64test;

	sprintf(tmp, "http://%s:%s", ps->ip, ps->port);
	soap_call_ns2__rcvautoinfo(&ps->soap, tmp, "", rq, ru);

	if (ps->soap.error)
	{
		printf("Server is not online! IP = %s, port = %s\n", ps->ip, ps->port);
		soap_print_fault(&ps->soap, stderr);
		ps->netstation = 1;
		ps->beused = 0;
		memset(ps->devid, 0, 32);
		return -1;
	}
	else
	{
		ps->netstation = 0;
	}

	return 0;
}

void * netThread(void * arg)
{
	printf("netThread START!\n");
	vector<pServer> * pSvrList = (vector<pServer> *)arg;
	vector<pServer>::iterator iter;

	while(!g_Quit)
	{
		for(iter = pSvrList->begin(); iter != pSvrList->end(); iter++)
		{
			testNet(*iter);
			printf("devid = %s, IP = %s, use = %d, net = %d\n", (*iter)->devid, (*iter)->ip, (*iter)->beused, (*iter)->netstation);
			if((*iter)->beused == 1)
			{
				pthread_mutex_lock(&((*iter)->tlock));
				//printf("time : %d\n", time(0) - (*iter)->timestamp);
				if((time(0) - (*iter)->timestamp) >= 60)
				{
					//printf("");
					(*iter)->beused = 0;
					memset((*iter)->devid, 0, 32);
				}
				pthread_mutex_unlock(&((*iter)->tlock));
			}
		}
		Sleep(30000);
	}

	printf("netThread END!\n");
	return (void *)0;
}

char * findChildValue(const char * name, const char * endname, const char * xml, int num)
{
	static char val[1024*1024*10];
	char *p, *q;

	memset(val, 0, 1024*1024*10);
	for(int i = 0; i < num; i++)
	{
		if(i == 0)
			p = strstr(xml, name);
		else
		{
			p += strlen(name);
			p = strstr(p, name);
		}
	}
	if(p != NULL)
	{
		p += strlen(name);
		q = strstr(p, endname);
		if(p != q)
			memcpy(val, p, q-p);
	}
	return val;
}

WebSearchClient::WebSearchClient()
{
	this->getServerList();
	g_Quit = 0;
	pthread_create(&pthNetThead, NULL, netThread, (void *)&serverlist);
}

WebSearchClient::~WebSearchClient()
{
	g_Quit = 1;
	pthread_join(pthNetThead, NULL);
	this->freeServerList();
}

char b64send[512*3] = {0};
int WebSearchClient::getCarInfo(char* devid, char* platenum, char* platetype,
		char* Verify, char* outxml)
{
	pServer ps;
	for(vector<pServer>::iterator iter = serverlist.begin(); iter != serverlist.end(); iter++)
	{
		if(strcmp((*iter)->devid, devid) == 0)
		{
			ps = *iter;
			char tmp[128] = {0};
			string rq = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
						<userdata>\
						<head>\
						<type value=\"0001\"/><user value=\"\"/><dev value=\"\"/><Request_Index value=\"\"/>\
						</head>\
						<data>\
						<type value=\"0002\"/>";
			rq += new_strprop("maskpcode", "value", Verify);
			rq += new_strprop("platetype", "value", platetype);
			rq += new_strprop("platenum", "value", platenum);
			rq += "</data></userdata>";
			string ru;

			memset(b64send, 0, 512*3);
			b64_encode_string((const unsigned char *)rq.c_str(), rq.size(), (unsigned char *)b64send);
			rq = b64send;

			sprintf(tmp, "http://%s:%s", ps->ip, ps->port);
			soap_call_ns2__rcvautoinfo(&ps->soap, tmp, "", rq, ru);

			if (ps->soap.error)
			{
				soap_print_fault(&ps->soap, stderr);
				return -2;
			}
			else
			{
				cxx_xml xx_xml;
				xx_xml.readxml(ru.c_str());
				string str = xx_xml.xpath_getprop("/userdata/resultdata/item1/res", "value");
				if(str.compare("0") == 0)
				{
					string resXML = findChildValue("<resxml>", "</resxml>", ru.c_str(), 1);
					//printf("resXml = %s\n", resXML.c_str());
					sprintf(outxml, resXML.c_str());
					ps->beused = 0;
					memset(ps->devid, 0, 32);
					return 0;
				}
				else
				{
					return atoi(str.c_str());
				}
			}
		}
	}

	return -1;
}

int WebSearchClient::getVerify(char* devid, char * picdata, int & picsize)
{
	pServer ps;
	for(vector<pServer>::iterator iter = serverlist.begin(); iter != serverlist.end(); iter++)
	{
		if(strcmp((*iter)->devid, devid) == 0)
		{
			ps = *iter;
			return getWebVerify(ps, picdata, picsize);
		}
	}

	ps = getFreeServer(devid);
	if(ps != NULL)
	{
		return getWebVerify(ps, picdata, picsize);
	}

	printf("have no free Server!\n");

	return -1;
}

MYSQL * m_sql = 0;

int WebSearchClient::getServerList()
{
	connectMySQL(m_sql);

	string sql = "SELECT ip,port FROM xxdb_info_getsvr";
	int ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			printf("ip = %s, port = %s\n", row[0], row[1]);
			pServer ps = new Server;
			ps->beused = 0;
			ps->netstation = 0;
			sprintf(ps->ip, row[0]);			//IP从数据库获取
			sprintf(ps->port, row[1]);				//从服务器获取
			soap_init(&ps->soap);
			soap_set_mode(&ps->soap,SOAP_C_UTFSTRING);
			pthread_mutex_init(&ps->tlock, NULL);
			serverlist.push_back(ps);
		}
		mysql_free_result(result);
	}

	return 0;
}

pServer WebSearchClient::getFreeServer(char* devid)
{
	pServer ps;
	for(vector<pServer>::iterator iter = serverlist.begin(); iter != serverlist.end(); iter++)
	{
		printf("devid = %s, IP = %s, use = %d, net = %d\n", (*iter)->devid, (*iter)->ip, (*iter)->beused, (*iter)->netstation);
		if(((*iter)->beused == 0)&&((*iter)->netstation == 0))
		{
			ps = *iter;
			memset(ps->devid, 0, 32);
			sprintf(ps->devid, devid);
			ps->beused = 1;

			pthread_mutex_lock(&ps->tlock);
			time(&ps->timestamp);
			pthread_mutex_unlock(&ps->tlock);

			return ps;
		}
	}

	printf("Don not have free server!\n");

	return NULL;
}

int WebSearchClient::getWebCarInfo()
{
	return 0;
}

int WebSearchClient::freeServerList()
{
	for(unsigned int i = 0; i < serverlist.size(); i ++)
	{
		pthread_mutex_destroy(&serverlist.at(i)->tlock);
		delete serverlist.at(i);
	}
	serverlist.clear();
	return 0;
}

char b64out[512*3] = {0};
int WebSearchClient::getWebVerify(pServer ps, char * picdata, int &picsize)
{
	char tmp[128] = {0};
	string rq = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
		<userdata>\
		<head>\
		<type value=\"0001\"/><user value=\"\"/><dev value=\"\"/><Request_Index value=\"\"/>\
		</head>\
		<data>\
		<type value=\"0001\"/>\
		</data>\
		</userdata>";
	string ru;

	memset(b64out, 0, 512*3);
	b64_encode_string((const unsigned char *)rq.c_str(), rq.size()+1, (unsigned char *)b64out);
	rq = b64out;

	sprintf(tmp, "http://%s:%s", ps->ip, ps->port);
	soap_call_ns2__rcvautoinfo(&ps->soap, tmp, "", rq, ru);
	if (ps->soap.error)
	{
		printf("===== soap error =====\n");
		soap_print_fault(&ps->soap, stderr);
		return -2;
	}
	else
	{
		cxx_xml xx_xml;
		string pic;
		xx_xml.readxml(ru.c_str());

		string str = xx_xml.xpath_getprop("/userdata/resultdata/item1/res", "value");
		//printf("res = %s\n", xx_xml.xpath_getprop("/userdata/resultdata/item1/res", "value"));

		if(str.compare("0") == 0)
		{
			xx_xml.xpath_gettext("/userdata/resultdata/item1/photomask", pic);
			sprintf(picdata, pic.c_str());
			picsize = atoi(xx_xml.xpath_getprop("/userdata/resultdata/item1/photosize", "value"));

			pthread_mutex_lock(&ps->tlock);
			time(&ps->timestamp);
			pthread_mutex_unlock(&ps->tlock);

			return 0;
		}
		else
		{
			return atoi(str.c_str());
		}
	}
	return -1;
}

int WebSearchClient::cancelSearch(char *devid)
{
	pServer ps;
	for(vector<pServer>::iterator iter = serverlist.begin(); iter != serverlist.end(); iter++)
	{
		printf("cancelSearch devid = %s\n", devid);
		if(strcmp((*iter)->devid, devid) == 0)
		{
			printf("cancelSearch ip = %s\n", (*iter)->ip);
			ps = *iter;
			ps->beused = 0;
			memset(ps->devid, 0, 32);
			break;
		}
	}

	return 0;
}
