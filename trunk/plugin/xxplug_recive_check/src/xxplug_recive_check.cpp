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
#include <pthread.h>
#include <xxxml.h>
#include <winsock2.h>
#include "xxplug_recive_check.h"

using namespace std;

MYSQL * m_sql = 0;
string gresXML;

#define bzero(x,y) memset(x, 0, y)

int xx_rcv_gettype()
{
	printf("--xxplug_recive_check--\n");
	connectMySQL(m_sql);
	return TYPE;
}
int SelectSQLCheckDate(cxx_xml xx_xml, char ** outxmldata)
{
	printf("SelectSQLCheckDate\n");
	string strxml = "";
	string stritem = "";
	string sql = "";
	string str = "";
	int ret = 0;
	int count = 0;

	sql = "SELECT b.number,b.license,b.name,a.platetype,a.platenum,a.datetime,a.roadname,"
			"a.viotype,b.money,a.deviceuser,b.datetime,a.picurl0,a.picurl1,a.picurl2 "
			"FROM xxdb_data_viodata a LEFT JOIN xxdb_data_cfjds b ON a.vioid = b.vioid WHERE 1=1";
	//获取违法起始时间
	str = xx_xml.xpath_getprop("/userdata/data/wfsj1", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.datetime >= '";
		sql += str;
		sql += "'";
	}
    //获取违法截止时间
	str = xx_xml.xpath_getprop("/userdata/data/wfsj2", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.datetime <= '";
		sql += str;
		sql += "'";
	}
    //获取查询执勤民警号
	str = xx_xml.xpath_getprop("/userdata/data/zqmj", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.deviceuser = '";
		sql += str;
		sql += "'";
	}
    //查询数据库
	printf("sql == %s\r\n",sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());
	strxml = "<resultdata>";
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while((row = mysql_fetch_row(result)))
		{
		/*
		"SELECT b.number,b.license,b.name,a.platetype,a.platenum,a.datetime,a.roadname,"
		"a.viotype,b.money,a.deviceuser,b.datetime,a.picurl0,a.picurl1,a.picurl2 "
		"FROM xxdb_data_viodata a LEFT JOIN xxdb_data_cfjds b ON a.vioid = b.vioid WHERE 1=1";
		 */
			count++;
			stritem = new_strprop("jdslb", "value", "");    //< jdslb value=””>决定书类别
			stritem += new_strprop("jdsbh", "value", row[0]);//< jdsbh value=””>决定书编号
			stritem += new_strprop("jszh", "value",  row[1]); //< jszh value=””>驾驶证号
			stritem += new_strprop("zjcx", "value", ""); //< zjcx value=””>准驾车型
			stritem += new_strprop("dsr", "value",   row[2]);   //< dsr value=””>当事人
			stritem += new_strprop("hpzl", "value",  row[3]);//< hpzl value=””>号牌种类
			stritem += new_strprop("hphm", "value", row[4]); //< hphm value=””>号牌号码
			stritem += new_strprop("wfsj", "value", row[5]); //< wfsj value=””>违法时间
			stritem += new_strprop("wfdz", "value", row[6]); //< wfdz value=””>违法地址
			stritem += new_strprop("wfxw", "value", row[7]); //< wfxw value=””>违法行为
			stritem += new_strprop("wfjfs", "value", "");//< wfjfs value=””>违法记分数
			stritem += new_strprop("fkje", "value", row[8]); //< fkje value=””>罚款金额
			stritem += new_strprop("zqmj", "value", row[9]); //< zqmj value=””>执勤民警警号
			stritem += new_strprop("fxjg", "value", ""); //< fxjg value=””>发现机构
			stritem += new_strprop("fxjgmc", "value", "");//< fxjgmc value=””>发现机构名称
			stritem += new_strprop("cljgmc", "value", "");//< cljgmc value=””>处理机构名称
			stritem += new_strprop("clsj", "value", row[10]);  //< clsj value=””>处理时间
			stritem += new_strprop("url1value", "value", row[11]);//照片url
			stritem += new_strprop("url2value", "value", row[12]);//照片url
			stritem += new_strprop("url3value", "value", row[13]);//照片url
			strxml += setResItem(count, stritem);
		}
		mysql_free_result(result);
	}
	strxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, strxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
    printf("\"xxplug_recive_check\" send xml is:\r\n");
    printf("%s\r\n",*outxmldata);
	return ret;
}

int xx_rcv_procinfo(char * inxmldata, char ** outxmldata)
{
	printf("xx_rcv_procinfo\n");
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);

	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	printf("%s *** %s.%s *** IP: %s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	if (str.compare("0001") == 0)
	{
		return SelectSQLCheckDate(xx_xml, outxmldata);
	}
	return 0;
}
