/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <time.h>
#include <pthread.h>
#include <xxxml.h>
#include <winsock2.h>
#include "xxplug_recive_GPS.h"

using namespace std;

MYSQL * m_sql = 0;
string gresXML;
string username = "";
#define bzero(x,y) memset(x, 0, y)
pthread_t dataSend;
WSADATA wsaData;
SOCKET sockClient;
int IsConnect = 0;
char SvrIP[16] = {0};
int  SvrPort = 5885;

//tcp send
int SendN(SOCKET *s, char *buf, int buflen)
{
	int sendlen = buflen;
	int n = 0;

	while (sendlen > 0)
	{
		n = send(*s, buf, sendlen, 0);

		if (n < 0)
		{
			printf("[sendn] send error!\n");
			return -1;
		}

		if (n == 0)
		{
			return (buflen - sendlen);
		}

		buf += n;
		sendlen -= n;
	}

	return buflen;
}

//tcp recv
int RecvN(SOCKET *s, char *buf, int buflen)
{
	int recvlen = buflen;
	int n = 0;

	while (recvlen > 0)
	{
		n = recv(*s, buf, recvlen, 0);

		if (n < 0)
		{
			printf("[recvn] recv error!\n");
			return -1;
		}

		if (n == 0)
		{
			return (buflen - recvlen);
		}

		buf += n;
		recvlen -= n;
	}

	return buflen;
}

void ConnectServer()
{
	SOCKADDR_IN addrServer;
	while(1)
	{
		sockClient = socket(AF_INET, SOCK_STREAM, 0);

		addrServer.sin_addr.S_un.S_addr = inet_addr(SvrIP);
		addrServer.sin_family = AF_INET;
		addrServer.sin_port = htons(SvrPort);
		if(connect(sockClient, (SOCKADDR *)&addrServer, sizeof(addrServer)) == 0)
		{
			printf("Connect OK!\n");
			IsConnect = 1;
			break;
		}
		else
		{
			//printf("Connect error! Server is not online!Server IP = %s，Server port = %d\n", SvrIP, SvrPort);
			closesocket(sockClient);
		}
		Sleep(20000);
	}
}

void setBuf(char * phone, char * time, char * x, char * y, char * height, char * bearing, string & str)
{
	/*static char tmp[1024] = {0};
	memset(tmp, 0, 1024);
	sprintf(tmp, "{EHL,A,02,%s,%s,%s,%s,%s,,%s,,,,4,,EHL}",
		phone, time, y, x, height, bearing);
	str = tmp;*/
	str = "{EHL,A,02,";
	if(phone != NULL) str += phone;
	str += ",";
	if(time != NULL) str += time;
	str += ",";
	if(y != NULL) str += y;
	str += ",";
	if(x != NULL) str += x;
	str += ",,";
	if(height != NULL) str += height;
	str += ",";
	if(bearing != NULL) str += bearing;
	str += ",,,,4,,EHL}";
}

void * DataSendThread(void * arg)
{
	string sql = "";
	string sendbuf = "";
	MYSQL * m_threadsql = 0;
	vector<int> idList;
	char ltime[32] = {0};

	connectMySQL(m_threadsql);

	while(1)
	{
		memset(ltime, 0, 32);
		time_t tme;
		tme = time(NULL) - 1800;

		tm *t = NULL;
		t = localtime(&tme);


		sprintf(ltime, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year - 100 + 2000, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

		if(IsConnect == 0) ConnectServer();
		sql = "SELECT d.phone , g.id, g.datetime, g.latitude, g.longitude, g.altitude, g.bearing, g.upflag  FROM xxdb_info_device d, xxdb_data_gpsdata g\
				WHERE g.deviceid = d.deviceid AND g. upflag = 0 AND datetime >= '";
		sql += ltime;
		sql += "' ORDER BY g.datetime asc";
	    //printf("gps sql = %s\n", sql.c_str());
		if(xx_sql_query(m_threadsql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_threadsql);
			MYSQL_ROW row;
	
			while((row = mysql_fetch_row(result)))
			{
				setBuf(row[0],row[2],row[3],row[4],row[5],row[6],sendbuf);
				if(SendN(&sockClient, (char *)sendbuf.c_str(), sendbuf.size()) == -1)
				{
					IsConnect = 0;
					closesocket(sockClient);
					mysql_free_result(result);
					goto end;
				}
				idList.push_back(atoi(row[1]));

				Sleep(25);
			}
			//printf("Send GPS Data OK! count = %d\n", idList.size());
			mysql_free_result(result);
		}
end:
		for(vector<int>::iterator ite = idList.begin(); ite != idList.end(); ite++)
		{
			sql = "UPDATE xxdb_data_gpsdata SET upflag = 1 WHERE id = ";
			sql += itoaa(*ite);
			xx_sql_query(m_threadsql, sql.c_str());
		}
		idList.clear();
		Sleep(10000);
	}

	return (void *)0;
}

int xx_rcv_gettype()
{
	printf("--xxplug_recive_GPS--\n");
	IsConnect = 0;
	DWORD dwErr = WSAStartup(MAKEWORD(0x2, 0x2), &wsaData);
	if (0 != dwErr)
	{
		printf("winsock init error!\n");
		return -1;
	}

	if (0x2 != LOBYTE(wsaData.wVersion) ||
		0x2 != HIBYTE(wsaData.wVersion))
	{
		// couldn't find a usable WinSock DLL.
		printf("request winsock version error!\n");
		WSACleanup();
		return -1;
	}

	pthread_create(&dataSend, NULL, DataSendThread, NULL);
	connectMySQL(m_sql);

	memset(SvrIP, 0, 16);
	string sql = "SELECT IP FROM xxdb_info_base WHERE id = 1";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) strcpy(SvrIP, row[0]);
			else	strcpy(SvrIP, "127.0.0.1");
		}
		mysql_free_result(result);
	}
	//sprintf(SvrIP, "123.138.153.237");
	printf("SvrIP = %s\n", SvrIP);

	sql = "SELECT port FROM xxdb_info_base WHERE id = 1";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) SvrPort = atoi(row[0]);
			else	SvrPort = 5885;
		}
		mysql_free_result(result);
	}
	printf("SvrPort = %d\n", SvrPort);

	return TYPE;
}

