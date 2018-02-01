/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */
#include <vector>
#include "xxplug_recive_ReviceMessage.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xxxml.h>
#include <dbmepolice.h>
#include <time.h>
#include <pthread.h>
#include "chinese_code.h"
#include "xxlibpushclient.h"

using namespace std;

#define TYPE 8
#define TYPESTR "0008"

string gresXML = "";
using namespace std;
MYSQL * m_sql = 0;
string username = "";
void * SendMsg = NULL;
void * RcvMsg = NULL;
MYSQL * mysql;
string mqttIP = "";
string mqttPort = "";

typedef struct
{
	char username[40];
	char dev[40];
	int time;
}Online;

void * OnlineThread(void * arg)
{
	string sql = "";
	vector<Online> OnlineList;
	connectMySQL(mysql);

	while(1)
	{
		sql = "SELECT username,onlineTime,device FROM xxdb_info_deviceuser WHERE online = 1";
		if(xx_sql_query(mysql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(mysql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				Online online;
				memset(online.username, 0, 40);
				memset(online.dev, 0, 40);
				sprintf(online.username, row[0]);
				sprintf(online.dev, row[2]);
				online.time = atoi(row[1]);

				OnlineList.push_back(online);
			}
			mysql_free_result(result);
		}

		printf("======== online count = %d ========\n", OnlineList.size());
		for(vector<Online>::iterator iter = OnlineList.begin(); iter != OnlineList.end(); iter++)
		{
			if((time(0)-iter->time) > 120)
			{
				sql = "UPDATE xxdb_info_deviceuser SET online = 0 WHERE username = '";
				sql += iter->username;
				sql += "'";
				xx_sql_query(mysql, sql.c_str());
				printf("=== user:%s === drop line ===\n", iter->username);

				sql = "INSERT xxdb_data_devconnlog (datetime,devid,userid,type) VALUES (NOW(),'";
				sql += iter->dev;
				sql += "','";
				sql += iter->username;
				sql += "','Drop Line')";
				xx_sql_query(mysql, sql.c_str());
			}
		}

		if(OnlineList.size() > 0) OnlineList.clear();
		Sleep(30000);
	}

	return (void *)0;
}

int CheckDev(cxx_xml xx_xml, string &name)
{
	int checking = 0;

	string str = xx_xml.xpath_getprop("/root/dev", "value");
	string sql = "SELECT username from xxdb_info_deviceuser WHERE device = '";
	sql += str.c_str();
	sql += "'";
	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) name = row[0];
			checking = 0;
		}
		else
			checking = 1;
		mysql_free_result(result);
	}
	else
		checking = 1;

	return checking;
}

void mOn_Message(char * topic, int topic_len, char * message, int msg_len, void * obj)
{
	cxx_xml * xx_xml = NULL;
	time_t t;
	time(&t);

	//printf("topic = %s\n", topic);
	//printf("message = %s\n", message);

	if(strcmp(topic, "a/SvrRcvMsg") == 0)
	{
		xx_xml = new cxx_xml;
		xx_xml->readxml(message);

		string user;
		string dev = xx_xml->xpath_getprop("/root/dev", "value");
		int ret = CheckDev(*xx_xml, user);
		delete xx_xml;
		xx_xml = NULL;

		if(ret == 0)
		{
			int online = 0;
			string sql = "SELECT online FROM xxdb_info_deviceuser WHERE username = '";
			sql += user.c_str();
			sql += "'";
			//printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if ((row = mysql_fetch_row(result)))
				{
					online = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			if(online == 0)
			{
				sql = "INSERT xxdb_data_devconnlog (datetime,devid,userid,type) VALUES (NOW(),'";
				sql += dev;
				sql += "','";
				sql += user;
				sql += "','XT-Login')";
				//printf("SQL : %s\n", sql.c_str());
				xx_sql_query(mysql, sql.c_str());
			}

			time(&t);
			sql = "UPDATE xxdb_info_deviceuser SET online = 1, onlineTime =";
			sql += itoaa(t);
			sql += " WHERE username = '";
			sql += user.c_str();
			sql += "'";
			//printf("SQL : %s\n", sql.c_str());
			xx_sql_query(mysql, sql.c_str());
			printf("=== user:%s === online ===\n", user.c_str());
		}
	}
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

int xx_rcv_gettype()
{
	pthread_t t_OnlineThread;
	printf("--xxplug_recive_user--\n");
	connectMySQL(m_sql);

	string sql = "UPDATE xxdb_info_deviceuser SET online = 0";
	xx_sql_query(m_sql, sql.c_str());

	sql = "SELECT mqttIP,mqttPort FROM xxdb_info_base";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			mqttIP = row[0];
			mqttPort = row[1];
		}
		mysql_free_result(result);
	}

	SendMsg = Init((char *)"SvrSndMsg", (char *)mqttIP.c_str(), atoi(mqttPort.c_str()), mOn_Message);
	RcvMsg = Init((char *)"SvrRcvMsg", (char *)mqttIP.c_str(), atoi(mqttPort.c_str()), mOn_Message);

	subscribe(SendMsg, (char *)"a/SvrSndMsg");
	if(RcvMsg != NULL) subscribe(RcvMsg, (char *)"a/#");
	else printf("RcvMsg Init Error!\n");

	pthread_create(&t_OnlineThread, NULL, OnlineThread, NULL);

	return TYPE;
}

