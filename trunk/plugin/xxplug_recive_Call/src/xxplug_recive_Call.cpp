/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */

#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include <windows.h>
#include <stdio.h>
#include <winsock2.h>
#include <vector>

#include "xxplug_recive_Call.h"

using namespace std;

#define bzero(x,y) memset(x, 0, y)

MYSQL * m_sql = 0;
string gresXML;
string username = "";
vector<string> cmdList;
int clientCount = 0;

void * SelectProc (void * lpParameter)
{
    if (NULL == lpParameter)
    {
        printf("invalid thread param!\n");
        return (void *)-1;
    }

    MYSQL * p_sql = NULL;
    SOCKET sAccept = *(SOCKET*)(lpParameter);
    char szRecv[4096] = {0};

    connectMySQL(p_sql);

    fd_set fdRead;
    timeval time;
    time.tv_sec = 0;
    time.tv_usec = 1000000;

    for ( ; ;)
    {
        FD_ZERO(&fdRead);
        FD_SET(sAccept, &fdRead);
        int nRet = select(sAccept+1, &fdRead, NULL, NULL, &time);
        //printf("nRet select = %d\n", nRet);
        if (nRet < 0)
        {
            DWORD dwLastErr = WSAGetLastError();
            printf("select socket error! err: %u\n", dwLastErr);
            break;
        }

        if (FD_ISSET(sAccept,&fdRead))
        {
            //printf("select a read socket\n");
        	memset(szRecv, 0, 4096);
            nRet = recv(sAccept, szRecv, 4, 0);
            if (0 >= nRet)
            {
                printf("recv data error! nRet = %d\n", nRet);
                goto _END;
            }
            int len = *(int *) szRecv;

            memset(szRecv, 0, 4096);
            nRet = recv(sAccept, szRecv, len, 0);
            if (0 >= nRet)
            {
                printf("recv data error! nRet = %d\n", nRet);
                goto _END;
            }
            printf("recv data: %s\n", szRecv);

            if(strstr(szRecv, "XT") != NULL)
            {
            	len = strlen("XTACK");
    			nRet = send((unsigned int)sAccept, (const char *)&len, 4, 0);
    			if (0 >= nRet)
    			{
    				printf("send data error! nRet = %d\n", nRet);
    				goto _END;
    			}

    			nRet = send((unsigned int)sAccept, "XTACK", len, 0);
    			if (0 >= nRet)
    			{
    				printf("send data error! nRet = %d\n", nRet);
    				goto _END;
    			}
            }
        }

        //printf("cmd Count = %d\n", cmdList.size());
        for(vector<string>::iterator iter = cmdList.begin(); iter != cmdList.end(); iter++)
        {
        	int len = (*iter).size();
			nRet = send((unsigned int)sAccept, (const char *)&len, 4, 0);
			if (0 >= nRet)
			{
				printf("send data error! nRet = %d\n", nRet);
				goto _END;
			}

			nRet = send((unsigned int)sAccept, (*iter).c_str(), len, 0);
			if (0 >= nRet)
			{
				printf("send data error! nRet = %d\n", nRet);
				goto _END;
			}
        }
        cmdList.clear();

        Sleep(100);
    }

 _END:

    if (INVALID_SOCKET != sAccept)
    {
        closesocket(sAccept);
        sAccept = INVALID_SOCKET;
    }

    printf("thread exit\n");
    //退出时将客户端数量标记减一
    clientCount--;
    return (void *)0;
}


void * mainThread(void * arg)
{
    WSADATA wsaData;
    DWORD dwErr = 0;
    pthread_t pSelectPro;
    int ret = 0;

    dwErr = WSAStartup(MAKEWORD(0x2, 0x2), &wsaData);
    if (0 != dwErr)
    {
        printf("winsock init error!\n");
        return (void *)-1;
    }

    if (0x2 != LOBYTE(wsaData.wVersion) ||
        0x2 != HIBYTE(wsaData.wVersion))
    {
        // couldn't find a usable WinSock DLL.
        printf("request winsock version error!\n");
        WSACleanup();
        return (void *)-1;
    }

    SOCKET sAccept = INVALID_SOCKET;
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sListen)
    {
        printf("create a socket error!\n");
        WSACleanup();
        return (void *)-1;
    }

    sockaddr_in servAddr;
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9561);

    sockaddr_in clientAddr;
    bzero(&clientAddr, sizeof(clientAddr));
    int nLen = sizeof(clientAddr);

    if (SOCKET_ERROR == bind(sListen, (struct sockaddr*)&servAddr, sizeof(servAddr)))
    {
        printf("bind socket error!\n");
        dwErr = WSAGetLastError();
        goto _QUIT;
    }

    if (SOCKET_ERROR == listen(sListen, SOMAXCONN))
    {
        printf("listen socket error!\n");
        dwErr = WSAGetLastError();
        goto _QUIT;
    }

    printf("listen......\n");

    while (1)
    {
    	printf("Start to Accept!\n");
        sAccept = accept(sListen, (sockaddr *)&clientAddr, &nLen);
        if (INVALID_SOCKET == sAccept)
        {
            printf("accept a socket error!\n");
            dwErr = WSAGetLastError();
            break;
        }
        printf("accept a socket\n");

        //判断客户端数量是否为0  确保只有一个客户端运行
        if(clientCount == 0)
        {

			ret = pthread_create(&pSelectPro, NULL, SelectProc, &sAccept);
			if (ret != 0)
			{
				printf("create thread error!\n");
				dwErr = -1;
				goto _QUIT;
			}
			else
				printf("a new client!\n");

			clientCount++;
        }
        else
        {
        	printf("Already have a Client!\n");
        	closesocket(sAccept);
        }
    }


_QUIT:

    if (INVALID_SOCKET != sListen)
    {
        closesocket(sListen);
        sListen = INVALID_SOCKET;
    }

    WSACleanup();

    printf("======mainThread Quit======\n");
    return (void *)0;
}


int xx_rcv_gettype()
{
	printf("--xxplug_recive_Call--\n");
	pthread_t pmainThread;
	
	connectMySQL(m_sql);

	pthread_create(&pmainThread, NULL, mainThread, NULL);

	return TYPE;
}

int CheckUsername(cxx_xml xx_xml, char ** outxmldata)
{
	int checking = 0;
	string sql = "SELECT username from xxdb_info_deviceuser WHERE userlogintag = '";
	sql += xx_xml.xpath_getprop("/userdata/head/user", "value");
	sql += "'";

	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			username = row[0];
			checking = 1;
		}
		else
		{
			printf("%s ", xx_xml.xpath_getprop("/userdata/head/user", "value"));
			checking = 0;
		}
		mysql_free_result(result);
	}
	else
		checking = 0;

	if(checking == 0)
	{
		string tmp1 = "<resultdata></resultdata>";
		setResXML(TYPESTR, 0, 0, 0, tmp1, gresXML);
		*outxmldata = (char *) gresXML.c_str();
		return -1;
	}

	return 0;
}

int xx_rcv_procinfo(char * inxmldata, char ** outxmldata)
{
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);

	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf(" %s.%s Checking Username error!IP:%s\n", TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));
		return -1;
	}

	printf("%s *** %s.%s *** IP: %s\n", username.c_str(), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	printf("INXML: %s\n", inxmldata);

//Web
	//通话请求
	if (str.compare("0001") == 0)
		return CallStart(xx_xml, outxmldata);

	//通话结束
	if (str.compare("0002") == 0)
		return CallStop(xx_xml, outxmldata);

	return 0;
}
