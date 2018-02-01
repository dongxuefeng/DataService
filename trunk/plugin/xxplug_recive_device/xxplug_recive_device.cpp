/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */
#include <vector>
#include "xxplug_define.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xxxml.h>
#include <dbmepolice.h>
#include <mysql.h>
#include <time.h>
using namespace std;

#define TYPE 0005
#define strTYPE "0005"

string gResLoginXML = "";
MYSQL * m_sql = 0;
string username = "";

int xx_rcv_gettype()
{
	printf("--xxplug_recive_device----%s--\n",strTYPE);
	connectMySQL(m_sql);
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
		string res;
		setResXML(strTYPE, 0, 0, 0, tmp1, res);
		*outxmldata = (char *) res.c_str();
		printf("%s\n", res.c_str());

		return -1;
	}

	return 0;
}

/*
 REQUEST
 <type value=““/>		业务类型
 <deviceid value=““/>
 <regcode value=““/>
 <authcode value=““/>
 <regdate value=““/>
 RETURN
 <res value=““/>		执行结果

 */

/*
 `id` int(11) NOT NULL auto_increment,
 `deviceid` char(32) default NULL,
 `regcode` char(32) default NULL,
 `authcode` char(32) default NULL,
 `regdate` datetime default NULL,
 ) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='基础表：设备信息表'


 */
int xx_rcv_procinfo_add(char * inxmldata, char ** outxmldata) {
	int ret = 0;

	string strpath[] = { "/userdata/data/deviceid", "/userdata/data/regcode",
			"/userdata/data/authcode", "/userdata/data/regdate", "/userdata/data/phone"};
	string strvalue[6];
	char sqlstr[256];
	string respoxy;

	_set_data(inxmldata, 5, strpath, strvalue);

	respoxy = "<resultdata><item1>";
	string rusername = "";
	time_t t;
	struct tm *p;
	time(&t);
	p = localtime(&t);

	sprintf(sqlstr,"select deviceid from xxdb_info_device");
	if(xx_sql_query(m_sql,sqlstr) == 0)
	{
		ret = 0;
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			if(strcmp(row[0], strvalue[0].c_str()) == 0) ret = 3;
		}
	}
	else ret = 3;

	if(ret == 0)
	{
		sprintf(sqlstr,
				"insert into xxdb_info_device (deviceid,regcode,authcode,regdate,bind,phone) \
							values ('%s','%s','%s','%d-%d-%d %d:%d:%d',0,'%s')",
				strvalue[0].c_str(), strvalue[1].c_str(), strvalue[2].c_str(),
				1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, strvalue[4].c_str());

		if (xx_sql_query(m_sql, sqlstr) == 0)
		{
			ret = 1;
		}
		else
		{
			ret = 2;
		}
	}
	respoxy += " <res value=\"";
	respoxy += itoaa(ret);
	respoxy += "\"/>";
	respoxy += "</item1></resultdata>";

	//void setResXML(string type,int count,int sumcount,int FirstIndex,string &resXML)
	setResXML(strTYPE, 1, 1, 0, respoxy, gResLoginXML);

	//cout << "strXML: " << gResLoginXML.c_str() << endl;
	*outxmldata = (char *) gResLoginXML.c_str();

	return ret;
}

int xx_rcv_procinfo_del(char * inxmldata, char ** outxmldata) {
	int ret = 0;

	string strpath[] = { "/userdata/data/deviceid" };
	string strvalue[1];
	char sqlstr[256];
	string respoxy;
	_set_data(inxmldata, 1, strpath, strvalue);

	respoxy = "<resultdata><item1>";
	string rusername = "";
	string rissuccess = "1";

	sprintf(sqlstr, "delete from xxdb_info_device where deviceid = '%s'",
			strvalue[0].c_str());

	if (xx_sql_query(m_sql, sqlstr) == 0)
	{
		rissuccess = "1";
		ret = 1;
	}
	else
	{
		rissuccess = "2";
		ret = 2;
	}
	respoxy += " <res value=\"" + rissuccess + "\"/>";
	respoxy += "</item1></resultdata>";

	//void setResXML(string type,int count,int sumcount,int FirstIndex,string &resXML)
	setResXML(strTYPE, 1, 1, 0, respoxy, gResLoginXML);
	*outxmldata = (char *) gResLoginXML.c_str();
	return ret;
}
int xx_rcv_procinfo_update(char * inxmldata, char ** outxmldata) {
	int ret = 0;

	string strpath[] = { "/userdata/data/deviceid", "/userdata/data/regcode",
				"/userdata/data/authcode", "/userdata/data/regdate", "/userdata/data/phone"};

	string strvalue[6];
	char sqlstr[256];
	string respoxy;

	_set_data(inxmldata, 5, strpath, strvalue);

	respoxy = "<resultdata><item1>";
	string rusername = "";
	string rissuccess = "1";

	time_t t;
	struct tm *p;
	time(&t);
	p = localtime(&t);

	sprintf(sqlstr,
			"update xxdb_info_device set regcode = '%s',authcode = '%s' ,regdate = '%d-%d-%d %d:%d:%d', phone = '%s'\
				  where deviceid ='%s'",
			strvalue[1].c_str(), strvalue[2].c_str(), 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,
			strvalue[4].c_str() ,strvalue[0].c_str());

	//cout << "SQL :" << endl << sqlstr << endl;
	if (xx_sql_query(m_sql, sqlstr) == 0)
	{
		rissuccess = "1";
		ret = 1;
	}
	else
	{
		rissuccess = "2";
		ret = 2;
	}
	respoxy += " <res value=\"" + rissuccess + "\"/>";
	respoxy += "</item1></resultdata>";

	//void setResXML(string type,int count,int sumcount,int FirstIndex,string &resXML)
	setResXML(strTYPE, 1, 1, 1, respoxy, gResLoginXML);
	*outxmldata = (char *) gResLoginXML.c_str();

	return ret;
}

