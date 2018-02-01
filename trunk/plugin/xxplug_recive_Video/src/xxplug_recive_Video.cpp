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
#include <xxxml.h>
#include <windows.h>
#include "xxplug_recive_Video.h"

using namespace std;

MYSQL * m_sql = 0;
string gresXML;
string username = "";
vector<SPServer> svrList;
string LocalIP = "";
string PhoneIP = "";
string WebIP = "";
string videopath = "";
string SvrPort = "";
string picdir = "";

#define bzero(x,y) memset(x, 0, y)

int xx_rcv_gettype()
{
	printf("--xxplug_recive_Video--\n");
	connectMySQL(m_sql);

	string sql = "SELECT LocalIP,picdirectory,SeverPort,PhoneIP,WebIP,picdirectory FROM xxdb_info_base";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			LocalIP.append(row[0]);
			videopath.append(row[1]);
			SvrPort.append(row[2]);
			PhoneIP.append(row[3]);
			WebIP.append(row[4]);
			picdir.append(row[5]);
		}
		mysql_free_result(result);
	}
	printf("SvrPort = %s\n", SvrPort.c_str());
	printf("LocalIP = %s\n", LocalIP.c_str());
	printf("PhoneIP = %s\n", PhoneIP.c_str());
	printf("WebIP = %s\n", WebIP.c_str());
	printf("picdir = %s\n", picdir.c_str());

	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=TRUE;

	PROCESS_INFORMATION pi;//必备参数设置结束

	CreateProcess(TEXT(".\\live555MediaServer.exe"),
	"live555MediaServer.exe",
	NULL,
	NULL,
	FALSE,
	CREATE_NEW_CONSOLE,
	NULL,
	NULL,
	&si,
	&pi
	);

	return TYPE;
}

int CheckUsername(cxx_xml xx_xml, char ** outxmldata)
{
	int checking = 0;
	string str = xx_xml.xpath_getprop("/userdata/head/user", "value");

	if(str.compare("pcclient") == 0)
		return 0;
	if(str.compare("web") == 0)
		return 0;
	if(str.compare("server") == 0)
		return 0;

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

	if(str.compare("0001") == 0)
		return PhoneLogin(xx_xml, outxmldata);

	if(str.compare("0002") == 0)
		return GetUserList(xx_xml, outxmldata);

	if(str.compare("0003") == 0)
		return StartVideo(xx_xml, outxmldata);

	if(str.compare("0005") == 0)
		return StopVideo(xx_xml, outxmldata);

	if(str.compare("0006") == 0)
		return GetVideoFileList(xx_xml, outxmldata);

	return 0;
}