int CheckUsername(cxx_xml xx_xml, char ** outxmldata)
{
	int checking = 0;
	string str = xx_xml.xpath_getprop("/userdata/head/user", "value");
	string sql = "SELECT username from xxdb_info_deviceuser WHERE userlogintag = '";
	sql += str.c_str();
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
			printf("%s ", str.c_str());
			checking = 0;
		}
		mysql_free_result(result);
	}
	else
		checking = 0;

	if(checking == 0)
	{
		string tmp1 = "<resultdata></resultdata>";
		string res;
		setResXML(TYPESTR, 0, 0, 0, tmp1, res);
		*outxmldata = (char *) res.c_str();

		return -1;
	}

	return 0;
}

int InserGPSData(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "";
	string sql = "";
	string str = "";
	char tmp[50] = {0};
	int count = 0;
	int ret = 0;

	count = atoi(xx_xml.xpath_getprop("/userdata/data/count", "value"));
	//printf("count = %d\n", count);
	if(count > 0)
	{
		for(int i = 1; i <= count; i++)
		{
//			printf("i = %d\n", i);
			memset(tmp, '\0', 50);

			sprintf(tmp, "/userdata/data/item%d/", i);
			sql = "INSERT INTO xxdb_data_gpsdata (DATETIME, provider, latitude, longitude, altitude, bearing, jobs, userid, deviceid )VALUES(";
			str = tmp;
			str += "datetime";

			sql += "'";
			sql += xx_xml.xpath_getprop(str.c_str(), "value");
			sql += "',";

			str = tmp; str += "provider";
			sql += "'";
			sql += xx_xml.xpath_getprop(str.c_str(), "value");
			sql += "',";

			str = tmp; str += "latitude";
			sql += "'";
			sql += xx_xml.xpath_getprop(str.c_str(), "value");
			sql += "',";

			str = tmp; str += "longitude";
			sql += "'";
			sql += xx_xml.xpath_getprop(str.c_str(), "value");
			sql += "',";

			str = tmp; str += "altitude";
			sql += "'";
			sql += xx_xml.xpath_getprop(str.c_str(), "value");
			sql += "',";

			str = tmp; str += "bearing";
			sql += "'";
			sql += xx_xml.xpath_getprop(str.c_str(), "value");
			sql += "',";

			sql += "'',";

			sql += "'";
			sql += username;
			sql += "',";

			sql += "'";
			sql += xx_xml.xpath_getprop("/userdata/head/dev", "value");
			sql += "')";

			ret = xx_sql_query(m_sql, sql.c_str());
		}
	}

	strxml = "<resultdata><item1>";
	strxml += new_strprop("res", "value", itoaa(ret));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf(*outxmldata);

	return ret;
}

