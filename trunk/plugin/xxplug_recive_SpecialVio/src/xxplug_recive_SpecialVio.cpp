/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include "xxplug_recive_SpecialVio.h"
#include "xxlibpushclient.h"

using namespace std;

#define bzero(x,y) memset(x, 0, y)

MYSQL * m_sql = 0;
string gresXML;
string username = "";
string picpath = "";
void * SendMsg = NULL;
string mqttIP = "";
string mqttPort = "";

void * SendMsgThread(void * arg)
{
	string sql = "";
	string msgxml = "";
	string name = "";
	string str = "";

	while(1)
	{
		sql = "SELECT soleid,isPass,examin1,examin2,examin3,examin4,username FROM xxdb_data_specialvio WHERE isExamine = 1 AND sendflag = 0";
		printf("SQl : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				name = "a/";
				name += row[6];

				str = "<resultdata><item1>";
				str += new_strprop("msgtype", "value", "3");
				str += "<text>";
				str += new_strprop("soleid", "value", row[0]);
				str += new_strprop("rebukres", "value", row[1]);
				str += new_strprop("rebuksug1", "value", row[2]);
				str += new_strprop("rebuksug2", "value", row[3]);
				str += new_strprop("rebuksug3", "value", row[4]);
				str += new_strprop("rebuksug4", "value", row[5]);
				str += "</text>";
				str += "</item1></resultdata>";
				setResXML("0009", 1, 1, 0, str, msgxml);

				printf("specialvio : name=%s msg=%s\n", name.c_str(), msgxml.c_str());
				publish(SendMsg, (char *)name.c_str(), (char *)msgxml.c_str(), false);
				Sleep(100);
			}
			mysql_free_result(result);
		}
		Sleep(3000);
	}

	return (void *)0;
}

void mOn_Message(char * topic, int topic_len, char * message, int msg_len, void * obj)
{

}

int xx_rcv_gettype()
{
	printf("--xxplug_recive_SpecialVio--\n");

	connectMySQL(m_sql);

	string sql = "SELECT picdirectory FROM xxdb_info_base WHERE id = 1";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			picpath = "";
			picpath.append(row[0]);
		}
		mysql_free_result(result);
	}

	sql = "UPDATE xxdb_info_deviceuser SET online = 0";
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

	SendMsg = Init((char *)"SpecialVioMsg", (char *)mqttIP.c_str(), atoi(mqttPort.c_str()), mOn_Message);
	subscribe(SendMsg, (char *)"a/SpecialVioMsg");

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

	printf("INXML:%s\n", inxmldata);

	if (str.compare("0001") == 0)
		return insertExamine(xx_xml, outxmldata);
	if (str.compare("0002") == 0)
		return webGetAuto(xx_xml, outxmldata);
	if (str.compare("0003") == 0)
		return webSetAuto(xx_xml, outxmldata);
	if (str.compare("0004") == 0)
		return webGetExamine(xx_xml, outxmldata);
	if (str.compare("0005") == 0)
		return webSendResult(xx_xml, outxmldata);
	if (str.compare("0006") == 0)
		return uploadPic(xx_xml, outxmldata);
	if (str.compare("0007") == 0)
		return downloadPic(xx_xml, outxmldata);
	if (str.compare("0008") == 0)
		return callback(xx_xml, outxmldata);

	return 0;
}