int xx_rcv_procinfo_select(char * inxmldata, char ** outxmldata) {
	int ret = 0;

	string strpath[] = { "/userdata/data/deviceid", "/userdata/head/Request_Index", "/userdata/data/bind"};
	string strvalue[3];
	string sqlstr = "";
	string respoxy;

	_set_data(inxmldata, 3, strpath, strvalue);

	int isumcount = 0;
	int sendcount = 0;
	int startIndex  = atoi(strvalue[1].c_str());

	//cout << "XML : " << endl << inxmldata << endl;;

	//cout << "bind = " << strvalue[2].c_str() << endl;
	respoxy = "<resultdata>";
	string rusername = "";
	string rissuccess = "1";
	sqlstr
			+= "select id,deviceid,regcode,authcode,regdate,phone from xxdb_info_device where 1=1";
	if (strvalue[0] != "")
		sqlstr += " and deviceid = '" + strvalue[0] + "'";
	if (strvalue[2] != "")
		sqlstr += " and bind = '" + strvalue[2] + "'";

	//cout << "SQL :" << endl << sqlstr << endl;

	if (xx_sql_query(m_sql, sqlstr.c_str()) == 0) {
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;


		while ((row = mysql_fetch_row(result)))
		{
			//cout << "mysql_fetch_row OK!" << endl;
			/*
			 <deviceid value=““/>
			 <regcode value=““/>
			 <authcode value=““/>
			 <regdate value=““/>
			 */
			isumcount++;
			if((startIndex < isumcount)&&(sendcount < 10))
			{
				sendcount++;
				respoxy += "<item";
				respoxy += itoaa(sendcount);
				respoxy += ">";
				respoxy += new_strprop("deviceid", "value", row[1]);
				respoxy += new_strprop("regcode", "value", row[2]);
				respoxy += new_strprop("authcode", "value", row[3]);
				respoxy += new_strprop("regdate", "value", row[4]);
				respoxy += new_strprop("phone", "value", row[5]);
				respoxy += "</item";
				respoxy += itoaa(sendcount);
				respoxy += ">";
			}
		}
		ret = 0;
	}
	else
	{
		//cout << "ERROR : " << endl << mysql_error(m_sql) << endl;
		ret = 1;
	}
	respoxy += "</resultdata>";

	setResXML(strTYPE, sendcount, isumcount, startIndex, respoxy, gResLoginXML);

	*outxmldata = (char *) gResLoginXML.c_str();
	//cout << "XML:" << endl << *outxmldata << endl;

	return ret;
}

int xx_rcv_procinfo(char * inxmldata, char ** outxmldata) {
	string strpath[] = { "/userdata/data/type" };
	string strvalue[1];
	_set_data(inxmldata, 1, strpath, strvalue);

	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);

	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf("Checking Username error!\n");
		return -1;
	}

	int iresult = 0;
	if (!strvalue[0].compare("0001"))
		iresult = xx_rcv_procinfo_add(inxmldata, outxmldata);
	if (!strvalue[0].compare("0002"))
		iresult = xx_rcv_procinfo_del(inxmldata, outxmldata);
	if (!strvalue[0].compare("0003"))
		iresult = xx_rcv_procinfo_update(inxmldata, outxmldata);
	if (!strvalue[0].compare("0004"))
		iresult = xx_rcv_procinfo_select(inxmldata, outxmldata);

	return iresult;
}
