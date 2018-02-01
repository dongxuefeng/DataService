/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */

#include <list>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xxplug_recive_SearchInfo.h"
#include <xxxml.h>
#include <dbmepolice.h>
#include "getinfo/getinfo.h"
#include "webSearch/WebClient.h"

using namespace std;

#define TYPE 0001
#define bzero(x,y) memset(x, 0, y)

MYSQL * m_sql = 0;
string gresXML;
string username = "";
void * web = NULL;

int datafrom = 0;
int xx_rcv_gettype()
{
	printf("--xxplug_recive_SearchInfo--\n");
	connectMySQL(m_sql);
    //查询数据库表 xxdb_info_base 中数据来源 请求数据方式：0接口 1网页外挂
	string sql = "SELECT datafrom FROM xxdb_info_base LIMIT 0, 1";
	int res = xx_sql_query(m_sql, sql.c_str());
	if(res == 0)
	{
		MYSQL_RES *resule = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(resule)))
		{
			datafrom = atoi(row[0]);
		}
		mysql_free_result(resule);
	}
	else
	{
		printf("mysql_fetch_row error!");
		datafrom = 0;
	}

	printf("------------- datafrom = %d -------------\n", datafrom);

	InitSoap();
	xxWebInfoInit(&web);
	return TYPE;
}
//使用用户验证码查询用户名
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
			printf("mysql_fetch_row error!");
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

	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf("Checking Username error!\n");
		return -1;
	}

	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");
	printf("***** procxml *** %s.%s *****\n", TYPESTR, str.c_str());

	printf("INXML : %s\n", inxmldata);

	string sql = "INSERT INTO xxdb_data_searchlog (`user`,`type`,`xml`,`data`)VALUES('";
	sql += username;
	sql += "','";
	sql += str;
	sql += "','";
	//车辆基本信息
	if (str.compare("0001") == 0)
	{
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += " ";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "',NOW())";
		xx_sql_query(m_sql, sql.c_str());
		return xx_rcv_getcarinfo(xx_xml, outxmldata, datafrom);
	}

	//驾驶证基本信息
	if (str.compare("0002") == 0)
	{
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += " ";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "',NOW())";
		xx_sql_query(m_sql, sql.c_str());
		return xx_rcv_getdriverinfo(xx_xml, outxmldata, datafrom);
	}

	//车辆违法信息
	if (str.compare("0003") == 0)
	{
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += " ";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "',NOW())";
		xx_sql_query(m_sql, sql.c_str());
		return selectviodata(xx_xml, outxmldata, datafrom);
	}

	if (str.compare("0005") == 0)
	{
		sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
		sql += "',NOW())";
		xx_sql_query(m_sql, sql.c_str());
		return selectviodatapic(xx_xml, outxmldata, datafrom);
	}

	if (str.compare("0006") == 0)
	{
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += " ";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "',NOW())";
		xx_sql_query(m_sql, sql.c_str());
		return getcarpic(xx_xml, outxmldata, datafrom);
	}

	if (str.compare("0007") == 0)
	{
		sql += xx_xml.xpath_getprop("/userdata/data/driverlicense", "value");
		sql += "',NOW())";
		xx_sql_query(m_sql, sql.c_str());
		return getdriverpic(xx_xml, outxmldata, datafrom);
	}

	if (str.compare("0010") == 0)
	{
		return sendPIN(xx_xml, outxmldata);
	}

	if (str.compare("0011") == 0)
	{
		return getPinPic(xx_xml, outxmldata);
	}

	if (str.compare("0012") == 0)
	{
		return CancelSearch(xx_xml, outxmldata);
	}

	return 0;
}
