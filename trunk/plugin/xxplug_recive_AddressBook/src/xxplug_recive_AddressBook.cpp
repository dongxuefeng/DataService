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
#include "xxplug_recive_AddressBook.h"

using namespace std;

#define bzero(x,y) memset(x, 0, y)

MYSQL * m_sql = 0;
string gresXML;
string username = "";
string fileDir = "";

int xx_rcv_gettype()
{
	printf("--xxplug_recive_Law--\n");
	
	connectMySQL(m_sql);

	string sql = "SELECT picdirectory FROM xxdb_info_base";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result))) fileDir = row[0];

		mysql_free_result(result);
	}

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

	if((str.compare("0001") == 0)||(str.compare("0002") == 0)||(str.compare("0003") == 0))
	{
		int value = 0;
		string sql = "SELECT `value` FROM xxdb_info_versions WHERE `name` = 'addressBook'";

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result))) value = atoi(row[0]);

			mysql_free_result(result);
		}

		value++;
		sql = "UPDATE xxdb_info_versions SET `value` = ";
		sql += itoaa(value);
		sql += " WHERE `name` = 'addressBook'";

		printf("SQL: %s\n", sql.c_str());
		xx_sql_query(m_sql, sql.c_str());
	}

	//web
	if(str.compare("0001") == 0)
		return InsertLinkman(xx_xml, outxmldata);
	if(str.compare("0002") == 0)
		return DeleteLinkman(xx_xml, outxmldata);
	if(str.compare("0003") == 0)
		return UpdateLinkman(xx_xml, outxmldata);
	if(str.compare("0004") == 0)
		return SelectLinkman(xx_xml, outxmldata);
	if(str.compare("0005") == 0)
		return SelectGroup(xx_xml, outxmldata);
	//phone
	if(str.compare("0006") == 0)
		return GetAddressBookFileURL(xx_xml, outxmldata);

	return 0;
}