int xx_rcv_procinfo_InfoSend(cxx_xml xx_xml, char ** outxmldata)
{
	int ret = 0;
	char * tmp = new char[64];
	char * name = new char[64];
	string resxml = "<resultdata><item1>";
	string strtmp = "";
	string msgxml = "";
	string str = xx_xml.xpath_getprop("/userdata/data/text", "value");
	time_t t = time(&t);

	int count = atoi(xx_xml.xpath_getprop("/userdata/data/usercount", "value"));
	printf("==== count = %d\n", count);
	for(int i = 1; i <= count; i++)
	{
		strtmp = "";
		memset(tmp, 0, 64);
		sprintf(tmp, "/userdata/data/user%d", i);

		memset(name, 0, 64);
		sprintf(name, "a/%s", xx_xml.xpath_getprop(tmp, "value"));

		printf("------ SendMsg name = %s msg = %s\n", name, str.c_str());

		strtmp = "<resultdata><item1>";
		strtmp += new_strprop("msgtype", "value", "1");
		strtmp += "<text>";
		strtmp += str;
		strtmp += "</text>";
		strtmp += "</item1></resultdata>";
		setResXML("0009", 1, 1, 0, strtmp, msgxml);

		//send Msg
		publish(SendMsg, (char *)name, (char *)msgxml.c_str(), false);
	}

	//将消息数据插入数据库
	string sql = "INSERT xxdb_data_message (msg,msgtype,time,sole)VALUES('";
	sql += str;
	sql += "', 1, NOW(), '";
	sql += itoaa(t);
	sql += "')";

	printf("SQL:%s\n",sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML:%s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_SelectInfo(cxx_xml xx_xml, char ** outxmldata)
{
	int ret = 0;
	int count = 0;
	string stritem = "<resultdata>";
	string str = "";

	string sql = "SELECT msg,time FROM xxdb_data_message  ORDER BY TIME DESC LIMIT 0,10";
	ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			count++;
			str = new_strprop("text", "value", row[0]);
			str += new_strprop("time", "value", row[1]);
			stritem += setResItem(count, str);
		}
		mysql_free_result(result);
	}
	stritem += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, stritem, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("%s\n", *outxmldata);

	return ret;
}

int xx_rcv_procinfo_InfoSendAllocation(cxx_xml xx_xml, char ** outxmldata)
{
	string resxml = "<resultdata><item1>";

	string sql = "INSERT INTO xxdb_data_sndinfoallocation (deadstart,deadend,text,userList) VALUES (";
	sql += xx_xml.xpath_getprop("/userdata/data/deadstart", "value");
	sql += ",";
	sql += xx_xml.xpath_getprop("/userdata/data/deadend", "value");
	sql += ",'";
	sql += xx_xml.xpath_getprop("/userdata/data/text", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/nameList", "value");;
	sql += "')";
	//printf("SQL : %s\n", sql.c_str());
	int res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("%s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_SelectInfoSendAllocation(cxx_xml xx_xml, char ** outxmldata)
{
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int sumcount = 0;
	int count = 0;
	string resxml = "<resultdata>";
	string sql = "";

	sql = "SELECT COUNT(0) FROM xxdb_data_sndinfoallocation";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
			sumcount = atoi(row[0]);
		mysql_free_result(result);
	}

	if(sumcount > 0)
	{
		sql = "SELECT deadstart,deadend,text,userList FROM xxdb_data_sndinfoallocation LIMIT ";
		sql += itoaa(firstIndex);
		sql += ",5";

		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while ((row = mysql_fetch_row(result)))
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("deadstart", "value", row[0]);
				resxml += new_strprop("deadend", "value", row[1]);
				resxml += new_strprop("text", "value", row[2]);
				resxml += new_strprop("nameList", "value", row[3]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("XML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_DeleteInfoSendAllocation(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "DELETE FROM xxdb_data_sndinfoallocation WHERE deadstart = ";
	sql += xx_xml.xpath_getprop("/userdata/data/deadstart", "value");
	sql += " AND deadend = ";
	sql += xx_xml.xpath_getprop("/userdata/data/deadend", "value");

	int res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("%s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo11(char * inxmldata,char ** outxmldata)
{
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);
	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf("%s *** %s.%s *** Checking Username error!IP:%s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));
		return -1;
	}

	printf("%s *** %s.%s *** IP: %s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	printf("INXML:%s\n",inxmldata);
	int iresult = 0;
	if(str.compare("0001") == 0)
		iresult = xx_rcv_procinfo_InfoSend(xx_xml,outxmldata);
	if(str.compare("0002") == 0)
		iresult = xx_rcv_procinfo_InfoSendAllocation(xx_xml,outxmldata);
	if(str.compare("0003") == 0)
		iresult = xx_rcv_procinfo_SelectInfoSendAllocation(xx_xml,outxmldata);
	if(str.compare("0004") == 0)
		iresult = xx_rcv_procinfo_DeleteInfoSendAllocation(xx_xml,outxmldata);
	if(str.compare("0005") == 0)
		iresult = xx_rcv_procinfo_SelectInfo(xx_xml,outxmldata);

	return iresult;
}

int xx_rcv_procinfo(char * inxmldata,char ** outxmldata)
{
	return xx_rcv_procinfo11(inxmldata,outxmldata);
}
