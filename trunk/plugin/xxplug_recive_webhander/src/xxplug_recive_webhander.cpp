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
#include <xxxml.h>
#include "mysql.h"
#include <dbmepolice.h>
#include <xxbase64.h>
#include "chinese_code.h"
#include "xxplug_recive_webhander.h"
#include <map>

using namespace std;

MYSQL * m_sql = 0;

string username = "";
string realname = "";
string datafilter = "";
string picpath = "";
string downloadPath ="";
char jpegpic[1024*1024*5] = {0};//图片使用缓存

#define bzero(x,y) memset(x, 0, y)

int xx_rcv_gettype()
{
	printf("--xxplug_recive_webhander--\n");
	connectMySQL(m_sql);
	//Getsystemstorepath();
	return TYPE;
}
//使用vioid查询 上传者用户名
string FindUpNameWithVIOID(string vioid)
{
	string sql,res;
	res = "";
	//具体查找结果
	sql = "SELECT `deviceuser` FROM xxdb_data_viodata  where vioid ='";
	sql += vioid;
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{

			res = row[0];
		}
		mysql_free_result(result);
	}
	return res ;
}
//插入web处理结果到数据库（处罚决定书）
int InsertResultDataIntoSQL(cxx_xml xx_xml, char ** outxmldata)
{
	int res = 0;
	string strxml = "<resultdata><item1>";
	string gresXML;
	string sql;
	sql = "INSERT xxdb_data_cfjds (number, name, phone, license, record, office1, zjlx, platenum, platetype, office2, message, datetime, vioid, money, user,handusername) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/id", "value");//处罚决定书编号
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/name", "value");//被处罚人名字
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/phone", "value");//被处罚人电话
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/license", "value");//被处驾照
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/record", "value");//档案
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/office1", "value");//发证机关
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/zjlx", "value");//准驾类型
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");//车牌号
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");//车辆类型
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/office2", "value");//发证机关
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/message", "value");//违法内容
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");//违法时间
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");//违违法唯一识别id
	sql += "',";
	sql += xx_xml.xpath_getprop("/userdata/data/money", "value");//罚款金额
	sql += ",'";
	sql += FindUpNameWithVIOID(xx_xml.xpath_getprop("/userdata/data/vioid", "value"));//上传者用户名
	sql += "','";
	sql += username;//手机上传违法处理结果，xxdb_data_cfjds处罚决定书表的上传人和处理人相同
	sql += "')";
	printf("sql: %s\n", sql.c_str());
	int ret = 1;
	ret = xx_sql_query(m_sql, sql.c_str());

	//更新违法数据库disposetype=2 web已处理
	sql = "UPDATE xxdb_data_viodata SET disposetype = 2";
	sql += " WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	printf("SQL: %s\n", sql.c_str());
	xx_sql_query(m_sql, sql.c_str());
    //查询用户黑名单最大的TIMESTAMP
	int timestamp = 0;
	sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			timestamp = atoi(row[0]);
		}
		mysql_free_result(result);
	}
	//更新黑名单，标记为已删除，手机更新时，自己删除
	sql = "UPDATE xxdb_data_blacklist SET altertype = 2, TIMESTAMP = ";
	sql += itoaa(++timestamp);
	sql += " WHERE platenum = '";
	sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	sql += "' AND platetype = '";
	sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	sql += "'";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	printf("SQL: %s\n", sql.c_str());
	xx_sql_query(m_sql, sql.c_str());
    //设置回复字符串
	strxml += new_strprop("res", "value", itoaa(ret));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);
	return 0;
}
//web查询WEB处理罚款数据结果,每次查询10数据
int WebCheckWebHanderResultWithPageSQL(cxx_xml xx_xml, char ** outxmldata)
{

	string resxml = "<resultdata>";
	string gresXML;
	string sql;
	//获取当前用户起始时间和结束时间
	string stime  = xx_xml.xpath_getprop("/userdata/data/Stime", "value");
	string eimte  = xx_xml.xpath_getprop("/userdata/data/Etime", "value");
	string hander = xx_xml.xpath_getprop("/userdata/data/disposeuser", "value");//处理者用户名

	int count = 0, sumcount = 0;
    //获取开始索引编号
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
    //查询web处理的数据,xxdb_data_viodata.disposetype='2'
	sql = "SELECT COUNT(0) FROM xxdb_data_viodata WHERE disposetype='2'";

	if(!stime.empty())
	{
		sql += " AND datetime >= '";
		sql += stime;
		sql += "'";
	}
	if(!eimte.empty())
	{
		sql += " AND datetime <= '";
		sql += eimte;
		sql += "'";
	}
	if(!hander.empty())//处理人真实姓名
	{
		sql += " AND handusername = '";
		sql += hander;
		sql += "'";
	}

	printf("SQL: %s\n", sql.c_str());
	//执行查询总的数据条数
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			sumcount = atoi(row[0]);
		}
		mysql_free_result(result);
	}
	//具体查找结果
	sql = "SELECT a.vioid, b.datetime, b.handusername, a.viotype, a.roadname, a.platenum, a.platetype FROM "
			"xxdb_data_viodata a left join xxdb_data_cfjds b on a.vioid = b.vioid where 1=1";

	if(!stime.empty())
	{
		sql += " AND b.datetime >= '";
		sql += stime;
		sql += "'";
	}
	if(!eimte.empty())
	{
		sql += " AND b.datetime <= '";
		sql += eimte;
		sql += "'";
	}
	if(!hander.empty())//处理人真实姓名
	{
		sql += " AND b.handusername = '";
		sql += hander;
		sql += "'";
	}
	sql += " ORDER BY  b.datetime DESC LIMIT ";
	sql += itoaa(firstIndex);
	sql += ",10";
	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		count = 0;
		while((row = mysql_fetch_row(result)))
		{
			resxml += "<item";
			resxml += itoaa(++count);
			resxml += ">";
			resxml += new_strprop("vioid", "value", row[0]);
			resxml += new_strprop("disposetime", "value", row[1]);
			resxml += new_strprop("disposeuser", "value", row[2]);
			resxml += new_strprop("viotype", "value", row[3]);
			resxml += new_strprop("vioplace", "value", row[4]);
			resxml += new_strprop("platenum", "value", row[5]);
			resxml += new_strprop("platetype", "value", row[6]);
			resxml += "</item";
			resxml += itoaa(count);
			resxml += ">";
		}
		mysql_free_result(result);
	}
	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
