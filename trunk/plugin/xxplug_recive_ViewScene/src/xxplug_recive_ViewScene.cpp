/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */
 #include "xxplug_recive_ViewScene.h"
#include <dbmepolice.h>
#include <xxxml.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cxx_xml.h>
#include <time.h>
#include "chinese_code.h"
#include "xxlibpushclient.h"

using namespace std;

string gresXML = "";
MYSQL * m_sql = 0;
string username = "";
string picpath = "";
void * SendMsg = NULL;
string mqttIP = "";
string mqttPort = "";

void mOn_Message(char * topic, int topic_len, char * message, int msg_len, void * obj)
{}

int xx_rcv_gettype() {

	printf("--xxplug_recive_ViewScene--\n");
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

/*
	string gbk = "";
	string utf = "";

	sql = "select `function` from xxdb_info_func where id = 91";
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		//正常
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			utf = row[0];
		}
		mysql_free_result(result);
	}

	CChineseCode::UTF_8ToGB2312(gbk, (char *)utf.c_str(), utf.size());

	printf("function name = %s\n", gbk.c_str());
*/

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

	SendMsg = Init((char *)"AccidentMsg", (char *)mqttIP.c_str(), atoi(mqttPort.c_str()), mOn_Message);
	subscribe(SendMsg, (char *)"a/AccidentMsg");

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

	printf("%s *** %s.%s *** IP: %s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	printf("INXML : %s\n", inxmldata);

	if (str.compare("0005") == 0)
	{
		string sql = "SELECT sole FROM xxdb_data_accident ";
		sql += " where 1=1 ";
		sql += "AND sole = '";
		sql += xx_xml.xpath_getprop("/userdata/data/picsole", "value");
		sql += "'";

		int iresut = xx_sql_query(m_sql,sql.c_str());
		if(iresut==0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			string tmpprop="";

			if((row = mysql_fetch_row(result)))
			{
				//printf("This Accident is exist! sole = %s\n", row[0]);
				str = "0006";
			}
			//else
				//printf("This Accident is not exist!\n");

			mysql_free_result(result);
		}
	}

	if (str.compare("0001") == 0) {
		return insertWork(xx_xml, outxmldata);
	}

	if (str.compare("0002") == 0) {
		return deleteWork(xx_xml, outxmldata);
	}

	if (str.compare("0003") == 0) {
		return updateWork(xx_xml, outxmldata);
	}

	if (str.compare("0004") == 0) {
		return selectWork(xx_xml, outxmldata);
	}

	if (str.compare("0017") == 0) {
		return selectWorkAll(xx_xml, outxmldata);
	}

	if (str.compare("0013") == 0) {
		return tidyWork(xx_xml, outxmldata);
	}

	if (str.compare("0005") == 0) {
		return insertAccident(xx_xml, outxmldata);
	}

	if (str.compare("0010") == 0) {
		return insertAccidentpic(xx_xml, outxmldata);
	}

	if (str.compare("0006") == 0) {
		return updateAccident(xx_xml, outxmldata);
	}

	if (str.compare("0007") == 0) {
		return deleteAccident(xx_xml, outxmldata);
	}

	if (str.compare("0008") == 0) {
		return selectAccident(xx_xml, outxmldata);
	}

	if (str.compare("0018") == 0) {
		return selectAccidentAll(xx_xml, outxmldata);
	}

	if (str.compare("0009") == 0) {
		return selectAccidentProgress(xx_xml, outxmldata);
	}

	if (str.compare("0014") == 0) {
		return tidyAccident(xx_xml, outxmldata);
	}

	if (str.compare("0012") == 0) {
		return insertAccident_process(xx_xml, outxmldata);
	}

	if (str.compare("0011") == 0) {
		return selectAccidentpic(xx_xml, outxmldata);
	}

	if (str.compare("0015") == 0) {
		return getWorktypeCount(xx_xml, outxmldata);
	}

	if (str.compare("0016") == 0) {
		return getManworkCount(xx_xml, outxmldata);
	}

	return 0;
}
