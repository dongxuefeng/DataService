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
#include <xxxml.h>
#include <pthread.h>
#include "inifile.h"
#include "chinese_code.h"
#include "xxplug_recive_FProcess.h"

using namespace std;


MYSQL * m_sql = 0;
string gresXML;
string username = "";
string addr = "";
string phone = "";

#define bzero(x,y) memset(x, 0, y)

int xx_rcv_gettype()
{
	printf("--xxplug_recive_FProcess--\n");
	connectMySQL(m_sql);

	char c_addr[1024] = {0};
	char c_phone[13] = {0};
    //读取addr.ini文件中定义的地址和电话信息
	if(!read_profile_string("police", "add", c_addr, 1024, "", "addr.ini"))
	{
		printf("read ini add error!\n");
		sprintf(c_addr, "addr123456789");
	}

	if(!read_profile_string("police", "phone", c_phone, 13,"","addr.ini"))
	{
		printf("read ini phone error!\n");
		sprintf(c_phone, "12345678901");
	}

	printf("addr = %s\n", c_addr);
	printf("phone = %s\n", c_phone);

	CChineseCode::GB2312ToUTF_8(addr, c_addr, strlen(c_addr));
	CChineseCode::GB2312ToUTF_8(phone, c_phone, strlen(c_phone));

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

	printf("INXML : %s\n", inxmldata);

	if(str.compare("0001") == 0)
	{
		return getticketid(xx_xml, outxmldata);
	}

	if(str.compare("0010") == 0)
	{
		return cancleticketid(xx_xml, outxmldata);
	}

	if(str.compare("0002") == 0)
	{
		return putResult(xx_xml, outxmldata);
	}

	if(str.compare("0003") == 0)
	{
		return SearchVio(xx_xml, outxmldata);
	}

	if(str.compare("0012") == 0)
	{
		return WorkSearchVio(xx_xml, outxmldata);
	}

	if(str.compare("0011") == 0)
	{
		return DetailVio(xx_xml, outxmldata);
	}

	if(str.compare("0004") == 0)
	{
		return DetailVioDispose(xx_xml, outxmldata);
	}

	if(str.compare("0005") == 0)
	{
		return SearchQZCS(xx_xml, outxmldata);
	}

	if(str.compare("0006") == 0)
	{
		return SearchAccidentDuty(xx_xml, outxmldata);
	}

	if(str.compare("0007") == 0)
	{
		return DetailAccidentDuty(xx_xml, outxmldata);
	}

	if(str.compare("0008") == 0)
	{
		return AddNumber(xx_xml, outxmldata);
	}

	if(str.compare("0009") == 0)
	{
		return SearchNumber(xx_xml, outxmldata);
	}

	if(str.compare("0013") == 0)
	{
		return getIDLeaveLine(xx_xml, outxmldata);
	}

	return 0;
}
