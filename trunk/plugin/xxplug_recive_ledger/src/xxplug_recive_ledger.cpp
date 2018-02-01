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
#include "xxplug_recive_ledger.h"

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
	printf("--xxplug_recive_ledger--\n");
	connectMySQL(m_sql);
	Getsystemstorepath();
	return TYPE;
}

void Getsystemstorepath(void)
{
	string sql = "SELECT picdirectory,downloadPath FROM xxdb_info_base WHERE id = 1";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			picpath = "";
			downloadPath = "";
			picpath.append(row[0]);
			downloadPath.append(row[1]);
		}
		mysql_free_result(result);
	}
}
//使用警号，查询用户真实姓名，及datafitler(手机上传)
int InsertLedgerDataIntoSQL(cxx_xml xx_xml, char ** outxmldata)
{
	int res = 0;
	string strxml = "<resultdata><item1>";
	string gresXML;

	//判断超找的用户真实名称和对应的数据过滤
	if( realname.empty() || datafilter.empty() )
	{
		printf("realname or datafilter == NULL \r\n");
		res = 1;
		strxml += new_strprop("res", "value", itoaa(res));
		strxml += "</item1></resultdata>";
		setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
		*outxmldata = (char *) gresXML.c_str();
		return -1;
	}

    string sql;
    sql = "INSERT INTO xxdatabase.xxdb_data_ledger  (`platenum`,`cartype`,`owner`,`must`,`actual`,`drivername`,`phone`,`drivernum`,`viotype`,"
    		"`handleway`,`handler`,`uptime`,`memo`,`handlernum`,`userfilter`,`picurl`) values ('" ;

    sql += 	xx_xml.xpath_getprop("/userdata/data/platenum", "value");
    sql += 	"','";
    sql += 	xx_xml.xpath_getprop("/userdata/data/cartype", "value");
    sql += 	"','";
    sql += 	xx_xml.xpath_getprop("/userdata/data/owner", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/must", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/actual", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/drivername", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/phone", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/drivernum", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/viotype", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/handleway", "value");
	sql += 	"','";
	sql += realname;//处理人真实名称
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/time", "value");
	sql += 	"','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/memo", "value");
	sql += 	"','";
	sql += username;//用户名
	sql += 	"','";
	sql += 	datafilter;//数据过滤字段
	sql += 	"','";

	string filepath;
	//获取用户图片的储存路径
	getLedgerfilename(xx_xml.xpath_getprop("/userdata/head/dev","value"), xx_xml.xpath_getprop("/userdata/data/time", "value"), 0, filepath);
	sql += filepath;
	sql += 	"');";
    printf("insertledgerdata sql cmd :%s\r\n",sql.c_str());
    string picdata;
	xx_xml.xpath_gettext("/userdata/data/picdata",picdata);
	printf("insertledgerdata filename = %s\r\n", filepath.c_str());
	if(savepicfile(filepath.c_str(),picdata.c_str()))
	{
		printf("save  picture failed \r\n");
	}
    res = xx_sql_query(m_sql, sql.c_str());
    if(res)
    {   //返回失败
    	res=1;
    }
    strxml += new_strprop("res", "value", itoaa(res));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	return 0;
}
//web查询台账数据,每次查询10数据
int WebCheckLedgerWithPageSQL(cxx_xml xx_xml, char ** outxmldata)
{

	string resxml = "<resultdata>";
	string gresXML;
	string sql;
	//获取当前用户起始时间和结束时间
	string stime  = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	string eimte  = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	string hander = xx_xml.xpath_getprop("/userdata/data/hander", "value");//处理人名称
	string handernum = xx_xml.xpath_getprop("/userdata/data/handernum", "value");//处理人名称
	int count = 0, sumcount = 0;
    //获取开始索引编号
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
    //获取符合条件总数
	sql = "SELECT COUNT(0) FROM xxdb_data_ledger WHERE userfilter LIKE '";
	sql += datafilter;
	sql += "%'";

	if(!stime.empty())
	{
		sql += " AND uptime >= '";
		sql += stime;
		sql += "'";
	}
	if(!eimte.empty())
	{
		sql += " AND uptime <= '";
		sql += eimte;
		sql += "'";
	}
	if(!hander.empty())//处理人真实姓名
	{
		sql += " AND handler = '";
		sql += hander;
		sql += "'";
	}
	if(!handernum.empty())//处理人用户名
	{
		sql += " AND handlernum = '";
		sql += handernum;
		sql += "'";
	}
	sql += ";";
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
	sql = "SELECT `platenum`,`cartype`,`owner`,`must`,`actual`,`drivername`,"
			"`phone`,`drivernum`,`viotype`,`handleway`,`handler`,`uptime`,`memo`,"
			"`picurl`,`handlernum` FROM xxdb_data_ledger  where userfilter LIKE '";
	sql += datafilter;
	sql += "%'";

	if(!stime.empty())
	{
		sql += " AND uptime >= '";
		sql += stime;
		sql += "'";
	}
	if(!eimte.empty())
	{
		sql += " AND uptime <= '";
		sql += eimte;
		sql += "'";
	}
	if(!hander.empty())//处理人真实姓名
	{
		sql += " AND handler = '";
		sql += hander;
		sql += "'";
	}
	if(!handernum.empty())//处理人用户名
	{
		sql += " AND handlernum = '";
		sql += handernum;
		sql += "'";
	}
	sql += " ORDER BY uptime DESC LIMIT ";
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
			resxml += new_strprop("platenum", "value", row[0]);
			resxml += new_strprop("cartype", "value", row[1]);
			resxml += new_strprop("owner", "value", row[2]);
			resxml += new_strprop("must", "value", row[3]);
			resxml += new_strprop("actual", "value", row[4]);
			resxml += new_strprop("drivername", "value", row[5]);
			resxml += new_strprop("phone", "value", row[6]);
			resxml += new_strprop("drivernum", "value", row[7]);
			resxml += new_strprop("viotype", "value", row[8]);
			resxml += new_strprop("handleway", "value", row[9]);
			resxml += new_strprop("hander", "value", row[10]);//处理人真实姓名
			resxml += new_strprop("time", "value", row[11]);
			resxml += new_strprop("memo", "value", row[12]);
			resxml += new_strprop("picurl", "value", row[13]);
			resxml += new_strprop("handernum", "value", row[14]);//处理人真实姓名
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
int WebCheckLedgerWithAllSQL(cxx_xml xx_xml, char ** outxmldata)
{

	string resxml = "<resultdata>";
	string gresXML;
	string sql;
	//获取当前用户起始时间和结束时间
	string stime = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	string eimte = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	string hander = xx_xml.xpath_getprop("/userdata/data/hander", "value");//处理人名称
	string handernum = xx_xml.xpath_getprop("/userdata/data/handernum", "value");//处理人名称
	int count = 0, sumcount = 0;
    //获取开始索引编号
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
    //获取符合条件总数
	sql = "SELECT COUNT(0) FROM xxdb_data_ledger WHERE userfilter LIKE '";
	sql += datafilter;
	sql += "%'";

	if(!stime.empty())
	{
		sql += " AND uptime >= '";
		sql += stime;
		sql += "'";
	}
	if(!eimte.empty())
	{
		sql += " AND uptime <= '";
		sql += eimte;
		sql += "'";
	}
	if(!hander.empty())//处理人真实姓名
	{
		sql += " AND handler = '";
		sql += hander;
		sql += "'";
	}
	if(!handernum.empty())//处理人用户名
	{
		sql += " AND handlernum = '";
		sql += handernum;
		sql += "'";
	}
	sql += ";";
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
	sql = "SELECT `platenum`,`cartype`,`owner`,`must`,`actual`,`drivername`,"
			"`phone`,`drivernum`,`viotype`,`handleway`,`handler`,`uptime`,`memo`,"
			"`picurl`,`handlernum` FROM xxdb_data_ledger  where userfilter LIKE '";
	sql += datafilter;
	sql += "%'";

	if(!stime.empty())
	{
		sql += " AND uptime >= '";
		sql += stime;
		sql += "'";
	}
	if(!eimte.empty())
	{
		sql += " AND uptime <= '";
		sql += eimte;
		sql += "'";
	}
	if(!hander.empty())//处理人真实姓名
	{
		sql += " AND handler = '";
		sql += hander;
		sql += "'";
	}
	if(!handernum.empty())//处理人用户名
	{
		sql += " AND handlernum = '";
		sql += handernum;
		sql += "'";
	}
	sql += " ORDER BY uptime DESC ";

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
			resxml += new_strprop("platenum", "value", row[0]);
			resxml += new_strprop("cartype", "value", row[1]);
			resxml += new_strprop("owner", "value", row[2]);
			resxml += new_strprop("must", "value", row[3]);
			resxml += new_strprop("actual", "value", row[4]);
			resxml += new_strprop("drivername", "value", row[5]);
			resxml += new_strprop("phone", "value", row[6]);
			resxml += new_strprop("drivernum", "value", row[7]);
			resxml += new_strprop("viotype", "value", row[8]);
			resxml += new_strprop("handleway", "value", row[9]);
			resxml += new_strprop("hander", "value", row[10]);//处理人真实姓名
			resxml += new_strprop("time", "value", row[11]);
			resxml += new_strprop("memo", "value", row[12]);
			resxml += new_strprop("picurl", "value", row[13]);
			resxml += new_strprop("handernum", "value", row[14]);//处理人姓名
			resxml += "</item";
			resxml += itoaa(count);
			resxml += ">";
		}
		mysql_free_result(result);
	}
	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();

	printf("OUTXML : %s\n", *outxmldata);
	return 0;
}
//保存图片文件
int savepicfile(const char * filename, const char * filedata)
{
	char pathname[256]={0};
	sprintf(pathname,"%s/%s", picpath.c_str(), filename);
	memset(jpegpic, 0, 1024*1024*5);
	int datalen = b64_decode_string((const unsigned char *)filedata, strlen(filedata), (unsigned char *)jpegpic);
	FILE * fp = fopen(pathname, "wb");
	if(fp)
	{
		fwrite(jpegpic, 1, datalen, fp);
		fclose(fp);
		return 0;
	}
	return 1;
}
//创建系统的存储图片的路径路径为 ”picpath/ledger/年/月/日“
int getLedgerfilename(const char * dev, const char * datetime, int index, string &subfileurl)
{
	SYSTEMTIME sys;
	char fileurl[256] = {0};

	GetLocalTime(&sys);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "mkdir %s\\ledger\\%d\\%d\\%d", picpath.c_str(), sys.wYear, sys.wMonth, sys.wDay);
	system(fileurl);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "ledger/%d/%d/%d/%s_%s_%d.jpg", sys.wYear, sys.wMonth, sys.wDay, dev, datetime, index);
	printf("getfilegetLedgerfilenamename filename = %s\n", fileurl);
	for (char *p = fileurl; *p != '\0'; *p++)
	{
		if(*p == ':'||*p == '-'||*p == ' ')
			*p = '_';
	}

	subfileurl = fileurl;
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
    //手机上传台账数据
	if (str.compare("0001") == 0)
	{
		return InsertLedgerDataIntoSQL(xx_xml, outxmldata);
	}
	//web查询数据，采用分页xml查询方式
	else if (str.compare("0002") == 0)
	{
		return WebCheckLedgerWithPageSQL(xx_xml, outxmldata);
	}
	//web查询数据，整体数据的下载
	else if (str.compare("0003") == 0)
	{
		return WebCheckLedgerWithAllSQL(xx_xml, outxmldata);
	}
	return 0;
}
