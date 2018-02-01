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
#include "xxplug_recive_Law.h"

using namespace std;

#define bzero(x,y) memset(x, 0, y)

MYSQL * m_sql = 0;
string gresXML;
string username = "";

int xx_rcv_gettype()
{
	printf("--xxplug_recive_Law--\n");
	
	connectMySQL(m_sql);

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
		string sql = "SELECT `value` FROM xxdb_info_versions WHERE `name` = 'vioType'";

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
		sql += " WHERE `name` = 'vioType'";

		printf("SQL: %s\n", sql.c_str());
		xx_sql_query(m_sql, sql.c_str());
	}

//law
	//web
	if (str.compare("0001") == 0)
		return insertLaw(xx_xml, outxmldata);

	if (str.compare("0002") == 0)
		return deleteLaw(xx_xml, outxmldata);

	if (str.compare("0003") == 0)
		return updateLaw(xx_xml, outxmldata);

	if (str.compare("0004") == 0)
		return searchLaw(xx_xml, outxmldata);
	//phone
	if (str.compare("0005") == 0)
		return getLaw(xx_xml, outxmldata);

	if (str.compare("0006") == 0)
		return getKeyList(xx_xml, outxmldata);

	if (str.compare("0030") == 0)
		return getVioTypeList(xx_xml, outxmldata);

//plan
	//web
	//plan
	if (str.compare("0009") == 0)
		return insertPlan(xx_xml, outxmldata);

	if (str.compare("0010") == 0)
		return deletePlan(xx_xml, outxmldata);

	if (str.compare("0028") == 0)
		return searchPlan(xx_xml, outxmldata);

	if (str.compare("0012") == 0)
		return searchPlanDetail(xx_xml, outxmldata);

	if (str.compare("0029") == 0)
		return IsUsePlanName(xx_xml, outxmldata);

	//addr
	if (str.compare("0019") == 0)
		return insertAddr(xx_xml, outxmldata);

	if (str.compare("0020") == 0)
		return deleteAddr(xx_xml, outxmldata);

	if (str.compare("0021") == 0)
		return updateAddr(xx_xml, outxmldata);

	if (str.compare("0022") == 0)
		return searchAddr(xx_xml, outxmldata);
	//org
	if (str.compare("0023") == 0)
		return insertOrg(xx_xml, outxmldata);

	if (str.compare("0024") == 0)
		return deleteOrg(xx_xml, outxmldata);

	if (str.compare("0025") == 0)
		return updateOrg(xx_xml, outxmldata);

	if (str.compare("0026") == 0)
		return searchOrg(xx_xml, outxmldata);
	//phone
	if (str.compare("0007") == 0)
		return getAddPlan(xx_xml, outxmldata);

	if (str.compare("0008") == 0)
		return getAll(xx_xml, outxmldata);

	if (str.compare("0027") == 0)
		return getAllOrg(xx_xml, outxmldata);

//vioArticle
	//web
	if (str.compare("0013") == 0)
		return insertVioArticle(xx_xml, outxmldata);

	if (str.compare("0014") == 0)
		return deleteVioArticle(xx_xml, outxmldata);

	if (str.compare("0015") == 0)
		return updateVioArticle(xx_xml, outxmldata);

	if (str.compare("0016") == 0)
		return searchVioArticle(xx_xml, outxmldata);
	//phone
	if (str.compare("0017") == 0)
		return getVio(xx_xml, outxmldata);

	if (str.compare("0018") == 0)
		return getVioKeyList(xx_xml, outxmldata);

	return 0;
}