//    string tsrout;
//	CChineseCode::UTF_8ToGB2312(tsrout, (char *)gresXML.c_str(), gresXML.length());
	printf("OUTXML : %s\n", *outxmldata);
	return 0;
}
//web查询所有的数据
int WebCheckScreenCounter(cxx_xml xx_xml, char ** outxmldata)
{
	string gresXML;
	string resxml = "<resultdata>";
	std::map<string ,string> m_group_list;
	string sql;
	string m_grp="安康服务区";
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
    //获取所有包含关键字“安康服务区”的用户组

	m_group_list.clear();

	sql = "SELECT `group`,`datafilter` FROM xxdb_info_usergroup WHERE `group` LIKE '";
	sql += m_grp;
	sql += "%' OR `group` LIKE '%";
	sql += m_grp;
	sql += "%' OR `group` LIKE '%";
	sql += m_grp;
	sql += "'";
	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			m_group_list.insert(std::pair<string,string>(row[0], row[1]));
		}
		mysql_free_result(result);
	}
	int m_count=0;
	int count=0;
	int sumcount=0;
    //查询每个组下各自己使用计数总和
	for (std::map<string ,string>::iterator iter = m_group_list.begin(); iter
				!= m_group_list.end(); iter++,sumcount++)
	{

		sql = "SELECT chkcounter FROM xxdb_info_deviceuser where 1=1";
		if((string)iter->second != "")
		{
			sql += " AND datafilter = '";
			sql += iter->second;
			sql += "'";
		}
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				m_count += atoi(row[0]);//叠加所有用户数据的chkcounter
			}
			mysql_free_result(result);
		}
        string m_x=(string)(iter->first);
		resxml += "<item";
		resxml += itoaa(++count);
		resxml += ">";
		resxml += new_strprop("X", "value", m_x.c_str());
		resxml += new_strprop("Y", "value", itoaa(m_count));
		resxml += "</item";
		resxml += itoaa(count);
		resxml += ">";

	}
	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);
	return 0;
}


//校验用户是否登录，并返回用户的用户名username
int CheckUsername(cxx_xml xx_xml, char ** outxmldata)
{
	int checking = 0;
	string str = xx_xml.xpath_getprop("/userdata/head/user", "value");
	//使用用户随机码查询用户名名username
	string sql = "SELECT username, `name` ,datafilter from xxdb_info_deviceuser WHERE userlogintag = '";
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
			realname = row[1];
		    datafilter =row[2];
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
	{
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
	printf("xxplug_recive_ledger xx_rcv_procinfo\n");
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);
	//检查用户登录的随机码和数据库中存储的是否相同，并提取用户名，真实名称，以及数据过滤标识
	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf("xxplug_recive_ledger Checking Username error!\n");
		return -1;
	}
	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");
	printf("%s *** %s.%s *** IP: %s\n", username.c_str(), TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));
    //web提交处理违法结果至处罚决定书数据库（0021 0001）
	if (str.compare("0001") == 0)
	{
		return InsertResultDataIntoSQL(xx_xml, outxmldata);
	}
	//web查询数据，采用分页xml查询方式
	else if (str.compare("0002") == 0)
	{
		return WebCheckWebHanderResultWithPageSQL(xx_xml, outxmldata);
	}
	//web查询数据，整体数据的下载
	else if (str.compare("0003") == 0)
	{
		return WebCheckScreenCounter(xx_xml, outxmldata);
	}
	return 0;
}