int SelectGPSDate(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "";
	string stritem = "";
	string sql = "";
	string str = "";
	int ret = 0;
	int count = 0;

	sql = "SELECT datetime,provider,latitude,longitudem,altitude,bearing FROM xxdb_data_gpsdata WHERE 1=1 ";

	str = xx_xml.xpath_getprop("/userdata/data/user", "value");
	if(str.compare("") != 0)
	{
		sql += " AND userid = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/devid", "value");
	if(str.compare("") != 0)
	{
		sql += " AND deviceid = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/sdatetime", "value");
	if(str.compare("") != 0)
	{
		sql += " AND datetime >= '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/edatetime", "value");
	if(str.compare("") != 0)
	{
		sql += " AND datetime <= '";
		sql += str;
		sql += "'";
	}

	strxml = "<resultdata>";
	ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while((row = mysql_fetch_row(result)))
		{
			count++;
			stritem = new_strprop("datetime", "value", row[0]);
			stritem += new_strprop("provider", "value", row[1]);
			stritem += new_strprop("latitude", "value", row[2]);
			stritem += new_strprop("longitude", "value", row[3]);
			stritem += new_strprop("altitude", "value", row[4]);
			stritem += new_strprop("bearing", "value", row[5]);
			strxml += setResItem(count, stritem);
		}
		mysql_free_result(result);
	}
	strxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, strxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();

	return ret;
}

int webSelectAllGPSDate(cxx_xml xx_xml, char ** outxmldata)
{
	string resxml = "<resultdata>";
	string stritem = "";
	string sql = "";
	string str = "";
	int ret = 0;
	int count = 0;
	char stime[40] = {0};
	char etime[40] = {0};
	struct tm *local_time = NULL;
	time_t utc_time;

	utc_time = time(0);

	local_time = localtime(&utc_time);
	strftime(etime, sizeof(etime), "%Y-%m-%d %H:%M:%S", local_time);
	printf("etime = %s\n", etime);

	utc_time = utc_time - 86400;
	local_time = localtime(&utc_time);
	strftime(stime, sizeof(stime), "%Y-%m-%d %H:%M:%S", local_time);
	printf("stime = %s\n", stime);

	sql = "SELECT a.`datetime`,a.userid,a.latitude,a.longitude,a.altitude,a.bearing,b.name FROM (SELECT * FROM xxdb_data_gpsdata WHERE `datetime` BETWEEN '";
	sql += stime;
	sql += "' AND '";
	sql += etime;
	sql += "' ORDER BY `id` DESC ) a LEFT JOIN xxdb_info_deviceuser b ON a.userid = b.username GROUP BY userid";

	printf("sql : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while((row = mysql_fetch_row(result)))
		{
			count++;
			stritem = new_strprop("datetime", "value", row[0]);
			stritem += new_strprop("provider", "value", row[1]);
			stritem += new_strprop("latitude", "value", row[2]);
			stritem += new_strprop("longitude", "value", row[3]);
			stritem += new_strprop("altitude", "value", row[4]);
			stritem += new_strprop("bearing", "value", row[5]);
			stritem += new_strprop("realname", "value", row[6]);
			resxml += setResItem(count, stritem);
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return ret;
}

int webGetTrajectoryGPSDate(cxx_xml xx_xml, char ** outxmldata)
{
	string resxml = "<resultdata>";
	string stritem = "";
	string sql = "";
	string str = "";
	int ret = 0;
	int count = 0;

	sql = "SELECT datetime,userid,latitude,longitudem,altitude,bearing FROM xxdb_data_gpsdata WHERE 1=1 ";
	sql += " AND userid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/user", "value");
	sql += "'";
	sql += " AND datetime >= '";
	sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
	sql += "'";
	sql += " AND datetime <= '";
	sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
	sql += "'";

	printf("sql : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while((row = mysql_fetch_row(result)))
		{
			count++;
			stritem = new_strprop("datetime", "value", row[0]);
			stritem += new_strprop("provider", "value", row[1]);
			stritem += new_strprop("latitude", "value", row[2]);
			stritem += new_strprop("longitude", "value", row[3]);
			stritem += new_strprop("altitude", "value", row[4]);
			stritem += new_strprop("bearing", "value", row[5]);
			resxml += setResItem(count, stritem);
		}
		mysql_free_result(result);
	}
	resxml += "</resultdata>";

	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();

	printf("OUTXML : %s\n", *outxmldata);

	return ret;
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

	printf("%s *** %s.%s *** IP: %s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	if (str.compare("0001") == 0)
		return InserGPSData(xx_xml, outxmldata);

	if (str.compare("0002") == 0)
		return SelectGPSDate(xx_xml, outxmldata);

	if (str.compare("0003") == 0)
		return webSelectAllGPSDate(xx_xml, outxmldata);

	if (str.compare("0004") == 0)
		return webGetTrajectoryGPSDate(xx_xml, outxmldata);

	return 0;
}
