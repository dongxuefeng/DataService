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
#include "xxplug_recive_authority.h"

using namespace std;

MYSQL * m_sql = 0;
string gresXML;
string username = "";
string superuser = "";

int xx_rcv_gettype()
{
	printf("--xxplug_recive_authority--\n");
	connectMySQL(m_sql);

	string sql = "SELECT superuser FROM xxdb_info_base";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			superuser.append(row[0]);
		}
		mysql_free_result(result);
	}
	//printf("superuser = %s\n", superuser.c_str());

	return TYPE;
}

int CheckUsername(cxx_xml xx_xml, char ** outxmldata)
{
	int checking = 0;
	string str = xx_xml.xpath_getprop("/userdata/head/user", "value");

	if(strcmp(str.c_str(), superuser.c_str()) == 0)
	{
		username = str;
		checking = 1;
	}
	else
	{
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
	}

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

	printf("InXML: %s\n", inxmldata);

	if(str.compare("0001") == 0)
		return InsertData(xx_xml, outxmldata);

	if(str.compare("0002") == 0)
		return DeleteData(xx_xml, outxmldata);

	if(str.compare("0003") == 0)
		return RelevanceData(xx_xml, outxmldata);

	if(str.compare("0004") == 0)
		return GetAllData(xx_xml, outxmldata);

	if(str.compare("0005") == 0)
		return GetPartData(xx_xml, outxmldata);

	if(str.compare("0006") == 0)
		return GetModuleData(xx_xml, outxmldata);

	return 0;
}
