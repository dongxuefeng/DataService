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
#include "xxplug_recive_UpdateVioData.h"
#include "makezip.h"
#include <day.h>

using namespace std;

MYSQL * m_sql = 0;
string gresXML;
string username = "";
string picpath = "";
string downloadPath = "";
pthread_t packData;
void * mkzip = NULL;
#define bzero(x,y) memset(x, 0, y)

void * DataDelThread(void * arg)
{
	string str = "";
	string sql = "";
	string id = "";
	int outdate = 0;
	SYSTEMTIME t;
	char tmp[60] = {0};
	vector<string> idList;
	MYSQL * mth_sql = 0;

	connectMySQL(mth_sql);
	while(1)
	{
		sql = "SELECT outdate FROM xxdb_info_base";
		if(xx_sql_query(mth_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(mth_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				outdate = atoi(row[0]);
			}
			mysql_free_result(result);
		}

		GetLocalTime(&t);
		str = itoaa(t.wYear);
		str += "-";
		str += itoaa(t.wMonth);
		str += "-";
		str += itoaa(t.wDay);

		bzero(tmp,60);
		printf("GetDate date = %s\n", str.c_str());
		GetDate((char *)str.c_str(), tmp, outdate*(-1));

		sql = "SELECT id FROM xxdb_data_blacklist WHERE DATETIME < '";
		sql += tmp;
		sql += "' AND blacklisttype != 2 AND altertype = 1";
		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(mth_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(mth_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				id = row[0];
				idList.push_back(id);
			}
			mysql_free_result(result);
		}

		if(idList.size() != 0)
		{
			sql = "UPDATE xxdb_data_blacklist SET altertype = 2, TIMESTAMP = 9 WHERE 1=1 ";
			for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
			{
				sql += "OR id = ";
				sql += *iter;
			}
			printf("SQL : %s\n", sql.c_str());
			xx_sql_query(mth_sql, sql.c_str());
		}

		idList.clear();
		Sleep(100000);
	}
	return (void *)0;
}

int xx_rcv_gettype()
{
	printf("--xxplug_recive_UpdateVioData--\n");
	connectMySQL(m_sql);

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

	mkzip = init();

	pthread_t delTh;
	pthread_create(&delTh, NULL, DataDelThread, NULL);
	pthread_detach(delTh);

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

	//违法
	//手机提交违法数据
	if (str.compare("0001") == 0) {
		return insertViodata(xx_xml, outxmldata);
	}

	/*//手机提交违法的处理
	if (str.compare("0014") == 0) {
		return insertSpotDispose(xx_xml, outxmldata);
	}

	if (str.compare("0015") == 0) {
		return insertDisposePic(xx_xml, outxmldata);
	}*///老版本的违法处理  已经停止使用

	//web本地违法查询
	if (str.compare("0010") == 0) {
		return selectLocalViodate(xx_xml, outxmldata);
	}

	//web本地违法图片下载
	if (str.compare("0011") == 0) {
		return selectLocalViopic(xx_xml, outxmldata);
	}

	//web未确认违法数据查询
	if (str.compare("0020") == 0) {
		return selectUnaffirmData(xx_xml, outxmldata);
	}

	//web违法数据确认
	if (str.compare("0002") == 0) {
		return affirmVioData(xx_xml, outxmldata);
	}

	/*//web已处理违法数据查询
	if (str.compare("0018") == 0) {
		return selectAffirmData(xx_xml, outxmldata);
	}

	//web已处理违法数据图片下载
	if (str.compare("0019") == 0) {
		return selectAffirmPic(xx_xml, outxmldata);
	}*///老版本的违法处理  已经停止使用

	//web已确认违法数据打包
	if (str.compare("0012") == 0) {
		return packAffirmData(xx_xml, outxmldata);
	}

	//web违法数据包列表查询
	if (str.compare("0013") == 0) {
		return selectPackList(xx_xml, outxmldata);
	}

	//web违法数据包删除
	if (str.compare("0016") == 0) {
		return deletePack(xx_xml, outxmldata);
	}
//
//	//web违法数据包下载
//	if (str.compare("0017") == 0) {
//		return downloadPack(xx_xml, outxmldata);
//	}

	//黑名单
	//手机同步
	if (str.compare("0003") == 0) {
		return BlackIsUpdate(xx_xml, outxmldata);
	}

	if (str.compare("0004") == 0) {
		return GetBlackCount(xx_xml, outxmldata);
	}

	if (str.compare("0005") == 0) {
		return GetBlack(xx_xml, outxmldata);
	}

	//web页面增加
	if (str.compare("0006") == 0) {
		return InsertBlack(xx_xml, outxmldata);
	}

	if (str.compare("0007") == 0) {
		return DeleteBlack(xx_xml, outxmldata);
	}

	if (str.compare("0008") == 0) {
		return UpdateBlack(xx_xml, outxmldata);
	}

	if (str.compare("0009") == 0) {
		return SelectBlack(xx_xml, outxmldata);
	}

	//设施卡口数据过期时间
	if (str.compare("0021") == 0) {
		return setOutDate(xx_xml, outxmldata);
	}

	//获取卡口数据过期天数和未下卡口数据数量
	if (str.compare("0022") == 0) {
		return getOutDateAndCount(xx_xml, outxmldata);
	}

	return 0;
}
