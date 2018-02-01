/*
 * WebSearchClient.h
 *
 *  Created on: 2014-7-15
 *      Author: XXSP
 */

#ifndef WEBSEARCHCLIENT_H_
#define WEBSEARCHCLIENT_H_

#include "WebClient.h"
#include "soapH.h"
#include "webservice.nsmap"
#include <vector>
#include <time.h>
#include <pthread.h>

using namespace std;

#define rqsize 256
#define rusize 1024 * 512
typedef struct
{
	pthread_mutex_t tlock;
	time_t timestamp;
	int beused;			// 是否可用 0可用
	int netstation;		//网络状态，0正常，0以下不可用
	struct soap soap;
	char ip[18];
	char port[18];
	char devid[32];
} Server, *pServer;


class WebSearchClient {
public:
	vector<pServer> serverlist;
	pthread_t pthNetThead;

public:
	WebSearchClient();
	virtual ~WebSearchClient();

public:
	int getCarInfo(char *devid, char *platenum, char *platetype, char *Verify, char *outxml);
	int getVerify(char* devid, char * picdata, int & picsize);
	int cancelSearch(char *devid);

private:
	int getServerList();
	int freeServerList();
	pServer getFreeServer(char *devid);
	int getWebCarInfo();
	int getWebVerify(pServer ps, char * picdata, int &picsize);
};

void * xxWebInfoInit(void ** arg)
{
	if(*arg == NULL)
		*arg = new WebSearchClient;
	return *arg;
}

int xxGetCarInfo(void * arg, char *devid, char *platenum, char *platetype, char *Verify, char *outxml)
{
	WebSearchClient * web = (WebSearchClient *) arg;

	return web->getCarInfo(devid, platenum, platetype, Verify, outxml);
}

int xxGetVerify(void * arg, char* devid, char * picdata, int & picsize)
{
	WebSearchClient * web = (WebSearchClient *) arg;

	return web->getVerify(devid, picdata, picsize);
}

void xxCancelSearch(void * arg, char * devid)
{
	WebSearchClient * web = (WebSearchClient *) arg;
	web->cancelSearch(devid);
}

void xxDestroy(void * arg)
{
	WebSearchClient * web = (WebSearchClient *) arg;
	if(web != NULL)
		delete web;
}

#endif /* WEBSEARCHCLIENT_H_ */
